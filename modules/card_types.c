#include "card_types.h"
#include <string.h>

void cardscope_result_clear(CardScopeResult* result) {
    furi_assert(result);
    memset(result, 0, sizeof(CardScopeResult));
    result->nfc_protocol = NfcProtocolInvalid;
    result->rfid_protocol = LFRFIDProtocolMax;
}

const char* cardscope_technology_name(CardScopeTechnology tech) {
    switch(tech) {
    case CardScopeTechNfc:
        return "NFC";
    case CardScopeTechRfid125:
        return "RFID 125 kHz";
    case CardScopeTechNone:
    default:
        return "None";
    }
}

const char* cardscope_status_name(CardScopeStatus status) {
    switch(status) {
    case CardScopeStatusWaiting:
        return "WAITING";
    case CardScopeStatusIdentified:
        return "IDENTIFIED";
    case CardScopeStatusPartial:
        return "PARTIAL";
    case CardScopeStatusUnknown:
        return "UNKNOWN";
    case CardScopeStatusError:
        return "ERROR";
    case CardScopeStatusCancelled:
        return "CANCELLED";
    case CardScopeStatusNone:
    default:
        return "NONE";
    }
}

const char* cardscope_capability_name(CardScopeCapability cap) {
    switch(cap) {
    case CardScopeCapabilitySupported:
        return "SUPPORTED";
    case CardScopeCapabilityPartial:
        return "PARTIAL";
    case CardScopeCapabilityNotSupported:
    default:
        return "NOT SUPPORTED";
    }
}
