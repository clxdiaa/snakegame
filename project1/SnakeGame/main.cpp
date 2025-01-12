#include "ScoreManager.h"
#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <cstdio>
#include "resources.h"
#include <tchar.h>
#include <fstream>
// function to load string from resource
void LoadStringFromResource(int resourceId, TCHAR* buffer, int bufferSize) {
    LoadString(GetModuleHandle(NULL), resourceId, buffer, bufferSize);
}

// main definitions
#define WINDOW_TITLE "Snake Game"
#define TIMER_ID 1
#define TIMER_INTERVAL 100 // time to recharge game
#define SQUARE_SIZE 20    // snake dimension
#define GRID_WIDTH 35
#define GRID_HEIGHT 26
#define BUTTON_START_ID 1001 // ID button "Start"
#define BUTTON_RESTART_ID 1002 // ID button "Restart"
#define GRID_OFFSET_Y 2
#define GRID_OFFSET_X 0.5
HINSTANCE hDll = NULL;



int (*GetScore)(int) = NULL;
// snke structure
typedef struct {
    POINT body[100]; // Maximum of 100 segments
    int length;      // snake length
    int direction;   // snake direction
} Snake;

// SFood structure
typedef struct {
    int x;
    int y;
} Food;

Snake snake; // snake global variable
Food food;   // Food global variable
int score=0;
int elapsedTime = 0; // time elapsed
bool gameOver = false; // Flag to terminate game
bool scoreSaved = false; // Flag to avoid multiple score saving
// declaration of window procedure
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// initialization of snake at game start
void InitializeSnake() {
    snake.length = 5; // initial length
    snake.direction = 1; // initial moving direction is right
    for (int i = 0; i < snake.length; i++) {
        snake.body[i].x = 5 - i;
        snake.body[i].y = 5;
    }
}

// function to random generate food not on the snake
bool IsFoodOnSnake(int x, int y, Snake snake) {
    for (int i = 0; i < snake.length; i++) {
        if (snake.body[i].x == x && snake.body[i].y == y) {
            return true; // food is on snake segment
        }
    }
    return false; //food isn't on sake segment
}

// function to generate food
void SpawnFood() {
    do {
        food.x = rand() % GRID_WIDTH;  // random x coordinate
        food.y = rand() % GRID_HEIGHT; // random y coordinate
    } while (IsFoodOnSnake(food.x, food.y, snake)); //ensures that food is not on snake segment
}
// checks if snake has eaten on food
bool HasEatenFood() {
    return snake.body[0].x == food.x && snake.body[0].y == food.y;
}

//checking is game is finished
bool IsGameOver() {
    // checks borders of grid considering offset
    if (snake.body[0].x < 0 || snake.body[0].x >= GRID_WIDTH ||
        snake.body[0].y < 0 || snake.body[0].y >= GRID_HEIGHT) {
        return true;
    }

    //check if snake goes iinto himself
    for (int i = 1; i < snake.length; i++) {
        if (snake.body[0].x == snake.body[i].x &&
            snake.body[0].y == snake.body[i].y) {
            return true;
        }
    }

    return false;
}
// update snake position
void UpdateSnake(HWND hWnd) {
    // moves each segment to the next position
    for (int i = snake.length - 1; i > 0; i--) {
        snake.body[i] = snake.body[i - 1];
    }

    //updates head based on direction
    switch (snake.direction) {
        case 0: snake.body[0].y -= 1; break; // up
        case 1: snake.body[0].x += 1; break; // right
        case 2: snake.body[0].y += 1; break; // down
        case 3: snake.body[0].x -= 1; break; // left
    }

    // check if he has eaten food
    if (HasEatenFood()) {
        snake.length++; // increment snake length
        score += 10;    // Increment score
        SpawnFood();    // Generate new food
    }


    // Check if game is finished
    if (IsGameOver()) {
        gameOver = true;
    }

    // increment time elapsed
    if (!gameOver) {
        elapsedTime++;
    }
}
//function to draw snake
void DrawSnake(HDC hdc) {
    // body as rounded rectangles
    for (int i = 0; i < snake.length; i++) {
        //light green for body
        HBRUSH brush;

        //dark green for head
        if (i == 0) {
            brush = CreateSolidBrush(RGB(0, 128, 0));
        } else {
            brush = CreateSolidBrush(RGB(0, 255, 0));
        }

        //brush selection
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

        // define starting position
        RECT rect = {
            snake.body[i].x * SQUARE_SIZE + 20,
            (snake.body[i].y + GRID_OFFSET_Y) * SQUARE_SIZE + 20,
            (snake.body[i].x + 1) * SQUARE_SIZE + 20,
            (snake.body[i].y + GRID_OFFSET_Y + 1) * SQUARE_SIZE + 20
        };


        RoundRect(hdc, rect.left, rect.top, rect.right, rect.bottom, 10, 10);


        SelectObject(hdc, oldBrush);

        // release object after use
        DeleteObject(brush);
    }
}



// Function to draw food as red circles
void DrawFood(HDC hdc) {
    HBRUSH brush = CreateSolidBrush(RGB(255, 0, 0));
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);

    // calculate coordinates of food
    int left = food.x * SQUARE_SIZE + 20;
    int top = (food.y + GRID_OFFSET_Y) * SQUARE_SIZE + 20;
    int right = (food.x + 1) * SQUARE_SIZE + 20;
    int bottom = (food.y + GRID_OFFSET_Y + 1) * SQUARE_SIZE + 20;

    //drawing the circle
    Ellipse(hdc, left, top, right, bottom);

    SelectObject(hdc, oldBrush);
    DeleteObject(brush); //feeing the brush
}

// function to reset game
void RestartGame(HWND hWnd) {
    // Reset game state
    gameOver = false;
    score=0;
    elapsedTime = 0; // Reset of timer
    InitializeSnake();
    SpawnFood();
    scoreSaved = false;  // Reset of flag for score saving

    // redrawing of window is forced
    InvalidateRect(hWnd, NULL, TRUE);
    SetTimer(hWnd, TIMER_ID, TIMER_INTERVAL, NULL);  // Reset of timer

    // restart button is hidden
    ShowWindow(GetDlgItem(hWnd, BUTTON_RESTART_ID), SW_HIDE);

    //show start button and hide it when game is over
    ShowWindow(GetDlgItem(hWnd, BUTTON_START_ID), SW_HIDE);

    //showing game element
    ShowWindow(GetDlgItem(hWnd, 1), SW_SHOW);  // Mostra il serpente
    ShowWindow(GetDlgItem(hWnd, 2), SW_SHOW);  // Mostra il cibo
}

// function to start game
void StartGame(HWND hWnd) {
    // Hide start button
    ShowWindow(GetDlgItem(hWnd, BUTTON_START_ID), SW_HIDE);

    //game start
    InitializeSnake();
    SpawnFood();
    SetTimer(hWnd, TIMER_ID, TIMER_INTERVAL, NULL);  // start timer
    // show game element
    ShowWindow(GetDlgItem(hWnd, 1), SW_SHOW);
    ShowWindow(GetDlgItem(hWnd, 2), SW_SHOW);
}
//funcntion to save score on file
void SaveScore(int score) {
    HANDLE hFile = CreateFile("scores.txt", FILE_APPEND_DATA, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return;

    // score converted to string
    char scoreText[20];
    sprintf(scoreText, "%d\n", score);

    //score written as string
    DWORD bytesWritten;
    WriteFile(hFile, scoreText, strlen(scoreText), &bytesWritten, NULL);

    CloseHandle(hFile);
}

//function to read score
int LoadScores(int scores[], int maxScores) {
    HANDLE hFile = CreateFile("scores.txt", GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) return 0;

    char buffer[1024] = {0};
    DWORD bytesRead;
    if (!ReadFile(hFile, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
        CloseHandle(hFile);
        return 0;
    }

    CloseHandle(hFile);

    // Parse the scores from the buffer
    int conto = 0;
    char* line = strtok(buffer, "\n");
    while (line != NULL && conto < maxScores) {
        scores[conto++] = atoi(line); // Convert string in integer
        line = strtok(NULL, "\n");
    }

    return conto;
}
//function to sort scores
void SortScores(int scores[], int conto) {
    for (int i = 0; i < conto - 1; i++) {
        for (int j = i + 1; j < conto; j++) {
            if (scores[j] > scores[i]) {
                int temp = scores[i];
                scores[i] = scores[j];
                scores[j] = temp;
            }
        }
    }
}

// main
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    HINSTANCE hDll = LoadLibrary("ScoreManager.dll");
    if (!hDll) {
        MessageBox(NULL, "Impossible to load ScoreManager.dll", "Error", MB_ICONERROR);
        return 0;
    }

    //get popinters to function
    GetScore = (int (*)(int))GetProcAddress(hDll, "GetScore");

    if (!GetScore) {
        MessageBox(NULL, "Impossible to find GetScore in DLL", "Error", MB_ICONERROR);
        FreeLibrary(hDll);
        return 0;
    }
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "SnakeGameClass";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClass(&wc)) {
        MessageBox(NULL, "Error during window upload!", "Error", MB_ICONERROR);
        return 0;
    }

    //window has fixed dimension
    HWND hWnd = CreateWindow("SnakeGameClass", WINDOW_TITLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                             CW_USEDEFAULT, CW_USEDEFAULT, GRID_WIDTH * SQUARE_SIZE + 40, GRID_HEIGHT * SQUARE_SIZE + 120,
                             NULL, NULL, hInstance, NULL);

    if (!hWnd) {
        MessageBox(NULL, "Error during window creation!", "Error", MB_ICONERROR);
        return 0;
    }

    // Creates start button on top of the grid
    int startButtonWidth = 100;
    int startButtonHeight = 30;
    TCHAR buttonText[100];

// charge and upload button text from string table
LoadStringFromResource(IDS_START_GAME, buttonText, 100);
CreateWindow("BUTTON", buttonText, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
             (GRID_WIDTH * SQUARE_SIZE - startButtonWidth) / 2 + 20, 10, startButtonWidth, startButtonHeight,
             hWnd, (HMENU)BUTTON_START_ID, hInstance, NULL);


// New button dimensions
int restartButtonWidth = 100;
int restartButtonHeight = 30;

//new button position
int restartButtonY = 320;
int restartButtonX = (GRID_WIDTH * SQUARE_SIZE - restartButtonWidth) / 2 + 20;

// restart button created
CreateWindow("BUTTON", "Restart Game", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
             restartButtonX, restartButtonY, restartButtonWidth, restartButtonHeight, hWnd,
             (HMENU)BUTTON_RESTART_ID, hInstance, NULL);

// initially button is hidden
ShowWindow(GetDlgItem(hWnd, BUTTON_RESTART_ID), SW_HIDE);

    srand((unsigned int)time(NULL)); //random number generator
    InitializeSnake();
    SpawnFood();


    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}

// managing window messages
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_KEYDOWN:
            if (gameOver) return 0;
            switch (wParam) {
                case VK_UP:
                    if (snake.direction != 2) snake.direction = 0; break; // up
                case VK_RIGHT:
                    if (snake.direction != 3) snake.direction = 1; break; // right
                case VK_DOWN:
                    if (snake.direction != 0) snake.direction = 2; break; // down
                case VK_LEFT:
                    if (snake.direction != 1) snake.direction = 3; break; // left
            }
            break;

        case WM_TIMER:
            if (wParam == TIMER_ID) {
                UpdateSnake(hWnd);
                if (gameOver) {
                    KillTimer(hWnd, TIMER_ID); // if game ends timer stops
                }
                InvalidateRect(hWnd, NULL, TRUE); // window redrawing
            }
            break;

        case WM_COMMAND:
            if (LOWORD(wParam) == BUTTON_START_ID) {
                StartGame(hWnd);
            }
            if (LOWORD(wParam) == BUTTON_RESTART_ID) {
                RestartGame(hWnd);
            }
            break;

case WM_PAINT: {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    HBRUSH blueBrush = CreateSolidBrush(RGB(70, 111, 155));
    RECT gridRect = {20, (GRID_OFFSET_Y) * SQUARE_SIZE + 20, GRID_WIDTH * SQUARE_SIZE + 20, (GRID_HEIGHT + GRID_OFFSET_Y) * SQUARE_SIZE + 20};
    FillRect(hdc, &gridRect, blueBrush);
    DeleteObject(blueBrush);
    TCHAR textBuffer[100];
    // upload background only if game is finished
    if (gameOver) {
            if (!scoreSaved) {
        // Saves final score
        SaveScore(score);
        scoreSaved = true;  //flag for saving scores
    }

        HBITMAP hBitmap = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BACKGROUND));
        if (hBitmap) {
            HDC hdcMem = CreateCompatibleDC(hdc);
            SelectObject(hdcMem, hBitmap);

            //gets window dimension
            RECT clientRect;
            GetClientRect(hWnd, &clientRect);

            // gets bitmap dimension
            BITMAP bitmap;
            GetObject(hBitmap, sizeof(BITMAP), &bitmap);

            // bitmap fills window
            StretchBlt(hdc,
                       0, 0,
                       clientRect.right, clientRect.bottom,
                       hdcMem,
                       0, 0,
                       bitmap.bmWidth, bitmap.bmHeight,
                       SRCCOPY);

            DeleteDC(hdcMem);
            DeleteObject(hBitmap);
        }

        //Showing strings
        LoadStringFromResource(IDS_GAME_OVER, textBuffer, 100);
        TextOut(hdc, 200, 200, textBuffer, lstrlen(textBuffer));

        char scoreText[50];
        sprintf(scoreText, "Final Score: %d", GetScore(score));
        TextOut(hdc, 200, 250, scoreText, strlen(scoreText));

        char timeText[50];
        sprintf(timeText, "Final Time: %d", elapsedTime);
        TextOut(hdc, 200, 270, timeText, strlen(timeText));

        // loads scores on file
        int scores[150]; // maximum of 150 scores
        int scoreCount = LoadScores(scores, 150);

        // sort scores
        SortScores(scores, scoreCount);

        // shows best 3 scores
        char highScores[150];
        sprintf(highScores, "Top Scores:\n1: %d \n\n2: %d \n\n3: %d",
                scores[0],
                (scoreCount > 1 ? scores[1] : 0),
                (scoreCount > 2 ? scores[2] : 0));
        TextOut(hdc, 200, 300, highScores, strlen(highScores));

        // show restart button
        ShowWindow(GetDlgItem(hWnd, BUTTON_RESTART_ID), SW_SHOW);
    } else {
        //draw elements of game
        DrawSnake(hdc);
        DrawFood(hdc);

        // showing window elements
        char scoreText[50];
        sprintf(scoreText, "Score: %d", GetScore(score));
        TextOut(hdc, 10, 10, scoreText, strlen(scoreText));

        char timeText[50];
        sprintf(timeText, "Time: %d", elapsedTime);
        TextOut(hdc, 10, 30, timeText, strlen(timeText));
    }

    EndPaint(hWnd, &ps);
    break;
}


        case WM_DESTROY:
            if (hDll) FreeLibrary(hDll);
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
