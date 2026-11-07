#pragma once

#include "card_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void cardscope_compatibility_format_matrix(FuriString* out);
void cardscope_compatibility_format_for_result(const CardScopeResult* result, FuriString* out);

#ifdef __cplusplus
}
#endif
