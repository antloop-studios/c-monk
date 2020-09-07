#include <string.h>

#include "src/writer.h"
#include "test/unit.h"

void test_writer_simple()
{
    MonkConfig conf = {0};

    MonkConfig_putString(&conf, "key", "a");
    MonkConfig_putString(&conf, "key2", "b");
    MonkConfig_putString(&conf, "key3", "c");

    test_assert(conf.data.size == 3);

    test_assert(strcmp(conf.data.items[0].key, "key") == 0);
    test_assert(strcmp(conf.data.items[1].key, "key2") == 0);
    test_assert(strcmp(conf.data.items[2].key, "key3") == 0);

    test_assert(conf.data.items[0].value.type == MONK_STRING);
    test_assert(conf.data.items[1].value.type == MONK_STRING);
    test_assert(conf.data.items[2].value.type == MONK_STRING);

    test_assert(strcmp(conf.data.items[0].value.as.string, "a") == 0);
    test_assert(strcmp(conf.data.items[1].value.as.string, "b") == 0);
    test_assert(strcmp(conf.data.items[2].value.as.string, "c") == 0);

    MonkConfig_del(&conf, "key2");

    test_assert(conf.data.size == 2);

    test_assert(strcmp(conf.data.items[0].key, "key") == 0);
    test_assert(strcmp(conf.data.items[1].key, "key3") == 0);

    test_assert(conf.data.items[0].value.type == MONK_STRING);
    test_assert(conf.data.items[1].value.type == MONK_STRING);

    test_assert(strcmp(conf.data.items[0].value.as.string, "a") == 0);
    test_assert(strcmp(conf.data.items[1].value.as.string, "c") == 0);

    MonkConfig_drop(&conf);
    test_ok();
}
