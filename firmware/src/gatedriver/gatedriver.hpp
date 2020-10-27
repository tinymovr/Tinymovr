
//  * This file is part of the Tinymovr-Firmware distribution
//  * (https://github.com/yconst/tinymovr-firmware).
//  * Copyright (c) 2020 Ioannis Chatzikonstantinou.
//  * 
//  * This program is free software: you can redistribute it and/or modify  
//  * it under the terms of the GNU General Public License as published by  
//  * the Free Software Foundation, version 3.
//  *
//  * This program is distributed in the hope that it will be useful, but 
//  * WITHOUT ANY WARRANTY; without even the implied warranty of 
//  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
//  * General Public License for more details.
//  *
//  * You should have received a copy of the GNU General Public License 
//  * along with this program. If not, see <http://www.gnu.org/licenses/>.

#ifndef GATEDRIVER_GATEDRIVER_H_
#define GATEDRIVER_GATEDRIVER_H_

#include <src/component.hpp>

typedef enum {
	GATEDRIVER_DISABLED = 0,
	GATEDRIVER_ENABLED = 1
} GateDriverState;

class Driver : public Component
{
public:
    void Enable(void);
    void Disable(void);
    void SetDutyCycle(struct FloatTriplet *dc);
private:
    GateDriverState state = GATEDRIVER_DISABLED;

};

#endif /* GATEDRIVER_GATEDRIVER_H_ */
