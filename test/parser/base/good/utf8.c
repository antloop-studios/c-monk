#include <string.h>

#include "src/reader.h"
#include "test/unit.h"

int _assert_key(MonkMapEntry *pair, const char *expect_key)
{
    const char *key = pair->key;
    return key && strcmp(key, expect_key) == 0;
}

int _assert_value(MonkMapEntry *pair, const char *expect_value)
{
    MonkObject value = pair->value;
    return value.type == MONK_STRING && strcmp(value.as.string, expect_value) == 0;
}

void test_parser_base_good_utf8()
{
    char *src = "ᚠᛇᚻ        'ᚠᛇᚻ᛫ᛒᛦᚦ᛫ᚠᚱᚩᚠᚢᚱ᛫ᚠᛁᚱᚪ᛫ᚷᛖᚻᚹᛦᛚᚳᚢᛗ'\n"
                "γλώσσα     'Τη γλώσσα μου έδωσαν ελληνική'\n"
                "ვეპხის     'ვეპხის ტყაოსანი შოთა რუსთაველი'\n"
                "काचं         'काचं शक्नोम्यत्तुम् । नोपहिनस्ति माम् ॥'\n"
                "我能吞     '我能吞下玻璃而不伤身体'\n"
                "私はガ     '私はガラスを食べられます'\n";

    MonkConfigResult result = MonkConfig_fromString(src, strlen(src));
    test_assert((MonkPrintParseError(result.err), result.err.code == MONK_ERR_OK));

    MonkConfig config = result.ok;
    test_assert(config.data.size == 6);

    test_assert(_assert_key(&config.data.items[0], "ᚠᛇᚻ"));
    test_assert(_assert_value(&config.data.items[0], "ᚠᛇᚻ᛫ᛒᛦᚦ᛫ᚠᚱᚩᚠᚢᚱ᛫ᚠᛁᚱᚪ᛫ᚷᛖᚻᚹᛦᛚᚳᚢᛗ"));

    test_assert(_assert_key(&config.data.items[1], "γλώσσα"));
    test_assert(_assert_value(&config.data.items[1], "Τη γλώσσα μου έδωσαν ελληνική"));

    test_assert(_assert_key(&config.data.items[2], "ვეპხის"));
    test_assert(_assert_value(&config.data.items[2], "ვეპხის ტყაოსანი შოთა რუსთაველი"));

    test_assert(_assert_key(&config.data.items[3], "काचं"));
    test_assert(_assert_value(&config.data.items[3], "काचं शक्नोम्यत्तुम् । नोपहिनस्ति माम् ॥"));

    test_assert(_assert_key(&config.data.items[4], "我能吞"));
    test_assert(_assert_value(&config.data.items[4], "我能吞下玻璃而不伤身体"));

    test_assert(_assert_key(&config.data.items[5], "私はガ"));
    test_assert(_assert_value(&config.data.items[5], "私はガラスを食べられます"));

    MonkConfig_drop(&result.ok);
    test_ok();
}
