// This code is for the computer to play itself,
// though it could be easily changed to involve
// human vs. computer play
//
// TODO: Improve the AI

#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Checker Class

struct Checker{
    int loc;
    int side;
};

void newChecker(struct Checker* checker, int loc, int side);
char getCheckerSymbol(struct Checker* checker);
struct CheckerMove* getPossibleMoves(struct Checker* board, struct Checker* checker);

// CheckerMove Class

struct CheckerMove{
    int loc;
    int side;
};

static struct CheckerMove nullMove;

void newCheckerMove(struct CheckerMove* move, int loc, int side);
struct CheckerMove copyCheckerMove(struct CheckerMove* move);
struct CheckerMove makeNullCheckerMove();

// Checkerboard Methods

struct Checker board[42];

void resetBoard();
void drawBoard(struct Checker* board);
struct Checker* copyBoard(struct Checker* board);
bool isSpaceFree(struct Checker* board, int loc);
int getChainCountFromCell(struct Checker* board, int loc, int dir);
int getLongestChainCountFromCell(struct Checker* board, int loc);
void apply(struct Checker* board, struct CheckerMove* checkerMove);

// Game and AI Methods
struct CheckerMove* getAllPossibleMoves(struct Checker* board, int side);
bool hasNoMoves(struct Checker* board, int side);
bool isWinner(struct Checker* board, int side);

double evalBoard(struct Checker* board, int side);
struct CheckerMove findBestMove(struct Checker* board, int alpha, int beta, int depth, int side);

void main(){
    resetBoard();
    nullMove = makeNullCheckerMove();

    int turn = 1;
    struct CheckerMove nextMove;
    struct CheckerMove* allPossibleMoves;

    while(!isWinner(board, turn)){
        nextMove = findBestMove(board, INT_MIN, INT_MAX, 4, turn);
        apply(board, &nextMove);

        printf("Side %d's Move: %d\n\n", turn, nextMove.loc);
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

void newChecker(struct Checker *checker, int loc, int side){
    checker->loc = loc;
    checker->side = side;
}

char getCheckerSymbol(struct Checker *checker){
    if(checker->side == 1){
        return 'R';
    }else if(checker->side == -1){
        return 'B';
    }else{
        return ' ';
    }
}

struct CheckerMove* getAllPossibleMoves(struct Checker* board, int side){
    struct CheckerMove *allPossibleMoves = (struct CheckerMove*) malloc(8 * sizeof(struct CheckerMove));

    int allPossibleMovesCount = 0;
    int i;
    int j;

    for(i = 35; i < 42; i++){
        j = i;

        while(j >= 0){
            if(isSpaceFree(board, j)){
                struct CheckerMove possibleMove;
                newCheckerMove(&possibleMove, j, side);

                allPossibleMovesCount++;
                *(allPossibleMoves + allPossibleMovesCount - 1) = possibleMove;

                break;
            }

            j -= 7;
        }
    }

    *(allPossibleMoves + allPossibleMovesCount) = nullMove;
    return allPossibleMoves;
}

void newCheckerMove(struct CheckerMove *move, int loc, int side){
    move->loc = loc;
    move->side = side;
}

struct CheckerMove copyCheckerMove(struct CheckerMove *move){
    struct CheckerMove moveCopy;
    newCheckerMove(&moveCopy, move->loc, move->side);

    return moveCopy;
}

struct CheckerMove makeNullCheckerMove(){
    struct CheckerMove nullMove;
    newCheckerMove(&nullMove, -1, 0);

    return nullMove;
}

void resetBoard(){
    int i;

    for(i = 0; i < 42; i++){
        struct Checker nullPiece;
        newChecker(&nullPiece, i, 0);

        *(board + i) = nullPiece;
    }
}

void drawBoard(struct Checker* board){
    printf("|---|---|---|---|---|---|---|\n");
    int index;

    for(index = 0; index < 42; index++){
        if(index%7 == 0){
            printf("|");
        }

        printf(" %c ", getCheckerSymbol(&(*(board + index))));
        printf("|");

        if(index%7 == 6){
            printf("\n");
            printf("|---|---|---|---|---|---|---|\n");
        }
    }
}

struct Checker* copyBoard(struct Checker* board){
    struct Checker* boardCopy = malloc(64 * sizeof(struct Checker));
    int i;

    for(i = 0; i < 42; i++){
        *(boardCopy + i) = *(board + i);
    }

    return boardCopy;
}

bool isSpaceFree(struct Checker* board, int loc){
    return (*(board + loc)).side == 0;
}

void apply(struct Checker* board, struct CheckerMove* checkerMove){
    int loc = checkerMove->loc;
    int side = checkerMove->side;

    struct Checker placedPiece;
    newChecker(&placedPiece, loc, side);

    *(board + loc) = placedPiece;
}

int getChainCountFromCell(struct Checker* board, int loc, int dir){
    int side = (*(board + loc)).side;

    if(side != 1 && side != -1){
        return 0;
    }

    int delta;
    int maxCount;

    switch(dir){
    case 2:
        delta = -7;
        maxCount = (loc - (loc%7)) / 7;
        break;

    case 4:
        delta = 1;
        maxCount = 7 - (loc%7);
        break;

    case 6:
        delta = 7;
        maxCount = (35 - loc + (loc%7)) / 7;
        break;

    case 8:
        delta = -1;
        maxCount = loc%7;
        break;

    case 1:
        delta = -8;
        maxCount = (loc%7 >= (loc - (loc%7)) / 7) ? (loc - (loc%7)) / 7 : loc%7;
        break;

    case 3:
        delta = -6;
        maxCount = (7 - (loc%7) >= (loc - (loc%7)) / 7) ? (loc - (loc%7)) / 7 : 7 - (loc%7);
        break;

    case 5:
        delta = 8;
        maxCount = (7 - (loc%7) >= (35 - loc + (loc%7)) / 7) ? (35 - loc + (loc%7)) / 7 : 7 - (loc%7);
        break;

    case 7:
        delta = 6;
        maxCount = loc%7 >= (35 - loc + (loc%7)) / 7 ? (35 - loc + (loc%7)) / 7 : loc%7;
        break;

    default:
        break;
    }

    int totalCount = 0;

    while(totalCount < maxCount){
        if((*(board + loc + (totalCount + 1) * delta)).side == side){
            totalCount++;
        }else{
            return totalCount;
        }
    }

    return totalCount;
}

int getLongestChainCountFromCell(struct Checker* board, int loc){
    int maxChainCount = 1;
    int dirMaxChainCount;
    int i;

    for(i = 1; i <= 4; i++){
        dirMaxChainCount = getChainCountFromCell(board, loc, i) + 1 + getChainCountFromCell(board, loc, i + 4);
        maxChainCount = (dirMaxChainCount > maxChainCount) ? dirMaxChainCount : maxChainCount;
    }

    return maxChainCount;
}

bool hasNoMoves(struct Checker* board, int side){
    struct CheckerMove* allPossibleMoves = getAllPossibleMoves(board, side);
    return (*allPossibleMoves).loc == -1;
}

bool isWinner(struct Checker* board, int side){
    int i;

    for(i = 0; i < 42; i++){
        if((*(board + i)).side == side){
            if(getLongestChainCountFromCell(board, i) >= 4){
                return true;
            }
        }
    }

    return false;
}

bool isTie(struct Checker* board, int side){
    return hasNoMoves(board, side) && !isWinner(board, -side);
}

// TODO: Play around with the weights
double evalBoard(struct Checker* board, int side){
    if(isTie(board, side)){
        return 0.0;
    }

    if(isWinner(board, -side)){
        return INT_MIN;
    }

    if(isWinner(board, side)){
        return INT_MAX;
    }

    double centralScore = 0.0;

    double sideMaxLength = 1.0;
    double otherMaxLength = 1.0;
    double secondOtherMaxLength = otherMaxLength - 1;

    int sideMaxLengthCount = 0;
    int otherMaxLengthCount = 0;
    int secondOtherMaxLengthCount = 0;

    int i;
    int checkerSide;

    for(i = 0; i < 42; i++){
        checkerSide = (*(board + i)).side;

        if(checkerSide == side){
            centralScore -= abs(3 - i%7);
        }else if(checkerSide == -side){
            centralScore += abs(3 - i%7);
        }

        if(checkerSide == 1){
            int longestChainCountFromCell = getLongestChainCountFromCell(board, i);

            if(longestChainCountFromCell > sideMaxLength){
                sideMaxLength = longestChainCountFromCell;
                sideMaxLengthCount = 1;
            }else if(longestChainCountFromCell == sideMaxLength){
                sideMaxLengthCount++;
            }
        }else if(checkerSide == -1){
            int longestChainCountFromCell = getLongestChainCountFromCell(board, i);

            if(longestChainCountFromCell > otherMaxLength){
                secondOtherMaxLength = otherMaxLength;
                secondOtherMaxLengthCount = otherMaxLengthCount;

                otherMaxLength = longestChainCountFromCell;
                otherMaxLengthCount = 1;
            }else if(longestChainCountFromCell == otherMaxLength){
                otherMaxLengthCount++;
            }else if(longestChainCountFromCell > secondOtherMaxLength){
                secondOtherMaxLength = longestChainCountFromCell;
                secondOtherMaxLengthCount = 1;
            }else if(longestChainCountFromCell == secondOtherMaxLength){
                otherMaxLengthCount++;
            }
        }
    }

    return centralScore + side * (sideMaxLength * sideMaxLengthCount - otherMaxLength * otherMaxLengthCount - secondOtherMaxLength * secondOtherMaxLengthCount);
}

struct CheckerMove findBestMove(struct Checker* board, int alpha, int beta, int depth, int side){
    if(isWinner(board, side) || isWinner(board, -side) || isTie(board, side)){
        return nullMove;
    }

    struct CheckerMove* allPossibleMoves = getAllPossibleMoves(board, side);

    struct CheckerMove bestMove = nullMove;
    double bestEval = INT_MIN;
    int i = 0;

    while((*(allPossibleMoves + i)).loc != -1){
        struct CheckerMove possibleMove = *(allPossibleMoves + i);
        struct Checker* boardCopy = copyBoard(board);
        apply(boardCopy, &possibleMove);

        if(isWinner(boardCopy, side)){
            return possibleMove;
        }

        i++;
    }

    i = 0;

    while((*(allPossibleMoves + i)).loc != -1){
        struct CheckerMove possibleMove = *(allPossibleMoves + i);
        possibleMove.side = -possibleMove.side;
        struct Checker* boardCopy = copyBoard(board);
        apply(boardCopy, &possibleMove);

        if(isWinner(boardCopy, -side)){
            possibleMove.side = -possibleMove.side;
            return possibleMove;
        }

        i++;
    }

    i = 0;

    while((*(allPossibleMoves + i)).loc != -1){
        struct CheckerMove possibleMove = *(allPossibleMoves + i);
        struct Checker* boardCopy = copyBoard(board);
        apply(boardCopy, &possibleMove);

        double possibleEval;

        if(depth == 1){
            possibleEval = evalBoard(boardCopy, -side);
        }else{
            struct CheckerMove bestResponse = findBestMove(boardCopy, -beta, -alpha, depth - 1, -side);
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
