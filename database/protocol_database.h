#pragma once

#include "../modules/card_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char* name;
    const char* family;
    const char* frequency;
    CardScopeTechnology technology;
    CardScopeCapabilityMatrix capabilities;
    const char* notes;
} CardScopeProtocolInfo;

size_t cardscope_protocol_database_count(void);
const CardScopeProtocolInfo* cardscope_protocol_database_get(size_t index);
const CardScopeProtocolInfo* cardscope_protocol_database_find_nfc(NfcProtocol protocol);
const CardScopeProtocolInfo* cardscope_protocol_database_find_rfid(LFRFIDProtocol protocol);
void cardscope_apply_capabilities(CardScopeResult* result);

#ifdef __cplusplus
}
#endif
