#include "../cardscope_zero.h"

typedef enum {
    CardScopeMenuIndexAutoDetect,
    CardScopeMenuIndexNfcAnalyzer,
    CardScopeMenuIndexRfidAnalyzer,
    CardScopeMenuIndexCardDatabase,
    CardScopeMenuIndexScanHistory,
    CardScopeMenuIndexAbout,
} CardScopeMenuIndex;

static void cardscope_zero_scene_main_menu_submenu_callback(void* context, uint32_t index) {
    CardScopeZeroApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void cardscope_zero_scene_main_menu_on_enter(void* context) {
    CardScopeZeroApp* app = context;
    Submenu* submenu = app->submenu;

    submenu_add_item(
        submenu,
        "Auto Detect",
        CardScopeMenuIndexAutoDetect,
        cardscope_zero_scene_main_menu_submenu_callback,
        app);
    submenu_add_item(
        submenu,
        "NFC Analyzer",
        CardScopeMenuIndexNfcAnalyzer,
        cardscope_zero_scene_main_menu_submenu_callback,
        app);
    submenu_add_item(
        submenu,
        "RFID Analyzer",
        CardScopeMenuIndexRfidAnalyzer,
        cardscope_zero_scene_main_menu_submenu_callback,
        app);
    submenu_add_item(
        submenu,
        "Card Database",
        CardScopeMenuIndexCardDatabase,
        cardscope_zero_scene_main_menu_submenu_callback,
        app);
    submenu_add_item(
        submenu,
        "Scan History",
        CardScopeMenuIndexScanHistory,
        cardscope_zero_scene_main_menu_submenu_callback,
        app);
    submenu_add_item(
        submenu,
        "About",
        CardScopeMenuIndexAbout,
        cardscope_zero_scene_main_menu_submenu_callback,
        app);

    submenu_set_header(submenu, "CardScope Zero");
    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(app->scene_manager, CardScopeZeroSceneMainMenu));

    view_dispatcher_switch_to_view(app->view_dispatcher, CardScopeViewSubmenu);
}

bool cardscope_zero_scene_main_menu_on_event(void* context, SceneManagerEvent event) {
    CardScopeZeroApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(
            app->scene_manager, CardScopeZeroSceneMainMenu, event.event);
        consumed = true;

        switch(event.event) {
        case CardScopeMenuIndexAutoDetect:
            app->scan_mode = CardScopeScanModeAuto;
            scene_manager_next_scene(app->scene_manager, CardScopeZeroSceneScan);
            break;
        case CardScopeMenuIndexNfcAnalyzer:
            app->scan_mode = CardScopeScanModeNfc;
            scene_manager_next_scene(app->scene_manager, CardScopeZeroSceneScan);
            break;
        case CardScopeMenuIndexRfidAnalyzer:
            app->scan_mode = CardScopeScanModeRfid;
            scene_manager_next_scene(app->scene_manager, CardScopeZeroSceneScan);
            break;
        case CardScopeMenuIndexCardDatabase:
            scene_manager_next_scene(app->scene_manager, CardScopeZeroSceneDatabase);
            break;
        case CardScopeMenuIndexScanHistory:
            scene_manager_next_scene(app->scene_manager, CardScopeZeroSceneHistory);
            break;
        case CardScopeMenuIndexAbout:
            scene_manager_next_scene(app->scene_manager, CardScopeZeroSceneAbout);
            break;
        default:
            break;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = false;
    }

    return consumed;
}

void cardscope_zero_scene_main_menu_on_exit(void* context) {
    CardScopeZeroApp* app = context;
    submenu_reset(app->submenu);
}
