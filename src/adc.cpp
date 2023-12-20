#include "adc.h"
#include "util.h"

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

    // actual temperature channels
    for (int i = 1; i < 3; ++i) {
        Channel *ch0 = &D->channels[0];
        Channel *ch = &D->channels[i];
        const float r_ref = 20000;

        float r = float(ch0->counts) / float(ch->counts) * r_ref - r_ref;

        if (r < 1e3 || r > 1e6) {
            ch->present = false;
            continue;
        }

        ch->present = true;
        ch->R = r;
        ch->T = shModel(r);
    }

    // battery
    {
        Channel *ch = &D->channels[3];
        float v = ads1115->computeVolts(D->channels[3].counts);
        v *= 2; // voltage divider
        ch->V = v;
    }
}

void printADC(Datum *D) {
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
        Channel *ch = &D->channels[i];
        String summary;
        summary += "channel " + String(i);
        summary += " ";

        if (!ch->present) {
            summary += "no probe";
            Serial.println(summary);
            continue;
        }

        float tF = ch->T * 9.0f / 5.0f + 32.0f;

        summary += "r = " + String(ch->R / 1000.0) + "k";
        summary += " ";

        summary += "t = " + String(ch->T) + "C";
        summary += " ";

        summary += "t = " + String(tF) + "F";

        Serial.println(summary);
    }

    // battery
    {
        Channel *ch = &D->channels[3];
        String info;
        info += "v_bat = ";
        info += String(ch->V);
        Serial.println(info);
    }

    Serial.println("");
}

void sendTemp(RFM69 *radio, uint16_t toAddr, Channel *ch, char const *desc) {
    if (!ch->present) {
        return;
    }
    uint8_t buf[1 + 4 + 16];
    memset(buf, 0, sizeof(buf));

    uint8_t *pos = buf;

    pos[0] = 0x02;
    pos += 1;

    packFloat(ch->T, pos);
    pos += 4;

    memcpy(pos, desc, strlen(desc)); // TODO: limit length
    radio->send(toAddr, buf, sizeof(buf));
}

void sendBat(RFM69 *radio, uint16_t toAddr, Channel *ch, char const *desc) {
    uint8_t buf[1 + 4 + 4 + 4 + 1 + 16];
    memset(buf, 0, sizeof(buf));

    uint8_t *pos = buf;
    pos[0] = 0x03;
    pos += 1;

    packFloat(ch->V, pos);
    pos += 4;

    // TODO: consider NaN, or some way to specify absence
    packFloat(0, pos);
    pos += 4;

    packFloat(0, pos);
    pos += 4;

    pos[0] = 0;
    pos += 1;

    memcpy(pos, desc, strlen(desc)); // TODO: limit length

    radio->send(toAddr, buf, sizeof(buf));
}


void sendADC(RFM69 *radio, uint16_t toAddr, Datum *D) {
    sendTemp(radio, toAddr, &D->channels[1], "bbq01-bbq");
    sendTemp(radio, toAddr, &D->channels[2], "bbq01-meat");
    sendBat(radio, toAddr, &D->channels[3], "bbq01");
}


