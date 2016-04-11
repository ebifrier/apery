#include "common.hpp"
#include "bitboard.hpp"
#include "init.hpp"
#include "position.hpp"
#include "usi.hpp"
#include "thread.hpp"
#include "tt.hpp"
#include "search.hpp"

#if defined(GODWHALE_CLUSTER_SLAVE)
#include "godwhaleIo.hpp"
#endif

#if defined FIND_MAGIC
// Magic Bitboard の Magic Number を求める為のソフト
int main() {
	u64 RookMagic[SquareNum];
	u64 BishopMagic[SquareNum];

	std::cout << "const u64 RookMagic[81] = {" << std::endl;
	for (Square sq = I9; sq < SquareNum; ++sq) {
		RookMagic[sq] = findMagic(sq, false);
		std::cout << "\tUINT64_C(0x" << std::hex << RookMagic[sq] << ")," << std::endl;
	}
	std::cout << "};\n" << std::endl;

	std::cout << "const u64 BishopMagic[81] = {" << std::endl;
	for (Square sq = I9; sq < SquareNum; ++sq) {
		BishopMagic[sq] = findMagic(sq, true);
		std::cout << "\tUINT64_C(0x" << std::hex << BishopMagic[sq] << ")," << std::endl;
	}
	std::cout << "};\n" << std::endl;

	return 0;
}

#else
#if defined GODWHALE_CLUSTER_SLAVE
void validateLoginName(const std::string name) {
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
#endif

// 将棋を指すソフト
int main(int argc, char* argv[]) {
#if defined(GODWHALE_CLUSTER_SLAVE)
    if (argc < 4) {
        std::cerr << argv[0] << " host port name [threads] [evalkey]" << std::endl;
        exit(EXIT_FAILURE);
    }

    validateLoginName(argv[3]);
#endif

    std::unique_ptr<Searcher> s;
    std::unique_ptr<Evaluater> e;
    try {
        initTable();
        Position::initZobrist();
        s.reset(new Searcher);
        s->init();
        // 一時オブジェクトの生成と破棄
        e.reset(new Evaluater);

#if defined(GODWHALE_CLUSTER_SLAVE)
        s->options["Login_Name"] = USIOption(argv[3]);
        if (argc > 4) {
            s->options["Threads"] = std::stoi(argv[4]);
        }
        if (argc > 5) {
            e->EvalMemKey = argv[5];
        }
        e->init(s->options["Eval_Dir"], true);

        startGodwhaleIo(argv[1], argv[2]);
        s->doUSICommandLoop(1, argv);
        closeGodwhaleIo();
#else
        e->init(s->options["Eval_Dir"], true);
        s->doUSICommandLoop(argc, argv);
#endif
    }
    catch (std::exception &ex) {
        std::cerr << "unhandled exception: " << ex.what() << std::endl;
    }

    if (s != nullptr) s->threads.exit();
    if (e != nullptr) e->quit();
}

#endif
