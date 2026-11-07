#include "scan_view.h"

#include <gui/canvas.h>
#include <gui/elements.h>
#include <gui/icon_animation.h>
#include <furi.h>

#include "cardscope_zero_icons.h"

struct CardScopeScanView {
    View* view;
    IconAnimation* cat_animation;
};

typedef struct {
    IconAnimation* cat_animation;
    const char* status;
} CardScopeScanViewModel;

static void cardscope_scan_view_draw(Canvas* canvas, void* _model) {
    CardScopeScanViewModel* model = _model;

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);

    if(model->cat_animation) {
        canvas_draw_icon_animation(canvas, 2, 10, model->cat_animation);
    }

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 86, 2, AlignCenter, AlignTop, "Card Detector");

    canvas_set_font(canvas, FontSecondary);
    elements_multiline_text_aligned(
        canvas,
        86,
        18,
        AlignCenter,
        AlignTop,
        model->status ? model->status : "Waiting for card...");

    canvas_draw_str_aligned(canvas, 86, 60, AlignCenter, AlignBottom, "Back to cancel");
}

static bool cardscope_scan_view_input_callback(InputEvent* event, void* context) {
    UNUSED(event);
    UNUSED(context);
    return false;
}

CardScopeScanView* cardscope_scan_view_alloc(void) {
    CardScopeScanView* instance = malloc(sizeof(CardScopeScanView));

    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(CardScopeScanViewModel));
    view_set_context(instance->view, instance);
    view_set_draw_callback(instance->view, cardscope_scan_view_draw);
    view_set_input_callback(instance->view, cardscope_scan_view_input_callback);

    instance->cat_animation = icon_animation_alloc(&A_CatReading_42x42);
    view_tie_icon_animation(instance->view, instance->cat_animation);

    with_view_model(
        instance->view,
        CardScopeScanViewModel * model,
        {
            model->cat_animation = instance->cat_animation;
            model->status = "Waiting for card...";
        },
        true);

    return instance;
}

void cardscope_scan_view_free(CardScopeScanView* instance) {
    furi_assert(instance);
    icon_animation_stop(instance->cat_animation);
    icon_animation_free(instance->cat_animation);
    view_free(instance->view);
    free(instance);
}

View* cardscope_scan_view_get_view(CardScopeScanView* instance) {
    furi_assert(instance);
    return instance->view;
}

void cardscope_scan_view_set_status(CardScopeScanView* instance, const char* status) {
    furi_assert(instance);
    with_view_model(
        instance->view, CardScopeScanViewModel * model, { model->status = status; }, true);
}

void cardscope_scan_view_start_animation(CardScopeScanView* instance) {
    furi_assert(instance);
    icon_animation_start(instance->cat_animation);
}

void cardscope_scan_view_stop_animation(CardScopeScanView* instance) {
    furi_assert(instance);
    icon_animation_stop(instance->cat_animation);
}
