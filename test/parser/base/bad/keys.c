#include <string.h>

#include "src/reader.h"
#include "test/unit.h"

void test_parser_base_bad_keys()
{
    char *src = "this is a failure";

    MonkConfigResult result = MonkConfig_fromString(src, strlen(src));

    if (result.err.code != MONK_ERR_EXPECTED_MAP_VALUE)
    {
        MonkPrintParseError(result.err);
    }

    test_assert(result.err.code == MONK_ERR_EXPECTED_MAP_VALUE);

    MonkConfig_drop(&result.ok);
    test_ok();
}
