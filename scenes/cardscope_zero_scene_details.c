#include "../cardscope_zero.h"
#include "../modules/nfc_analyzer.h"
#include "../modules/rfid_analyzer.h"
#include "../modules/compatibility.h"
#include "../modules/security_analyzer.h"
#include "../modules/memory_analyzer.h"

void cardscope_zero_scene_details_on_enter(void* context) {
    CardScopeZeroApp* app = context;

    furi_string_reset(app->text_box_store);

    if(app->result.technology == CardScopeTechRfid125) {
        cardscope_rfid_analyzer_format_details(&app->result, app->text_box_store);
    } else {
        cardscope_nfc_analyzer_format_details(&app->result, app->text_box_store);
    }

    furi_string_cat_str(app->text_box_store, "\n");
    cardscope_memory_analyzer_format(&app->result, app->temp_str);
    furi_string_cat(app->text_box_store, app->temp_str);

    furi_string_cat_str(app->text_box_store, "\n");
    cardscope_security_analyzer_format(&app->result, app->temp_str);
    furi_string_cat(app->text_box_store, app->temp_str);

    furi_string_cat_str(app->text_box_store, "\n");
    cardscope_compatibility_format_for_result(&app->result, app->temp_str);
    furi_string_cat(app->text_box_store, app->temp_str);

    text_box_reset(app->text_box);
    text_box_set_font(app->text_box, TextBoxFontText);
    text_box_set_text(app->text_box, furi_string_get_cstr(app->text_box_store));
    view_dispatcher_switch_to_view(app->view_dispatcher, CardScopeViewTextBox);
}

bool cardscope_zero_scene_details_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void cardscope_zero_scene_details_on_exit(void* context) {
    CardScopeZeroApp* app = context;
    text_box_reset(app->text_box);
}
