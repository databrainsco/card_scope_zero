#include "history_manager.h"

#include <string.h>
#include <flipper_format/flipper_format.h>
#include <furi_hal_rtc.h>

static bool cardscope_history_entries_equal(
    const CardScopeHistoryEntry* a,
    const CardScopeResult* b) {
    if(a->technology != b->technology) return false;
    if(strncmp(a->protocol_name, b->protocol_name, CARDSCOPE_NAME_MAX_LEN) != 0) return false;
    if(strncmp(a->render_uid, b->render_uid, CARDSCOPE_DETAIL_MAX_LEN) != 0) return false;
    return true;
}

bool cardscope_history_load(CardScopeZeroApp* app) {
    furi_assert(app);
    memset(&app->history, 0, sizeof(app->history));

    storage_simply_mkdir(app->storage, CARDSCOPE_HISTORY_FOLDER);

    FlipperFormat* ff = flipper_format_file_alloc(app->storage);
    bool ok = false;

    do {
        if(!flipper_format_file_open_existing(ff, CARDSCOPE_HISTORY_FILE)) break;

        uint32_t version = 0;
        if(!flipper_format_read_header(ff, app->temp_str, &version)) break;
        if(!furi_string_equal_str(app->temp_str, "CardScope History")) break;
        if(version != 1) break;

        uint32_t count = 0;
        if(!flipper_format_read_uint32(ff, "Count", &count, 1)) break;
        if(count > CARDSCOPE_HISTORY_MAX) count = CARDSCOPE_HISTORY_MAX;

        for(uint32_t i = 0; i < count; i++) {
            CardScopeHistoryEntry* entry = &app->history.entries[i];
            uint32_t tech = 0;
            uint32_t status = 0;
            uint32_t year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;

            if(!flipper_format_read_uint32(ff, "Tech", &tech, 1)) break;
            if(!flipper_format_read_uint32(ff, "Status", &status, 1)) break;
            if(!flipper_format_read_string(ff, "Protocol", app->temp_str)) break;
            strlcpy(
                entry->protocol_name,
                furi_string_get_cstr(app->temp_str),
                sizeof(entry->protocol_name));
            if(!flipper_format_read_string(ff, "Family", app->temp_str)) break;
            strlcpy(
                entry->family_name,
                furi_string_get_cstr(app->temp_str),
                sizeof(entry->family_name));
            if(!flipper_format_read_string(ff, "UID", app->temp_str)) break;
            strlcpy(
                entry->render_uid,
                furi_string_get_cstr(app->temp_str),
                sizeof(entry->render_uid));
            if(!flipper_format_read_uint32(ff, "Year", &year, 1)) break;
            if(!flipper_format_read_uint32(ff, "Month", &month, 1)) break;
            if(!flipper_format_read_uint32(ff, "Day", &day, 1)) break;
            if(!flipper_format_read_uint32(ff, "Hour", &hour, 1)) break;
            if(!flipper_format_read_uint32(ff, "Minute", &minute, 1)) break;
            if(!flipper_format_read_uint32(ff, "Second", &second, 1)) break;

            entry->technology = (CardScopeTechnology)tech;
            entry->status = (CardScopeStatus)status;
            entry->datetime.year = (uint16_t)year;
            entry->datetime.month = (uint8_t)month;
            entry->datetime.day = (uint8_t)day;
            entry->datetime.hour = (uint8_t)hour;
            entry->datetime.minute = (uint8_t)minute;
            entry->datetime.second = (uint8_t)second;
            app->history.count++;
        }
        ok = true;
    } while(false);

    flipper_format_free(ff);
    return ok;
}

bool cardscope_history_save(CardScopeZeroApp* app) {
    furi_assert(app);
    storage_simply_mkdir(app->storage, CARDSCOPE_HISTORY_FOLDER);

    FlipperFormat* ff = flipper_format_file_alloc(app->storage);
    bool ok = false;

    do {
        if(!flipper_format_file_open_always(ff, CARDSCOPE_HISTORY_FILE)) break;
        if(!flipper_format_write_header_cstr(ff, "CardScope History", 1)) break;

        uint32_t count = (uint32_t)app->history.count;
        if(!flipper_format_write_uint32(ff, "Count", &count, 1)) break;

        for(size_t i = 0; i < app->history.count; i++) {
            CardScopeHistoryEntry* entry = &app->history.entries[i];
            uint32_t tech = entry->technology;
            uint32_t status = entry->status;
            uint32_t year = entry->datetime.year;
            uint32_t month = entry->datetime.month;
            uint32_t day = entry->datetime.day;
            uint32_t hour = entry->datetime.hour;
            uint32_t minute = entry->datetime.minute;
            uint32_t second = entry->datetime.second;

            if(!flipper_format_write_uint32(ff, "Tech", &tech, 1)) break;
            if(!flipper_format_write_uint32(ff, "Status", &status, 1)) break;
            if(!flipper_format_write_string_cstr(ff, "Protocol", entry->protocol_name)) break;
            if(!flipper_format_write_string_cstr(ff, "Family", entry->family_name)) break;
            if(!flipper_format_write_string_cstr(ff, "UID", entry->render_uid)) break;
            if(!flipper_format_write_uint32(ff, "Year", &year, 1)) break;
            if(!flipper_format_write_uint32(ff, "Month", &month, 1)) break;
            if(!flipper_format_write_uint32(ff, "Day", &day, 1)) break;
            if(!flipper_format_write_uint32(ff, "Hour", &hour, 1)) break;
            if(!flipper_format_write_uint32(ff, "Minute", &minute, 1)) break;
            if(!flipper_format_write_uint32(ff, "Second", &second, 1)) break;
        }
        ok = true;
    } while(false);

    flipper_format_free(ff);
    return ok;
}

bool cardscope_history_add(CardScopeZeroApp* app, const CardScopeResult* result) {
    furi_assert(app);
    furi_assert(result);

    if(result->status != CardScopeStatusIdentified && result->status != CardScopeStatusPartial &&
       result->status != CardScopeStatusUnknown) {
        return false;
    }

    if(app->history.count > 0 &&
       cardscope_history_entries_equal(&app->history.entries[0], result)) {
        return false;
    }

    if(app->history.count >= CARDSCOPE_HISTORY_MAX) {
        memmove(
            &app->history.entries[0],
            &app->history.entries[1],
            sizeof(CardScopeHistoryEntry) * (CARDSCOPE_HISTORY_MAX - 1));
        app->history.count = CARDSCOPE_HISTORY_MAX - 1;
    } else if(app->history.count > 0) {
        memmove(
            &app->history.entries[1],
            &app->history.entries[0],
            sizeof(CardScopeHistoryEntry) * app->history.count);
    }

    CardScopeHistoryEntry* entry = &app->history.entries[0];
    memset(entry, 0, sizeof(*entry));
    entry->technology = result->technology;
    entry->status = result->status;
    strlcpy(entry->protocol_name, result->protocol_name, sizeof(entry->protocol_name));
    strlcpy(entry->family_name, result->family_name, sizeof(entry->family_name));
    strlcpy(entry->render_uid, result->render_uid, sizeof(entry->render_uid));
    furi_hal_rtc_get_datetime(&entry->datetime);

    app->history.count++;
    return cardscope_history_save(app);
}

bool cardscope_history_delete(CardScopeZeroApp* app, size_t index) {
    furi_assert(app);
    if(index >= app->history.count) return false;

    if(index + 1 < app->history.count) {
        memmove(
            &app->history.entries[index],
            &app->history.entries[index + 1],
            sizeof(CardScopeHistoryEntry) * (app->history.count - index - 1));
    }
    app->history.count--;
    return cardscope_history_save(app);
}

void cardscope_history_clear(CardScopeZeroApp* app) {
    furi_assert(app);
    memset(&app->history, 0, sizeof(app->history));
    cardscope_history_save(app);
}
