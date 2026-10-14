#include "flow.h"

#include <cstddef>

#include "dp_intl/lang_match.h"

#include "utils.h"


using namespace dp::intl;


namespace {


using LangCodes = std::vector<std::string>;


std::string joinTag(const std::vector<std::string_view>& subtags)
{
    std::string result;

    for (std::size_t i{}; i < subtags.size(); ++i) {
        if (i > 0)
            result += "-";

        result += subtags[i];
    }

    return result;
}


void testLangMatch()
{
    const struct {
        LangCodes codes;
        LangCodes matches;
    } tests[]{
        {
            {},
            {}},
        {
            {""},
            {}},
        {
            {"", " "},
            {}},
        {
            {"en"},
            {"en"}},
        {
            {"en-US", },
            {"en-US", "en"}},
        {
            {"en-Latn-US", },
            {"en-Latn-US", "en-US", "en-Latn", "en"}},
        {
            {"En", "eN-US", "EN-us", "en"},
            {"En", "eN-US"}},
        {
            {"en-Latn-US", "en-US-Latn"},
            {
                "en-Latn-US", "en-US", "en-Latn", "en",
                "en-US-Latn"}},
        {
            {"az-Arab-IR", "az_Ru-Cyrl"},
            {
                "az-Arab-IR", "az-IR", "az-Arab", "az",
                "az-Ru-Cyrl", "az-Cyrl", "az-Ru"}},
        {
            // Alternatives for zh-Hans.
            {"zh-Hans"},
            {"zh-Hans", "zh-CN", "zh-SG", "zh"}},
        {
            // zh-Hans as an alternative for zh-CN.
            {"zh-CN"},
            {"zh-CN", "zh-Hans", "zh"}},
        {
            // Alternatives for zh-Hant.
            {"zh-Hant"},
            {"zh-Hant", "zh-TW", "zh-HK", "zh"}},
        {
            // zh-Hant as an alternative to zh-TW.
            {"zh-TW"},
            {"zh-TW", "zh-Hant", "zh"}},
        {
            // zh-Hant as an alternative to zh-HK.
            {"zh-HK"},
            {"zh-HK", "zh-Hant", "zh"}},
        {
            // Mixed separators.
            {"en_Latn-US", },
            {"en-Latn-US", "en-US", "en-Latn", "en"}},
        {
            // Empty subtags: leading, in the middel, trailing,
            // trailing before an invalid character (?).
            {"-aa", "bb--BB", "cc-", "dd-?"},
            {"bb", "cc", "dd"}},
        {
            // BCP 47 one-character subtags:
            // * "x" starts private use subtags.
            // * Any other character except "x" (singletone) starts
            //   extension subtags.
            {"en-r-extension", "de-x-private", "x-private"},
            {"en", "de"}},
        {
            // Unix-style locale names with ".encoding" and "@variant"
            // extensions.
            {"en_US.UTF-8", "de_DE@euro"},
            {"en-US", "en", "de-DE", "de"}},
    };

    for (const auto& test : tests) {
        LangCodes actualMatches;
        matchLang(
            test.codes,
            [&](const std::vector<std::string_view>& subtags)
            {
                actualMatches.push_back(joinTag(subtags));
                return false;
            });

        if (actualMatches == test.matches)
            continue;

        test::failure(
            "matchLang() with " + test::toStr(test.codes) + "\n"
            "  Expected " + test::toStr(test.matches) + "\n"
            "  Got      " + test::toStr(actualMatches));
    }
}


}


TEST_REGISTER(testLangMatch);
