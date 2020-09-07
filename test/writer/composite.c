#include <string.h>

#include "src/writer.h"
#include "test/unit.h"

void test_writer_composite()
{
    MonkConfig conf = {0};
    {
        MonkMap *map = MonkConfig_putMap(&conf, "stuff", (MonkMap) {0});
        {
            MonkList *child = MonkMap_putList(map, "a", (MonkList) {0});
            MonkList_putString(child, "hello");
        }
        {
            MonkList *child = MonkMap_putList(map, "b", (MonkList) {0});
            MonkList_putString(child, "world");
        }
        {
            MonkMap *child = MonkMap_putMap(map, "c", (MonkMap) {0});
            MonkMap_putString(child, "key", "value");
            {
                MonkList *child2 = MonkMap_putList(child, "list", (MonkList) {0});
                {
                    MonkMap *child3 = MonkList_putMap(child2, (MonkMap) {0});
                    MonkMap_putString(child3, "3 levels deep", "a big value in\nthe nested map");
                }
            }
        }
    }

    test_assert(conf.data.size == 1);

    test_assert(strcmp(conf.data.items[0].key, "stuff") == 0);
    test_assert(conf.data.items[0].value.type == MONK_MAP);

    {
        MonkMap *map = &conf.data.items[0].value.as.map;

        test_assert(map->size == 3);

        test_assert(strcmp(map->items[0].key, "a") == 0);
        test_assert(map->items[0].value.type == MONK_LIST);
        test_assert(map->items[0].value.as.list.size == 1);
        test_assert(map->items[0].value.as.list.items[0].type == MONK_STRING);
        test_assert(strcmp(map->items[0].value.as.list.items[0].as.string, "hello") == 0);

        test_assert(strcmp(map->items[1].key, "b") == 0);
        test_assert(map->items[1].value.type == MONK_LIST);
        test_assert(map->items[1].value.as.list.size == 1);
        test_assert(map->items[1].value.as.list.items[0].type == MONK_STRING);
        test_assert(strcmp(map->items[1].value.as.list.items[0].as.string, "world") == 0);

        test_assert(strcmp(map->items[2].key, "c") == 0);
        test_assert(map->items[2].value.type == MONK_MAP);

        {
            MonkMap *child = &map->items[2].value.as.map;

            test_assert(child->size == 2);

            test_assert(strcmp(child->items[0].key, "key") == 0);
            test_assert(child->items[0].value.type == MONK_STRING);
            test_assert(strcmp(child->items[0].value.as.string, "value") == 0);

            test_assert(strcmp(child->items[1].key, "list") == 0);
            test_assert(child->items[1].value.type == MONK_LIST);

            {
                MonkList *child2 = &child->items[1].value.as.list;

                test_assert(child2->size == 1);

                test_assert(child2->items[0].type == MONK_MAP);

                {
                    MonkMap *child3 = &child2->items[0].as.map;

                    test_assert(child3->size == 1);

                    test_assert(strcmp(child3->items[0].key, "3 levels deep") == 0);
                    test_assert(child3->items[0].value.type == MONK_STRING);
                    test_assert(
                        strcmp(child3->items[0].value.as.string, "a big value in\nthe nested map")
                        == 0);
                }
            }
        }
    }

    MonkConfig_drop(&conf);
    test_ok();
}
