
#include "emu.h"
#include "adapter.h"

#define VERBOSE 1
#include "logmacro.h"

DEFINE_DEVICE_TYPE(NABU_NETWORK_ADAPTER, bus::nabu::nabu_network_adapter, "nabu_net_adpt", "NABU Network Adapter")

namespace bus::nabu {

static INPUT_PORTS_START( nabu_network_adapter )
	PORT_START("CONFIG")
	PORT_CONFNAME(0x01, 0x00, "Prompt for channel?")
	PORT_CONFSETTING(0x01, "Yes")
	PORT_CONFSETTING(0x00, "No")
INPUT_PORTS_END

nabu_network_adapter::nabu_network_adapter(machine_config const &mconfig, char const *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, NABU_NETWORK_ADAPTER, tag, owner, clock)
	, device_buffered_serial_interface(mconfig, *this)
	, device_rs232_port_interface(mconfig, *this)
	, m_config(*this, "CONFIG")
	, m_channel(0)
	, m_packet(0)
	, m_segment(0)
	, m_state(State::START)
	, m_substate(0)
	{
}

ioport_constructor nabu_network_adapter::device_input_ports() const
{
	return INPUT_PORTS_NAME( nabu_network_adapter );
}

void nabu_network_adapter::device_add_mconfig(machine_config &config)
{
}

void nabu_network_adapter::device_start()
{
	save_item(NAME(m_state));
	save_item(NAME(m_substate));
	save_item(NAME(m_channel));
	save_item(NAME(m_packet));
	save_item(NAME(m_segment));
}

void nabu_network_adapter::device_reset()
{
	m_state = State::START;
	m_substate = 0;
	// initialise state
	clear_fifo();

	// configure device_buffered_serial_interface
	set_data_frame(START_BIT_COUNT, DATA_BIT_COUNT, PARITY, STOP_BITS);
	set_rate(BAUD);
	receive_register_reset();
	transmit_register_reset();
}

WRITE_LINE_MEMBER(nabu_network_adapter::input_txd)
{
	device_buffered_serial_interface::rx_w(state);
}

void nabu_network_adapter::tra_callback()
{
	output_rxd(transmit_register_get_data_bit());
}

void nabu_network_adapter::connect(uint8_t byte, bool channel_request = true)
{
	if (byte == 0x83 && m_substate == 0) {
		transmit_byte(0x10);
		transmit_byte(0x06);
		transmit_byte(0xE4);
	} else if (byte == 0x82 && m_substate == 1) {
		transmit_byte(0x10);
		transmit_byte(0x06);
	} else if (byte == 0x01 && m_substate == 2) {
		transmit_byte(channel_request ? 0x9F : 0x1F);
		transmit_byte(0x10);
		transmit_byte(0xE1);
		m_state = State::IDLE;
	} else {
		LOG("Unexpected byte: 0x%02X (%d), restarting Adapter.\n", byte, m_substate);
		m_state = State::START;
		m_substate = 0;
		return;
	}
	++m_substate;
}

void nabu_network_adapter::idle(uint8_t byte)
{
	m_substate = 0;
	switch(byte) {
	case 0x85:
		transmit_byte(0x10);
		transmit_byte(0x06);
		m_state = State::CHANNEL_REQUEST;
		break;
	case 0x84:
		transmit_byte(0x10);
		transmit_byte(0x06);
		m_state = State::SEGMENT_REQUEST;
		break;
	case 0x83:
		transmit_byte(0x10);
		transmit_byte(0x06);
		break;
	case 0x82:
		transmit_byte(0x10);
		transmit_byte(0x06);
		break;
	case 0x81:
		transmit_byte(0x10);
		transmit_byte(0x06);
		m_state = State::HEX81_REQUEST;
		break;
	case 0x01:
		transmit_byte(0x10);
		transmit_byte(0x06);
		break;
	}

}
void nabu_network_adapter::hex81_request(uint8_t byte)
{
	if (m_substate == 0) {
		m_dummy = (m_dummy & 0xFF00) | (byte);
	} else if (m_substate == 1) {
		m_dummy = (m_dummy & 0xFF) | (byte << 8);
		transmit_byte(0xE4);
		LOG("HEX81 Request: 0x%04X\n", m_dummy);
		m_state = State::IDLE;
	}
	++m_substate;

}

void nabu_network_adapter::channel_request(uint8_t byte)
{
	if (m_substate == 0) {
		m_channel = (m_channel & 0xFF00) | (byte);
	} else if (m_substate == 1) {
		m_channel = (m_channel & 0xFF) | (byte << 8);
		transmit_byte(0xE4);
		LOG("Channel: 0x%04X\n", m_channel);
		m_state = State::IDLE;
	}
	++m_substate;
}

void nabu_network_adapter::segment_request(uint8_t byte)
{
	if (m_substate == 0) {
		m_packet = byte;
	} else if (m_substate == 1) {
		m_segment = (m_segment & 0xFFFF00) | (byte);
	} else if (m_substate == 2) {
		m_segment = (m_segment & 0xFF00FF) | (byte << 8);
	} else if (m_substate == 3) {
		m_segment = (m_segment & 0xFFFF) | (byte << 16);
		transmit_byte(0xE4);
		transmit_byte(0x91);
		m_state = State::SEND_SEGMENT;
		m_substate = 0;
		LOG("Segment: 0x%06X, Packet: 0x%02X\n", m_segment, m_packet);
		return;
	}
	++m_substate;
}

void nabu_network_adapter::send_segment(uint8_t byte)
{
	if (m_substate == 0) {
		if (byte != 0x10) {
			LOG("Expecting byte 0x10 got %02X, restarting.\n", byte);
			m_state = State::START;
			m_substate = 0;
			return;
		}
	} else if (m_substate == 1) {
		if (byte != 0x06) {
			LOG("Expecting byte 0x06 got %02X, restarting.\n", byte);
			m_state = State::START;
			m_substate = 0;
			return;
		}
		transmit_byte(0x10);
		transmit_byte(0xe1);
		LOG("Segment sent, returning to idle state\n");
		m_state = State::IDLE;
	}

	++m_substate;
}

void nabu_network_adapter::received_byte(uint8_t byte)
{
	LOG("Recieved Byte 0x%02X\n", byte);
	switch(m_state) {
	case State::START:
		connect(byte, bool(m_config->read() & 1));
		break;
	case State::IDLE:
		idle(byte);
		break;
	case State::CHANNEL_REQUEST:
		channel_request(byte);
		break;
	case State::SEGMENT_REQUEST:
		segment_request(byte);
		break;
	case State::HEX81_REQUEST:
		hex81_request(byte);
		break;
	case State::SEND_SEGMENT:
		send_segment(byte);
		break;
	}
}

}

