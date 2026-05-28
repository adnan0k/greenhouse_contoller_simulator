/*
    * Controller.h
    * Author: Wania Amir
    * Created on: 2024-06-01
*/ 

#include <string>
#include <cstdint>
using namespace std;

// State Definitions
const int IDLE = 0;
const int IRRIGATION = 1;
const int CRITICAL_CLIMATE = 2;
const int EMERGENCY_STOP = 3;

struct EnvironmentalData {
    uint16_t Data; 
};

struct ControllerOutputs {
    bool pumpOn;
    bool fanOn;
    bool heaterOn;
    bool emergencyStatus;
};

class Controller {
private:
    int state;

public:
    Controller () {
        state = IDLE;
    }

    ControllerOutputs systemController(EnvironmentalData data) {
        ControllerOutputs outputs = {false, false, false, false};
        
        // Environment Data Extraction
        // Check for error (bit 15)
        uint16_t checkError = (data.Data & 0x8000) >> 15;
        if (checkError) {
            state = EMERGENCY_STOP;
        }

        // Extracting values using masks and shifts
        int moisture = data.Data & 0x000F; // Bits 0-3
        int temperature = (data.Data & 0x07F0) >> 4; // Bits 4-10
        int sunlight = (data.Data & 0x7800) >> 11; // Bits 11-24

        // Signed bit handling for temperature (7 bits)
        int tempSign = (temperature & 0x40) >> 6; // Bit 6 is the sign bit
        int tempValue = temperature & 0x3F; // Bits 0-5 are the magnitude
        
        // Converting to negative if sign bit is set
        if (tempSign == 1)
            temperature = -tempValue; 

        // Output Control based on state
        // Water Pump Control
        if (state == IRRIGATION) {
            if (temperature <= 0)
                outputs.pumpOn = false;
            else
                outputs.pumpOn = true;
        }

        // Heater and Fan Control
        if (state == CRITICAL_CLIMATE) {
            if (temperature < 10)
                outputs.heaterOn = true;
            else if (temperature > 35 || sunlight > 12)
                outputs.fanOn = true;
        }

        return outputs;
    }

    string getState() {
        switch (state) {
            case IDLE:
                return "Idle";
            case IRRIGATION:
                return "Irrigation";
            case CRITICAL_CLIMATE:
                return "Critical Climate";
            case EMERGENCY_STOP:
                return "Emergency Stop";
            default:
                return "Unknown State";
        }
    }
};