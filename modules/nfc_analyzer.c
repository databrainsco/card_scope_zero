#include "nfc_analyzer.h"
#include "security_analyzer.h"
#include "memory_analyzer.h"
#include "../database/protocol_database.h"

#include <string.h>

#include <nfc/protocols/iso14443_3a/iso14443_3a.h>
#include <nfc/protocols/mf_ultralight/mf_ultralight.h>
#include <nfc/protocols/mf_classic/mf_classic.h>
#include <nfc/protocols/mf_desfire/mf_desfire.h>
#include <nfc/protocols/mf_plus/mf_plus.h>
#include <nfc/protocols/felica/felica.h>

static const char* cardscope_mf_ul_type_name(MfUltralightType type) {
    switch(type) {
    case MfUltralightTypeOrigin:
        return "Ultralight Origin";
    case MfUltralightTypeNTAG203:
        return "NTAG203";
    case MfUltralightTypeMfulC:
        return "Ultralight C";
    case MfUltralightTypeUL11:
        return "Ultralight EV1 640";
    case MfUltralightTypeUL21:
        return "Ultralight EV1 1312";
    case MfUltralightTypeNTAG213:
        return "NTAG213";
    case MfUltralightTypeNTAG215:
        return "NTAG215";
    case MfUltralightTypeNTAG216:
        return "NTAG216";
    case MfUltralightTypeNTAGI2C1K:
        return "NTAG I2C 1K";
    case MfUltralightTypeNTAGI2C2K:
        return "NTAG I2C 2K";
    case MfUltralightTypeNTAGI2CPlus1K:
        return "NTAG I2C Plus 1K";
    case MfUltralightTypeNTAGI2CPlus2K:
        return "NTAG I2C Plus 2K";
    default:
        return "Ultralight/NTAG";
    }
}

static const char* cardscope_mf_classic_type_name(MfClassicType type) {
    switch(type) {
    case MfClassicTypeMini:
        return "Classic Mini";
    case MfClassicType1k:
        return "Classic 1K";
    case MfClassicType4k:
        return "Classic 4K";
    default:
        return "Classic";
    }
}

static void cardscope_format_uid(CardScopeResult* result, const uint8_t* uid, size_t uid_len) {
    if(!uid || uid_len == 0 || uid_len > CARDSCOPE_UID_MAX_LEN) {
        strlcpy(result->render_uid, "N/A", sizeof(result->render_uid));
        result->uid_len = 0;
        return;
    }

    memcpy(result->uid, uid, uid_len);
    result->uid_len = uid_len;

    FuriString* tmp = furi_string_alloc();
    for(size_t i = 0; i < uid_len; i++) {
        if(i > 0) furi_string_cat_str(tmp, " ");
        furi_string_cat_printf(tmp, "%02X", uid[i]);
    }
    strlcpy(result->render_uid, furi_string_get_cstr(tmp), sizeof(result->render_uid));
    furi_string_free(tmp);
}

static void cardscope_fill_ultralight(CardScopeResult* result, const MfUltralightData* data) {
    strlcpy(result->family_name, "MIFARE Ultralight", sizeof(result->family_name));
    strlcpy(
        result->type_name, cardscope_mf_ul_type_name(data->type), sizeof(result->type_name));
    result->pages_or_blocks_total = mf_ultralight_get_pages_total(data->type);
    result->pages_or_blocks_read = data->pages_read;

    uint32_t features = mf_ultralight_get_feature_support_set(data->type);
    if(features & MfUltralightFeatureSupportPasswordAuth) {
        result->auth_required = true;
        strlcpy(
            result->auth_info,
            "Password AUTH supported by card family",
            sizeof(result->auth_info));
    } else if(features & MfUltralightFeatureSupportAuthenticate) {
        result->auth_required = true;
        strlcpy(
            result->auth_info,
            "3DES AUTH (Ultralight C) may protect pages",
            sizeof(result->auth_info));
    } else {
        strlcpy(result->auth_info, "No password auth feature reported", sizeof(result->auth_info));
    }

    if(data->pages_read < result->pages_or_blocks_total) {
        result->data_partial = true;
        result->status = CardScopeStatusPartial;
        snprintf(
            result->memory_info,
            sizeof(result->memory_info),
            "Pages read %u/%u (some unread/protected)",
            data->pages_read,
            result->pages_or_blocks_total);
        strlcpy(
            result->protection_info,
            "Unread pages may be locked or auth-gated",
            sizeof(result->protection_info));
    } else {
        result->status = CardScopeStatusIdentified;
        snprintf(
            result->memory_info,
            sizeof(result->memory_info),
            "Pages read %u/%u",
            data->pages_read,
            result->pages_or_blocks_total);
        strlcpy(result->protection_info, "No unread pages reported", sizeof(result->protection_info));
    }
}

static void cardscope_fill_classic(CardScopeResult* result, const MfClassicData* data) {
    strlcpy(result->family_name, "MIFARE Classic", sizeof(result->family_name));
    strlcpy(
        result->type_name,
        cardscope_mf_classic_type_name(data->type),
        sizeof(result->type_name));

    uint8_t total_sectors = mf_classic_get_total_sectors_num(data->type);
    uint16_t total_blocks = mf_classic_get_total_block_num(data->type);
    result->pages_or_blocks_total = total_blocks;

    uint16_t read_blocks = 0;
    for(uint16_t i = 0; i < total_blocks; i++) {
        if(mf_classic_is_block_read(data, i)) {
            read_blocks++;
        }
    }
    result->pages_or_blocks_read = read_blocks;
    result->auth_required = true;
    strlcpy(
        result->auth_info,
        "Crypto1 Key A/B required per sector",
        sizeof(result->auth_info));

    if(read_blocks == 0) {
        result->data_partial = true;
        result->status = CardScopeStatusPartial;
        snprintf(
            result->memory_info,
            sizeof(result->memory_info),
            "Sectors %u, blocks %u (none readable without keys)",
            total_sectors,
            total_blocks);
        strlcpy(
            result->protection_info,
            "Sector access bits enforce authentication",
            sizeof(result->protection_info));
    } else if(read_blocks < total_blocks) {
        result->data_partial = true;
        result->status = CardScopeStatusPartial;
        snprintf(
            result->memory_info,
            sizeof(result->memory_info),
            "Blocks readable %u/%u",
            read_blocks,
            total_blocks);
        strlcpy(
            result->protection_info,
            "Some sectors not authenticated",
            sizeof(result->protection_info));
    } else {
        result->status = CardScopeStatusIdentified;
        snprintf(
            result->memory_info,
            sizeof(result->memory_info),
            "All %u blocks readable with available keys",
            total_blocks);
        strlcpy(
            result->protection_info,
            "Keys present for accessible sectors",
            sizeof(result->protection_info));
    }
}

static void cardscope_fill_desfire(CardScopeResult* result, const MfDesfireData* data) {
    strlcpy(result->family_name, "MIFARE DESFire", sizeof(result->family_name));
    const char* type_name = "DESFire";
    switch(data->version.hw_major) {
    default:
        /* Prefer typed enum when available via poller fill */
        break;
    }
    UNUSED(type_name);
    strlcpy(result->type_name, "DESFire (see version)", sizeof(result->type_name));
    result->auth_required = true;
    result->data_partial = true;
    result->status = CardScopeStatusPartial;
    strlcpy(
        result->auth_info,
        "AES/3DES app keys may be required for files",
        sizeof(result->auth_info));
    strlcpy(
        result->protection_info,
        "Free memory/app list only when card allows",
        sizeof(result->protection_info));
    if(data->free_memory.is_present) {
        snprintf(
            result->memory_info,
            sizeof(result->memory_info),
            "Free memory reported: %lu bytes",
            (unsigned long)data->free_memory.bytes_free);
    } else {
        strlcpy(
            result->memory_info,
            "Free memory field not present/readable",
            sizeof(result->memory_info));
    }
}

void cardscope_nfc_analyzer_fill_from_device(CardScopeResult* result, NfcDevice* device) {
    furi_assert(result);
    furi_assert(device);

    cardscope_result_clear(result);
    result->technology = CardScopeTechNfc;
    result->is_nfc = true;
    result->nfc_protocol = nfc_device_get_protocol(device);
    strlcpy(result->frequency, "13.56 MHz", sizeof(result->frequency));

    const char* proto_name = nfc_device_get_protocol_name(result->nfc_protocol);
    if(proto_name) {
        strlcpy(result->protocol_name, proto_name, sizeof(result->protocol_name));
    } else {
        strlcpy(result->protocol_name, "Unknown NFC", sizeof(result->protocol_name));
    }

    size_t uid_len = 0;
    const uint8_t* uid = nfc_device_get_uid(device, &uid_len);
    cardscope_format_uid(result, uid, uid_len);

    /* Manufacturer hint from first UID byte is not always accurate; only note ISO cascade. */
    strlcpy(result->manufacturer, "See ATQA/SAK / version", sizeof(result->manufacturer));

    if(result->nfc_protocol == NfcProtocolMfUltralight ||
       nfc_protocol_has_parent(result->nfc_protocol, NfcProtocolMfUltralight)) {
        const MfUltralightData* data =
            (const MfUltralightData*)nfc_device_get_data(device, NfcProtocolMfUltralight);
        if(data) {
            cardscope_fill_ultralight(result, data);
            if(data->iso14443_3a_data) {
                cardscope_format_uid(
                    result, data->iso14443_3a_data->uid, data->iso14443_3a_data->uid_len);
            }
        }
    } else if(
        result->nfc_protocol == NfcProtocolMfClassic ||
        nfc_protocol_has_parent(result->nfc_protocol, NfcProtocolMfClassic)) {
        const MfClassicData* data =
            (const MfClassicData*)nfc_device_get_data(device, NfcProtocolMfClassic);
        if(data) {
            cardscope_fill_classic(result, data);
            if(data->iso14443_3a_data) {
                cardscope_format_uid(
                    result, data->iso14443_3a_data->uid, data->iso14443_3a_data->uid_len);
            }
        }
    } else if(
        result->nfc_protocol == NfcProtocolMfDesfire ||
        nfc_protocol_has_parent(result->nfc_protocol, NfcProtocolMfDesfire)) {
        const MfDesfireData* data =
            (const MfDesfireData*)nfc_device_get_data(device, NfcProtocolMfDesfire);
        if(data) {
            cardscope_fill_desfire(result, data);
        }
    } else if(result->nfc_protocol == NfcProtocolFelica) {
        strlcpy(result->family_name, "FeliCa", sizeof(result->family_name));
        strlcpy(result->type_name, "FeliCa", sizeof(result->type_name));
        result->status = CardScopeStatusPartial;
        result->data_partial = true;
        strlcpy(
            result->auth_info,
            "Service keys may be required for block data",
            sizeof(result->auth_info));
        strlcpy(
            result->memory_info,
            "IDm identified; service dump may be limited",
            sizeof(result->memory_info));
    } else {
        strlcpy(result->family_name, result->protocol_name, sizeof(result->family_name));
        strlcpy(result->type_name, result->protocol_name, sizeof(result->type_name));
        result->status = CardScopeStatusIdentified;
        strlcpy(
            result->auth_info,
            "Protocol-level auth not analyzed further",
            sizeof(result->auth_info));
        strlcpy(
            result->memory_info,
            "Base protocol data captured by poller",
            sizeof(result->memory_info));
    }

    if(result->status == CardScopeStatusNone) {
        result->status = CardScopeStatusIdentified;
    }

    cardscope_apply_capabilities(result);
}

void cardscope_nfc_analyzer_format_details(const CardScopeResult* result, FuriString* out) {
    furi_string_printf(
        out,
        "Technology: %s\n"
        "Frequency: %s\n"
        "Protocol: %s\n"
        "Family: %s\n"
        "Type: %s\n"
        "UID: %s\n"
        "Manufacturer: %s\n"
        "Auth: %s\n"
        "Protection: %s\n"
        "Memory: %s\n"
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
        result->type_name,
        result->render_uid,
        result->manufacturer,
        result->auth_info,
        result->protection_info,
        result->memory_info,
        cardscope_status_name(result->status),
        cardscope_capability_name(result->capabilities.detect),
        cardscope_capability_name(result->capabilities.read),
        cardscope_capability_name(result->capabilities.identify),
        cardscope_capability_name(result->capabilities.analyze),
        cardscope_capability_name(result->capabilities.write),
        cardscope_capability_name(result->capabilities.emulate),
        result->notes);
}

void cardscope_nfc_analyzer_format_security(const CardScopeResult* result, FuriString* out) {
    cardscope_security_analyzer_format(result, out);
}

void cardscope_nfc_analyzer_format_memory(const CardScopeResult* result, FuriString* out) {
    cardscope_memory_analyzer_format(result, out);
}
