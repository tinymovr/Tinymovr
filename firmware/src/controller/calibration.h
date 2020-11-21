/*
 * calibration.h
 *
 *  Created on: 28 ??? 2020
 *      Author: conta
 */

#ifndef SRC_CONTROLLER_CALIBRATION_H_
#define SRC_CONTROLLER_CALIBRATION_H_

#include <src/controller/controller.h>

ControlError CalibrateResistance(void);
ControlError CalibrateInductance(void);
ControlError CalibrateOffsetDirectionAndPolePairs(void);

#endif /* SRC_CONTROLLER_CALIBRATION_H_ */
