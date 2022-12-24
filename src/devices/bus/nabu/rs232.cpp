// license:BSD-3-Clause
// copyright-holders:Brian Johnson
/*******************************************************************
 *
 * NABU PC RS232 Card
 *
 *******************************************************************/

#include "emu.h"
#include "rs232.h"

#include "bus/rs232/rs232.h"

//**************************************************************************
//  NABU PC RS232 DEVICE
//**************************************************************************

DEFINE_DEVICE_TYPE(NABU_OPTION_RS232, bus::nabu::rs232_device, "nabupc_option_rs232", "NABU PC RS232 Card")

namespace bus::nabu {

//-------------------------------------------------
//  rs232_device - constructor
//-------------------------------------------------
rs232_device::rs232_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	device_t(mconfig, NABU_OPTION_RS232, tag, owner, clock),
	device_option_expansion_interface(mconfig, *this),
	m_i8251(*this, "i8251"),
	m_pit8253(*this, "i8253")
{
}

static DEVICE_INPUT_DEFAULTS_START( terminal )
	DEVICE_INPUT_DEFAULTS( "RS232_RXBAUD", 0xff, RS232_BAUD_9600 )
	DEVICE_INPUT_DEFAULTS( "RS232_TXBAUD", 0xff, RS232_BAUD_9600 )
	DEVICE_INPUT_DEFAULTS( "RS232_DATABITS", 0xff, RS232_DATABITS_8 )
	DEVICE_INPUT_DEFAULTS( "RS232_PARITY", 0xff, RS232_PARITY_NONE )
	DEVICE_INPUT_DEFAULTS( "RS232_STOPBITS", 0xff, RS232_STOPBITS_1 )
DEVICE_INPUT_DEFAULTS_END

//-------------------------------------------------
//  device_add_mconfig - device-specific config
//-------------------------------------------------
void rs232_device::device_add_mconfig(machine_config &config)
{
	I8251(config, m_i8251, clock() / 2);
	m_i8251->txd_handler().set("rs232", FUNC(rs232_port_device::write_txd));
	PIT8253(config, m_pit8253, clock() / 2);
	m_pit8253->set_clk<0>(clock() / 2);
	m_pit8253->out_handler<0>().set(m_i8251, FUNC(i8251_device::write_txc));
	m_pit8253->set_clk<1>(clock() / 2);
	m_pit8253->out_handler<1>().set(m_i8251, FUNC(i8251_device::write_rxc));

	rs232_port_device &rs232(RS232_PORT(config, "rs232", default_rs232_devices, nullptr));
	rs232.rxd_handler().set(m_i8251, FUNC(i8251_device::write_rxd));
	rs232.cts_handler().set(m_i8251, FUNC(i8251_device::write_cts));
	rs232.set_option_device_input_defaults("terminal", DEVICE_INPUT_DEFAULTS_NAME(terminal));
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------
void rs232_device::device_start()
{
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------
void rs232_device::device_reset()
{
}

uint8_t rs232_device::read(offs_t offset)
{
	uint8_t result = 0xff;
	switch (offset) {
	case 0:
	case 1:
		result = m_i8251->read(offset);
		break;
	case 4:
	case 5:
	case 7:
		result = m_pit8253->read(offset & 3);
		break;
	case 0x0F:
		result = 0x08;
	}
	return result;
}

void rs232_device::write(offs_t offset, uint8_t data)
{
	switch (offset) {
	case 0:
	case 1:
		m_i8251->write(offset, data);
		break;
	case 4:
	case 5:
	case 7:
		m_pit8253->write(offset & 3, data);
		break;
	}
}

} // namespace bus::nabu
