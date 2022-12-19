// license:BSD-3-Clause
// copyright-holders:Brian Johnson
/*********************************************************************

    formats/nabupc_dsk.cpp

    NABU PC floppy-disk images

*********************************************************************/
#ifndef MAME_FORMATS_NABUPC_DSK_H
#define MAME_FORMATS_NABUPC_DSK_H

#pragma once

#include "flopimg.h"

class nabupc_format : public floppy_image_format_t {
public:
	nabupc_format(bool cpm);

	virtual int identify(util::random_read &io, uint32_t form_factor, const std::vector<uint32_t> &variants) const override;
	virtual bool load(util::random_read &io, uint32_t form_factor, const std::vector<uint32_t> &variants, floppy_image *image) const override;
	virtual bool save(util::random_read_write &io, const std::vector<uint32_t> &variants, floppy_image *image) const override;

	virtual bool supports_save() const override;

protected:
	static void build_nabu_track_mfm(int track, int head, floppy_image *image, int cell_count, int sector_count, const desc_pc_sector *sects, int gap_3, int gap_1, int gap_2, bool cpmldr);
private:
	const bool m_cpmldr;
	static constexpr uint8_t dpb_table[]{0x19, 0x0F, 0x2D, 0x1A, 0x00, 0x28, 0x00, 0x03, 0x07, 0x00, 0xC2, 0x00, 0x5F, 0x00, 0xE0, 0x00, 0x00, 0x18, 0x01, 0x00, 0x03, 0x07};
};


class nabupc_cpm_format : public nabupc_format {
public:
	nabupc_cpm_format();
	virtual const char *name() const override;
	virtual const char *description() const override;
	virtual const char *extensions() const override;
};

class nabupc_osborne_format : public nabupc_format {
public:
	nabupc_osborne_format();
	virtual const char *name() const override;
	virtual const char *description() const override;
	virtual const char *extensions() const override;
};

extern const nabupc_cpm_format FLOPPY_NABUPC_CPM_FORMAT;
extern const nabupc_osborne_format FLOPPY_NABUPC_OSBORNE_FORMAT;

#endif // MAME_FORMATS_NABUPC_DSK_H
