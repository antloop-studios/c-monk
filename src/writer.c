/**
 * Monk configuration writer API
 *
 * @copyright Antloop Studios, 2020 <https://antloop.world/>
 * @license MIT License <https://choosealicense.com/licenses/mit/>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "writer.h"

/*
 * INTERNAL -------------------------------------------------------------------
 */

MonkObject *MonkList_put(MonkList *this, MonkObject item)
{
    if (!this)
    {
        return NULL;
    }

    if (!this->items)
    {
        this->items = malloc(sizeof(MonkObject) * 4);
        this->alloc_size = 4;
    }

    if (this->size >= this->alloc_size)
    {
        this->items = realloc(this->items, sizeof(MonkObject) * (this->alloc_size *= 2));
    }

    this->items[this->size++] = item;

    return &this->items[this->size - 1];
}

MonkObject *MonkMap_put(MonkMap *this, const char *key, MonkObject item)
{
    if (!this)
    {
        return NULL;
    }

    if (!this->items)
    {
        this->items = malloc(sizeof(MonkMapEntry) * 4);
        this->alloc_size = 4;
    }

    char *key_dup = malloc(strlen(key) + 1);
    strcpy(key_dup, key);

    size_t found = MonkMap_find(this, key);

    if (found != (size_t)-1)
    {
        MonkMapEntry_drop(&this->items[found]);
        this->items[found] = (MonkMapEntry) {Monk_hash((unsigned char *)key_dup), key_dup, item};

        return &this->items[found].value;
    }
    else
    {
        if (this->size >= this->alloc_size)
        {
            this->items = realloc(this->items, sizeof(MonkMapEntry) * (this->alloc_size *= 2));
        }

        this->items[this->size++] = (MonkMapEntry) {Monk_hash((unsigned char *)key_dup), key_dup, item};

        return &this->items[this->size - 1].value;
    }

    return NULL;
}

MonkObject *MonkConfig_put(MonkConfig *this, const char *key, MonkObject item)
{
    return MonkMap_put(&this->data, key, item);
}

char *MonkObject_export(MonkObject *this, int indent);

char *MonkString_export(const char *string, int indent)
{
    size_t len = strlen(string);
    size_t len_new = len + 3; // string + quotes + newline

    char *result = malloc(len_new + 1);

    result[0] = '"';

    for (size_t i = 0, j = 1; i <= len; i++, j++)
    {
        switch (string[i])
        {
            case '\n':
                result = realloc(result, (len_new += (size_t)indent + 1) + 1);
                snprintf(result + j, (size_t)indent + 2, "\n%*s", indent, "");
                j += (size_t)indent;
                break;

            case '\\':
            case '"':
                result = realloc(result, ++len_new + 1);
                result[j++] = '\\';
            default:
                result[j] = string[i];
        }
    }

    strcat(result, "\"\n");

    return result;
}

char *MonkMap_export(MonkMap *this, int indent)
{
    char *result = calloc(1, 1);

    if (indent >= 0)
    {
        size_t tail_size = strlen("{\n") + 1;
        result = realloc(result, strlen(result) + tail_size);
        snprintf(result + strlen(result), tail_size, "{\n");
    }

    indent += 4;

    for (size_t i = 0; i < this->size; i++)
    {
        char *key = this->items[i].key;

        size_t tail_size = (size_t)indent + strlen(key) + strlen("`` ") + 1;
        result = realloc(result, strlen(result) + tail_size);
        snprintf(result + strlen(result), tail_size, "%*s`%s` ", indent, "", key);

        char *str = MonkObject_export(&this->items[i].value, indent);

        result = realloc(result, strlen(result) + strlen(str) + 1);
        strcat(result, str);

        free(str);
    }

    indent -= 4;

    if (indent >= 0)
    {
        size_t tail_size = strlen("}\n") + (size_t)indent + 1;
        result = realloc(result, strlen(result) + tail_size);
        snprintf(result + strlen(result), tail_size, "%*s}\n", indent, "");
    }

    return result;
}

char *MonkList_export(MonkList *this, int indent)
{
    char *result = calloc(1, 1);

    {
        size_t tail_size = strlen("[\n") + 1;
        result = realloc(result, strlen(result) + tail_size);
        snprintf(result + strlen(result), tail_size, "[\n");
    }

    indent += 4;

    for (size_t i = 0; i < this->size; i++)
    {
        char *str = MonkObject_export(&this->items[i], indent);
        size_t tail_size = strlen(str) + (size_t)indent + 1;

        result = realloc(result, strlen(result) + tail_size);
        snprintf(result + strlen(result), tail_size, "%*s%s", indent, "", str);
        free(str);
    }

    indent -= 4;

    {
        size_t tail_size = strlen("]\n") + (size_t)indent + 1;
        result = realloc(result, strlen(result) + tail_size);
        snprintf(result + strlen(result), tail_size, "%*s]\n", indent, "");
    }

    return result;
}

char *MonkObject_export(MonkObject *object, int indent)
{
    char *str;

    switch (object->type)
    {
        case MONK_LIST:
            str = MonkList_export(&object->as.list, indent);
            break;
        case MONK_MAP:
            str = MonkMap_export(&object->as.map, indent);
            break;
        case MONK_STRING:
            str = MonkString_export(object->as.string, indent);
            break;
    }

    return str;
}

/*
 * PUBLIC API -----------------------------------------------------------------
 */

char *MonkList_putString(MonkList *this, const char *item)
{
    char *item_dup = malloc(strlen(item) + 1);
    strcpy(item_dup, item);

    MonkList_put(this, (MonkObject) {.as.string = item_dup, MONK_STRING});

    return item_dup;
}

MonkList *MonkList_putList(MonkList *this, const MonkList item)
{
    return &MonkList_put(this, (MonkObject) {.as.list = item, MONK_LIST})->as.list;
}

MonkMap *MonkList_putMap(MonkList *this, const MonkMap item)
{
    return &MonkList_put(this, (MonkObject) {.as.map = item, MONK_MAP})->as.map;
}

void MonkList_del(MonkList *this, const size_t index)
{
    if (this)
    {
        if (index < this->size)
        {
            MonkObject_drop(&this->items[index]);
            memcpy(this->items + index, this->items + index + 1, sizeof(MonkObject) * --this->size);
        }
    }
}

char *MonkMap_putString(MonkMap *this, const char *key, const char *item)
{
    char *item_dup = malloc(strlen(item) + 1);
    strcpy(item_dup, item);

    MonkMap_put(this, key, (MonkObject) {.as.string = item_dup, MONK_STRING});

    return item_dup;
}

MonkList *MonkMap_putList(MonkMap *this, const char *key, const MonkList item)
{
    return &MonkMap_put(this, key, (MonkObject) {.as.list = item, MONK_LIST})->as.list;
}

MonkMap *MonkMap_putMap(MonkMap *this, const char *key, const MonkMap item)
{
    return &MonkMap_put(this, key, (MonkObject) {.as.map = item, MONK_MAP})->as.map;
}

void MonkMap_del(MonkMap *this, const char *key)
{
    if (this)
    {
        size_t found = MonkMap_find(this, key);

        if (found != (size_t)-1)
        {
            MonkMapEntry_drop(&this->items[found]);

            memcpy(this->items + found, this->items + found + 1, sizeof(MonkMapEntry) * --this->size);
        }
    }
}

char *MonkConfig_putString(MonkConfig *this, const char *key, const char *item)
{
    return MonkMap_putString(&this->data, key, item);
}

MonkList *MonkConfig_putList(MonkConfig *this, const char *key, const MonkList item)
{
    return MonkMap_putList(&this->data, key, item);
}

MonkMap *MonkConfig_putMap(MonkConfig *this, const char *key, const MonkMap item)
{
    return MonkMap_putMap(&this->data, key, item);
}

void MonkConfig_del(MonkConfig *this, const char *key)
{
    MonkMap_del(&this->data, key);
}

char *MonkConfig_export(MonkConfig *this)
{
    return MonkMap_export(&this->data, -4);
}
