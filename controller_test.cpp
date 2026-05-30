#include <iostream>
#include <cstdint>
#include <cmath>
#include "Controller.h"

// Corrected Mock Packing to match the exact 4-bit, 6-bit, 1-bit, 4-bit, 1-bit boundaries
uint16_t createMockPacket(int moisture, int temp, int light, int error) {
    uint16_t packedMoisture = (moisture & 0x000F);                   // Bits 0-3 (Max value 15)
    
    uint16_t signBit = (temp < 0) ? 1 : 0;
    uint16_t magnitude = std::abs(temp) & 0x003F;                    // 6 bits for magnitude (Max 63)
    uint16_t packedTemp = ((signBit << 6) | magnitude) << 4;         // Positioned at bits 4-10

    uint16_t packedLight = (light & 0x000F) << 11;                  // Bits 11-14 (Max value 15)
    uint16_t errorBit = (error & 0x0001) << 15;                     // Bit 15

    return packedMoisture | packedTemp | packedLight | errorBit;
}

// Helper function to cleanly print results for each isolated test case
void verifyControllerOutput(Controller& controller, ControllerOutputs result, uint16_t packedData) {
    // Unpack variables locally just for verifying the test print output matches
    int moisture = (packedData & 0x000F);
    int rawTempField = (packedData & 0x07F0) >> 4;
    int signBit = (rawTempField & 0x0040) >> 6;
    int magnitude = (rawTempField & 0x003F);
    int temp = (signBit == 1) ? -magnitude : magnitude;
    int light = (packedData & 0x7800) >> 11;
    int error = (packedData & 0x8000) >> 15;

    std::cout << "  [Inputs] -> Moisture: " << moisture << " | Temperature: " << temp << "C | Light: " << light << " | Error: " << error << "\n";
    std::cout << "  [State ] -> " << controller.getState() << "\n";
    std::cout << "  [Pins  ] -> Pump: " << (result.pumpOn ? "ON" : "OFF") 
              << " | Fan: " << (result.fanOn ? "ON" : "OFF") 
              << " | Heater: " << (result.heaterOn ? "ON" : "OFF") << "\n";
    
    // Explicit, safe print checks based on the currently evaluated unpacked data
    if (controller.getState() == "IRRIGATION_ACTIVE" && temp <= 0) {
        std::cout << "  [ALERT ] -> Pump is forced OFF due to freezing temperatures!\n";
    }
    if (controller.getState() == "CLIMATE_CRITICAL" && light > 12) {
        std::cout << "  [ALERT ] -> Ventilation Fan activated due to high solar glare!\n";
    }
    std::cout << "--------------------------------------------------\n";
}

int main() {
    EnvironmentalData mockData;
    ControllerOutputs result;

    std::cout << "==================================================\n";
    std::cout << "          RUNNING CONTROLLER UNIT TESTS          \n";
    std::cout << "==================================================\n";

    // ------------------------------------------------------------------------
    // TEST 1: Normal Conditions
    // ------------------------------------------------------------------------
    Controller controller1;
    mockData.Data = createMockPacket(10, 22, 7, 0);
    result = controller1.systemController(mockData);
    std::cout << "Test 1: Normal Operation Bounds\n";
    verifyControllerOutput(controller1, result, mockData.Data);

    // ------------------------------------------------------------------------
    // TEST 2: Dry Soil (Should trigger Irrigation)
    // ------------------------------------------------------------------------
    Controller controller2;
    mockData.Data = createMockPacket(2, 20, 7, 0);
    result = controller2.systemController(mockData);
    std::cout << "Test 2: Dry Soil Trigger\n";
    verifyControllerOutput(controller2, result, mockData.Data);

    // ------------------------------------------------------------------------
    // TEST 3: Dry soil but Freezing temperatures
    // ------------------------------------------------------------------------
    Controller controller3;
    mockData.Data = createMockPacket(2, -5, 5, 0); // -5 Degrees Celsius
    result = controller3.systemController(mockData);
    std::cout << "Test 3: Freezing Pipe Safety\n";
    verifyControllerOutput(controller3, result, mockData.Data);

    // ------------------------------------------------------------------------
    // TEST 4: Dry Soil & High Temperature 
    // ------------------------------------------------------------------------
    Controller controller4;
    mockData.Data = createMockPacket(2, 50, 15, 0);
    result = controller4.systemController(mockData);
    std::cout << "Test 4: Edge Case - Dry Soil & High Temperature\n";
    verifyControllerOutput(controller4, result, mockData.Data);

    // ------------------------------------------------------------------------
    // TEST 5: Blazing Midday Glare (Should trigger Fan via Light data)
    // ------------------------------------------------------------------------
    Controller controller5;
    mockData.Data = createMockPacket(10, 24, 14, 0); // Light intensity 14/15
    result = controller5.systemController(mockData);
    std::cout << "Test 5: High Solar Glare Proactive Cooling\n";
    verifyControllerOutput(controller5, result, mockData.Data);

    // ------------------------------------------------------------------------
    // TEST 6: Dry soil, low temperatures, and high light (worst-case scenario)
    // ------------------------------------------------------------------------
    Controller controller6;
    mockData.Data = createMockPacket(2, 9, 14, 0); // Dry soil, 9C, High light
    result = controller6.systemController(mockData);
    std::cout << "Test 6: Worst-Case Scenario (Dry Soil, Low Temperature, High Light)\n";
    verifyControllerOutput(controller6, result, mockData.Data);

    // ------------------------------------------------------------------------
    // TEST 7: Hardware Sensor Fault (Emergency Shutdown)
    // ------------------------------------------------------------------------
    Controller controller7;
    mockData.Data = createMockPacket(16, 22, 5, 1); // Error bit set to 1
    result = controller7.systemController(mockData);
    std::cout << "Test 7: Hardware Sensor Fault Lockdown\n";
    verifyControllerOutput(controller7, result, mockData.Data);

    return 0;
}