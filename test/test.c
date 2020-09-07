#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unit.h"

#include "parser/base/bad/empty.c"
#include "parser/base/bad/keys.c"
#include "parser/base/bad/null.c"
#include "parser/base/bad/symbols.c"
#include "parser/base/bad/unclosed.c"
#include "parser/base/bad/values.c"
#include "parser/base/good/list1.c"
#include "parser/base/good/list2.c"
#include "parser/base/good/map1.c"
#include "parser/base/good/map2.c"
#include "parser/base/good/mixed.c"
#include "parser/base/good/string1.c"
#include "parser/base/good/string2.c"
#include "parser/base/good/utf8.c"

#include "parser/list/bad/keys1.c"
#include "parser/list/bad/keys2.c"
#include "parser/list/bad/unclosed1.c"
#include "parser/list/bad/unclosed2.c"
#include "parser/list/good/lists.c"
#include "parser/list/good/maps.c"
#include "parser/list/good/mixed.c"
#include "parser/list/good/strings.c"

#include "parser/map/bad/unclosed1.c"
#include "parser/map/bad/unclosed2.c"
#include "parser/map/good/lists.c"
#include "parser/map/good/maps.c"
#include "parser/map/good/mixed.c"
#include "parser/map/good/strings.c"

#include "reader/composite.c"
#include "reader/simple.c"

#include "writer/composite.c"
#include "writer/export.c"
#include "writer/simple.c"

int main()
{
    test_run_all(&test_parser_base_bad_null,
                 &test_parser_base_bad_empty,
                 &test_parser_base_bad_keys,
                 &test_parser_base_bad_values,
                 &test_parser_base_bad_symbols,
                 &test_parser_base_bad_unclosed,
                 &test_parser_base_good_string1,
                 &test_parser_base_good_string2,
                 &test_parser_base_good_utf8,
                 &test_parser_base_good_list1,
                 &test_parser_base_good_list2,
                 &test_parser_base_good_map1,
                 &test_parser_base_good_map2,
                 &test_parser_base_good_mixed,

                 &test_parser_list_bad_keys1,
                 &test_parser_list_bad_keys2,
                 &test_parser_list_bad_unclosed1,
                 &test_parser_list_bad_unclosed2,
                 &test_parser_list_good_strings,
                 &test_parser_list_good_lists,
                 &test_parser_list_good_maps,
                 &test_parser_list_good_mixed,

                 &test_parser_map_bad_unclosed1,
                 &test_parser_map_bad_unclosed2,
                 &test_parser_map_good_strings,
                 &test_parser_map_good_lists,
                 &test_parser_map_good_maps,
                 &test_parser_map_good_mixed,

                 &test_reader_simple,
                 &test_reader_composite,

                 &test_writer_simple,
                 &test_writer_composite,
                 &test_writer_export, );

    return 0;
}
