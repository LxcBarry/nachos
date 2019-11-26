/*

Here are the method signatures for the Elevator and Building classes.
You should feel free to add to these classes, but do not change the
existing interfaces.

*/
#ifndef ELEVATOR_H
#define ELEVATOR_H
#include "EventBarrier.h"
#include "Alarm.h"

#define UP   0
#define DOWN 1

class Elevator {
   public:
     Elevator(char *debugName, int m_numFloors, int m_ID);
     ~Elevator();
     char *getName() { return name; }
   
     // elevator control interface: called by Elevator thread
     void openDoors();                //   signal exiters and enterers to action
     void closeDoors();               //   after exiters are out and enterers are in
   
     // elevator rider interface (part 1): called by rider threads.
     bool Enter(int dir);                    //   get in
     void Exit();                     //   get out (iff destinationFloor)
     void RequestFloor(int floor);    //   tell the elevator our destinationFloor
   
     // insert your methods here, if needed
     void Move();
     bool Used();
     
     EventBarrier ** PtrFloorBarriers_Up;
     EventBarrier ** PtrFloorBarriers_Down;
     EventBarrier ** PtrFloorBarriers_Out;
     bool * floorToStop_Up;
     bool * floorToStop_Down;
     bool * floorToStop_Out;
   private:
     char *name;
   
     int currentFloor;           // floor where currently stopped
     int occupancy;              // how many riders currently onboard
     int numFloors;
     // insert your data structures here, if needed
     int ID;
     Alarm *alarm;
     int upOrDown;
};
   
class Building 
{
   public:
     Building(char *debugname, int m_numFloors, int m_numElevators) ;
     ~Building();
     char *getName() { return name; }
   	
     // elevator rider interface (part 2): called by rider threads
     void CallUp(int fromFloor);      //   signal an elevator we want to go up
     void CallDown(int fromFloor);    //   ... down
     Elevator * AwaitDown(int fromFloor);
     Elevator * AwaitUp(int fromFloor);
     void Begin();      
     
     Elevator *elevator;         // the elevators in the building (array)
   private:
     char *name;
     int numFloors;
     int numElevators;
     
   
     // insert your data structures here, if needed
};

#endif

