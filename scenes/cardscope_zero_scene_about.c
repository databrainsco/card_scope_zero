#include "../cardscope_zero.h"

void cardscope_zero_scene_about_on_enter(void* context) {
    CardScopeZeroApp* app = context;

    furi_string_set(
        app->text_box_store,
        "CardScope Zero\n"
        "v0.1.0\n"
        "\n"
        "NFC/RFID identification\n"
        "and analysis tool.\n"
        "\n"
        "Target firmware:\n"
        "Official 1.5.1 (uFBT)\n"
        "\n"
        "Uses official SDK APIs:\n"
        "- NfcScanner / NfcPoller\n"
        "- LFRFIDWorker\n"
        "- ProtocolDict\n"
        "\n"
        "Read-only by default.\n"
        "No auth bypass.\n"
        "\n"
        "Antennas are scanned\n"
        "sequentially (NFC then\n"
        "125 kHz) in Auto mode.");

    text_box_reset(app->text_box);
    text_box_set_font(app->text_box, TextBoxFontText);
    text_box_set_text(app->text_box, furi_string_get_cstr(app->text_box_store));
    view_dispatcher_switch_to_view(app->view_dispatcher, CardScopeViewTextBox);
}

bool cardscope_zero_scene_about_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void cardscope_zero_scene_about_on_exit(void* context) {
    CardScopeZeroApp* app = context;
    text_box_reset(app->text_box);
}
