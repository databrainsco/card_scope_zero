#include "cardscope_zero.h"
#include "modules/card_detector.h"
#include "modules/history_manager.h"

#include <string.h>

bool cardscope_zero_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    CardScopeZeroApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

bool cardscope_zero_back_event_callback(void* context) {
    furi_assert(context);
    CardScopeZeroApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

void cardscope_zero_blink_start(CardScopeZeroApp* app) {
    notification_message(app->notifications, &sequence_blink_start_cyan);
}

void cardscope_zero_blink_stop(CardScopeZeroApp* app) {
    notification_message(app->notifications, &sequence_blink_stop);
}

void cardscope_zero_notify_success(CardScopeZeroApp* app) {
    notification_message(app->notifications, &sequence_success);
}

void cardscope_zero_notify_error(CardScopeZeroApp* app) {
    notification_message(app->notifications, &sequence_error);
}

CardScopeZeroApp* cardscope_zero_app_alloc(void) {
    CardScopeZeroApp* app = malloc(sizeof(CardScopeZeroApp));
    memset(app, 0, sizeof(CardScopeZeroApp));

    app->gui = furi_record_open(RECORD_GUI);
    app->notifications = furi_record_open(RECORD_NOTIFICATION);
    app->storage = furi_record_open(RECORD_STORAGE);
    app->dialogs = furi_record_open(RECORD_DIALOGS);

    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager = scene_manager_alloc(&cardscope_zero_scene_handlers, app);

    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, cardscope_zero_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, cardscope_zero_back_event_callback);

    app->submenu = submenu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, CardScopeViewSubmenu, submenu_get_view(app->submenu));

    app->popup = popup_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, CardScopeViewPopup, popup_get_view(app->popup));

    app->widget = widget_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, CardScopeViewWidget, widget_get_view(app->widget));

    app->text_box = text_box_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, CardScopeViewTextBox, text_box_get_view(app->text_box));

    app->dialog_ex = dialog_ex_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, CardScopeViewDialogEx, dialog_ex_get_view(app->dialog_ex));

    app->variable_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        CardScopeViewVariableItemList,
        variable_item_list_get_view(app->variable_item_list));

    app->scan_view = cardscope_scan_view_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher,
        CardScopeViewScan,
        cardscope_scan_view_get_view(app->scan_view));

    app->text_box_store = furi_string_alloc();
    app->temp_str = furi_string_alloc();

    app->nfc = nfc_alloc();
    app->nfc_device = nfc_device_alloc();
    app->nfc_protocol = NfcProtocolInvalid;

    app->lfrfid_dict = protocol_dict_alloc(lfrfid_protocols, LFRFIDProtocolMax);
    app->lfrfid_worker = lfrfid_worker_alloc(app->lfrfid_dict);
    app->lfrfid_protocol = PROTOCOL_NO;

    app->scan_mode = CardScopeScanModeAuto;
    app->scan_phase = CardScopeScanPhaseIdle;
    app->scan_phase_timeout_ms = 3000;
    app->scan_timer =
        furi_timer_alloc(cardscope_detector_on_timer, FuriTimerTypeOnce, app);

    cardscope_result_clear(&app->result);
    cardscope_history_load(app);

    return app;
}

void cardscope_zero_app_free(CardScopeZeroApp* app) {
    furi_assert(app);

    cardscope_detector_stop_all(app);

    furi_timer_free(app->scan_timer);

    lfrfid_worker_free(app->lfrfid_worker);
    protocol_dict_free(app->lfrfid_dict);

    nfc_device_free(app->nfc_device);
    nfc_free(app->nfc);

    furi_string_free(app->text_box_store);
    furi_string_free(app->temp_str);

    view_dispatcher_remove_view(app->view_dispatcher, CardScopeViewScan);
    cardscope_scan_view_free(app->scan_view);

    view_dispatcher_remove_view(app->view_dispatcher, CardScopeViewVariableItemList);
    variable_item_list_free(app->variable_item_list);

    view_dispatcher_remove_view(app->view_dispatcher, CardScopeViewDialogEx);
    dialog_ex_free(app->dialog_ex);

    view_dispatcher_remove_view(app->view_dispatcher, CardScopeViewTextBox);
    text_box_free(app->text_box);

    view_dispatcher_remove_view(app->view_dispatcher, CardScopeViewWidget);
    widget_free(app->widget);

    view_dispatcher_remove_view(app->view_dispatcher, CardScopeViewPopup);
    popup_free(app->popup);

    view_dispatcher_remove_view(app->view_dispatcher, CardScopeViewSubmenu);
    submenu_free(app->submenu);

    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);

    furi_record_close(RECORD_DIALOGS);
    furi_record_close(RECORD_STORAGE);
    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_GUI);

    free(app);
}

int32_t cardscope_zero_app(void* p) {
    UNUSED(p);

    CardScopeZeroApp* app = cardscope_zero_app_alloc();

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);
    scene_manager_next_scene(app->scene_manager, CardScopeZeroSceneMainMenu);
    view_dispatcher_run(app->view_dispatcher);

    cardscope_zero_app_free(app);
    return 0;
}
