// license:BSD-3-Clause
// copyright-holders:Brian Johnson

#ifndef MAME_BUS_NABU_ADAPTER_HLE_H
#define MAME_BUS_NABU_ADAPTER_HLE_H

#pragma once

#include "bus/rs232/rs232.h"
#include "diserial.h"

namespace bus::nabu {

class nabu_network_adapter
	: public device_t
	, public device_buffered_serial_interface<16U>
	, public device_rs232_port_interface
{
public:
	// constructor/destructor
	nabu_network_adapter(machine_config const &mconfig, char const *tag, device_t *owner, u32 clock);

	virtual DECLARE_WRITE_LINE_MEMBER(input_txd) override;
protected:
	// device overrides
	virtual void device_add_mconfig(machine_config &config) override;
	virtual ioport_constructor device_input_ports() const override;
	virtual void device_start() override;
	virtual void device_reset() override;

	// device_buffered_serial_interface overrides
	virtual void tra_callback() override;

private:
	static constexpr int START_BIT_COUNT = 1;
	static constexpr int DATA_BIT_COUNT = 8;
	static constexpr device_serial_interface::parity_t PARITY = device_serial_interface::PARITY_NONE;
	static constexpr device_serial_interface::stop_bits_t STOP_BITS = device_serial_interface::STOP_BITS_1;
	static constexpr int BAUD = 111'900;

	enum State {START, IDLE, HEX81_REQUEST, CHANNEL_REQUEST, SEGMENT_REQUEST, SEND_SEGMENT};

	virtual void received_byte(uint8_t byte) override;

	// State Machine
	void connect(uint8_t byte, bool channel_request);
	void idle(uint8_t byte);
	void channel_request(uint8_t byte);
	void segment_request(uint8_t byte);
	void hex81_request(uint8_t byte);
	void send_segment(uint8_t byte);

	required_ioport m_config;

	uint16_t m_channel;
	uint8_t  m_packet;
	uint32_t m_segment;
	uint8_t  m_state;
	uint8_t  m_substate;

	uint16_t m_dummy;
};

}

DECLARE_DEVICE_TYPE_NS(NABU_NETWORK_ADAPTER, bus::nabu, nabu_network_adapter)

#endif

