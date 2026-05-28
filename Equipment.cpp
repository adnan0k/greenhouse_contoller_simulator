#include <iostream>
#include<bitset>
using namespace std;


#define MAX_POWER_OF_SYSTEM 5000



struct Sensor{
  
    string sensorType;
    float readingValue;
    

};



class Equipment{

    private:
        string name;
        bool isOn;
        int power;
        int location;
        int8_t positionBits = 0b00000000;
        Sensor s;

    public:

    Equipment(string n, int p, int pos, string sType, float val ):name(n),power(p),location(pos),isOn(false), s{sType,val}{
        cout<<"Constructor called for equipment class to initialize members"<<endl;
    }

    void setOn(){
        isOn = true;
        positionBits |= 1<<location;
        
    }

    void setOff(){
        isOn = false;
        positionBits &= ~(1 << location);

    }

    int8_t getPositionBits(){
        return positionBits;
    }

    int getPowerUsage(){
        if(isOn == true){
            return power;
        }
        return 0;
    }

    void setSensorValue(float val){
        s.readingValue = val;
    }

};

int powerTrackingAlgo(Equipment* eqarr, int count){
    int totalPowerUsage=0;
    for(int i=0; i<count; i++){
        if(totalPowerUsage < MAX_POWER_OF_SYSTEM){

            totalPowerUsage += eqarr[i].getPowerUsage();
        }else{
            cout<<"Power threshold will exceed so cannot turn this sensor on";
            eqarr[i].setOff();
        }
    }
    return totalPowerUsage;
}


int main(){




    return 0;
}

