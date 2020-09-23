#include "parser.hpp"
#include "parser_impl.hpp"
#include "base/singleton.hpp"
#include "base/misc.hpp"
#include "skin/enums.hpp"
#include <unordered_map>
#include <variant>

using namespace tao;

int evaluateNumber(const parser::Expression& expr)
{
    return expr.evaluateNumber(PseudoPythonParser::ctx);
}

std::string evaluateString(const parser::Expression& expr)
{
    return expr.cast<parser::String>().value;
}

namespace multicontent {

class AbstractBuilder
{
public:
    std::unique_ptr<Entry> build(const parser::FunctionCall& f)
    {

        size_t i = 0;
        for (auto& arg : f.args) {
            if (i < params.size()) {
                params[i].apply(*arg);
            } else {
                throw std::runtime_error{ "too many arguments" };
            }
            i++;
        }

        for (auto& kvarg : f.kvargs) {
            size_t j = i;
            for (; j < params.size(); ++j) {
                if (kvarg.key == params[j].name) {
                    params[j].apply(*kvarg.value);
                    break;
                }
            }
            if (j == params.size()) {
                throw std::runtime_error{ "unknow kvarg " + kvarg.key };
            }
        }

        return constructEntry();
    }

protected:
    struct Param
    {
        const char* name;
        const std::function<void(parser::Expression&)> apply;
    };

    std::vector<Param> params;

    template<typename Func>
    void addParam(const char* name, Func&& f)
    {
        params.push_back({ name, f });
    }

    virtual std::unique_ptr<Entry> constructEntry() = 0;
};

class EntryTextBuilder : public AbstractBuilder
{
public:
    static constexpr char functionName[] = "MultiContentEntryText";

    EntryTextBuilder()
    {
        addParam("pos", [this](const parser::Expression& expr) {
            auto& tuple = expr.cast<parser::TupleExpr>();
            obj.pos = Coordinate{ evaluateNumber(*tuple.values.at(0)),
                                  evaluateNumber(*tuple.values.at(1)) };
        });
        addParam("size", [this](const parser::Expression& expr) {
            auto& tuple = expr.cast<parser::TupleExpr>();
            obj.size = Coordinate{ evaluateNumber(*tuple.values.at(0)),
                                   evaluateNumber(*tuple.values.at(1)) };
        });
        addParam("font", [this](const parser::Expression& expr) {
            obj.fontIdx = expr.evaluateNumber(PseudoPythonParser::ctx);
        });
        addParam("flags", [this](const parser::Expression& expr) {
            obj.flags = expr.evaluateNumber(PseudoPythonParser::ctx);
        });
        addParam("text", [this](const parser::Expression& expr) {
            obj.valueIdx = expr.evaluateNumber(PseudoPythonParser::ctx);
        });
    }

protected:
    std::unique_ptr<Entry> constructEntry() final { return std::make_unique<EntryText>(obj); }

private:
    EntryText obj;
};

class EntryPixmapBuilder : public AbstractBuilder
{
public:
    static constexpr char functionName[] = "MultiContentEntryPixmap";
    using EntryType = EntryPixmap;

    explicit EntryPixmapBuilder(Property::Alphatest alpha = Property::Alphatest::off)
    {
        obj.alpha = alpha;
        addParam("pos", [this](const parser::Expression& expr) {
            auto& tuple = expr.cast<parser::TupleExpr>();
            obj.pos = Coordinate{ evaluateNumber(*tuple.values.at(0)),
                                  evaluateNumber(*tuple.values.at(1)) };
        });
        addParam("size", [this](const parser::Expression& expr) {
            auto& tuple = expr.cast<parser::TupleExpr>();
            obj.size = Coordinate{ evaluateNumber(*tuple.values.at(0)),
                                   evaluateNumber(*tuple.values.at(1)) };
        });
        addParam("png",
                 [this](const parser::Expression& expr) { obj.valueIdx = evaluateNumber(expr); });
        addParam("flags",
                 [this](const parser::Expression& expr) { obj.flags = evaluateNumber(expr); });
    }

protected:
    std::unique_ptr<Entry> constructEntry() final { return std::make_unique<EntryPixmap>(obj); }

private:
    EntryPixmap obj;
};

class EntryPixmapAlphaTestBuilder : public EntryPixmapBuilder
{
public:
    static constexpr char functionName[] = "MultiContentEntryPixmapAlphaTest";
    EntryPixmapAlphaTestBuilder()
        : EntryPixmapBuilder(Property::Alphatest::on)
    {}
};

class EntryPixmapAlphaBlendBuilder : public EntryPixmapBuilder
{
public:
    static constexpr char functionName[] = "MultiContentEntryPixmapAlphaBlend";
    EntryPixmapAlphaBlendBuilder()
        : EntryPixmapBuilder(Property::Alphatest::blend)
    {}
};

class EntryProgressBuilder : public AbstractBuilder
{
public:
    static constexpr char functionName[] = "MultiContentEntryProgressPixmap";
    EntryProgressBuilder() {}

protected:
    std::unique_ptr<Entry> constructEntry() final { return std::make_unique<EntryProgress>(obj); }

private:
    EntryProgress obj;
};

class EntryProgressPixmapBuilder : public AbstractBuilder
{
public:
    static constexpr char functionName[] = "MultiContentEntryProgressPixmap";
    EntryProgressPixmapBuilder() {}

protected:
    std::unique_ptr<Entry> constructEntry() final { return std::make_unique<EntryProgress>(obj); }

private:
    EntryProgress obj;
};

class Factory : public SingletonMixin<Factory>
{
public:
    Factory()
    {
        registerBuilder<EntryTextBuilder>();
        registerBuilder<EntryPixmapBuilder>();
        registerBuilder<EntryPixmapAlphaTestBuilder>();
        registerBuilder<EntryPixmapAlphaBlendBuilder>();
    }

    template<typename B>
    void registerBuilder()
    {
        builders[B::functionName] = std::make_unique<B>();
    }

    std::unique_ptr<multicontent::Entry> parseEntry(const parser::Expression& expr)
    {
        auto& f = expr.cast<const parser::FunctionCall>();

        auto it = builders.find(f.name);
        if (it == builders.end()) {
            throw std::runtime_error{ "unknown entry " + f.name };
        }
        return it->second->build(f);
    };

private:
    std::unordered_map<std::string_view, std::unique_ptr<AbstractBuilder>> builders;
};

} // multicontent

PseudoPythonParser::PseudoPythonParser() {}

std::vector<std::unique_ptr<multicontent::Entry>> PseudoPythonParser::parse(const QString& s)
{
    using namespace multicontent;
    auto& factory = Factory::instance();

    // FIXME: conversion is suboptimal solution
    const auto str = s.toStdString();
    pegtl::memory_input in{ str, "" };
    auto root = parser::parseAst(in);

    Q_ASSERT(root->children.front()->expression->is<parser::ListExpr>());
    auto list = uptr_static_cast<parser::ListExpr>(std::move(root->children.front()->expression));

    std::vector<std::unique_ptr<Entry>> result;
    for (const auto& c : list->values) {
        result.push_back(factory.parseEntry(*c));
    }
    return result;
}

std::vector<std::unique_ptr<multicontent::Entry>> parseTemplate(const parser::Expression& expr)
{
    using namespace multicontent;
    auto& factory = Factory::instance();

    auto& list = expr.cast<parser::ListExpr>();
    std::vector<std::unique_ptr<Entry>> result;
    for (const auto& c : list.values) {
        result.push_back(factory.parseEntry(*c));
    }
    return result;
}

FontAttr parseFont(const parser::Expression& expr)
{
    auto& f = expr.cast<parser::FunctionCall>();
    if (f.args.size() == 2) {
        return FontAttr(QString::fromStdString(evaluateString(*f.args[0])),
                        evaluateNumber(*f.args[1]));
    } else {
        throw std::runtime_error{ "Expecting exactly 2 arguments for font" };
    }
}

std::vector<FontAttr> parseFonts(const parser::Expression& expr)
{
    Q_ASSERT(expr.is<parser::ListExpr>());
    auto& list = expr.cast<parser::ListExpr>();

    std::vector<FontAttr> result;
    for (const auto& c : list.values) {
        result.push_back(parseFont(*c));
    }
    return result;
}

TemplatedContent PseudoPythonParser::parseFull(const QString& s)
{
    const auto str = s.toStdString();
    pegtl::memory_input in{ str, "" };

    auto expr = parser::parseExpression(in);
    if (!expr->is<parser::DictExpr>()) {
        throw std::runtime_error{ "TemplatedContent should be dict" };
    }
    auto dict = uptr_static_cast<parser::DictExpr>(std::move(expr));

    TemplatedContent tpl;

    for (auto& kv : dict->pairs) {
        std::string key = evaluateString(*kv->key);
        if (key == "template") {
            tpl.entries = std::make_shared<TemplatedContent::vecType>(parseTemplate(*kv->value));
        } else if (key == "itemHeight") {
            tpl.itemHeight = evaluateNumber(*kv->value);
        } else if (key == "fonts") {
            tpl.fonts = parseFonts(*kv->value);
        }
    }
    return tpl;
}

const parser::Context PseudoPythonParser::ctx{
    // horizontal align
    { "RT_HALIGN_LEFT", Qt::AlignLeft },
    { "RT_HALIGN_CENTER", Qt::AlignHCenter },
    { "RT_HALIGN_RIGHT", Qt::AlignRight },
    // vertical align
    { "RT_VALIGN_TOP", Qt::AlignTop },
    { "RT_VALIGN_CENTER", Qt::AlignVCenter },
    { "RT_VALIGN_BOTTOM", Qt::AlignBottom },
    // additional text flag
    { "RT_WRAP", Qt::TextWordWrap },
    // pixmap flag
    { "BT_SCALE", multicontent::EntryPixmap::scaleOn },
};
