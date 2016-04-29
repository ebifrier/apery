#include "move.hpp"

namespace {
	const std::string HandPieceToStringTable[HandPieceNum] = {
        "P*", "L*", "N*", "S*", "G*", "B*", "R*"
    };
	inline std::string handPieceToString(const HandPiece hp) { return HandPieceToStringTable[hp]; }

	const std::string PieceTypeToStringTable[PieceTypeNum] = {
		"",
        "FU", "KY", "KE", "GI", "KA", "HI", "KI", "OU",
        "TO", "NY", "NK", "NG", "UM", "RY"
	};
	inline std::string pieceTypeToString(const PieceType pt) { return PieceTypeToStringTable[pt]; }

    const std::array<std::string, PieceTypeNum> PieceTypeToKIFTable = {
        "　",
        "歩", "香", "桂", "銀", "角", "飛", "金", "玉",
        "と", "杏", "圭", "全", "馬", "竜"
    };
    inline std::string pieceTypeToKIF(const PieceType pt) { return PieceTypeToKIFTable[pt]; }
}

std::string Move::toUSI() const {
	if (this->isNone()) return "None";

	const Square from = this->from();
	const Square to = this->to();
	if (this->isDrop())
		return handPieceToString(this->handPieceDropped()) + squareToStringUSI(to);
	std::string usi = squareToStringUSI(from) + squareToStringUSI(to);
	if (this->isPromotion()) usi += "+";
	return usi;
}

std::string Move::toCSA() const {
	if (this->isNone()) return "None";

	std::string s = (this->isDrop() ? std::string("00") : squareToStringCSA(this->from()));
	s += squareToStringCSA(this->to()) + pieceTypeToString(this->pieceTypeTo());
	return s;
}

std::string Move::toKIF() const {
    if (this->isNone()) return "None";

    // 54杏(34)
    std::string s = squareToStringCSA(this->to());
    if (this->isPromotion()) {
        s += pieceTypeToKIF(this->pieceTypeTo() - PTPromote);
        s += "成";
    }
    else {
        s += pieceTypeToKIF(this->pieceTypeTo());
    }
    s += (this->isDrop() ? "打" : "(" + squareToStringCSA(this->from()) + ")");
    return s;
}