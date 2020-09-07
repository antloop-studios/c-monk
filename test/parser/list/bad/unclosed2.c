#include <string.h>

#include "src/reader.h"
#include "test/unit.h"

void test_parser_list_bad_unclosed2()
{
    char *src = "key ['the' 'list' 'is' 'open'";

    MonkConfigResult result = MonkConfig_fromString(src, strlen(src));

    if (result.err.code != MONK_ERR_UNCLOSED_LIST)
    {
        MonkPrintParseError(result.err);
    }

    test_assert(result.err.code == MONK_ERR_UNCLOSED_LIST);

    test_ok();
}
