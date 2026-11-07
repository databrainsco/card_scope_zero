#include "protocol_database.h"
#include <string.h>

/*
 * Capability matrix based on Flipper Zero firmware SDK 1.5.1 APIs.
 * Values reflect detection/read/identify/analyze/write/emulate support
 * exposed by libnfc / liblfrfid — not assumed hardware magic.
 */

#define CAP(d, r, i, a, w, e)                                                    \
    {                                                                            \
        .detect = (d), .read = (r), .identify = (i), .analyze = (a), .write = (w), \
        .emulate = (e)                                                           \
    }

#define S CardScopeCapabilitySupported
#define P CardScopeCapabilityPartial
#define N CardScopeCapabilityNotSupported

static const CardScopeProtocolInfo cardscope_protocol_db[] = {
    /* NFC families */
    {
        .name = "ISO14443-3A",
        .family = "ISO14443-A",
        .frequency = "13.56 MHz",
        .technology = CardScopeTechNfc,
        .capabilities = CAP(S, S, S, P, N, P),
        .notes = "Base Type A layer; UID/ATQA/SAK only unless child protocol matches.",
    },
    {
        .name = "ISO14443-3B",
        .family = "ISO14443-B",
        .frequency = "13.56 MHz",
        .technology = CardScopeTechNfc,
        .capabilities = CAP(S, S, S, P, N, N),
        .notes = "Type B detection and basic polling supported.",
    },
    {
        .name = "ISO14443-4A",
        .family = "ISO14443-4",
        .frequency = "13.56 MHz",
        .technology = CardScopeTechNfc,
        .capabilities = CAP(S, S, S, P, N, P),
        .notes = "ISO-DEP layer; higher apps may need DESFire/Plus pollers.",
    },
    {
        .name = "ISO14443-4B",
        .family = "ISO14443-4",
        .frequency = "13.56 MHz",
        .technology = CardScopeTechNfc,
        .capabilities = CAP(S, S, S, P, N, N),
        .notes = "ISO-DEP Type B supported for detection/read.",
    },
    {
        .name = "ISO15693-3",
        .family = "ISO15693",
        .frequency = "13.56 MHz",
        .technology = CardScopeTechNfc,
        .capabilities = CAP(S, S, S, P, N, N),
        .notes = "Vicinity cards; SLIX is a child protocol.",
    },
    {
        .name = "FeliCa",
        .family = "FeliCa",
        .frequency = "13.56 MHz",
        .technology = CardScopeTechNfc,
        .capabilities = CAP(S, P, S, P, N, P),
        .notes = "IDm readable; service blocks may require keys.",
    },
    {
        .name = "MIFARE Ultralight / NTAG",
        .family = "MIFARE Ultralight",
        .frequency = "13.56 MHz",
        .technology = CardScopeTechNfc,
        .capabilities = CAP(S, P, S, S, P, S),
        .notes = "Type from GET_VERSION when available. Password/UL-C pages may be locked.",
    },
    {
        .name = "MIFARE Classic",
        .family = "MIFARE Classic",
        .frequency = "13.56 MHz",
        .technology = CardScopeTechNfc,
        .capabilities = CAP(S, P, S, P, P, S),
        .notes = "Mini/1K/4K identified. Sector data requires known keys; no silent bypass.",
    },
    {
        .name = "MIFARE Plus",
        .family = "MIFARE Plus",
        .frequency = "13.56 MHz",
        .technology = CardScopeTechNfc,
        .capabilities = CAP(S, P, S, P, N, P),
        .notes = "Detection supported; secure levels need credentials.",
    },
    {
        .name = "MIFARE DESFire",
        .family = "MIFARE DESFire",
        .frequency = "13.56 MHz",
        .technology = CardScopeTechNfc,
        .capabilities = CAP(S, P, S, P, N, N),
        .notes = "Version/apps readable when free. Encrypted files need keys.",
    },
    {
        .name = "SLIX",
        .family = "ISO15693",
        .frequency = "13.56 MHz",
        .technology = CardScopeTechNfc,
        .capabilities = CAP(S, P, S, P, N, N),
        .notes = "Password-protected privacy features may block reads.",
    },
    {
        .name = "ST25TB",
        .family = "ST25TB",
        .frequency = "13.56 MHz",
        .technology = CardScopeTechNfc,
        .capabilities = CAP(S, S, S, P, N, N),
        .notes = "ST Type B family supported by firmware poller.",
    },

    /* LF RFID protocols from LFRFIDProtocol enum */
    {
        .name = "EM4100",
        .family = "EM410x",
        .frequency = "125 kHz",
        .technology = CardScopeTechRfid125,
        .capabilities = CAP(S, S, S, S, S, S),
        .notes = "Includes EM4100/16/32 variants in firmware.",
    },
    {
        .name = "HID / H10301",
        .family = "HID Prox",
        .frequency = "125 kHz",
        .technology = CardScopeTechRfid125,
        .capabilities = CAP(S, S, S, S, S, S),
        .notes = "Mapped to H10301 / HidGeneric / HidExGeneric in SDK.",
    },
    {
        .name = "AWID",
        .family = "AWID",
        .frequency = "125 kHz",
        .technology = CardScopeTechRfid125,
        .capabilities = CAP(S, S, S, S, S, S),
        .notes = "Supported by LFRFID worker auto-detect.",
    },
    {
        .name = "Indala26",
        .family = "Indala",
        .frequency = "125 kHz",
        .technology = CardScopeTechRfid125,
        .capabilities = CAP(S, S, S, S, S, S),
        .notes = "PSK protocol; auto mode covers ASK then PSK.",
    },
    {
        .name = "IOProx XSF",
        .family = "ioProx",
        .frequency = "125 kHz",
        .technology = CardScopeTechRfid125,
        .capabilities = CAP(S, S, S, S, S, S),
        .notes = "Supported decode/emulate in firmware.",
    },
    {
        .name = "Pyramid",
        .family = "Farpointe Pyramid",
        .frequency = "125 kHz",
        .technology = CardScopeTechRfid125,
        .capabilities = CAP(S, S, S, S, S, S),
        .notes = "Farpointe Pyramid via LFRFIDProtocolPyramid.",
    },
    {
        .name = "Viking",
        .family = "Viking",
        .frequency = "125 kHz",
        .technology = CardScopeTechRfid125,
        .capabilities = CAP(S, S, S, S, S, S),
        .notes = "Supported by firmware dictionary.",
    },
    {
        .name = "Jablotron",
        .family = "Jablotron",
        .frequency = "125 kHz",
        .technology = CardScopeTechRfid125,
        .capabilities = CAP(S, S, S, S, S, S),
        .notes = "Supported by firmware dictionary.",
    },
    {
        .name = "Paradox",
        .family = "Paradox",
        .frequency = "125 kHz",
        .technology = CardScopeTechRfid125,
        .capabilities = CAP(S, S, S, S, S, S),
        .notes = "Supported by firmware dictionary.",
    },
    {
        .name = "PAC/Stanley",
        .family = "PAC",
        .frequency = "125 kHz",
        .technology = CardScopeTechRfid125,
        .capabilities = CAP(S, S, S, S, S, S),
        .notes = "LFRFIDProtocolPACStanley.",
    },
    {
        .name = "Keri",
        .family = "Keri",
        .frequency = "125 kHz",
        .technology = CardScopeTechRfid125,
        .capabilities = CAP(S, S, S, S, S, S),
        .notes = "Supported by firmware dictionary.",
    },
    {
        .name = "Gallagher",
        .family = "Gallagher",
        .frequency = "125 kHz",
        .technology = CardScopeTechRfid125,
        .capabilities = CAP(S, S, S, S, S, S),
        .notes = "Supported by firmware dictionary.",
    },
    {
        .name = "Nexwatch",
        .family = "Nexwatch",
        .frequency = "125 kHz",
        .technology = CardScopeTechRfid125,
        .capabilities = CAP(S, S, S, S, S, S),
        .notes = "Supported by firmware dictionary.",
    },
    {
        .name = "Securakey",
        .family = "Securakey",
        .frequency = "125 kHz",
        .technology = CardScopeTechRfid125,
        .capabilities = CAP(S, S, S, S, S, S),
        .notes = "Supported by firmware dictionary.",
    },
    {
        .name = "GProx II",
        .family = "GProx",
        .frequency = "125 kHz",
        .technology = CardScopeTechRfid125,
        .capabilities = CAP(S, S, S, S, S, S),
        .notes = "Supported by firmware dictionary.",
    },
    {
        .name = "Noralsy",
        .family = "Noralsy",
        .frequency = "125 kHz",
        .technology = CardScopeTechRfid125,
        .capabilities = CAP(S, S, S, S, S, S),
        .notes = "Supported by firmware dictionary.",
    },
    {
        .name = "FDX-A / FDX-B",
        .family = "Animal ID",
        .frequency = "125 kHz",
        .technology = CardScopeTechRfid125,
        .capabilities = CAP(S, S, S, S, P, S),
        .notes = "Pet/animal tags; write depends on blank type.",
    },
    {
        .name = "Electra",
        .family = "Electra",
        .frequency = "125 kHz",
        .technology = CardScopeTechRfid125,
        .capabilities = CAP(S, S, S, S, S, S),
        .notes = "Supported by firmware dictionary.",
    },
    {
        .name = "Idteck",
        .family = "IDTECK",
        .frequency = "125 kHz",
        .technology = CardScopeTechRfid125,
        .capabilities = CAP(S, S, S, S, S, S),
        .notes = "LFRFIDProtocolIdteck.",
    },
};

size_t cardscope_protocol_database_count(void) {
    return COUNT_OF(cardscope_protocol_db);
}

const CardScopeProtocolInfo* cardscope_protocol_database_get(size_t index) {
    if(index >= COUNT_OF(cardscope_protocol_db)) {
        return NULL;
    }
    return &cardscope_protocol_db[index];
}

const CardScopeProtocolInfo* cardscope_protocol_database_find_nfc(NfcProtocol protocol) {
    const char* key = NULL;
    switch(protocol) {
    case NfcProtocolIso14443_3a:
        key = "ISO14443-3A";
        break;
    case NfcProtocolIso14443_3b:
        key = "ISO14443-3B";
        break;
    case NfcProtocolIso14443_4a:
        key = "ISO14443-4A";
        break;
    case NfcProtocolIso14443_4b:
        key = "ISO14443-4B";
        break;
    case NfcProtocolIso15693_3:
        key = "ISO15693-3";
        break;
    case NfcProtocolFelica:
        key = "FeliCa";
        break;
    case NfcProtocolMfUltralight:
        key = "MIFARE Ultralight / NTAG";
        break;
    case NfcProtocolMfClassic:
        key = "MIFARE Classic";
        break;
    case NfcProtocolMfPlus:
        key = "MIFARE Plus";
        break;
    case NfcProtocolMfDesfire:
        key = "MIFARE DESFire";
        break;
    case NfcProtocolSlix:
        key = "SLIX";
        break;
    case NfcProtocolSt25tb:
        key = "ST25TB";
        break;
    default:
        return NULL;
    }

    for(size_t i = 0; i < COUNT_OF(cardscope_protocol_db); i++) {
        if(strcmp(cardscope_protocol_db[i].name, key) == 0) {
            return &cardscope_protocol_db[i];
        }
    }
    return NULL;
}

const CardScopeProtocolInfo* cardscope_protocol_database_find_rfid(LFRFIDProtocol protocol) {
    const char* key = NULL;
    switch(protocol) {
    case LFRFIDProtocolEM4100:
    case LFRFIDProtocolEM410032:
    case LFRFIDProtocolEM410016:
        key = "EM4100";
        break;
    case LFRFIDProtocolElectra:
        key = "Electra";
        break;
    case LFRFIDProtocolH10301:
    case LFRFIDProtocolHidGeneric:
    case LFRFIDProtocolHidExGeneric:
        key = "HID / H10301";
        break;
    case LFRFIDProtocolIdteck:
        key = "Idteck";
        break;
    case LFRFIDProtocolIndala26:
        key = "Indala26";
        break;
    case LFRFIDProtocolIOProxXSF:
        key = "IOProx XSF";
        break;
    case LFRFIDProtocolAwid:
        key = "AWID";
        break;
    case LFRFIDProtocolFDXA:
    case LFRFIDProtocolFDXB:
        key = "FDX-A / FDX-B";
        break;
    case LFRFIDProtocolPyramid:
        key = "Pyramid";
        break;
    case LFRFIDProtocolViking:
        key = "Viking";
        break;
    case LFRFIDProtocolJablotron:
        key = "Jablotron";
        break;
    case LFRFIDProtocolParadox:
        key = "Paradox";
        break;
    case LFRFIDProtocolPACStanley:
        key = "PAC/Stanley";
        break;
    case LFRFIDProtocolKeri:
        key = "Keri";
        break;
    case LFRFIDProtocolGallagher:
        key = "Gallagher";
        break;
    case LFRFIDProtocolNexwatch:
        key = "Nexwatch";
        break;
    case LFRFIDProtocolSecurakey:
        key = "Securakey";
        break;
    case LFRFIDProtocolGProxII:
        key = "GProx II";
        break;
    case LFRFIDProtocolNoralsy:
        key = "Noralsy";
        break;
    default:
        return NULL;
    }

    for(size_t i = 0; i < COUNT_OF(cardscope_protocol_db); i++) {
        if(strcmp(cardscope_protocol_db[i].name, key) == 0) {
            return &cardscope_protocol_db[i];
        }
    }
    return NULL;
}

void cardscope_apply_capabilities(CardScopeResult* result) {
    furi_assert(result);
    const CardScopeProtocolInfo* info = NULL;

    if(result->technology == CardScopeTechNfc) {
        info = cardscope_protocol_database_find_nfc(result->nfc_protocol);
    } else if(result->technology == CardScopeTechRfid125) {
        info = cardscope_protocol_database_find_rfid(result->rfid_protocol);
    }

    if(info) {
        result->capabilities = info->capabilities;
        if(result->family_name[0] == '\0') {
            strlcpy(result->family_name, info->family, sizeof(result->family_name));
        }
        if(result->frequency[0] == '\0') {
            strlcpy(result->frequency, info->frequency, sizeof(result->frequency));
        }
        if(result->notes[0] == '\0' && info->notes) {
            strlcpy(result->notes, info->notes, sizeof(result->notes));
        }
    } else {
        result->capabilities = (CardScopeCapabilityMatrix){
            .detect = CardScopeCapabilityPartial,
            .read = CardScopeCapabilityNotSupported,
            .identify = CardScopeCapabilityPartial,
            .analyze = CardScopeCapabilityNotSupported,
            .write = CardScopeCapabilityNotSupported,
            .emulate = CardScopeCapabilityNotSupported,
        };
    }
}
