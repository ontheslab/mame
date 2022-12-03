// license:BSD-3-Clause
// copyright-holders:Brian Johnson


/*****************************************************************************
 *
 * 0x40 -> ctrl
 * 0x41 -> shift?
 * 0x42 -> caps?
 *
 * They keyboard also contains 4 read-only gameport registers with 0 meaning
 * the digital contact is set
 * 0x5000 - Port 1
 * 0x5100 - Port 2
 * 0x5200 - Port 3
 * 0x5300 - Port 4
 *
 *   7   6   5   4   3   2   1   0
 * ---------------------------------
 * | F | - | - | - | U | R | D | L |
 * --------------------------------
 *
 *****************************************************************************/
#include "emu.h"
#include "nabu_kbd.h"


DEFINE_DEVICE_TYPE(NABU_KBD, bus::nabu::keyboard::nabu_keyboard_device, "nabu_kb", "NABU PC keyboard")

namespace bus::nabu::keyboard {

ROM_START(nabu_kb)
	ROM_REGION( 0x800, "mcu", 0 )
	ROM_LOAD( "nabukeyboard-90020070-reva-2716.bin", 0x000, 0x800, CRC(eead3abc) SHA1(2f6ff63ca2f2ac90f3e03ef4f2b79883205e8a4e) )
ROM_END


nabu_keyboard_device::nabu_keyboard_device(machine_config const &mconfig, char const *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, NABU_KBD, tag, owner, clock)
	, device_rs232_port_interface(mconfig, *this)
	, m_mcu(*this, "mcu")
	, m_port1(0)
	, m_gameport{0xff,0xff,0xff,0xff}
{
}

void nabu_keyboard_device::device_start()
{
	save_item(NAME(m_port1));
	save_item(NAME(m_gameport));
}

void nabu_keyboard_device::device_reset()
{
	m_port1 = 0;
	m_gameport[0] = 0xff;
	m_gameport[1] = 0xff;
	m_gameport[2] = 0xff;
	m_gameport[3] = 0xff;

	output_rxd(1);
}

void nabu_keyboard_device::device_add_mconfig(machine_config &config)
{
	M6803(config, m_mcu, XTAL(3'579'545)); // Crystal verified from schematics and visual inspection
	m_mcu->set_addrmap(AS_PROGRAM, &nabu_keyboard_device::nabu_kb_mem);
	m_mcu->in_p1_cb().set(FUNC(nabu_keyboard_device::port1_r));
	m_mcu->out_p1_cb().set(FUNC(nabu_keyboard_device::port1_w));
	m_mcu->out_ser_tx_cb().set(FUNC(nabu_keyboard_device::output_rxd));
}

void nabu_keyboard_device::nabu_kb_mem(address_map &map)
{
	map(0x5000, 0x5300).r(FUNC(nabu_keyboard_device::gameport_r));
	map(0xf800, 0xffff).rom().region("mcu", 0);
}

const tiny_rom_entry *nabu_keyboard_device::device_rom_region() const
{
	return ROM_NAME(nabu_kb);
}

uint8_t nabu_keyboard_device::port1_r()
{
	return m_port1;
}

void nabu_keyboard_device::port1_w(uint8_t data)
{
	m_port1 = data & 0x7f;
}

uint8_t nabu_keyboard_device::gameport_r(offs_t offset)
{
	uint8_t port = (offset >> 8) & 0x03;
	return m_gameport[port];
}

} // bus::nabu::keyboard
