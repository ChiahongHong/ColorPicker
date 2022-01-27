// Copyright (C) 2022 Chiahong Hong.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include "ColorPicker.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ PWSTR pCmdLine,
                      _In_ int nCmdShow)
{
    SetProcessDPIAware();

    // Register the window class.
    TCHAR CLASS_NAME[] = "MainWindow";
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(wcex);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = CLASS_NAME;
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    RegisterClassEx(&wcex);

    // Create the window.

    HWND hwnd = CreateWindowEx(
        WS_EX_TOPMOST,                                        // Always on top
        CLASS_NAME,                                           // Window class
        TEXT("Color Picker"),                                 // Window text
        WS_OVERLAPPEDWINDOW & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX,  // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, 500, 300,

        NULL,       // Parent window
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    // Run the message loop.
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static COLORREF color, prevColor;

    switch (uMsg)
    {
    case WM_CREATE:
        SetTimer(hwnd, 1, 50, NULL);
        return 0;

    case WM_TIMER:
        HDC monitor;
        POINT cursor;
        monitor = GetDC(NULL);
        GetCursorPos(&cursor);
        color = GetPixel(monitor, cursor.x, cursor.y);
        ReleaseDC(NULL, monitor);

        if (color != prevColor) {
            prevColor = color;
            InvalidateRect(hwnd, NULL, TRUE);
        }
        return 0;

    case WM_DESTROY:
        KillTimer(hwnd, 1);
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT rect;
        GetClientRect(hwnd, &rect);

        FillRect(hdc, &rect, CreateSolidBrush(color));

        if (GetRValue(color) + GetGValue(color) + GetBValue(color) > 127 * 3)
            SetTextColor(hdc, RGB(0, 0, 0));
        else
            SetTextColor(hdc, RGB(255, 255, 255));

        SetBkMode(hdc, TRANSPARENT);

        TCHAR buffer[16];
        StringCchPrintf(buffer, 16, TEXT("#%2X%2X%2X"), GetRValue(color), GetGValue(color), GetBValue(color));

        HGDIOBJ hNewFont = CreateFont(70, 0, 0, 0, FW_DONTCARE, 0, 0, 0, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
            CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Consolas"));
        
        HGDIOBJ hOldFont = (HFONT)SelectObject(hdc, hNewFont);

        // Draw text
        DrawText(hdc, buffer, -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

        // Always select the old font back into the DC
        SelectObject(hdc, hOldFont);
        DeleteObject(hNewFont);

        EndPaint(hwnd, &ps);
    }
    return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
