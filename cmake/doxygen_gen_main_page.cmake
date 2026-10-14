file(READ "${README_PATH}" README)
string(
    JOIN "\n"
    MAIN_PAGE
    "\\mainpage notitle"
    "\\tableofcontents"
    ""
    "${README}")
file(WRITE "${MAIN_PAGE_PATH}" "${MAIN_PAGE}")
