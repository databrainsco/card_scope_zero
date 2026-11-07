#include "../cardscope_zero.h"
#include "../modules/history_manager.h"

typedef enum {
    CardScopeHistoryDetailDelete = 0,
} CardScopeHistoryDetailEvent;

static void
    cardscope_zero_scene_history_detail_callback(GuiButtonType result, InputType type, void* context) {
    CardScopeZeroApp* app = context;
    if(type != InputTypeShort) return;
    if(result == GuiButtonTypeLeft) {
        view_dispatcher_send_custom_event(
            app->view_dispatcher, CardScopeHistoryDetailDelete);
    }
}

void cardscope_zero_scene_history_detail_on_enter(void* context) {
    CardScopeZeroApp* app = context;
    size_t index =
        scene_manager_get_scene_state(app->scene_manager, CardScopeZeroSceneHistoryDetail);

    widget_reset(app->widget);

    if(index >= app->history.count) {
        widget_add_string_element(
            app->widget, 64, 24, AlignCenter, AlignCenter, FontPrimary, "Missing entry");
    } else {
        CardScopeHistoryEntry* entry = &app->history.entries[index];
        furi_string_printf(
            app->temp_str,
            "%04u-%02u-%02u %02u:%02u:%02u\n"
            "Tech: %s\n"
            "Proto: %s\n"
            "Family: %s\n"
            "UID: %s\n"
            "Status: %s",
            entry->datetime.year,
            entry->datetime.month,
            entry->datetime.day,
            entry->datetime.hour,
            entry->datetime.minute,
            entry->datetime.second,
            cardscope_technology_name(entry->technology),
            entry->protocol_name,
            entry->family_name,
            entry->render_uid,
            cardscope_status_name(entry->status));
        widget_add_text_scroll_element(
            app->widget, 0, 0, 128, 40, furi_string_get_cstr(app->temp_str));
        widget_add_button_element(
            app->widget,
            GuiButtonTypeLeft,
            "Delete",
            cardscope_zero_scene_history_detail_callback,
            app);
    }

    view_dispatcher_switch_to_view(app->view_dispatcher, CardScopeViewWidget);
}

bool cardscope_zero_scene_history_detail_on_event(void* context, SceneManagerEvent event) {
    CardScopeZeroApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == CardScopeHistoryDetailDelete) {
            size_t index = scene_manager_get_scene_state(
                app->scene_manager, CardScopeZeroSceneHistoryDetail);
            cardscope_history_delete(app, index);
            consumed = scene_manager_previous_scene(app->scene_manager);
        }
    }

    return consumed;
}

void cardscope_zero_scene_history_detail_on_exit(void* context) {
    CardScopeZeroApp* app = context;
    widget_reset(app->widget);
}
