#ifndef __APPLICATION_HOOKS_H__
#define __APPLICATION_HOOKS_H__

#include "FreeRTOS.h"
#include "task.h"

#ifdef __cplusplus
extern "C" {
#endif


	/* This project provides two demo applications.  A simple blinky style demo
	application, and a more comprehensive test and demo application.  The
	mainCREATE_SIMPLE_BLINKY_DEMO_ONLY setting is used to select between the two.

	If mainCREATE_SIMPLE_BLINKY_DEMO_ONLY is 1 then the blinky demo will be built.
	The blinky demo is implemented and described in main_blinky.c.

	If mainCREATE_SIMPLE_BLINKY_DEMO_ONLY is not 1 then the comprehensive test and
	demo application will be built.  The comprehensive test and demo application is
	implemented and described in main_full.c. */
#define mainCREATE_SIMPLE_BLINKY_DEMO_ONLY	1

	/* This demo uses heap_5.c, and these constants define the sizes of the regions
	that make up the total heap.  heap_5 is only used for test and example purposes
	as this demo could easily create one large heap region instead of multiple
	smaller heap regions - in which case heap_4.c would be the more appropriate
	choice.  See http://www.freertos.org/a00111.html for an explanation. */


	/* This project provides two demo applications.  A simple blinky style demo
	application, and a more comprehensive test and demo application.  The
	mainCREATE_SIMPLE_BLINKY_DEMO_ONLY setting is used to select between the two.

	If mainCREATE_SIMPLE_BLINKY_DEMO_ONLY is 1 then the blinky demo will be built.
	The blinky demo is implemented and described in main_blinky.c.

	If mainCREATE_SIMPLE_BLINKY_DEMO_ONLY is not 1 then the comprehensive test and
	demo application will be built.  The comprehensive test and demo application is
	implemented and described in main_full.c. */
#define mainCREATE_SIMPLE_BLINKY_DEMO_ONLY	1

	/* This demo uses heap_5.c, and these constants define the sizes of the regions
	that make up the total heap.  heap_5 is only used for test and example purposes
	as this demo could easily create one large heap region instead of multiple
	smaller heap regions - in which case heap_4.c would be the more appropriate
	choice.  See http://www.freertos.org/a00111.html for an explanation. */
#define mainREGION_1_SIZE	7201
#define mainREGION_2_SIZE	29905
#define mainREGION_3_SIZE	6407


	/*
	 * Prototypes for the standard FreeRTOS application hook (callback) functions
	 * implemented within this file.  See http://www.freertos.org/a00016.html .
	 */
	void vApplicationMallocFailedHook(void);
	void vApplicationIdleHook(void);
	void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName);
	void vApplicationTickHook(void);
	void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize);
	void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize);


	void main_blinky(void);
	void main_full(void);


#ifdef __cplusplus
}
#endif

#endif // !__APPLICATION_HOOKS_H__
