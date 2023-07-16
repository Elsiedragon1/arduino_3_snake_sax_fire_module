#include <ModbusRTUSlave.h>

//  Modbus ID
const uint16_t id = 2;

//  Serial configuration
const uint32_t baud = 115200;
const uint8_t config = SERIAL_8E1;
const uint16_t bufferSize = 256;
const uint8_t dePin = 2;

const uint8_t flamethrowers = 5;

//  Modbus data structures
const uint8_t coils = flamethrowers + 1;

uint8_t buffer[bufferSize];
ModbusRTUSlave modbus(Serial, buffer, bufferSize, dePin);

bool fireAll = false;

//  Thing!
uint32_t currentTick = 0;

uint32_t flameOnTime = 150; // Half a second operation time for the flamethrowers is a good starting point
// This can be as low as 100-150 ms according to the DMX system paka uses

uint32_t flameStartTick[flamethrowers] = { 0 };

int8_t coilRead(uint16_t address)
{
    return false;
}

bool coilWrite(uint16_t address, bool data)
{
    if (address <= coils && address > 0)
    {
        if (data)
        {   
            if (address == 5 && fireAll)
            {
                for (uint8_t f = 0; f < flamethrowers; f++)
                {
                    flameStartTick[f] = currentTick;
                }
            } else {
                flameStartTick[address-1] = currentTick;
            }
            return true;
        }
        return true;
    }
    return false;
}

const bool LowOn = true;

uint32_t lastFlamethrowerTick = 0;
uint32_t flamethrowerInterval = 1000/60;    // 60fps

void updateFlamethrowers()
{
    if (currentTick - lastFlamethrowerTick >= flamethrowerInterval)
    {
        // Currently this will fire all the flames on start up!
        for (uint8_t n = 0; n < flamethrowers; n++)
        {
            if (currentTick - flameStartTick[n] >= flameOnTime)
            {
                //  LED OFF!
                if (LowOn)
                {
                    digitalWrite(3+n, HIGH);
                } 
                else
                {
                    digitalWrite(3+n, LOW);
                }

            } else {
                //  LED ON!
                if (LowOn)
                {
                    digitalWrite(3+n, LOW);
                }
                else
                {
                    digitalWrite(3+n, HIGH);
                }
            }
        }
    }
}

void setup() {
    //  Modbus Comms
        // TX
        // RX
        // RS485 Enable/Disable
    pinMode(2, OUTPUT);

    // Flamethrower relay pins
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(7, OUTPUT);

    if (LowOn) {
        digitalWrite(3, HIGH);
        digitalWrite(4, HIGH);
        digitalWrite(5, HIGH);
        digitalWrite(6, HIGH);
        digitalWrite(7, HIGH);
    } else {
        digitalWrite(3, LOW);
        digitalWrite(4, LOW);
        digitalWrite(5, LOW);
        digitalWrite(6, LOW);
        digitalWrite(7, LOW);
    }


    Serial.begin(baud, config);
    modbus.begin(id, baud, config);
    modbus.configureCoils(coils, coilRead, coilWrite);
}

void loop() {
    currentTick = millis();
    updateFlamethrowers();
    modbus.poll();
}
