/*
 * calibration.h
 *
 *  Created on: 28 ??? 2020
 *      Author: conta
 */

#ifndef SRC_MOTOR_CALIBRATION_H_
#define SRC_MOTOR_CALIBRATION_H_

#include <src/common.h>
#include <src/motor/motor.h>

bool CalibrateResistance(void);
bool CalibrateInductance(void);
bool CalibrateOffsetDirectionAndPolePairs(void);

#endif /* SRC_MOTOR_CALIBRATION_H_ */
