// license:BSD-3-Clause
// copyright-holders:Brian Johnson
/*********************************************************************

    formats/nabupc_dsk.cpp

    NABU PC floppy-disk images

    Disk Layout:
      Single Sided Double Density 5 1024 byte sectors per track
*********************************************************************/

#include "nabupc_dsk.h"

#include "ioprocs.h"


nabupc_format::nabupc_format()
{
}

const char *nabupc_format::name() const
{
	return "nabupc";
}

const char *nabupc_format::description() const
{
	return "NABU PC disk image";
}

const char *nabupc_format::extensions() const
{
	return "img";
}

bool nabupc_format::supports_save() const
{
	return true;
}

int nabupc_format::identify(util::random_read &io, uint32_t form_factor, const std::vector<uint32_t> &variants) const
{
	uint64_t size;
	if (io.length(size))
		return 0;

	if (size == 204800)
		return FIFID_SIZE;

	return 0;
}

bool nabupc_format::load(util::random_read &io, uint32_t form_factor, const std::vector<uint32_t> &variants, floppy_image *image) const
{
	for (int track = 0; track < 40; track++) {
			desc_pc_sector sects[5];
			uint8_t sectdata[5*1024];
			size_t actual;
			io.read_at(5*1024*track, sectdata, 5*1024, actual);
			for (int i = 0; i < 5; i++) {
				sects[i].track = track;
				sects[i].head = 0;
				sects[i].sector = i+1;
				sects[i].size = 3;
				sects[i].actual_size = 1024;
				sects[i].data = sectdata + 1024*i;
				sects[i].deleted = false;
				sects[i].bad_crc = false;
			}
			build_wd_track_mfm(track, 0, image, 100000, 5, sects, 80, 32, 22);
	}
	return true;
}

bool nabupc_format::save(util::random_read_write &io, const std::vector<uint32_t> &variants, floppy_image *image) const
{

	uint64_t file_offset = 0;

	for (int track = 0; track < 40; track++) {
		auto bitstream = generate_bitstream_from_track(track, 0, 2000, image);
		auto sectors = extract_sectors_from_bitstream_mfm_pc(bitstream);

		for (int i = 0; i < 5; i++) {
			size_t actual;
			io.write_at(file_offset, sectors[i + 1].data(), 1024, actual);
			file_offset += 1024;
		}
	}

	return true;
}

const nabupc_format FLOPPY_NABUPC_FORMAT;
