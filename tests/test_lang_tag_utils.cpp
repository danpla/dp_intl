#include "flow.h"

#include <cstddef>

#include "dp_intl/lang_tag_utils.h"

#include "utils.h"


using namespace dp::intl;


namespace {


void testBuildLangTag()
{
    const struct {
        std::vector<std::string_view> subtags;
        std::string_view tag;
        LangTagFormat format{
            LangTagFormat::Separator::Hyphen,
            LangTagFormat::LetterCase::Bcp47};
    } tests[]{
        {
            {},
            ""},
        {
            {"", "", " "},
            "-- "},
        {
            {"En", "Us"},
            "en-US"},
        {
            // Language-script-region
            {"En", "latn", "Us"},
            "en-Latn-US"},
        {
            // Language-region-script (invalid BCP 47 order)
            {"En", "Us", "latn"},
            "en-US-Latn"},
        {
            // Script-language-region (invalid BCP 47 order)
            {"Latn", "en", "Us"},
            "latn-EN-US"},
        {
            // Language-script-variant (according to BCP 47, the
            // variant subtag must be at least 5 characters long if
            // starts with a letter, or at least 4 character long if
            // starts with a digit).
            {"En", "latn", "variant"},
            "en-Latn-variant"},
        {
            // Language-variant-script (invalid BCP 47 order)
            {"En", "Variant", "latn"},
            "en-variant-Latn"},
        {
            // Language-variant-region-script (invalid BCP 47 order)
            {"En", "Variant", "Us", "latn"},
            "en-variant-US-Latn"},
        {
            // Extended language subtag (primary language subtag +
            // 3-letter ISO639-3] code) + script + region.
            {"En", "Ext", "latn", "Us"},
            "en-ext-Latn-US"},
        {
            // A single-character subtag starts an extension or a
            // private use subtag sequence.
            {"En", "A", "Latn", "Us"},
            "en-a-latn-us"},
        {
            // A leading single-character subtag (BCP 47 only allows
            // this for "x", i.e. private use subtags sequence).
            {"X", "En", "Us"},
            "x-en-us"},
        {
            {
                "En", "Ext", "LATN", "Gb", "Variant", "X", "En",
                "LATN", "Gb"},
            "en-ext-Latn-GB-variant-x-en-latn-gb"},
        {
            {"En", "LATN", "Gb"},
            "en_Latn_GB",
            {
                LangTagFormat::Separator::Underscore,
                LangTagFormat::LetterCase::Bcp47}},
        {
            {"En", "Latn", "Gb"},
            "EN-LATN-GB",
            {
                LangTagFormat::Separator::Hyphen,
                LangTagFormat::LetterCase::Upper}},
        {
            {"En", "Latn", "Gb"},
            "en-latn-gb",
            {
                LangTagFormat::Separator::Hyphen,
                LangTagFormat::LetterCase::Lower}},
    };

    for (const auto& test : tests) {
        const auto actualTag = buildLangTag(
            test.subtags, test.format);
        if (actualTag == test.tag)
            continue;

        test::failure(
            "buildLangTag() with " + test::toStr(test.subtags) + "\n"
            "  Expected " + test::toStr(test.tag) + "\n"
            "  Got      " + test::toStr(actualTag));
    }
}


void testLangTagUtils()
{
    testBuildLangTag();
}


}


TEST_REGISTER(testLangTagUtils);
