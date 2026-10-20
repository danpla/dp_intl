About
=====

dp_intl is a [Gettext]-compatible internationalization library for
C++.

Links:

*   Website and API reference: https://danpla.github.io/dp_intl
*   Source code and issue tracker: https://github.com/danpla/dp_intl

[Gettext]: https://en.wikipedia.org/wiki/Gettext


Improvements over Gettext
-------------------------

dp_intl is intended to be an enhanced version of the Gettext runtime
library (also known as libintl), with the following improvements:

*   **No dependency on the locale**

    The only way to set the language in Gettext is to call the
    setlocale() function from the C standard library. In turn, this
    will alter the behavior of other locale-dependent C routines,
    leading to subtle bugs that only arise in specific locales.

    For example, calling setlocale() with `LC_ALL` will change all
    locale categories, including `LC_NUMERIC` that affects the decimal
    separator in functions that convert floating-point number types to
    or from strings, such as snprintf() and strtof(). A number
    serialized with a locale that uses a period as a decimal separator
    will not be parsed correctly with a locale that uses a comma, and
    vice versa.

    Even if you don't explicitly use the affected C routines in your
    code, they might still be used in the third-party libraries,
    including the C++ standard library, where (as of C++26) the
    std::stof/d() functions are implemented on top of strtof/d(), and
    where std::to_string() prior to C++26 was implemented on top of
    sprintf().

    To keep `LC_NUMERIC` intact, you can avoid `LC_ALL` and instead
    explicitly set only `LC_CTYPE` and `LC_MESSAGES` - the only two
    categories that required by Gettext. `LC_CTYPE`, however, will
    still affect the functions from `ctype.h` and `strings.h`, such as
    tolower(), strcasecmp(), etc. In particular, your code will break
    with the Turkish locale, where the uppercase of "i" is "İ" and the
    lowercase of "I" is "ı".

    In contrast, dp_intl does not depend on the locale. Instead, it
    explicitly determines the preferred language based on the system
    interface language settings.

*   **No platform-specific number types**

    The Gettext routines for translating plural form messages use the
    `unsigned long` type for numbers. The size of this type depends on
    the platform: for example, it is 64 bits wide on 64-bit Unix-like
    systems (Linux, macOS) but 32 bits wide on 64-bit Windows. This
    makes it impossible to write cross-platform code without limiting
    your numbers to 2³² (4,294,967,295) or using similar workarounds;
    otherwise, your translations will have incorrect plural forms due
    to unsigned wrap around (modulo 2³²) on systems with a 32-bit
    `unsigned long`.

    In contrast, dp_intl uses a 64-bit integer type (`uint64_t`) on
    all platforms.

*   **Full control over file layout and loading**

    Gettext can only load message catalogs from the local file
    system; the catalogs must reside in the directory structure
    required by the bindtextdomain() function
    (`<locale>/LC_MESSAGES/<domain>.mo`). If your application stores
    catalogs in an archive or package (such as an APK on Android), you
    will need to extract them locally for Gettext to work.

    In contrast, dp_intl reads files through a virtual stream class
    that can wrap any file-like object, including those from archives
    and packages. It also imposes no requirements on file layout or
    naming conventions, since language selection and message catalog
    loading are not tied together.

*   **Any language in source strings**

    The Gettext routines for translating plural form messages are
    limited to English and several other languages with two plural
    forms: "one" for 1 and "many" for all other numbers. This makes
    it impossible to use any other language that doesn't fit this
    pattern. For example, French is not a suitable source language for
    Gettext because, although it also has two plural forms, "one" is
    used for both 0 and 1.

    In contrast, dp_intl supports source strings in any language
    because its plural rules are configurable rather than hardcoded in
    the API.

Smaller improvements include:

*   Ability to use `std::string_view` in addition to C strings

*   Exceptions with comprehensive messages to report errors

*   Small code size, with no dependencies outside the C++ standard
    library


Gettext compatibility
---------------------

dp_intl is designed to be compatible with the Gettext runtime library,
which means the following:

*   Compatible interface

    The translation routines of dp_intl are modeled after those of the
    Gettext library. The only two differences are that dp_intl is a
    C++ library (so its routines are class methods rather than free
    functions), and the order of the arguments is different.

*   Tool compatibility

    Although dp_intl does not implement the Gettext tools for working
    with message catalog files, its API is designed to be compatible
    with the `xgettext` tool, which extracts translatable strings from
    source code. This means that the workflow for message catalogs is
    the same as for any other Gettext-based project, except that you
    must specify `-k/--keyword` options for `xgettext` so that it
    recognizes the dp_intl API calls.

*   Message catalog compatibility

    dp_intl fully supports the message catalog file format (MO files)
    used by the Gettext runtime library, including the full support
    for plural rule formulas defined in the "Plural-Forms" header
    entry. The message catalogs **must use UTF-8** (see below).

There is one area where dp_intl is not compatible with Gettext:
**dp_intl only works with UTF-8**. Unlike the Gettext library, it
doesn't support conversion from 1-byte encodings at runtime. This is
intentional: before UTF-8 became common, Gettext had to support
various 1-byte encodings to work with different languages, but now
that the whole world has adopted UTF-8, this is no longer practical.
Even the `msgfmt` utility from the GNU Gettext project, starting with
version 0.22, creates MO files in UTF-8 by default, regardless of the
source PO file encoding.

If your MO message catalogs use 1-byte encodings, you will need to
convert them to UTF-8 in one of the following ways:

*   Regenerate the MO files using `msgfmt` version 0.22 or newer. The
    encoding of the source PO files can stay the same.

*   Convert the PO files to UTF-8 and regenerate the MOs. You can do
    the conversion either using the `msgconv` tool from the GNU
    Gettext project, or manually. In the latter case, don't forget to
    update the "Content-Type" header entry in each PO file to use
    "charset=UTF-8".

If you don't have access to the original PO files, you can recover
them from MOs using the `msgunfmt` utility from the GNU Gettext
project.


Integration
===========

There are three primary ways to integrate dp_intl into your project:
embed it directly into your project's source tree, fetch it on demand
using [FetchContent], or use a traditional system-wide installation.
In all cases, you will need a C++ compiler that supports C++17, and
CMake >= 3.22.

[FetchContent]: https://cmake.org/cmake/help/latest/module/FetchContent.html


Embedding
---------

Since dp_intl is a small library with no external dependencies, you
can include it directly in your project's source tree and then use
add_subdirectory():

    add_subdirectory("path/to/dp_intl")
    target_link_libraries(your_target PRIVATE dp_intl::dp_intl)

To minimize the number of files in the source tree, you can leave only
the following in the dp_intl directory:

    include/
    src/
    CMakeLists.txt
    license.txt


FetchContent
------------

With [FetchContent], CMake will automatically download dp_intl source
code, either as an archive or directly from the Git repository:

    include(FetchContent)

    FetchContent_Declare(
        dp_intl
        URL https://github.com/danpla/dp_intl/archive/refs/tags/v<version>.tar.gz
        URL_HASH SHA256=<TAR.GZ file hash>)
    FetchContent_MakeAvailable(dp_intl)

    target_link_libraries(your_target PRIVATE dp_intl::dp_intl)


Installation
------------

For a traditional installation, run the following commands from the
dp_intl directory:

    cmake -DCMAKE_BUILD_TYPE=Release -B build
    cmake --build build
    cmake --install build --strip

You can use the `CMAKE_INSTALL_PREFIX` option to change the default
installation directory.

In addition to the dp_intl library and its header files, the
installation provides:

*   A CMake package:

        find_package(dp_intl REQUIRED)
        target_link_libraries(your_target PRIVATE dp_intl::dp_intl)

*   A [pkg-config] file for other build systems:

        pkg-config --cflags --libs dp_intl

[pkg-config]: https://www.freedesktop.org/wiki/Software/pkg-config/


Build-time options
------------------

You can configure the library using CMake options, which you can view
by running `cmake -LH -B build`. In addition to the standard CMake
options that start with `CMAKE_`, there are several dp_intl-specific
options starting with `DP_INTL_`. Probably the only such option that
will be interesting for the end user is `DP_INTL_BUILD_SHARED_LIB`,
which enables building a shared library instead of a static one.

To set options for the library added via add_subdirectory() or
FetchContent, use the set() command before calling add_subdirectory()
or FetchContent_MakeAvailable():

    set(DP_INTL_BUILD_SHARED_LIB YES)
    add_subdirectory("path/to/dp_intl")

To set options for the library being built for installation, use the
`-D` flag during the CMake configuration step:

    cmake -DDP_INTL_BUILD_SHARED_LIB=YES -B build
    cmake --build build


Documentation
=============

Since the dp_intl library only implements the runtime part of Gettext
(libintl), the overall translation workflow is the same as for any
Gettex-based project, and is already described in detail in the
[GNU Gettext manual]. If you are unfamiliar with Gettext, it is
recommended that you read the "Introduction" section first to
familiarize yourself with the basic Gettext concepts before diving
into the dp_intl API.

The dp_intl API documentation is available online and in the library
headers. The best way to start exploring the API is to begin with
these two main entities:

*   The dp::intl::Translator class to load a message catalog (MO file)
    and perform the translation.

*   The dp::intl::matchLang() function to select the language (and the
    corresponding MO file) to be used by the Translator class.

[GNU Gettext manual]: https://www.gnu.org/software/gettext/manual/html_node/index.html
