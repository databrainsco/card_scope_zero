# CardScope Zero — Protocol Support

Target firmware: **Flipper Zero Official 1.5.1** (uFBT release channel).

Capability states:

- **SUPPORTED** — available through official SDK APIs used by this app
- **PARTIAL** — detection/identification works; full dump or features need keys/conditions
- **NOT SUPPORTED** — not exposed as a complete path in this app / SDK usage

## NFC (13.56 MHz)

| Family | Detect | Read | Identify | Analyze | Write | Emulate | Notes |
|---|---|---|---|---|---|---|---|
| ISO14443-3A | SUPPORTED | SUPPORTED | SUPPORTED | PARTIAL | NOT SUPPORTED | PARTIAL | UID/ATQA/SAK |
| ISO14443-3B | SUPPORTED | SUPPORTED | SUPPORTED | PARTIAL | NOT SUPPORTED | NOT SUPPORTED | |
| ISO14443-4A/4B | SUPPORTED | SUPPORTED | SUPPORTED | PARTIAL | NOT SUPPORTED | PARTIAL | ISO-DEP |
| ISO15693-3 | SUPPORTED | SUPPORTED | SUPPORTED | PARTIAL | NOT SUPPORTED | NOT SUPPORTED | |
| FeliCa | SUPPORTED | PARTIAL | SUPPORTED | PARTIAL | NOT SUPPORTED | PARTIAL | IDm; services may need keys |
| MIFARE Ultralight / NTAG | SUPPORTED | PARTIAL | SUPPORTED | SUPPORTED | PARTIAL | SUPPORTED | Type via GET_VERSION; pwd/UL-C pages may be locked |
| MIFARE Classic Mini/1K/4K | SUPPORTED | PARTIAL | SUPPORTED | PARTIAL | PARTIAL | SUPPORTED | Sector data needs keys; no silent bypass |
| MIFARE Plus | SUPPORTED | PARTIAL | SUPPORTED | PARTIAL | NOT SUPPORTED | PARTIAL | |
| MIFARE DESFire | SUPPORTED | PARTIAL | SUPPORTED | PARTIAL | NOT SUPPORTED | NOT SUPPORTED | Free/version data when allowed |
| SLIX | SUPPORTED | PARTIAL | SUPPORTED | PARTIAL | NOT SUPPORTED | NOT SUPPORTED | Privacy password may block |
| ST25TB | SUPPORTED | SUPPORTED | SUPPORTED | PARTIAL | NOT SUPPORTED | NOT SUPPORTED | |

Subtypes identified when SDK data allows:

- Ultralight Origin, Ultralight C, UL EV1 (UL11/UL21)
- NTAG203 / 213 / 215 / 216 / I2C variants
- Classic Mini / 1K / 4K

CardScope never claims a precise commercial model when ATQA/SAK/version data is ambiguous.

## RFID 125 kHz

Protocols from `LFRFIDProtocol` in SDK 1.5.1:

EM4100 / EM4100-16 / EM4100-32, Electra, H10301, Idteck, Indala26, IOProx XSF, AWID, FDX-A/B, HidGeneric, HidExGeneric, Pyramid, Viking, Jablotron, Paradox, PAC/Stanley, Keri, Gallagher, Nexwatch, Securakey, GProx II, Noralsy.

| Capability | Status |
|---|---|
| Detect | SUPPORTED (`LFRFIDWorker` auto ASK/PSK) |
| Read / Identify | SUPPORTED (`ProtocolDict` decode) |
| Analyze | SUPPORTED (UID + rendered data) |
| Write | PARTIAL / SUPPORTED depending on blank (T5577/EM4305) — **not implemented in CardScope v0.1** |
| Emulate | SUPPORTED in firmware — **not implemented in CardScope v0.1** |

Naming note: “HID Prox” in product language maps to `H10301` / `HidGeneric` / `HidExGeneric` in firmware.

## Authentication limitations

- No dictionary attack, key recovery, or auth bypass.
- Protected Classic sectors / DESFire files / Ultralight password pages are reported as unread/protected.
- Security Overview is informational and does **not** auto-label a card “vulnerable” solely by technology.

## Antenna policy

NFC and LF RFID antennas are independent. Auto Detect runs **sequential** phases:

1. NFC scanner (+ poller on detect)
2. Stop NFC completely
3. LF RFID worker auto read

Only one radio path is active at a time.
