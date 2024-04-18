#ifndef MAME_BUS_NABU_9918_H
#define MAME_BUS_NABU_9918_H

#pragma once

#include "emu.h"
#include "screen.h"
#include "video.h"
#include "video/tms9928a.h"

namespace bus::nabu {

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> apricot_mono_display_device

class tms9918a_card_device : public device_t, public video_card_interface
{
public:
	tms9918a_card_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	// device-level overrides
	virtual void device_add_mconfig(machine_config &config) override;
	virtual void device_start() override;
	virtual void device_reset() override;
	
	virtual uint8_t read(offs_t offset) override;
	virtual void write(offs_t offset, uint8_t data) override;

private:
	required_device<screen_device> m_screen;
	required_device<tms9918a_device> m_tms9918a;

};

} // namespace bus::nabu

DECLARE_DEVICE_TYPE_NS(NABU_TMS9918A_CARD, bus::nabu, tms9918a_card_device)

#endif
