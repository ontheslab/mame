// license:BSD-3-Clause
// copyright-holders:Brian Johnson
/*******************************************************************
 *
 * NABU PC HDD Card
 *
 *******************************************************************/

#include "emu.h"
#include "hdd.h"

//**************************************************************************
//  NABU PC FDC DEVICE
//**************************************************************************

DEFINE_DEVICE_TYPE(NABU_OPTION_HDD, bus::nabu::hdd_device, "nabupc_option_hdd", "NABU PC Hard Disk Controller")

namespace bus::nabu {

//-------------------------------------------------
//  hdd_device - constructor
//-------------------------------------------------
hdd_device::hdd_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	device_t(mconfig, NABU_OPTION_HDD, tag, owner, clock),
	device_option_expansion_interface(mconfig, *this),
	m_hdd(*this, "hdd")
{
}

//-------------------------------------------------
//  device_add_mconfig - device-specific config
//-------------------------------------------------
void hdd_device::device_add_mconfig(machine_config &config)
{
	WD1000(config, m_hdd, 0);

	HARDDISK(config, "hdd:0", 0);
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------
void hdd_device::device_start()
{
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------
void hdd_device::device_reset()
{
}

uint8_t hdd_device::read(offs_t offset)
{
	uint8_t result = 0xff;

	switch(offset) {
	case 0x00:
	case 0x01:
	case 0x02:
	case 0x03:
	case 0x04:
	case 0x05:
	case 0x06:
	case 0x07:
		result = m_hdd->read(offset);
		break;
	case 0x0F:
		result = 0xE8;
		break;
	default:
		result = 0xFF;
	}
	return result;
}

void hdd_device::write(offs_t offset, uint8_t data)
{
	switch(offset) {
	case 0x00:
	case 0x01:
	case 0x02:
	case 0x03:
	case 0x04:
	case 0x05:
	case 0x06:
	case 0x07:
		m_hdd->write(offset, data);
		break;
	case 0x0F:
		break;
	}
}

} // namespace bus::nabupc
