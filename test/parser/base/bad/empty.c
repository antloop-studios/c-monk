#include <string.h>

#include "src/reader.h"
#include "test/unit.h"

void test_parser_base_bad_empty()
{
    char *src = "";

    MonkConfigResult result = MonkConfig_fromString(src, strlen(src));
    test_assert((MonkPrintParseError(result.err), result.err.code == MONK_ERR_OK));

    MonkConfig config = result.ok;
    test_assert(config.data.size == 0);

    MonkConfig_drop(&result.ok);
    test_ok();
}
