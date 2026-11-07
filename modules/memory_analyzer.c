#include "memory_analyzer.h"

void cardscope_memory_analyzer_format(const CardScopeResult* result, FuriString* out) {
    furi_assert(result);
    furi_assert(out);

    furi_string_printf(
        out,
        "Memory Overview\n"
        "---------------\n"
        "Family: %s\n"
        "Type: %s\n"
        "Units total: %u\n"
        "Units read: %u\n"
        "Partial data: %s\n"
        "Summary: %s\n"
        "\n"
        "Unread or protected regions are reported as unknown.\n"
        "No authentication bypass is attempted.\n",
        result->family_name,
        result->type_name,
        result->pages_or_blocks_total,
        result->pages_or_blocks_read,
        result->data_partial ? "yes" : "no",
        result->memory_info[0] ? result->memory_info : "N/A");
}
