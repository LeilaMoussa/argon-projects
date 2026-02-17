#include "Particle.h"
// #include "dct.h"  // Gen 2 only, not available on Argon (Gen 3)

SYSTEM_MODE(MANUAL);

void setup() {
    // Gen 2 approach (Photon/Electron):
    // const uint8_t val = 0x01;
    // dct_write_app_data(&val, DCT_SETUP_DONE_OFFSET, 1);

    // Gen 3 approach (Argon):
    WiFi.clearCredentials();
    System.disable(SYSTEM_FLAG_STARTUP_LISTEN_MODE);
}