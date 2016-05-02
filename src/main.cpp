#include "common.hpp"
#include "bitboard.hpp"
#include "init.hpp"
#include "position.hpp"
#include "usi.hpp"
#include "thread.hpp"
#include "tt.hpp"
#include "search.hpp"

#if defined GODWHALE_CLUSTER_SLAVE 
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

#elif defined GODWHALE_CLUSTER_SLAVE 
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

// 将棋を指すソフト
int main(int argc, char* argv[]) {
    // usage godwhale_apery host port name [threads]

    try {
        initTable();
        Position::initZobrist();
        Search::init();

        USI::init(Options);

        // Threads.initの前にスレッド数を設定する必要がある
        if (argc > 3) {
            auto loginName = argv[3];
            validateLoginName(loginName);
            Options["Login_Name"] = USI::Option(loginName);
            if (argc > 4) {
                Options["Threads"] = std::stoi(argv[4]);
            }
        }

        Threads.init();
        TT.resize(Options["Hash"]);

        std::unique_ptr<Evaluater> e(new Evaluater);
        e->init(Options["Eval_Dir"], true);

        if (argc > 3) {
            startGodwhaleIo(argv[1], argv[2]);
            USI::loop(1, argv);
            closeGodwhaleIo();
        }
        else {
            USI::loop(argc, argv);
        }
    }
    catch (std::exception &ex) {
        std::cerr << "unhandled exception: " << ex.what() << std::endl;
    }

    Threads.exit();
}

#else

// 将棋を指すソフト
int main(int argc, char* argv[]) {
	initTable();
	Position::initZobrist();
    Search::init();

    USI::init(Options);
    Threads.init();
    TT.resize(Options["Hash"]);

	// 一時オブジェクトの生成と破棄
	std::unique_ptr<Evaluater>(new Evaluater)->init(Options["Eval_Dir"], true);
	USI::loop(argc, argv);
	Threads.exit();
}

#endif
