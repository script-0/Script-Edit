#ifndef PROTOTYPES_H_INCLUDED
#define PROTOTYPES_H_INCLUDED


BOOL CaptureImage(HWND,HWND,HINSTANCE);

void modify_size(HWND,BOOL,DWORD);
void goto_function(HWND , int);
BOOL setfont(HWND,HFONT*,COLORREF*);
BOOL isRegistred(HWND);
LPSTR getFileName(HWND);

#endif // PROTOTYPES_H_INCLUDED
