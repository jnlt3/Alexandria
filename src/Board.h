#pragma once
#ifndef NDEBUG
#define NDEBUG
#endif
#include <cassert>
#include <cctype>

#include "nnue.h"
#include "stdint.h"
#ifdef __GNUC__
#define PACK(__Declaration__) __Declaration__ __attribute__((__packed__))
#endif
#include "move.h"

#ifdef _MSC_VER
#define PACK(__Declaration__) \
    __pragma(pack(push, 1)) __Declaration__ __pragma(pack(pop))
#endif

#define NAME "Alexandria"
#define MAXGAMEMOVES \
    1024 // maximum number of moves possibile,no recorderd game has ever gone
// past 1000 moves so it shoukd be a good approximation
#define MAXDEPTH 128
#define Board_sq_num 64
#define UNDOSIZE MAXGAMEMOVES + MAXDEPTH

// define bitboard data type
#define Bitboard unsigned long long

// set/get/pop bit macros
#define set_bit(bitboard, square) ((bitboard) |= (1ULL << (square)))
#define get_bit(bitboard, square) ((bitboard) & (1ULL << (square)))
#define pop_bit(bitboard, square) ((bitboard) &= ~(1ULL << (square)))
#define clr_bit(bitboard) ((bitboard) &= (bitboard - 1))

#define TEST

#define get_antidiagonal(sq) (get_rank[sq] + get_file[sq])

extern uint8_t PopCnt16[1 << 16];

// FEN dedug positions
#define empty_board "8/8/8/8/8/8/8/8 b - - "
#define start_position \
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "
#define tricky_position \
    "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 "
#define killer_position \
    "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
#define cmk_position \
    "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9 "

// board squares
enum {
	a8, b8, c8, d8, e8, f8, g8, h8,
	a7, b7, c7, d7, e7, f7, g7, h7,
	a6, b6, c6, d6, e6, f6, g6, h6,
	a5, b5, c5, d5, e5, f5, g5, h5,
	a4, b4, c4, d4, e4, f4, g4, h4,
	a3, b3, c3, d3, e3, f3, g3, h3,
	a2, b2, c2, d2, e2, f2, g2, h2,
	a1, b1, c1, d1, e1, f1, g1, h1, no_sq
};

// extract rank from a square [square]
extern const int get_rank[Board_sq_num];

extern const int Color[12];

// extract rank from a square [square]
extern const int get_file[Board_sq_num];

// extract diagonal from a square [square]
extern const int get_diagonal[Board_sq_num];

enum {
	FILE_A,
	FILE_B,
	FILE_C,
	FILE_D,
	FILE_E,
	FILE_F,
	FILE_G,
	FILE_H,
	FILE_NONE
};
enum {
	RANK_1,
	RANK_2,
	RANK_3,
	RANK_4,
	RANK_5,
	RANK_6,
	RANK_7,
	RANK_8,
	RANK_NONE
};

// encode pieces
enum {
	WP,
	WN,
	WB,
	WR,
	WQ,
	WK,
	BP,
	BN,
	BB,
	BR,
	BQ,
	BK,
	EMPTY = 14
};

// sides to move (colors)
enum {
	WHITE,
	BLACK,
	BOTH
};

// bishop and rook
enum {
	rook,
	bishop
};

enum {
	WKCA = 1,
	WQCA = 2,
	BKCA = 4,
	BQCA = 8
};

enum {
	FALSE,
	TRUE
};

enum {
	HFNONE,
	HFALPHA,
	HFBETA,
	HFEXACT
};

// game phase scores
const int opening_phase_score = 6192;
const int endgame_phase_score = 518;

// game phases
enum {
	opening,
	endgame,
	middlegame
};

// piece types
enum {
	PAWN,
	KNIGHT,
	BISHOP,
	ROOK,
	QUEEN,
	KING
};

extern int reductions[256];

PACK(typedef struct HASHENTRY {
	int32_t move = NOMOVE;
	int16_t score = 0;
	uint16_t tt_key = 0;
	uint8_t depth = 0;
	uint8_t flags = HFNONE;
})
S_HASHENTRY;

typedef struct HASHTABLE {
	S_HASHENTRY* pTable;
	int64_t numEntries=0;

} S_HASHTABLE;

typedef struct Undo {
	int move = 0;
	int castlePerm = 15;
	int capture = EMPTY;
	int enPas = 0;
	int fiftyMove = 0;
	int eval = 0;
	Bitboard posKey = 0ULL;
	Bitboard occupancies[3];
} S_Undo; // stores a move and the state of the game before that move is made
// for rollback purposes

typedef struct Board {
	int pieces[Board_sq_num]; // array that stores for every square of the board
	// if there's a piece, or if the square is invalid

	int side=-1; // what side has to move
	int enPas=-1; // if enpassant is possible and in which square
	int fiftyMove=-1; // Counter for the 50 moves rule
	int ply=-1; // number of halfmoves in a search instance
	int hisPly=-1; // total number of halfmoves
	int castleperm=-1; // integer that represents the castling permission in his
	// bits (1111) = all castlings allowed (0000) no castling
	// allowed, (0101) only WKCA and BKCA allowed...

	Bitboard posKey=-1; // unique  hashkey  che codifica the  position on the
	// board,utile per il controllo delle posizioni ripetute.

	S_Undo	history[UNDOSIZE]; // stores every single move and the state of the board
	// when that move was made for rollback purposes

	int pvArray[MAXDEPTH + 1];

	int searchHistory[12][MAXDEPTH];
	int searchKillers[2][MAXDEPTH];
	int checks=-1;
	int excludedMoves[MAXDEPTH];
	Bitboard pinHV=0ULL;
	Bitboard pinD=0ULL;
	Bitboard checkMask=0ULL;

	// piece pos->bitboards
	Bitboard bitboards[12];

	// occupancy pos->bitboards
	Bitboard occupancies[3];

} S_Board;

extern S_HASHTABLE HashTable[1];

extern Bitboard SQUARES_BETWEEN_BB[Board_sq_num][Board_sq_num];

typedef struct info {
	int starttime=-1;
	int stoptime = -1;
	int depth=-1;
	int seldepth = -1;
	bool depthset=false;
	int timeset = -1;
	int movestogo = -1;
	bool infinite=false;

	bool quit=false;
	bool stopped=false;

	long nodes=0;

} S_SearchINFO;

extern int CounterMoves[Board_sq_num][Board_sq_num];
// castling rights update constants
extern const int castling_rights[Board_sq_num];

// convert squares to coordinates
extern const char* square_to_coordinates[];

extern char RankChar[];
extern char FileChar[];

// ASCII pieces
extern const char ascii_pieces[13];

// NNUE
extern NNUE nnue;
extern bool nnue_eval;
// counts how many bits are set in a bitboard
int count_bits(Bitboard bitboard);

// get least significant 1st bit index
int get_ls1b_index(Bitboard bitboard);
int square_distance(int a, int b);

// parse FEN string
void parse_fen(const char* fen, S_Board* pos);

void Reset_info(S_SearchINFO* info);

extern std::vector<std::array<int16_t, HIDDEN_BIAS>> accumulatorStack;

//Pieces info retrival

//Retrieve the pawns on the board of a specific color
Bitboard GetPawnsColorBB(const S_Board* pos, int color);
//Retrieve all the pawns on the board
Bitboard GetPawnsBB(const S_Board* pos);
//Retrieve the knights on the board of a specific color
Bitboard GetKnightsColorBB(const S_Board* pos, int color);
//Retrieve all the pawns on the board
Bitboard GetKnightsBB(const S_Board* pos);
//Retrieve the bishops on the board of a specific color
Bitboard GetBishopsColorBB(const S_Board* pos, int color);
//Retrieve all the bishops on the board
Bitboard GetBishopsBB(const S_Board* pos);
//Retrieve the rooks on the board of a specific color
Bitboard GetRooksColorBB(const S_Board* pos, int color);
//Retrieve all the rooks on the board
Bitboard GetRooksBB(const S_Board* pos);
//Retrieve the queens on the board of a specific color
Bitboard GetQueensColorBB(const S_Board* pos, int color);
//Retrieve all the queens on the board
Bitboard GetQueensBB(const S_Board* pos);
//Retrieve the king on the board of a specific color
Bitboard GetKingColorBB(const S_Board* pos, int color);
//Retrieve the 2 kings on the board
Bitboard GetKingsBB(const S_Board* pos);
//Retrieve a generic piece (useful when we don't know what type of piece we are dealing with
Bitboard GetGenericPiecesColorBB(const S_Board* pos, int piecetype, int color);
//Retrieve a generic piece (useful when we don't know what type of piece we are dealing with
Bitboard GetGenericPiecesBB(const S_Board* pos, int piecetype);
//Return a piece based on the type and the color 
int GetPiece(int piecetype, int color);
//Returns true if side has at least one piece on the board that isn't a pawn, false otherwise
bool BoardHasNonPawns(S_Board* pos, int side);
