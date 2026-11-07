# Tests

Host-side unit tests for Flipper FAP modules are limited without a device/simulator.

Current approach:

1. **Build gate:** `ufbt` must produce `cardscope_zero.fap`
2. **API contract:** modules only call symbols present in SDK 1.5.1 headers
3. **Hardware suite:** documented in `TESTING.md` (pending on-device)

Optional future: add hosted stubs for `cardscope_result_*` formatting helpers.
