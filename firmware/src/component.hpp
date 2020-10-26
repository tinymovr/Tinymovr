/*
 * iface.hpp
 *
 *  Created on: 25 ??? 2020
 *      Author: conta
 */

#ifndef SRC_COMPONENT_HPP_
#define SRC_COMPONENT_HPP_

#include <src/system.hpp>

class Component
{
public:
	Component(System sys_);
	System systm;
};

#endif /* SRC_COMPONENT_HPP_ */
