#include "compatibility.h"
#include "../database/protocol_database.h"

void cardscope_compatibility_format_matrix(FuriString* out) {
    furi_assert(out);
    furi_string_reset(out);
    furi_string_cat_str(out, "Card Compatibility\nSDK 1.5.1\n\n");

    size_t count = cardscope_protocol_database_count();
    for(size_t i = 0; i < count; i++) {
        const CardScopeProtocolInfo* info = cardscope_protocol_database_get(i);
        furi_string_cat_printf(
            out,
            "%s\n"
            " D:%s R:%s I:%s\n"
            " A:%s W:%s E:%s\n\n",
            info->name,
            cardscope_capability_name(info->capabilities.detect),
            cardscope_capability_name(info->capabilities.read),
            cardscope_capability_name(info->capabilities.identify),
            cardscope_capability_name(info->capabilities.analyze),
            cardscope_capability_name(info->capabilities.write),
            cardscope_capability_name(info->capabilities.emulate));
    }
}

void cardscope_compatibility_format_for_result(const CardScopeResult* result, FuriString* out) {
    furi_assert(result);
    furi_assert(out);
    furi_string_printf(
        out,
        "Compatibility\n"
        "-------------\n"
        "Protocol: %s\n"
        "Detect: %s\n"
        "Read: %s\n"
        "Identify: %s\n"
        "Analyze: %s\n"
        "Write: %s\n"
        "Emulate: %s\n"
        "\n"
        "Detection does not imply full dump or emulation.\n",
        result->protocol_name,
        cardscope_capability_name(result->capabilities.detect),
        cardscope_capability_name(result->capabilities.read),
        cardscope_capability_name(result->capabilities.identify),
        cardscope_capability_name(result->capabilities.analyze),
        cardscope_capability_name(result->capabilities.write),
        cardscope_capability_name(result->capabilities.emulate));
}
