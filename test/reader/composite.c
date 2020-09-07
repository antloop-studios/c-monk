#include <string.h>

#include "src/reader.h"
#include "test/unit.h"

void test_reader_composite()
{
    char *src = "mykey {\n"
                "   a 'first'\n"
                "   b {\n"
                "       second \"second\"\n"
                "       another \"item\"\n"
                "   }\n"
                "   c ['3']\n"
                "}\n"
                "key2[['first']'second'{and'third'}]";

    MonkConfigResult result = MonkConfig_fromString(src, strlen(src));
    test_assert((MonkPrintParseError(result.err), result.err.code == MONK_ERR_OK));

    MonkConfig config = result.ok;
    test_assert(config.data.size == 2);

    {
        MonkObject *item = MonkConfig_get(&config, "mykey");
        test_assert(item);
        test_assert(item->type == MONK_MAP);

        MonkMap *map = &item->as.map;
        test_assert(map->size == 3);

        {
            MonkObject *child = MonkMap_get(map, "a");
            test_assert(child);
            test_assert(child->type == MONK_STRING);
            test_assert(strcmp(child->as.string, "first") == 0);
        }

        {
            MonkObject *child = MonkMap_get(map, "b");
            test_assert(child);
            test_assert(child->type == MONK_MAP);

            {
                MonkMap *map = &child->as.map;
                test_assert(map->size == 2);

                {
                    MonkObject *child = MonkMap_get(map, "second");
                    test_assert(child);
                    test_assert(child->type == MONK_STRING);
                    test_assert(strcmp(child->as.string, "second") == 0);
                }

                {
                    MonkObject *child = MonkMap_get(map, "another");
                    test_assert(child);
                    test_assert(child->type == MONK_STRING);
                    test_assert(strcmp(child->as.string, "item") == 0);
                }
            }
        }

        {
            MonkObject *child = MonkMap_get(map, "c");
            test_assert(child);
            test_assert(child->type == MONK_LIST);

            {
                MonkList *list = &child->as.list;

                test_assert(list->size == 1);

                MonkObject *item = MonkList_get(list, 0);
                test_assert(item);
                test_assert(item->type == MONK_STRING);
                test_assert(strcmp(item->as.string, "3") == 0);
            }
        }
    }

    {
        MonkObject *item = MonkConfig_get(&config, "key2");
        test_assert(item);
        test_assert(item->type == MONK_LIST);

        MonkList *list = &item->as.list;
        test_assert(list->size == 3);

        {
            MonkObject *child = MonkList_get(list, 0);
            test_assert(child);
            test_assert(child->type == MONK_LIST);

            MonkList *list = &child->as.list;
            test_assert(list->size == 1);

            MonkObject *item = MonkList_get(list, 0);
            test_assert(item);
            test_assert(item->type == MONK_STRING);
            test_assert(strcmp(item->as.string, "first") == 0);
        }

        {
            MonkObject *child = MonkList_get(list, 1);
            test_assert(child);
            test_assert(child->type == MONK_STRING);
            test_assert(strcmp(child->as.string, "second") == 0);
        }

        {
            MonkObject *child = MonkList_get(list, 2);
            test_assert(child);
            test_assert(child->type == MONK_MAP);

            {
                MonkMap *map = &child->as.map;
                test_assert(map->size == 1);

                {
                    MonkObject *child = MonkMap_get(map, "and");
                    test_assert(child);
                    test_assert(child->type == MONK_STRING);
                    test_assert(strcmp(child->as.string, "third") == 0);
                }
            }
        }
    }

    MonkConfig_drop(&config);
    test_ok();
}
