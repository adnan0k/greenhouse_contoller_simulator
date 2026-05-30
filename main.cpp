#include "Controller.h"
#include "Equipment.h"
#include <fstream>
#include <sstream>
#include <cmath>

int main()
{

    // 1. Initialize Hardware (Indexes: 0=Fan, 1=Heater, 2=Pump)
    Equipment eqarr[] = {
        Equipment("Exhaust Fan", 800, 0),
        Equipment("Ceramic Heater", 1800, 1),
        Equipment("Water Pump", 1200, 2)};
    int equipmentCount = 3;

    Controller brain;

    cout << "========================================" << endl;
    cout << " GREENHOUSE SIMULATOR  " << endl;
    cout << "========================================" << endl;

    ifstream dataFile("sensor_data.csv");
    string line;
    int lineCount = 1;

    if (!dataFile.is_open())
    {
        cout << "[ERROR] Could not open sensor_data.csv" << endl;
        return 1;
    }
    while (getline(dataFile, line))
    {

        // Skip the text header row or empty lines
        if (line.empty() || isalpha(line[0]))
        {
            continue;
        }

        // Parse the comma-separated data
        stringstream ss(line);
        string moistureStr, tempStr, lightStr;

        getline(ss, moistureStr, ',');
        getline(ss, tempStr, ',');
        getline(ss, lightStr, ',');

        // Convert the string text into actual integers
        int moisture = stoi(moistureStr);
        int temp = stoi(tempStr);
        int light = stoi(lightStr);

        cout << "\n[ READING " << lineCount << " ]: Moisture: " << moisture << " | Temp: " << temp << "C | Light: " << light << endl;
        cout << "----------------------------------------" << endl;

        // Pack the data safely (Handles negative temperatures perfectly)
        uint16_t packedMoisture = moisture & 0x000F;
        uint16_t tempMagnitude = abs(temp) & 0x3F;
        uint16_t tempSign = (temp < 0) ? 1 : 0;
        uint16_t packedTemp = ((tempSign << 6) | tempMagnitude) << 4;
        uint16_t packedSunlight = (light & 0x000F) << 11;
        uint16_t packedError = 0;
        if (moisture > 15 || moisture < 0 || temp < -63 || temp > 63 || light > 15 || light < 0) {
            packedError = 1 << 15;
        }

        EnvironmentalData tickData = {static_cast<uint16_t>(packedMoisture | packedTemp | packedSunlight | packedError)};

        // 5. FSM Processing
        ControllerOutputs commands = brain.systemController(tickData);
        cout << "[FSM STATUS]: " << brain.getState() << endl;

        // 6. Process Commands through Power Gatekeeper
        if (commands.emergencyStatus)
        {
            cout << "[CRITICAL] EMERGENCY STOP! Shutting down all hardware." << endl;
            for (int i = 0; i < equipmentCount; i++)
                eqarr[i].setOff();
        }
        else
        {
            if (commands.fanOn)
                TurnOnDeviceRequest(eqarr[0], powerTrackingAlgo(eqarr, equipmentCount));
            else
                eqarr[0].setOff();

            if (commands.heaterOn)
                TurnOnDeviceRequest(eqarr[1], powerTrackingAlgo(eqarr, equipmentCount));
            else
                eqarr[1].setOff();

            if (commands.pumpOn)
                TurnOnDeviceRequest(eqarr[2], powerTrackingAlgo(eqarr, equipmentCount));
            else
                eqarr[2].setOff();
        }

        // 7. Update Hardware Register & Render Output
        uint8_t hardwareRegister = 0;
        for (int i = 0; i < equipmentCount; i++)
        {
            if (eqarr[i].getIsOn())
            {
                hardwareRegister |= eqarr[i].getBitMask();
            }
            cout << "- " << eqarr[i].getName() << ": " << (eqarr[i].getIsOn() ? "ON" : "OFF")
                 << " (" << eqarr[i].getPowerUsage() << "W)" << endl;
        }

        cout << "Total Load  : " << powerTrackingAlgo(eqarr, equipmentCount) << "W / " << MAX_POWER_OF_SYSTEM << "W" << endl;
        cout << "HW Register : " << bitset<8>(hardwareRegister) << endl;

        lineCount++;
    }

    // Close the file when done
    dataFile.close();

    cout << "========================================" << endl;
    cout << " SIMULATION COMPLETE" << endl;

    return 0;
}
