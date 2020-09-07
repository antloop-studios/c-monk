#include <string.h>

#include "src/reader.h"
#include "test/unit.h"

void test_parser_list_bad_keys1()
{
    char *src = "mykey [ key1 key2 3key ]";

    MonkConfigResult result = MonkConfig_fromString(src, strlen(src));

    if (result.err.code != MONK_ERR_EXPECTED_LIST_VALUE)
    {
        MonkPrintParseError(result.err);
    }

    test_assert(result.err.code == MONK_ERR_EXPECTED_LIST_VALUE);

    test_ok();
}
