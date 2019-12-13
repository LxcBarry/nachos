#include "Elevator.h"
Elevator::Elevator(char *debugName, int m_numFloors, int m_ID)
{
    ID = m_ID;
    name = debugName;
    numFloors = m_numFloors;		//一共numFloors层，从第0层到第numFloors – 1层
    currentFloor = 0;                   //电梯一开始永远在第0层，且向上开
    occupancy = 0;              
    upOrDown = UP;
    // alarm = new Alarm();
    PtrFloorBarriers_Up = new EventBarrier*[numFloors];   //每一层都有一个up_in事件栅栏
    PtrFloorBarriers_Down = new EventBarrier*[numFloors];   //每一层都有一个up_down事件栅栏
    PtrFloorBarriers_Out = new EventBarrier*[numFloors];   //每一层都有一个out事件栅栏
    floorToStop_Up = new bool[numFloors];		          //记录了哪一层需要停靠
    floorToStop_Out = new bool[numFloors];
    floorToStop_Down = new bool[numFloors];

    for(int i = 0; i < numFloors; i++)
    {
        floorToStop_Up[i] = false;
        floorToStop_Out[i] = false;
	floorToStop_Down[i] = false;
        PtrFloorBarriers_Up[i] = new EventBarrier;
	PtrFloorBarriers_Down[i] = new EventBarrier;
	PtrFloorBarriers_Out[i] = new EventBarrier;
    }
}

Elevator::~Elevator()
{
    for(int i = 0; i < numFloors; i++)
    {
        delete PtrFloorBarriers_Up[i];  
	delete PtrFloorBarriers_Down[i]; 
	delete PtrFloorBarriers_Out[i]; 
    }
    delete [] PtrFloorBarriers_Up;
    delete [] PtrFloorBarriers_Down;
    delete [] PtrFloorBarriers_Out;
    delete [] floorToStop_Up;
    delete [] floorToStop_Down;
    delete [] floorToStop_Out;
}

void Elevator::Move()
{
    if(upOrDown == UP)
    {
	for(int i = currentFloor;i < numFloors;i++)
        {
            printf("----Elevator %d has arrived floor %d up----\n",ID,i);
            // alarm->Pause(10);
            if((floorToStop_Up[i] == true)||(floorToStop_Out[i] == true))
                {
            currentFloor = i;
            openDoors();
                closeDoors();
            floorToStop_Up[currentFloor] = false;
            floorToStop_Out[currentFloor] = false;
            }	
	}
        currentFloor = numFloors - 1;
	upOrDown = DOWN; 
    }
    else if(upOrDown == DOWN)
    {
	for(int i = currentFloor;i > 0;i--)
        {
            printf("----Elevator %d has arrived floor %d down----\n",ID,i);
            // alarm->Pause(10);
            if((floorToStop_Down[i] == true)||(floorToStop_Out[i] == true))
                {
            //alarm->Pause(10);
                currentFloor = i;
                openDoors();
                    closeDoors();
                floorToStop_Down[currentFloor] = false;
                floorToStop_Out[currentFloor] = false;
	            }	
	}
        currentFloor = 0;
	upOrDown = UP; 
    }
}

void Elevator::openDoors()
{
    printf("Elevator %d has opened door on the floor %d\n",ID,currentFloor);
    if((upOrDown == UP)&&(floorToStop_Up[currentFloor] == true))	PtrFloorBarriers_Up[currentFloor]->Signal();
    else if((upOrDown == DOWN)&&(floorToStop_Down[currentFloor] == true))	PtrFloorBarriers_Down[currentFloor]->Signal();

    if(floorToStop_Out[currentFloor] == true)  PtrFloorBarriers_Out[currentFloor]->Signal();
}

void Elevator::closeDoors()
{
    printf("Elevator %d has closed door on the floor %d\n",ID,currentFloor);
}


//表示既标记floorToStop而有阻塞进程，用于以下情况：电梯在0楼，A要从5层到10层，那么A会调用RequestFloor(10)，不到10层永远阻塞
void Elevator::RequestFloor(int floor)
{
    floorToStop_Out[floor] = true;
    PtrFloorBarriers_Out[floor]->Wait();
}

//该complete是与Building::AwaitDown/Up(int fromFloor)中的wait()对应。
bool Elevator::Enter(int dir)
{
    if(dir == upOrDown) 
    {
        if(dir == UP) PtrFloorBarriers_Up[currentFloor]->Complete();
            else if (dir == DOWN) PtrFloorBarriers_Down[currentFloor]->Complete();
            occupancy++;
        return true;
    }
    else return false;
}

//该complete是与Elevator::RequestFloor(int floor)中的wait()对应。
void Elevator::Exit()
{
    occupancy--;
    PtrFloorBarriers_Out[currentFloor]->Complete();
} 

bool Elevator::Used()
{
    for(int i=0;i < numFloors;i++)
    {
	if(floorToStop_Up[i] == true)   	return true;
	if(floorToStop_Down[i] == true)		return true;
	if(floorToStop_Out[i] == true)          return true;
    }
    return false;
}

/*---------------Building--------------------------*/
Building::Building(char *debugname, int m_numFloors, int m_numElevators) 
{ 
    name = debugname; 
    numFloors = m_numFloors; 
    numElevators = m_numElevators; 
    elevator=new Elevator("E1",m_numFloors,1); 
} 


Building::~Building()
{
    delete elevator;
}

void Building::Begin() 
{
    while(elevator->Used()) 
    { 
	elevator->Move(); 
	currentThread->Yield(); 
    } 
} 

void Building::CallUp(int fromFloor) 
{
    elevator->floorToStop_Up[fromFloor] = true;
} 

void Building::CallDown(int fromFloor) 
{
    elevator->floorToStop_Down[fromFloor] = true;
}
 
Elevator * Building::AwaitUp(int fromFloor) // wait for elevator arrival & going up 
{ 
 
    elevator->PtrFloorBarriers_Up[fromFloor]->Wait();
    return elevator; 
} 

Elevator * Building::AwaitDown(int fromFloor) // ... down 
{ 
    elevator->PtrFloorBarriers_Down[fromFloor]->Wait();
    return elevator; 
} 
