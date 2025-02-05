#include <SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define SIZE 3
#define CELL_SIZE 150
#define SUB_CELL_SIZE 50

typedef struct {
    char board[SIZE][SIZE];
    char winner;
} SubGame;

typedef enum {
    MAIN_BOARD_VIEW,
    SUB_GAME_VIEW
} GameView;


SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* font = NULL;
GameView currentView = MAIN_BOARD_VIEW;
int selectedMainRow = -1, selectedMainCol = -1;

SubGame mainBoard[SIZE][SIZE];
int currentPlayer = 1;
int gameMode = 0; // 0 = PvP, 1 = PvE

bool playing = true;

// Function Declarations
void initializeSDL();
void closeSDL();
void renderMainBoard();
void renderSubGame(SubGame* sub);
void handleMainBoardClick(int x, int y);
int checkSubWin(char board[SIZE][SIZE]);
int checkSubDraw(char board[SIZE][SIZE]);
int checkMainWin(char symbol);
int checkMainDraw();
void SDL_RenderDrawCircle(SDL_Renderer* renderer, int x, int y, int radius);
void playSubGame(SubGame* sub);
void aiMainMove();
void aiSubMove(SubGame* sub);
void renderDifficultyMenu();
void renderMainMenu();


// Initialize SDL and related subsystems
void initializeSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! Error: %s\n", SDL_GetError());
        exit(1);
    }

    window = SDL_CreateWindow("Hierarchical Tic Tac Toe",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! Error: %s\n", TTF_GetError());
        exit(1);
    }
    font = TTF_OpenFont("arial.ttf", 24); // Update the font path as needed
    if (!font) {
        printf("Failed to load font! Error: %s\n", TTF_GetError());
    }
}

// Clean up SDL resources
void closeSDL() {
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

// Draw a circle using SDL
void SDL_RenderDrawCircle(SDL_Renderer* renderer, int x, int y, int radius) {
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;
            int dy = radius - h;
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
    }
}

// Render the main game board
void renderMainBoard() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (int i = 1; i < SIZE; i++) {
        SDL_RenderDrawLine(renderer, i * CELL_SIZE, 0, i * CELL_SIZE, CELL_SIZE * SIZE);
        SDL_RenderDrawLine(renderer, 0, i * CELL_SIZE, CELL_SIZE * SIZE, i * CELL_SIZE);
    }

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            SDL_Rect cell = { j * CELL_SIZE, i * CELL_SIZE, CELL_SIZE, CELL_SIZE };
            if (mainBoard[i][j].winner == 'X') {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                SDL_RenderDrawLine(renderer, cell.x + 10, cell.y + 10, cell.x + cell.w - 10, cell.y + cell.h - 10);
                SDL_RenderDrawLine(renderer, cell.x + cell.w - 10, cell.y + 10, cell.x + 10, cell.y + cell.h - 10);
            }
            else if (mainBoard[i][j].winner == 'O') {
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                SDL_RenderDrawCircle(renderer, cell.x + CELL_SIZE / 2, cell.y + CELL_SIZE / 2, CELL_SIZE / 2 - 10);
            }
            else if (mainBoard[i][j].winner == 'D') {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                SDL_RenderDrawLine(renderer, cell.x + 10, cell.y + 10, cell.x + cell.w - 10, cell.y + cell.h - 10);
                SDL_RenderDrawLine(renderer, cell.x + cell.w - 10, cell.y + 10, cell.x + 10, cell.y + cell.h - 10);
            }
        }
    }

    SDL_RenderPresent(renderer);
}

// Render a sub-game board
void renderSubGame(SubGame* sub) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    int offsetX = (WINDOW_WIDTH - SUB_CELL_SIZE * SIZE) / 2;
    int offsetY = (WINDOW_HEIGHT - SUB_CELL_SIZE * SIZE) / 2;

    for (int i = 1; i < SIZE; i++) {
        SDL_RenderDrawLine(renderer, offsetX + i * SUB_CELL_SIZE, offsetY,
            offsetX + i * SUB_CELL_SIZE, offsetY + SUB_CELL_SIZE * SIZE);
        SDL_RenderDrawLine(renderer, offsetX, offsetY + i * SUB_CELL_SIZE,
            offsetX + SUB_CELL_SIZE * SIZE, offsetY + i * SUB_CELL_SIZE);
    }

    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            SDL_Rect cell = {
                offsetX + j * SUB_CELL_SIZE,
                offsetY + i * SUB_CELL_SIZE,
                SUB_CELL_SIZE,
                SUB_CELL_SIZE
            };

            if (sub->board[i][j] == 'X') {
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                SDL_RenderDrawLine(renderer, cell.x + 10, cell.y + 10, cell.x + cell.w - 10, cell.y + cell.h - 10);
                SDL_RenderDrawLine(renderer, cell.x + cell.w - 10, cell.y + 10, cell.x + 10, cell.y + cell.h - 10);
            }
            else if (sub->board[i][j] == 'O') {
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                SDL_RenderDrawCircle(renderer,
                    cell.x + SUB_CELL_SIZE / 2,
                    cell.y + SUB_CELL_SIZE / 2,
                    SUB_CELL_SIZE / 2 - 10);
            }
        }
    }

    SDL_RenderPresent(renderer);
}

// Handle clicks on the main board
void handleMainBoardClick(int x, int y) {
    int col = x / CELL_SIZE;
    int row = y / CELL_SIZE;

    if (row >= 0 && row < SIZE && col >= 0 && col < SIZE) {
        if (mainBoard[row][col].winner == ' ') {
            selectedMainRow = row;
            selectedMainCol = col;
            currentView = SUB_GAME_VIEW;
            playSubGame(&mainBoard[selectedMainRow][selectedMainCol]);
        }
    }
}

// Play a sub-game
void playSubGame(SubGame* sub) {
    if (sub->winner != ' ') return;

    SDL_Event e;
    int quitSubGame = 0;

    while (!quitSubGame) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                closeSDL();
                exit(0);
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);

                int offsetX = (WINDOW_WIDTH - SUB_CELL_SIZE * SIZE) / 2;
                int offsetY = (WINDOW_HEIGHT - SUB_CELL_SIZE * SIZE) / 2;

                x -= offsetX;
                y -= offsetY;

                int col = x / SUB_CELL_SIZE;
                int row = y / SUB_CELL_SIZE;

                if (row >= 0 && row < SIZE && col >= 0 && col < SIZE) {
                    if (sub->board[row][col] == ' ') {
                        sub->board[row][col] = (currentPlayer == 1) ? 'X' : 'O';

                        if (checkSubWin(sub->board)) {
                            sub->winner = sub->board[row][col];
                            mainBoard[selectedMainRow][selectedMainCol].winner = sub->winner;
                            quitSubGame = 1;
                        }
                        else if (checkSubDraw(sub->board)) {
                            sub->winner = 'D';
                            quitSubGame = 1;
                        }
                        else {
                            currentPlayer = (currentPlayer == 1) ? 2 : 1;
                            if (gameMode == 1 && currentPlayer == 2) {
                                aiSubMove(sub);
                                if (checkSubWin(sub->board)) {
                                    sub->winner = 'O';
                                    mainBoard[selectedMainRow][selectedMainCol].winner = 'O';
                                }
                                else if (checkSubDraw(sub->board)) {
                                    sub->winner = 'D';
                                }
                                else {
                                    currentPlayer = 1;
                                }
                            }
                        }
                    }
                }
            }
        }
        renderSubGame(sub);
    }
    currentView = MAIN_BOARD_VIEW;
}

// AI makes a move on the main board
void aiMainMove() {
    int row, col;
    do {
        row = rand() % SIZE;
        col = rand() % SIZE;
    } while (mainBoard[row][col].winner != ' ');
    selectedMainRow = row;
    selectedMainCol = col;
}

// AI makes a move on a sub-game board
void aiSubMove(SubGame* sub) {
    int row, col;
    do {
        row = rand() % SIZE;
        col = rand() % SIZE;
    } while (sub->board[row][col] != ' ');
    sub->board[row][col] = 'O';
}


// Render the difficulty selection menu
void renderDifficultyMenu() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_Color textColor;
    textColor.r = 0;
    textColor.g = 0;
    textColor.b = 0;
    textColor.a = 255;

    SDL_Surface* textSurface;
    SDL_Texture* textTexture;

    textSurface = TTF_RenderText_Solid(font, "Select Difficulty:", textColor);
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_Rect textRect = { 300, 100, textSurface->w, textSurface->h };
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);

    const char* difficulties[] = { "1. Easy", "2. Medium", "3. Hard" };
    for (int i = 0; i < 3; i++) {
        textSurface = TTF_RenderText_Solid(font, difficulties[i], textColor);
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect modeRect = { 300, 200 + i * 50, textSurface->w, textSurface->h };
        SDL_RenderCopy(renderer, textTexture, NULL, &modeRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }

    SDL_RenderPresent(renderer);
}

// Render the main menu for game mode selection
void renderMainMenu() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_Color textColor = { 0, 0, 0, 255 };
    SDL_Surface* textSurface;
    SDL_Texture* textTexture;

    // Title
    textSurface = TTF_RenderText_Solid(font, "Select Game Mode:", textColor);
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = { 300, 100, textSurface->w, textSurface->h };
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);

    // Options
    const char* modes[] = { "1. Player vs Player", "2. Player vs Computer" };
    for (int i = 0; i < 2; i++) {
        textSurface = TTF_RenderText_Solid(font, modes[i], textColor);
        textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect modeRect = { 300, 200 + i * 50, textSurface->w, textSurface->h };
        SDL_RenderCopy(renderer, textTexture, NULL, &modeRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }

    SDL_RenderPresent(renderer);
}

// Check if a sub-game has been won
int checkSubWin(char board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        if (board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][1] == board[i][2])
            return 1;
        if (board[0][i] != ' ' && board[0][i] == board[1][i] && board[1][i] == board[2][i])
            return 1;
    }
    if (board[0][0] != ' ' && board[0][0] == board[1][1] && board[1][1] == board[2][2])
        return 1;
    if (board[0][2] != ' ' && board[0][2] == board[1][1] && board[1][1] == board[2][0])
        return 1;
    return 0;
}

// Check if a sub-game is a draw
int checkSubDraw(char board[SIZE][SIZE]) {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            if (board[i][j] == ' ') return 0;
    return 1;
}

// Check if the main game has been won
int checkMainWin(char symbol) {
    for (int i = 0; i < SIZE; i++) {
        if (mainBoard[i][0].winner == symbol &&
            mainBoard[i][1].winner == symbol &&
            mainBoard[i][2].winner == symbol)
            return 1;
        if (mainBoard[0][i].winner == symbol &&
            mainBoard[1][i].winner == symbol &&
            mainBoard[2][i].winner == symbol)
            return 1;
    }
    if (mainBoard[0][0].winner == symbol &&
        mainBoard[1][1].winner == symbol &&
        mainBoard[2][2].winner == symbol)
        return 1;
    if (mainBoard[0][2].winner == symbol &&
        mainBoard[1][1].winner == symbol &&
        mainBoard[2][0].winner == symbol)
        return 1;
    return 0;
}

// Check if the main game is a draw
int checkMainDraw() {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            if (mainBoard[i][j].winner == ' ') return 0;
    return 1;
}

// Main function
int main(int argc, char* argv[]) {
    while (playing)
    {
        srand(time(0));
        initializeSDL();

        // Main menu selection
        int gameModeSelected = 0;
        SDL_Event e;
        while (!gameModeSelected) {
            renderMainMenu();
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) {
                    closeSDL();
                    return 0;
                }
                else if (e.type == SDL_KEYDOWN) {
                    if (e.key.keysym.sym == SDLK_1) {
                        gameMode = 0; // PvP
                        gameModeSelected = 1;
                    }
                    else if (e.key.keysym.sym == SDLK_2) {
                        gameMode = 1; // PvE
                        gameModeSelected = 1;
                    }
                }
            }
        }


        // Initialize the main board
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                memset(&mainBoard[i][j].board, ' ', sizeof(mainBoard[i][j].board));
                mainBoard[i][j].winner = ' ';
            }
        }
        currentPlayer = 1;

        int quit = 0;
        while (!quit) {
            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) {
                    quit = 1;
                }
                else if (e.type == SDL_MOUSEBUTTONDOWN && currentView == MAIN_BOARD_VIEW) {
                    int x, y;
                    SDL_GetMouseState(&x, &y);
                    handleMainBoardClick(x, y);
                }
            }

            if (currentView == MAIN_BOARD_VIEW) {
                renderMainBoard();

                if (gameMode == 1 && currentPlayer == 2) {
                    aiMainMove();
                    playSubGame(&mainBoard[selectedMainRow][selectedMainCol]);
                }

                if (checkMainWin('X')) {
                    printf("Player 1 (X) wins!\n");
                    quit = 1;
                }
                else if (checkMainWin('O')) {
                    printf("Computer (O) wins!\n");
                    quit = 1;
                }
                else if (checkMainDraw()) {
                    printf("Game draw!\n");
                    quit = 1;
                }
            }
        }

        closeSDL();
    }

    return 0;
}