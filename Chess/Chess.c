// This code is for the computer to play itself,
// though it could be easily changed to involve
// human vs. computer play
//
// TODO: Improve the AI

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// ChessPiece Class

enum SideTypes {BLACK = -1, NONE, WHITE};
enum PieceTypes {PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING};
enum CastlingTypes {NOCASTLE, LEFTCASTLE, RIGHTCASTLE};

struct ChessPiece{
    int loc;
    int side;
    int type;
    bool canCastle;
};

void newChessPiece(struct ChessPiece* chessPiece, int loc, int side, int type, bool canCastle);
struct ChessPiece copyChessPiece(struct ChessPiece* chessPiece);
void promote(struct ChessPiece* chessPiece, int type);
void movePiece(struct ChessPiece* board, struct ChessPiece* chessPiece, int newLoc);
void captured(struct ChessPiece* board, struct ChessPiece* chessPiece);
bool canPromote(struct ChessPiece* chessPiece, int loc);
char* getChessPieceSymbol(struct ChessPiece* chessPiece);
bool canCastle(struct ChessPiece* board, int side, int castlingType);
struct ChessMove* getPossibleMoves(struct ChessPiece* board, struct ChessPiece* chessPiece);

// ChessMove Class

struct ChessMove{
    int source;
    int destination;
    int capturedLoc;
    int castlingType;
    int promotionType;
};

static struct ChessMove nullMove;

void newChessMove(struct ChessMove *move, int source, int destination, int capturedLoc, int castlingType, int promotionType);
struct ChessMove copyChessMove(struct ChessMove *move);
struct ChessMove makeNullChessMove();

struct ChessPiece board[64];

void resetBoard();
void drawBoard(struct ChessPiece* board);
struct ChessPiece* copyBoard(struct ChessPiece* board);
bool isSpaceFree(struct ChessPiece* board, int loc);
bool isSpaceEnemyOccupied(struct ChessPiece* board, int loc, int side);
struct ChessPiece* getPiece(struct ChessPiece* board, int loc);
void apply(struct ChessPiece* board, struct ChessMove* chessMove);
void castle(struct ChessPiece* board, int side, int castlingType);
bool isAttacked(struct ChessPiece* board, int loc, int side);

// Game and AI Methods
int getKingLocation(struct ChessPiece* board, int side);
struct ChessMove* getAllPossibleMoves(struct ChessPiece* board, int side);
bool isStalemate(struct ChessPiece* board, int side);
bool isCheckmate(struct ChessPiece* board, int side);

double evalBoard(struct ChessPiece* board, int side);
struct ChessMove findBestMove(struct ChessPiece* board, int alpha, int beta, int depth, int side);

void main(){
    resetBoard();
    nullMove = makeNullChessMove();

    int turn = WHITE;
    struct ChessMove nextMove = findBestMove(board, INT_MIN, INT_MAX, 1, turn);

    while(nextMove.source != -1){
        printf("Move: %d, %d, %d, %d, %d\n", nextMove.source, nextMove.destination, nextMove.capturedLoc, nextMove.castlingType, nextMove.promotionType);
        apply(board, &nextMove);
        drawBoard(board);

        char ENTER;
        printf("\nPress ENTER to continue:");
        scanf("%c", &ENTER);

        turn = -turn;
        nextMove = findBestMove(board, INT_MIN, INT_MAX, 1, turn);
    }
}

void newChessPiece(struct ChessPiece* chessPiece, int loc, int side, int type, bool canCastle){
    chessPiece->loc = loc;
    chessPiece->side = side;
    chessPiece->type = type;
    chessPiece->canCastle = canCastle;
}

struct ChessPiece copyChessPiece(struct ChessPiece* chessPiece){
    struct ChessPiece copyChessPiece;
    newChessPiece(&copyChessPiece, chessPiece->loc, chessPiece->side, chessPiece->type, chessPiece->canCastle);

    return copyChessPiece;
}

void promote(struct ChessPiece* chessPiece, int type){
    if((chessPiece->type) == PAWN && type != PAWN && type != KING){
        chessPiece->type = type;
    }
}

void movePiece(struct ChessPiece* board, struct ChessPiece* chessPiece, int newLoc){
    int origLoc = chessPiece->loc;
    int pieceType = chessPiece->type;

    chessPiece->loc = newLoc;
    struct ChessPiece chessPieceCopy = copyChessPiece(chessPiece);

    if(pieceType == KING || pieceType == ROOK){
        int kingLoc = getKingLocation(board, chessPiece->side);
        (*(board + kingLoc)).canCastle = false;
    }

    (*(board + origLoc)).side = NONE;
    *(board + newLoc) = chessPieceCopy;
}

void captured(struct ChessPiece* board, struct ChessPiece* chessPiece){
    (*(board + (chessPiece->loc))).side = NONE;
}

bool canPromote(struct ChessPiece* chessPiece, int loc){
    if(chessPiece->type == PAWN){
        int side = chessPiece->side;
        return(side == WHITE && loc <= 7) || (side == BLACK && loc >= 56);
    }

    return false;
}

char* getChessPieceSymbol(struct ChessPiece* chessPiece){
    int side = chessPiece->side;

    if(side == NONE){
        return "  ";
    }

    int type = chessPiece->type;

    if(side == WHITE){
        switch(type){
        case PAWN:
            return "WP";
        case KNIGHT:
            return "WN";
        case BISHOP:
            return "WB";
        case ROOK:
            return "WR";
        case QUEEN:
            return "WQ";
        case KING:
            return "WK";
        default:
            return "  ";
        }
    }else if(side == BLACK){
        switch(type){
        case PAWN:
            return "BP";
        case KNIGHT:
            return "BN";
        case BISHOP:
            return "BB";
        case ROOK:
            return "BR";
        case QUEEN:
            return "BQ";
        case KING:
            return "BK";
        default:
            return "  ";
        }
    }
}

void newChessMove(struct ChessMove *move, int source, int destination, int capturedLoc, int castlingType, int promotionType){
    move->source = source;
    move->destination = destination;
    move->capturedLoc = capturedLoc;
    move->castlingType = castlingType;
    move->promotionType = promotionType;
}

struct ChessMove copyChessMove(struct ChessMove *move){
    struct ChessMove chessMoveCopy;
    newChessMove(&chessMoveCopy, move->source, move->destination, move->capturedLoc, move->castlingType, move->promotionType);

    return chessMoveCopy;
}

struct ChessMove makeNullChessMove(){
    struct ChessMove nullMove;
    newChessMove(&nullMove, -1, -1, -1, NOCASTLE, -1);

    return nullMove;
}

void resetBoard(){
    int i;
    int backRow[8] = {ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK};

    for(i = 0; i < 8; i++){
        struct ChessPiece newWhitePiece;
        struct ChessPiece newBlackPiece;

        int pieceType = backRow[i];
        bool canCastle = (pieceType == KING || pieceType == ROOK);

        newChessPiece(&newWhitePiece, i + 56, WHITE, pieceType, canCastle);
        newChessPiece(&newBlackPiece, i, BLACK, pieceType, canCastle);

        *(board + i + 56) = newWhitePiece;
        *(board + i) = newBlackPiece;
    }

    for(i = 8; i < 16; i++){
        struct ChessPiece newWhitePawn;
        struct ChessPiece newBlackPawn;

        newChessPiece(&newWhitePawn, i + 40, WHITE, PAWN, false);
        newChessPiece(&newBlackPawn, i, BLACK, PAWN, false);

        *(board + i + 40) = newWhitePawn;
        *(board + i) = newBlackPawn;
    }

    for(i = 16; i < 48; i++){
        struct ChessPiece nullPiece;
        newChessPiece(&nullPiece, i, NONE, -1, false);

        *(board + i) = nullPiece;
    }
}

void drawBoard(struct ChessPiece* board){
    printf("|----|----|----|----|----|----|----|----|\n");
    int index;

    for(index = 0; index < 64; index++){
        if(index%8 == 0){
            printf("|");
        }

        printf(" %s ", getChessPieceSymbol(&(*(board + index))));
        printf("|");

        if(index%8 == 7){
            printf("\n");
            printf("|----|----|----|----|----|----|----|----|\n");
        }
    }
}

struct ChessPiece* copyBoard(struct ChessPiece* board){
    struct ChessPiece* boardCopy = malloc(64 * sizeof(struct ChessPiece));
    int i;

    for(i = 0; i < 64; i++){
        *(boardCopy + i) = *(board + i);
    }

    return boardCopy;
}

bool isSpaceFree(struct ChessPiece* board, int loc){
    return (*(board + loc)).side == NONE;
}

bool isSpaceEnemyOccupied(struct ChessPiece* board, int loc, int side){
    return (*(board + loc)).side == -side;
}

struct ChessPiece* getPiece(struct ChessPiece* board, int loc){
    return &(*(board + loc));
}

void apply(struct ChessPiece* board, struct ChessMove* chessMove){
    if(chessMove->castlingType != NOCASTLE){
        int side = ((chessMove->source) <= 7) ? BLACK : WHITE;
        castle(board, side, chessMove->castlingType);
    }else{
        int source = chessMove->source;
        struct ChessPiece* chessPiece = getPiece(board, source);

        int destination = chessMove->destination;
        movePiece(board, chessPiece, destination);

        struct ChessPiece* newChessPiece = getPiece(board, destination);

        if(canPromote(newChessPiece, destination)){
            promote(newChessPiece, chessMove->promotionType);
        }

        if((chessMove->capturedLoc) != -1){
            struct ChessPiece* capturedPiece = getPiece(board, chessMove->capturedLoc);
            captured(board, capturedPiece);
        }
    }
}

void castle(struct ChessPiece* board, int side, int castlingType){
    int kingLoc = (side == BLACK) ? 4 : 60;
    struct ChessPiece* king = getPiece(board, kingLoc);
    struct ChessPiece* rook;

    if(castlingType == RIGHTCASTLE){
        rook = getPiece(board, kingLoc + 3);
        movePiece(board, king, kingLoc + 2);
        movePiece(board, rook, kingLoc + 1);

        (*(board + kingLoc + 1)).canCastle = false;
        (*(board + kingLoc + 2)).canCastle = false;
        (*(board + kingLoc - 4)).canCastle = false;

    }else{
        rook = getPiece(board, kingLoc - 4);
        movePiece(board, king, kingLoc - 2);
        movePiece(board, rook, kingLoc - 1);

        (*(board + kingLoc - 1)).canCastle = false;
        (*(board + kingLoc - 2)).canCastle = false;
        (*(board + kingLoc + 3)).canCastle = false;
    }
}

bool isAttacked(struct ChessPiece* board, int loc, int side){
    switch(side){
    case WHITE:
        if(loc < 48){
            if(loc%8 < 7){
                if((*(board + loc + 9)).type == PAWN && (*(board + loc + 9)).side == WHITE){
                    return true;
                }
            }

            if(loc%8 > 0){
                if((*(board + loc + 7)).type == PAWN && (*(board + loc + 7)).side == WHITE){
                    return true;
                }
            }
        }

        break;
    case BLACK:
        if(loc > 15){
            if(loc%8 < 7){
                if((*(board + loc - 7)).type == PAWN && (*(board + loc - 7)).side == BLACK){
                    return true;
                }
            }

            if(loc%8 > 0){
                if((*(board + loc - 9)).type == PAWN && (*(board + loc - 9)).side == BLACK){
                    return true;
                }
            }
        }

        break;
    default:
        break;
    }

    if(loc%8 >= 2){
        if(loc >= 10){
            if((*(board + loc - 10)).type == KNIGHT && (*(board + loc - 10)).side == side){
                return true;
            }
        }

        if(loc <= 55){
            if((*(board + loc + 6)).type == KNIGHT && (*(board + loc + 6)).side == side){
                return true;
            }
        }
    }

    if(loc%8 >= 1){
        if(loc >= 17){
            if((*(board + loc - 17)).type == KNIGHT && (*(board + loc - 17)).side == side){
                return true;
            }
        }

        if(loc <= 47){
            if((*(board + loc + 15)).type == KNIGHT && (*(board + loc + 15)).side == side){
                return true;
            }
        }
    }

    if(loc%8 <= 6){
        if(loc >= 16){
            if((*(board + loc - 15)).type == KNIGHT && (*(board + loc - 15)).side == side){
                return true;
            }
        }

        if(loc <= 46){
            if((*(board + loc + 17)).type == KNIGHT && (*(board + loc + 17)).side == side){
                return true;
            }
        }
    }

    if(loc%8 <= 5){
        if(loc >= 8){
            if((*(board + loc - 6)).type == KNIGHT && (*(board + loc - 6)).side == side){
                return true;
            }
        }

        if(loc <= 53){
            if((*(board + loc + 10)).type == KNIGHT && (*(board + loc + 10)).side == side){
                return true;
            }
        }
    }

    int delta;

    bool leftBlocked = false;
    bool rightBlocked = false;
    bool topBlocked = false;
    bool bottomBlocked = false;

    for(delta = 1; delta <= 7; delta++){
        if((loc - delta)%8 < loc%8 && !leftBlocked){
            if(delta == 1){
                if((*(board + loc - delta)).type == KING && (*(board + loc - delta)).side == side){
                    return true;
                }
            }

            if((*(board + loc - delta)).side == side){
                int pieceType = (*(board + loc - delta)).type;

                if(pieceType == ROOK || pieceType == QUEEN){
                    return true;
                }
            }

            if(!isSpaceFree(board, loc - delta)){
                leftBlocked = true;
            }
        }

        if((loc + delta)%8 > loc%8 && !rightBlocked){
            if(delta == 1){
                if((*(board + loc + delta)).type == KING && (*(board + loc + delta)).side == side){
                    return true;
                }
            }

            if((*(board + loc + delta)).side == side){
                int pieceType = (*(board + loc + delta)).type;

                if(pieceType == ROOK || pieceType == QUEEN){
                    return true;
                }
            }

            if(!isSpaceFree(board, loc + delta)){
                rightBlocked = true;
            }
        }

        if(loc - 8 * delta >= 0 && !topBlocked){
            if(delta == 1){
                if((*(board + loc - 8 * delta)).type == KING && (*(board + loc - 8 * delta)).side == side){
                    return true;
                }
            }

            if((*(board + loc - 8 * delta)).side == side){
                int pieceType = (*(board + loc - 8 * delta)).type;

                if(pieceType == ROOK || pieceType == QUEEN){
                    return true;
                }
            }

            if(!isSpaceFree(board, loc - 8 * delta)){
                topBlocked = true;
            }
        }

        if(loc + 8 * delta <= 63 && !bottomBlocked){
            if(delta == 1){
                if((*(board + loc + 8 * delta)).type == KING && (*(board + loc + 8 * delta)).side == side){
                    return true;
                }
            }

            if((*(board + loc + 8 * delta)).side == side){
                int pieceType = (*(board + loc + 8 * delta)).type;

                if(pieceType == ROOK || pieceType == QUEEN){
                    return true;
                }
            }

            if(!isSpaceFree(board, loc + 8 * delta)){
                bottomBlocked = true;
            }
        }
    }

    bool bottomLeftBlocked = false;
    bool bottomRightBlocked = false;
    bool topLeftBlocked = false;
    bool topRightBlocked = false;

    for(delta = 1; delta <= 7; delta++){
        if(loc - 8 * delta >= 0 && (loc - delta)%8 < loc%8 && !topLeftBlocked){
            if(delta == 1){
                if((*(board + loc - 9 * delta)).type == KING && (*(board + loc - 9 * delta)).side == side){
                    return true;
                }
            }

            if((*(board + loc - 9 * delta)).side == side){
                int pieceType = (*(board + loc - 9 * delta)).type;

                if(pieceType == BISHOP || pieceType == QUEEN){
                    return true;
                }
            }

            if(!isSpaceFree(board, loc - 9 * delta)){
                topLeftBlocked = true;
            }
        }

        if(loc - 8 * delta >= 0 && (loc + delta)%8 > loc%8 && !topRightBlocked){
            if(delta == 1){
                if((*(board + loc - 7 * delta)).type == KING && (*(board + loc - 7 * delta)).side == side){
                    return true;
                }
            }

            if((*(board + loc - 7 * delta)).side == side){
                int pieceType = (*(board + loc - 7 * delta)).type;

                if(pieceType == BISHOP || pieceType == QUEEN){
                    return true;
                }
            }

            if(!isSpaceFree(board, loc - 7 * delta)){
                topRightBlocked = true;
            }
        }

        if(loc + 8 * delta <= 63 && (loc - delta)%8 < loc%8 && !bottomLeftBlocked){
            if(delta == 1){
                if((*(board + loc + 7 * delta)).type == KING && (*(board + loc + 7 * delta)).side == side){
                    return true;
                }
            }

            if((*(board + loc + 7 * delta)).side == side){
                int pieceType = (*(board + loc + 7 * delta)).type;

                if(pieceType == BISHOP || pieceType == QUEEN){
                    return true;
                }
            }

            if(!isSpaceFree(board, loc + 7 * delta)){
                bottomLeftBlocked = true;
            }
        }

        if(loc + 8 * delta <= 63 && (loc + delta)%8 > loc%8 && !bottomRightBlocked){
            if(delta == 1){
                if((*(board + loc + 9 * delta)).type == KING && (*(board + loc + 9 * delta)).side == side){
                    return true;
                }
            }

            if((*(board + loc + 9 * delta)).side == side){
                int pieceType = (*(board + loc + 9 * delta)).type;

                if(pieceType == BISHOP || pieceType == QUEEN){
                    return true;
                }
            }

            if(!isSpaceFree(board, loc + 9 * delta)){
                bottomRightBlocked = true;
            }
        }
    }

    return false;
}

bool canCastle(struct ChessPiece* board, int side, int castlingType){
    int kingLoc = getKingLocation(board, side);

    if((*(board + kingLoc)).canCastle == false || isAttacked(board, kingLoc, -side)){
        return false;
    }

    switch(castlingType){
    case RIGHTCASTLE:
        if(isSpaceFree(board, kingLoc + 1) && !isAttacked(board, kingLoc + 1, -side) &&
           isSpaceFree(board, kingLoc + 2) && !isAttacked(board, kingLoc + 2, -side)){
            return true;
        }
        break;
    case LEFTCASTLE:
        if(isSpaceFree(board, kingLoc - 1) && !isAttacked(board, kingLoc - 1, -side) &&
           isSpaceFree(board, kingLoc - 2) && !isAttacked(board, kingLoc - 2, -side) &&
           isSpaceFree(board, kingLoc - 3)){
            return true;
        }
        break;
    default:
        break;
    }

    return false;
}

struct ChessMove* getPossibleMoves(struct ChessPiece* board, struct ChessPiece* chessPiece){
    struct ChessMove* possibleMoves = (struct ChessMove*) malloc(28 * sizeof(struct ChessMove));
    int possibleMovesCount = 0;

    int pieceType = chessPiece->type;
    int pieceSide = chessPiece->side;
    int pieceLoc = chessPiece->loc;

    int kingLoc = getKingLocation(board, pieceSide);

    if(pieceType != KING){
        struct ChessPiece* boardCopy = copyBoard(board);
        captured(boardCopy, chessPiece);

        if(!isAttacked(board, kingLoc, -pieceSide) && isAttacked(boardCopy, kingLoc, -pieceSide)){
            *possibleMoves = nullMove;
            return possibleMoves;
        }
    }

    if(pieceType == PAWN){
        if(pieceLoc%8 > 0){
            int delta = (pieceSide == WHITE) ? -9 : 7;

            if(isSpaceEnemyOccupied(board, pieceLoc + delta, pieceSide)){
                struct ChessMove possibleMove;
                newChessMove(&possibleMove, pieceLoc, pieceLoc + delta, -1, NOCASTLE, -1);

                struct ChessPiece* boardCopy = copyBoard(board);
                apply(boardCopy, &possibleMove);

                if(!isAttacked(boardCopy, kingLoc, -pieceSide)){
                    if(canPromote(chessPiece, pieceLoc + delta)){
                        int i;

                        for(i = KNIGHT; i <= QUEEN; i++){
                            struct ChessMove possibleMoveCopy = copyChessMove(&possibleMove);
                            possibleMoveCopy.promotionType = i;

                            possibleMovesCount++;
                            *(possibleMoves + possibleMovesCount - 1) = possibleMoveCopy;
                        }
                    }else{
                        possibleMovesCount++;
                        *(possibleMoves + possibleMovesCount - 1) = possibleMove;
                    }
                }
            }
        }

        if(pieceLoc%8 < 7){
            int delta = (pieceSide == WHITE) ? -7 : 9;

            if(isSpaceEnemyOccupied(board, pieceLoc + delta, pieceSide)){
                struct ChessMove possibleMove;
                newChessMove(&possibleMove, pieceLoc, pieceLoc + delta, -1, NOCASTLE, -1);

                struct ChessPiece* boardCopy = copyBoard(board);
                apply(boardCopy, &possibleMove);

                if(!isAttacked(boardCopy, kingLoc, -pieceSide)){
                    if(canPromote(chessPiece, pieceLoc + delta)){
                        int i;

                        for(i = KNIGHT; i <= QUEEN; i++){
                            struct ChessMove possibleMoveCopy = copyChessMove(&possibleMove);
                            possibleMoveCopy.promotionType = i;

                            possibleMovesCount++;
                            *(possibleMoves + possibleMovesCount - 1) = possibleMoveCopy;
                        }
                    }else{
                        possibleMovesCount++;
                        *(possibleMoves + possibleMovesCount - 1) = possibleMove;
                    }
                }
            }
        }

        bool forwardBlocked = false;

        if(isSpaceFree(board, pieceLoc - 8 * pieceSide)){
            struct ChessMove possibleMove;
            newChessMove(&possibleMove, pieceLoc, pieceLoc - 8 * pieceSide, -1, NOCASTLE, -1);

            struct ChessPiece* boardCopy = copyBoard(board);
            apply(boardCopy, &possibleMove);

            if(!isAttacked(boardCopy, kingLoc, -pieceSide)){
                if(canPromote(chessPiece, pieceLoc - 8 * pieceSide)){
                    int i;

                    for(i = KNIGHT; i <= QUEEN; i++){
                        struct ChessMove possibleMoveCopy = copyChessMove(&possibleMove);
                        possibleMoveCopy.promotionType = i;

                        possibleMovesCount++;
                        *(possibleMoves + possibleMovesCount - 1) = possibleMoveCopy;
                    }
                }else{
                    possibleMovesCount++;
                    *(possibleMoves + possibleMovesCount - 1) = possibleMove;
                }
            }
        }else{
            forwardBlocked = true;
        }

        if(!forwardBlocked){
            if((pieceSide == WHITE && pieceLoc >= 48 && pieceLoc <= 55) ||
               (pieceSide == BLACK && pieceLoc >= 8 && pieceLoc <= 15)){
                if(isSpaceFree(board, pieceLoc - 16 * pieceSide)){
                    struct ChessMove possibleMove;
                    newChessMove(&possibleMove, pieceLoc, pieceLoc - 16 * pieceSide, -1, NOCASTLE, -1);

                    struct ChessPiece* boardCopy = copyBoard(board);
                    apply(boardCopy, &possibleMove);

                    if(!isAttacked(boardCopy, kingLoc, -pieceSide)){
                        possibleMovesCount++;
                        *(possibleMoves + possibleMovesCount - 1) = possibleMove;
                    }
                }
            }
        }
    }

    if(pieceType == KNIGHT){
        if(pieceLoc%8 >= 2){
            if(pieceLoc >= 10){
                if(isSpaceFree(board, pieceLoc - 10) || isSpaceEnemyOccupied(board, pieceLoc - 10, pieceSide)){
                    struct ChessMove possibleMove;
                    newChessMove(&possibleMove, pieceLoc, pieceLoc - 10, -1, NOCASTLE, -1);

                    struct ChessPiece* boardCopy = copyBoard(board);
                    apply(boardCopy, &possibleMove);

                    if(!isAttacked(boardCopy, kingLoc, -pieceSide)){
                        possibleMovesCount++;
                        *(possibleMoves + possibleMovesCount - 1) = possibleMove;
                    }
                }
            }

            if(pieceLoc <= 55){
                if(isSpaceFree(board, pieceLoc + 6) || isSpaceEnemyOccupied(board, pieceLoc + 6, pieceSide)){
                    struct ChessMove possibleMove;
                    newChessMove(&possibleMove, pieceLoc, pieceLoc + 6, -1, NOCASTLE, -1);

                    struct ChessPiece* boardCopy = copyBoard(board);
                    apply(boardCopy, &possibleMove);

                    if(!isAttacked(boardCopy, kingLoc, -pieceSide)){
                        possibleMovesCount++;
                        *(possibleMoves + possibleMovesCount - 1) = possibleMove;
                    }
                }
            }
        }

        if(pieceLoc%8 >= 1){
            if(pieceLoc >= 17){
                if(isSpaceFree(board, pieceLoc - 17) || isSpaceEnemyOccupied(board, pieceLoc - 17, pieceSide)){
                    struct ChessMove possibleMove;
                    newChessMove(&possibleMove, pieceLoc, pieceLoc - 17, -1, NOCASTLE, -1);

                    struct ChessPiece* boardCopy = copyBoard(board);
                    apply(boardCopy, &possibleMove);

                    if(!isAttacked(boardCopy, kingLoc, -pieceSide)){
                        possibleMovesCount++;
                        *(possibleMoves + possibleMovesCount - 1) = possibleMove;
                    }
                }
            }

            if(pieceLoc <= 47){
                if(isSpaceFree(board, pieceLoc + 15) || isSpaceEnemyOccupied(board, pieceLoc + 15, pieceSide)){
                    struct ChessMove possibleMove;
                    newChessMove(&possibleMove, pieceLoc, pieceLoc + 15, -1, NOCASTLE, -1);

                    struct ChessPiece* boardCopy = copyBoard(board);
                    apply(boardCopy, &possibleMove);

                    if(!isAttacked(boardCopy, kingLoc, -pieceSide)){
                        possibleMovesCount++;
                        *(possibleMoves + possibleMovesCount - 1) = possibleMove;
                    }
                }
            }
        }

        if(pieceLoc%8 <= 6){
            if(pieceLoc >= 16){
                if(isSpaceFree(board, pieceLoc - 15) || isSpaceEnemyOccupied(board, pieceLoc - 15, pieceSide)){
                    struct ChessMove possibleMove;
                    newChessMove(&possibleMove, pieceLoc, pieceLoc - 15, -1, NOCASTLE, -1);

                    struct ChessPiece* boardCopy = copyBoard(board);
                    apply(boardCopy, &possibleMove);

                    if(!isAttacked(boardCopy, kingLoc, -pieceSide)){
                        possibleMovesCount++;
                        *(possibleMoves + possibleMovesCount - 1) = possibleMove;
                    }
                }
            }

            if(pieceLoc <= 46){
                if(isSpaceFree(board, pieceLoc + 17) || isSpaceEnemyOccupied(board, pieceLoc + 17, pieceSide)){
                    struct ChessMove possibleMove;
                    newChessMove(&possibleMove, pieceLoc, pieceLoc + 17, -1, NOCASTLE, -1);

                    struct ChessPiece* boardCopy = copyBoard(board);
                    apply(boardCopy, &possibleMove);

                    if(!isAttacked(boardCopy, kingLoc, -pieceSide)){
                        possibleMovesCount++;
                        *(possibleMoves + possibleMovesCount - 1) = possibleMove;
                    }
                }
            }
        }

        if(pieceLoc%8 <= 5){
            if(pieceLoc >= 8){
                if(isSpaceFree(board, pieceLoc - 6) || isSpaceEnemyOccupied(board, pieceLoc - 6, pieceSide)){
                    struct ChessMove possibleMove;
                    newChessMove(&possibleMove, pieceLoc, pieceLoc - 6, -1, NOCASTLE, -1);

                    struct ChessPiece* boardCopy = copyBoard(board);
                    apply(boardCopy, &possibleMove);

                    if(!isAttacked(boardCopy, kingLoc, -pieceSide)){
                        possibleMovesCount++;
                        *(possibleMoves + possibleMovesCount - 1) = possibleMove;
                    }
                }
            }

            if(pieceLoc <= 53){
                if(isSpaceFree(board, pieceLoc + 10) || isSpaceEnemyOccupied(board, pieceLoc + 10, pieceSide)){
                    struct ChessMove possibleMove;
                    newChessMove(&possibleMove, pieceLoc, pieceLoc + 10, -1, NOCASTLE, -1);

                    struct ChessPiece* boardCopy = copyBoard(board);
                    apply(boardCopy, &possibleMove);

                    if(!isAttacked(boardCopy, kingLoc, -pieceSide)){
                        possibleMovesCount++;
                        *(possibleMoves + possibleMovesCount - 1) = possibleMove;
                    }
                }
            }
        }
    }

    if(pieceType == BISHOP || pieceType == QUEEN){
        int delta;

        bool bottomLeftBlocked = false;
        bool bottomRightBlocked = false;
        bool topLeftBlocked = false;
        bool topRightBlocked = false;

        for(delta = 1; delta <= 7; delta++){
            if(pieceLoc - 8 * delta >= 0 && pieceLoc >= delta && (pieceLoc - delta)%8 < pieceLoc%8 && !topLeftBlocked){
                if(isSpaceFree(board, pieceLoc - 9 * delta) || isSpaceEnemyOccupied(board, pieceLoc - 9 * delta, pieceSide)){
                    struct ChessMove possibleMove;
                    newChessMove(&possibleMove, pieceLoc, pieceLoc - 9 * delta, -1, NOCASTLE, -1);

                    struct ChessPiece* boardCopy = copyBoard(board);
                    apply(boardCopy, &possibleMove);

                    if(!isAttacked(boardCopy, kingLoc, -pieceSide)){
                        possibleMovesCount++;
                        *(possibleMoves + possibleMovesCount - 1) = possibleMove;
                    }
                }

                if(!isSpaceFree(board, pieceLoc - 9 * delta)){
                    topLeftBlocked = true;
                }
            }

            if(pieceLoc - 8 * delta >= 0 && (pieceLoc + delta)%8 > pieceLoc%8 && !topRightBlocked){
                if(isSpaceFree(board, pieceLoc - 7 * delta) || isSpaceEnemyOccupied(board, pieceLoc - 7 * delta, pieceSide)){
                    struct ChessMove possibleMove;
                    newChessMove(&possibleMove, pieceLoc, pieceLoc - 7 * delta, -1, NOCASTLE, -1);

                    struct ChessPiece* boardCopy = copyBoard(board);
                    apply(boardCopy, &possibleMove);

                    if(!isAttacked(boardCopy, kingLoc, -pieceSide)){
                        possibleMovesCount++;
                        *(possibleMoves + possibleMovesCount - 1) = possibleMove;
                    }
                }

                if(!isSpaceFree(board, pieceLoc - 7 * delta)){
                    topRightBlocked = true;
                }
            }

            if(pieceLoc + 8 * delta <= 63 && pieceLoc >= delta && (pieceLoc - delta)%8 < pieceLoc%8 && !bottomLeftBlocked){
                if(isSpaceFree(board, pieceLoc + 7 * delta) || isSpaceEnemyOccupied(board, pieceLoc + 7 * delta, pieceSide)){
                    struct ChessMove possibleMove;
                    newChessMove(&possibleMove, pieceLoc, pieceLoc + 7 * delta, -1, NOCASTLE, -1);

                    struct ChessPiece* boardCopy = copyBoard(board);
                    apply(boardCopy, &possibleMove);

                    if(!isAttacked(boardCopy, kingLoc, -pieceSide)){
                        possibleMovesCount++;
                        *(possibleMoves + possibleMovesCount - 1) = possibleMove;
                    }
                }

                if(!isSpaceFree(board, pieceLoc + 7 * delta)){
                    bottomLeftBlocked = true;
                }
            }

            if(pieceLoc + 8 * delta <= 63 && (pieceLoc + delta)%8 > pieceLoc%8 && !bottomRightBlocked){
                if(isSpaceFree(board, pieceLoc + 9 * delta) || isSpaceEnemyOccupied(board, pieceLoc + 9 * delta, pieceSide)){
                    struct ChessMove possibleMove;
                    newChessMove(&possibleMove, pieceLoc, pieceLoc + 9 * delta, -1, NOCASTLE, -1);

                    struct ChessPiece* boardCopy = copyBoard(board);
                    apply(boardCopy, &possibleMove);

                    if(!isAttacked(boardCopy, kingLoc, -pieceSide)){
                        possibleMovesCount++;
                        *(possibleMoves + possibleMovesCount - 1) = possibleMove;
                    }
                }

                if(!isSpaceFree(board, pieceLoc + 9 * delta)){
                    bottomRightBlocked = true;
                }
            }
        }
    }

    if(pieceType == ROOK || pieceType == QUEEN){
        int delta;

        bool leftBlocked = false;
        bool rightBlocked = false;
        bool topBlocked = false;
        bool bottomBlocked = false;

        for(delta = 1; delta <= 7; delta++){
            if(pieceLoc >= delta && (pieceLoc - delta)%8 < pieceLoc%8 && !leftBlocked){
                if(isSpaceFree(board, pieceLoc - delta) || isSpaceEnemyOccupied(board, pieceLoc - delta, pieceSide)){
                    struct ChessMove possibleMove;
                    newChessMove(&possibleMove, pieceLoc, pieceLoc - delta, -1, NOCASTLE, -1);

                    struct ChessPiece* boardCopy = copyBoard(board);
                    apply(boardCopy, &possibleMove);

                    if(!isAttacked(boardCopy, kingLoc, -pieceSide)){
                        possibleMovesCount++;
                        *(possibleMoves + possibleMovesCount - 1) = possibleMove;
                    }
                }

                if(!isSpaceFree(board, pieceLoc - delta)){
                    leftBlocked = true;
                }
            }

            if((pieceLoc + delta)%8 > pieceLoc%8 && !rightBlocked){
                if(isSpaceFree(board, pieceLoc + delta) || isSpaceEnemyOccupied(board, pieceLoc + delta, pieceSide)){
                    struct ChessMove possibleMove;
                    newChessMove(&possibleMove, pieceLoc, pieceLoc + delta, -1, NOCASTLE, -1);

                    struct ChessPiece* boardCopy = copyBoard(board);
                    apply(boardCopy, &possibleMove);

                    if(!isAttacked(boardCopy, kingLoc, -pieceSide)){
                        possibleMovesCount++;
                        *(possibleMoves + possibleMovesCount - 1) = possibleMove;
                    }
                }

                if(!isSpaceFree(board, pieceLoc + delta)){
                    rightBlocked = true;
                }
            }

            if(pieceLoc - 8 * delta >= 0 && !topBlocked){
                if(isSpaceFree(board, pieceLoc - 8 * delta) || isSpaceEnemyOccupied(board, pieceLoc - 8 * delta, pieceSide)){
                    struct ChessMove possibleMove;
                    newChessMove(&possibleMove, pieceLoc, pieceLoc - 8 * delta, -1, NOCASTLE, -1);

                    struct ChessPiece* boardCopy = copyBoard(board);
                    apply(boardCopy, &possibleMove);

                    if(!isAttacked(boardCopy, kingLoc, -pieceSide)){
                        possibleMovesCount++;
                        *(possibleMoves + possibleMovesCount - 1) = possibleMove;
                    }
                }

                if(!isSpaceFree(board, pieceLoc - 8 * delta)){
                    topBlocked = true;
                }
            }

            if(pieceLoc + 8 * delta <= 63 && !bottomBlocked){
                if(isSpaceFree(board, pieceLoc + 8 * delta) || isSpaceEnemyOccupied(board, pieceLoc + 8 * delta, pieceSide)){
                    struct ChessMove possibleMove;
                    newChessMove(&possibleMove, pieceLoc, pieceLoc + 8 * delta, -1, NOCASTLE, -1);

                    struct ChessPiece* boardCopy = copyBoard(board);
                    apply(boardCopy, &possibleMove);

                    if(!isAttacked(boardCopy, kingLoc, -pieceSide)){
                        possibleMovesCount++;
                        *(possibleMoves + possibleMovesCount - 1) = possibleMove;
                    }
                }

                if(!isSpaceFree(board, pieceLoc + 8 * delta)){
                    bottomBlocked = true;
                }
            }
        }
    }

    if(pieceType == KING){
        if(pieceLoc <= 55){
            if(!isAttacked(board, pieceLoc + 8, -pieceSide) && ((isSpaceFree(board, pieceLoc + 8)) || isSpaceEnemyOccupied(board, pieceLoc + 8, pieceSide))){
                possibleMovesCount++;

                struct ChessMove possibleMove;
                newChessMove(&possibleMove, pieceLoc, pieceLoc + 8, -1, NOCASTLE, -1);
                *(possibleMoves + possibleMovesCount - 1) = possibleMove;
            }

            if(pieceLoc%8 <= 6){
                if(!isAttacked(board, pieceLoc + 9, -pieceSide) && ((isSpaceFree(board, pieceLoc + 9)) || isSpaceEnemyOccupied(board, pieceLoc + 9, pieceSide))){
                    possibleMovesCount++;

                    struct ChessMove possibleMove;
                    newChessMove(&possibleMove, pieceLoc, pieceLoc + 9, -1, NOCASTLE, -1);
                    *(possibleMoves + possibleMovesCount - 1) = possibleMove;
                }
            }

            if(pieceLoc%8 >= 1){
                if(!isAttacked(board, pieceLoc + 7, -pieceSide) && ((isSpaceFree(board, pieceLoc + 7)) || isSpaceEnemyOccupied(board, pieceLoc + 7, pieceSide))){
                    possibleMovesCount++;

                    struct ChessMove possibleMove;
                    newChessMove(&possibleMove, pieceLoc, pieceLoc + 7, -1, NOCASTLE, -1);
                    *(possibleMoves + possibleMovesCount - 1) = possibleMove;
                }
            }
        }

        if(pieceLoc >= 8){
            if(!isAttacked(board, pieceLoc - 8, -pieceSide) && ((isSpaceFree(board, pieceLoc - 8)) || isSpaceEnemyOccupied(board, pieceLoc - 8, pieceSide))){
                possibleMovesCount++;

                struct ChessMove possibleMove;
                newChessMove(&possibleMove, pieceLoc, pieceLoc - 8, -1, NOCASTLE, -1);
                *(possibleMoves + possibleMovesCount - 1) = possibleMove;
            }

            if(pieceLoc%8 <= 6){
                if(!isAttacked(board, pieceLoc - 7, -pieceSide) && ((isSpaceFree(board, pieceLoc - 7)) || isSpaceEnemyOccupied(board, pieceLoc - 7, pieceSide))){
                    possibleMovesCount++;

                    struct ChessMove possibleMove;
                    newChessMove(&possibleMove, pieceLoc, pieceLoc - 7, -1, NOCASTLE, -1);
                    *(possibleMoves + possibleMovesCount - 1) = possibleMove;
                }
            }

            if(pieceLoc%8 >= 1){
                if(!isAttacked(board, pieceLoc - 9, -pieceSide) && ((isSpaceFree(board, pieceLoc - 9)) || isSpaceEnemyOccupied(board, pieceLoc - 9, pieceSide))){
                    possibleMovesCount++;

                    struct ChessMove possibleMove;
                    newChessMove(&possibleMove, pieceLoc, pieceLoc - 9, -1, NOCASTLE, -1);
                    *(possibleMoves + possibleMovesCount - 1) = possibleMove;
                }
            }
        }

        if(pieceLoc%8 <= 6){
            if(!isAttacked(board, pieceLoc + 1, -pieceSide) && ((isSpaceFree(board, pieceLoc + 1)) || isSpaceEnemyOccupied(board, pieceLoc + 1, pieceSide))){
                possibleMovesCount++;

                struct ChessMove possibleMove;
                newChessMove(&possibleMove, pieceLoc, pieceLoc + 1, -1, NOCASTLE, -1);
                *(possibleMoves + possibleMovesCount - 1) = possibleMove;
            }
        }

        if(pieceLoc%8 >= 1){
            if(!isAttacked(board, pieceLoc - 1, -pieceSide) && ((isSpaceFree(board, pieceLoc - 1)) || isSpaceEnemyOccupied(board, pieceLoc - 1, pieceSide))){
                possibleMovesCount++;

                struct ChessMove possibleMove;
                newChessMove(&possibleMove, pieceLoc, pieceLoc - 1, -1, NOCASTLE, -1);
                *(possibleMoves + possibleMovesCount - 1) = possibleMove;
            }
        }
    }

    *(possibleMoves + possibleMovesCount) = nullMove;
    return possibleMoves;
}


int getKingLocation(struct ChessPiece* board, int side){
    int i;

    for(i = 0; i < 64; i++){
        if((*(board + i)).type == KING && (*(board + i)).side == side){
            return i;
        }
    }
}

// TODO: Implement en passant
struct ChessMove* getAllPossibleMoves(struct ChessPiece* board, int side){
    struct ChessMove* allPossibleMoves = (struct ChessMove*) malloc(16 * 28 * sizeof(struct ChessMove));
    int allPossibleMovesCount = 0;

    int i, j;

    for(i = 0; i < 64; i++){
        struct ChessPiece* chessPiece = getPiece(board, i);

        if((chessPiece->side) == side){
            struct ChessMove* possibleMoves = getPossibleMoves(board, chessPiece);
            j = 0;

            while((*(possibleMoves + j)).source != -1){
                allPossibleMovesCount++;
                *(allPossibleMoves + allPossibleMovesCount - 1) = *(possibleMoves + j);
                j++;
            }

            free(possibleMoves);
        }
    }

    int kingLoc = getKingLocation(board, side);

    if(canCastle(board, side, RIGHTCASTLE)){
        struct ChessMove rightCastle;
        newChessMove(&rightCastle, kingLoc, kingLoc + 2, -1, RIGHTCASTLE, -1);

        allPossibleMovesCount++;
        *(allPossibleMoves + allPossibleMovesCount - 1) = rightCastle;
    }

    if(canCastle(board, side, LEFTCASTLE)){
        struct ChessMove leftCastle;
        newChessMove(&leftCastle, kingLoc, kingLoc - 2, -1, RIGHTCASTLE, -1);

        allPossibleMovesCount++;
        *(allPossibleMoves + allPossibleMovesCount - 1) = leftCastle;
    }

    *(allPossibleMoves + allPossibleMovesCount) = nullMove;
    return allPossibleMoves;
}

bool isStalemate(struct ChessPiece* board, int side){
    int kingLoc = getKingLocation(board, side);
    struct ChessMove* allPossibleMoves = getAllPossibleMoves(board, side);

    return !isAttacked(board, kingLoc, -side) && ((*(allPossibleMoves)).source == -1);
}

bool isCheckmate(struct ChessPiece* board, int side){
    int kingLoc = getKingLocation(board, side);
    struct ChessMove* allPossibleMoves = getAllPossibleMoves(board, side);

    return isAttacked(board, kingLoc, -side) && ((*(allPossibleMoves)).source == -1);
}

// TODO: Play around with the weights
double evalBoard(struct ChessPiece* board, int side){
    if(isStalemate(board, side) || isStalemate(board, -side)){
        return 0.0;
    }

    if(isCheckmate(board, side)){
        return INT_MIN;
    }

    if(isCheckmate(board, -side)){
        return INT_MAX;
    }

    double boardEval = 0.0;

    int pieceValues[5] = {1, 3, 3, 5, 9};
    int i;

    for(i = 0; i < 64; i++){
        int pieceSide = (*(board + i)).side;

        if(pieceSide != NONE){
            int pieceLoc = (*(board + i)).loc;
            int pieceType = (*(board + i)).type;
            double pieceContribution = 0.0;

            if(pieceType != KING){
                pieceContribution += pieceSide * side * pieceValues[pieceType];
            }

            if(pieceType == PAWN || pieceType == KNIGHT){
                if(pieceLoc == 27 || pieceLoc == 28 || pieceLoc == 35 || pieceLoc == 36){
                    pieceContribution += pieceSide * side * pieceValues[pieceType];
                }else if(pieceLoc == 18 || pieceLoc == 21 || pieceLoc == 42 || pieceLoc == 45){
                    pieceContribution += pieceSide * side * pieceValues[pieceType];
                }
            }

            if(pieceType == BISHOP || pieceType == ROOK || pieceType == QUEEN){
                struct ChessPiece* chessPiece = getPiece(board, i);
                struct ChessMove* possibleMoves = getPossibleMoves(board, chessPiece);
                int mobilityScore = 0;

                while((*(possibleMoves + mobilityScore)).source != -1){
                    mobilityScore++;
                }

                pieceContribution += 0.1 * pieceSide * side * pieceValues[pieceType] * mobilityScore;
            }

            if(pieceType == KING){
                if((*(board + i)).canCastle == false){
                    if(pieceType == WHITE){
                        if(pieceLoc >= 56 && pieceLoc <= 58){
                            if((*(board + 56)).type != ROOK && (*(board + 57)).type != ROOK){
                                pieceContribution += 5.0 * pieceSide * side;
                            }
                        }else if(pieceLoc >= 61 && pieceLoc <= 62){
                            if((*(board + 63)).type != ROOK){
                                pieceContribution += 5.0 * pieceSide * side;
                            }
                        }
                    }else{
                        if(pieceLoc >= 0 && pieceLoc <= 2){
                            if((*(board + 0)).type != ROOK && (*(board + 1)).type != ROOK){
                                pieceContribution += 5.0 * pieceSide * side;
                            }
                        }else if(pieceLoc >= 5 && pieceLoc <= 6){
                            if((*(board + 7)).type != ROOK){
                                pieceContribution += 5.0 * pieceSide * side;
                            }
                        }
                    }
                }
            }

            boardEval += pieceContribution;
        }
    }

    return boardEval;
}

struct ChessMove findBestMove(struct ChessPiece* board, int alpha, int beta, int depth, int side){
    if(isCheckmate(board, side) || isCheckmate(board, -side) || isStalemate(board, side)){
        return nullMove;
    }

    struct ChessMove* allPossibleMoves = getAllPossibleMoves(board, side);

//    printf("\n\nConsidering the following position for side %d:\n", side);
//    drawBoard(board);
//
//    char ENTER;
//    printf("Press enter to continue:");
//    scanf("%c", &ENTER);

    struct ChessMove bestMove = nullMove;
    double bestEval = INT_MIN;
    int i = 0;

    while((*(allPossibleMoves + i)).source != -1){
        struct ChessMove possibleMove = *(allPossibleMoves + i);
        struct ChessPiece* boardCopy = copyBoard(board);
        apply(boardCopy, &possibleMove);

        if(isCheckmate(boardCopy, -side)){
            return possibleMove;
        }

        if(evalBoard(board, side) < 0 && isStalemate(boardCopy, -side)){
            return possibleMove;
        }

        i++;
    }

    i = 0;

    while((*(allPossibleMoves + i)).source != -1){
        struct ChessMove possibleMove = *(allPossibleMoves + i);
        struct ChessPiece* boardCopy = copyBoard(board);
        apply(boardCopy, &possibleMove);

        double possibleEval;

        if(depth == 1){
            possibleEval = evalBoard(boardCopy, -side);
        }else{
            struct ChessMove bestResponse = findBestMove(boardCopy, -beta, -alpha, depth - 1, -side);
            apply(boardCopy, &bestResponse);
            possibleEval = -evalBoard(boardCopy, side);
        }

        if(-possibleEval > bestEval){
            bestMove = possibleMove;
            bestEval = -possibleEval;

            if(depth > 1 && bestEval > alpha){
                alpha = bestEval;

                if(alpha >= beta){
                    break;
                }
            }
        }

        i++;
    }

    return bestMove;
}
