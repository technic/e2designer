#include <QtTest>
#include <QString>

#include "parser/parser_impl.hpp"
#include "parser/parser.hpp"
#include "base/misc.hpp"

#include <iomanip>
#include <tao/pegtl/contrib/parse_tree.hpp>
#include <tao/pegtl/analyze.hpp>

using namespace tao;

class TestParser : public QObject
{
    Q_OBJECT
public:
    TestParser() = default;

private:
    long toNumber(parser::Expression* ptr)
    {
        auto n = dynamic_cast<parser::Number*>(ptr);
        Q_ASSERT(n);
        return n->value;
    };

    std::string& toIdentifier(parser::Expression* ptr)
    {
        auto x = dynamic_cast<parser::Identifier*>(ptr);
        Q_ASSERT(x);
        return x->name;
    }

    parser::KvArg* toKvArg(parser::Expression* ptr)
    {
        auto kv = dynamic_cast<parser::KvArg*>(ptr);
        Q_ASSERT(kv);
        return kv;
    }

    parser::KvPair& toKvPair(parser::Expression& ptr) { return dynamic_cast<parser::KvPair&>(ptr); }

    parser::TupleExpr* toTuple(parser::Expression* ptr)
    {
        auto tup = dynamic_cast<parser::TupleExpr*>(ptr);
        Q_ASSERT(tup);
        return tup;
    }

    void walkNode(parser::AstNode& node, int lvl = 1)
    {
        for (auto& c : node.children) {
            qDebug().noquote() << QString(">").repeated(lvl) << c->name().c_str() << ":"
                               << c->string().c_str();
            walkNode(*c, lvl + 1);
        }
    };

private slots:
    void test_grammar()
    {

        size_t problems = pegtl::analyze<parser::grammar>();
        QVERIFY(problems == 0);
    }

    void test_number()
    {
        pegtl::memory_input in{ "1234", "" };
        auto root = parser::parseAst(in);
        QVERIFY(root);
        // walkNode(*root);

        QVERIFY(!root->children.empty());
        auto n = dynamic_cast<parser::Number*>(root->children.front()->expression.get());
        QVERIFY(n);
        QCOMPARE(n->value, 1234);
    }

    void test_tuple()
    {
        pegtl::memory_input in{ "(10, 20)", "" };

        auto root = parser::parseAst(in);
        QVERIFY(root);
        // walkNode(*root);

        QVERIFY(!root->children.empty());
        auto t = dynamic_cast<parser::TupleExpr*>(root->children.front()->expression.get());
        QVERIFY(t);
        QCOMPARE(t->values.size(), 2);
        QCOMPARE(toNumber(t->values[0].get()), 10);
        QCOMPARE(toNumber(t->values[1].get()), 20);
    }

    void test_list()
    {
        pegtl::memory_input in{ "[10, 20]", "" };

        auto root = parser::parseAst(in);
        QVERIFY(root);
        // walkNode(*root);

        QVERIFY(!root->children.empty());
        auto t = dynamic_cast<parser::ListExpr*>(root->children.front()->expression.get());
        QVERIFY(t);
        QCOMPARE(t->values.size(), 2);
        QCOMPARE(toNumber(t->values[0].get()), 10);
        QCOMPARE(toNumber(t->values[1].get()), 20);
    }

    void test_dict()
    {
        pegtl::memory_input in{ "{10: 100, 20: 200}", "" };
        auto root = parser::parseAst(in);
        QVERIFY(root);
        auto& d = dynamic_cast<parser::DictExpr&>(*root->children.front()->expression);
        QCOMPARE(toNumber(d.pairs[0]->key.get()), 10);
        QCOMPARE(toNumber(d.pairs[0]->value.get()), 100);
        QCOMPARE(toNumber(d.pairs[1]->key.get()), 20);
        QCOMPARE(toNumber(d.pairs[1]->value.get()), 200);
    }

    void test_operator()
    {
        pegtl::memory_input in{ "1 | 4", "" };
        auto root = parser::parseAst(in);
        QVERIFY(root);
        walkNode(*root);

        auto& seq = dynamic_cast<parser::OpSequence&>(*root->children.front()->expression);
        QCOMPARE(seq.evaluateNumber(parser::Context{}), 5);
    }

    void test_func_call()
    {
        {
            pegtl::memory_input in{ "f_positional_args(a, b, c)", "" };
            auto root = parser::parseAst(in);
            QVERIFY(root);
            walkNode(*root);

            auto f = dynamic_cast<parser::FunctionCall*>(root->children.front()->expression.get());
            QVERIFY(f);
            QCOMPARE(f->args.size(), 3);
            QCOMPARE(toIdentifier(f->args[0].get()), "a");
            QCOMPARE(toIdentifier(f->args[1].get()), "b");
            QCOMPARE(toIdentifier(f->args[2].get()), "c");
        }
        {
            pegtl::memory_input in{ "f_named_args(x=3, y=4, z=(foo, bar))", "" };
            auto root = parser::parseAst(in);
            QVERIFY(root);
            walkNode(*root);

            auto f = dynamic_cast<parser::FunctionCall*>(root->children.front()->expression.get());
            QVERIFY(f);
            QCOMPARE(f->kvargs.size(), 3);

            QCOMPARE(f->kvargs[0].key, "x");
            QCOMPARE(toNumber(f->kvargs[0].value.get()), 3);

            QCOMPARE(f->kvargs[1].key, "y");
            QCOMPARE(toNumber(f->kvargs[1].value.get()), 4);

            QCOMPARE(f->kvargs[2].key, "z");
            QCOMPARE(toIdentifier(toTuple(f->kvargs[2].value.get())->values[0].get()), "foo");
            QCOMPARE(toIdentifier(toTuple(f->kvargs[2].value.get())->values[1].get()), "bar");
        }
        {
            pegtl::memory_input in{ "f_combined_args(100, 200, x=3, y=4, z=(foo, bar))", "" };
            auto root = parser::parseAst(in);
            QVERIFY(root);
            walkNode(*root);
        }
    }

    void test_template()
    {
        using namespace multicontent;

        QString s = "[MultiContentEntryText(pos=(0,0), size=(100,10))]";
        PseudoPythonParser parser;

        auto elements = parser.parse(s);
        QCOMPARE(elements.size(), 1);
        EntryText txt;
        txt.pos = Coordinate{ 0, 0 };
        txt.size = Coordinate{ 100, 10 };
        QCOMPARE(dynamic_cast<multicontent::EntryText&>(*elements[0]), txt);
    }

    void test_full_01()
    {
        using namespace multicontent;

        QString s =
          R"#({"template":[MultiContentEntryText(pos=(0,0),size=(100,10))],"fonts":[gFont("Regular",10)],"itemHeight":20})#";
        auto tpl = PseudoPythonParser().parseFull(s);

        QCOMPARE(tpl.itemHeight, 20);
        QCOMPARE(tpl.fonts[0], FontAttr("Regular", 10));
        EntryText txt;
        txt.pos = Coordinate{ 0, 0 };
        txt.size = Coordinate{ 100, 10 };
        QCOMPARE(dynamic_cast<multicontent::EntryText&>(*(*tpl.entries)[0]), txt);
    }

    void test_full_02()
    {
        using namespace multicontent;

        QString s = R"#({
            "template": [
                MultiContentEntryPixmapAlphaTest(pos = (0, 5), size = (16, 16), png = 1), # red dot
                MultiContentEntryText(pos = (18, 2),  size = (40, 22),  text = 2), # day
                MultiContentEntryText(pos = (62, 2),  size = (90, 22),  text = 3), # time
                MultiContentEntryText(pos = (140, 2), size = (585, 24), text = 4), # title
            ],
            "fonts": [gFont("Regular", 20)],
            "itemHeight": 28
        })#";

        auto tpl = PseudoPythonParser().parseFull(s);
        QCOMPARE(tpl.itemHeight, 28);
        QCOMPARE(tpl.fonts[0], FontAttr("Regular", 20));

        EntryPixmap px(Property::Alphatest::on);
        px.pos = Coordinate{ 0, 5 };
        px.size = Coordinate{ 16, 16 };
        px.valueIdx = 1;
        QCOMPARE(dynamic_cast<EntryPixmap&>(*(*tpl.entries)[0]), px);

        EntryText txt;

        txt.pos = Coordinate{ 18, 2 };
        txt.size = Coordinate{ 40, 22 };
        txt.valueIdx = 2;
        QCOMPARE(dynamic_cast<EntryText&>(*(*tpl.entries)[1]), txt);

        txt.pos = Coordinate{ 62, 2 };
        txt.size = Coordinate{ 90, 22 };
        txt.valueIdx = 3;
        QCOMPARE(dynamic_cast<EntryText&>(*(*tpl.entries)[2]), txt);

        txt.pos = Coordinate{ 140, 2 };
        txt.size = Coordinate{ 585, 24 };
        txt.valueIdx = 4;
        QCOMPARE(dynamic_cast<EntryText&>(*(*tpl.entries)[3]), txt);
    }

    void test_full_03()
    {
        using namespace multicontent;

        QString s = R"#({
            "template": [
                MultiContentEntryPixmapAlphaTest(pos = (1, 5), size = (100, 40), png = 1), # logo
                MultiContentEntryText(pos = (110, 2),  size = (400, 46), flags = RT_HALIGN_LEFT | RT_VALIGN_CENTER, text = 2), # title,
            ],
            "fonts": [gFont("Regular", 33)],
            "itemHeight": 50
        })#";

        auto tpl = PseudoPythonParser().parseFull(s);
        QCOMPARE(tpl.itemHeight, 50);
        QCOMPARE(tpl.fonts[0], FontAttr("Regular", 33));

        EntryPixmap px(Property::Alphatest::on);
        px.pos = Coordinate{ 1, 5 };
        px.size = Coordinate{ 100, 40 };
        px.valueIdx = 1;
        QCOMPARE(dynamic_cast<EntryPixmap&>(*(*tpl.entries)[0]), px);

        EntryText txt;
        txt.pos = Coordinate{ 110, 2 };
        txt.size = Coordinate{ 400, 46 };
        txt.valueIdx = 2;
        txt.flags = Qt::AlignLeft | Qt::AlignVCenter;
        QCOMPARE(dynamic_cast<EntryText&>(*(*tpl.entries)[1]), txt);
    }
};

QTEST_APPLESS_MAIN(TestParser)

#include "tst_parser.moc"
