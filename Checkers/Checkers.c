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

// Checker Class

struct Checker{
    int loc;
    int side;
    bool is_king;
};

void newChecker(struct Checker *checker, int loc, int side, bool is_king);
struct Checker copyChecker(struct Checker *checker);
void promote(struct Checker *checker);
void movePiece(struct Checker* board, struct Checker *checker, int newLoc);
void captured(struct Checker* board, struct Checker *checker);
bool canPromote(struct Checker *checker, int loc);
bool canCapture(struct Checker* board, struct Checker *checker);
char getCheckerSymbol(struct Checker *checker);
struct CheckerMove* getPossibleMoves(struct Checker* board, struct Checker *checker);

// CheckerMove Class

struct CheckerMove{
    int type;
    int source;
    int destination;
    int capturedLoc;
    bool promote;
    struct CheckerMove *next;
};

static struct CheckerMove nullMove;

void newCheckerMove(struct CheckerMove *move, int type, int source, int destination, int capturedLoc, bool promote, struct CheckerMove *next);
void appendNewCapture(struct CheckerMove *move, struct CheckerMove *capture);
struct CheckerMove copyCheckerMove(struct CheckerMove *move);
struct CheckerMove makeNullCheckerMove();

// Checkerboard Methods

struct Checker board[64];

void resetBoard();
void drawBoard(struct Checker* board);
struct Checker* copyBoard(struct Checker* board);
bool isSpaceFree(struct Checker* board, int loc);
struct Checker* getPiece(struct Checker* board, int loc);
void apply(struct Checker* board, struct CheckerMove* checkerMove);

// Game and AI Methods
struct CheckerMove* getAllPossibleMoves(struct Checker* board, int side);
bool hasNoPieces(struct Checker* board, int side);
bool hasNoMoves(struct Checker* board, int side);
bool isWinner(struct Checker* board, int side);

double evalBoard(struct Checker* board, int side);
struct CheckerMove findBestMove(struct Checker* board, int side);
struct CheckerMove alphaBeta(struct Checker* board, int alpha, int beta, int depth, int side);

void main(){
    resetBoard();
    nullMove = makeNullCheckerMove();

    int turn = 1;
    struct CheckerMove nextMove;

    while(!isWinner(board, turn)){
        nextMove = findBestMove(board, turn);
        apply(board, &nextMove);

        printf("Side %d's Move: %d, %d, %d, %d, %d\n\n", turn, nextMove.type, nextMove.source, nextMove.destination, nextMove.capturedLoc, (nextMove.next)->type);
        drawBoard(board);

        char enter;
        printf("\nPress Enter to Continue:");
        scanf("%c", &enter);

        turn = -turn;
    }

    if(isWinner(board, turn)){
        printf("\nWinner: %d\n", turn);
    }
}

void newChecker(struct Checker *checker, int loc, int side, bool is_king){
    checker->loc = loc;
    checker->side = side;
    checker->is_king = is_king;
}

struct Checker copyChecker(struct Checker *checker){
    struct Checker copyChecker;
    newChecker(&copyChecker, checker->loc, checker->side, checker->is_king);

    return copyChecker;
};

void promote(struct Checker *checker){
    checker->is_king = true;
}

void movePiece(struct Checker* board, struct Checker* checker, int newLoc){
    int origLoc = checker->loc;

    checker->loc = newLoc;
    struct Checker checkerCopy = copyChecker(checker);

    (*(board + origLoc)).side = 0;
    *(board + newLoc) = checkerCopy;
}

void captured(struct Checker* board, struct Checker *checker){
    (*(board + (checker->loc))).side = 0;
}

bool canPromote(struct Checker *checker, int loc){
    if(!checker->is_king){
        return(checker->side == 1 && loc <= 7) || (checker->side == -1 && loc >= 56);
    }

    return false;
}

bool canCapture(struct Checker* board, struct Checker *checker){
    int loc = checker->loc;
    int side = checker->side;
    bool is_king = checker->is_king;

    if(is_king || side == 1){
        if(loc%8 > 1){
            if(loc >= 18){
                if((*(board + loc - 9)).side == -side && isSpaceFree(board, loc - 18)){
                    return true;
                }
            }
        }

        if(loc%8 < 6){
            if(loc >= 16){
                if((*(board + loc - 7)).side == -side && isSpaceFree(board, loc - 14)){
                    return true;
                }
            }
        }
    }

    if(is_king || side == -1){
        if(loc%8 > 1){
            if(loc <= 47){
                if((*(board + loc + 7)).side == -side && isSpaceFree(board, loc + 14)){
                    return true;
                }
            }
        }

        if(loc%8 < 6){
            if(loc <= 45){
                if((*(board + loc + 9)).side == -side && isSpaceFree(board, loc + 18)){
                    return true;
                }
            }
        }
    }

    return false;
}

char getCheckerSymbol(struct Checker *checker){
    if(checker->side == 0){
        return ' ';
    }else if(checker->side == 1){
        if(checker->is_king){
            return 'W';
        }else{
            return 'w';
        }
    }else{
        if(checker->is_king){
            return 'B';
        }else{
            return 'b';
        }
    }
}

struct CheckerMove* getPossibleMoves(struct Checker* board, struct Checker *checker){
    int loc = checker->loc;
    int side = checker->side;
    bool is_king = checker->is_king;

    if(canCapture(board, checker)){
        struct CheckerMove* possibleMoves = (struct CheckerMove*) malloc(11 * sizeof(struct CheckerMove));
        int possibleMovesCount = 0;

        if(is_king || side == 1){
            if(loc%8 > 1){
                if(loc >= 18){
                    if((*(board + loc - 9)).side == -side && isSpaceFree(board, loc - 18)){
                        bool promote = canPromote(checker, loc - 18);
                        bool chained = false;

                        struct CheckerMove possibleMove;
                        newCheckerMove(&possibleMove, 1, loc, loc - 18, loc - 9, promote, &nullMove);

                        if(!promote){
                            struct CheckerMove possibleMoveCopy = copyCheckerMove(&possibleMove);
                            struct Checker* boardCopy = copyBoard(board);

                            apply(boardCopy, &possibleMoveCopy);
                            struct Checker* capturingPiece = getPiece(boardCopy, loc - 18);
                            struct CheckerMove* nextPossibleMoves = getPossibleMoves(boardCopy, capturingPiece);

                            if((*(nextPossibleMoves)).type == 1){
                                chained = true;
                                int i = 0;

                                while((*(nextPossibleMoves + i)).type == 1){
                                    possibleMovesCount++;

                                    struct CheckerMove possibleMoveCopy = copyCheckerMove(&possibleMove);
                                    possibleMoveCopy.next = &(*(nextPossibleMoves + i));

                                    *(possibleMoves + possibleMovesCount - 1) = possibleMoveCopy;
                                    i++;
                                }
                            }
                        }

                        if(!chained){
                            possibleMovesCount++;
                            *(possibleMoves + possibleMovesCount - 1) = possibleMove;
                        }
                    }
                }
            }

            if(loc%8 < 6){
                if(loc >= 16){
                    if((*(board + loc - 7)).side == -side && isSpaceFree(board, loc - 14)){
                        bool promote = canPromote(checker, loc - 14);
                        bool chained = false;

                        struct CheckerMove possibleMove;
                        newCheckerMove(&possibleMove, 1, loc, loc - 14, loc - 7, promote, &nullMove);

                        if(!promote){
                            struct CheckerMove possibleMoveCopy = copyCheckerMove(&possibleMove);
                            struct Checker* boardCopy = copyBoard(board);

                            apply(boardCopy, &possibleMoveCopy);
                            struct Checker* capturingPiece = getPiece(boardCopy, loc - 14);
                            struct CheckerMove* nextPossibleMoves = getPossibleMoves(boardCopy, capturingPiece);

                            if((*(nextPossibleMoves)).type == 1){
                                chained = true;
                                int i = 0;

                                while((*(nextPossibleMoves + i)).type == 1){
                                    possibleMovesCount++;

                                    struct CheckerMove possibleMoveCopy = copyCheckerMove(&possibleMove);
                                    possibleMoveCopy.next = &(*(nextPossibleMoves + i));

                                    *(possibleMoves + possibleMovesCount - 1) = possibleMoveCopy;
                                    i++;
                                }
                            }
                        }

                        if(!chained){
                            possibleMovesCount++;
                            *(possibleMoves + possibleMovesCount - 1) = possibleMove;
                        }
                    }
                }
            }
        }

        if(is_king || side == -1){
            if(loc%8 > 1){
                if(loc <= 47){
                    if((*(board + loc + 7)).side == -side && isSpaceFree(board, loc + 14)){
                        bool promote = canPromote(checker, loc + 14);
                        bool chained = false;

                        struct CheckerMove possibleMove;
                        newCheckerMove(&possibleMove, 1, loc, loc + 14, loc + 7, promote, &nullMove);

                        if(!promote){
                            struct CheckerMove possibleMoveCopy = copyCheckerMove(&possibleMove);
                            struct Checker* boardCopy = copyBoard(board);

                            apply(boardCopy, &possibleMoveCopy);
                            struct Checker* capturingPiece = getPiece(boardCopy, loc + 14);
                            struct CheckerMove* nextPossibleMoves = getPossibleMoves(boardCopy, capturingPiece);

                            if((*(nextPossibleMoves)).type == 1){
                                chained = true;
                                int i = 0;

                                while((*(nextPossibleMoves + i)).type == 1){
                                    possibleMovesCount++;

                                    struct CheckerMove possibleMoveCopy = copyCheckerMove(&possibleMove);
                                    possibleMoveCopy.next = &(*(nextPossibleMoves + i));

                                    *(possibleMoves + possibleMovesCount - 1) = possibleMoveCopy;
                                    i++;
                                }
                            }
                        }

                        if(!chained){
                            possibleMovesCount++;
                            *(possibleMoves + possibleMovesCount - 1) = possibleMove;
                        }
                    }
                }
            }

            if(loc%8 < 6){
                if(loc <= 45){
                    if((*(board + loc + 9)).side == -side && isSpaceFree(board, loc + 18)){
                        bool promote = canPromote(checker, loc + 18);
                        bool chained = false;

                        struct CheckerMove possibleMove;
                        newCheckerMove(&possibleMove, 1, loc, loc + 18, loc + 9, promote, &nullMove);

                        if(!promote){
                            struct CheckerMove possibleMoveCopy = copyCheckerMove(&possibleMove);
                            struct Checker* boardCopy = copyBoard(board);

                            apply(boardCopy, &possibleMoveCopy);
                            struct Checker* capturingPiece = getPiece(boardCopy, loc + 18);
                            struct CheckerMove* nextPossibleMoves = getPossibleMoves(boardCopy, capturingPiece);

                            if((*(nextPossibleMoves)).type == 1){
                                chained = true;
                                int i = 0;

                                while((*(nextPossibleMoves + i)).type == 1){
                                    possibleMovesCount++;

                                    struct CheckerMove possibleMoveCopy = copyCheckerMove(&possibleMove);
                                    possibleMoveCopy.next = &(*(nextPossibleMoves + i));

                                    *(possibleMoves + possibleMovesCount - 1) = possibleMoveCopy;
                                    i++;
                                }
                            }
                        }

                        if(!chained){
                            possibleMovesCount++;
                            *(possibleMoves + possibleMovesCount - 1) = possibleMove;
                        }
                    }
                }
            }
        }

        *(possibleMoves + possibleMovesCount) = nullMove;
        return possibleMoves;
    }else{
        struct CheckerMove* possibleMoves = (struct CheckerMove*) malloc(5 * sizeof(struct CheckerMove));
        int possibleMovesCount = 0;

        if(is_king || side == 1){
            if(loc%8 >= 1 && loc >= 9 && isSpaceFree(board, loc - 9)){
                possibleMovesCount++;
                bool promote = canPromote(checker, loc - 9);

                struct CheckerMove possibleMove;
                newCheckerMove(&possibleMove, 0, loc, loc - 9, -1, promote, &nullMove);

                *(possibleMoves + possibleMovesCount - 1) = possibleMove;
            }

            if(loc%8 <= 6 && loc >= 8 && isSpaceFree(board, loc - 7)){
                possibleMovesCount++;
                bool promote = canPromote(checker, loc - 7);

                struct CheckerMove possibleMove;
                newCheckerMove(&possibleMove, 0, loc, loc - 7, -1, promote, &nullMove);

                *(possibleMoves + possibleMovesCount - 1) = possibleMove;
            }
        }

        if(is_king || side == -1){
            if(loc%8 >= 1 && loc <= 55 && isSpaceFree(board, loc + 7)){
                possibleMovesCount++;
                bool promote = canPromote(checker, loc + 7);

                struct CheckerMove possibleMove;
                newCheckerMove(&possibleMove, 0, loc, loc + 7, -1, promote, &nullMove);

                *(possibleMoves + possibleMovesCount - 1) = possibleMove;
            }

            if(loc%8 <= 6 && loc <= 54 && isSpaceFree(board, loc + 9)){
                possibleMovesCount++;
                bool promote = canPromote(checker, loc + 9);

                struct CheckerMove possibleMove;
                newCheckerMove(&possibleMove, 0, loc, loc + 9, -1, promote, &nullMove);

                *(possibleMoves + possibleMovesCount - 1) = possibleMove;
            }
        }

        *(possibleMoves + possibleMovesCount) = nullMove;
        return possibleMoves;
    }
}

struct CheckerMove* getAllPossibleMoves(struct Checker* board, int side){
    struct CheckerMove *allPossibleMoves = (struct CheckerMove*) malloc(12 * 11 * sizeof(struct CheckerMove*));

    int allPossibleMovesCount = 0;
    int i;

    bool capturesExist = false;

    for(i = 0; i < 64; i++){
        if((*(board + i)).side == side){
            struct Checker* piece = getPiece(board, i);
            struct CheckerMove *possiblePieceMoves = getPossibleMoves(board, piece);

            if((*possiblePieceMoves).type == 1){
                capturesExist = true;
            }else if(capturesExist){
                continue;
            }

            int j = 0;

            while((*(possiblePieceMoves + j)).type != -1){
                allPossibleMovesCount++;
                *(allPossibleMoves + allPossibleMovesCount - 1) = *(possiblePieceMoves + j);
                j++;
            }
        }
    }

    *(allPossibleMoves + allPossibleMovesCount) = nullMove;

    if(!capturesExist){
        return allPossibleMoves;
    }

    struct CheckerMove* allPossibleCaptures = (struct CheckerMove*) malloc(12 * 11 * sizeof(struct CheckerMove*));;
    int allPossibleCapturesCount = 0;
    i = 0;

    while((*(allPossibleMoves + i)).type != -1){
        if((*(allPossibleMoves + i)).type == 1){
            allPossibleCapturesCount++;
            *(allPossibleCaptures + allPossibleCapturesCount - 1) = *(allPossibleMoves + i);
        }

        i++;
    }

    *(allPossibleCaptures + allPossibleCapturesCount) = nullMove;
    return allPossibleCaptures;
};

void newCheckerMove(struct CheckerMove *move, int type, int source, int destination, int capturedLoc, bool promote, struct CheckerMove *next){
    move->type = type;
    move->source = source;
    move->destination = destination;
    move->capturedLoc = capturedLoc;
    move->promote = promote;
    move->next = next;
}

void appendNewCapture(struct CheckerMove *move, struct CheckerMove *capture){
    struct CheckerMove *p = move;

    while(p->next != NULL){
        p = p->next;
    }

    p->next = capture;
}

struct CheckerMove copyCheckerMove(struct CheckerMove *move){
    struct CheckerMove move_copy;
    newCheckerMove(&move_copy, move->type, move->source, move->destination, move->capturedLoc, move->promote, move->next);

    return move_copy;
};

struct CheckerMove makeNullCheckerMove(){
    struct CheckerMove nullMove;
    newCheckerMove(&nullMove, -1, -1, -1, -1, false, &nullMove);

    return nullMove;
};

void resetBoard(){
    int resetBlackSquares[12] = {1, 3, 5, 7, 8, 10, 12, 14, 17, 19, 21, 23};
    int resetWhiteSquares[12] = {40, 42, 44, 46, 49, 51, 53, 55, 56, 58, 60, 62};

    int i;

    for(i = 0; i < 12; i++){
        struct Checker blackPiece;
        struct Checker whitePiece;

        newChecker(&blackPiece, resetBlackSquares[i], -1, false);
        newChecker(&whitePiece, resetWhiteSquares[i], 1, false);

        *(board + resetBlackSquares[i]) = blackPiece;
        *(board + resetWhiteSquares[i]) = whitePiece;
    }

    int resetEmptySquares[8] = {24, 26, 28, 30, 33, 35, 37, 39};

    for(i = 0; i < 8; i++){
        struct Checker emptySquare;
        newChecker(&emptySquare, resetEmptySquares[i], 0, false);
        *(board + resetEmptySquares[i]) = emptySquare;
    }
}

void drawBoard(struct Checker* board){
    printf("|---|---|---|---|---|---|---|---|\n");
    int index;

    for(index = 0; index < 64; index++){
        if(index%8 == 0){
            printf("|");
        }

        printf(" %c ", getCheckerSymbol(&(*(board + index))));
        printf("|");

        if(index%8 == 7){
            printf("\n");
            printf("|---|---|---|---|---|---|---|---|\n");
        }
    }
}

struct Checker* copyBoard(struct Checker* board){
    struct Checker* boardCopy = malloc(64 * sizeof(struct Checker));
    int i;

    for(i = 0; i < 64; i++){
        boardCopy[i] = *(board + i);
    }

    return boardCopy;
}

bool isSpaceFree(struct Checker* board, int loc){
    return (*(board + loc)).side == 0;
}

struct Checker* getPiece(struct Checker* board, int loc){
    return &(*(board + loc));
};

void apply(struct Checker* board, struct CheckerMove* checkerMove){
    int source = checkerMove->source;
    struct Checker* piece = getPiece(board, source);

    int destination = checkerMove->destination;
    movePiece(board, piece, destination);

    struct Checker* newPiece = getPiece(board, destination);

    if(checkerMove->promote){
        promote(newPiece);
    }

    if(checkerMove->type == 1){
        struct Checker* capturedPiece = getPiece(board, checkerMove->capturedLoc);
        captured(board, capturedPiece);

        if(!(checkerMove->promote) && checkerMove->next->type != -1){
            apply(board, checkerMove->next);
        }
    }
}

bool hasNoPieces(struct Checker* board, int side){
    int i;

    for(i = 0; i < 64; i++){
        if((*(board + i)).side == side){
            return false;
        }
    }

    return true;
}

bool hasNoMoves(struct Checker* board, int side){
    struct CheckerMove* allPossibleMoves = getAllPossibleMoves(board, side);
    return (*allPossibleMoves).type == -1;
}

bool isWinner(struct Checker* board, int side){
    return hasNoPieces(board, -side) || hasNoMoves(board, -side);
}

// TODO: Play around with the weights
double evalBoard(struct Checker* board, int side){
    double boardEval = 0.0;
    int i;

    for(i = 0; i < 64; i++){
        if((*(board + i)).side != 0){

            struct Checker* piece = getPiece(board, i);
            double colorWeight = (double) piece->side;
            int loc = piece->loc;
            double kingWeight = 1.0;

            if(piece->is_king){
                kingWeight = 1.5;
            }

            double pieceWeight = colorWeight * kingWeight;
            boardEval += 2.0 * pieceWeight;

            // Check for centralization
            if(loc%8 >= 2 && loc%8 <= 5 && loc >= 18 && loc <= 45){
                boardEval += pieceWeight * 1.25;
            }

            // Check for how advanced the piece is
            if((colorWeight == -1 && loc >= 32) || (colorWeight == 1 && loc <= 31)){
                boardEval += pieceWeight * 1.25;
            }else{ // Piece is in the "back" of the board
                boardEval += pieceWeight * 1.0;
            }
        }
    }

    return ((double) side) * boardEval;
}

struct CheckerMove findBestMove(struct Checker* board, int side){
    struct CheckerMove bestMove;

    if(isWinner(board, -side)){
        return nullMove;
    }

    return alphaBeta(board, INT_MIN, INT_MAX, 4, side);
};

struct CheckerMove alphaBeta(struct Checker* board, int alpha, int beta, int depth, int side){
    struct CheckerMove* allPossibleMoves = getAllPossibleMoves(board, side);

    struct CheckerMove bestMove = nullMove;
    double bestEval = INT_MIN;
    int i = 0;

    while((*(allPossibleMoves + i)).type != -1){
        struct CheckerMove possibleMove = *(allPossibleMoves + i);
        struct Checker* boardCopy = copyBoard(board);
        apply(boardCopy, &possibleMove);

        double possibleEval;

        if(depth == 1){
            possibleEval = evalBoard(boardCopy, -side);
        }else{
            struct CheckerMove bestResponse = alphaBeta(boardCopy, -beta, -alpha, depth - 1, -side);
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
