#include <string.h>

#include "src/reader.h"
#include "test/unit.h"

void test_parser_base_good_map2()
{
    char *src = "mykey {\n"
                "   mykey 'myvalue'\n"
                "}\n";

    MonkConfigResult result = MonkConfig_fromString(src, strlen(src));
    test_assert((MonkPrintParseError(result.err), result.err.code == MONK_ERR_OK));

    MonkConfig config = result.ok;
    test_assert(config.data.size == 1);

    MonkMapEntry item = config.data.items[0];

    const char *key = item.key;
    test_assert(key);
    test_assert(strcmp(key, "mykey") == 0);

    MonkObject value = item.value;
    test_assert(value.type == MONK_MAP);

    MonkMap map = value.as.map;
    test_assert(map.size == 1);

    MonkMapEntry mapItem = map.items[0];

    const char *itemKey = mapItem.key;
    test_assert(itemKey);
    test_assert(strcmp(itemKey, "mykey") == 0);

    MonkObject itemValue = mapItem.value;
    test_assert(itemValue.type == MONK_STRING);
    test_assert(strcmp(itemValue.as.string, "myvalue") == 0);

    MonkConfig_drop(&result.ok);
    test_ok();
}
