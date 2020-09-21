#pragma once

#include <memory>
#include <string>
#include <vector>

#include <QDebug>

#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/parse_tree.hpp>
#include <tao/pegtl/contrib/tracer.hpp>
#include "base/misc.hpp"
#include "parser.hpp"

/// Defines ridiculously simple python-like grammar for MultiContent declarations in skin.xml
/// This is heavy header, you want to include it only in one translation unit
namespace parser {

using namespace tao::pegtl;

// Rules

struct comment : if_must<one<'#'>, until<eolf>>
{};

struct ws : sor<one<' ', '\t', '\n', '\r'>, comment>
{};

template<char C>
struct sep : pad<one<C>, blank>
{};

template<typename Rule>
struct wspad : pad<Rule, ws>
{};

struct number : seq<opt<one<'+', '-'>>, plus<digit>>
{};

struct string_content : until<at<one<'"'>>, must<ascii::alnum>>
{};
struct string : if_must<one<'"'>, string_content, one<'"'>>
{};

struct expr;

struct atom : sor<number, string, identifier>
{};

template<unsigned int N, typename Rule, typename Sep>
struct list_min : seq<Rule, rep_min<N - 1, Sep, must<Rule>>>
{
    static_assert(N > 1, "There must be at least one element in the list");
};
// template<typename E, typename O>
// struct op_concat : list_min<2, E, pad<O, blank>>
//{};

template<typename E, typename O>
struct op_concat : list_must<E, O, blank>
{};

struct list_elements : list_tail<expr, one<','>, ws>
{};
struct tuple_expr : if_must<sep<'('>, wspad<list_elements>, sep<')'>>
{};
struct list_expr : if_must<sep<'['>, wspad<list_elements>, sep<']'>>
{};

struct dict_kv : if_must<seq<expr, sep<':'>>, expr>
{};
struct dict_elements : list_tail<dict_kv, one<','>, ws>
{};
struct dict_expr : if_must<sep<'{'>, wspad<dict_elements>, sep<'}'>>
{};

struct kvarg : if_must<seq<identifier, sep<'='>>, expr>
{};
struct func_args : list_tail<sor<kvarg, expr>, one<','>, ws>
{};
struct func_call : seq<identifier, if_must<sep<'('>, wspad<opt<func_args>>, sep<')'>>>
{};

struct bracket : if_must<sep<'('>, wspad<expr>, sep<')'>>
{};

struct expr01 : sor<func_call, dict_expr, tuple_expr, list_expr, bracket, atom>
{};

struct op01 : sep<'|'>
{};
struct expr : op_concat<expr01, op01>
{};

struct grammar : must<pad<expr, ws>, eof>
{};

// AST

class Expression
{
public:
    template<typename U>
    bool is() const
    {
        static_assert(std::is_base_of_v<Expression, U>,
                      "It makes sense to check only for derived types");
        return std::is_same_v<Expression, U> || m_id == typeid(U);
    }

    template<typename U>
    U& cast()
    {
        if (is<U>()) {
            return static_cast<U&>(*this);
        }
        std::stringstream message;
        message << "Expected " << typeid(U).name() << " got " << m_id.name();
        throw std::runtime_error{ message.str() };
    }

    template<typename U>
    const U& cast() const
    {
        if (is<U>()) {
            return static_cast<const U&>(*this);
        }
        std::stringstream message;
        message << "Expected " << typeid(U).name() << " got " << m_id.name();
        throw std::runtime_error{ message.str() };
    }

    virtual long evaluateNumber(const Context& ctx) const
    {
        std::stringstream message;
        message << "Expression of type " << m_id.name() << " does not evaluate to number";
        throw std::runtime_error{ message.str() };
    }

    virtual ~Expression() = default;

protected:
    explicit Expression(const std::type_info& type)
        : m_id(std::type_index(type))
    {}

private:
    std::type_index m_id = std::type_index(typeid(void));
};

class Number : public Expression
{
public:
    Number(long value)
        : Expression(typeid(Number))
        , value(value)
    {}
    long value;
    bool operator==(const Number& other) const { return value == other.value; }

    long evaluateNumber(const Context& ctx) const override { return value; }
};

class Identifier : public Expression
{
public:
    Identifier(std::string name)
        : Expression(typeid(Identifier))
        , name(name)
    {}
    std::string name;
    bool operator==(const Identifier& other) const { return name == other.name; }

    long evaluateNumber(const Context& constants) const override
    {
        auto it = constants.find(name);
        if (it != constants.end()) {
            return it->second;
        } else {
            throw std::runtime_error{ "unknown constant:" + name };
        }
    }
};

class String : public Expression
{
public:
    String(std::string s)
        : Expression(typeid(String))
        , value(s)
    {}
    std::string value;
};

class TupleExpr : public Expression
{
public:
    TupleExpr()
        : Expression(typeid(TupleExpr))
    {}

    std::vector<std::unique_ptr<Expression>> values;
};

class ListExpr : public Expression
{
public:
    ListExpr()
        : Expression(typeid(ListExpr))
    {}
    std::vector<std::unique_ptr<Expression>> values;
};

class OpSequence : public Expression
{
public:
    OpSequence()
        : Expression(typeid(OpSequence))
    {}
    std::vector<std::unique_ptr<Expression>> values;

    long evaluateNumber(const Context& ctx) const override
    {
        long value = 0;
        for (auto& expr : values) {
            value |= expr->evaluateNumber(ctx);
        }
        return value;
    }
};

class KvArg : public Expression
{
public:
    KvArg(std::string key, std::unique_ptr<Expression>&& value)
        : Expression(typeid(KvArg))
        , key(key)
        , value(std::move(value))
    {}
    std::string key;
    std::unique_ptr<Expression> value;
};

class KvPair : public Expression
{
public:
    KvPair(std::unique_ptr<Expression>&& key, std::unique_ptr<Expression>&& value)
        : Expression(typeid(KvPair))
        , key(std::move(key))
        , value(std::move(value))
    {}
    std::unique_ptr<Expression> key;
    std::unique_ptr<Expression> value;
};

class DictExpr : public Expression
{
public:
    DictExpr()
        : Expression(typeid(DictExpr))
    {}
    std::vector<std::unique_ptr<KvPair>> pairs;
};

class FunctionCall : public Expression
{
public:
    FunctionCall(std::string name)
        : Expression(typeid(FunctionCall))
        , name(name)
    {}
    std::string name;
    std::vector<std::unique_ptr<Expression>> args;
    std::vector<KvArg> kvargs;
};

//
// Parser Actions
//

struct AstNode : public parse_tree::basic_node<AstNode>
{
    AstNode()
        : expression()
    {}
    std::unique_ptr<Expression> expression;
};

template<typename Rule>
struct AstSelector : std::false_type
{};

template<>
struct AstSelector<number> : std::true_type
{
    static void transform(std::unique_ptr<AstNode>& n)
    {
        qDebug() << n->name().c_str() << "=" << n->string().c_str();
        long value = std::stol(n->string());
        n->expression = std::make_unique<Number>(value);
    }
};

template<>
struct AstSelector<identifier> : std::true_type
{
    static void transform(std::unique_ptr<AstNode>& n)
    {
        qDebug() << n->name().c_str() << "=" << n->string().c_str();
        std::string value = n->string();
        n->expression = std::make_unique<Identifier>(value);
    }
};

template<>
struct AstSelector<string_content> : std::true_type
{
    static void transform(std::unique_ptr<AstNode>& n)
    {
        qDebug() << n->name().c_str() << "=" << n->string().c_str();
        n->expression = std::make_unique<String>(n->string());
    }
};

/// Common transform code for tuple and list
template<typename Collection>
void transformCollection(std::unique_ptr<AstNode>& n)
{
    qDebug() << n->name().c_str() << n->children.size() << "=" << n->string().c_str();

    auto expression = std::make_unique<Collection>();
    for (auto& c : n->children) {
        expression->values.push_back(std::move(c->expression));
    }
    n->expression = std::move(expression);
}

template<>
struct AstSelector<tuple_expr> : std::true_type
{
    static void transform(std::unique_ptr<AstNode>& n) { transformCollection<TupleExpr>(n); }
};

template<>
struct AstSelector<list_expr> : std::true_type
{
    static void transform(std::unique_ptr<AstNode>& n) { transformCollection<ListExpr>(n); }
};

template<>
struct AstSelector<kvarg> : std::true_type
{
    static void transform(std::unique_ptr<AstNode>& n)
    {
        qDebug() << n->name().c_str() << n->children.size() << "=" << n->string().c_str();

        if (n->children.size() != 2) {
            throw std::logic_error("invalid kvarg node");
        }
        if (!n->children.front()->is<identifier>()) {
            throw std::logic_error("invalid kvarg node");
        }

        auto name = static_cast<Identifier&>(*n->children[0]->expression).name;
        auto expression = std::make_unique<KvArg>(name, std::move(n->children[1]->expression));
        n->expression = std::move(expression);
    }
};

template<>
struct AstSelector<func_call> : std::true_type
{
    static void transform(std::unique_ptr<AstNode>& n)
    {
        qDebug() << n->name().c_str() << n->children.size() << "=" << n->string().c_str();

        if (n->children.empty()) {
            throw std::logic_error("invalid func_call node");
        }
        if (!n->children.front()->is<identifier>()) {
            throw std::logic_error("invalid func_call node");
        }

        auto name = static_cast<Identifier&>(*n->children.front()->expression).name;
        auto expression = std::make_unique<FunctionCall>(name);

        for (auto it = ++n->children.cbegin(); it != n->children.cend(); ++it) {
            auto& c = *it;
            if (c->is<kvarg>()) {
                KvArg& kv = static_cast<KvArg&>(*c->expression);
                expression->kvargs.push_back(KvArg(kv.key, std::move(kv.value)));
            } else {
                expression->args.push_back(std::move(c->expression));
            }
        }
        n->expression = std::move(expression);
    }
};

template<>
struct AstSelector<dict_kv> : std::true_type
{
    static void transform(std::unique_ptr<AstNode>& n)
    {
        qDebug() << n->name().c_str() << n->children.size() << "=" << n->string().c_str();

        if (n->children.size() != 2) {
            throw std::logic_error("invalid dict kv node");
        }

        n->expression = std::make_unique<KvPair>(std::move(n->children[0]->expression),
                                                 std::move(n->children[1]->expression));
    }
};

template<>
struct AstSelector<dict_expr> : std::true_type
{
    static void transform(std::unique_ptr<AstNode>& n)
    {
        qDebug() << n->name().c_str() << n->children.size() << "=" << n->string().c_str();

        auto expression = std::make_unique<DictExpr>();
        for (auto& c : n->children) {
            if (c->expression->is<KvPair>()) {
                expression->pairs.push_back(uptr_static_cast<KvPair>(std::move(c->expression)));
            } else {
                throw std::logic_error{ "invalid dict element" };
            }
        }
        n->expression = std::move(expression);
    }
};

template<>
struct AstSelector<expr> : std::true_type
{
    static void transform(std::unique_ptr<AstNode>& n)
    {
        qDebug() << n->name().c_str() << n->children.size() << "=" << n->string().c_str();

        if (n->children.size() == 1) {
            n->expression = std::move(n->children.front()->expression);
            n->children.clear();
        } else {
            auto expression = std::make_unique<OpSequence>();
            for (auto& c : n->children) {
                expression->values.push_back(std::move(c->expression));
            }
            n->expression = std::move(expression);
        }
    }
};

template<typename Input>
std::unique_ptr<AstNode> parseAst(Input&& in)
{
    try {
        return parse_tree::parse<grammar, AstNode, AstSelector, nothing, tracer>(in);
    } catch (const pegtl::parse_error& e) {
        // This catch block needs access to the input
        const auto p = e.positions.front();
        qDebug().noquote() << e.what() << '\n'
                           << in.line_at(p).c_str() << '\n'
                           << QString('^').leftJustified((int)p.byte_in_line);
        throw e;
    }
}

template<typename Input>
std::unique_ptr<Expression> parseExpression(Input&& in)
{
    auto root = parseAst(std::forward<Input>(in));
    Q_ASSERT(root->children.size() == 1);
    return std::move(root->children.front()->expression);
}

} // parser
