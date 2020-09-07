#include "src/reader.h"
#include "test/unit.h"

void test_parser_base_bad_null()
{
    MonkConfigResult result = MonkConfig_fromString(NULL, 0);
    test_assert((MonkPrintParseError(result.err), result.err.code == MONK_ERR_OK));

    MonkConfig_drop(&result.ok);
    test_ok();
}
