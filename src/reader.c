/**
 * Monk configuration reader API
 *
 * @copyright Antloop Studios, 2020 <https://antloop.world/>
 * @license MIT License <https://choosealicense.com/licenses/mit/>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "reader.h"

MonkSourcePosition MonkSourcePosition_fromIndex(const char *source, const size_t index)
{
    size_t line = 1, column = 1;

    for (size_t c = 0; c < index; c++)
    {
        if (source[c] == '\n')
        {
            line++;
            column = 0;
        }

        column++;
    }

    return (MonkSourcePosition) {line, column};
}

/*
 * TOKENIZER ------------------------------------------------------------------
 */

enum MonkTokenType
{
    T_END,
    T_KEY,
    T_KEY_QUOTED,
    T_STRING,
    T_MAP_START,
    T_MAP_END,
    T_LIST_START,
    T_LIST_END,
};

typedef struct
{
    enum MonkTokenType type;
    size_t from, to;
    int indent;
} MonkToken;

typedef struct
{
    const char *source;
    size_t source_length;

    MonkToken *tokens;
    size_t token_count;
} MonkTokenStream;

typedef struct
{
    MonkTokenStream ok;
    MonkError err;
} MonkTokenStreamResult;

void MonkTokenStream_drop(MonkTokenStream *this)
{
    if (this && this->tokens)
    {
        free(this->tokens);
        this->tokens = NULL;
    }
}

void MonkTokenStream_push(MonkTokenStream *this, MonkToken t)
{
    this->tokens = realloc(this->tokens, sizeof(MonkToken) * ++this->token_count);
    this->tokens[this->token_count - 1] = t;
}

MonkTokenStreamResult MonkTokenStream_fromString(const char *source, const size_t length)
{
    static const char NON_KEY_CHARS[] = ";{[]}\"'\n\r\t ";

    MonkTokenStream tokens = (MonkTokenStream) {source, length, NULL, 0};

    if (!source || length == 0)
    {
        return (MonkTokenStreamResult) {.ok = tokens};
    }

    int indent = 0;
    for (size_t c = 0; c < length; c++)
    {
        switch (source[c])
        {
            case ';': // comment, skip till end of line
                while (source[++c] != '\n') { }
                break;

            case '{': // map start
                MonkTokenStream_push(&tokens, (MonkToken) {T_MAP_START, c, c + 1, indent});
                break;
            case '}': // map end
                MonkTokenStream_push(&tokens, (MonkToken) {T_MAP_END, c, c + 1, indent});
                break;
            case '[': // list start
                MonkTokenStream_push(&tokens, (MonkToken) {T_LIST_START, c, c + 1, indent});
                break;
            case ']': // list end
                MonkTokenStream_push(&tokens, (MonkToken) {T_LIST_END, c, c + 1, indent});
                break;

            case '"': // string
            case '\'':
            case '`': // quoted key
            {
                char delimiter = source[c];
                size_t to = c++;

                while (source[++to] != delimiter && to < length)
                {
                    if (source[to] == '\\')
                    {
                        to++;
                    }
                }

                if (to >= length)
                {
                    MonkTokenStream_drop(&tokens);
                    return (MonkTokenStreamResult) {
                        .err = {MONK_ERR_UNCLOSED_STRING, MonkSourcePosition_fromIndex(source, c - 1)}};
                }

                if (delimiter == '`')
                {
                    MonkTokenStream_push(&tokens, (MonkToken) {T_KEY_QUOTED, c, to, indent});
                }
                else
                {
                    MonkTokenStream_push(&tokens, (MonkToken) {T_STRING, c, to, indent});
                }

                c = to;
            }
            break;

            case ' ': // whitespace
            case '\t':
            case '\r':
                break;

            case '\n': // newline
            {
                indent = 0;
                while (source[c + 1] == ' ' || source[c + 1] == '\t')
                {
                    indent++;
                    c++;
                }
            }
            break;

            default: // unquoted key
            {
                size_t to = c;
                while (!strchr(NON_KEY_CHARS, source[++to])) { }
                MonkTokenStream_push(&tokens, (MonkToken) {T_KEY, c, to, indent});
                c = to - 1;
            }
        }
    }

    MonkTokenStream_push(&tokens, (MonkToken) {T_END, 0, 0, 0});
    --tokens.token_count;

    return (MonkTokenStreamResult) {.ok = tokens};
}

/*
 * PARSER ---------------------------------------------------------------------
 */

typedef struct
{
    MonkObject ok;
    MonkError err;
} MonkObjectResult;

MonkObjectResult MonkObject_fromTokenStream(MonkTokenStream *stream, size_t *token);

MonkSourcePosition MonkSourcePosition_fromToken(MonkTokenStream *stream, const size_t token)
{
    if (!(stream->tokens && stream->source && token < stream->token_count))
    {
        return (MonkSourcePosition) {0, 0};
    }

    return MonkSourcePosition_fromIndex(stream->source, stream->tokens[token].from);
}

MonkObjectResult MonkString_fromTokenStream(MonkTokenStream *stream, size_t *token)
{
    const MonkToken *t = &stream->tokens[*token];
    const char *source = stream->source;

    char *aligned = malloc(t->to - t->from + 1);

    size_t a_size = 0;

    for (size_t c = t->from; c < t->to; c++)
    {
        aligned[a_size++] = source[c];

        if (source[c] == '\n')
        {
            int dent = t->indent;
            while ((source[c + 1] == ' ' || source[c + 1] == '\t') && dent--)
            {
                c++;
            }
        }

        if (source[c] == '\\')
        {
            switch (source[++c])
            {
                case 'n':
                    aligned[++a_size] = '\n';
                    break;
                case 'r':
                    aligned[++a_size] = '\r';
                    break;
                case 't':
                    aligned[++a_size] = '\t';
                    break;
                case '\n': // exclude newline and trim indent
                {
                    int dent = t->indent;
                    while ((source[c + 1] == ' ' || source[c + 1] == '\t') && dent--)
                    {
                        c++;
                    }
                }
                break;
            }
        }
    }

    aligned = realloc(aligned, a_size + 1);
    aligned[a_size] = 0;

    return (MonkObjectResult) {.ok = {.as.string = aligned, MONK_STRING}};
}

MonkObjectResult MonkList_fromTokenStream(MonkTokenStream *stream, size_t *i_token)
{
    MonkList list = {.size = 0, .items = malloc(sizeof(MonkObject) * 4), .alloc_size = 4};
    size_t list_start = *i_token;

    while (stream->tokens[++*i_token].type != T_LIST_END)
    {
        if (*i_token >= stream->token_count)
        {
            MonkList_drop(&list);
            return (MonkObjectResult) {.err
                                       = {MONK_ERR_UNCLOSED_LIST, MonkSourcePosition_fromToken(stream, list_start)}};
        }

        MonkObjectResult child = MonkObject_fromTokenStream(stream, i_token);

        if (child.err.code == MONK_ERR_OK)
        {
            if (list.size >= list.alloc_size)
                list.items = realloc(list.items, sizeof(MonkObject) * (list.alloc_size *= 2));

            list.items[list.size++] = child.ok;
        }
        else
        {
            MonkList_drop(&list);
            if (child.err.code == MONK_ERR_EXPECTED_VALUE)
            {
                return (MonkObjectResult) {.err = {MONK_ERR_EXPECTED_LIST_VALUE, child.err.position}};
            }
            return child;
        }
    }

    return (MonkObjectResult) {.ok = {.as.list = list, .type = MONK_LIST}};
}

MonkObjectResult MonkMap_fromTokenStream(MonkTokenStream *stream, size_t *i_token, int is_root)
{
    MonkMap map = {.size = 0, .items = malloc(sizeof(MonkMapEntry) * 4), .alloc_size = 4};

    if (!stream || stream->token_count == 0)
    {
        return (MonkObjectResult) {.ok = (MonkObject) {.as.map = map, MONK_MAP}};
    }

    if (!is_root)
    {
        ++*i_token;
    }

    size_t map_start = *i_token;

    while (is_root ? (*i_token < stream->token_count) : (stream->tokens[*i_token].type != T_MAP_END))
    {
        if (!is_root && *i_token >= stream->token_count)
        {
            MonkMap_drop(&map);
            return (MonkObjectResult) {.err = {MONK_ERR_UNCLOSED_MAP, MonkSourcePosition_fromToken(stream, map_start)}};
        }

        MonkToken token = stream->tokens[*i_token];

        if (!(token.type == T_KEY || token.type == T_KEY_QUOTED))
        {
            MonkMap_drop(&map);
            return (MonkObjectResult) {.err = {is_root ? MONK_ERR_EXPECTED_ROOT_KEY : MONK_ERR_EXPECTED_MAP_KEY,
                                               MonkSourcePosition_fromToken(stream, *i_token)}};
        }

        char *key = NULL;

        if (token.type == T_KEY_QUOTED)
        {
            MonkObjectResult keyResult = MonkString_fromTokenStream(stream, i_token);
            key = keyResult.ok.as.string;
        }
        else
        {
            size_t key_length = token.to - token.from;
            key = malloc(key_length + 1);
            memcpy(key, stream->source + token.from, key_length);
            key[key_length] = 0;
        }

        if (MonkMap_find(&map, key) != (size_t)-1)
        {
            free(key);
            MonkMap_drop(&map);
            return (MonkObjectResult) {.err = {MONK_ERR_DUPLICATE_KEY, MonkSourcePosition_fromToken(stream, *i_token)}};
        }

        ++*i_token;

        MonkObjectResult child = MonkObject_fromTokenStream(stream, i_token);

        ++*i_token;

        if (child.err.code == MONK_ERR_OK)
        {
            if (map.size >= map.alloc_size)
                map.items = realloc(map.items, sizeof(MonkMapEntry) * (map.alloc_size *= 2));

            map.items[map.size++] = (MonkMapEntry) {Monk_hash((unsigned char *)key), key, child.ok};
        }
        else
        {
            free(key);
            MonkMap_drop(&map);

            if (child.err.code == MONK_ERR_EXPECTED_VALUE)
            {
                return (MonkObjectResult) {.err = {MONK_ERR_EXPECTED_MAP_VALUE, child.err.position}};
            }

            return child;
        }
    }

    return (MonkObjectResult) {.ok = {.as.map = map, MONK_MAP}};
}

MonkObjectResult MonkObject_fromTokenStream(MonkTokenStream *stream, size_t *i_token)
{
    MonkObjectResult result;

    switch (stream->tokens[*i_token].type)
    {
        case T_LIST_START:
            result = MonkList_fromTokenStream(stream, i_token);
            break;
        case T_MAP_START:
            result = MonkMap_fromTokenStream(stream, i_token, 0);
            break;
        case T_STRING:
            result = MonkString_fromTokenStream(stream, i_token);
            break;

        default:
            return (MonkObjectResult) {.err
                                       = {MONK_ERR_EXPECTED_VALUE, MonkSourcePosition_fromToken(stream, *i_token)}};
    }

    return result;
}

MonkConfigResult MonkConfig_fromTokenStream(MonkTokenStream *stream)
{
    size_t token = 0;

    MonkObjectResult result = MonkMap_fromTokenStream(stream, &token, 1);

    switch (result.err.code)
    {
        case MONK_ERR_OK:
            return (MonkConfigResult) {.ok = {result.ok.as.map}};
        default:
            return (MonkConfigResult) {.err = result.err};
    }
}

/*
 * PUBLIC API -----------------------------------------------------------------
 */

MonkConfigResult MonkConfig_fromString(const char *source, const size_t length)
{
    MonkTokenStreamResult tokenResult = MonkTokenStream_fromString(source, length);
    switch (tokenResult.err.code)
    {
        case MONK_ERR_OK:
        {
            MonkTokenStream tokens = tokenResult.ok;
            MonkConfigResult configResult = MonkConfig_fromTokenStream(&tokens);
            MonkTokenStream_drop(&tokens);

            return configResult;
        }
        break;

        default:
            return (MonkConfigResult) {.err = tokenResult.err};
    }
}

MonkObject *MonkList_get(MonkList *this, const size_t index)
{
    return index < this->size ? &this->items[index] : NULL;
}

MonkObject *MonkMap_get(MonkMap *this, const char *key)
{
    size_t index = MonkMap_find(this, key);
    return index == (size_t)-1 ? NULL : &this->items[index].value;
}

MonkObject *MonkConfig_get(MonkConfig *this, const char *key)
{
    return MonkMap_get(&this->data, key);
}

void MonkPrintParseError(MonkError error)
{
    static const char *messages[] = {
        "",                                                                      // MONK_ERR_OK,
        "error[%d:%d]: unclosed List\n",                                         // MONK_ERR_UNCLOSED_LIST,
        "error[%d:%d]: unclosed Map\n",                                          // MONK_ERR_UNCLOSED_MAP,
        "error[%d:%d]: unclosed String\n",                                       // MONK_ERR_UNCLOSED_STRING,
        "error[%d:%d]: expected a Key followed by a Value; or end of Map '}'\n", // MONK_ERR_EXPECTED_MAP_KEY,
        "error[%d:%d]: expected a Key followed by a Value\n",                    // MONK_ERR_EXPECTED_ROOT_KEY,
        "error[%d:%d]: expected String, List or Map\n",                          // MONK_ERR_EXPECTED_VALUE,
        "error[%d:%d]: expected String, List, Map, or end of List ']'\n",        // MONK_ERR_EXPECTED_LIST_VALUE,
        "error[%d:%d]: expected String, List or Map\n",                          // MONK_ERR_EXPECTED_MAP_VALUE,
        "error[%d:%d]: duplicate Key\n"                                          // MONK_ERR_DUPLICATE_KEY
    };

    printf(messages[error.code], error.position.line, error.position.column);
}
