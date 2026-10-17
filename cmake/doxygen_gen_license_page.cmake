file(READ "${LICENSE_PATH}" LICENSE)
string(
    JOIN "\n"
    LICENSE_PAGE
    "\\page license License"
    ""
    "${LICENSE}")
file(WRITE "${LICENSE_PAGE_PATH}" "${LICENSE_PAGE}")
