#pragma once

#include "card_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void cardscope_security_analyzer_format(const CardScopeResult* result, FuriString* out);

#ifdef __cplusplus
}
#endif
