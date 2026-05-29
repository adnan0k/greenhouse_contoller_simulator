#include "Controller.h"
#include "Equipment.h"




int main() {

    // 1. Initialize Hardware (Indexes: 0=Fan, 1=Heater, 2=Pump)
    Equipment eqarr[] = {
        Equipment("Exhaust Fan", 800, 0),
        Equipment("Ceramic Heater", 1800, 1),
        Equipment("Water Pump", 1200, 2)
    };
    int equipmentCount = 3;

    Controller brain;

    cout << "========================================" << endl;
    cout << " GREENHOUSE SIMULATOR TICK INITIALIZED  " << endl;
    cout << "========================================" << endl;

    // 3. Create a simulated 16-bit environmental reading: HEATWAVE
    uint16_t mockMoisture = 8;              // Perfect moisture 
    uint16_t mockTemp = 38 << 4;            // 38 Degrees Celsius (Too hot!)
    uint16_t mockSunlight = 14 << 11;       // Very bright sunlight
    uint16_t mockError = 0 << 15;           
    
    EnvironmentalData tickData = { static_cast<uint16_t>(mockMoisture | mockTemp | mockSunlight | mockError) };

    // 4. Pass data to FSM and get commands
    ControllerOutputs commands = brain.systemController(tickData);
    
    cout << "[FSM STATUS]: " << brain.getState() << endl;
    cout << "----------------------------------------" << endl;

    // 5. Process Commands through Power Gatekeeper
    if (commands.emergencyStatus) {
        cout << "[CRITICAL] EMERGENCY STOP! Shutting down all hardware." << endl;
        for (int i = 0; i < equipmentCount; i++) eqarr[i].setOff();
    } else {
        if (commands.fanOn) TurnOnSensorRequest(eqarr[0], powerTrackingAlgo(eqarr, equipmentCount));
        else eqarr[0].setOff();

        if (commands.heaterOn) TurnOnSensorRequest(eqarr[1], powerTrackingAlgo(eqarr, equipmentCount));
        else eqarr[1].setOff();

        if (commands.pumpOn) TurnOnSensorRequest(eqarr[2], powerTrackingAlgo(eqarr, equipmentCount));
        else eqarr[2].setOff();
    }

    // 6. Update Hardware Register & Render Output
    uint8_t hardwareRegister = 0;
    cout << "EQUIPMENT STATE:" << endl;
    for (int i = 0; i < equipmentCount; i++) {
        if (eqarr[i].getIsOn()) {
            hardwareRegister |= eqarr[i].getBitMask();
        }
        cout << "- " << eqarr[i].getName() << ": " << (eqarr[i].getIsOn() ? "ON" : "OFF") 
             << " (" << eqarr[i].getPowerUsage() << "W)" << endl;
    }

    cout << "----------------------------------------" << endl;
    cout << "Total Load  : " << powerTrackingAlgo(eqarr, equipmentCount) << "W / " << MAX_POWER_OF_SYSTEM << "W" << endl;
    cout << "HW Register : " << bitset<8>(hardwareRegister) << endl;
    cout << "========================================" << endl;

    return 0;
}

