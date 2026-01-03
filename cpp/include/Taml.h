#pragma once

#include "Taml/Document.h"
#include "Taml/Serializer.h"
#include "Taml/Exception.h"
#include "Taml/Validator.h"
#include "Taml/Converter.h"

// Version information provided as macros and a helper function
#define TAML_VERSION_MAJOR 1
#define TAML_VERSION_MINOR 0
#define TAML_VERSION_PATCH 0
#define TAML_VERSION "1.0.0"

namespace Taml
{
    inline constexpr const char *version() { return TAML_VERSION; }
}
