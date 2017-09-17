/*
Rodent, a UCI chess playing engine derived from Sungorus 1.4
Copyright (C) 2009-2011 Pablo Vazquez (Sungorus author)
Copyright (C) 2011-2017 Pawel Koziol

Rodent is free software: you can redistribute it and/or modify it under the terms of the GNU
General Public License as published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

Rodent is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.
If not, see <http://www.gnu.org/licenses/>.
*/

#include "rodent.h"
#include "eval.h"
#include <cstdio>
#include <cstdlib>
#include <cmath>

void cParam::DefaultWeights() {  // tuned automatically

    // Switch off weakening parameters

    search_skill = 10;
    nps_limit = 0;
    fl_weakening = false;
    elo = 2800;
    eval_blur = 0;
    book_depth = 256;

    // Opening book

    use_book = true;
    verbose_book = true; // TODO: change to false in release
    book_filter = 20;

    // Timing

    time_percentage = 100;

    // Piece values

    SetVal(P_MID, 91, 50, 150, true);
    SetVal(N_MID, 305, 200, 400, true);
    SetVal(B_MID, 334, 200, 400, true);
    SetVal(R_MID, 501, 400, 600, true);
    SetVal(Q_MID, 1001, 800, 1200, true);

    SetVal(P_END, 105, 50, 150, true);
    SetVal(N_END, 301, 200, 400, true);
    SetVal(B_END, 315, 200, 400, true);
    SetVal(R_END, 543, 400, 600, true);
    SetVal(Q_END, 1014, 800, 1200, true);

    // Tendency to keep own pieces

    keep_pc[P] = 0;
    keep_pc[N] = 0;
    keep_pc[B] = 0;
    keep_pc[R] = 0;
    keep_pc[Q] = 0;
    keep_pc[K] = 0;
    keep_pc[K + 1] = 0;

    // Material adjustments

    SetVal(B_PAIR, 51, 0, 100, true);
    SetVal(N_PAIR, 0, -50, 50, true);
    SetVal(R_PAIR, -22, -50, 50, true);
    SetVal(ELEPH, 10, -50, 50, true);   // queen loses that much with each enemy minor on the board
    SetVal(A_EXC, 31, -50, 50, true);   // exchange advantage additional bonus
    SetVal(A_MIN, 58, 0, 100, true);    // additional bonus for a minor piece advantage
    SetVal(A_MAJ, 55, 0, 100, true);    // additional bonus for a major piece advantage
    SetVal(A_TWO, 29, 0, 100, true);    // additional bonus for two minors against a rook
    SetVal(A_ALL, 65, 0, 100, true);    // additional bonus for advantage in both majors and minors
    SetVal(N_CL, 6, -50, 50, true);     // knight gains this much with each own pawn present on the board
    SetVal(R_OP, 0, -50, 50, true);     // rook loses that much with each own pawn present on the board  

    // King attack values

    // "_ATT1" values are awarded for attacking squares not defended by enemy pawns
    // "_ATT2" values are awarded for attacking squares defended by enemy pawns
    // "_CHK"  values are awarded for threatening check to enemy king
    // "_CONTACT" values are awarded for contact checks threats
    //
    // All these values are NOT the actual bonuses; their sum is used as index
    // to a non-linear king safety table. Tune them with extreme caution.

    SetVal(N_ATT1, 6,  0, 50, false);
    SetVal(N_ATT2, 4,  0, 50, false);
    SetVal(B_ATT1, 7,  0, 50, false);
    SetVal(B_ATT2, 2,  0, 50, false);
    SetVal(R_ATT1, 10, 0, 50, false);
    SetVal(R_ATT2, 4,  0, 50, false);
    SetVal(Q_ATT1, 16, 0, 50, false);
    SetVal(Q_ATT2, 5,  0, 50, false);

    SetVal(N_CHK,  11, 0, 50, false);
    SetVal(B_CHK,  18, 0, 50, false);
    SetVal(R_CHK,  16, 0, 50, false);
    SetVal(Q_CHK,  12, 0, 50, false);

    SetVal(R_CONTACT, 29, 0, 50, false);
    SetVal(Q_CONTACT, 36, 0, 50, false);

    // King tropism

    SetVal(NTR_MG,  13, -50, 50, true);
    SetVal(NTR_EG, -11, -50, 50, true);
    SetVal(BTR_MG,   2, -50, 50, true);
    SetVal(BTR_EG,  -9, -50, 50, true);
    SetVal(RTR_MG,  -1, -50, 50, true);
    SetVal(RTR_EG,  -7, -50, 50, true);
    SetVal(QTR_MG,   7, -50, 50, true);
    SetVal(QTR_EG,  14, -50, 50, true);

    // Varia

    SetVal(W_MATERIAL, 98,  0, 200, true);
    SetVal(W_PST, 73, 0, 200, true);
    pst_style = 0;
    mob_style = 1;

    // Attack and mobility weights that can be set independently for each side
    // - the core of personality mechanism

    SetVal(W_OWN_ATT, 100, 0, 500, false);
    SetVal(W_OPP_ATT, 100, 0, 500, false);
    SetVal(W_OWN_MOB, 103, 0, 500, false);
    SetVal(W_OPP_MOB, 103, 0, 500, false);

    // Positional weights

    SetVal(W_THREATS, 109, 0, 500, true);
    SetVal(W_TROPISM,  25, 0, 500, true);
    SetVal(W_FWD,       0, 0, 500, false);
    SetVal(W_PASSERS, 102, 0, 500, true);
    SetVal(W_MASS,     98, 0, 500, true);
    SetVal(W_CHAINS,  100, 0, 500, true);
    SetVal(W_OUTPOSTS, 73, 0, 500, true);
    SetVal(W_LINES,   109, 0, 500, true);
    SetVal(W_STRUCT,  113, 0, 500, true);
    SetVal(W_SHIELD,  120, 0, 500, true);
    SetVal(W_STORM,    95, 0, 500, true);
    SetVal(W_CENTER,   48, 0, 500, true);

    // Pawn structure parameters

    SetVal(DB_MID,  -8, -50, 0, true);  // doubled
    SetVal(DB_END, -21, -50, 0, true);

    SetVal(ISO_MG, -7,  -50, 0, true);  // isolated
    SetVal(ISO_EG, -7,  -50, 0, true);
    SetVal(ISO_OF, -13, -50, 0, true);  // additional midgame penalty for isolated pawn on an open file

    SetVal(BK_MID,  -2, -50, 0, true);  // backward
    SetVal(BK_END,  -1, -50, 0, true); 
    SetVal(BK_OPE, -10, -50, 0, true);  // additional midgame penalty for backward pawn on an open file

    SetVal(P_BIND, 2, 0, 50, true);     // two pawns control central square
    SetVal(P_BADBIND, 13, 0, 50, true); // penalty for a "wing triangle" like a4-b3-c4

    SetVal(P_ISL, 5, 0, 50, true);      // penalty for each pawn island
    SetVal(P_THR, 3, 0, 50, true);      // pawn move threatens to attack enemy minor

    // Pawn chain values

    SetVal(P_BIGCHAIN, 38, 0, 50, true);   // general penalty for a compact pawn chain pointing at our king
    SetVal(P_SMALLCHAIN, 27, 0, 50, true); // similar penalty for a chain that is not fully blocked by enemy pawns
    SetVal(P_CS1, 12, 0, 50, true); // additional bonus for a pawn storm next to a fixed chain - like g5 in King's Indian
    SetVal(P_CS2, 3, 0, 50, true); // as above, this time like g4 in King's Indian
    SetVal(P_CSFAIL, 32, 0, 50, true); // penalty for misplaying pawn strom next to a chain

    // Passed pawn bonuses per rank

    SetVal(PMG2,   2, 0, 300, true);
    SetVal(PMG3,   2, 0, 300, true);
    SetVal(PMG4,  11, 0, 300, true);
    SetVal(PMG5,  33, 0, 300, true);
    SetVal(PMG6,  71, 0, 300, true);
    SetVal(PMG7, 135, 0, 300, true);

    SetVal(PEG2,  12, 0, 300, true);
    SetVal(PEG3,  21, 0, 300, true);
    SetVal(PEG4,  48, 0, 300, true);
    SetVal(PEG5,  93, 0, 300, true);
    SetVal(PEG6, 161, 0, 300, true);
    SetVal(PEG7, 266, 0, 300, true);

    // Passed pawn value percentage modifiers

    SetVal(P_BL_MUL, 42, 0, 50, true);      // blocked passer
    SetVal(P_OURSTOP_MUL, 27, 0, 50, true); // side with a passer controls its stop square
    SetVal(P_OPPSTOP_MUL, 29, 0, 50, true); // side playing against a passer controls its stop square
    SetVal(P_DEFMUL, 6, 0, 50, true);       // passer defended by own pawn
    SetVal(P_STOPMUL, 6, 0, 50, true);      // passers' stop square defended by own pawn

    // King's pawn shield

    SetVal(P_SH_NONE, -40, -50, 50, true);
    SetVal(P_SH_2,   2, -50, 50, true);
    SetVal(P_SH_3,  -6, -50, 50, true);
    SetVal(P_SH_4, -15, -50, 50, true);
    SetVal(P_SH_5, -23, -50, 50, true);
    SetVal(P_SH_6, -24, -50, 50, true);
    SetVal(P_SH_7, -35, -50, 50, true);

    // Pawn storm

    SetVal(P_ST_OPEN, -6, -50, 50, true);
    SetVal(P_ST_3, -16, -50, 50, true);
    SetVal(P_ST_4, -16, -50, 50, true);
    SetVal(P_ST_5, -3, -50, 50, true);

    // Knight parameters

    SetVal(N_TRAP, -168, -300, 0, true); // trapped knight
    SetVal(N_BLOCK, -17, -50, 0, true);  // knight blocks c pawn in queen pawn openings
    SetVal(N_OWH, -1, -50, 0, true);     // knight can move only to own half of the board
    SetVal(N_REACH, 11, 0, 50, true);    // knight can reach an outpost square
    SetVal(BN_SHIELD,  5,  0, 50, true); // pawn in front of a minor

    // Bishop parameters

    SetVal(B_FIANCH, 13, 0, 50, true);   // general bonus for fianchettoed bishop
    SetVal(B_KING, 20, 0, 50, true);     // fianchettoed bishop near own king
    SetVal(B_BADF, -27, -50, 0, true);   // enemy pawns hamper fianchettoed bishop 
    SetVal(B_TRAP_A2, -138, -300, 0, true);
    SetVal(B_TRAP_A3, -45, -300, 0, true);
    SetVal(B_BLOCK, -45, -100, 0, true); // blocked pawn at d2/e2 hampers bishop's development
    SetVal(B_BF_MG, -12, -50, 0, true);  // fianchettoed bishop blocked by own pawn (ie. Bg2, Pf3)
    SetVal(B_BF_EG, -20, -50, 0, true);
    SetVal(B_WING, 3, 0, 50, true);      // bishop on "expected" wing (ie. Pe4, Bc5/b5/a4/b3/c2) 
    SetVal(B_OVH, -7, -50, 0, true);     // bishop can move only to own half of the board
    SetVal(B_REACH, 2, 0, 50, true);     // bishop can reach an outpost square
    SetVal(B_TOUCH, 5, 0, 50, true);     // two bishops on adjacent squares
    SetVal(B_OWN_P, -3, -50, 0, false);   // own pawn on the square of own bishop's color
    SetVal(B_OPP_P, -1, -50, 0, false);   // enemy pawn on the square of own bishop's color
    SetVal(B_RETURN, 7, 0, 50, true);    // bishop returning to initial position after castling

    // Rook parameters

    SetVal(RSR_MG, 16, 0, 50, true); // rook on the 7th rank
    SetVal(RSR_EG, 32, 0, 50, true);
    SetVal(RS2_MG, 20, 0, 50, true); // additional bonus for two rooks on 7th rank
    SetVal(RS2_EG, 31, 0, 50, true);
    SetVal(ROF_MG, 30, 0, 50, true); // rook on open file
    SetVal(ROF_EG,  2, 0, 50, true);
    SetVal(RGH_MG, 15, 0, 50, true); // rook on half-open file with undefended enemy pawn
    SetVal(RGH_EG, 20, 0, 50, true);
    SetVal(RBH_MG,  0, 0, 50, true); // rook on half-open file with defended enemy pawn
    SetVal(RBH_EG,  0, 0, 50, true);
    SetVal(ROQ_MG,  9, 0, 50, true); // rook and queen on the same file, open or closed
    SetVal(ROQ_EG, 18, 0, 50, true);
    SetVal(R_BLOCK, -50, -100, 0, true);

    // Queen parameters

    SetVal(QSR_MG, 0, 0, 50, true);       // queen on the 7th rank
    SetVal(QSR_EG, 2, 0, 50, true);

    // King parameters

    SetVal(K_NO_LUFT, -11, -50, 0, true); // queen on the 7th rank
    SetVal(K_CASTLE, 32, 0, 50, true);

    // Forwardness parameters

    SetVal(N_FWD,   1, 0, 50, false);
    SetVal(B_FWD,   1, 0, 50, false);
    SetVal(R_FWD,   2, 0, 50, false);
    SetVal(Q_FWD,   4, 0, 50, false);

    // Mobility

    SetVal(NMG0, -32, -50, 50, true);
    SetVal(NMG1, -14, -50, 50, true);
    SetVal(NMG2,  -7, -50, 50, true);
    SetVal(NMG3,  -7, -50, 50, true);
    SetVal(NMG4,   2, -50, 50, true);
    SetVal(NMG5,   7, -50, 50, true);
    SetVal(NMG6,  13, -50, 50, true);
    SetVal(NMG7,  13, -50, 50, true);
    SetVal(NMG8,  25, -50, 50, true);

    SetVal(NEG0, -41, -50, 50, true);
    SetVal(NEG1, -20, -50, 50, true);
    SetVal(NEG2,  -7, -50, 50, true);
    SetVal(NEG3,   0, -50, 50, true);
    SetVal(NEG4,   3, -50, 50, true);
    SetVal(NEG5,  12, -50, 50, true);
    SetVal(NEG6,   9, -50, 50, true);
    SetVal(NEG7,  11, -50, 50, true);
    SetVal(NEG8,   2, -50, 50, true);

    SetVal(BMG0, -41, -50, 50, true);
    SetVal(BMG1, -24, -50, 50, true);
    SetVal(BMG2, -16, -50, 50, true);
    SetVal(BMG3,  -9, -50, 50, true);
    SetVal(BMG4,  -7, -50, 50, true);
    SetVal(BMG5,   0, -50, 50, true);
    SetVal(BMG6,   4, -50, 50, true);
    SetVal(BMG7,   6, -50, 50, true);
    SetVal(BMG8,   8, -50, 50, true);
    SetVal(BMG9,  10, -50, 50, true);
    SetVal(BMG10, 16, -50, 50, true);
    SetVal(BMG11, 24, -50, 50, true);
    SetVal(BMG12, 17, -50, 50, true);
    SetVal(BMG13, 22, -50, 50, true);

    SetVal(BEG0, -43, -50, 50, true);
    SetVal(BEG1, -40, -50, 50, true);
    SetVal(BEG2, -19, -50, 50, true);
    SetVal(BEG3,  -6, -50, 50, true);
    SetVal(BEG4,   1, -50, 50, true);
    SetVal(BEG5,   3, -50, 50, true);
    SetVal(BEG6,   5, -50, 50, true);
    SetVal(BEG7,   8, -50, 50, true);
    SetVal(BEG8,  15, -50, 50, true);
    SetVal(BEG9,  11, -50, 50, true);
    SetVal(BEG10, 10, -50, 50, true);
    SetVal(BEG11, 13, -50, 50, true);
    SetVal(BEG12, 22, -50, 50, true);
    SetVal(BEG13, 19, -50, 50, true);

    SetVal(RMG0, -14, -50, 50, true);
    SetVal(RMG1, -16, -50, 50, true);
    SetVal(RMG2, -14, -50, 50, true);
    SetVal(RMG3,  -9, -50, 50, true);
    SetVal(RMG4,  -9, -50, 50, true);
    SetVal(RMG5, -10, -50, 50, true);
    SetVal(RMG6,  -5, -50, 50, true);
    SetVal(RMG7,  -2, -50, 50, true);
    SetVal(RMG8,  -3, -50, 50, true);
    SetVal(RMG9,  -2, -50, 50, true);
    SetVal(RMG10,  5, -50, 50, true);
    SetVal(RMG11,  7, -50, 50, true);
    SetVal(RMG12,  9, -50, 50, true);
    SetVal(RMG13, 23, -50, 50, true);
    SetVal(RMG14, 24, -50, 50, true);

    SetVal(REG0, -28, -50, 50, true);
    SetVal(REG1, -50, -50, 50, true);
    SetVal(REG2, -38, -50, 50, true);
    SetVal(REG3, -14, -50, 50, true);
    SetVal(REG4, -9, -50, 50, true);
    SetVal(REG5,  1, -50, 50, true);
    SetVal(REG6,  2, -50, 50, true);
    SetVal(REG7,  8, -50, 50, true);
    SetVal(REG8,  9, -50, 50, true);
    SetVal(REG9,  15, -50, 50, true);
    SetVal(REG10, 18, -50, 50, true);
    SetVal(REG11, 22, -50, 50, true);
    SetVal(REG12, 22, -50, 50, true);
    SetVal(REG13, 24, -50, 50, true);
    SetVal(REG14, 29, -50, 50, true);


#ifdef USE_RISKY_PARAMETER
    riskydepth = 0;
#endif
    draw_score = 0;
    shut_up = false;       // true suppresses displaying info currmove etc.

    // Specialized functions

    InitPst();
    InitMobility();
    InitMaterialTweaks();
    InitBackward();
    InitPassers();

    // History limit to prunings and reductions

    hist_perc = 175;
    hist_limit = 24576;
}

void cParam::InitialPersonalityWeights() { // tuned manually for good experience of Rodent personalities

    // Switch off weakening parameters

    search_skill = 10;
    nps_limit = 0;
    fl_weakening = false;
    elo = 2800;
    eval_blur = 0;
    book_depth = 256;

    // Opening book

    use_book = true;
    verbose_book = true; // TODO: change to false in release
    book_filter = 20;

    // Timing

    time_percentage = 100;

    // Piece values

    values[P_MID] = 95;
    values[N_MID] = 310;
    values[B_MID] = 322;
    values[R_MID] = 514;
    values[Q_MID] = 1000;

    values[P_END] = 110;
    values[N_END] = 305;
    values[B_END] = 320;
    values[R_END] = 529;
    values[Q_END] = 1013;

    // Tendency to keep own pieces

    keep_pc[P] = 0;
    keep_pc[N] = 0;
    keep_pc[B] = 0;
    keep_pc[R] = 0;
    keep_pc[Q] = 0;
    keep_pc[K] = 0;
    keep_pc[K + 1] = 0;

    // Material adjustments

    values[B_PAIR]  = 51;
    values[N_PAIR]  = -9;
    values[R_PAIR]  = -9;
    values[ELEPH]  = 4;  // queen loses that much with each enemy minor on the board
    values[A_EXC]  = 30; // exchange advantage additional bonus
    values[A_MIN] = 53;  // additional bonus for minor piece advantage
    values[A_MAJ] = 60;  // additional bonus for major piece advantage
    values[A_TWO] = 44;  // additional bonus for two minors for a rook
    values[A_ALL] = 80;  // additional bonus for advantage in both majors and minors
    values[N_CL]  = 6;   // knight gains this much with each own pawn present on th board
    values[R_OP]  = 3;   // rook loses that much with each own pawn present on the board

    // King attack values

    // "_ATT1" values are awarded for attacking squares not defended by enemy pawns
    // "_ATT2" values are awarded for attacking squares defended by enemy pawns
    // "_CHK"  values are awarded for threatening check to enemy king
    // "_CONTACT" values are awarded for contact checks threats
    //
    // All these values are NOT the actual bonuses; their sum is used as index
    // to a non-linear king safety table. Tune them with extreme caution.

    values[N_ATT1] = 6;
    values[N_ATT2] = 3;
    values[B_ATT1] = 6;
    values[B_ATT2] = 2;
    values[R_ATT1] = 9;
    values[R_ATT2] = 4;
    values[Q_ATT1] = 16;
    values[Q_ATT2] = 5;

    values[N_CHK] = 4;
    values[B_CHK] = 6;
    values[R_CHK] = 11;
    values[Q_CHK] = 12;

    values[R_CONTACT] = 24;
    values[Q_CONTACT] = 36;

    // King tropism

    values[NTR_MG] = 3;
    values[NTR_EG] = 3;
    values[BTR_MG] = 2;
    values[BTR_EG] = 1;
    values[RTR_MG] = 2;
    values[RTR_EG] = 1;
    values[QTR_MG] = 2;
    values[QTR_EG] = 4;

    // Varia

    values[W_MATERIAL] = 100;
    values[W_PST] = 75;
    pst_style = 0;
    mob_style = 0;         // 1 is only marginally behind

#ifdef USE_RISKY_PARAMETER
    riskydepth = 0;
#endif
    draw_score = 0;
    shut_up = false;       // true suppresses displaying info currmove etc.

    // Attack and mobility weights that can be set independently for each side
    // - the core of personality mechanism

    values[W_OWN_ATT] = 100;
    values[W_OPP_ATT] = 100;
    values[W_OWN_MOB] = 100;
    values[W_OPP_MOB] = 100;

    // Positional weights

    values[W_THREATS] = 109;
    values[W_TROPISM] = 20;
    values[W_FWD] = 0;
    values[W_PASSERS] = 100;
    values[W_MASS] = 100; // seems optimal
    values[W_CHAINS] = 100;
    values[W_OUTPOSTS] = 78;
    values[W_LINES] = 100;
    values[W_STRUCT] = 100;
    values[W_SHIELD] = 119;
    values[W_STORM] = 99;
    values[W_CENTER] = 50;

    // Pawn structure parameters

    values[DB_MID] = -12;  // doubled
    values[DB_END] = -23;
    values[ISO_MG] = -10;  // isolated
    values[ISO_EG] = -20;
    values[ISO_OF] = -10;  // additional midgame penalty for isolated pawn on an open file
    values[BK_MID] = -8;   // backward
    values[BK_END] = -8;
    values[BK_OPE] = -10;  // additional midgame penalty for backward pawn on an open file
    values[P_BIND] = 5;    // two pawns control central square
    values[P_BADBIND] = 10; // penalty for a "wing triangle" like a4-b3-c4
    values[P_ISL] = 7;     // penalty for each pawn island
    values[P_THR] = 4;     // pawn move threatens to attack enemy minor

    // Pawn chain values

    values[P_BIGCHAIN] = 18; // general penalty for a compact pawn chain pointing at our king
    values[P_SMALLCHAIN] = 13; // similar penalty for a chain that is not fully blocked by enemy pawns
    values[P_CS1] = 4;         // additional evaluation of a pawn storm next to a fixed chain - like g5 in King's Indian
    values[P_CS2] = 12;        // as above, this time like g4 in King's Indian
    values[P_CSFAIL] = 10;     // penalty for a badly performed pawn strom next to a chain


    // Passed pawn bonuses per rank

    values[PMG2] = 11;
    values[PMG3] = 12;
    values[PMG4] = 24;
    values[PMG5] = 45;
    values[PMG6] = 78;
    values[PMG7] = 130;

    values[PEG2] = 22;
    values[PEG3] = 23;
    values[PEG4] = 57;
    values[PEG5] = 96;
    values[PEG6] = 161;
    values[PEG7] = 260;

    // Passed pawn value percentage modifiers

    values[P_BL_MUL] = 24;      // blocked passer
    values[P_OURSTOP_MUL] = 14; // side with a passer controls its stop square
    values[P_OPPSTOP_MUL] = 10; // side without a passer controls its stop square
    values[P_DEFMUL] = 4;       // passer defended by own pawn
    values[P_STOPMUL] = 4;      // passers' stop square defended by own pawn

    // King's pawn shield

    values[P_SH_NONE] = -36;
    values[P_SH_2]    =   2;
    values[P_SH_3]    = -11;
    values[P_SH_4]    = -20;
    values[P_SH_5]    = -27;
    values[P_SH_6]    = -32;
    values[P_SH_7]    = -35;

    // Pawn storm

    values[P_ST_OPEN] = -16;
    values[P_ST_3] = -32;
    values[P_ST_4] = -16;
    values[P_ST_5] = -8;

    // Knight parameters

    values[N_TRAP] = -150; // trapped knight
    values[N_BLOCK] = -20; // knight blocks c pawn in queen pawn openings
    values[N_OWH] = -5;    // knight can move only to own half of the board
    values[N_REACH] = 4;   // knight can reach an outpost square WAS 2
    values[BN_SHIELD] = 5;

    // Bishop parameters

    values[B_FIANCH] = 4;  // general bonus for fianchettoed bishop
    values[B_KING] = 6;    // fianchettoed bishop near king: 0
    values[B_BADF] = -20;  // enemy pawns hamper fianchettoed bishop
    values[B_TRAP_A2] = -150;
    values[B_TRAP_A3] = -50;
    values[B_BLOCK] = -50; // blocked pawn at d2/e2 hampers bishop's development
    values[B_BF_MG] = -10; // fianchettoed bishop blocked by own pawn (ie. Bg2, Pf3)
    values[B_BF_EG] = -20;
    values[B_WING] = 10;   // bishop on "expected" wing (ie. Pe4, Bc5/b5/a4/b3/c2)
    values[B_OVH] = -5;    // bishop can move only to own half of the board
    values[B_REACH] = 2;   // bishop can reach an outpost square
    values[B_TOUCH] = 4;   // two bishops on adjacent squares
    values[B_OWN_P] = -3;  // own pawn on the square of own bishop's color
    values[B_OPP_P] = -1;  // enemy pawn on the square of own bishop's color
    values[B_RETURN] = 10; // bishop returning to initial position after castling

    // Rook parameters

    values[RSR_MG] = 16; // rook on 7th rank
    values[RSR_EG] = 32;
    values[RS2_MG] = 8;  // additional bonus for two rooks on 7th rank
    values[RS2_EG] = 16;
    values[ROF_MG] = 14; // rook on open file
    values[ROF_EG] = 14;
    values[RGH_MG] = 7;  // rook on half-open file with undefended enemy pawn
    values[RGH_EG] = 7;
    values[RBH_MG] = 5;  // rook on half-open file with defended enemy pawn
    values[RBH_EG] = 5;
    values[ROQ_MG] = 5;  // rook and queen on the same file, open or closed
    values[ROQ_EG] = 5;
    values[R_BLOCK] = -50;

    // Queen parameters

    values[QSR_MG] = 4;  // queen on the 7th rank
    values[QSR_EG] = 8;

    // King parameters

    values[K_NO_LUFT] = -15;
    values[K_CASTLE] = 10;

    // Forwardness parameters

    values[N_FWD] = 1;
    values[B_FWD] = 1;
    values[R_FWD] = 2;
    values[Q_FWD] = 4;

    // Specialized functions

    InitPst();
    InitMobility();
    InitMaterialTweaks();
    InitBackward();
    InitPassers();

    // History limit to prunings and reductions

    hist_perc = 175;
    hist_limit = 24576;

    // when testing a personality, place changes in relation to default below:

}

void cParam::InitPassers() {

    passed_bonus_mg[WC][0] = 0;                passed_bonus_mg[BC][7] = 0;
    passed_bonus_mg[WC][1] = values[PMG2];     passed_bonus_mg[BC][6] = values[PMG2];
    passed_bonus_mg[WC][2] = values[PMG3];     passed_bonus_mg[BC][5] = values[PMG3];
    passed_bonus_mg[WC][3] = values[PMG4];     passed_bonus_mg[BC][4] = values[PMG4];
    passed_bonus_mg[WC][4] = values[PMG5];     passed_bonus_mg[BC][3] = values[PMG5];
    passed_bonus_mg[WC][5] = values[PMG6];     passed_bonus_mg[BC][2] = values[PMG6];
    passed_bonus_mg[WC][6] = values[PMG7];     passed_bonus_mg[BC][1] = values[PMG7];
    passed_bonus_mg[WC][7] = 0;                passed_bonus_mg[BC][0] = 0;

    passed_bonus_eg[WC][0] = 0;                passed_bonus_eg[BC][7] = 0;
    passed_bonus_eg[WC][1] = values[PEG2];     passed_bonus_eg[BC][6] = values[PEG2];
    passed_bonus_eg[WC][2] = values[PEG3];     passed_bonus_eg[BC][5] = values[PEG3];
    passed_bonus_eg[WC][3] = values[PEG4];     passed_bonus_eg[BC][4] = values[PEG4];
    passed_bonus_eg[WC][4] = values[PEG5];     passed_bonus_eg[BC][3] = values[PEG5];
    passed_bonus_eg[WC][5] = values[PEG6];     passed_bonus_eg[BC][2] = values[PEG6];
    passed_bonus_eg[WC][6] = values[PEG7];     passed_bonus_eg[BC][1] = values[PEG7];
    passed_bonus_eg[WC][7] = 0;                passed_bonus_eg[BC][0] = 0;
}

void cParam::InitBackward() {

    // add file-dependent component to backward pawns penalty
    // (assuming backward pawns on central files are bigger liability)

    backward_malus_mg[FILE_A] = values[BK_MID] + 3;
    backward_malus_mg[FILE_B] = values[BK_MID] + 1;
    backward_malus_mg[FILE_C] = values[BK_MID] - 1;
    backward_malus_mg[FILE_D] = values[BK_MID] - 3;
    backward_malus_mg[FILE_E] = values[BK_MID] - 3;
    backward_malus_mg[FILE_F] = values[BK_MID] - 1;
    backward_malus_mg[FILE_G] = values[BK_MID] + 1;
    backward_malus_mg[FILE_H] = values[BK_MID] + 3;
}

void cParam::InitPst() {

    for (int sq = 0; sq < 64; sq++) {
        for (int sd = 0; sd < 2; sd++) {

            mg_pst[sd][P][REL_SQ(sq, sd)] = ((values[P_MID] * Par.values[W_MATERIAL]) / 100) + (pstPawnMg  [pst_style][sq] * Par.values[W_PST]) / 100;
            eg_pst[sd][P][REL_SQ(sq, sd)] = ((values[P_END] * Par.values[W_MATERIAL]) / 100) + (pstPawnEg  [pst_style][sq] * Par.values[W_PST]) / 100;
            mg_pst[sd][N][REL_SQ(sq, sd)] = ((values[N_MID] * Par.values[W_MATERIAL]) / 100) + (pstKnightMg[pst_style][sq] * Par.values[W_PST]) / 100;
            eg_pst[sd][N][REL_SQ(sq, sd)] = ((values[N_END] * Par.values[W_MATERIAL]) / 100) + (pstKnightEg[pst_style][sq] * Par.values[W_PST]) / 100;
            mg_pst[sd][B][REL_SQ(sq, sd)] = ((values[B_MID] * Par.values[W_MATERIAL]) / 100) + (pstBishopMg[pst_style][sq] * Par.values[W_PST]) / 100;
            eg_pst[sd][B][REL_SQ(sq, sd)] = ((values[B_END] * Par.values[W_MATERIAL]) / 100) + (pstBishopEg[pst_style][sq] * Par.values[W_PST]) / 100;
            mg_pst[sd][R][REL_SQ(sq, sd)] = ((values[R_MID] * Par.values[W_MATERIAL]) / 100) + (pstRookMg  [pst_style][sq] * Par.values[W_PST]) / 100;
            eg_pst[sd][R][REL_SQ(sq, sd)] = ((values[R_END] * Par.values[W_MATERIAL]) / 100) + (pstRookEg  [pst_style][sq] * Par.values[W_PST]) / 100;
            mg_pst[sd][Q][REL_SQ(sq, sd)] = ((values[Q_MID] * Par.values[W_MATERIAL]) / 100) + (pstQueenMg [pst_style][sq] * Par.values[W_PST]) / 100;
            eg_pst[sd][Q][REL_SQ(sq, sd)] = ((values[Q_END] * Par.values[W_MATERIAL]) / 100) + (pstQueenEg [pst_style][sq] * Par.values[W_PST]) / 100;
            mg_pst[sd][K][REL_SQ(sq, sd)] = (pstKingMg  [pst_style][sq] * Par.values[W_PST]) / 100;
            eg_pst[sd][K][REL_SQ(sq, sd)] = (pstKingEg  [pst_style][sq] * Par.values[W_PST]) / 100;

            sp_pst[sd][N][REL_SQ(sq, sd)] = pstKnightOutpost[sq];
            sp_pst[sd][B][REL_SQ(sq, sd)] = pstBishopOutpost[sq];
            sp_pst[sd][DEF_MG][REL_SQ(sq, sd)] = pstDefendedPawnMg[sq];
            sp_pst[sd][PHA_MG][REL_SQ(sq, sd)] = pstPhalanxPawnMg[sq];
            sp_pst[sd][DEF_EG][REL_SQ(sq, sd)] = pstDefendedPawnEg[sq];
            sp_pst[sd][PHA_EG][REL_SQ(sq, sd)] = pstPhalanxPawnEg[sq];
        }
    }
}

void cParam::InitMobility() {

    for (int i = 0; i < 9; i++) {
        n_mob_mg[i] = Par.mob_style == 0 ? 4 * (i - 4) : values[NMG0+i]; 
        n_mob_eg[i] = Par.mob_style == 0 ? 4 * (i - 4) : values[NEG0+i];
    }

    for (int i = 0; i < 14; i++) {
        b_mob_mg[i] = Par.mob_style == 0 ? 5 * (i - 6) : values[BMG0+i];
        b_mob_eg[i] = Par.mob_style == 0 ? 5 * (i - 6) : values[BEG0+i];
    }

    for (int i = 0; i < 15; i++) {
        r_mob_mg[i] = Par.mob_style == 0 ? 2 * (i - 7) : values[RMG0+i];
        r_mob_eg[i] = Par.mob_style == 0 ? 4 * (i - 7) : values[REG0+i];
    }

    for (int i = 0; i < 28; i++) {
        q_mob_mg[i] = Par.mob_style == 0 ? 1 * (i - 14) : 1 * (i - 14);
        q_mob_eg[i] = Par.mob_style == 0 ? 2 * (i - 14) : 2 * (i - 14);
    }

}

void cParam::InitMaterialTweaks() {

    // Init tables for adjusting piece values
    // according to the number of own pawns

    for (int i = 0; i < 9; i++) {
        np_table[i] = adj[i] * values[N_CL];
        rp_table[i] = adj[i] * values[R_OP];
    }

    // Init imbalance table, so that we can expose option for exchange delta

    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {

            // insert original values
            imbalance[i][j] = imbalance_data[i][j];

            // insert value defined in Par.values

            if (imbalance_data[i][j] == A_EXC) imbalance[i][j] = values[A_EXC];
            if (imbalance_data[i][j] == -A_EXC) imbalance[i][j] = -values[A_EXC];
            if (imbalance_data[i][j] == A_MIN) imbalance[i][j] = values[A_MIN];
            if (imbalance_data[i][j] == -A_MIN) imbalance[i][j] = -values[A_MIN];
            if (imbalance_data[i][j] == A_MAJ) imbalance[i][j] = values[A_MAJ];
            if (imbalance_data[i][j] == -A_MAJ) imbalance[i][j] = -values[A_MAJ];
            if (imbalance_data[i][j] == A_TWO) imbalance[i][j] = values[A_TWO];
            if (imbalance_data[i][j] == -A_TWO) imbalance[i][j] = -values[A_TWO];
            if (imbalance_data[i][j] == A_ALL) imbalance[i][j] = values[A_ALL];
            if (imbalance_data[i][j] == -A_ALL) imbalance[i][j] = -values[A_ALL];
        }
    }
}

void cParam::InitTables() {

    // Init king attack table

    for (int t = 0, i = 1; i < 511; ++i) {
        t = (int)Min(1280.0, Min((0.027 * i * i), t + 8.0));
        danger[i] = (t * 100) / 256; // rescale to centipawns
    }
}

void cParam::SetSpeed(int elo_in) {
    nps_limit = 0;
    eval_blur = 0;

    if (fl_weakening) {
        nps_limit = EloToSpeed(elo_in);
        eval_blur = EloToBlur(elo_in);
        book_depth = EloToBookDepth(elo_in);
    }
}

int cParam::EloToSpeed(int elo_in) {

    // this formula abuses Michael Byrne's code from CraftySkill.
    // He used  it to calculate max nodes per elo. By  dividing,
    // I derive speed that yields similar result in standard blitz.
    // Formula has a little bit of built-in randomness.

    const int lower_elo = elo_in - 25;
    const int upper_elo = elo_in + 25;
    int use_rating = rand() % (upper_elo - lower_elo + 1) + lower_elo;
    int search_nodes = (int)(pow(1.0069555500567, (((use_rating) / 1200) - 1)
                             + (use_rating - 1200)) * 128);

    return search_nodes / 7;
}

int cParam::EloToBlur(int elo_in) {

    // Weaker levels get their evaluation blurred

    if (elo_in < 1500) return (1500 - elo_in) / 4;
    return 0;
}

int cParam::EloToBookDepth(int elo_in) {
    if (elo_in < 2000) return (elo_in - 700) / 100;
    return 256;
}

void cDistance::Init() {

    // Init distance tables

    for (int sq1 = 0; sq1 < 64; ++sq1) {
        for (int sq2 = 0; sq2 < 64; ++sq2) {
            int r_delta = Abs(Rank(sq1) - Rank(sq2));
            int f_delta = Abs(File(sq1) - File(sq2));
            bonus[sq1][sq2] = 14 - (r_delta + f_delta);  // for king tropism evaluation
            metric[sq1][sq2] = Max(r_delta, f_delta);    // chebyshev distance for unstoppable passers
        }
    }
}

void cParam::SetVal(int slot, int val, int min, int max, bool tune) {

    values[slot] = val;
    min_val[slot] = min;
    max_val[slot] = max;
    tunable[slot] = tune;
    if (val < min || val > max)
        printf("%14s ERROR\n", paramNames[slot]);
}

void cParam::PrintValues() {

    int iter = 0;

    printf("\n\n");
    for (int i = 0; i < N_OF_VAL; ++i) {
        if (tunable[i] == true) {
            printf("%14s : %4d     ", paramNames[i], Par.values[i]);
            iter++;
            if (iter % 4 == 0) printf("\n");
        }
    }
    printf("\n\n");
}