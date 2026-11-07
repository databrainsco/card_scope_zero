#include "rfid_analyzer.h"
#include "../database/protocol_database.h"

void cardscope_rfid_analyzer_fill(
    CardScopeResult* result,
    ProtocolDict* dict,
    ProtocolId protocol) {
    furi_assert(result);
    furi_assert(dict);

    cardscope_result_clear(result);
    result->technology = CardScopeTechRfid125;
    result->is_nfc = false;
    result->rfid_protocol = (LFRFIDProtocol)protocol;
    strlcpy(result->frequency, "125 kHz", sizeof(result->frequency));

    const char* name = protocol_dict_get_name(dict, protocol);
    const char* manufacturer = protocol_dict_get_manufacturer(dict, protocol);

    if(name) {
        strlcpy(result->protocol_name, name, sizeof(result->protocol_name));
        strlcpy(result->type_name, name, sizeof(result->type_name));
    } else {
        strlcpy(result->protocol_name, "Unknown LF", sizeof(result->protocol_name));
        strlcpy(result->type_name, "Unknown", sizeof(result->type_name));
    }

    if(manufacturer) {
        strlcpy(result->manufacturer, manufacturer, sizeof(result->manufacturer));
    } else {
        strlcpy(result->manufacturer, "Unknown", sizeof(result->manufacturer));
    }

    FuriString* uid_str = furi_string_alloc();
    FuriString* data_str = furi_string_alloc();
    protocol_dict_render_uid(dict, uid_str, protocol);
    protocol_dict_render_data(dict, data_str, protocol);

    strlcpy(result->render_uid, furi_string_get_cstr(uid_str), sizeof(result->render_uid));
    strlcpy(result->render_data, furi_string_get_cstr(data_str), sizeof(result->render_data));

    size_t data_size = protocol_dict_get_data_size(dict, protocol);
    result->pages_or_blocks_total = (uint16_t)data_size;
    result->pages_or_blocks_read = (uint16_t)data_size;
    result->status = CardScopeStatusIdentified;
    result->auth_required = false;

    strlcpy(result->auth_info, "LF RFID: no challenge-response on read path", sizeof(result->auth_info));
    strlcpy(
        result->protection_info,
        "Credential formats vary; treat as raw decoded ID",
        sizeof(result->protection_info));
    snprintf(
        result->memory_info,
        sizeof(result->memory_info),
        "Decoded data size: %u bytes",
        (unsigned)data_size);

    furi_string_free(uid_str);
    furi_string_free(data_str);

    cardscope_apply_capabilities(result);
    if(result->family_name[0] == '\0') {
        strlcpy(result->family_name, result->protocol_name, sizeof(result->family_name));
    }
}

void cardscope_rfid_analyzer_format_details(const CardScopeResult* result, FuriString* out) {
    furi_string_printf(
        out,
        "Technology: %s\n"
        "Frequency: %s\n"
        "Protocol: %s\n"
        "Family: %s\n"
        "Manufacturer: %s\n"
        "UID/ID: %s\n"
        "Data: %s\n"
        "Status: %s\n"
        "Detect: %s\n"
        "Read: %s\n"
        "Identify: %s\n"
        "Analyze: %s\n"
        "Write: %s\n"
        "Emulate: %s\n"
        "Notes: %s\n",
        cardscope_technology_name(result->technology),
        result->frequency,
        result->protocol_name,
        result->family_name,
        result->manufacturer,
        result->render_uid,
        result->render_data,
        cardscope_status_name(result->status),
        cardscope_capability_name(result->capabilities.detect),
        cardscope_capability_name(result->capabilities.read),
        cardscope_capability_name(result->capabilities.identify),
        cardscope_capability_name(result->capabilities.analyze),
        cardscope_capability_name(result->capabilities.write),
        cardscope_capability_name(result->capabilities.emulate),
        result->notes);
}
