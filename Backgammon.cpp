#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <conio.h>
#include <cstdio>
#include <time.h>
#include <stdio.h>
#include <math.h>

#define SIZE 12

#define UP_PART 1
#define INVALID 0
#define DOWN_PART -1
#define BAR 100
#define HOME 200
#define MAGENTA         5
#define BLACK           0
#define WHITE           15
#define DARKGRAY        8
struct Game
{
    int i = 12;
    int j = 12;
    int h = 6;//height
    int you = 0;
    int me = 0;
    int cub1 = 0;
    int cub2 = 0;
    short x = 30, y = 5;
    char input = '\0';
    int matrix[SIZE][SIZE];
    int colStart = 0;
    int colEnd = 0;
    const char* SaveFile = "gamedata.txt";
    int catchW = 0;//white
    int catchB = 0;//black
    bool isWhiteMove = true;
    int firstmove1 = 0;
    int firstmove2 = 0;
    bool playerCheck = true;
    bool badMove = false;
    char buff[20];
    int countOfUserMoves = 0;
};


///////   TEXT SETTINGS   /////////////////////
COORD GetConsoleCursorPosition(HANDLE hConsoleOutput) {
    CONSOLE_SCREEN_BUFFER_INFO cbsi;
    if (GetConsoleScreenBufferInfo(hConsoleOutput, &cbsi)) {
        return cbsi.dwCursorPosition;
    }
    else {
        COORD invalid = { 0, 0 };
        return invalid;
    }
}
void setConsoleColor(HANDLE hConsole, int text, int back) {
    SetConsoleTextAttribute(hConsole, back << 4 | text);
}
void ResetColor(HANDLE hConsole) {
    SetConsoleTextAttribute(hConsole, 0 << 4 | 7);
}
void PrintColorText(HANDLE hConsole, const char* text, int front, int back) {
    setConsoleColor(hConsole, front, back);
    _cprintf(text);
    ResetColor(hConsole);
}
/////////////////////////////////////////////
///////   FILE SAVING   /////////////////////

void Saving(Game* game) {
    FILE* file = fopen(game->SaveFile, "w");
    for (int i = 0; i < 12; i++)
    {
        if (i == 6) {
            fprintf(file, "\n");
        }
        for (int j = 0; j < 12; j++)
        {
            fprintf(file, "%d ", game->matrix[i][j]);
        }
        fprintf(file, "\n");
    }
    fprintf(file, "%d %d\n", game->you, game->me);
    fprintf(file, "%d %d\n", game->catchB, game->catchW);
    fprintf(file, "%d %d\n", game->cub1, game->cub2);
    fprintf(file, "%d\n", (int)game->isWhiteMove);
    fprintf(file, "%d\n", game->countOfUserMoves);



    //fwrite(game, sizeof(Game), 1, file);
    fclose(file);
}
void Load(Game* game) {
    FILE* file = fopen(game->SaveFile, "r");
    for (int i = 0; i < 12; i++)
    {
        for (int j = 0; j < 12; j++)
        {
            fscanf(file, "%d", &game->matrix[i][j]);
        }
    }
    fscanf(file, "%d %d", &game->you, &game->me);
    fscanf(file, "%d %d", &game->catchB, &game->catchW);
    fscanf(file, "%d %d", &game->cub1, &game->cub2);
    int tmp;
    fscanf(file, "%d", &tmp);
    game->isWhiteMove = tmp;
    fscanf(file, "%d", &game->countOfUserMoves);

    //fread(game, sizeof(Game), 1, file);
    fclose(file);
}
bool IsFileExist(const char* gamedata) {
    FILE* file = fopen(gamedata, "r");
    if (file == NULL) {
        return false;
    }
    else {
        fclose(file);
        return true;
    }
}

/////////////////////////////////////////////
///////   LOGIC   ///////////////////////////

void MatrixClear(Game* game) {
    for (int i = 0; i < 12; i++)
    {
        for (int j = 0; j < 12; j++)
        {
            game->matrix[i][j] = 0;
        }
    }

    for (int i = 0; i < 5; i++) game->matrix[i][0] = 1;
    for (int i = 0; i < 2; i++) game->matrix[i][11] = 1;
    for (int i = 9; i < 12; i++) game->matrix[i][4] = 1;
    for (int i = 7; i < 12; i++) game->matrix[i][6] = 1;

    for (int i = 7; i < 12; i++) game->matrix[i][0] = 2;
    for (int i = 0; i < 3; i++) game->matrix[i][4] = 2;
    for (int i = 0; i < 5; i++) game->matrix[i][6] = 2;
    for (int i = 10; i < 12; i++) game->matrix[i][11] = 2;

}
void Score(HANDLE hConsole, Game* game) {
    SetConsoleCursorPosition(hConsole, COORD{ short(game->x + 70), short(0) });
    game->you = 0;
    game->me = 0;
    printf("You: %d", game->you);
    printf("Me: %d", game->me);
}
bool IsAllWhitePawsInTheHome(Game* game) {
    if (game->catchW != 0) {
        return false;
    }
    for (int i = 0; i < 12; i++)
    {
        for (int j = 0; j < 12; j++)
        {
            if (!(i >= 6 && j >= 6)) {
                if (game->matrix[i][j] == 1) {
                    return false;
                }
            }
        }
    }
    return true;
}
bool IsAllBlackPawsInTheHome(Game* game) {
    if (game->catchB != 0) {
        return false;
    }
    for (int i = 0; i < 12; i++)
    {
        for (int j = 0; j < 12; j++)
        {
            if (!(i < 6 && j >= 6)) {
                if (game->matrix[i][j] == 2) {
                    return false;
                }
            }
        }
    }
    return true;
}
void GenerateFirstMove(HANDLE h, Game* game) {

    srand(time(NULL));
    game->firstmove1 = rand() % 6 + 1;
    game->firstmove2 = rand() % 6 + 1;
    if (game->firstmove1 > game->firstmove2) {
        game->isWhiteMove = true;
    }
    else if ((game->firstmove1 < game->firstmove2)) {
        game->isWhiteMove = false;
    }
}
void GenerateNumbers(HANDLE h, Game* game) {
    srand(time(NULL));

    if (game->countOfUserMoves != 0) {
        return;
    }
    game->cub1 = rand() % 6 + 1;
    if ((game->isWhiteMove && !IsAllWhitePawsInTheHome(game)) ||
        (!game->isWhiteMove && !IsAllBlackPawsInTheHome(game))) {
        game->cub2 = rand() % 6 + 1;
    }
    else {
        game->cub2 = 0;
    }

    if (game->cub1 == game->cub2) {
        game->countOfUserMoves = game->cub1 * 4;
    }
    else {
        game->countOfUserMoves = (game->cub1 + game->cub2);
    }


}

bool FullHomeforWHITE(Game* game) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (game->matrix[i][j] == 1) {
                return false;
            }
        }
    }
    return true;
}
bool FullHomeforBLACK(Game* game) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (game->matrix[i][j] == 2) {
                return false;
            }
        }
    }
    return true;

}

int PartType(int col) {
    if (1 <= col && col <= 12) {
        return DOWN_PART;
    }
    else if (13 <= col && col <= 24) {
        return UP_PART;
    }
    return INVALID;
}

int ConvertUserColToMatrixCol(int userCol) {
    if (PartType(userCol) == DOWN_PART) {
        return 12 - userCol;
    }
    else if (PartType(userCol) == UP_PART) {
        return userCol - 13;
    }

    return -1;
}

void GetPawnCoord(Game* game, int col, int* x, int* y) {

    int col_ = ConvertUserColToMatrixCol(col);

    if (PartType(col) == DOWN_PART) {
        *x = col_;
        for (int i = 6; i < 12; i++)
        {
            if (game->matrix[i][col_] == 1 || game->matrix[i][col_] == 2) {
                *y = i - 1;
                return;
            }
        }
        *y = 11;
    }
    else if (PartType(col) == UP_PART) {
        *x = col_;
        for (int i = 5; i >= 0; i--)
        {
            if (game->matrix[i][col_] == 1 || game->matrix[i][col_] == 2) {
                *y = i + 1;
                return;
            }
        }
        *y = 0;
    }

}
bool IsPawnExist(Game* game, int y) {
    if (PartType(game->colStart) == UP_PART && y == 0) {
        return false;
    }
    if (PartType(game->colStart) == DOWN_PART && y == 11) {
        return false;
    }

    return true;

}
void CheckBoard(Game* game, int xEnd, int yEnd) {
    if (PartType(game->colEnd) == UP_PART && yEnd == 1 &&
        game->matrix[yEnd - 1][xEnd] != game->matrix[yEnd][xEnd]) {
        if (game->matrix[yEnd - 1][xEnd] == 1) {
            game->catchW++;
        }
        else {
            game->catchB++;
        }
        game->matrix[yEnd - 1][xEnd] = game->matrix[yEnd][xEnd];
        game->matrix[yEnd][xEnd] = 0;
    }
    else if (PartType(game->colEnd) == DOWN_PART && yEnd == 10 &&
        game->matrix[yEnd + 1][xEnd] != game->matrix[yEnd][xEnd]) {
        if (game->matrix[yEnd + 1][xEnd] == 1) {
            game->catchW++;
        }
        else {
            game->catchB++;
        }
        game->matrix[yEnd + 1][xEnd] = game->matrix[yEnd][xEnd];
        game->matrix[yEnd][xEnd] = 0;
    }

}
void FromBarMove(Game* game) {
    int xEnd, yEnd;
    GetPawnCoord(game, game->colEnd, &xEnd, &yEnd);
    if (game->isWhiteMove) {
        game->catchW--;
        game->matrix[yEnd][xEnd] = 1;
    }
    else {
        game->catchB--;
        game->matrix[yEnd][xEnd] = 2;
    }
    CheckBoard(game, xEnd, yEnd);
}
void ToHomeMove(Game* game) {
    int xStart, yStart;
    GetPawnCoord(game, game->colStart, &xStart, &yStart);
    if (IsPawnExist(game, yStart) == false) {
        game->badMove = true;
        return;
    }

    if (PartType(game->colStart) == UP_PART) {
        yStart--;
    }
    else if (PartType(game->colStart) == DOWN_PART) {
        yStart++;
    }
    if (game->matrix[yStart][xStart] != (game->isWhiteMove ? 1 : 2)) {
        game->badMove = true;
        return;
    }
    game->matrix[yStart][xStart] = 0;
}
void SimpleMoveChangeY(Game* game, int& yStart) {
    if (PartType(game->colStart) == UP_PART) {
        yStart--;
    }
    else if (PartType(game->colStart) == DOWN_PART) {
        yStart++;
    }
}
bool SimpleMoveCheckCorrect(Game* game, int yStart, int xStart, int yEnd, int xEnd) {
    if (game->matrix[yStart][xStart] != (game->isWhiteMove ? 1 : 2)) {

        return !(game->badMove = true);
    }
    if (PartType(game->colEnd) == UP_PART && yEnd != 0 &&
        (game->matrix[yEnd - 1][xEnd] != game->matrix[yStart][xStart] && yEnd != 1)) {
        return !(game->badMove = true);
    }
    if (PartType(game->colEnd) == DOWN_PART && yEnd != 11 &&
        (game->matrix[yEnd + 1][xEnd] != game->matrix[yStart][xStart] && yEnd != 10)) {
        return !(game->badMove = true);
    }
    return true;
}
void SimpleMove(Game* game) {
    int xStart, yStart, xEnd, yEnd;

    GetPawnCoord(game, game->colStart, &xStart, &yStart);
    GetPawnCoord(game, game->colEnd, &xEnd, &yEnd);


    if (IsPawnExist(game, yStart) == false) {
        game->badMove = true;
        return;
    }

    SimpleMoveChangeY(game, yStart);
    if (SimpleMoveCheckCorrect(game, yStart, xStart, yEnd, xEnd) == false) {
        return;
    }

    game->matrix[yEnd][xEnd] = game->matrix[yStart][xStart];
    game->matrix[yStart][xStart] = 0;
    CheckBoard(game, xEnd, yEnd);

}
void SaveMove(Game* game) {
    const char* fileName = "moves.txt";
    if (IsFileExist(fileName) == false) {
        FILE* f = fopen(fileName, "w");
        fclose(f);
    }
    FILE* f = fopen(fileName, "a");
    fprintf(f, "%d -> %d [%d]\n", game->colStart, game->colEnd, !game->isWhiteMove);
    fclose(f);
}

bool IsCorrectDirection(Game* game, int start, int end) {
    if (game->isWhiteMove != true) {
        return end - start > 0;
    }
    else {
        return start - end > 0;
    }
}

int PlaceToCoord(Game* game, int place) {
    if (place == BAR) {
        return (game->isWhiteMove ? 24 : 1);
    }
    else if (place == HOME) {
        return (game->isWhiteMove ? 0 : 25);
    }
    return place;
}
int GetDelta(Game* game, int place = 0) {
    int delta;
    if (place == BAR) {
        delta = abs(PlaceToCoord(game, BAR) - game->colEnd);
        if (delta > game->countOfUserMoves) {
            game->badMove = true;
            return 0;
        }
    }
    else if (place == HOME) {
        delta = abs(PlaceToCoord(game, HOME) - game->colStart);
        if (delta > game->countOfUserMoves) {
            game->badMove = true;
            return 0;
        }
    }
    else {
        delta = abs(game->colStart - game->colEnd);
        if (delta > game->countOfUserMoves) {
            game->badMove = true;
            return 0;
        }
    }
    return delta;
}
int Move(Game* game) {
    int delta;
    if (game->colStart == BAR) {
        if ((delta = GetDelta(game, BAR)) == 0) return 0;
        FromBarMove(game);
        if (game->badMove == false) {
            game->countOfUserMoves -= delta;
        }
    }
    else if (game->colEnd == HOME) {
        if ((delta = GetDelta(game, HOME)) == 0) return 0;
        ToHomeMove(game);
        if (game->badMove == false) {
            game->countOfUserMoves -= delta;
        }
    }
    else {
        if ((delta = GetDelta(game)) == 0) return 0;
        SimpleMove(game);
        if (game->badMove == false) {
            game->countOfUserMoves -= delta;
        }
    }
    return delta;
}
void PawnMove(Game* game) {

    if (!IsCorrectDirection(game, PlaceToCoord(game, game->colStart),
        PlaceToCoord(game, game->colEnd))) {
        game->badMove = true;
        return;
    }
    Move(game);

    if (game->badMove == false && game->countOfUserMoves == 0) {
        game->isWhiteMove = !game->isWhiteMove;
        game->cub1 = 0;
        game->cub2 = 0;
    }

    SaveMove(game);
}
void SelectPawnFrom(HANDLE h, Game* game) {
    SetConsoleCursorPosition(h, COORD{ short(game->x - 20), short(game->y + 20) });
    printf("From Col:\t");
    scanf(" %s", game->buff);
    if (game->buff[0] == 'B') {
        game->colStart = BAR;
        return;
    }
    game->colStart = atoi(game->buff);

}
void SelectPawnTo(HANDLE h, Game* game) {
    SetConsoleCursorPosition(h, COORD{ short(game->x - 20), short(game->y + 21) });
    printf("To Col:\t");
    scanf(" %s", game->buff);
    if (game->buff[0] == 'H') {
        game->colEnd = HOME;
        return;
    }
    game->colEnd = atoi(game->buff);
}
void SelectPawn(HANDLE h, Game* game) {
    if (game->countOfUserMoves <= 0) {
        game->badMove = true;
        return;
    }

    SelectPawnFrom(h, game);
    SelectPawnTo(h, game);

    PawnMove(game);
}
/////////////////////////////////////////////
///////   DRAW   ////////////////////////////

void DrawHorizontalLine(HANDLE hConsole, short x, short y, int length) {
    SetConsoleCursorPosition(hConsole, COORD{ x, y });
    for (int i = 0; i < length; i++) {
        _cprintf("=");
    }
}
void DrawUpVerticalLine(HANDLE hConsole, Game* game, int h) {
    for (int i = 0; i < h; i++) {
        SetConsoleCursorPosition(hConsole, COORD{ game->x, short(game->y + 1 + i) });
        _cprintf(":: ");
        for (int j = 0; j < 12; j++)
        {
            if (j == 6) {
                _cprintf("|||  ");
            }
            const char* out1 = (j % 2 == 0) ? ".." : "\\/";
            const char* out2 = (game->matrix[i][j] == 0) ? out1 : "[]";
            const char* space = (j == 11) ? "  " : "   ";
            if (game->matrix[i][j] == 1) {
                PrintColorText(hConsole, out2, WHITE, WHITE);
            }
            else if (game->matrix[i][j] == 2) {
                PrintColorText(hConsole, out2, DARKGRAY, DARKGRAY);
            }
            else {
                _cprintf(out2);
            }
            _cprintf(space);
        }
        _cprintf("::");
    }
}
void DrawDownVerticalLine(HANDLE hConsole, Game* game, int h) {//height
    int y = game->y + 10;
    for (int i = 0; i < game->h; i++) {
        SetConsoleCursorPosition(hConsole, COORD{ game->x, short(y + i) });
        _cprintf(":: ");
        for (int j = 0; j < 12; j++)
        {
            if (j == 6) {
                _cprintf("|||  ");
            }
            const char* out1 = (j % 2 == 0) ? ".." : "/\\";//output
            const char* out2 = (game->matrix[i + 6][j] == 0) ? out1 : "[]";
            const char* space = (j == 11) ? "  " : "   ";
            if (game->matrix[i + 6][j] == 1) {
                PrintColorText(hConsole, out2, WHITE, WHITE);
            }
            else if (game->matrix[i + 6][j] == 2) {
                PrintColorText(hConsole, out2, DARKGRAY, DARKGRAY);
            }
            else {
                _cprintf(out2);
            }
            _cprintf(space);
        }
        _cprintf("::");
    }
}
void DrawCenterVerticalLine(HANDLE hConsole, short x, short y) {
    for (int i = 0; i < 3; i++) {
        SetConsoleCursorPosition(hConsole, COORD{ short(x), short(y + 7 + i) });
        _cprintf("::");
        SetConsoleCursorPosition(hConsole, COORD{ short(x + 67), short(y + 7 + i) });
        _cprintf("::");
    }
}
void DrawCatchPaws(HANDLE hConsole, Game* game) {
    short new_x = (6 * 2 + 6 * 3) + 4;

    if (game->catchB != 0) {
        SetConsoleCursorPosition(hConsole, COORD{ short(game->x + new_x - 12), short(game->y + 8) });
        _cprintf("B: %d", game->catchB);
    }
    if (game->catchW != 0) {
        SetConsoleCursorPosition(hConsole, COORD{ short(game->x + new_x - 3 + 12), short(game->y + 8) });
        _cprintf("W: %d", game->catchW);
    }

}
void DrawBarWord(HANDLE hConsole, Game* game) {
    _cprintf("[");
    PrintColorText(hConsole, "B", MAGENTA, BLACK);
    _cprintf("AR]");
}
void DrawHome(HANDLE hConsole, Game* game) {
    _cprintf("[");
    PrintColorText(hConsole, "H", MAGENTA, BLACK);
    _cprintf("OME]");
}
void DrawCenter(HANDLE hConsole, Game* game) {
    short new_x = (6 * 2 + 6 * 3) + 4;
    SetConsoleCursorPosition(hConsole, COORD{ short(game->x + new_x - 1), short(game->y + 7) });
    _cprintf("|||");
    SetConsoleCursorPosition(hConsole, COORD{ short(game->x + new_x - 2), short(game->y + 8) });
    DrawBarWord(hConsole, game);
    SetConsoleCursorPosition(hConsole, COORD{ short(game->x + new_x - 1), short(game->y + 9) });
    _cprintf("|||");
    SetConsoleCursorPosition(hConsole, COORD{ short(game->x + 70), short(game->y + 8) });
    DrawHome(hConsole, game);

    DrawCenterVerticalLine(hConsole, game->x, game->y);
    DrawCatchPaws(hConsole, game);
}
void DrawFullHorizontalLine(HANDLE hConsole, short x, short y) {
    int lineLength = (6 * 2 + 6 * 3) + 4;
    SetConsoleCursorPosition(hConsole, COORD{ short(x), short(y) });
    _cprintf(":");
    DrawHorizontalLine(hConsole, x + 1, y, lineLength - 1);
    _cprintf("0");
    DrawHorizontalLine(hConsole, x + lineLength + 1, y, lineLength - 1);
    _cprintf(":");

}
void DrawGameBoard(HANDLE hConsole, Game* game) {

    DrawFullHorizontalLine(hConsole, game->x, game->y);
    DrawUpVerticalLine(hConsole, game, game->y + 1);
    DrawCenter(hConsole, game);
    DrawDownVerticalLine(hConsole, game, game->y + 1 + 6 + 3);
    DrawFullHorizontalLine(hConsole, game->x, game->y + 16);
}
void DrawUPNumberLabels(HANDLE hConsole, short x, short y) {
    x += 3;
    SetConsoleCursorPosition(hConsole, COORD{ short(x), short(y - 1) });
    setConsoleColor(hConsole, MAGENTA, BLACK);
    for (int i = 13; i <= 24; i++) {
        if (i == 18) {
            x += 5;
        }
        _cprintf("%d", i);
        x += 5;
        SetConsoleCursorPosition(hConsole, COORD{ x,short(y - 1) });
    }
    ResetColor(hConsole);
}
void DrawDOWNNumberLabels(HANDLE hConsole, short x, short y) {
    x += 3;
    SetConsoleCursorPosition(hConsole, COORD{ short(x), short(y + 17) });
    setConsoleColor(hConsole, MAGENTA, BLACK);
    for (int i = 12; i >= 1; i--) {
        if (i == 7) {
            x += 5;
        }
        _cprintf("%d", i);
        x += 5;
        SetConsoleCursorPosition(hConsole, COORD{ x,short(y + 17) });
    }
    ResetColor(hConsole);
}
void DrawMoveType(HANDLE h, Game* game) {
    int y = 12;
    SetConsoleCursorPosition(h, COORD{ 7,short(y) });
    _cprintf("User color move:");
    SetConsoleCursorPosition(h, COORD{ 7,short(y + 1) });
    _cprintf("+----+");
    SetConsoleCursorPosition(h, COORD{ 7,short(y + 2) });
    _cprintf("|");
    PrintColorText(h, "    ", BLACK, (game->isWhiteMove) ? 15 : 0);
    ResetColor(h);
    _cprintf("|");
    SetConsoleCursorPosition(h, COORD{ 7,short(y + 3) });
    _cprintf("+----+");
}
void DrawUserScore(HANDLE h, Game* game) {
    if (game->firstmove1 == game->firstmove2) {
        SetConsoleCursorPosition(h, COORD{ short(game->x),short(game->y + 20) });
        _cprintf("   roll the dice one more time");

    }
    else {
        SetConsoleCursorPosition(h, COORD{ short(game->x - 10), short(game->y + 20) });
        printf("Player1: %d ", game->firstmove1);
        printf("Player2: %d", game->firstmove2);
    }
}
void DrawCubs(HANDLE h, Game* game) {
    SetConsoleCursorPosition(h, COORD{ short(game->x), short(game->y + 20) });
    printf("Cub1: %d ", game->cub1);
    printf("Cub2: %d", game->cub2);
    printf(" => TOTAL: %d", game->countOfUserMoves);
}
void DrawBadMoveError(HANDLE h, Game* game) {
    if (game->badMove == true) {
        int back = 12;
        SetConsoleCursorPosition(h, COORD{ short(game->x), short(game->y + 18) });
        SetConsoleTextAttribute(h, back << 4 | 7);
        _cprintf("You can not make a move");
        SetConsoleTextAttribute(h, 0 << 4 | 7);
    }
}
void DrawWinnerCheck(HANDLE h, Game* game) {
    if (FullHomeforWHITE(game)) {
        SetConsoleCursorPosition(h, COORD{ 0, 5 });
        _cprintf("White won");

    }
    else if (FullHomeforBLACK(game)) {
        SetConsoleCursorPosition(h, COORD{ 5, 5 });
        _cprintf("Black won");
    }
}
void Draw(HANDLE h, Game* game) {
    system("cls");
    SetConsoleCursorPosition(h, COORD{ game->x, 0 });
    _cprintf("The Peelgrunt Game of GAMMON IV.20");
    Score(h, game);
    DrawGameBoard(h, game);
    DrawDOWNNumberLabels(h, game->x, game->y);
    DrawUPNumberLabels(h, game->x, game->y);

    DrawWinnerCheck(h, game);

    SetConsoleCursorPosition(h, COORD{ 0, 5 });
    _cprintf("Select:\nChoose Pawn Move = C\nQuit = Q\nGenerate numbers = S\nSee who goes first = F");
    DrawMoveType(h, game);
    if (game->playerCheck == true) {
        DrawUserScore(h, game);
    }
    else {
        DrawCubs(h, game);
    }

    DrawBadMoveError(h, game);
}
/////////////////////////////////////////////


void Start(Game* game, HANDLE h) {
    char ch = '\0';
    _cprintf("Press P to start");
    if (ch == 'P' || ch == 'p') {
        Draw(h, game);

    }
    if (IsFileExist(game->SaveFile) == true) {
        Load(game);
    }
    else {
        MatrixClear(game);
    }
}
void MainLoop(HANDLE h, Game* game) {
    char ch = '\0';
    while (ch != 'q' && ch != 'Q') {
        ch = _getwch();
        if (ch == 'F' || ch == 'f') {
            GenerateFirstMove(h, game);

        }
        if (ch == 's' || ch == 'S') {
            GenerateNumbers(h, game);
            game->playerCheck = false;
        }

        if (ch == 'C' || ch == 'c') {
            SelectPawn(h, game);
        }
        if (ch == 'W' || ch == 'w') {
            game->isWhiteMove = !game->isWhiteMove;
            game->cub1 = game->cub2 = 0;
        }

        Draw(h, game);
        game->badMove = false;

    }
}

int main() {
    SetConsoleTitleA("Volkova Mariia, 196660");

    Game game;
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

    Start(&game, h);

    Draw(h, &game);
    MainLoop(h, &game);

    Saving(&game);
    return 0;
}