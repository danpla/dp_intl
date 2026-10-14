#include "flow.h"

#include <memory>
#include <optional>
#include <string>
#include <type_traits>

#include "dp_intl/file_stream.h"
#include "dp_intl/translator.h"
#include "dp_intl/translator_plural_spec.h"

#include "files_dir.h"
#include "utils.h"


using namespace dp::intl;


namespace {


using Tr = Translator<GermanicPluralSpec>;


static_assert(std::is_nothrow_default_constructible_v<Tr>);

static_assert(!std::is_copy_constructible_v<Tr>);
static_assert(!std::is_copy_assignable_v<Tr>);

static_assert(std::is_nothrow_move_constructible_v<Tr>);
static_assert(std::is_nothrow_move_assignable_v<Tr>);


template<typename StreamT = FileStream>
std::unique_ptr<StreamT> openFile(std::string_view fileName)
{
    try {
        return std::make_unique<StreamT>(
            test::filesDirPath / fileName);
    } catch (const Error& e) {
        throw test::FatalError{
            test::toStr(fileName) + ": " + e.what()};
    }
}


Tr loadFromFile(std::string_view fileName)
{
    try {
        return Tr{*openFile(fileName)};
    } catch (const Error& e) {
        throw test::FatalError{
            "Translator can't load " + test::toStr(fileName) + ": "
            + e.what()};
    }
}


void testLoading()
{
    const struct {
        std::string name;
        std::string errorText;
    } tests[]{
        {
            "duplicate_header",
            "Duplicate header entry"},
        {
            "header_content_type_charset_empty",
            "\"Content-Type\" field: Invalid \"charset=\" property: "
            "Value is empty"},
        {
            "header_content_type_charset_iso-8859-1",
            "Unsupported charset \"ISO-8859-1\""},
        {
            "header_content_type_charset_missing",
            "\"Content-Type\" field: Missing \"charset\" property"},
        {
            "header_content_type_charset_placeholder",
            "Unsupported charset \"CHARSET\""},
        {
            "header_content_type_charset_utf-8_lower", ""},
        {
            "header_content_type_charset_utf-8_upper", ""},
        {
            "header_content_type_text_plain_missing", ""},
        {
            "header_empty_properties", ""},
        {
            "header_missing", ""},
        {
            "header_plural_forms_empty",
            "\"Plural-Forms\" field: Missing \"nplurals\" property"},
        {
            "header_plural_forms_nplurals_-2",
            "\"Plural-Forms\" field: Invalid \"nplurals\" property: "
            "Invalid number format \"-2\""},
        {
            "header_plural_forms_nplurals_0",
            "Number of plural forms must be > 0"},
        {
            "header_plural_forms_nplurals_15", ""},
        {
            "header_plural_forms_nplurals_16",
            "Suspiciously large number of plural forms 16"},
        {
            "header_plural_forms_nplurals_18446744073709551615",
            "Suspiciously large number of plural forms "
            "18446744073709551615"},
        {
            "header_plural_forms_nplurals_18446744073709551616",
            "\"Plural-Forms\" field: Invalid \"nplurals\" property: "
            "Number 18446744073709551616[...] is out of range"},
        {
            "header_plural_forms_nplurals_empty",
            "\"Plural-Forms\" field: Invalid \"nplurals=\" property: "
            "Value is empty"},
        {
            "header_plural_forms_nplurals_missing",
            "\"Plural-Forms\" field: Missing \"nplurals\" property"},
        {
            "header_plural_forms_plural_empty",
            "\"Plural-Forms\" field: Invalid \"plural=\" property: "
            "Value is empty"},
        {
            "header_plural_forms_plural_missing",
            "\"Plural-Forms\" field: Missing \"plural\" property"},
        {
            "missing_translated_string_plural_forms",
            "Less than 2 plural forms in translated string (missing "
            "null separators)"},
        {
            "normal_ru", ""},
        {
            "normal_ru_big_endian", ""},
        {
            "source_string_without_null_offset_44",
            "source string at offset 44: No null terminator at "
            "offset 48"},
        {
            "translated_string_without_null_offset_49",
            "translated string at offset 49: No null terminator at "
            "offset 54"},
        {
            "unsupported_major_revision",
            "Unsupported major revision 2"},
    };

    for (const auto& test : tests) {
        const auto moName = test.name + ".mo";
        const auto isValid = test.errorText.empty();

        try {
            Tr tr{*openFile(moName)};

            if (!isValid)
                test::failure(moName + " is expected to be invalid");
        } catch (const Error& e) {
            const std::string_view errorText{e.what()};

            if (isValid) {
                test::failure(
                    moName + " is expected to be valid, but got "
                    "error: " + std::string{errorText});
                continue;
            }

            if (errorText.find(test.errorText) == errorText.npos)
                test::failure(
                    moName + ": Error message "
                    + test::toStr(errorText) + " doesn't match "
                    + test::toStr(test.errorText));
        }
    }
}



void testTranslate(
    std::string_view call,
    std::string_view result,
    std::string_view expectedResult)
{
    if (result == expectedResult)
        return;

    test::failure(
        std::string{call}
        + ": expected " + test::toStr(expectedResult) + ", got "
        + test::toStr(result));
}


#define TEST_TRANSLATE(CALL, EXPECTED_RESULT) \
    testTranslate(#CALL, CALL, EXPECTED_RESULT);


void testTranslation()
{
    const auto tr = loadFromFile("normal_ru.mo");

    #define TEST(EXPECTED, METHOD, ...) \
        TEST_TRANSLATE(tr.METHOD(__VA_ARGS__), EXPECTED); \
        TEST_TRANSLATE(tr.METHOD ## CStr(__VA_ARGS__), EXPECTED)

    TEST("?", translate, "?");
    TEST("?", translate, "?");
    TEST("@Text", translate, "Text");

    TEST("Text", translateContext, "Context", "Text");
    TEST("Text", translateN, 1, "Text", "Text");
    TEST("Text", translateContextN, "Context", 1, "Text", "Text");

    // ===

    TEST("? (context)", translateContext, "Context", "? (context)");
    TEST(
        "@Text (context)", translateContext, "Context",
        "Text (context)");

    TEST("Text (context)", translate, "Text (context)");
    TEST(
        "Text (context)", translateN, 1, "Text (context)",
        "Text (context)");
    TEST(
        "Text (context)", translateContextN, "Context", 1,
        "Text (context)", "Text (context)");

    // ===

    TEST("?One", translateN, 1, "?One", "?Many");
    TEST("?Many", translateN, 2, "?One", "?Many");

    #define TEST_N(EXPECTED, N) \
        TEST(EXPECTED, translateN, N, "One", "Many")

    TEST_N("@One", 1);
    TEST_N("@One", 21);
    TEST_N("@One", 31);

    TEST_N("@Few", 2);
    TEST_N("@Few", 3);
    TEST_N("@Few", 4);

    TEST_N("@Many", 0);
    TEST_N("@Many", 5);
    TEST_N("@Many", 6);

    #undef TEST_N

    TEST("One", translate, "One");
    TEST("One", translateContext, "Context", "One");
    TEST("One", translateContextN, "Context", 1, "One", "Many");

    // ===

    TEST(
        "?One (context)", translateContextN, "Context", 1,
        "?One (context)", "?Many (context)");
    TEST(
        "?Many (context)", translateContextN, "Context", 2,
        "?One (context)", "?Many (context)");

    #define TEST_N(EXPECTED, N) \
        TEST( \
            EXPECTED, translateContextN, "Context", N, \
            "One (context)", "Many (context)")

    TEST_N("@One (context)", 1);
    TEST_N("@One (context)", 21);
    TEST_N("@One (context)", 31);

    TEST_N("@Few (context)", 2);
    TEST_N("@Few (context)", 3);
    TEST_N("@Few (context)", 4);

    TEST_N("@Many (context)", 0);
    TEST_N("@Many (context)", 5);
    TEST_N("@Many (context)", 6);

    #undef TEST_N

    TEST("One (context)", translate, "One (context)");
    TEST(
        "One (context)", translateN, 1, "One (context)",
        "Many (context)");
    TEST(
        "One (context)", translateContext, "Context",
        "One (context)");

    #undef TEST
}


void testImplicitGermanicPluralForms()
{
    const auto tr = loadFromFile("implicit_plural_forms.mo");

    TEST_TRANSLATE(tr.translateN(0, "Text", "Text"), "@Many");
    TEST_TRANSLATE(tr.translateN(1, "Text", "Text"), "@One");
    TEST_TRANSLATE(tr.translateN(2, "Text", "Text"), "@Many");
}


void testUnsortedMo()
{
    const auto tr = loadFromFile("unsorted.mo");

    for (auto c : std::string_view{"abcde"})
        if (std::string id(1, c); tr.translate(id) != "@" + id) {
            test::failure(
                "Lookup in unsorted MO file doesn't work properly");
            break;
        }
}


void testLimits()
{
    // When testing a file size limit such as an offset, we don’t want
    // to keep/generate an MO file that's dozens of megabytes in size
    // just to make the offset to be valid (i.e. be inside the file
    // bounds). At the same time, we need to make sure that the
    // exception we received was actually caused by exceeding the
    // limit, and not by an attempt to read data beyond the end of the
    // file.
    //
    // The FileStreamProxy class addresses this issue by reporting an
    // error when attempting to read or position the stream past the
    // end of the file, thereby indicating that the test data is
    // misconfigured. For example, if a file has an offset that is
    // smaller than the limit we are trying to test, the library will
    // accept it and read the rest of the (truncated) file.
    struct FileStreamProxy : Stream {
        explicit FileStreamProxy(const std::filesystem::path& path)
            : f{path}
            , fPath{path.u8string()}
        {
            try {
                fSize = static_cast<std::int64_t>(
                    std::filesystem::file_size(path));
            } catch (const std::filesystem::filesystem_error& e) {
                throw Error{
                    std::string{"filesystem::file_size(): "}
                    + e.what()};
            }
        }

        std::size_t readSome(void* dst, std::size_t dstSize) override
        {
            const auto dstSizeS64 = static_cast<std::int64_t>(
                dstSize);

            if (fSize - fPos < dstSizeS64)
                reportError(
                    "An attempt to read past the end of file");

            const auto numRead = f.readSome(dst, dstSize);
            fPos += numRead;
            return numRead;
        }

        void setPosition(std::int64_t newPosition) override
        {
            if (newPosition > fSize)
                reportError(
                    "An attempt to set position past the end of "
                    "file");

            f.setPosition(newPosition);
            fPos = newPosition;
        }
    private:
        FileStream f;
        std::string fPath;
        std::int64_t fSize;
        std::int64_t fPos{};

        void reportError(const char* message) const
        {
            test::failure(
                "Misconfigured test for " + test::toStr(fPath) + ": "
                + message);
        }
    };

    struct {
        std::string name;
        std::string errorText;
    } tests[]{
        {
            "too_large_source+translated_string_data",
            "Can't read translated strings table at offset 540: "
            "Total string data size exceeds limit"},
        {
            "too_large_source_string_data",
            "Can't read source strings table at offset 28: "
            "Total string data size exceeds limit"},
        {
            "too_large_source_string_offset",
            "Can't read source strings table at offset 28: "
            "Can't read string descriptor at offset 28: "
            "Can't read string offset: "
            "Offset 64000001 exceeds file size limit"},
        {
            "too_large_source_string_offset+size",
            "Can't read source strings table at offset 28: "
            "Can't read string descriptor at offset 28: "
            "String offset (63999996) + size (5, including null "
            "terminator) exceed file size limit"},
        {
            "too_large_source_string_size",
            "Can't read source strings table at offset 28: "
            "Can't read string descriptor at offset 28: "
            "Suspiciously long (1000001 bytes) string"},
        {
            "too_large_source_string_table_offset",
            "Can't read source strings table offset: "
            "Offset 64000001 exceeds file size limit"},
        {
            "too_large_string_count",
            "Suspiciously large string count 500001"},
        {
            "too_large_translated_string_data",
            "Can't read translated strings table at offset 540: "
            "Total string data size exceeds limit"},
        {
            "too_large_translated_string_offset",
            "Can't read translated strings table at offset 36: "
            "Can't read string descriptor at offset 36: "
            "Can't read string offset: "
            "Offset 64000001 exceeds file size limit"},
        {
            "too_large_translated_string_offset+size",
            "Can't read translated strings table at offset 36: "
            "Can't read string descriptor at offset 36: "
            "String offset (63999995) + size (6, including null "
            "terminator) exceed file size limit"},
        {
            "too_large_translated_string_size",
            "Can't read translated strings table at offset 36: "
            "Can't read string descriptor at offset 36: "
            "Suspiciously long (1000001 bytes) string"},
        {
            "too_large_translated_string_table_offset",
            "Can't read translated strings table offset: "
            "Offset 64000001 exceeds file size limit"},
    };

    for (const auto& test : tests) {
        const auto moName = test.name + ".mo";

        try {
            Tr tr{*openFile<FileStreamProxy>(moName)};
            test::failure(moName + " is expected to be invalid");
        } catch (const Error& e) {
            const std::string_view errorText{e.what()};

            if (errorText.find(test.errorText) == errorText.npos)
                test::failure(
                    moName + ": Error message "
                    + test::toStr(errorText) + " doesn't match "
                    + test::toStr(test.errorText));
        }
    }
}


void testUtf8()
{
    {
        const std::string moName = "utf8_normal.mo";

        try {
            Tr tr{*openFile(moName)};
        } catch (const Error& e) {
            test::failure(
                moName + " is expected to be valid, but got error: "
                + e.what());
        }
    }

    const struct {
        std::string name;
        std::string errorText;
    } tests[]{
        {
            "utf8_exceeds_limit_0xf4908080_U+110000",
            "U+110000 exceeds U+10FFFF"},
        {
            "utf8_invalid_lead_0x80",
            "Invalid leading byte 0x80"},
        {
            "utf8_invalid_lead_0x82",
            "Invalid leading byte 0x82"},
        {
            "utf8_invalid_lead_0xbf",
            "Invalid leading byte 0xbf"},
        {
            "utf8_invalid_lead_0xf8",
            "Invalid leading byte 0xf8"},
        {
            "utf8_invalid_lead_0xff",
            "Invalid leading byte 0xff"},
        {
            "utf8_overlong_0xc080_U+0000",
            "Overlong encoding for U+0000"},
        {
            "utf8_overlong_0xc1bf_U+007F",
            "Overlong encoding for U+007F"},
        {
            "utf8_overlong_0xe08080_U+0000",
            "Overlong encoding for U+0000"},
        {
            "utf8_overlong_0xe09fbf_U+07FF",
            "Overlong encoding for U+07FF"},
        {
            "utf8_overlong_0xf0808080_U+0000",
            "Overlong encoding for U+0000"},
        {
            "utf8_overlong_0xf08fbfbf_U+FFFF",
            "Overlong encoding for U+FFFF"},
        {
            "utf8_surrogate_0xeda080_U+D800",
            "Surrogate U+D800"},
        {
            "utf8_surrogate_0xedbfbf_U+DFFF",
            "Surrogate U+DFFF"},
        {
            "utf8_truncated_2-byte_sequence_0xd0",
            "Truncated 2-byte sequence"},
        {
            "utf8_truncated_3-byte_sequence_0xe280",
            "Truncated 3-byte sequence"},
        {
            "utf8_truncated_4-byte_sequence_0xf09f98",
            "Truncated 4-byte sequence"},
    };

    for (const auto& test : tests) {
        const auto moName = test.name + ".mo";

        try {
            Tr tr{*openFile(moName)};
            test::failure(moName + " is expected to be invalid");
        } catch (const Error& e) {
            const std::string_view errorText{e.what()};

            if (errorText.find(test.errorText) == errorText.npos)
                test::failure(
                    moName + ": Error message "
                    + test::toStr(errorText) + " doesn't match "
                    + test::toStr(test.errorText));
        }
    }
}


void testTranslator()
{
    testLoading();
    testTranslation();
    testImplicitGermanicPluralForms();
    testUnsortedMo();
    testLimits();
    testUtf8();
}


}


TEST_REGISTER(testTranslator);
