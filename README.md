<h1 align=center>MONK</h1>

Monk is a simple and practical configuration language with few bells and whistles.

## An example

```lua
; monk - minimal configuration language

; comments start with ; and end with EOL

; there are only 3 types:
; - string
; - list
; - map

; the root type is always a map
; map key is an unquoted string that does not contain any of:
;     ';', '{', '}', '[', ']', '"', '\'', '\t' , '\r', '\n', ' '
; map and list values are heterogeneous

map_item "value"

map_item2
    "raw string
    this can span multiple lines
    the baseline is the indent of the line the quote first appears in"

other_item [
    "list item"
    [
        "list-in-list item"
    ]
    {
        map "value"
    }
]

last_item {
    child "value"
}

; minification
map_item"value"map_item2"raw string\nthis can span multiple lines\nthe baseline is the indent of the item"other_item["nested item""list item"["list-in-list item"]]
```


## Specification

### Basics

- Comments start with a semicolon `;` and end at a newline. There are no multiline comments
```lua
; hello, this is a comment
```

- The root of the configuration is always an implicit map, wrapping it with `{}` is invalid
```lua
this "is my config"
there_are "many like it"
but {
    this {
        one [
            ; "is" commented out
            "mine"
        ]
    }
}
```

### Datatypes

#### Map

- A Map starts with `{` and ends with `}`
- A Map is a heterogeneous collection of Strings, Lists and Maps associated with a Key
```lua
mymap {
    child "value"
    nested_list [
        "apple"
        {
            fruit "orange"
        }
    ]
    map {
        of "the world"
    }
}
```
- A Key is any unquoted byte sequence that does not contain any of these characters: `;{[ ]}"'\n\r\t`, **OR** is quoted with backticks `` ` ``
- Keys wrapped in `` ` `` follow the same rules as strings, `` ` `` in the middle of the key can be escaped with a backslash `\`
- `` ` `` in the middle of a key does not need to be escaped, it is treated as a simple character and will not start a quoted key
```lua
key "a typical key"
3 "this is a valid key too"
! "this as well"

.<й>? [
    "this is fine too"
]

{this is not fine} {
    and 'will error'
}
`{this is \` fine however}` {
    and 'will not error'
}

key_with`accent 'doesn\'t need escaping'
```
- The only valid map entry is a Key followed by a Value (String, List, Map)
```lua
map {
    this 'is valid'
}
```
- Key followed by a Key will return [MONK_ERR_EXPECTED_MAP_VALUE](src/reader.h#L29)
```lua
mymap {
    this is invalid
}

so is this
```
- Value without key will return [MONK_ERR_EXPECTED_MAP_KEY](src/reader.h#L25)
- In configuration root it will instead result in [MONK_ERR_EXPECTED_ROOT_KEY](src/reader.h#L26)
```lua
mymap {
    "this is not"
    "a valid map"
}
```
- Keys must be unique in the same map, duplicated keys result in [MONK_ERR_DUPLICATE_KEY](src/reader.h#L30)
```
key "value"
key "will fail"
```
- Unclosed maps return an error [MONK_ERR_UNCLOSED_MAP](src/reader.h#L23)

#### List

- A List starts with `[` and ends with `]`
- It is a heterogeneous collection of Strings, Lists and Maps
```lua
mylist [
    "bread"
    [
        "banana"
        "apple"
    ]
    {
        key "value"
    }
]
```
- Unquoted Strings (Key literals) are treated as errors [MONK_ERR_EXPECTED_LIST_VALUE](src/reader.h#L28)
```lua
mylist [
    this is wrong and will return an error
]
```
- Unclosed lists return an error [MONK_ERR_UNCLOSED_LIST](src/reader.h#L22)

#### String

- A String is any multiline byte sequence enclosed with single or double quotes
```lua
key "Здравствуйте"
```
- Quotes can be escaped using a single backslash: `\"` or `\'`
```lua
key "\"hello\""
other '\'wack\''
```
- A few common escapes are available as well: `\n` `\r` `\t`
```lua
hello "\tworld\n"
```
- Strings may merge multiple lines into one by escaping the end of line
```lua
key "this is a very long string \
that spans multiple lines \
but will result in one line when parsed"
```
- All other escapes will simply yield the byte that follows
```lua
this "does n\othing but result in a literal 'o'"
```
- The String's baseline is determined by the indentation of the line where the first quote appears
```lua
key {
    nestedkey "hello
    | the string is parsed as if the start of line is where this vertical bar is
    <-- this indent will not appear in the string,
    the baseline is matched up with the indent of the quote"

    otherkey
        "the indent of this string is two levels deep
        ^ but baseline starts with the quote
            <-- so only one of three indents here will be in the resulting value
    negative indent is ignored, and will be treated as it's start of line"
}
```
- Spaces and tabs are both treated as a single unit of indentation
- Unclosed strings return an error [MONK_ERR_UNCLOSED_STRING](src/reader.h#L24)

## API

### Config commons: see [common.h](src/common.h)
### Config reader: see [reader.h](src/reader.h)
### Config writer: see [writer.h](src/writer.h)

### Compiling

Library is built against C99 and requires C standard libraries `stdlib.h`, `string.h`, `stdio.h`, `stddef.h`.
No other specific features or flags are needed, it compiles even with [TCC](https://bellard.org/tcc/).

Reader and Writer APIs can be used independently, however both require Common API.
If you don't plan on building and exporting configs at runtime and just want to read configs, include these files in your project:
```
src/common.c
src/common.h
src/reader.c
src/reader.h
```
