/*  ================================================================
    $File: WinMain.cpp
    $Date: 05/26/2020
    $Revision: 05/26/2020
    $Creator: Diogo Junqueira Geraldo
    $Notice: (C) Copyright 2020 by d.junqueira. All Rights Reserved.
    ================================================================  */

#include <windows.h>

#define internal static
#define persist_static static
#define global_static static

global_static bool running; // TODO(Diogo Junqueira Geraldo): This is a global for now
global_static BITMAPINFO bitMapInfo;
global_static void* bitMapMemory;   


internal void Win32ResizeDIBSection(int width, int height)
{
    if (bitMapMemory)
    {
        VirtualFree(bitMapMemory, 0, MEM_RELEASE);
    }

    bitMapInfo.bmiHeader.biSize = sizeof(bitMapInfo.bmiHeader);
    bitMapInfo.bmiHeader.biWidth = width;
    bitMapInfo.bmiHeader.biHeight = height;
    bitMapInfo.bmiHeader.biPlanes = 1;
    bitMapInfo.bmiHeader.biBitCount = 32;
    bitMapInfo.bmiHeader.biCompression = BI_RGB;

    int bytesPerPixel = 4;
    int bitMapMemorySizeBytes = (width * height) * bytesPerPixel;
    bitMapMemory = VirtualAlloc(0, bitMapMemorySizeBytes, MEM_COMMIT, PAGE_READWRITE);
}

internal void Win32UpdateWindow(HDC deviceContext, int x, int y, int width, int height)
{
    StretchDIBits(deviceContext,
                  x,y,width,height, // target
                  x,y,width,height, // source
                  bitMapMemory,
                  &bitMapInfo,
                  DIB_RGB_COLORS,SRCCOPY);
}

LRESULT CALLBACK Win32MainWindowCallback(HWND window,UINT message,WPARAM wParam, LPARAM lParam)
{
    LRESULT handleMessageCode = 0;

    switch (message)
    {
        case WM_SIZE:
        {
            RECT clientRect;
            GetClientRect(window, &clientRect);

            int width = clientRect.right - clientRect.left;
            int height = clientRect.bottom - clientRect.top;

            Win32ResizeDIBSection(width, height);
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
                Win32UpdateWindow(deviceContext, x, y, width, height);
            } EndPaint(window, &paint);
        } break;

        case WM_CLOSE:
        {
            running = false;
        } break; // TODO(Diogo Junqueira Geraldo): Handle this with a message to the user

        case WM_DESTROY:
        {
            running = false;
        } break; // TODO(Diogo Junqueira Geraldo): Handle this as an error - try recreate window

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
    windowClass.lpfnWndProc = Win32MainWindowCallback;
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

        if (running = windowHandle)
        {
            MSG message;
            while(running)
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