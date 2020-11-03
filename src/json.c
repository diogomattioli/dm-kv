#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "pointer.h"
#include "atomic.h"
#include "avl_tree.h"
#include "hash.h"
#include "json.h"

static void *deserialize_value(char **json);

static char *deserialize_string(char **json)
{
    char *ptr = *json;

    char *c = ptr;
    if (*c != '"' && *c != '\'')
        return NULL;

    char *value = NULL;

    while (*++ptr != '\0')
    {
        if (*ptr == *c)
        {
            size_t len = ptr - c;
            value = ptr_malloc(len, PTR_STRING, NULL);
            snprintf(value, len, "%s", c + 1);
            break;
        }
    }

    *json = ptr;

    return value;
}

static void *deserialize_bool_or_null(char **json)
{
    char *ptr = *json;

    void *value = NULL;

    if (strncasecmp(ptr, "null", 4) == 0)
    {
        value = ptr_malloc(0, PTR_NULL, NULL);
        ptr += 3;
    }
    else if (strncasecmp(ptr, "true", 4) == 0)
    {
        value = ptr_malloc(0, PTR_TRUE, NULL);
        ptr += 3;
    }
    else if (strncasecmp(ptr, "false", 5) == 0)
    {
        value = ptr_malloc(0, PTR_FALSE, NULL);
        ptr += 4;
    }

    *json = ptr;

    return value;
}

static void *deserialize_number(char **json)
{
    char *ptr = *json;
    if (*ptr == '\0')
        return NULL;

    while (isspace(*ptr) && *ptr++ != '\0');

    char *c = ptr;
    if (*ptr == '\0')
        return NULL;

    char *value = NULL;

    while ((isdigit(*ptr) || *ptr == '.' || *ptr == '-' || *ptr == '+' || *ptr == 'e' || *ptr == 'E') && *ptr++ != '\0');

//    if (*ptr != '\0')
    if (ptr > c)
    {
        size_t len = ptr - c + 1;
        value = ptr_malloc(len, PTR_NUMBER, NULL);
        snprintf(value, len, "%s", c);
    }

    *json = --ptr;

    return value;
}

static avl_tree_t *deserialize_array(char **json)
{
    char *ptr = *json;

    if (*ptr != '[')
        return NULL;

    avl_tree_t *tree = avl_create(atomic_create());

    bool error = false;

    void *value = NULL;

    uint32_t index = 0;

    while (*++ptr != '\0')
    {
        if (isspace(*ptr))
            continue;

        if (*ptr == ']')
            break;

        if (value == NULL)
        {
            value = deserialize_value(&ptr);

            if (value == NULL)
            {
                error = true;
                break;
            }
            else
            {
                char str_index[11];
                snprintf(str_index, sizeof(str_index), "%i", index);
                uint32_t id = hash_multiplication(str_index);
                avl_insert(tree, id, value);
                index++;
            }
        }
        else
        {
            if (*ptr == ',')
                value = NULL;
            else
            {
                error = true;
                break;
            }
        }
    }

    *json = ptr;

    if (error)
    {
        avl_destroy(tree);
        return NULL;
    }

    return tree;
}

static avl_tree_t *deserialize_object(char **json)
{
    char *ptr = *json;

    if (*ptr != '{')
        return NULL;

    avl_tree_t *tree = avl_create(atomic_create());

    bool error = false;

    void *member = NULL;
    void *value = NULL;

    while (*++ptr != '\0')
    {
        if (isspace(*ptr))
            continue;

        if ((member == NULL || value != NULL) && *ptr == '}')
            break;

        if (member == NULL)
        {
            if (*ptr == '"' || *ptr == '\'')
                member = deserialize_string(&ptr);

            if (member == NULL)
            {
                error = true;
                break;
            }
        }
        else if (value == NULL)
        {
            if (*ptr == ':')
            {
                ptr++;
                value = deserialize_value(&ptr);
            }

            if (value == NULL)
            {
                error = true;
                break;
            }
            else
            {
                uint32_t id = hash_multiplication(member);
                avl_insert(tree, id, value);
                ptr_free(member);
            }
        }
        else
        {
            if (*ptr == ',')
            {
                member = NULL;
                value = NULL;
            }
            else
            {
                error = true;
                break;
            }
        }
    }

    *json = ptr;

    if (error)
    {
        avl_destroy(tree);
        return NULL;
    }

    return tree;
}

static void *deserialize_value(char **json)
{
    char *ptr = *json;

    if (*ptr == '\0')
        return NULL;

    void *value = NULL;

    while (isspace(*ptr) && *ptr++ != '\0');

    switch (*ptr)
    {
        case '\0':
            break;
        case 'T':
        case 't':
        case 'F':
        case 'f':
        case 'N':
        case 'n':
            value = deserialize_bool_or_null(&ptr);
            break;
        case '"':
        case '\'':
            value = deserialize_string(&ptr);
            break;
        case '{':
            value = deserialize_object(&ptr);
            break;
        case '[':
            value = deserialize_array(&ptr);
            break;
        default:
            value = deserialize_number(&ptr);
            break;
    }

    *json = ptr;

    return value;
}

void *json_deserialize(char **json)
{
    char *ptr = *json;

    if (*ptr == '\0')
        return NULL;

    void *value = deserialize_value(&ptr);

    if (*++ptr != '\0')
    {
        if (ptr_type(value) == PTR_TREE)
            avl_destroy(value);
        else
            ptr_free(value);
        value = NULL;
    }

    return value;
}
