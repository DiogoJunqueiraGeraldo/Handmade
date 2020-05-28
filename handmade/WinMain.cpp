/*  ================================================================
    $File: WinMain.cpp
    $Date: 05/26/2020
    $Revision: 05/28/2020
    $Creator: Diogo Junqueira Geraldo
    $Notice: (C) Copyright 2020 by d.junqueira. All Rights Reserved.
    ================================================================  */

#include <windows.h>
#include <stdint.h>

#define internal static
#define persist_static static
#define global_static static

#define RED 2
#define GREEN 1
#define BLUE 0

global_static bool running; // TODO(Diogo Junqueira Geraldo): This is a global for now
global_static BITMAPINFO bitMapInfo;
global_static void* bitMapMemory;   
global_static int bitMapWidth;
global_static int bitMapHeight;
global_static int bytesPerPixel = 4;

internal void Win32CalculateColors(uint8_t* pixel, int x, int y, int rgb, int xoffset, int yoffset)
{
    switch (rgb)
    {
        case RED:
        {
            *pixel = 100;
        } break;
        case GREEN:
        {
            *pixel = 100;
        } break;
        case BLUE:
        {
            *pixel = 0;
        } break;
    }
}

internal void Win32Render(int xoffset, int yoffset)
{
    int pitch = (bitMapWidth * bytesPerPixel);
    uint8_t* row = ((uint8_t*)bitMapMemory);
    for (int y = 0; y < bitMapHeight; y++)
    {
        uint8_t* pixel = ((uint8_t*)row);
        for (int x = 0; x < bitMapWidth; x++) // NOTE(Diogo Junqueira Geraldo): 0xBBGGRRxx
        {
            for (int rgb = 0; rgb < 4; rgb++)
            {
                Win32CalculateColors(pixel, x, y, rgb, xoffset, yoffset);
                pixel++;
            }
        }
        row += pitch;
    }
}

internal void Win32ResizeDIBSection(int width, int height)
{
    if (bitMapMemory)
    {
        VirtualFree(bitMapMemory, 0, MEM_RELEASE);
    }

    bitMapWidth = width;
    bitMapHeight = height;
    bitMapInfo.bmiHeader.biSize = sizeof(bitMapInfo.bmiHeader);
    bitMapInfo.bmiHeader.biWidth = bitMapWidth;
    bitMapInfo.bmiHeader.biHeight = -bitMapHeight; // NOTE(Diogo Junqueira Geraldo): negative value makes the render start from top-left
    bitMapInfo.bmiHeader.biPlanes = 1;
    bitMapInfo.bmiHeader.biBitCount = 32;
    bitMapInfo.bmiHeader.biCompression = BI_RGB;

    
    int bitMapMemorySizeBytes = (width * height) * bytesPerPixel;
    bitMapMemory = VirtualAlloc(0, bitMapMemorySizeBytes, MEM_COMMIT, PAGE_READWRITE);;
}

internal void Win32UpdateWindow(HDC deviceContext,RECT *clientRect, int x, int y, int width, int height)
{
    int windowWidth = clientRect->right - clientRect->left;
    int windowHeight = clientRect->bottom - clientRect->top;


    StretchDIBits(deviceContext,
                  0,0, bitMapWidth, bitMapHeight,
                  0,0, windowWidth, windowHeight,
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

                RECT clientRect;
                GetClientRect(window, &clientRect);
                Win32UpdateWindow(deviceContext, &clientRect, x, y, width, height);
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

        HWND window = 
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

        if (running = window)
        {
            int xoffset = 0;
            int yoffset = 0;
            MSG message;
            while(running)
            {
                while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
                {   
                    if (message.message == WM_QUIT) {
                        running = false;
                    }

                    TranslateMessage(&message);
                    DispatchMessage(&message);
                }
                Win32Render(xoffset, yoffset);

                HDC deviceContext = GetDC(window);
                RECT clientRect;
                GetClientRect(window, &clientRect);
                int windowWidth = clientRect.right - clientRect.left;
                int windowHeight = clientRect.bottom - clientRect.top;

                Win32UpdateWindow(deviceContext, &clientRect, 0, 0, windowWidth, windowHeight);

                ReleaseDC(window, deviceContext);
                xoffset++;
                yoffset++;
            }
        } // TODO(Diogo Junqueira Geraldo): If windows can´t create the window, handler and/or log the error.

    } // TODO(Diogo Junqueira Geraldo): If windows can´t register the class, handle and/or log the error.

    return 0;
}