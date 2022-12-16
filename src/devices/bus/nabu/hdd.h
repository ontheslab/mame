// license:BSD-3-Clause
// copyright-holders:Brian Johnson
/*******************************************************************
 *
 * NABU PC Hard Drive Controller
 *
 *******************************************************************/

#ifndef MAME_BUS_NABUPC_HDD_H
#define MAME_BUS_NABUPC_HDD_H

#pragma once

#include "option.h"

#include "machine/wd1000.h"

namespace bus::nabu {

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

/* NABU PC HDD Device */

class hdd_device : public device_t, public bus::nabu::device_option_expansion_interface
{
public:
	// construction/destruction
	hdd_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual uint8_t read(offs_t offset) override;
	virtual void write(offs_t offset, uint8_t data) override;
protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// optional information overrides
	virtual void device_add_mconfig(machine_config &config) override;
private:
	required_device<wd1000_device>              m_hdd;
};

} // namespace bus::nabupc

// device type definition
DECLARE_DEVICE_TYPE_NS(NABU_OPTION_HDD, bus::nabu, hdd_device)

#endif // MAME_BUS_NABUPC_HDD_H
