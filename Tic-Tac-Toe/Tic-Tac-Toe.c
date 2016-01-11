#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

char board[10];
char playerLetter;
char computerLetter;

bool playerTurn;
bool playerGoesFirst();

void resetBoard();
void drawBoard();
char inputPlayerLetter();

void makeMove(char letter, int move);
void undoMove(int move);

int forkCount(int move);
bool isWinner(char side);
bool isSpaceFree(int move);

int getPlayerMove();
int getComputerMove();

bool isBoardFull();
bool isBoardEmpty();
bool isPlayingAgain();

int main()
{
    printf("Welcome to Tic Tac Toe!\n");

    for(;;){
        resetBoard();
        playerLetter = inputPlayerLetter();
        playerTurn = playerGoesFirst();

        if(playerLetter == 'X'){
            computerLetter = 'O';
        }else{
            computerLetter = 'X';
        }

        printf("\nYou will play '%c'\n", playerLetter);

        if(playerTurn){
            printf("You will move first\n\n");
        }else{
            printf("The computer will go first\n\n");
        }

        bool gameIsPlaying = true;

        while(gameIsPlaying){
            if(playerTurn){
                drawBoard();
                int move = getPlayerMove();
                makeMove(playerLetter, move);

                if(isWinner(playerLetter)){
                    drawBoard();
                    printf("Hooray! You have won the game!\n");
                    gameIsPlaying = false;
                }else{
                    if(isBoardFull()){
                        drawBoard();
                        printf("The game is a tie!\n");
                        gameIsPlaying = false;
                    }else{
                        playerTurn = false;
                    }
                }
            }else{
                int move = getComputerMove();
                printf("Computer's Move: %d\n\n", move);
                makeMove(computerLetter, move);

                if(isWinner(computerLetter)){
                    drawBoard();
                    printf("The computer has beaten you! You lose!\n");
                    gameIsPlaying = false;
                }else{
                    if(isBoardFull()){
                        drawBoard();
                        printf("The game is a tie!\n");
                        gameIsPlaying = false;
                    }else{
                        playerTurn = true;
                    }
                }
            }
        }

        if(!isPlayingAgain()){
            return 0;
        }
    }
}

void resetBoard(){
    int i;

    for(i = 1; i <= 9; i++){
        board[i] = ' ';
    }
}

void drawBoard(){
    printf("   |   |\n");
    printf(" %c | %c | %c\n", board[1], board[2], board[3]);
    printf("   |   |\n");
    printf("-----------\n");
    printf("   |   |\n");
    printf(" %c | %c | %c\n", board[4], board[5], board[6]);
    printf("   |   |\n");
    printf("-----------\n");
    printf("   |   |\n");
    printf(" %c | %c | %c\n", board[7], board[8], board[9]);
    printf("   |   |\n\n");
}

bool playerGoesFirst(){
    int toss = rand() % 2;
    return (toss == 1);
}

char inputPlayerLetter(){
    char letter;
    bool valid = false;

    while(valid == false){
        printf("Do you want to be X or O? ");
        scanf(" %c", &letter);

        letter = toupper(letter);

        if(letter != 'X' && letter != 'O'){
            printf("Invalid choice '%c'! Expected either X or O (non-cap sensitive)\n", letter);
        }else{
            valid = true;
        }
    }

    return letter;
}

void makeMove(char letter, int move){
    board[move] = letter;
}

void undoMove(int move){
    board[move] = ' ';
}

int forkCount(int location){
    int side = board[location];
    int twoChainCount = 0;

    if((board[(location + 2)%9 + 1] == side && \
        isSpaceFree((location + 5)%9 + 1)) || \
       (board[(location + 5)%9 + 1] == side && \
        isSpaceFree((location + 2)%9 + 1))){
            twoChainCount += 1;
        }

    int rowOffset;

    if(location%3 == 0){
        rowOffset = location - location%3 - 3;
    }else{
        rowOffset = location - location%3;
    }

    if((board[location%3 + 1 + rowOffset] == side && \
        isSpaceFree((location + 1)%3 + 1 + rowOffset)) || \
        (board[(location + 1)%3 + 1 + rowOffset] == side && \
        isSpaceFree(location%3 + 1 + rowOffset))){
        twoChainCount++;
    }

    if(location == 1 || location == 5 || location == 9){
        if((board[(location + 4)%12] == side && \
            isSpaceFree((location + 8)%12)) || \
           (board[(location + 8)%12] == side && \
            isSpaceFree((location + 4)%12))){
                twoChainCount++;
            }
    }

    if(location == 3){
        if((board[5] == side && isSpaceFree(7)) || \
           (board[7] == side && isSpaceFree(5))){
            twoChainCount++;
           }
    }else if(location == 5){
        if((board[3] == side && isSpaceFree(7)) || \
           (board[7] == side && isSpaceFree(3))){
            twoChainCount++;
           }
    }else if(location == 7){
        if((board[3] == side && isSpaceFree(5)) || \
           (board[5] == side && isSpaceFree(3))){
            twoChainCount++;
           }
    }

    return twoChainCount;
}

bool isWinner(char side){
    return (board[7] == side && board[8] == side && board[9] == side) || \
    (board[4] == side && board[5] == side && board[6] == side) || \
    (board[1] == side && board[2] == side && board[3] == side) || \
    (board[7] == side && board[4] == side && board[1] == side) || \
    (board[8] == side && board[5] == side && board[2] == side) || \
    (board[9] == side && board[6] == side && board[3] == side) || \
    (board[7] == side && board[5] == side && board[3] == side) || \
    (board[9] == side && board[5] == side && board[1] == side);
}

bool isSpaceFree(int move){
    return board[move] == ' ';
}

int getPlayerMove(){
    int move = 0;
    bool valid = false;

    while(valid == false){
        printf("What is your next move (1-9)? ");
        scanf("%d", &move);

        if(!(move >= 1 && move <= 9)){
            printf("Invalid move choice!\n");
        }else if(!isSpaceFree(move)){
            printf("Square occupied!\n");
        }else{
            valid = true;
        }
    }

    return move;
}

int getComputerMove(){
    int i;

    int sides[4] = {2, 4, 6, 8};
    int corners[4] = {1, 3, 7, 9};

    if(isBoardEmpty()){
        int cornerIndex = rand()%4;
        return corners[cornerIndex];
    }

    for(i = 1; i < 10; i++){
        if(isSpaceFree(i)){
            makeMove(computerLetter, i);

            if(isWinner(computerLetter)){
                undoMove(i);
                return i;
            }

            undoMove(i);
        }
    }

    for(i = 1; i < 10; i++){
        if(isSpaceFree(i)){
            makeMove(playerLetter, i);

            if(isWinner(playerLetter)){
                undoMove(i);
                return i;
            }

            undoMove(i);
        }
    }

    for(i = 1; i < 10; i++){
        if(isSpaceFree(i)){
            makeMove(computerLetter, i);

            if(forkCount(i) == 2){
                undoMove(i);
                return i;
            }

            undoMove(i);
        }
    }

    int playerForkCount = 0;
    int firstPlayerFork = 0;

    for(i = 1; i < 10; i++){
        if(isSpaceFree(i)){
            makeMove(playerLetter, i);

            if(forkCount(i) == 2){
                playerForkCount++;

                if(firstPlayerFork == 0){
                    firstPlayerFork = i;
                }
            }

            undoMove(i);
        }
    }

    if(playerForkCount > 0){
        if(playerForkCount == 1){
            return firstPlayerFork;
        }else{
            if(board[5] == computerLetter){
                for(i = 0; i < 4; i++){
                    if(isSpaceFree(sides[i])){
                        return sides[i];
                    }
                }
            }else{
                for(i = 0; i < 4; i++){
                    if(isSpaceFree(corners[i])){
                        return corners[i];
                    }
                }
            }
        }
    }

    int cornerPlayer = 0;
    int cornerComputer = 0;
    int foundPattern = true;

    for(i = 0; i < 4; i++){
        if(board[corners[i]] == playerLetter){
            if(cornerPlayer == 0){
                cornerPlayer = corners[i];
            }else{
                foundPattern = false;
                break;
            }
        }else if(board[corners[i]] == computerLetter){
            if(cornerComputer == 0){
                cornerComputer = corners[i];
            }else{
                foundPattern = false;
                break;
            }
        }
    }

    printf("Here!");

    if(foundPattern && cornerPlayer > 0 && cornerComputer > 0){
        for(i = 0; i < 4; i++){
            if(isSpaceFree(corners[i])){
                return corners[i];
            }
        }
    }

    if(board[5] == playerLetter){
        board[5] = ' ';

        for(i = 0; i < 4; i++){
            if(board[corners[i]] == computerLetter){
                board[corners[i]] = ' ';

                if(isBoardEmpty()){
                    board[corners[i]] = computerLetter;
                    board[5] = playerLetter;

                    return 10 - corners[i];
                }

                board[corners[i]] = computerLetter;
            }
        }

        board[5] = playerLetter;
    }

    if(isSpaceFree(5)){
        return 5;
    }

    for(i = 0; i < 4; i++){
        if(board[10 - corners[i]] == playerLetter && isSpaceFree(corners[i])){
            return corners[i];
        }
    }

	for(i = 0; i < 4; i++){
		if(isSpaceFree(corners[i])){
			return corners[i];
		}
	}

	for(i = 0; i < 4; i++){
		if(isSpaceFree(sides[i])){
			return sides[i];
		}
	}
}

bool isBoardFull(){
    int i;

    for(i = 1; i <= 9; i++){
        if(isSpaceFree(i)){
            return false;
        }
    }

    return true;
}

bool isBoardEmpty(){
    int i;

    for(i = 1; i <= 9; i++){
        if(!isSpaceFree(i)){
            return false;
        }
    }

    return true;
}

bool isPlayingAgain(){
    printf("Do you want to play again? (Y or N)");

    char response;
    scanf(" %c", &response);

    return(toupper(response) == 'Y');
}
