#define CAP_ERROR -1

#ifndef __TCHAR_H__
    #include <tchar.h>
    #define __TCHAR_H__
#endif // __TCHAR_H__

#ifndef __STDIO_H__
    #include <stdio.h>
    #define __STDIO_H__
#endif // __STDIO_H__

#ifndef __WINDOWS_H__
    #define __WINDOWS_H__
    #include <windows.h>
#endif // __WINDOWS_H__

#ifndef __COMMCTRL_H__
    #include <commctrl.h> //necessatry for inicommontcontrols();
    #define __COMMCTRL_H__
#endif // __COMMCTRL_H__

#ifndef __COMMDLG_H__
    #include <commdlg.h>
    #define __COMMDLG_H__
#endif // __COMMDLG_H__


void saveBitmap(HWND g_hWnd,HINSTANCE hinst);

BOOL CaptureImage(HWND hwnd,HWND hwnd2,HINSTANCE hinst)
{
    //MoveWindow(hwnd,0,0,450,860,FALSE);
   // UpdateWindow(hwnd);
    //SetWindowPos(hwnd,HWND_BOTTOM,18,18,520,520,SWP_DRAWFRAME);
    HDC hdcScreen=NULL;
    HDC hdcWindow=NULL;
    HDC hdcMemDC=NULL;
    RECT rcClient;

    hdcScreen = GetDC(hwnd2);
    hdcWindow = GetDC(hwnd);
    hdcMemDC = CreateCompatibleDC(hdcWindow);

    if(!hdcMemDC)
    {
        showMessageError(hwnd,"Acces denied to DC ->CaptureImage function");
        return CAP_ERROR;
    }

    GetClientRect(hwnd,&rcClient);
    SetStretchBltMode(hdcWindow,HALFTONE);

    if(!StretchBlt(hdcWindow,0,0,rcClient.right,rcClient.bottom,hdcScreen,0,0,GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN),SRCCOPY))
    {
        showMessageError(hwnd,"StretchBit has failed->CaptureImage function");
        return CAP_ERROR;
    }
   // SetWindowPos(hwnd,HWND_TOP,18,18,520,520,SWP_DRAWFRAME);
    saveBitmap(hwnd,hinst);
    ReleaseDC(NULL,hdcScreen);
    ReleaseDC(hwnd,hdcWindow);
    DeleteDC(hdcMemDC);
    DeleteObject(hdcMemDC);
    return TRUE;
}

void saveBitmap(HWND g_hWnd,HINSTANCE hinst)
{
    /****take name and path of bmp file****/
    OPENFILENAME ofn;
    static char fname[MAX_PATH]=" ";
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = g_hWnd;
    ofn.lpstrTitle ="Select Bmp file";
    ofn.lpstrFilter = "Bmp Files (*.bmp)\0*.bmp\0";
    ofn.lpstrFile = fname;
    ofn.hInstance=hinst;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY|OFN_CREATEPROMPT;
    ofn.lpstrDefExt = "bmp";
    InitCommonControls();
    if(!GetOpenFileName(&ofn))
    {
        showMessageError(g_hWnd,"Undefined or incorrect file name->SaveBitmap function");
    }
    else
    {
        HDC hdcScreen;
        HDC hdcWindow;
        HDC hdcMemDC;
        HBITMAP hbmScreen = NULL;
        BITMAP bmpScreen;

    // Retrieve the handle to a display device context for the client area of the window.
        hdcScreen = GetDC(NULL);
        hdcWindow = GetDC(g_hWnd);
        // Create a compatible DC which is used in a BitBlt from the window DC
        hdcMemDC = CreateCompatibleDC(hdcScreen);
        if(!hdcMemDC)
        {
        ReleaseDC(NULL,hdcScreen);
        ReleaseDC(g_hWnd,hdcWindow);
        }
        else{
            RECT rcClient;
            GetClientRect(g_hWnd, &rcClient);//change g_hWnd to NULL if you want to take screenshot of all screen
            // hbmScreen = CreateCompatibleBitmap(hdcScreen,GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
            // hbmScreen = CreateCompatibleBitmap(hdcMemDC, rcClient.right-rcClient.left, rcClient.bottom-rcClient.top);
            hbmScreen = CreateCompatibleBitmap(hdcScreen, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
            if(!hbmScreen)
            {
                DeleteObject(hdcMemDC);
                ReleaseDC(NULL,hdcScreen);
                ReleaseDC(g_hWnd,hdcWindow);
            }
            else{
                SelectObject(hdcMemDC,hbmScreen);

                if(!BitBlt(hdcMemDC,0,0,rcClient.right-rcClient.left, rcClient.bottom-rcClient.top,hdcWindow,0,0,SRCCOPY))//remplacer hdcWindow/hdcscreen par le dc de la fenetre qu'on veut screensheter
                {

        DeleteObject(hbmScreen);
        DeleteObject(hdcMemDC);
        ReleaseDC(NULL,hdcScreen);
        ReleaseDC(g_hWnd,hdcWindow);
            }
    /*GetClientRect(g_hWnd, &rcClient);
        BitBlt(hdcMemDC,0,0,rcClient.right-rcClient.left, rcClient.bottom-rcClient.top,hdcWindow,0,0,SRCAND);
         GetClientRect(NULL, &rcClient);*/
                    GetObject(hbmScreen,sizeof(BITMAP),&bmpScreen);

                    BITMAPFILEHEADER   bmfHeader;
                    BITMAPINFOHEADER   bi;

                    bi.biSize = sizeof(BITMAPINFOHEADER);
                    bi.biWidth = bmpScreen.bmWidth;
                    bi.biHeight = bmpScreen.bmHeight;
                    bi.biPlanes = 1;
                    bi.biBitCount = 32;
                    bi.biCompression = BI_RGB;
                    bi.biSizeImage = 0;
                    bi.biXPelsPerMeter = 0;
                    bi.biYPelsPerMeter = 0;
                    bi.biClrUsed = 0;
                    bi.biClrImportant = 0;

                    DWORD dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 *
                      bmpScreen.bmHeight;

    // Starting with 32-bit Windows, GlobalAlloc and LocalAlloc are implemented as wrapper functions that
    // call HeapAlloc using a handle to the process's default heap. Therefore, GlobalAlloc and LocalAlloc
    // have greater overhead than HeapAlloc.
                    HANDLE hDIB = GlobalAlloc(GHND,dwBmpSize);
                    char *lpbitmap = (char *)GlobalLock(hDIB);

    // Gets the "bits" from the bitmap and copies them into a buffer
    // which is pointed to by lpbitmap.
                    GetDIBits(hdcWindow, hbmScreen, 0,(UINT)bmpScreen.bmHeight, lpbitmap,(BITMAPINFO *)&bi, DIB_RGB_COLORS);

    // A file is created, this is where we will save the screen capture.
                    HANDLE hFile = CreateFile(fname,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL, NULL);

    // Add the size of the headers to the size of the bitmap to get the total file size
                    DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    //Offset to where the actual bitmap bits start.
                    bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

    //Size of the file
                    bmfHeader.bfSize = dwSizeofDIB;

    //bfType must always be BM for Bitmaps
                    bmfHeader.bfType = 0x4D42; //BM

                    DWORD dwBytesWritten = 0;
                    WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten,  NULL);
                    WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
                    WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);

    //Unlock and Free the DIB from the heap
                    GlobalUnlock(hDIB);
                    GlobalFree(hDIB);

    //Close the handle for the file that was created
                    CloseHandle(hFile);

    //Clean up
                    DeleteObject(hbmScreen);
                    DeleteObject(hdcMemDC);
                    ReleaseDC(NULL,hdcScreen);
                    ReleaseDC(g_hWnd,hdcWindow);
            }
        }

    }
}
