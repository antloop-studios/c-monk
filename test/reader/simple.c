#include <string.h>

#include "src/reader.h"
#include "test/unit.h"

void test_reader_simple()
{
    char *src = "1 'a' 2 'bvalue' 3key '3value' ";

    MonkConfigResult result = MonkConfig_fromString(src, strlen(src));
    test_assert((MonkPrintParseError(result.err), result.err.code == MONK_ERR_OK));

    MonkConfig config = result.ok;
    test_assert(config.data.size == 3);

    {
        MonkObject *item = MonkConfig_get(&config, "1");
        test_assert(item);
        test_assert(item->type == MONK_STRING);
        test_assert(strcmp(item->as.string, "a") == 0);
    }

    {
        MonkObject *item = MonkConfig_get(&config, "2");
        test_assert(item);
        test_assert(item->type == MONK_STRING);
        test_assert(strcmp(item->as.string, "bvalue") == 0);
    }

    {
        MonkObject *item = MonkConfig_get(&config, "3key");
        test_assert(item);
        test_assert(item->type == MONK_STRING);
        test_assert(strcmp(item->as.string, "3value") == 0);
    }

    MonkConfig_drop(&result.ok);
    test_ok();
}
