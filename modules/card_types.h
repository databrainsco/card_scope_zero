#pragma once

#include <furi.h>
#include <datetime/datetime.h>
#include <nfc/protocols/nfc_protocol.h>
#include <lfrfid/protocols/lfrfid_protocols.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CARDSCOPE_UID_MAX_LEN      (10U)
#define CARDSCOPE_NAME_MAX_LEN     (32U)
#define CARDSCOPE_DETAIL_MAX_LEN   (96U)
#define CARDSCOPE_HISTORY_MAX      (32U)

typedef enum {
    CardScopeTechNone = 0,
    CardScopeTechNfc,
    CardScopeTechRfid125,
} CardScopeTechnology;

typedef enum {
    CardScopeStatusNone = 0,
    CardScopeStatusWaiting,
    CardScopeStatusIdentified,
    CardScopeStatusPartial,
    CardScopeStatusUnknown,
    CardScopeStatusError,
    CardScopeStatusCancelled,
} CardScopeStatus;

typedef enum {
    CardScopeScanPhaseIdle = 0,
    CardScopeScanPhaseNfc,
    CardScopeScanPhaseRfid,
    CardScopeScanPhaseDone,
} CardScopeScanPhase;

typedef enum {
    CardScopeCapabilitySupported = 0,
    CardScopeCapabilityPartial,
    CardScopeCapabilityNotSupported,
} CardScopeCapability;

typedef struct {
    CardScopeCapability detect;
    CardScopeCapability read;
    CardScopeCapability identify;
    CardScopeCapability analyze;
    CardScopeCapability write;
    CardScopeCapability emulate;
} CardScopeCapabilityMatrix;

typedef struct {
    CardScopeTechnology technology;
    CardScopeStatus status;

    char protocol_name[CARDSCOPE_NAME_MAX_LEN];
    char family_name[CARDSCOPE_NAME_MAX_LEN];
    char type_name[CARDSCOPE_NAME_MAX_LEN];
    char manufacturer[CARDSCOPE_NAME_MAX_LEN];
    char frequency[CARDSCOPE_NAME_MAX_LEN];
    char auth_info[CARDSCOPE_DETAIL_MAX_LEN];
    char protection_info[CARDSCOPE_DETAIL_MAX_LEN];
    char memory_info[CARDSCOPE_DETAIL_MAX_LEN];
    char notes[CARDSCOPE_DETAIL_MAX_LEN];
    char render_uid[CARDSCOPE_DETAIL_MAX_LEN];
    char render_data[CARDSCOPE_DETAIL_MAX_LEN];

    uint8_t uid[CARDSCOPE_UID_MAX_LEN];
    size_t uid_len;

    bool is_nfc;
    NfcProtocol nfc_protocol;
    LFRFIDProtocol rfid_protocol;

    uint16_t pages_or_blocks_total;
    uint16_t pages_or_blocks_read;
    bool auth_required;
    bool data_partial;

    CardScopeCapabilityMatrix capabilities;
} CardScopeResult;

typedef struct {
    DateTime datetime;
    CardScopeTechnology technology;
    CardScopeStatus status;
    char protocol_name[CARDSCOPE_NAME_MAX_LEN];
    char family_name[CARDSCOPE_NAME_MAX_LEN];
    char render_uid[CARDSCOPE_DETAIL_MAX_LEN];
} CardScopeHistoryEntry;

typedef struct {
    CardScopeHistoryEntry entries[CARDSCOPE_HISTORY_MAX];
    size_t count;
} CardScopeHistory;

void cardscope_result_clear(CardScopeResult* result);
const char* cardscope_technology_name(CardScopeTechnology tech);
const char* cardscope_status_name(CardScopeStatus status);
const char* cardscope_capability_name(CardScopeCapability cap);

#ifdef __cplusplus
}
#endif
