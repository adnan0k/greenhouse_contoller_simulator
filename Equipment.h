#include <iostream>
#include<bitset>
using namespace std;


#define MAX_POWER_OF_SYSTEM 5000






class Equipment{

    private:
        string name;
        bool isOn;
        int power;
        int location;

    public:

    Equipment(string n, int p, int pos):name(n),power(p),location(pos),isOn(false){
        cout<<"Constructor called for equipment class to initialize the "<< getName() <<  " members"<<endl;
    }

    void setOn(){
        isOn = true;
        
    }

    void setOff(){
        isOn = false;

    }

    uint8_t getBitMask(){
        return (1<<location);
    }

   int getPowerUsage(){
       
        if(isOn){
            return power;
        }
        return 0;
    }

    int getPowerRating(){
        // returns the required wattage (used for the safety check)
        return power;
    }

    bool getIsOn(){
        return isOn;
    }

    string getName(){
        return name;
    }

};


int powerTrackingAlgo(Equipment* eqarr, int count){
    int totalPowerUsage=0;
    for(int i=0; i<count; i++){
            totalPowerUsage += eqarr[i].getPowerUsage();
    }
    return totalPowerUsage;
}



bool TurnOnDeviceRequest(Equipment& eq, int currentLoad){
    if(eq.getIsOn()){
        return true;
    }

    if(currentLoad + eq.getPowerRating() <= MAX_POWER_OF_SYSTEM){
        eq.setOn();
        cout << eq.getName() << " turned ON Successfully." << endl;
        return true;
    }else{
        cout<<"Cannot Turn On the " << eq.getName() << " The power limit will be exceeded from " << MAX_POWER_OF_SYSTEM << endl;
        return false;
    }
}
