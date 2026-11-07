#pragma once

#include "../cardscope_zero.h"

#ifdef __cplusplus
extern "C" {
#endif

void cardscope_detector_stop_all(CardScopeZeroApp* app);
void cardscope_detector_start(CardScopeZeroApp* app);
bool cardscope_detector_on_custom_event(CardScopeZeroApp* app, uint32_t event);
void cardscope_detector_on_timer(void* context);

#ifdef __cplusplus
}
#endif
