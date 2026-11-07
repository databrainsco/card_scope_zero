#include "../cardscope_zero.h"
#include "../modules/card_detector.h"

void cardscope_zero_scene_scan_on_enter(void* context) {
    CardScopeZeroApp* app = context;
    cardscope_detector_start(app);
}

bool cardscope_zero_scene_scan_on_event(void* context, SceneManagerEvent event) {
    CardScopeZeroApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = cardscope_detector_on_custom_event(app, event.event);
    } else if(event.type == SceneManagerEventTypeBack) {
        cardscope_detector_on_custom_event(app, CardScopeCustomEventScanCancel);
        consumed = false;
    }

    return consumed;
}

void cardscope_zero_scene_scan_on_exit(void* context) {
    CardScopeZeroApp* app = context;
    cardscope_detector_stop_all(app);
    cardscope_scan_view_stop_animation(app->scan_view);
}
