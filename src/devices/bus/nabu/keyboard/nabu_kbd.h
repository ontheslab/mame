// license:BSD-3-Clause
// copyright-holders:Brian Johnson


#ifndef MAME_BUS_NABU_KBD_H
#define MAME_BUS_NABU_KBD_H

#pragma once

#include "bus/rs232/rs232.h"
#include "cpu/m6800/m6801.h"
#include "machine/adc0808.h"

namespace bus::nabu::keyboard {

class nabu_keyboard_device: public device_t, public device_rs232_port_interface
{
public:
	nabu_keyboard_device(machine_config const &mconfig, char const *tag, device_t *owner, uint32_t clock = 0);

protected:
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_add_mconfig(machine_config &config) override;
	virtual tiny_rom_entry const *device_rom_region() const override;
	virtual ioport_constructor device_input_ports() const override;
private:
	void nabu_kb_mem(address_map &map);

	uint8_t port1_r();
	void port1_w(uint8_t data);

	void irq_w(uint8_t data);

	uint8_t gameport_r(offs_t offset);
	uint8_t adc_data_r();
	void adc_latch_w(offs_t offset, uint8_t data);
	void adc_start_w(offs_t offset, uint8_t data);
	void cpu_ack_irq_w(offs_t offset, uint8_t data);

	required_device<m6803_cpu_device> m_mcu;
	required_device<adc0809_device> m_adc;

	required_ioport m_modifiers;
	required_ioport_array<8> m_keyboard;
	required_ioport_array<4> m_gameport;

	uint8_t m_port1;
	uint8_t m_eoc;
};

} // namespace bus::nabu::keyboard

DECLARE_DEVICE_TYPE_NS(NABU_KBD, bus::nabu::keyboard, nabu_keyboard_device)

#endif
