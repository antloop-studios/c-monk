#include <string.h>

#include "src/reader.h"
#include "test/unit.h"

void test_parser_base_bad_unclosed()
{
    char *src = "key \"this string has no end";

    MonkConfigResult result = MonkConfig_fromString(src, strlen(src));

    if (result.err.code != MONK_ERR_UNCLOSED_STRING)
    {
        MonkPrintParseError(result.err);
    }

    test_assert(result.err.code == MONK_ERR_UNCLOSED_STRING);

    MonkConfig_drop(&result.ok);
    test_ok();
}
