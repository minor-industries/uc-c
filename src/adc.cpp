#include "adc.h"

float shModel(float r) {
    // https://www.thinksrs.com/downloads/programs/therm%20calc/ntccalibrator/ntccalculator.html

    const float A = 0.6875638699e-3f;
    const float B = 2.215599769e-4f;
    const float C = 0.8187742954e-7f;

    float lnR = std::log(r);
    float t_inv = A + B * lnR + C * std::pow(lnR, 3);

    float tK = 1.0f / t_inv;
    float tC = tK - 273.15f;
    return tC;
}

void readADC(Adafruit_ADS1115 *ads1115, Datum *D) {
    for (int i = 0; i < 4; ++i) {
        D->channels[i].counts = ads1115->readADC_SingleEnded(i);
    }

    for (int i = 0; i < 4; ++i) {
        D->channels[i].V = ads1115->computeVolts(D->channels[i].counts);
    }

    for (int i = 0; i < 4; ++i) {
        Channel *ch = &D->channels[i];
        String info;
        info += "t = " + String(millis()) + "ms";;
        info += " ";

        info += "channel = " + String(i);
        info += " ";

        info += "counts = " + String(ch->counts);
        info += " ";

        info += "v = " + String(ch->V, 4);
        Serial.println(info);
    }

    // actual temperature channels
    for (int i = 1; i < 3; ++i) {
        String summary;
        summary += "channel " + String(i);
        summary += " ";

        Channel *ch0 = &D->channels[0];
        Channel *ch = &D->channels[i];
        const float r_ref = 20000;

        float r = float(ch0->counts) / float(ch->counts) * r_ref - r_ref;

        if (r < 1e3 || r > 1e6) {
            ch->present = false;
            summary += "no probe";
            Serial.println(summary);
            continue;
        }

        ch->present = true;
        ch->T = shModel(r);
        float tF = ch->T * 9.0f / 5.0f + 32.0f;

        summary += "r = " + String(r / 1000.0) + "k";
        summary += " ";

        summary += "t = " + String(ch->T) + "C";
        summary += " ";

        summary += "t = " + String(tF) + "F";

        Serial.println(summary);
    }

    // battery
    {
        float v = ads1115->computeVolts(D->channels[3].counts);
        v *= 2; // voltage divider

        String info;
        info += "v_bat = ";
        info += String(v);
        Serial.println(info);
    }

    Serial.println("");
}
