/*
 * iface.hpp
 *
 *  Created on: 25 ??? 2020
 *      Author: conta
 */

#ifndef SRC_COMMS_COM_INTERFACE_HPP_
#define SRC_COMMS_COM_INTERFACE_HPP_

#include <src/system.hpp>

class ComInterface
{
public:
	ComInterface(System sys_);
private:
	System systm;
};

#endif /* SRC_COMMS_COM_INTERFACE_HPP_ */
