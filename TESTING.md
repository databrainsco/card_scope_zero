# CardScope Zero — Testing

## Environment

- SDK / firmware target: Official **1.5.1** via uFBT
- Host: macOS build with `ufbt`
- Device hardware validation: **pending** (no Flipper attached during initial development)

## Automated / build checks

| Test | Result |
|---|---|
| `ufbt` compile to FAP | See latest build log |
| Resource cleanup on exit (`*_free`, record close) | Implemented in `cardscope_zero_app_free` |
| Sequential antenna stop/start | Implemented in `card_detector` |

## Functional matrix

| # | Case | Status |
|---|---|---|
| 1 | NFC identification | Pending hardware |
| 2 | RFID identification | Pending hardware |
| 3 | Protocol classification | Code path present; pending hardware |
| 4 | Unknown card | Pending hardware |
| 5 | No response / timeout | Implemented (phase timeout → no card) |
| 6 | Read errors | NFC poller error event handled |
| 7 | Auth limitations reported | Classic/UL/DESFire mark partial/protected |
| 8 | Scan cancel (Back) | Stops workers in scan `on_exit` / cancel event |
| 9 | History save | FlipperFormat writer implemented |
| 10 | History load | FlipperFormat reader implemented |
| 11 | Memory free on exit | Implemented |
| 12 | Firmware 1.5.1 API compatibility | Headers matched against local SDK |

## Pending on device

- Real NTAG213/215/216 type strings from GET_VERSION
- Classic Mini/1K/4K without keys → PARTIAL
- EM4100 / HID / Indala / AWID samples
- History persistence across reboot with SD present
- Confirm 3 s NFC→RFID handoff feel on device

## Safety checks

- App never starts write/emulate workers in v0.1
- App does not run dict attacks or key recovery
