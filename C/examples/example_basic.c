/**
 * Basic TAML Example
 * Demonstrates parsing and serializing TAML documents
 */

#include <stdio.h>
#include <stdlib.h>
#include "taml.h"

void example_parse() {
    printf("=== Parsing TAML ===\n\n");
    
    const char *taml_text = 
        "# Application Configuration\n"
        "application\tMyApp\n"
        "version\t1.0.0\n"
        "debug\ttrue\n"
        "\n"
        "server\n"
        "\thost\tlocalhost\n"
        "\tport\t8080\n"
        "\tssl\tfalse\n"
        "\n"
        "database\n"
        "\ttype\tpostgresql\n"
        "\tconnection\n"
        "\t\thost\tdb.example.com\n"
        "\t\tport\t5432\n"
        "\n"
        "features\n"
        "\tauthentication\n"
        "\tapi-gateway\n"
        "\tlogging\n";
    
    printf("Input TAML:\n%s\n", taml_text);
    
    taml_document_t *doc = taml_parse(taml_text, NULL);
    if (!doc) {
        printf("Failed to parse TAML\n");
        return;
    }
    
    if (doc->error_message) {
        printf("Parse error: %s (line %d)\n", doc->error_message, doc->error_line);
        taml_free_document(doc);
        return;
    }
    
    printf("\n=== Accessing Values ===\n\n");
    
    /* Access simple values */
    taml_value_t *app = taml_object_get(doc->root, "application");
    if (app && app->type == TAML_TYPE_STRING) {
        printf("Application: %s\n", app->data.string_val);
    }
    
    taml_value_t *version = taml_object_get(doc->root, "version");
    if (version && version->type == TAML_TYPE_STRING) {
        printf("Version: %s\n", version->data.string_val);
    }
    
    taml_value_t *debug = taml_object_get(doc->root, "debug");
    if (debug && debug->type == TAML_TYPE_BOOL) {
        printf("Debug: %s\n", debug->data.bool_val ? "true" : "false");
    }
    
    /* Access nested object */
    taml_value_t *server = taml_object_get(doc->root, "server");
    if (server && server->type == TAML_TYPE_OBJECT) {
        printf("\nServer Configuration:\n");
        
        taml_value_t *host = taml_object_get(server, "host");
        if (host && host->type == TAML_TYPE_STRING) {
            printf("  Host: %s\n", host->data.string_val);
        }
        
        taml_value_t *port = taml_object_get(server, "port");
        if (port && port->type == TAML_TYPE_INT) {
            printf("  Port: %lld\n", port->data.int_val);
        }
    }
    
    /* Access array */
    taml_value_t *features = taml_object_get(doc->root, "features");
    if (features && features->type == TAML_TYPE_ARRAY) {
        printf("\nFeatures:\n");
        size_t count = taml_array_size(features);
        for (size_t i = 0; i < count; i++) {
            taml_value_t *item = taml_array_get(features, i);
            if (item && item->type == TAML_TYPE_STRING) {
                printf("  - %s\n", item->data.string_val);
            }
        }
    }
    
    taml_free_document(doc);
}

void example_create_and_serialize() {
    printf("\n\n=== Creating and Serializing TAML ===\n\n");
    
    /* Create a document programmatically */
    taml_value_t *root = taml_create_object();
    
    /* Add simple values */
    taml_object_set(root, "name", taml_create_string("Test App"));
    taml_object_set(root, "version", taml_create_string("2.0.0"));
    taml_object_set(root, "enabled", taml_create_bool(true));
    taml_object_set(root, "count", taml_create_int(100));
    
    /* Add nested object */
    taml_value_t *config = taml_create_object();
    taml_object_set(config, "timeout", taml_create_int(30));
    taml_object_set(config, "retries", taml_create_int(3));
    taml_object_set(root, "config", config);
    
    /* Add array */
    taml_value_t *tags = taml_create_array();
    taml_array_append(tags, taml_create_string("production"));
    taml_array_append(tags, taml_create_string("web"));
    taml_array_append(tags, taml_create_string("api"));
    taml_object_set(root, "tags", tags);
    
    /* Create document */
    taml_document_t doc;
    doc.root = root;
    doc.error_message = NULL;
    doc.error_line = 0;
    
    /* Serialize to TAML */
    char *taml_str = taml_stringify(&doc, NULL);
    if (taml_str) {
        printf("Generated TAML:\n%s\n", taml_str);
        taml_free_string(taml_str);
    }
    
    taml_free_value(root);
}

void example_validation() {
    printf("\n=== Validating TAML ===\n\n");
    
    /* Valid TAML */
    const char *valid_taml = "key\tvalue\nother\t123\n";
    char error_msg[256];
    int error_line;
    
    printf("Validating: %s\n", valid_taml);
    taml_error_t err = taml_validate(valid_taml, error_msg, sizeof(error_msg), &error_line);
    
    if (err == TAML_OK) {
        printf("✓ Valid TAML\n\n");
    } else {
        printf("✗ Invalid: %s (line %d)\n\n", error_msg, error_line);
    }
    
    /* Invalid TAML - spaces instead of tabs */
    const char *invalid_taml = "  key\tvalue\n";
    printf("Validating: (string with leading spaces)\n");
    err = taml_validate(invalid_taml, error_msg, sizeof(error_msg), &error_line);
    
    if (err == TAML_OK) {
        printf("✓ Valid TAML\n");
    } else {
        printf("✗ Invalid: %s (line %d)\n", error_msg, error_line);
    }
}

void example_json_conversion() {
    printf("\n\n=== JSON to TAML Conversion ===\n\n");
    
    const char *json = "{\"name\": \"John\", \"age\": 30, \"active\": true}";
    printf("Input JSON:\n%s\n\n", json);
    
    taml_document_t *doc = taml_from_json(json);
    if (doc) {
        char *taml_str = taml_stringify(doc, NULL);
        if (taml_str) {
            printf("Converted to TAML:\n%s\n", taml_str);
            taml_free_string(taml_str);
        }
        
        /* Convert back to JSON */
        char *json_out = taml_to_json(doc);
        if (json_out) {
            printf("Converted back to JSON:\n%s\n", json_out);
            taml_free_string(json_out);
        }
        
        taml_free_document(doc);
    }
}

int main() {
    printf("TAML C Library - Basic Examples\n");
    printf("================================\n\n");
    printf("Library Version: %s\n\n", taml_version());
    
    example_parse();
    example_create_and_serialize();
    example_validation();
    example_json_conversion();
    
    printf("\n=== Examples Complete ===\n");
    
    return 0;
}
