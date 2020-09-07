#include <string.h>

#include "src/reader.h"
#include "test/unit.h"

void test_parser_map_good_mixed()
{
    char *src = "mykey {\n"
                "   a 'first'\n"
                "   b {\n"
                "       second \"second\"\n"
                "       another \"item\"\n"
                "   }\n"
                "   c ['3']\n"
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
    test_assert(map.size == 3);

    {
        MonkMapEntry mapItem = map.items[0];

        const char *itemKey = mapItem.key;
        test_assert(itemKey);
        test_assert(strcmp(itemKey, "a") == 0);

        MonkObject itemValue = mapItem.value;
        test_assert(itemValue.type == MONK_STRING);
        test_assert(strcmp(itemValue.as.string, "first") == 0);
    }

    {
        MonkMapEntry mapItem = map.items[1];

        const char *itemKey = mapItem.key;
        test_assert(itemKey);
        test_assert(strcmp(itemKey, "b") == 0);

        MonkObject itemValue = mapItem.value;
        test_assert(itemValue.type == MONK_MAP);

        MonkMap childMap = itemValue.as.map;
        test_assert(childMap.size == 2);

        {
            MonkMapEntry childItem = childMap.items[0];

            const char *childKey = childItem.key;
            test_assert(childKey);
            test_assert(strcmp(childKey, "second") == 0);

            MonkObject childValue = childItem.value;
            test_assert(childValue.type == MONK_STRING);
            test_assert(strcmp(childValue.as.string, "second") == 0);
        }

        {
            MonkMapEntry childItem = childMap.items[1];

            const char *childKey = childItem.key;
            test_assert(childKey);
            test_assert(strcmp(childKey, "another") == 0);

            MonkObject childValue = childItem.value;
            test_assert(childValue.type == MONK_STRING);
            test_assert(strcmp(childValue.as.string, "item") == 0);
        }
    }

    {
        MonkMapEntry mapItem = map.items[2];

        const char *itemKey = mapItem.key;
        test_assert(itemKey);
        test_assert(strcmp(itemKey, "c") == 0);

        MonkObject item = mapItem.value;
        test_assert(item.type == MONK_LIST);

        MonkList list = item.as.list;
        test_assert(list.size == 1);

        MonkObject listItem = list.items[0];
        test_assert(listItem.type == MONK_STRING);
        test_assert(listItem.as.string);
        test_assert(strcmp(listItem.as.string, "3") == 0);
    }

    MonkConfig_drop(&result.ok);
    test_ok();
}
