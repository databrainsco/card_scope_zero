#include "../cardscope_zero.h"
#include "../modules/history_manager.h"

typedef enum {
    CardScopeResultEventDetails,
    CardScopeResultEventSave,
    CardScopeResultEventRescan,
} CardScopeResultEvent;

static void cardscope_zero_scene_result_callback(GuiButtonType result, InputType type, void* context) {
    CardScopeZeroApp* app = context;
    if(type != InputTypeShort) return;

    if(result == GuiButtonTypeLeft) {
        view_dispatcher_send_custom_event(app->view_dispatcher, CardScopeResultEventDetails);
    } else if(result == GuiButtonTypeRight) {
        view_dispatcher_send_custom_event(app->view_dispatcher, CardScopeResultEventRescan);
    } else if(result == GuiButtonTypeCenter) {
        view_dispatcher_send_custom_event(app->view_dispatcher, CardScopeResultEventSave);
    }
}

void cardscope_zero_scene_result_on_enter(void* context) {
    CardScopeZeroApp* app = context;
    Widget* widget = app->widget;
    CardScopeResult* result = &app->result;

    widget_reset(widget);

    if(result->status == CardScopeStatusUnknown && result->technology == CardScopeTechNone) {
        widget_add_string_element(
            widget, 64, 0, AlignCenter, AlignTop, FontPrimary, "No Card Detected");
        widget_add_text_scroll_element(
            widget,
            0,
            14,
            128,
            36,
            "No NFC or 125 kHz response\nin the scan window.\n\nTry again closer to the\ncorrect antenna.");
        widget_add_button_element(
            widget, GuiButtonTypeRight, "Retry", cardscope_zero_scene_result_callback, app);
    } else if(result->status == CardScopeStatusCancelled) {
        widget_add_string_element(
            widget, 64, 20, AlignCenter, AlignCenter, FontPrimary, "Scan Cancelled");
        widget_add_button_element(
            widget, GuiButtonTypeRight, "Retry", cardscope_zero_scene_result_callback, app);
    } else {
        furi_string_printf(
            app->temp_str,
            "CARD DETECTED\n"
            "Tech: %s\n"
            "Proto: %s\n"
            "Freq: %s\n"
            "UID: %s\n"
            "Status: %s",
            cardscope_technology_name(result->technology),
            result->protocol_name,
            result->frequency,
            result->render_uid[0] ? result->render_uid : "N/A",
            cardscope_status_name(result->status));

        widget_add_text_scroll_element(
            widget, 0, 0, 128, 40, furi_string_get_cstr(app->temp_str));
        widget_add_button_element(
            widget, GuiButtonTypeLeft, "Details", cardscope_zero_scene_result_callback, app);
        widget_add_button_element(
            widget, GuiButtonTypeCenter, "Save", cardscope_zero_scene_result_callback, app);
        widget_add_button_element(
            widget, GuiButtonTypeRight, "Rescan", cardscope_zero_scene_result_callback, app);
    }

    view_dispatcher_switch_to_view(app->view_dispatcher, CardScopeViewWidget);
}

bool cardscope_zero_scene_result_on_event(void* context, SceneManagerEvent event) {
    CardScopeZeroApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        switch(event.event) {
        case CardScopeResultEventDetails:
            scene_manager_next_scene(app->scene_manager, CardScopeZeroSceneDetails);
            break;
        case CardScopeResultEventSave:
            if(cardscope_history_add(app, &app->result)) {
                notification_message(app->notifications, &sequence_success);
            } else {
                notification_message(app->notifications, &sequence_semi_success);
            }
            break;
        case CardScopeResultEventRescan:
            scene_manager_next_scene(app->scene_manager, CardScopeZeroSceneScan);
            break;
        default:
            break;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = scene_manager_search_and_switch_to_previous_scene(
            app->scene_manager, CardScopeZeroSceneMainMenu);
    }

    return consumed;
}

void cardscope_zero_scene_result_on_exit(void* context) {
    CardScopeZeroApp* app = context;
    widget_reset(app->widget);
}
