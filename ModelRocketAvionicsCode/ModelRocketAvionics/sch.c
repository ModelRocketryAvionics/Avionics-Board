/*
 * sch.c
 *
 *  Created on: Jul 25, 2018
 *      Author: Michael Graves
 */

#include "stdint.h"
#include "stdbool.h"

#include "inc/tm4c123gh6pm.h"

#include "driverlib/systick.h"
#include "driverlib/interrupt.h"

#include "sch.h"

//#############################################//
//                Scheduler Defines
//#############################################//
#define MAX_NUMBER_OF_TASKS 16

#define TASK_STATE_INIT 0   //Task needs initialization
#define TASK_STATE_IDLE 1   //Task is idle and waiting for taskCondition
#define TASK_STATE_EXEC 2   //Task has executed and wasn't set back to idle
#define TASK_STATE_DEAD 3   //Task has died (will not run again)

#define TASK_CONDITION_TYPE_BOOL 0
#define TASK_CONDITION_TYPE_TIMER 1
#define TASK_CONDITION_TYPE_FUNCTION 2

//Set this to 0 to setup SysTick elsewhere, will break TimerTasks
#define SYSTICK_ENABLE 1
#if SYSTICK_ENABLE == 1
#define SYSTICK_PERIOD 80000    //80,000 = 1 millisecond @ 80MHz

//Declare SysTick Functions explicitly for use in earlier functions
// Unfortunately this is a bit messy up here, plz forgive me!
uint8_t InitSysTick(void);
void SysTickInterruptHandler(void);
#endif


//#############################################//
//               Scheduler Structures
//#############################################//
struct TaskStructure {
    uint8_t TaskID;
    uint8_t TaskState;
    uint8_t TaskConditionType;
    void *TaskCondition;
    uint8_t (*TaskInit)(void);
    _Bool (*TaskExec)(uint8_t);
};

struct TaskTimerStructure {
    _Bool TaskFlag;
    uint32_t TaskPeriod;
    uint32_t TaskLastTime;
};


//#############################################//
//               Scheduler Variables
//#############################################//

uint8_t numOfTasks = 0;
struct TaskStructure TaskList[MAX_NUMBER_OF_TASKS];

uint8_t numOfTaskTimers = 0;
struct TaskTimerStructure TaskTimerList[MAX_NUMBER_OF_TASKS];


//#############################################//
//               Scheduler Functions
//#############################################//

/*
 * GetTaskState(taskID)
 *  Returns the current TaskState of taskID where:
 *      TASK_STATE_INIT 0
 *      TASK_STATE_IDLE 1
 *      TASK_STATE_EXEC 2
 *      TASK_STATE_DEAD 3
 */
uint8_t GetTaskState(uint8_t taskID) {
    return TaskList[taskID].TaskState;
}

/*
 * SetTaskState(taskID)
 *  Sets the current TaskState of taskID to taskState where:
 *      TASK_CONDITION_TYPE_BOOL 0
 *      TASK_CONDITION_TYPE_TIMER 1
 *      TASK_CONDITION_TYPE_FUNCTION 2
 */
void SetTaskState(uint8_t taskID, uint8_t taskState) {
    TaskList[taskID].TaskState = taskState;
}

/*
 * AddTaskCond()
 *  Adds a Conditional Task
 *  Returns the taskID
 */
uint8_t AddTask(uint8_t (*taskInit)(void), _Bool (*taskExec)(uint8_t), _Bool (*taskConditionFunction)(void)) {
    //Add the task to the task list and set to initialize state
    TaskList[numOfTasks].TaskID = numOfTasks;
    TaskList[numOfTasks].TaskState = TASK_STATE_INIT;
    TaskList[numOfTasks].TaskConditionType = TASK_CONDITION_TYPE_FUNCTION;
    TaskList[numOfTasks].TaskCondition = taskConditionFunction;
    TaskList[numOfTasks].TaskInit = taskInit;
    TaskList[numOfTasks].TaskExec = taskExec;

    numOfTasks++;

    //Return the taskID that was just created
    return numOfTasks - 1;
}

/*
 * AddTaskCond()
 *  Adds a Conditional Task
 *  Returns the taskID
 */
uint8_t AddTaskCond(uint8_t (*taskInit)(void), _Bool (*taskExec)(uint8_t), _Bool *taskCondition) {
    //Add the task to the task list and set to initialize state
    TaskList[numOfTasks].TaskID = numOfTasks;
    TaskList[numOfTasks].TaskState = TASK_STATE_INIT;
    TaskList[numOfTasks].TaskConditionType = TASK_CONDITION_TYPE_BOOL;
    TaskList[numOfTasks].TaskCondition = taskCondition;
    TaskList[numOfTasks].TaskInit = taskInit;
    TaskList[numOfTasks].TaskExec = taskExec;

    numOfTasks++;

    //Return the taskID that was just created
    return numOfTasks - 1;
}

/*
 * AddTaskTime()
 *  Adds a Time Based Task
 *  Returns the TaskID
 */
uint8_t AddTaskTime(uint8_t (*taskInit)(void), _Bool (*taskExec)(uint8_t), uint32_t updatePeriod) {
    //Create a TaskTimer for the task
    TaskTimerList[numOfTaskTimers].TaskFlag = false;
    TaskTimerList[numOfTaskTimers].TaskPeriod = updatePeriod;
    TaskTimerList[numOfTaskTimers].TaskLastTime = 0;

    //Add the task to the task list and set to initialize state using the above TaskFlag..
    // as the taskCondition. (Technically this is no different from AddTaskCond(), just..
    // separate to set TaskConditionType to Timer.
    TaskList[numOfTasks].TaskID = numOfTasks;
    TaskList[numOfTasks].TaskState = TASK_STATE_INIT;
    TaskList[numOfTasks].TaskConditionType = TASK_CONDITION_TYPE_TIMER;
    TaskList[numOfTasks].TaskCondition = &(TaskTimerList[numOfTaskTimers].TaskFlag);
    TaskList[numOfTasks].TaskInit = taskInit;
    TaskList[numOfTasks].TaskExec = taskExec;

    numOfTasks++;

    //Return the taskID that was just created
    return numOfTasks - 1;
}

/*
 * InitScheduler()
 *  Initializes the Scheduler, this function configures SysTick timer if enabled.
 */
uint8_t InitScheduler() {

#if SYSTICK_ENABLE == 1
    InitSysTick();
#endif

    return 0;
}

/*
 * UpdateScheduler()
 *  Updates the Tasks that require updates (TaskCondition), run this as often as you can..
 *   i.e in main()'s while(1)
 *
 */
void UpdateScheduler() {
    uint8_t currentTask = 0;
    for (currentTask = 0; currentTask < numOfTasks; currentTask++) {
        switch(TaskList[currentTask].TaskState) {
        case TASK_STATE_INIT:
            //Task hasn't been initialized, call TaskInit() function
            // This function call returns an uint8_t that isn't handled. In the future..
            //  this could allow error codes or other messages to be returned/printed.
            (*TaskList[currentTask].TaskInit)();
            break;

        case TASK_STATE_IDLE:
            //Task is waiting for its taskCondition to be true.

            //Depending on the TaskConditionType we need to cast the void* to the correct..
            // pointer type.
            switch(TaskList[currentTask].TaskConditionType) {
            case TASK_CONDITION_TYPE_BOOL: case TASK_CONDITION_TYPE_TIMER:
                //TaskConditionType is Bool (or Timer)

                //Cast the TaskCondition from void* to a _Bool* and check its value.
                if(*((_Bool*)TaskList[currentTask].TaskCondition)) {
                    //TaskCondition is true..

                    //Cast the TaskCondition from void* to _Bool* and set to false.
                    *((_Bool*)TaskList[currentTask].TaskCondition) = false;

                    //Execute the TaskExec() function with the TaskID as a parameter, the..
                    // function should return true to set it's state back to idle, or..
                    // handle it manually.
                    if((*TaskList[currentTask].TaskExec)(TaskList[currentTask].TaskID)) {
                        TaskList[currentTask].TaskState = TASK_STATE_IDLE;
                    } else {
                        TaskList[currentTask].TaskState = TASK_STATE_EXEC;
                    }
                }
                break;
            case TASK_CONDITION_TYPE_FUNCTION:
                //TaskConditionType is a Function returning a _Bool

                //Cast the TaskCondition from void* to a _Bool *(function)(void) and call it..
                // i.e cast a [void pointer] to a [pointer to a function returning _Bool].
                //The function should reset any flags that it may use prior to returning.
                if((*((_Bool (*)(void))TaskList[currentTask].TaskCondition))()) {

                    //Execute the TaskExec() function with the TaskID as a parameter, the..
                    // function should return true to set it's state back to idle, or..
                    // handle it manually.
                    if(!TaskList[currentTask].TaskExec && (*TaskList[currentTask].TaskExec)(TaskList[currentTask].TaskID)) {
                        TaskList[currentTask].TaskState = TASK_STATE_IDLE;
                    } else {
                        TaskList[currentTask].TaskState = TASK_STATE_EXEC;
                    }
                }
                break;
            }
            break;

        case TASK_STATE_EXEC:
            //Do nothing, tasks should only enter this if they handle it (for example..
            // in an interrupt).
            break;

        case TASK_STATE_DEAD:
            //Do nothing
            break;
        }
    }
}


#if SYSTICK_ENABLE == 1
//#############################################//
//                SysTick Variables
//#############################################//
//This variable is outside the #if since it may be referenced elsewhere.
uint32_t currentTime = 0;

//#############################################//
//                SysTick Functions
//#############################################//
uint8_t InitSysTick() {
    IntMasterDisable();
    SysTickDisable();

    SysTickPeriodSet(SYSTICK_PERIOD - 1);

    //Force the counter to reload by writing any value to this register
    NVIC_ST_CURRENT_R = 0;

    //Set the priority of the SysTick Interrupt to 2 (value is in the top 3 bits)
    NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|(2 << 29); // priority 2

    //Set SysTick clock source to main core clock.
    NVIC_ST_CTRL_R = NVIC_ST_CTRL_CLK_SRC;

    // Register the interrupt handler
    SysTickIntRegister(SysTickInterruptHandler);

    //Enable SysTick and resume interrupts
    SysTickEnable();
    IntMasterEnable();

    //Return 0, possible future use for error codes or other returns.
    return 0;
}

/*
 * SysTickInterruptHandler()
 *  The interrupt handler that receives SysTick interrupts, ensure this in tm4c1230h6pm_startup_ccs.c
 */
void SysTickInterruptHandler(void) {
    currentTime++;

    //Iterate through the TaskTimerList and update the TaskFlags
    uint8_t currentTimer = 0;
    for (currentTimer = 0; currentTimer < numOfTaskTimers; currentTimer++){

        //If the currentTime is past the TaskLastRunTime + TaskPeriod, set the flag to true.
        if(currentTime >= TaskTimerList[currentTimer].TaskLastTime + TaskTimerList[currentTimer].TaskPeriod) {
            TaskTimerList[currentTimer].TaskFlag = true;
            TaskTimerList[currentTimer].TaskLastTime = currentTime;
        }
    }
}
#endif
