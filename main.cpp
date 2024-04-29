#include "RFIDReader.h"

RFIDReader rfidReader(15, 0);

void setup() {
    rfidReader.setup();
}

void loop() {
    rfidReader.loop();
}
