#include <Arduino.h>
#include <Wire.h>


bool detectI2c(TwoWire *wire) {
    byte error, address;

    for (address = 1; address < 127; address++) {
        Serial.println(address);

        // The i2c_scanner uses the return value of
        // the Write.endTransmisstion to see if
        // a device did acknowledge to the address.
        wire->beginTransmission(address);
        error = wire->endTransmission();

        if (error == 0) {
            return true;
        } else if (error == 4) {
            // pass
        }
    }

    return false;
}