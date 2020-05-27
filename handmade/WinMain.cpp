/*  ================================================================
    $File: WinMain.cpp
    $Date: 05/26/2020
    $Revision: 05/26/2020
    $Creator: Diogo Junqueira Geraldo
    $Notice: (C) Copyright 2020 by d.junqueira. All Rights Reserved.
    ================================================================  */

#include <windows.h>

LRESULT CALLBACK MainWindowCallback(HWND window,UINT message,WPARAM wParam, LPARAM lParam)
{
    LRESULT handleMessageCode = 0;

    switch (message)
    {
        case WM_SIZE:
        {
            OutputDebugString(L"WM_SIZE\n");
        } break;

        case WM_DESTROY:
        {
            OutputDebugString(L"WM_DESTROY\n");
        } break;

        case WM_CLOSE:
        {
            OutputDebugString(L"WM_CLOSE\n");
        } break;

        case WM_ACTIVATE:
        {
            OutputDebugString(L"WM_ACTIVATE\n");
        } break;

        case WM_PAINT:
        {
            PAINTSTRUCT paint;
            HDC deviceContext = BeginPaint(window, &paint); {
                int x = paint.rcPaint.left;
                int y = paint.rcPaint.top;
                int width = paint.rcPaint.right - x;
                int height = paint.rcPaint.bottom - y;
                PatBlt(deviceContext, x, y, width, height, WHITENESS);
            } EndPaint(window, &paint);
        } break;

        default:
        {
            handleMessageCode = DefWindowProc(window, message, wParam, lParam);
        } break;
    }

    return handleMessageCode;
}

INT CALLBACK WinMain(HINSTANCE instance,
                     HINSTANCE prevInstance,
                     PSTR commandLine,
                     INT showCode)
{
    WNDCLASS windowClass = {};
    windowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
    windowClass.lpfnWndProc = MainWindowCallback;
    windowClass.hInstance = instance;
    windowClass.lpszClassName = L"HandmadeHerowindowClass";
    //TODO(Diogo Junqueira Geraldo): Please select a icon for the game window >> windowClass.hIcon;

    if (RegisterClass(&windowClass)) {

        HWND windowHandle = 
             CreateWindowEx(0,
                            windowClass.lpszClassName,
                            L"Handmade Hero",
                            WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                            CW_USEDEFAULT,
                            CW_USEDEFAULT,
                            CW_USEDEFAULT,
                            CW_USEDEFAULT,
                            0,
                            0,
                            instance,
                            0);

        if (windowHandle)
        {     
            MSG message;
            while(true)
            {
                if (GetMessage(&message, 0, 0, 0) > 0)
                {
                    TranslateMessage(&message);
                    DispatchMessage(&message);
                } else break;
            }
        } // TODO(Diogo Junqueira Geraldo): If windows can´t create the window, handler and/or log the error.

    } // TODO(Diogo Junqueira Geraldo): If windows can´t register the class, handle and/or log the error.

    return 0;
}