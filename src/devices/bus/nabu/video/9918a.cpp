#include "emu.h"
#include "9918a.h"

//**************************************************************************
//  DEVICE DEFINITIONS
//**************************************************************************

DEFINE_DEVICE_TYPE(NABU_TMS9918A_CARD, bus::nabu::tms9918a_card_device, "nabu_tms9918a_display", "NABU TMS9918A Display")

namespace bus::nabu {

tms9918a_card_device::tms9918a_card_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	device_t(mconfig, NABU_TMS9918A_CARD, tag, owner, clock),
	video_card_interface(mconfig, *this),
	m_screen(*this, "screen"),
	m_tms9918a(*this, "tms9918a")
{
}

//-------------------------------------------------
//  device_add_mconfig - add device configuration
//-------------------------------------------------

void tms9918a_card_device::device_add_mconfig(machine_config &config)
{
	// Video hardware
	SCREEN(config, m_screen, SCREEN_TYPE_RASTER);

	TMS9918A(config, m_tms9918a, 10.738635_MHz_XTAL);
	m_tms9918a->set_screen(m_screen);
	m_tms9918a->set_vram_size(0x4000);
	m_tms9918a->int_callback().set(*m_slot, FUNC(video_port_device::int_w));
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void tms9918a_card_device::device_start()
{
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void tms9918a_card_device::device_reset()
{
}

uint8_t tms9918a_card_device::read(offs_t offset)
{
	return m_tms9918a->read(offset);
}

void tms9918a_card_device::write(offs_t offset, uint8_t data)
{
	m_tms9918a->write(offset, data);
}

} // namespace bus::nabu

