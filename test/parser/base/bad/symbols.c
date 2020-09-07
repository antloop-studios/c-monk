#include <string.h>

#include "src/reader.h"
#include "test/unit.h"

int _test_string(const char *src)
{
    MonkConfigResult result = MonkConfig_fromString(src, strlen(src));

    if (result.err.code != MONK_ERR_EXPECTED_ROOT_KEY)
    {
        MonkPrintParseError(result.err);
    }

    return result.err.code == MONK_ERR_EXPECTED_ROOT_KEY;
}

void test_parser_base_bad_symbols()
{
    test_assert(_test_string("{"));
    test_assert(_test_string("}"));
    test_assert(_test_string("["));
    test_assert(_test_string("]"));
    test_assert(_test_string("{}"));
    test_assert(_test_string("[]"));
    test_assert(_test_string("}{"));
    test_assert(_test_string("]["));

    test_ok();
}
