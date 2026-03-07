/**
 * Basic TAML Tests
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "taml.h"

void test_version() {
    printf("Testing version...\n");
    const char *version = taml_version();
    assert(version != NULL);
    assert(strlen(version) > 0);
    printf("  Version: %s\n", version);
    printf("  PASSED\n\n");
}

void test_create_values() {
    printf("Testing value creation...\n");
    
    taml_value_t *null_val = taml_create_null();
    assert(null_val != NULL);
    assert(null_val->type == TAML_TYPE_NULL);
    taml_free_value(null_val);
    
    taml_value_t *bool_val = taml_create_bool(true);
    assert(bool_val != NULL);
    assert(bool_val->type == TAML_TYPE_BOOL);
    assert(bool_val->data.bool_val == true);
    taml_free_value(bool_val);
    
    taml_value_t *int_val = taml_create_int(42);
    assert(int_val != NULL);
    assert(int_val->type == TAML_TYPE_INT);
    assert(int_val->data.int_val == 42);
    taml_free_value(int_val);
    
    taml_value_t *float_val = taml_create_float(3.14);
    assert(float_val != NULL);
    assert(float_val->type == TAML_TYPE_FLOAT);
    taml_free_value(float_val);
    
    taml_value_t *string_val = taml_create_string("hello");
    assert(string_val != NULL);
    assert(string_val->type == TAML_TYPE_STRING);
    assert(strcmp(string_val->data.string_val, "hello") == 0);
    taml_free_value(string_val);
    
    printf("  PASSED\n\n");
}

void test_object_operations() {
    printf("Testing object operations...\n");
    
    taml_value_t *obj = taml_create_object();
    assert(obj != NULL);
    assert(obj->type == TAML_TYPE_OBJECT);
    
    taml_value_t *val1 = taml_create_string("value1");
    taml_error_t err = taml_object_set(obj, "key1", val1);
    assert(err == TAML_OK);
    
    taml_value_t *retrieved = taml_object_get(obj, "key1");
    assert(retrieved != NULL);
    assert(retrieved->type == TAML_TYPE_STRING);
    assert(strcmp(retrieved->data.string_val, "value1") == 0);
    
    taml_free_value(obj);
    
    printf("  PASSED\n\n");
}

void test_array_operations() {
    printf("Testing array operations...\n");
    
    taml_value_t *arr = taml_create_array();
    assert(arr != NULL);
    assert(arr->type == TAML_TYPE_ARRAY);
    assert(taml_array_size(arr) == 0);
    
    taml_value_t *val1 = taml_create_string("item1");
    taml_error_t err = taml_array_append(arr, val1);
    assert(err == TAML_OK);
    assert(taml_array_size(arr) == 1);
    
    taml_value_t *val2 = taml_create_string("item2");
    err = taml_array_append(arr, val2);
    assert(err == TAML_OK);
    assert(taml_array_size(arr) == 2);
    
    taml_value_t *retrieved = taml_array_get(arr, 0);
    assert(retrieved != NULL);
    assert(retrieved->type == TAML_TYPE_STRING);
    assert(strcmp(retrieved->data.string_val, "item1") == 0);
    
    retrieved = taml_array_get(arr, 1);
    assert(retrieved != NULL);
    assert(strcmp(retrieved->data.string_val, "item2") == 0);
    
    taml_free_value(arr);
    
    printf("  PASSED\n\n");
}

void test_parse_simple() {
    printf("Testing simple parse...\n");
    
    const char *taml_text = 
        "name\tJohn\n"
        "age\t30\n"
        "active\ttrue\n";
    
    taml_document_t *doc = taml_parse(taml_text, NULL);
    assert(doc != NULL);
    assert(doc->root != NULL);
    assert(doc->root->type == TAML_TYPE_OBJECT);
    
    taml_value_t *name = taml_object_get(doc->root, "name");
    assert(name != NULL);
    assert(name->type == TAML_TYPE_STRING);
    assert(strcmp(name->data.string_val, "John") == 0);
    
    taml_value_t *age = taml_object_get(doc->root, "age");
    assert(age != NULL);
    assert(age->type == TAML_TYPE_INT);
    assert(age->data.int_val == 30);
    
    taml_value_t *active = taml_object_get(doc->root, "active");
    assert(active != NULL);
    assert(active->type == TAML_TYPE_BOOL);
    assert(active->data.bool_val == true);
    
    taml_free_document(doc);
    
    printf("  PASSED\n\n");
}

void test_parse_nested() {
    printf("Testing nested parse...\n");
    
    const char *taml_text = 
        "server\n"
        "\thost\tlocalhost\n"
        "\tport\t8080\n";
    
    taml_document_t *doc = taml_parse(taml_text, NULL);
    assert(doc != NULL);
    assert(doc->root != NULL);
    
    taml_value_t *server = taml_object_get(doc->root, "server");
    assert(server != NULL);
    assert(server->type == TAML_TYPE_OBJECT);
    
    taml_value_t *host = taml_object_get(server, "host");
    assert(host != NULL);
    assert(strcmp(host->data.string_val, "localhost") == 0);
    
    taml_value_t *port = taml_object_get(server, "port");
    assert(port != NULL);
    assert(port->data.int_val == 8080);
    
    taml_free_document(doc);
    
    printf("  PASSED\n\n");
}

void test_parse_array() {
    printf("Testing array parse...\n");
    
    const char *taml_text = 
        "items\n"
        "\titem1\n"
        "\titem2\n"
        "\titem3\n";
    
    taml_document_t *doc = taml_parse(taml_text, NULL);
    assert(doc != NULL);
    assert(doc->root != NULL);
    
    taml_value_t *items = taml_object_get(doc->root, "items");
    assert(items != NULL);
    assert(items->type == TAML_TYPE_ARRAY);
    assert(taml_array_size(items) == 3);
    
    taml_value_t *item1 = taml_array_get(items, 0);
    assert(item1 != NULL);
    assert(strcmp(item1->data.string_val, "item1") == 0);
    
    taml_free_document(doc);
    
    printf("  PASSED\n\n");
}

void test_stringify() {
    printf("Testing stringify...\n");
    
    taml_value_t *obj = taml_create_object();
    taml_object_set(obj, "name", taml_create_string("John"));
    taml_object_set(obj, "age", taml_create_int(30));
    
    taml_document_t doc;
    doc.root = obj;
    doc.error_message = NULL;
    doc.error_line = 0;
    
    char *result = taml_stringify(&doc, NULL);
    assert(result != NULL);
    assert(strlen(result) > 0);
    
    printf("  Result:\n%s\n", result);
    
    taml_free_string(result);
    taml_free_value(obj);
    
    printf("  PASSED\n\n");
}

void test_validation() {
    printf("Testing validation...\n");
    
    /* Valid TAML */
    const char *valid_taml = "key\tvalue\n";
    char error_msg[256];
    int error_line;
    
    taml_error_t err = taml_validate(valid_taml, error_msg, sizeof(error_msg), &error_line);
    assert(err == TAML_OK);
    
    /* Invalid TAML - spaces instead of tabs */
    const char *invalid_taml = "  key\tvalue\n";
    err = taml_validate(invalid_taml, error_msg, sizeof(error_msg), &error_line);
    assert(err == TAML_ERROR_INVALID_INDENTATION);
    printf("  Caught invalid indentation: %s\n", error_msg);
    
    printf("  PASSED\n\n");
}

void test_json_conversion() {
    printf("Testing JSON conversion...\n");
    
    const char *json = "{\"name\": \"John\", \"age\": 30}";
    taml_document_t *doc = taml_from_json(json);
    assert(doc != NULL);
    assert(doc->root != NULL);
    
    taml_value_t *name = taml_object_get(doc->root, "name");
    assert(name != NULL);
    assert(strcmp(name->data.string_val, "John") == 0);
    
    char *json_out = taml_to_json(doc);
    assert(json_out != NULL);
    printf("  JSON output:\n%s\n", json_out);
    
    taml_free_string(json_out);
    taml_free_document(doc);
    
    printf("  PASSED\n\n");
}

int main() {
    printf("=== TAML C Library Tests ===\n\n");
    
    test_version();
    test_create_values();
    test_object_operations();
    test_array_operations();
    test_parse_simple();
    test_parse_nested();
    test_parse_array();
    test_stringify();
    test_validation();
    test_json_conversion();
    
    printf("=== All Tests Passed! ===\n");
    
    return 0;
}
