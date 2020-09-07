/**
 * Monk configuration common API
 *
 * @author Antloop Studios, 2020 <https://antloop.world/>
 * @license MIT License <https://choosealicense.com/licenses/mit/>
 */

#include <stdlib.h>
#include <string.h>

#include "common.h"

/**
 * djb2 hash algorithm
 * @see http://www.cse.yorku.ca/~oz/hash.html
 */
unsigned long Monk_hash(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + (unsigned long)c; /* hash * 33 + c */

    return hash;
}

size_t MonkMap_find(MonkMap *this, const char *key)
{
    size_t found = (size_t)-1;

    if (this->items && this->size > 0)
    {
        unsigned long hash = Monk_hash((unsigned char *)key);

        for (size_t i = 0; i < this->size; i++)
        {
            if (this->items[i].hash == hash) // quicker iterative lookup
            {
                // double check key as well in case there's a collision
                if (strcmp(this->items[i].key, key) == 0)
                {
                    found = i;
                    break;
                }
            }
        }
    }

    return found;
}

void MonkObject_drop(MonkObject *this);

void MonkList_drop(MonkList *this)
{
    if (this && this->items)
    {
        for (size_t i = 0; i < this->size; i++)
        {
            MonkObject_drop(&this->items[i]);
        }

        free(this->items);
        this->items = NULL;
        this->size = 0;
    }
}

void MonkMapEntry_drop(MonkMapEntry *this)
{
    if (this)
    {
        free(this->key);
        this->key = NULL;
        MonkObject_drop(&this->value);
    }
}

void MonkMap_drop(MonkMap *this)
{
    if (this && this->items)
    {
        for (size_t i = 0; i < this->size; i++)
        {
            MonkMapEntry_drop(&this->items[i]);
        }

        free(this->items);
        this->items = NULL;
        this->size = 0;
    }
}

void MonkObject_drop(MonkObject *this)
{
    if (this)
    {
        switch (this->type)
        {
            case MONK_STRING:
                if (this->as.string)
                {
                    free(this->as.string);
                    this->as.string = NULL;
                }
                break;
            case MONK_LIST:
                MonkList_drop(&this->as.list);
                break;
            case MONK_MAP:
                MonkMap_drop(&this->as.map);
                break;
        }
    }
}

void MonkConfig_drop(MonkConfig *this)
{
    if (this)
    {
        MonkMap_drop(&this->data);
    }
}
