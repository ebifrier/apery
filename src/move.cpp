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
    if (!(*this)) return "None";

    const Square from = this->from();
    const Square to = this->to();
    if (this->isDrop())
        return handPieceToString(this->handPieceDropped()) + squareToStringUSI(to);
    std::string usi = squareToStringUSI(from) + squareToStringUSI(to);
    if (this->isPromotion()) usi += "+";
    return usi;
}

std::string Move::toCSA() const {
    if (!(*this)) return "None";

    std::string s = (this->isDrop() ? std::string("00") : squareToStringCSA(this->from()));
    s += squareToStringCSA(this->to()) + pieceTypeToString(this->pieceTypeTo());
    return s;
}

std::string Move::toKIF() const {
    if (!(*this)) return "None";

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
