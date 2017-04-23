#ifndef CRC32_H
#define CRC32_H

#include "boost/crc.hpp"

#ifndef PRIVATE_BUFFER_SIZE
#define PRIVATE_BUFFER_SIZE  1024
#endif

static unsigned int crc32_file(const std::string &filepath) {
    std::ifstream ifs(filepath, std::ios_base::binary);
    boost::crc_32_type result;

    if (ifs) {
        do {
            char buffer[PRIVATE_BUFFER_SIZE];

            ifs.read(buffer, sizeof(buffer));
            result.process_bytes(buffer, ifs.gcount());
        } while (ifs);
    }
    else {
        return 0;
    }

    return result.checksum();
}

#endif
