#ifndef MAME_BUS_NABU_VIDEO_H
#define MAME_BUS_NABU_VIDEO_H

#pragma once

namespace bus::nabu {

//**************************************************************************
//  FORWARD DECLARATIONS
//**************************************************************************

class video_card_interface;

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

/* NABU PC Video Port Device */

class video_port_device : public device_t, public device_single_card_slot_interface<video_card_interface>
{
public:
	// construction/destruction
	template <typename T>
	video_port_device(const machine_config &mconfig, const char *tag, device_t *owner, T &&opts, const char *dflt)
		: video_port_device(mconfig, tag, owner, uint32_t(0))
	{
		option_reset();
		opts(*this);
		set_default_option(dflt);
		set_fixed(false);
	}
	video_port_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	
	// callbacks
	auto int_callback() { return m_int_handler.bind(); }

	// called from cart device
	DECLARE_WRITE_LINE_MEMBER( int_w ) { m_int_handler(state); }

	uint8_t io_read(offs_t offset);
	void io_write(offs_t offset, uint8_t data);

protected:
	// device-level overrides
	virtual void device_start() override;

private:
	devcb_write_line m_int_handler;
};

/* NABU PC Video Port interface */
class video_card_interface : public device_interface
{
public:
	virtual uint8_t read(offs_t offset) = 0;
	virtual void write(offs_t offset, uint8_t data) = 0;

protected:
	video_card_interface(const machine_config &mconfig, device_t &device);

	video_port_device *m_slot;
};

void video_card_devices(video_port_device &device);

} // namespace bus::nabu

DECLARE_DEVICE_TYPE_NS(NABU_VIDEO_PORT, bus::nabu, video_port_device)

#endif
