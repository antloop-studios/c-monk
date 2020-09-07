#include <string.h>

#include "src/reader.h"
#include "test/unit.h"

void test_parser_base_good_list2()
{
    char *src = "mykey ['myvalue']";

    MonkConfigResult result = MonkConfig_fromString(src, strlen(src));
    test_assert((MonkPrintParseError(result.err), result.err.code == MONK_ERR_OK));

    MonkConfig config = result.ok;
    test_assert(config.data.size == 1);

    MonkMapEntry item = config.data.items[0];

    const char *key = item.key;
    test_assert(key);
    test_assert(strcmp(key, "mykey") == 0);

    MonkObject value = item.value;
    test_assert(value.type == MONK_LIST);

    MonkList list = value.as.list;
    test_assert(list.size == 1);

    MonkObject listItem = list.items[0];
    test_assert(listItem.type == MONK_STRING);
    test_assert(listItem.as.string);
    test_assert(strcmp(listItem.as.string, "myvalue") == 0);

    MonkConfig_drop(&result.ok);
    test_ok();
}
