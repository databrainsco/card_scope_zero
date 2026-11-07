#pragma once

#include <gui/view.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CardScopeScanView CardScopeScanView;

CardScopeScanView* cardscope_scan_view_alloc(void);
void cardscope_scan_view_free(CardScopeScanView* instance);
View* cardscope_scan_view_get_view(CardScopeScanView* instance);

void cardscope_scan_view_set_status(CardScopeScanView* instance, const char* status);
void cardscope_scan_view_start_animation(CardScopeScanView* instance);
void cardscope_scan_view_stop_animation(CardScopeScanView* instance);

#ifdef __cplusplus
}
#endif
