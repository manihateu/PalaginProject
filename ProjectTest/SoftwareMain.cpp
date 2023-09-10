#include <windows.h>
#include <iostream>
#define IDC_CHECKBOX 1001 // ���������� ������������� ��� ��������

const int BOARD_SIZE = 5;
HWND hwndButton[BOARD_SIZE][BOARD_SIZE];
HWND hwndCheckbox;
char board[BOARD_SIZE][BOARD_SIZE] = { 0 };
bool playerTurn = true; // Flag indicating player's turn
bool againstComputer = true; // Flag indicating the game mode (against computer or player)


bool CheckWin(char player) {
    // �������� ������� ���������� ���������� ��� ���������� ������

    // �������� �� �����������
    for (int row = 0; row < BOARD_SIZE; row++) {
        if (board[row][0] == player && board[row][1] == player && board[row][2] == player && board[row][3] == player && board[row][4] == player)
            return true;
    }

    // �������� �� ���������
    for (int col = 0; col < BOARD_SIZE; col++) {
        if (board[0][col] == player && board[1][col] == player && board[2][col] == player && board[3][col] == player && board[4][col] == player)
            return true;
    }

    // �������� �� ��������� (����� �������)
    if (board[0][0] == player && board[1][1] == player && board[2][2] == player && board[3][3] == player && board[4][4] == player)
        return true;

    // �������� �� ��������� (������ ������)
    if (board[0][4] == player && board[1][3] == player && board[2][2] == player && board[3][1] == player && board[4][0] == player)
        return true;

    return false;
}


bool CheckDraw() {
    // �������� �� �����

    // �������� �� ������� ������ ������
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            if (board[row][col] == 0)
                return false; // ���� ������ ������, ���� ������������
        }
    }
    return true; // ��� ������ ���������, �����
}

void MakeComputerMove() {
    // Computer's move
    int bestScore = INT_MIN;
    int bestRow = -1;
    int bestCol = -1;

    // ���������� ��� ������ ������ �� �����
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            if (board[row][col] == 0) {
                // ���������, ���� ������� ��� �������� � ������ ����������
                board[row][col] = 'O';
                if (CheckWin('O')) {
                    board[row][col] = 0;  // �������� ��������� ���
                    bestRow = row;
                    bestCol = col;
                    break;
                }
                board[row][col] = 0;  // �������� ��������� ���

                // ���������, ���� ������� ��� �������� � ������ ������
                board[row][col] = 'X';
                if (CheckWin('X')) {
                    board[row][col] = 0;  // �������� ��������� ���
                    bestRow = row;
                    bestCol = col;
                    break;
                }
                board[row][col] = 0;  // �������� ��������� ���

                // ���������� �������, ���� ��� �� �������� �� � ������, �� � ���������
                bestRow = row;
                bestCol = col;
            }
        }
    }

    // ��������� ��� ����������
    board[bestRow][bestCol] = 'O';
    SetWindowText(hwndButton[bestRow][bestCol], L"O");
    EnableWindow(hwndButton[bestRow][bestCol], FALSE);
}

void ResetGame() {
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            board[row][col] = 0;
            SetWindowText(hwndButton[row][col], L"");
            EnableWindow(hwndButton[row][col], TRUE);
        }
    }
    playerTurn = true;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
    {
        // Create buttons
        HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(GetWindowLongPtr(hwnd, GWLP_HINSTANCE));

        int buttonWidth = 100;
        int buttonHeight = 100;
        int spacing = 10;

        int x = spacing;
        int y = spacing;

        for (int row = 0; row < BOARD_SIZE; row++) {
            for (int col = 0; col < BOARD_SIZE; col++) {
                hwndButton[row][col] = CreateWindow(L"BUTTON", L"",
                    WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                    x, y+50, buttonWidth, buttonHeight,
                    hwnd, nullptr, hInstance, nullptr);

                x += buttonWidth + spacing;
            }

            x = spacing;
            y += buttonHeight + spacing;
        }

        hwndCheckbox = CreateWindow(L"BUTTON", L"Against Computer",
            WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
            0, -10, buttonWidth * BOARD_SIZE, 30, // ������� � ������ ��������
            hwnd, reinterpret_cast<HMENU>(IDC_CHECKBOX), hInstance, nullptr);

        // ���������� ��������� ��������� �������� � ����������� �� �������� againstComputer
        SendMessage(hwndCheckbox, BM_SETCHECK, againstComputer ? BST_CHECKED : BST_UNCHECKED, 0);

        return 0;
    }
    case WM_COMMAND:
    {
        // Handle button events
        int buttonId = LOWORD(wParam);
        int controlId = LOWORD(wParam); // �������� ������������� �������� ����������
        int notificationCode = HIWORD(wParam); // �������� ��� �����������
        int row = -1, col = -1;

        if (controlId == IDC_CHECKBOX && notificationCode == BN_CLICKED) {
            // ��������� ����� �� ��������
            int checked = SendMessage(hwndCheckbox, BM_GETCHECK, 0, 0);
            againstComputer = (checked == BST_CHECKED);
            ResetGame();
        }

                for (int i = 0; i < BOARD_SIZE; i++) {
                    for (int j = 0; j < BOARD_SIZE; j++) {
                        if (hwndButton[i][j] == reinterpret_cast<HWND>(lParam)) {
                            row = i;
                            col = j;
                            break;
                        }
                    }
                }
                if (row != -1 && col != -1 && board[row][col] == 0) {
                    if (playerTurn) {
                        // Handle player's move
                        board[row][col] = 'X';
                        SetWindowText(hwndButton[row][col], L"X");
                        EnableWindow(hwndButton[row][col], FALSE);
                    }
                    else if (!playerTurn && !againstComputer) {
                        // Handle second player's move
                        board[row][col] = 'O';
                        SetWindowText(hwndButton[row][col], L"O");
                        EnableWindow(hwndButton[row][col], FALSE);
                    }
                    // Check for a win
                    if (CheckWin('X')) {
                        MessageBox(hwnd, L"Player X wins!", L"Victory", MB_OK);
                        ResetGame();
                        return 0;
                    }
                    if (!againstComputer) {
                        if (CheckWin('O')) {
                            MessageBox(hwnd, L"Computer O wins!", L"Victory", MB_OK);
                            ResetGame();
                            return 0;
                        }
                    }
                    else if (CheckDraw()) {
                        MessageBox(hwnd, L"It's a draw!", L"Result", MB_OK);
                        ResetGame();
                        return 0;
                    }

                    playerTurn = !playerTurn;

                    if (againstComputer && !playerTurn) {
                        // Computer's move
                        MakeComputerMove();
                        // Check for a win
                        if (CheckWin('O')) {
                            MessageBox(hwnd, L"Computer O wins!", L"Victory", MB_OK);
                            ResetGame();
                            return 0;
                        }
                        else if (CheckDraw()) {
                            MessageBox(hwnd, L"It's a draw!", L"Result", MB_OK);
                            ResetGame();
                            return 0;
                        }
                        playerTurn = !playerTurn;
                    }
                }
        return 0;
    }
    case WM_SIZE:
    {
        // Handle window and button resizing
        int clientWidth = LOWORD(lParam);
        int clientHeight = HIWORD(lParam);

        int buttonWidth = (clientWidth - 4) / BOARD_SIZE - 10;
        int buttonHeight = (clientHeight - 4) / BOARD_SIZE - 10;
        int spacing = 10;

        int x = spacing;
        int y = spacing;

        for (int row = 0; row < BOARD_SIZE; row++) {
            for (int col = 0; col < BOARD_SIZE; col++) {
                SetWindowPos(hwndButton[row][col], nullptr, x, y, buttonWidth, buttonHeight, SWP_NOZORDER);
                x += buttonWidth + spacing;
            }

            x = spacing;
            y += buttonHeight + spacing;
        }

        return 0;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Draw the game board
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);

        int boardWidth = clientRect.right - clientRect.left;
        int boardHeight = clientRect.bottom - clientRect.top;

        int cellWidth = boardWidth / BOARD_SIZE;
        int cellHeight = boardHeight / BOARD_SIZE;

        for (int row = 0; row < BOARD_SIZE; row++) {
            for (int col = 0; col < BOARD_SIZE; col++) {
                RECT cellRect;
                cellRect.left = col * cellWidth;
                cellRect.top = row * cellHeight;
                cellRect.right = (col + 1) * cellWidth;
                cellRect.bottom = (row + 1) * cellHeight;

                // Draw the cell rectangle
                DrawEdge(hdc, &cellRect, EDGE_SUNKEN, BF_RECT);

                // Draw the symbol in the cell, if any
                if (board[row][col] != 0) {
                    WCHAR symbol = (board[row][col] == 'X') ? L'X' : L'O';
                    DrawText(hdc, &symbol, 1, &cellRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                }
            }
        }

        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_CLOSE:
        // Close the window
        DestroyWindow(hwnd);
        return 0;
    case WM_DESTROY:
        // Quit the application
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const wchar_t CLASS_NAME[] = L"TicTacToeClass";
    const wchar_t WINDOW_NAME[] = L"Tic Tac Toe";

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    RegisterClass(&wc);

    HWND hwnd = CreateWindow(CLASS_NAME, WINDOW_NAME, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 320, 320, nullptr, nullptr, hInstance, nullptr);

    if (hwnd == nullptr)
        return 0;

    ShowWindow(hwnd, nCmdShow);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}
