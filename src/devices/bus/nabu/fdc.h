// license:BSD-3-Clause
// copyright-holders:Brian Johnson
/*******************************************************************
 *
 * NABU PC Floppy Controller
 *
 *******************************************************************/

#ifndef MAME_BUS_NABU_FDC_H
#define MAME_BUS_NABU_FDC_H

#pragma once

#include "option.h"

#include "imagedev/floppy.h"
#include "machine/wd_fdc.h"

namespace bus::nabu {

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

/* NABU fdc Device */

class fdc_device : public device_t, public bus::nabu::device_option_expansion_interface
{
public:
	// construction/destruction
	fdc_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual uint8_t read(offs_t offset) override;
	virtual void write(offs_t offset, uint8_t data) override;
protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// optional information overrides
	virtual void device_add_mconfig(machine_config &config) override;
private:
	void ds_w(uint8_t data);

	static void floppy_formats(format_registration &fr);

	required_device<wd2797_device>              m_wd2797;
	required_device_array<floppy_connector, 2>  m_floppies;
};

} // namespace bus::nabu

// device type definition
DECLARE_DEVICE_TYPE_NS(NABU_OPTION_FDC, bus::nabu, fdc_device)

#endif // MAME_BUS_NABU_FDC_H

