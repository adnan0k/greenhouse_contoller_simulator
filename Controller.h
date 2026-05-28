/*
    * Controller.h
    * Author: Wania Amir
    * Created on: 2024-06-01
*/ 

#include <iostream>
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
        
        /* FINITE STATE MACHINE
            Optimal thresholds for moisture, temperature, and sunlight are defined based on typical plant requirements.
            Temperature is best between 10°C and 32°C.
            Soil moisture should be above 20% (3 out of 15) and below 80% (12 out of 15).
            Sunlight should be between 40% (6 out of 15) to 60% (9 out of 15) of full sunlight.
            Soil moisture and sunlight are rated from 0-15, as they are represented as 4-bit values.
        */

        switch (state) {
            case IDLE:
                if (moisture < 3) 
                    state = IRRIGATION;
                else if (temperature > 32 || temperature < 10 || sunlight < 12)
                    state = CRITICAL_CLIMATE;
                break;

            case IRRIGATION:
                if (moisture >= 12) {  
                    if (temperature > 32  || temperature < 10 || sunlight < 12)
                        state = CRITICAL_CLIMATE;
                    else
                        state = IDLE;
                }
                break;

            case CRITICAL_CLIMATE:
                if (temperature <= 32 && temperature >= 10 && sunlight <= 6)  
                    state = IDLE;
                break;
                
            case EMERGENCY_STOP:
                outputs.emergencyStatus = true;
                break;
        }

        // Output Control based on state
        // Water Pump Control
        if (state == IRRIGATION) {
            if (temperature <= 0) {
                outputs.pumpOn = false;
                cout << "Pump is turned OFF due to freezing temperatures." << endl;
            }
            else
                outputs.pumpOn = true;
        }

        // Heater and Fan Control
        if (state == CRITICAL_CLIMATE) {
            if (temperature < 10)
                outputs.heaterOn = true;
            else if (temperature > 32 || sunlight > 9) 
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