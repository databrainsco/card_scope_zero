#include "card_detector.h"
#include "nfc_analyzer.h"
#include "rfid_analyzer.h"

#include <nfc/protocols/nfc_generic_event.h>

#define CARDSCOPE_PHASE_TIMEOUT_MS (3000U)

static void cardscope_detector_stop_nfc(CardScopeZeroApp* app) {
    if(app->nfc_poller) {
        nfc_poller_stop(app->nfc_poller);
        nfc_poller_free(app->nfc_poller);
        app->nfc_poller = NULL;
    }
    if(app->nfc_scanner) {
        nfc_scanner_stop(app->nfc_scanner);
        nfc_scanner_free(app->nfc_scanner);
        app->nfc_scanner = NULL;
    }
    app->nfc_active = false;
}

static void cardscope_detector_stop_rfid(CardScopeZeroApp* app) {
    if(app->lfrfid_worker) {
        lfrfid_worker_stop(app->lfrfid_worker);
        lfrfid_worker_stop_thread(app->lfrfid_worker);
    }
    app->lfrfid_active = false;
}

void cardscope_detector_stop_all(CardScopeZeroApp* app) {
    furi_assert(app);
    if(app->scan_timer) {
        furi_timer_stop(app->scan_timer);
    }
    cardscope_detector_stop_nfc(app);
    cardscope_detector_stop_rfid(app);
    if(app->scan_view) {
        cardscope_scan_view_stop_animation(app->scan_view);
    }
    app->scan_phase = CardScopeScanPhaseIdle;
    cardscope_zero_blink_stop(app);
}

static void cardscope_detector_restart_timer(CardScopeZeroApp* app) {
    furi_timer_stop(app->scan_timer);
    furi_timer_start(app->scan_timer, app->scan_phase_timeout_ms);
}

static void cardscope_nfc_scanner_callback(NfcScannerEvent event, void* context) {
    CardScopeZeroApp* app = context;
    if(event.type == NfcScannerEventTypeDetected && event.data.protocol_num > 0) {
        /* Prefer the most specific (last) protocol when multiple are reported. */
        app->nfc_protocol = event.data.protocols[event.data.protocol_num - 1];
        view_dispatcher_send_custom_event(
            app->view_dispatcher, CardScopeCustomEventScanNfcDetected);
    }
}

static NfcCommand cardscope_nfc_poller_callback(NfcGenericEvent event, void* context) {
    UNUSED(event);
    CardScopeZeroApp* app = context;

    const NfcDeviceData* data = nfc_poller_get_data(app->nfc_poller);
    if(data) {
        nfc_device_set_data(app->nfc_device, app->nfc_protocol, data);
        view_dispatcher_send_custom_event(
            app->view_dispatcher, CardScopeCustomEventScanNfcReadDone);
        return NfcCommandStop;
    }

    view_dispatcher_send_custom_event(app->view_dispatcher, CardScopeCustomEventScanNfcError);
    return NfcCommandStop;
}

static void cardscope_rfid_read_callback(
    LFRFIDWorkerReadResult result,
    ProtocolId protocol,
    void* context) {
    CardScopeZeroApp* app = context;
    if(result == LFRFIDWorkerReadDone) {
        app->lfrfid_protocol = protocol;
        view_dispatcher_send_custom_event(app->view_dispatcher, CardScopeCustomEventScanRfidDone);
    }
}

static void cardscope_detector_update_scan_view(CardScopeZeroApp* app, const char* line) {
    cardscope_scan_view_set_status(app->scan_view, line);
    cardscope_scan_view_start_animation(app->scan_view);
    view_dispatcher_switch_to_view(app->view_dispatcher, CardScopeViewScan);
}

static void cardscope_detector_start_nfc_phase(CardScopeZeroApp* app) {
    app->scan_phase = CardScopeScanPhaseNfc;
    cardscope_detector_update_scan_view(app, "Scanning NFC...\nHold card near back");
    cardscope_zero_blink_start(app);

    app->nfc_scanner = nfc_scanner_alloc(app->nfc);
    nfc_scanner_start(app->nfc_scanner, cardscope_nfc_scanner_callback, app);
    app->nfc_active = true;
    cardscope_detector_restart_timer(app);
}

static void cardscope_detector_start_rfid_phase(CardScopeZeroApp* app) {
    cardscope_detector_stop_nfc(app);

    app->scan_phase = CardScopeScanPhaseRfid;
    cardscope_detector_update_scan_view(app, "Scanning RFID...\nHold tag near left");
    cardscope_zero_blink_start(app);

    lfrfid_worker_start_thread(app->lfrfid_worker);
    lfrfid_worker_read_start(
        app->lfrfid_worker, LFRFIDWorkerReadTypeAuto, cardscope_rfid_read_callback, app);
    app->lfrfid_active = true;
    cardscope_detector_restart_timer(app);
}

static void cardscope_detector_finish_none(CardScopeZeroApp* app) {
    cardscope_detector_stop_all(app);
    cardscope_result_clear(&app->result);
    app->result.status = CardScopeStatusUnknown;
    app->result.technology = CardScopeTechNone;
    strlcpy(app->result.protocol_name, "No card detected", sizeof(app->result.protocol_name));
    strlcpy(app->result.notes, "No NFC or LF RFID response in scan window", sizeof(app->result.notes));
    app->scan_phase = CardScopeScanPhaseDone;
    scene_manager_next_scene(app->scene_manager, CardScopeZeroSceneResult);
}

void cardscope_detector_start(CardScopeZeroApp* app) {
    furi_assert(app);
    cardscope_detector_stop_all(app);
    cardscope_result_clear(&app->result);
    app->result.status = CardScopeStatusWaiting;
    app->scan_phase_timeout_ms = CARDSCOPE_PHASE_TIMEOUT_MS;

    if(app->scan_mode == CardScopeScanModeRfid) {
        cardscope_detector_start_rfid_phase(app);
    } else {
        cardscope_detector_start_nfc_phase(app);
    }
}

void cardscope_detector_on_timer(void* context) {
    CardScopeZeroApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, CardScopeCustomEventScanTimeout);
}

static bool cardscope_detector_advance_after_nfc_miss(CardScopeZeroApp* app) {
    if(app->scan_mode == CardScopeScanModeAuto) {
        cardscope_detector_start_rfid_phase(app);
        return true;
    }
    cardscope_detector_finish_none(app);
    return true;
}

bool cardscope_detector_on_custom_event(CardScopeZeroApp* app, uint32_t event) {
    furi_assert(app);

    switch(event) {
    case CardScopeCustomEventScanTimeout:
        if(app->scan_phase == CardScopeScanPhaseNfc) {
            return cardscope_detector_advance_after_nfc_miss(app);
        }
        if(app->scan_phase == CardScopeScanPhaseRfid) {
            cardscope_detector_finish_none(app);
            return true;
        }
        break;

    case CardScopeCustomEventScanNfcDetected:
        if(app->scan_phase != CardScopeScanPhaseNfc) break;
        furi_timer_stop(app->scan_timer);
        nfc_scanner_stop(app->nfc_scanner);
        nfc_scanner_free(app->nfc_scanner);
        app->nfc_scanner = NULL;

        cardscope_detector_update_scan_view(app, "Reading NFC...\nStay still");
        app->nfc_poller = nfc_poller_alloc(app->nfc, app->nfc_protocol);
        nfc_poller_start(app->nfc_poller, cardscope_nfc_poller_callback, app);
        return true;

    case CardScopeCustomEventScanNfcReadDone:
        cardscope_detector_stop_nfc(app);
        furi_timer_stop(app->scan_timer);
        cardscope_nfc_analyzer_fill_from_device(&app->result, app->nfc_device);
        cardscope_zero_notify_success(app);
        app->scan_phase = CardScopeScanPhaseDone;
        scene_manager_next_scene(app->scene_manager, CardScopeZeroSceneResult);
        return true;

    case CardScopeCustomEventScanNfcError:
        cardscope_detector_stop_nfc(app);
        if(app->scan_mode == CardScopeScanModeAuto) {
            cardscope_detector_start_rfid_phase(app);
        } else {
            cardscope_result_clear(&app->result);
            app->result.status = CardScopeStatusError;
            app->result.technology = CardScopeTechNfc;
            strlcpy(app->result.protocol_name, "NFC read error", sizeof(app->result.protocol_name));
            scene_manager_next_scene(app->scene_manager, CardScopeZeroSceneResult);
        }
        return true;

    case CardScopeCustomEventScanRfidDone:
        furi_timer_stop(app->scan_timer);
        cardscope_rfid_analyzer_fill(&app->result, app->lfrfid_dict, app->lfrfid_protocol);
        cardscope_detector_stop_rfid(app);
        cardscope_zero_notify_success(app);
        app->scan_phase = CardScopeScanPhaseDone;
        scene_manager_next_scene(app->scene_manager, CardScopeZeroSceneResult);
        return true;

    case CardScopeCustomEventScanCancel:
        cardscope_detector_stop_all(app);
        cardscope_result_clear(&app->result);
        app->result.status = CardScopeStatusCancelled;
        return true;

    default:
        break;
    }

    return false;
}
