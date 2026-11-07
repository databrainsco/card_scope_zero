#include "../cardscope_zero.h"
#include "../modules/history_manager.h"

static void cardscope_zero_scene_history_submenu_callback(void* context, uint32_t index) {
    CardScopeZeroApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void cardscope_zero_scene_history_on_enter(void* context) {
    CardScopeZeroApp* app = context;
    Submenu* submenu = app->submenu;

    cardscope_history_load(app);
    submenu_reset(submenu);
    submenu_set_header(submenu, "Scan History");

    if(app->history.count == 0) {
        submenu_add_item(
            submenu,
            "No entries",
            (uint32_t)(CARDSCOPE_HISTORY_MAX + 1),
            cardscope_zero_scene_history_submenu_callback,
            app);
    } else {
        for(size_t i = 0; i < app->history.count; i++) {
            CardScopeHistoryEntry* entry = &app->history.entries[i];
            furi_string_printf(
                app->temp_str,
                "%02u/%02u %s",
                entry->datetime.day,
                entry->datetime.month,
                entry->protocol_name);
            submenu_add_item(
                submenu,
                furi_string_get_cstr(app->temp_str),
                (uint32_t)i,
                cardscope_zero_scene_history_submenu_callback,
                app);
        }
    }

    submenu_add_item(
        submenu,
        "Clear History",
        (uint32_t)CARDSCOPE_HISTORY_MAX,
        cardscope_zero_scene_history_submenu_callback,
        app);

    view_dispatcher_switch_to_view(app->view_dispatcher, CardScopeViewSubmenu);
}

bool cardscope_zero_scene_history_on_event(void* context, SceneManagerEvent event) {
    CardScopeZeroApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        if(event.event == CARDSCOPE_HISTORY_MAX) {
            cardscope_history_clear(app);
            cardscope_zero_scene_history_on_enter(app);
        } else if(event.event < app->history.count) {
            scene_manager_set_scene_state(
                app->scene_manager, CardScopeZeroSceneHistoryDetail, event.event);
            scene_manager_next_scene(app->scene_manager, CardScopeZeroSceneHistoryDetail);
        }
    }

    return consumed;
}

void cardscope_zero_scene_history_on_exit(void* context) {
    CardScopeZeroApp* app = context;
    submenu_reset(app->submenu);
}
