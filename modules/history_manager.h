#pragma once

#include "../cardscope_zero.h"

#ifdef __cplusplus
extern "C" {
#endif

bool cardscope_history_load(CardScopeZeroApp* app);
bool cardscope_history_save(CardScopeZeroApp* app);
bool cardscope_history_add(CardScopeZeroApp* app, const CardScopeResult* result);
bool cardscope_history_delete(CardScopeZeroApp* app, size_t index);
void cardscope_history_clear(CardScopeZeroApp* app);

#ifdef __cplusplus
}
#endif
