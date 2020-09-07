#include <stdlib.h>
#include <string.h>

#include "src/reader.h"
#include "src/writer.h"
#include "test/unit.h"

void test_writer_export()
{
    const char *expected = "`stuff` {\n"
                           "    `key` [\n"
                           "        \"hello\"\n"
                           "        \"world\"\n"
                           "    ]\n"
                           "    `2` [\n"
                           "        \"oof\"\n"
                           "    ]\n"
                           "    `3` [\n"
                           "        {\n"
                           "            `a` \"value\"\n"
                           "            `b` \"value\"\n"
                           "        }\n"
                           "        [\n"
                           "            \"oof\"\n"
                           "        ]\n"
                           "        {\n"
                           "            `a` \"value\"\n"
                           "            `2` [\n"
                           "                \"oof\"\n"
                           "                [\n"
                           "                    \"oof multiline\n"
                           "                    string\"\n"
                           "                ]\n"
                           "            ]\n"
                           "        }\n"
                           "    ]\n"
                           "}\n";

    MonkConfigResult result = MonkConfig_fromString(expected, strlen(expected));
    test_assert((MonkPrintParseError(result.err), result.err.code == MONK_ERR_OK));

    char *export = MonkConfig_export(&result.ok);
    MonkConfig_drop(&result.ok);

    if (strcmp(export, expected) != 0)
    {
        printf("expected: %s", expected);
        printf("exported: %s", export);
    }

    test_assert(strcmp(export, expected) == 0);

    free(export);
    test_ok();
}
