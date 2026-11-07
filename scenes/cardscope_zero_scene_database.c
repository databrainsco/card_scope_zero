#include "../cardscope_zero.h"
#include "../modules/compatibility.h"

void cardscope_zero_scene_database_on_enter(void* context) {
    CardScopeZeroApp* app = context;

    cardscope_compatibility_format_matrix(app->text_box_store);
    text_box_reset(app->text_box);
    text_box_set_font(app->text_box, TextBoxFontText);
    text_box_set_text(app->text_box, furi_string_get_cstr(app->text_box_store));
    view_dispatcher_switch_to_view(app->view_dispatcher, CardScopeViewTextBox);
}

bool cardscope_zero_scene_database_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void cardscope_zero_scene_database_on_exit(void* context) {
    CardScopeZeroApp* app = context;
    text_box_reset(app->text_box);
}
