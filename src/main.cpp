/*
  Apery, a USI shogi playing engine derived from Stockfish, a UCI chess playing engine.
  Copyright (C) 2004-2008 Tord Romstad (Glaurung author)
  Copyright (C) 2008-2015 Marco Costalba, Joona Kiiski, Tord Romstad
  Copyright (C) 2015-2016 Marco Costalba, Joona Kiiski, Gary Linscott, Tord Romstad
  Copyright (C) 2011-2016 Hiraoka Takuya

  Apery is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Apery is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "common.hpp"
#include "bitboard.hpp"
#include "init.hpp"
#include "position.hpp"
#include "usi.hpp"
#include "thread.hpp"
#include "tt.hpp"
#include "search.hpp"

#if defined GODWHALE_CLUSTER_SLAVE
#include "version.hpp"
#include "godwhaleIo.hpp"
#endif

#if defined FIND_MAGIC
// Magic Bitboard の Magic Number を求める為のソフト
int main() {
    u64 RookMagic[SquareNum];
    u64 BishopMagic[SquareNum];

    std::cout << "const u64 RookMagic[81] = {" << std::endl;
    for (Square sq = SQ11; sq < SquareNum; ++sq) {
        RookMagic[sq] = findMagic(sq, false);
        std::cout << "\tUINT64_C(0x" << std::hex << RookMagic[sq] << ")," << std::endl;
    }
    std::cout << "};\n" << std::endl;

    std::cout << "const u64 BishopMagic[81] = {" << std::endl;
    for (Square sq = SQ11; sq < SquareNum; ++sq) {
        BishopMagic[sq] = findMagic(sq, true);
        std::cout << "\tUINT64_C(0x" << std::hex << BishopMagic[sq] << ")," << std::endl;
    }
    std::cout << "};\n" << std::endl;

    return 0;
}

#else
static void validateLoginName(const std::string name) {
    if (name.empty()) {
        std::cerr << "The Login_Name is empty !" << std::endl;
        exit(EXIT_FAILURE);
    }
  
    if (name.length() > LoginNameMaxLength) {
        std::cerr << "The Login_Name is too long !" << std::endl;
        exit(EXIT_FAILURE);
    }
  
    if (!std::all_of(name.begin(), name.end(), [](char _) { return (isalnum(_) || _ == '_'); })) {
        std::cerr << "The Login_Name '" << name << "' has invalid character !" << std::endl;
        exit(EXIT_FAILURE);
    }
}

// usage godwhale_apery host port name [threads]
int main(int argc, char* argv[]) {
    std::unique_ptr<Searcher> s;

    try {
        initTable();
        Position::initZobrist();
        HuffmanCodedPos::init();
        s.reset(new Searcher);

        int threads = -1;
        if (argc > 3) {
            auto loginName = argv[3];
            validateLoginName(loginName);
            s->options["Login_Name"] = USIOption(loginName);
            if (argc > 4) {
                threads = std::stoi(argv[4]);
            }

            IsGodwhaleMode = true;
            startGodwhaleIo(argv[1], argv[2]);
            s->init(threads);
            s->doUSICommandLoop(1, argv);
            closeGodwhaleIo();
        }
        else {
            s->init();
            s->doUSICommandLoop(argc, argv);
        }
    }
    catch (std::exception &ex) {
        std::cerr << "unhandled exception: " << ex.what() << std::endl;
    }

    if (s != nullptr) s->threads.exit();
}

/*#else
// 将棋を指すソフト
int main(int argc, char* argv[]) {
    initTable();
    Position::initZobrist();
    HuffmanCodedPos::init();
    auto s = std::unique_ptr<Searcher>(new Searcher);
    s->init();
    s->doUSICommandLoop(argc, argv);
    s->threads.exit();
}*/

#endif
