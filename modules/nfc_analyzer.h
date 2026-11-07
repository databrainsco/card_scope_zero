#pragma once

#include "../cardscope_zero.h"

#ifdef __cplusplus
extern "C" {
#endif

void cardscope_nfc_analyzer_fill_from_device(CardScopeResult* result, NfcDevice* device);
void cardscope_nfc_analyzer_format_details(const CardScopeResult* result, FuriString* out);
void cardscope_nfc_analyzer_format_security(const CardScopeResult* result, FuriString* out);
void cardscope_nfc_analyzer_format_memory(const CardScopeResult* result, FuriString* out);

#ifdef __cplusplus
}
#endif
