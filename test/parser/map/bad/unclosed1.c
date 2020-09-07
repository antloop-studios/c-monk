#include <string.h>

#include "src/reader.h"
#include "test/unit.h"

void test_parser_map_bad_unclosed1()
{
    char *src = "key {";

    MonkConfigResult result = MonkConfig_fromString(src, strlen(src));

    if (result.err.code != MONK_ERR_UNCLOSED_MAP)
    {
        MonkPrintParseError(result.err);
    }

    test_assert(result.err.code == MONK_ERR_UNCLOSED_MAP);

    test_ok();
}
