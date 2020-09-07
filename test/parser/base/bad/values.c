#include <string.h>

#include "src/reader.h"
#include "test/unit.h"

void test_parser_base_bad_values()
{
    char *src = "'this' ['is'] {a 'a'} 'failure'";

    MonkConfigResult result = MonkConfig_fromString(src, strlen(src));

    if (result.err.code != MONK_ERR_EXPECTED_ROOT_KEY)
    {
        MonkPrintParseError(result.err);
    }

    test_assert(result.err.code == MONK_ERR_EXPECTED_ROOT_KEY);

    MonkConfig_drop(&result.ok);
    test_ok();
}
