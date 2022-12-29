// license:BSD-3-Clause
// copyright-holders:Brian Johnson
/*******************************************************************
 *
 * NABU PC RS232 Controller
 *
 *******************************************************************/

#ifndef MAME_BUS_NABUPC_RS232_H
#define MAME_BUS_NABUPC_RS232_H

#pragma once

#include "option.h"
#include "machine/i8251.h"
#include "machine/pit8253.h"

namespace bus::nabu {

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

/* NABU PC RS232 Device */

class rs232_device : public device_t, public bus::nabu::device_option_expansion_interface
{
public:
	// construction/destruction
	rs232_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual uint8_t read(offs_t offset) override;
	virtual void write(offs_t offset, uint8_t data) override;
protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// optional information overrides
	virtual void device_add_mconfig(machine_config &config) override;

	DECLARE_WRITE_LINE_MEMBER(rxrdy_w);
private:
	required_device<i8251_device> m_i8251;
	required_device<pit8253_device> m_pit8253;

};

} // namespace bus::nabu

// device type definition
DECLARE_DEVICE_TYPE_NS(NABU_OPTION_RS232, bus::nabu, rs232_device)

#endif // MAME_BUS_NABUPC_RS232_H
