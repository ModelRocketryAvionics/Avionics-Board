/*
 * sch.h
 *
 *  Created on: Jul 26, 2018
 *      Author: Newhb
 */

#ifndef SCH_H_
#define SCH_H_

//#############################################//
//               Scheduler Functions
//#############################################//

uint8_t AddTask(uint8_t (*taskInit)(void), _Bool (*taskExec)(uint8_t), _Bool (*taskConditionFunction)(void));
uint8_t AddTaskCond(uint8_t (*taskInit)(void), _Bool (*taskExec)(uint8_t), _Bool *taskCondition);
uint8_t AddTaskTime(uint8_t (*taskInit)(void), _Bool (*taskExec)(uint8_t), uint32_t updatePeriod);

uint8_t InitScheduler();
void UpdateScheduler();

//#############################################//
//                SysTick Functions
//#############################################//
#if SYSTICK_ENABLE == 1
uint8_t InitSysTick();
void SysTickInterruptHandler(void);
#endif


#endif /* SCH_H_ */
