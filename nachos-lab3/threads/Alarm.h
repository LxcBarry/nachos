#pragma once
#include "copyright.h" 
#include "list.h" 
#include "system.h"

 
class Alarm { 
   public: 
     Alarm(); 
     ~Alarm(); 
	void Pause(int howLong); //设定睡眠时间，将自身和唤醒时刻存入队列
	void Call(); //发生时钟中断的时候调用该函数，将队列中已经到时的进程调出唤醒
   private: 
	List *queue;	 
	int waiters; 
    
} ;
