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
#include "strformat.h"


nabupc_format::nabupc_format(bool cpm) : m_cpmldr(cpm)
{
}

bool nabupc_format::supports_save() const
{
	return true;
}

int nabupc_format::identify(util::random_read &io, uint32_t form_factor, const std::vector<uint32_t> &variants) const
{
	uint64_t size;
	uint8_t  sign[7];
	size_t   actual;
	bool     cpm = false;

	sign[6] = 0;
	if (io.length(size))
		return 0;

	if (size != 204800)
		return 0;

	io.read_at(0x4d1, sign, 6, actual);
	if (memcmp(sign, "CPMLDR", 6) == 0) {
		cpm = true;
	}

	return FIFID_SIZE | (m_cpmldr == cpm ? FIFID_HINT : 0);
}

void nabupc_format::build_nabu_track_mfm(int track, int head, floppy_image *image, int cell_count, int sector_count, const desc_pc_sector *sects, int gap_3, int gap_1, int gap_2, bool cpmldr)
{
	std::vector<uint32_t> track_data;

	if (cpmldr && track == 0) {
		for(int i=0; i< 2; i++) raw_w(track_data, 16, 0x4489);
		raw_w(track_data, 16, 0x1054);  // 4e
		for(int i=0; i<11; i++) mfm_w(track_data, 8, 0x4e);
		for(int i=0; i<sizeof(dpb_table); i++) mfm_w(track_data, 8, dpb_table[i]);
	}

	for(int i=0; i<gap_1; i++) mfm_w(track_data, 8, 0x4e);

	int total_size = 0;
	for(int i=0; i<sector_count; i++)
		total_size += sects[i].actual_size;

	int etpos = int(track_data.size()) + (sector_count*(12+3+5+2+gap_2+12+3+1+2) + total_size)*16;

	if(etpos > cell_count)
		throw std::invalid_argument(util::string_format("Incorrect layout on track %d head %d, expected_size=%d, current_size=%d", track, head, cell_count, etpos));

	if(etpos + gap_3*16*(sector_count-1) > cell_count)
		gap_3 = (cell_count - etpos) / 16 / (sector_count-1);

	// Build the track
	for(int i=0; i<sector_count; i++) {
		uint16_t crc;
		// sync and IDAM and gap 2
		for(int j=0; j<12; j++) mfm_w(track_data, 8, 0x00);
		unsigned int cpos = track_data.size();
		for(int j=0; j< 3; j++) raw_w(track_data, 16, 0x4489);
		mfm_w(track_data, 8, 0xfe);
		mfm_w(track_data, 8, sects[i].track);
		mfm_w(track_data, 8, sects[i].head);
		mfm_w(track_data, 8, sects[i].sector);
		mfm_w(track_data, 8, sects[i].size);
		crc = calc_crc_ccitt(track_data, cpos, track_data.size());
		mfm_w(track_data, 16, crc);
		for(int j=0; j<gap_2; j++) mfm_w(track_data, 8, 0x4e);

		if(!sects[i].data)
			for(int j=0; j<12+4+sects[i].actual_size+2+(i != sector_count-1 ? gap_3 : 0); j++) mfm_w(track_data, 8, 0x4e);

		else {
			// sync, DAM, data and gap 3
			for(int j=0; j<12; j++) mfm_w(track_data, 8, 0x00);
			cpos = track_data.size();
			for(int j=0; j< 3; j++) raw_w(track_data, 16, 0x4489);
			mfm_w(track_data, 8, sects[i].deleted ? 0xf8 : 0xfb);
			for(int j=0; j<sects[i].actual_size; j++) mfm_w(track_data, 8, sects[i].data[j]);
			crc = calc_crc_ccitt(track_data, cpos, track_data.size());
			if(sects[i].bad_crc)
				crc = 0xffff^crc;
			mfm_w(track_data, 16, crc);
			if(i != sector_count-1)
				for(int j=0; j<gap_3; j++) mfm_w(track_data, 8, 0x4e);
		}
	}

	// Gap 4b

	while(int(track_data.size()) < cell_count-15) mfm_w(track_data, 8, 0x4e);
	raw_w(track_data, cell_count-int(track_data.size()), 0x9254 >> (16+int(track_data.size())-cell_count));

	generate_track_from_levels(track, head, track_data, 0, image);
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
			build_nabu_track_mfm(track, 0, image, 100000, 5, sects, 80, 32, 22, m_cpmldr);
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

nabupc_cpm_format::nabupc_cpm_format() : nabupc_format(true)
{
}

const char *nabupc_cpm_format::name() const
{
	return "nabupc_cpm";
}

const char *nabupc_cpm_format::description() const
{
	return "NABU PC CPM Disk Image";
}

const char *nabupc_cpm_format::extensions() const
{
	return "img";
}

nabupc_osborne_format::nabupc_osborne_format() : nabupc_format(false)
{
}

const char *nabupc_osborne_format::name() const
{
	return "nabupc_osborne";
}

const char *nabupc_osborne_format::description() const
{
	return "NABU PC Data Disk Image";
}

const char *nabupc_osborne_format::extensions() const
{
	return "img";
}

const nabupc_cpm_format FLOPPY_NABUPC_CPM_FORMAT;
const nabupc_osborne_format FLOPPY_NABUPC_OSBORNE_FORMAT;
