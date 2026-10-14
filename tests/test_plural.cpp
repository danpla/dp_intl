#include "flow.h"

#include <limits>
#include <optional>

#include "dp_intl/error.h"

#include "plural/eval.h"

#include "utils.h"


using namespace dp::intl;


namespace {


const auto countTMax = std::numeric_limits<CountT>::max();
const auto countTMaxStr = test::toStr(countTMax);


void testEval(CountT n, CountT result, std::string_view expr)
{
    std::optional<plural::Eval> eval;

    try {
        eval.emplace(expr);
    } catch (const Error& e) {
        test::failure(
            "Unexpected error when compiling "
            + test::toStr(expr) + ": " + e.what());
        return;
    }

    CountT got;
    try {
        got = (*eval)(n);
    } catch (const Error& e) {
        test::failure(
            "Unexpected error when evaluating "
            + test::toStr(expr) + " with n="
            + test::toStr(n) + ": " + e.what());
        return;
    }

    if (got != result)
        test::failure(
            test::toStr(expr) + " with n=" + test::toStr(n)
            + ": expected " + test::toStr(result)
            + ", got " + test::toStr(got));
}


void testEval()
{
    const struct Test {
        CountT n;
        CountT result;
        std::string_view expr;
    } tests[]{
        {0, 123, "123"},
        {0, 123, "(((123)))"},
        {0, countTMax, countTMaxStr},

        {123, 123, "n"},
        {countTMax, countTMax, "n"},
        {123, 10, "n == 123 ? 10 : 20"},
        {123, 20, "n == 321 ? 10 : 20"},
        {1, 11, "n == 1 ? n + 10 : n * 10"},
        {2, 20, "n == 1 ? n + 10 : n * 10"},

        {0, 2, "2 + 2 - 2"},
        {0, 4, "2 + 2 - (2 - 2)"},

        {0, 6, "2 + 2 * 2"},
        {0, 4, "2 + 2 * 2 - 2"},
        {0, 2, "2 + 2 * (2 - 2)"},
        {0, 8, "(2 + 2) * 2"},
        {0, 6, "(2 + 2) * 2 - 2"},
        {0, 0, "(2 + 2) * (2 - 2)"},

        {0, 4, "2 + 4 / 2"},
        {0, 3, "(2 + 4) / 2"},
        {0, 2, "2 + 4 % 2"},
        {0, 0, "(2 + 4) % 2"},

        {5, 2, "n / 2"},
        {5, 1, "n % 2"},

        // Short-circuit evaluation guards against division by zero.
        {0, 0, "1 ? 0 : 1 / 0"},
        {0, 0, "1 ? 0 : 1 % 0"},

        {0, 1, "!0"},
        {0, 0, "!!0"},

        {1, 1, "n == 1"},
        {1, 0, "n != 1"},
        {1, 0, "n < 1"},
        {1, 1, "n <= 1"},
        {1, 0, "n > 1"},
        {1, 1, "n >= 1"},

        {5, 1, "n > 0 && n < 10"},
        {10, 0, "n > 0 && n < 10"},

        {0, 10, "1 ? 10 : 0 ? 20 : 30"},
        {0, 20, "(1 ? 10 : 0) ? 20 : 30"},

        {0, 12, "2 + (1 ? 10 : 20)"},

        {0, 0, "1 && 0 || 0"},
        {0, 1, "1 || 0 && 0"},
    };

    for (const auto& test : tests)
        testEval(test.n, test.result, test.expr);
}


void testEvalError(CountT n, std::string_view expr)
{
    std::optional<plural::Eval> eval;

    try {
        eval.emplace(expr);
    } catch (const Error& e) {
        test::failure(
            "Unexpected error when compiling "
            + test::toStr(expr) + ": " + e.what());
        return;
    }

    try {
        [[maybe_unused]]
        const auto result = (*eval)(n);
    } catch (const Error& e) {
        return;
    }

    test::failure(
        "No error was thrown when evaluating " + test::toStr(expr)
        + " with n=" + test::toStr(n));
}


void testEvalErrors()
{
    testEval(0, countTMax, countTMaxStr + " + 0");
    testEval(0, countTMax, test::toStr(countTMax - 1) + " + 1");
    testEvalError(0, countTMaxStr + " + 1");

    testEval(0, 0 , "0 - 0");
    testEvalError(0, "0 - 1");

    const auto halfBits = sizeof(CountT) * 8 / 2;

    testEval(0, countTMax, countTMaxStr + " * 1");
    testEval(
        0, countTMax,
        test::toStr((CountT{1} << halfBits) - 1)
            + " * "
            + test::toStr((CountT{1} << halfBits) + 1));
    testEvalError(
        0,
        test::toStr(CountT{1} << halfBits)
            + " * "
            + test::toStr(CountT{1} << halfBits));

    testEvalError(0, "1 / 0");
    testEvalError(0, "1 % 0");
}


void testCompileErrors()
{
    const std::string tests[]{
        "",
        "*",
        "-1",
        "+1",
        "()",
        "(1",
        "1)",
        "(1 + 2",
        "1 + 2)",
        "1 ? 2",
        "1 ? 2 :",
        "1 : 2",
        "1 @ 2",
        "1   2",
        "1 +",
        "1 ++ 2",
        "n n",
        "N",
        countTMaxStr + "0",
    };

    for (const auto& expr : tests) {
        try {
            const plural::Eval eval{expr};
        } catch (const Error&) {
            continue;
        }

        test::failure(
            "No error was thrown for invalid expression "
            + test::toStr(expr));
    }
}


void testPlural()
{
    testEval();
    testEvalErrors();
    testCompileErrors();
}


}


TEST_REGISTER(testPlural);
