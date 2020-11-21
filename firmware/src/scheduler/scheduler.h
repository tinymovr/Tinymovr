/*
 * scheduler.h
 *
 *  Created on: 28 ??? 2020
 *      Author: conta
 */

#ifndef SRC_SCHEDULER_SCHEDULER_H_
#define SRC_SCHEDULER_SCHEDULER_H_

void WaitForControlLoopInterrupt(void);

uint32_t Scheduler_GetTotalCycles(void);
uint32_t Scheduler_GetBusyCycles(void);

#endif /* SRC_SCHEDULER_SCHEDULER_H_ */
