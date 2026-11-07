#pragma once

#include <furi.h>
#include <furi_hal.h>

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <gui/modules/popup.h>
#include <gui/modules/widget.h>
#include <gui/modules/text_box.h>
#include <gui/modules/dialog_ex.h>
#include <gui/modules/variable_item_list.h>

#include <notification/notification_messages.h>
#include <storage/storage.h>
#include <dialogs/dialogs.h>
#include <datetime/datetime.h>

#include <nfc/nfc.h>
#include <nfc/nfc_device.h>
#include <nfc/nfc_poller.h>
#include <nfc/nfc_scanner.h>
#include <nfc/protocols/nfc_protocol.h>

#include <toolbox/protocols/protocol_dict.h>
#include <lfrfid/lfrfid_worker.h>
#include <lfrfid/protocols/lfrfid_protocols.h>

#include "scenes/cardscope_zero_scene.h"
#include "modules/card_types.h"
#include "views/scan_view.h"

#define CARDSCOPE_ZERO_TAG "CardScopeZero"

#define CARDSCOPE_APP_FOLDER     APP_DATA_PATH("")
#define CARDSCOPE_HISTORY_FOLDER APP_DATA_PATH("history")
#define CARDSCOPE_HISTORY_FILE   APP_DATA_PATH("history/history.txt")

typedef enum {
    CardScopeViewSubmenu,
    CardScopeViewPopup,
    CardScopeViewWidget,
    CardScopeViewTextBox,
    CardScopeViewDialogEx,
    CardScopeViewVariableItemList,
    CardScopeViewScan,
} CardScopeView;

typedef enum {
    CardScopeCustomEventScanNfcDetected,
    CardScopeCustomEventScanNfcReadDone,
    CardScopeCustomEventScanNfcError,
    CardScopeCustomEventScanRfidDone,
    CardScopeCustomEventScanTimeout,
    CardScopeCustomEventScanCancel,
} CardScopeCustomEvent;

typedef enum {
    CardScopeScanModeAuto,
    CardScopeScanModeNfc,
    CardScopeScanModeRfid,
} CardScopeScanMode;

typedef struct CardScopeZeroApp {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    NotificationApp* notifications;
    Storage* storage;
    DialogsApp* dialogs;

    Submenu* submenu;
    Popup* popup;
    Widget* widget;
    TextBox* text_box;
    DialogEx* dialog_ex;
    VariableItemList* variable_item_list;
    CardScopeScanView* scan_view;

    FuriString* text_box_store;
    FuriString* temp_str;

    /* NFC */
    Nfc* nfc;
    NfcScanner* nfc_scanner;
    NfcPoller* nfc_poller;
    NfcDevice* nfc_device;
    NfcProtocol nfc_protocol;
    bool nfc_active;

    /* LF RFID */
    ProtocolDict* lfrfid_dict;
    LFRFIDWorker* lfrfid_worker;
    ProtocolId lfrfid_protocol;
    bool lfrfid_active;

    /* Scan state */
    CardScopeScanMode scan_mode;
    CardScopeScanPhase scan_phase;
    FuriTimer* scan_timer;
    uint32_t scan_phase_timeout_ms;

    /* Last result */
    CardScopeResult result;

    /* History */
    CardScopeHistory history;
} CardScopeZeroApp;

CardScopeZeroApp* cardscope_zero_app_alloc(void);
void cardscope_zero_app_free(CardScopeZeroApp* app);

void cardscope_zero_blink_start(CardScopeZeroApp* app);
void cardscope_zero_blink_stop(CardScopeZeroApp* app);
void cardscope_zero_notify_success(CardScopeZeroApp* app);
void cardscope_zero_notify_error(CardScopeZeroApp* app);
