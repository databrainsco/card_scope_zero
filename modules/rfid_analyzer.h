#pragma once

#include "../cardscope_zero.h"

#ifdef __cplusplus
extern "C" {
#endif

void cardscope_rfid_analyzer_fill(
    CardScopeResult* result,
    ProtocolDict* dict,
    ProtocolId protocol);
void cardscope_rfid_analyzer_format_details(const CardScopeResult* result, FuriString* out);

#ifdef __cplusplus
}
#endif
