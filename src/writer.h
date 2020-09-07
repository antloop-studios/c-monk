/**
 * Monk configuration writer API
 *
 * @copyright Antloop Studios, 2020 <https://antloop.world/>
 * @license MIT License <https://choosealicense.com/licenses/mit/>
 */

#ifndef ANTLOOP_MONK_WRITER_H
#define ANTLOOP_MONK_WRITER_H

#include <stddef.h>

#include "common.h"

/**
 * Append a String (char*).
 * This method will allocate and copy the string internally, then return pointer to the new copy.
 * This copy will be released when the List, or the root Config, is dropped.
 */
char *MonkList_putString(MonkList *this, const char *item);

/**
 * Append a List.
 * This method will allocate or extend the internal array if necessary, insert the new item and
 * return pointer to the new copy.
 * Do not drop the original List as the original data pointer is kept as-is (shallow copy).
 * The data store will be released when the List, or the root Config is dropped.
 */
MonkList *MonkList_putList(MonkList *this, const MonkList item);

/**
 * Append a Map.
 * This method will allocate or extend the internal array if necessary, insert the new item and
 * return pointer to the new copy.
 * Do not drop the original Map as the original data pointer is kept as-is (shallow copy).
 * The data store will be released when the List, or the root Config is dropped.
 */
MonkMap *MonkList_putMap(MonkList *this, const MonkMap item);

/**
 * Delete an item at index.
 * This method will clear the item at the index and move data back an element.
 */
void MonkList_del(MonkList *this, const size_t index);

/**
 * Insert a String (char*).
 * If the key exists, the existing item will be dropped and the new one will be inserted in place.
 * This method will allocate and copy the string internally, then return pointer to the new copy.
 * This copy will be released when the Map, or the root Config, is dropped.
 */
char *MonkMap_putString(MonkMap *this, const char *key, const char *item);

/**
 * Insert a List.
 * If the key exists, the existing item will be dropped and the new one will be inserted in place.
 * This method will allocate or extend the internal array if necessary, insert the new item and
 * return pointer to the item location.
 * Do not drop the original List as the original data pointer is kept as-is (shallow copy).
 * The data store will be released when the Map, or the root Config is dropped.
 */
MonkList *MonkMap_putList(MonkMap *this, const char *key, const MonkList item);

/**
 * Insert a Map.
 * If the key exists, the existing item will be dropped and the new one will be inserted in place.
 * This method will allocate or extend the internal array if necessary, insert the new item and
 * return pointer to the item location.
 * Do not drop the original Map as the original data pointer is kept as-is (shallow copy).
 * The data store will be released when the Map, or the root Config is dropped.
 */
MonkMap *MonkMap_putMap(MonkMap *this, const char *key, const MonkMap item);

/**
 * Delete an item with Key.
 * This method will clear the item with the key and move internal data back an element.
 */
void MonkMap_del(MonkMap *this, const char *key);

/**
 * Insert a String (char*).
 * If the key exists, the existing item will be dropped and the new one will be inserted in place.
 * This method will allocate and copy the string internally, then return pointer to the new copy.
 * This copy will be released when the Map, or the root Config, is dropped.
 */
char *MonkConfig_putString(MonkConfig *this, const char *key, const char *item);

/**
 * Insert a List.
 * If the key exists, the existing item will be dropped and the new one will be inserted in place.
 * This method will allocate or extend the internal array if necessary, insert the new item and
 * return pointer to the item location.
 * Do not drop the original List as the original data pointer is kept as-is (shallow copy).
 * The data store will be released when the Map, or the root Config is dropped.
 */
MonkList *MonkConfig_putList(MonkConfig *this, const char *key, const MonkList item);

/**
 * Insert a Map.
 * If the key exists, the existing item will be dropped and the new one will be inserted in place.
 * This method will allocate or extend the internal array if necessary, insert the new item and
 * return pointer to the item location.
 * Do not drop the original Map as the original data pointer is kept as-is (shallow copy).
 * The data store will be released when the Map, or the root Config is dropped.
 */
MonkMap *MonkConfig_putMap(MonkConfig *this, const char *key, const MonkMap item);

/**
 * Delete an item with Key.
 * This method will clear the item with the key and move internal data back an element.
 */
void MonkConfig_del(MonkConfig *this, const char *key);

/**
 * Export the Config in a readable format as a string (char*).
 * This method allocates, free the resulting string when you're done with it.
 */
char *MonkConfig_export(MonkConfig *this);

#endif
