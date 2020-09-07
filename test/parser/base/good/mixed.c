#include <string.h>

#include "src/reader.h"
#include "test/unit.h"

void test_parser_base_good_mixed()
{
    char *src = "mykey1 \"value\"\n"
                "mykey2 [ 'myvalue' ]\n"
                "mykey3 {\n"
                "   mykey 'myvalue'\n"
                "}";

    MonkConfigResult result = MonkConfig_fromString(src, strlen(src));
    test_assert((MonkPrintParseError(result.err), result.err.code == MONK_ERR_OK));

    MonkConfig config = result.ok;
    test_assert(config.data.size == 3);

    {
        MonkMapEntry item = config.data.items[0];

        const char *key = item.key;
        test_assert(key);
        test_assert(strcmp(key, "mykey1") == 0);

        MonkObject value = item.value;
        test_assert(value.type == MONK_STRING);
        test_assert(strcmp(value.as.string, "value") == 0);
    }

    {
        MonkMapEntry item = config.data.items[1];

        const char *key = item.key;
        test_assert(key);
        test_assert(strcmp(key, "mykey2") == 0);

        MonkObject value = item.value;
        test_assert(value.type == MONK_LIST);

        MonkList list = value.as.list;
        test_assert(list.size == 1);

        MonkObject listItem = list.items[0];
        test_assert(listItem.type == MONK_STRING);
        test_assert(listItem.as.string);
        test_assert(strcmp(listItem.as.string, "myvalue") == 0);
    }

    {
        MonkMapEntry item = config.data.items[2];

        const char *key = item.key;
        test_assert(key);
        test_assert(strcmp(key, "mykey3") == 0);

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
    }

    MonkConfig_drop(&result.ok);
    test_ok();
}
