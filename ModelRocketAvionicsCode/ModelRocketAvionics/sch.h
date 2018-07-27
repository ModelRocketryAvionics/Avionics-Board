/*
 * sch.h
 *
 *  Created on: Jul 26, 2018
 *      Author: Michael Graves
 */

#ifndef SCH_H_
#define SCH_H_

//#############################################//
//               Scheduler Functions
//#############################################//
uint8_t GetTaskState(uint8_t taskID);
void SetTaskState(uint8_t taskID, uint8_t taskState);

uint8_t AddTask(uint8_t (*taskInit)(void), _Bool (*taskExec)(uint8_t), _Bool (*taskConditionFunction)(void));
uint8_t AddTaskCond(uint8_t (*taskInit)(void), _Bool (*taskExec)(uint8_t), _Bool *taskCondition);
uint8_t AddTaskTime(uint8_t (*taskInit)(void), _Bool (*taskExec)(uint8_t), uint32_t updatePeriod);

uint8_t InitScheduler();
void UpdateScheduler();

#if SYSTICK_ENABLE == 1
//#############################################//
//                SysTick Variables
//#############################################//
extern uint32_t currentTime = 0;

//#############################################//
//                SysTick Functions
//#############################################//
uint8_t InitSysTick();
void SysTickInterruptHandler(void);
#endif


#endif /* SCH_H_ */
