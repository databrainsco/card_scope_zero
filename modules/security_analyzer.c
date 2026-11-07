#include "security_analyzer.h"

void cardscope_security_analyzer_format(const CardScopeResult* result, FuriString* out) {
    furi_assert(result);
    furi_assert(out);

    furi_string_printf(
        out,
        "Security Overview\n"
        "-----------------\n"
        "Technology: %s\n"
        "Auth model: %s\n"
        "Protections: %s\n"
        "Read limits: %s\n"
        "\n"
        "Guidance:\n"
        "- Technology alone does not mean a card is vulnerable.\n"
        "- Protected sectors/files were not bypassed.\n"
        "- Treat decoded IDs as sensitive credentials.\n"
        "- Prefer modern authenticated tokens for access control.\n",
        cardscope_technology_name(result->technology),
        result->auth_info[0] ? result->auth_info : "Not determined",
        result->protection_info[0] ? result->protection_info : "Not determined",
        result->memory_info[0] ? result->memory_info : "Not determined");
}
