
#ifndef APERY_GODWHALE_IO_HPP
#define APERY_GODWHALE_IO_HPP

#include "common.hpp"

#if defined GODWHALE_CLUSTER_SLAVE

// ----------------------
//  stream for godwhale
// ----------------------
extern void startGodwhaleIo(const std::string &host, const std::string &port);
extern void closeGodwhaleIo();

#endif

#endif
