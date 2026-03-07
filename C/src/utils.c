/**
 * TAML Utility Functions
 */

#include "../include/taml.h"
#include <stdio.h>

/* Get error message string */
const char* taml_error_string(taml_error_t error) {
    switch (error) {
        case TAML_OK:
            return "Success";
        case TAML_ERROR_NULL_INPUT:
            return "Null input provided";
        case TAML_ERROR_MEMORY:
            return "Memory allocation failed";
        case TAML_ERROR_INVALID_INDENTATION:
            return "Invalid indentation (spaces used instead of tabs)";
        case TAML_ERROR_INVALID_TAB_IN_KEY:
            return "Invalid tab character in key";
        case TAML_ERROR_INVALID_TAB_IN_VALUE:
            return "Invalid tab character in value";
        case TAML_ERROR_INCONSISTENT_INDENT:
            return "Inconsistent indentation level";
        case TAML_ERROR_ORPHANED_LINE:
            return "Indented line has no parent";
        case TAML_ERROR_PARENT_WITH_VALUE:
            return "Parent key cannot have value on same line";
        case TAML_ERROR_EMPTY_KEY:
            return "Empty key";
        case TAML_ERROR_MIXED_INDENT:
            return "Mixed spaces and tabs in indentation";
        case TAML_ERROR_PARSE_FAILED:
            return "Parse failed";
        default:
            return "Unknown error";
    }
}

/* Get library version */
const char* taml_version(void) {
    static char version[32];
    snprintf(version, sizeof(version), "%d.%d.%d",
            TAML_VERSION_MAJOR, TAML_VERSION_MINOR, TAML_VERSION_PATCH);
    return version;
}
