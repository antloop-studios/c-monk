/**
 * Monk configuration reader API
 *
 * @copyright Antloop Studios, 2020 <https://antloop.world/>
 * @license MIT License <https://choosealicense.com/licenses/mit/>
 */

#ifndef ANTLOOP_MONK_READER_H
#define ANTLOOP_MONK_READER_H

#include <stddef.h>

#include "common.h"

/*
 * ERROR HANDLING -------------------------------------------------------------
 */

enum MonkParseError
{
    MONK_ERR_OK,
    MONK_ERR_UNCLOSED_LIST,
    MONK_ERR_UNCLOSED_MAP,
    MONK_ERR_UNCLOSED_STRING,
    MONK_ERR_EXPECTED_MAP_KEY,
    MONK_ERR_EXPECTED_ROOT_KEY,
    MONK_ERR_EXPECTED_VALUE,
    MONK_ERR_EXPECTED_LIST_VALUE,
    MONK_ERR_EXPECTED_MAP_VALUE,
    MONK_ERR_DUPLICATE_KEY
};

typedef struct
{
    size_t line, column;
} MonkSourcePosition;

typedef struct
{
    enum MonkParseError code;
    MonkSourcePosition position;
} MonkError;

/**
 * This is the type returned by MonkConfig_fromString.
 * Before using .ok value, .err.code must be checked for MonkParseError status above.
 */
typedef struct
{
    MonkConfig ok;
    MonkError err;
} MonkConfigResult;

/**
 * Print a Monk parse error in a human readable format
 *
 * Usage:
 *      MonkConfigResult cfgRes = MonkConfig_fromString(src, strlen(src));
 *      MonkPrintParseError(cfgRes.err);
 */
void MonkPrintParseError(MonkError error);

/*
 * READER API -----------------------------------------------------------------
 */

/**
 * Parse a string containing Monk configuration.
 *
 *! Call MonkConfig_drop on the MonkConfig object after you are done,
 *! to release allocated resources.
 *! If parsing fails, it is not necessary to call drop, as resources will be automatically released.
 *
 * Usage:
 *      const char *src = " keyA 'value' keyB 'value2' list ['a' 'b' 'c'] ";
 *
 *      MonkConfigResult cfgRes = MonkConfig_fromString(src, strlen(src));
 *      MonkConfig cfg;
 *
 *      switch (cfgRes.err.code) {
 *          case MONK_ERR_OK:
 *              cfg = cfgRes.ok;
 *
 *              /// use config here
 *
 *              MonkConfig_drop(&cfg); // drop resources after you're done
 *
 *              break;
 *          default:
 *              MonkPrintParseError(cfgRes.err);
 *      }
 */
MonkConfigResult MonkConfig_fromString(const char *source, const size_t length);

/**
 * Get item from a MonkList
 *
 * Usage:
 *      MonkList myList = {};
 *      MonkObject *item = MonkList_get(&myList, 0);
 *
 *      if (item) switch (item.type) {
 *          case MONK_STRING:
 *              break;
 *          case MONK_LIST:
 *              break;
 *          case MONK_MAP:
 *              break;
 *      }
 */
MonkObject *MonkList_get(MonkList *this, const size_t index);

/**
 * Get item from a MonkMap
 *
 * Usage:
 *      MonkMap myMap = {};
 *      MonkObject *item = MonkMap_get(&myMap, "itemkey");
 *
 *      if (item) switch (item.type) {
 *          case MONK_STRING:
 *              break;
 *          case MONK_LIST:
 *              break;
 *          case MONK_MAP:
 *              break;
 *      }
 */
MonkObject *MonkMap_get(MonkMap *this, const char *key);

/**
 * Get item from a MonkConfig
 *
 * Usage:
 *      MonkConfig myConfig = {};
 *      MonkObject *item = MonkConfig_get(&myConfig, "itemkey");
 *
 *      if (item) switch (item.type) {
 *          case MONK_STRING:
 *              break;
 *          case MONK_LIST:
 *              break;
 *          case MONK_MAP:
 *              break;
 *      }
 */
MonkObject *MonkConfig_get(MonkConfig *this, const char *key);

#endif
