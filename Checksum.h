#ifndef __Checksum__H
#define __Checksum__H

// based on crc.h/.cpp - written and placed in the public domain by Wei Dai (http://www.cryptopp.com/)
// modifications made and likewise placed in the public domain by Markus Thiele (omega@elowar.com)

#define IS_LITTLE_ENDIAN /* for x86 Windows PCs */

#include <stdint.h>
#include <stddef.h>

const uint32_t CRC32_NEGL = 0xffffffffL;

class Checksum {
	public:
		Checksum() { reset(); }
		void update(const char* input, size_t length);
		uint32_t value() { return m_crc ^ CRC32_NEGL; }
		
	private:
		void reset() { m_crc = CRC32_NEGL; }
		uint32_t m_crc;
		static const uint32_t m_tab[256];
};

#endif
