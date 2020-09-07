#include <string.h>

#include "src/reader.h"
#include "test/unit.h"

void test_parser_list_good_maps()
{
    char *src = "mykey [\n"
                "   { firstkey 'first' }\n"
                "   { key2 'second' }\n"
                "   { 3key 'third' }\n"
                "]\n";

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
    test_assert(list.size == 3);

    {
        MonkObject item = list.items[0];
        test_assert(item.type == MONK_MAP);

        MonkMap map = item.as.map;
        test_assert(map.size == 1);

        MonkMapEntry mapItem = map.items[0];

        const char *itemKey = mapItem.key;
        test_assert(itemKey);
        test_assert(strcmp(itemKey, "firstkey") == 0);

        MonkObject itemValue = mapItem.value;
        test_assert(itemValue.type == MONK_STRING);
        test_assert(strcmp(itemValue.as.string, "first") == 0);
    }

    {
        MonkObject item = list.items[1];
        test_assert(item.type == MONK_MAP);

        MonkMap map = item.as.map;
        test_assert(map.size == 1);

        MonkMapEntry mapItem = map.items[0];

        const char *itemKey = mapItem.key;
        test_assert(itemKey);
        test_assert(strcmp(itemKey, "key2") == 0);

        MonkObject itemValue = mapItem.value;
        test_assert(itemValue.type == MONK_STRING);
        test_assert(strcmp(itemValue.as.string, "second") == 0);
    }

    {
        MonkObject item = list.items[2];
        test_assert(item.type == MONK_MAP);

        MonkMap map = item.as.map;
        test_assert(map.size == 1);

        MonkMapEntry mapItem = map.items[0];

        const char *itemKey = mapItem.key;
        test_assert(itemKey);
        test_assert(strcmp(itemKey, "3key") == 0);

        MonkObject itemValue = mapItem.value;
        test_assert(itemValue.type == MONK_STRING);
        test_assert(strcmp(itemValue.as.string, "third") == 0);
    }

    MonkConfig_drop(&result.ok);
    test_ok();
}
