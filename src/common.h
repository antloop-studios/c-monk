/**
 * Monk configuration common API
 *
 * @copyright Antloop Studios, 2020 <https://antloop.world/>
 * @license MIT License <https://choosealicense.com/licenses/mit/>
 */

#ifndef ANTLOOP_MONK_COMMON_H
#define ANTLOOP_MONK_COMMON_H

#include <stddef.h>

/**
 * djb2 hash algorithm
 * @see http://www.cse.yorku.ca/~oz/hash.html
 */
unsigned long Monk_hash(unsigned char *string);

/*
 * OBJECT TYPES ---------------------------------------------------------------
 */

/**
 * Simple growing dynamic array
 *
 * Usage:
 *      MonkList list = {0}; // no special allocation, simply 0-initialize the stack variable
 *
 *      /// items can be directly iterated over
 *      for (int i = 0; i < list.size; i++) {
 *          /// item is preferrably taken by reference if you plan to modify it
 *          MonkObject *item = &list.items[i];
 *      }
 */
typedef struct MonkList
{
    size_t size; // can be used to directly iterate the list
    size_t alloc_size;
    struct MonkObject *items;
} MonkList;

/**
 * A key-value map that's not quite a hash map, as it doesn't use hashing for indices, to reduce
 * complexity for collision and reallocation logic
 *
 * Usage:
 *      MonkMap map = {0}; // no special allocation, simply 0-initialize the stack variable
 *
 *      /// items can be directly iterated over
 *      for (int i = 0; i < map.size; i++) {
 *          const char *key = map.items[i].key;
 *
 *          /// item is preferrably taken by reference if you plan to modify it
 *          MonkObject *item = &map.items[i].value;
 *      }
 */
typedef struct MonkMap
{
    size_t size; // can be used to directly iterate the map
    size_t alloc_size;
    struct MonkMapEntry *items;
} MonkMap;

enum MonkObjectType
{
    MONK_STRING,
    MONK_LIST,
    MONK_MAP,
};

/**
 * Heterogenous object type used for storage in List and Map
 */
typedef struct MonkObject
{
    union
    {
        char *string;
        MonkList list;
        MonkMap map;
    } as;
    enum MonkObjectType type;
} MonkObject;

typedef struct MonkMapEntry
{
    unsigned long hash; // hash is used for quicker iterative lookup instead of an index
    char *key;
    MonkObject value;
} MonkMapEntry;

/**
 * Main Config type, this is the root of your configuration.
 * It can be filled with data using the Writer API,
 * or loaded from a string using MonkConfig_fromString, from Reader API
 *
 * Usage:
 *      MonkConfig conf = {0}; // no special allocation, simply 0-initialize the stack variable

 *      /// items can be directly iterated over
 *      for (int i = 0; i < conf.data.size; i++) {
 *          const char *key = conf.data.items[i].key;
 *
 *          /// item is preferrably taken by reference if you plan to modify it
 *          MonkObject *item = &conf.data.items[i].value;
 *      }
 */
typedef struct
{
    MonkMap data;
} MonkConfig;

/**
 * Return index of element in a Map, if it exists, or (size_t)-1 if it doesn't
 */
size_t MonkMap_find(MonkMap *this, const char *key);

/*
 * MEMORY MANAGEMENT ----------------------------------------------------------
 */

/**
 * Recursively release all associated resources in the config object
 */
void MonkConfig_drop(MonkConfig *this);

/*
 ! INTERNAL METHODS, only here because they are shared, don't use individually unless necessary,
 ! e.g. if you constructed your own instances, but did not insert into a MonkConfig
 */
void MonkList_drop(MonkList *this);
void MonkMap_drop(MonkMap *this);
void MonkObject_drop(MonkObject *this);
void MonkMapEntry_drop(MonkMapEntry *this);

#endif
