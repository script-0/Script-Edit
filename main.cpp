
#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif
#define _WIN32_WINNT 0x600 /**< needed to use SetLayeredWindowsAttributes (transparency)*/
#define _WIN32_IE 0x600
#define MY_WM_NOTIFYICON WM_USER+1

#include <tchar.h>
#define __TCHAR_H__

#include <stdlib.h>
#define __STDLIB_H__

#include <stdio.h>
#define __STDIO_H__

#include <windows.h>
#define __WINDOWS_H__

#include <commctrl.h> /**<necessatry for inicommontcontrols();*/
#define __COMMCTRL_H__

#include <dbt.h> //Usage de DEVICE CHANGE

#include <commdlg.h>
#define __COMMDLG_H__

#include <math.h>
#define __MATH_H__

#include <richedit.h>
#include <shellapi.h>

#include "coordonnees.h"
#include "resource.h"
#include "initial.h"
#include "capture.c"
#include "prototypes.h"
#include "color.h"
#include "fonctions1.c"
#include "set_hyperlink.c"
#define PARAM_FILE_LOCATION "config.ms"
#define MUTEX_LOCATION "config.ms"


//#include "mainPrototype.h"

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);
BOOL APIENTRY DialProcAbout(HWND,UINT,WPARAM,LPARAM);
BOOL APIENTRY DialView(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);
BOOL APIENTRY DialReg(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);
BOOL APIENTRY manage_text(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);
BOOL APIENTRY GotoProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);
BOOL APIENTRY renameProc(HWND hwnd ,UINT message, WPARAM wParam, LPARAM lParam);
BOOL save_as(HWND);
LPSTR getMutexLocation();
HICON CreateSmallIcon( HWND hWnd );
BOOL save(HWND);
void read (HWND ,BOOL*);
void update_status_bar(HWND,HWND);

LRESULT CALLBACK edit_control_proc(HWND, UINT, WPARAM, LPARAM);/**<procedure used for sub classing edit control IDM_TEXT*/
WNDPROC default_edit_control_proc;/**<Variable used for store default procedure of control edit IDM_TEXT*/
HMENU d_menu,d_s_menu,sub_menu,sub_menu2;/**<Handle of the menu of windows*/
HANDLE icon;
BOOL change=FALSE;/**<Determine si le contenu d'un fichier a ete modifie sans etre suvegarde*/
TCHAR ClassName[ ] = _TEXT("TextEditor");
BOOL Setfile[2] ={FALSE,FALSE};/**<Determine si un fichier a ete ouvert par l'editeur de texte(Setfile[0]) et si son backup a ete cree(Setfile[1])*/
BOOL sel_state=FALSE;/**<Determine si une selection est faite dans le control edit*/
BOOL undo_state=FALSE;/**<Determine si une operation dans le control edit peut etre annulee*/
HINSTANCE hinst;
NOTIFYICONDATA TrayIcon;

HFONT hfont=(HFONT)GetStockObject(DEFAULT_GUI_FONT);

HBRUSH hbdialog=DEFAULT_BRUSH_DIALOG_BOX;
HBRUSH hscrool=DEFAULT_BRUSH_SCROOL_BAR;
HBRUSH he=DEFAULT_BRUSH_BKCOLOR_S;//=CreatePatternBrush(LoadBitmap(hinst,MAKEINTRESOURCE(SCP_LOGO)));//
HBRUSH hbstatic=DEFAULT_BRUSH_STATIC_CONTROL;

INITIAL_INFO Info;
APP_COLOR default_color;

short nb_tab;
BOOL size_change=FALSE;/**<Determine si les dimensions de la fenetre ont changees( utilise dans update_status_bar())**/

char*** paths=(char***)GlobalAlloc(GPTR,sizeof(char**));/**<Recupere le chemin d'acces absolu des fichiers dropper (Function:DialProc -> WM_COMMAND ->WM_DROPFILES)*/
char mutex[_MAX_PATH];
LPSTR* commande_line;
HWND statusbar;
BOOL S_State_bar=TRUE;/**<Boolean equal to TRUE (if state bar is visible)*/
char caption[MAX_PATH+3]=_TEXT("TextEditor");
char commande[MAX_PATH+20];/**<Neccessary to run command with gcc see WndProc > WM_COMMAND>IDM_COM_GCC*/
char* newName = NULL;

POINT position; //Position of default window (use by dialog box during their initialisations)
float width;
float height;
int yWindows;
BOOL flag = FALSE;

char first_char=' ';//use in edit control proc
char second_char=' ';

UINT uReplaceMsg=RegisterWindowMessage(FINDMSGSTRING);

const char cueBannerText[] = _TEXT("---My Script--- said: 'Enter text here'\0");

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
{
    ShowWindow(GetConsoleWindow(),SW_HIDE);/**<Hide Debug console*/
    commande_line=&lpszArgument;
    LoadColor(&default_color);/**<Load default color used in the app*/
    HACCEL haccel;/**<Necessary to enabled accelerators(Keyboard shortcuts)*/
    hinst=hInstance;
    HWND hwnd;
    MSG message;
    WNDCLASSEX wincl;
    wincl.cbSize = sizeof (WNDCLASSEX);
    wincl.style =CS_DBLCLKS;
    //CS_DROPSHADOW CS_SAVEBITS:cree une ombre autour de la fenetre ,
    //CS_DBLCLKS:le double clic de la souris est envoye a la fonctions des messages
    wincl.lpfnWndProc =(WNDPROC) WndProc;
    wincl.hInstance = hInstance;
    wincl.lpszClassName = ClassName;

    wincl.hIcon = LoadIcon (hInstance,MAKEINTRESOURCE(SCP_ICON));
    wincl.hIconSm =(HICON) LoadIcon(hInstance,MAKEINTRESOURCE(SCP_ICON));
    wincl.hCursor =LoadCursor (hInstance, MAKEINTRESOURCE(SCP_CUR));
    wincl.lpszMenuName = 0;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground =hbdialog;
    if (!RegisterClassEx(&wincl))
    {
        showMessageError(NULL,"\t     We are sorry!\nRegistration of your application failed.\n\tPlease try again later.");
        return 0;
    }
    if(!isRegistred(hwnd)){DialogBox(hinst,(LPCTSTR)IDD_REGISTRATION,hwnd,(DLGPROC)DialReg);}
    ZeroMemory(&TrayIcon, sizeof(NOTIFYICONDATA));
    //-----------------------------------

    load_initial_info(&Info);

    hwnd = CreateWindowEx (
               0,
               ClassName,
               _TEXT("TextEditor"),
               WS_BORDER|WS_CAPTION|WS_OVERLAPPEDWINDOW|WS_EX_CONTEXTHELP,
               (GetSystemMetrics(SM_CXSCREEN)- Info.width)/2,//Info.x0,
               (GetSystemMetrics(SM_CYSCREEN) - Info.height)/2,//Info.y0,
               Info.width,//DIALOG_WIDTH-375,
               Info.height,//DIALOG_HEIGHT-43,
               HWND_DESKTOP,
               NULL,
               hInstance,
               NULL
           );
    if(!hwnd){
        showMessageError(NULL,"\tWe are sorry!\n  Initialisation of window failed. \n         Please, try again later...");
        return FALSE;
    }
    ShowWindow (hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    SetTimer(hwnd,ID_TIMER,300,NULL);
    InitCommonControls();/**<Neccessary for all common control*/

    haccel=LoadAccelerators(hInstance,MAKEINTRESOURCE(ID_ACCEL));

    while (GetMessage (&message, NULL, 0, 0))
    {
        if(!TranslateAccelerator(hwnd, haccel, &message))
        {
            TranslateMessage(&message);

            DispatchMessage(&message);
        }
    }

    return message.wParam;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HWND button= NULL;
    HDC hdc;
    RECT rc;
    HFONT font;
    HANDLE icon;
    HWND temp ;
    SHELLEXECUTEINFO info;
    short int trackbar_pos=100;

    DWORD sel_start=0,sel_end=0;

    FINDREPLACE fr;       // common dialog box structure
    CHAR szFindWhat[100]={'\0'};  // buffer receiving string
    HWND hdlg = NULL;     // handle of Find dialog box

   //HANDLE note;/**<use on WM_COMMAND > IDM_TEST for Test directory changes*/
   //DWORD note_state;/**<use on WM_COMMAND > IDM_TEST for Test directory changes*/
    LPSTR fname = NULL; //Use to get path of bmp file in WM_COMMAND > IDM_BCK_S_IMAGE
    switch (message)
    {
    case WM_TIMER:
    {
        if(S_State_bar==TRUE)
        {
                    update_status_bar(statusbar,hwnd);
        }
         HMENU menu_temp=GetMenu(hwnd);
        temp = GetDlgItem(hwnd,IDM_TEXT);
        SendMessage(temp,EM_GETSEL,(WPARAM)&sel_start,(LPARAM)&sel_end);//Teste si un texte est selectionne. Dans ce cas, sel_start et sel_end sont differents.
        if(sel_start==sel_end)
        {
            if(sel_state==TRUE)
            {
                sel_state=FALSE;
                EnableMenuItem(menu_temp,IDM_EDCOPY,MF_BYCOMMAND|MF_DISABLED);
                EnableMenuItem(menu_temp,IDM_EDCUT,MF_BYCOMMAND|MF_DISABLED);
                EnableMenuItem(menu_temp,IDM_EDDEL,MF_BYCOMMAND|MF_DISABLED);
            }
        }
        else{
             if(sel_state==FALSE)
             {
                sel_state=TRUE;
                //HMENU menu_temp=GetMenu(hwnd);
                EnableMenuItem(menu_temp,IDM_EDCOPY,MF_BYCOMMAND|MF_ENABLED);
                EnableMenuItem(menu_temp,IDM_EDCUT,MF_BYCOMMAND|MF_ENABLED);
                EnableMenuItem(menu_temp,IDM_EDDEL,MF_BYCOMMAND|MF_ENABLED);
             }
            }
        if(SendMessage(temp,EM_CANUNDO,0,0))
        {
            if(undo_state==FALSE)
            {
                EnableMenuItem(menu_temp,IDM_EDUNDO,MF_BYCOMMAND|MF_ENABLED);
                undo_state=TRUE;
            }
        }
        else{
             if(undo_state==TRUE)
             {
                EnableMenuItem(menu_temp,IDM_EDUNDO,MF_BYCOMMAND|MF_DISABLED);
                undo_state=FALSE;
             }
           }
        if(SendDlgItemMessage(hwnd,IDM_CAPTION,CB_GETCOUNT,0,0) == 0)
           {
               EnableMenuItem(menu_temp,IDM_CLOSE,MF_BYCOMMAND|MF_DISABLED);
           }
         else  EnableMenuItem(menu_temp,IDM_CLOSE,MF_BYCOMMAND|MF_ENABLED);
       temp = NULL;
       break;
    }
    case WM_CREATE:
    {
      SECURITY_ATTRIBUTES sa ;
      sa.bInheritHandle = true;
      sa.nLength = sizeof(SECURITY_ATTRIBUTES);
      sa.lpSecurityDescriptor = NULL;
      HANDLE mutex = NULL;
      mutex = CreateMutex(&sa,true,"ScriptEdit");
      if(GetLastError() == ERROR_ALREADY_EXISTS)
      {
        if(fileExist(getMutexLocation()))
        {
          showMessageError(hwnd,"Another instance of ScriptEdit is running. Only one instance are authorized. To change this go to \"Edit\" tab and deselect \"Only One Instance\"");
          KillTimer(hwnd,ID_TIMER);
          DestroyWindow(hwnd);
        }
      }
      /**<Enable layered style*/
      SetWindowLong(hwnd,GWL_EXSTYLE,WS_EX_LAYERED|GetWindowLong(hwnd,GWL_EXSTYLE));
      /**<Enable drap and drop*/
       DragAcceptFiles(hwnd,TRUE);
      {/**<Different button and trackbar for opacity*/
       GetClientRect(hwnd,&rc);
       LoadLibrary("RICHED32.DLL");
       button=CreateWindowEx(0,"RichEdit",
                             "***No input file***",
                             ES_LEFT|WS_VISIBLE|WS_CHILD|ES_READONLY|ES_AUTOHSCROLL,
                             20,
                             0,
                             -rc.left + rc.right -20,//DIALOG_WIDTH-15,
                             23,
                             hwnd,
                             (HMENU)IDM_FILENAME,
                             hinst,
                             NULL);

        /**<Define text alignment: center*/
       PARAFORMAT parag_format;
       parag_format.cbSize=sizeof(PARAFORMAT);
       parag_format.dwMask=PFM_ALIGNMENT;
       parag_format.wAlignment=PFA_CENTER;
       SendMessage(button,EM_SETPARAFORMAT,(WPARAM)SCF_ALL,(LPARAM) (PARAFORMAT FAR *) &parag_format);
       /*************/

       CHARFORMAT char_format;
       char_format.cbSize=sizeof(CHARFORMAT);
       char_format.dwMask=CFM_COLOR|CFM_FACE|CFM_SIZE|CFM_CHARSET;
       char_format.dwEffects=0;
       char_format.crTextColor=default_color.text_color_file_name;
       char_format.bCharSet=DEFAULT_CHARSET;
       char_format.bPitchAndFamily=DEFAULT_PITCH|FF_DECORATIVE;
       font_copy(char_format.szFaceName,"Lucida Calligraphy");
       char_format.yHeight=(LONG)-MulDiv(14, GetDeviceCaps(GetDC(NULL), LOGPIXELSY), 72);
       if(SendMessage(GetDlgItem(hwnd,IDM_FILENAME),EM_SETCHARFORMAT,(WPARAM)SCF_ALL,(LPARAM) (CHARFORMAT FAR *) &char_format)==0)
       {
           showMessageError(hwnd,"\tWe are sorry!\nInitialisation of filename box failed.\n   Please reload the application.");
       }

       SendMessage(button,EM_SETBKGNDCOLOR,(WPARAM)0,(LPARAM)default_color.bck_color_file_name);
       UpdateWindow(button);

       button=CreateWindowEx(0,
                             "COMBOBOX",
                             "",
                             WS_CHILD|WS_VISIBLE|CBS_DISABLENOSCROLL|CBS_DROPDOWNLIST,
                             0,
                             -5,
                             20,
                             10,
                             hwnd,
                             (HMENU)IDM_CAPTION,
                             hinst,
                             NULL);

        button=CreateWindowEx(0,
                              "EDIT",
                               "",
                               WS_EX_LAYERED|ES_LEFT|ES_MULTILINE|WS_VSCROLL|WS_HSCROLL|ES_AUTOHSCROLL|ES_AUTOVSCROLL|//
                               WS_CHILD|WS_VISIBLE|WS_BORDER|ES_WANTRETURN|WS_TABSTOP|ES_NOHIDESEL ,
                               5,
                               50,
                               -rc.left + rc.right - 6,//812,
                               rc.bottom - rc.top - 75,//602,
                               hwnd,
                               (HMENU)IDM_TEXT,
                               hinst,
                               NULL);
       hfont=CreateFont(19,0,0,0,250,FALSE,FALSE,FALSE,0,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_DECORATIVE,"Lucida Calligraphy");
       SendDlgItemMessage(hwnd,IDM_CAPTION,WM_SETFONT,(WPARAM)(hfont),TRUE);
       SendMessage(button,WM_SETFONT,(WPARAM)(hfont),TRUE);
       SendMessage(button,EM_SETMARGINS,(WPARAM)EC_LEFTMARGIN,(LPARAM)MAKELONG(20,1));/**<Set margin*/
       SetFocus(button);
/**<Subclass edit control :IDM_TEXT **/
        default_edit_control_proc=(WNDPROC)GetWindowLong(button,GWLP_WNDPROC);
        SetWindowLong(button,GWLP_WNDPROC,(LONG_PTR)edit_control_proc);

/******/

       button=CreateWindowEx(0,"BUTTON",
                               "Save",
                               BS_PUSHBUTTON|WS_CHILD|WS_VISIBLE|WS_DISABLED|WS_TABSTOP,
                               -3,
                               22,
                               48,
                               23,
                               hwnd,
                               (HMENU)IDM_SAVE,
                               hinst,
                               NULL);
     SendDlgItemMessage(hwnd,IDM_SAVE,WM_SETFONT,(WPARAM)(hfont),TRUE);
     button=CreateWindowEx(0,"BUTTON",
                               "Cancel ",
                               BS_PUSHBUTTON|WS_CHILD|WS_VISIBLE|WS_DISABLED|WS_TABSTOP,
                               rc.right-60,
                               22,
                               60,
                               23,
                               hwnd,
                               (HMENU)IDCANCEL,
                               hinst,
                               NULL);
       SendDlgItemMessage(hwnd,IDCANCEL,WM_SETFONT,(WPARAM)(hfont),TRUE);
       /**<Trackbar and ok button*/
       button=CreateWindowEx(0,TRACKBAR_CLASS,
                           "Opacity",
                           WS_CHILD|WS_VISIBLE|TBS_AUTOTICKS|TBS_ENABLESELRANGE,
                           X_TRACKBAR,
                           22,
                           TRACKBAR_WD,
                           23,
                           hwnd,
                           (HMENU)ID_TRACKBAR,
                           hinst,
                           NULL);

       SendMessage(button, TBM_SETRANGE,(WPARAM)TRUE,(LPARAM)MAKELONG(40,100));
       SendMessage(button, TBM_SETPAGESIZE,0,(LPARAM)1);
       SendMessage(button, TBM_SETSEL,(WPARAM)TRUE,(LPARAM)MAKELONG(40,100));
       SendMessage(button, TBM_SETPOS,(WPARAM)TRUE,(LPARAM)100);
       ShowWindow(button,SW_HIDE);

        button=CreateWindowEx(0,
                              "EDIT",
                               "100%",
                               ES_CENTER|WS_CHILD|WS_VISIBLE|ES_READONLY,
                               X_TRACKBAR_EDIT,
                               22,
                               TRACKBAR_EDIT_WD,
                               23,
                               hwnd,
                               (HMENU)IDM_TRACKBAR_TEXT,
                               hinst,
                               NULL);
       SendDlgItemMessage(hwnd,IDM_TRACKBAR_TEXT,WM_SETFONT,(WPARAM)(hfont),TRUE);
       ShowWindow(button,SW_HIDE);
       button=CreateWindowEx(0,"BUTTON",
                            "Ok",
                            WS_CHILD|BS_PUSHBUTTON|WS_VISIBLE,
                            X_TRACKBAR_OK,
                            22,
                            30,
                            23,
                            hwnd,
                            (HMENU)IDM_TRACKBAR_OK,
                            hinst,
                            NULL);
      SendDlgItemMessage(hwnd,IDM_TRACKBAR_OK,WM_SETFONT,(WPARAM)(hfont),TRUE);
      ShowWindow(button,SW_HIDE);
      }
      {/**<State bar*/
        statusbar=CreateWindowEx(0,STATUSCLASSNAME,NULL,WS_CHILD|WS_VISIBLE|SBARS_SIZEGRIP,0,0,STATUS_WIDTH,STATUS_HEIGHT,hwnd,(HMENU)ID_STATUSBAR,hinst,NULL);
        if(!statusbar)
        {
            showMessageError(hwnd,"\tWe are sorry!.\nError occurs while initialising state bar.\n      Please reload the application.");
            return 0;
        }
        size_change=TRUE;
        update_status_bar(statusbar,hwnd);
      }
      {/**<Set Menu*/

        d_menu=CreateMenu();
        d_s_menu=CreateMenu();
        sub_menu=CreatePopupMenu();
        sub_menu2 = CreatePopupMenu();
       {//File
            AppendMenu(d_s_menu,MF_STRING,IDM_NEW_FILE,"&New File\tCtrl+N");
            AppendMenu(d_s_menu,MF_SEPARATOR,0,0);
            AppendMenu(d_s_menu,MF_STRING,IDM_LIRE,"&Open File...\tCtrl+0");
            AppendMenu(d_s_menu,MF_SEPARATOR,0,0);
          {//Sub menu for recent files
            int i;
            char text[7]={'\0','\0','\0','\0','\0','\0','\0'};
            for(i=0;i<10;i++)
            {
                strcpy(text,"file ");
                text[strlen(text)]=(char)(48+i);
                AppendMenu(sub_menu,MF_STRING|MF_DISABLED,IDM_RECENT_FILE+i,text);
                text[strlen(text)-1]='\0';
            }
                      /***/
            }
            AppendMenu(d_s_menu,MF_STRING|MF_POPUP,(UINT_PTR)sub_menu,"&Recent files");
            AppendMenu(d_s_menu,MF_STRING|MF_DISABLED,IDM_SAVE,"&Save Changes\tCtrl+S");
            AppendMenu(d_s_menu,MF_SEPARATOR,0,0);
            AppendMenu(d_s_menu,MF_STRING|MF_DISABLED,IDM_SAVE_AS,"Save &As...\tCtrl+Shift+S");
            AppendMenu(d_s_menu,MF_STRING,IDM_CLOSE,"&Close\t Ctrl+Shift+C");
            AppendMenu(d_s_menu,MF_SEPARATOR,0,0);
            AppendMenu(d_s_menu,MF_STRING,IDM_RENAME,"Rename &File");
            AppendMenu(d_s_menu,MF_STRING,IDM_QUIT,"&Exit\tAlt+F4");
            AppendMenu(d_menu,MF_POPUP,PtrToUint(d_s_menu),"&File");
       }
        d_s_menu=CreateMenu();
       {//Appearence
            AppendMenu(d_s_menu,MF_STRING,IDM_FONT,"&Font...");
          {/**<Sub menu for color*/
            sub_menu=CreatePopupMenu();
            AppendMenu(sub_menu,MF_STRING,IDM_TXT_COLOR,"&Text Color...");
            AppendMenu(sub_menu,MF_STRING,IDM_BK_COLOR,"&First Background Color...");
            sub_menu2 = CreatePopupMenu();
            {/**<Sub Menu of second background color*/
                AppendMenu(sub_menu2,MF_STRING,IDM_BCK_S_COLOR,"Select &Color...");
                AppendMenu(sub_menu2,MF_STRING,IDM_BCK_S_IMAGE,"Select &Image...");
                AppendMenu(sub_menu2,MF_STRING,IDM_HIDE_BCK_SECOND,"&Disable");
            }
            AppendMenu(sub_menu,MF_STRING|MF_POPUP,(UINT_PTR)sub_menu2,"&Second Background Color");
            AppendMenu(sub_menu,MF_STRING,IDM_RESET_COLOR,"Default colo&rs");
          }
            AppendMenu(d_s_menu,MF_STRING|MF_POPUP,(UINT_PTR)sub_menu,"&Colors");
            AppendMenu(d_s_menu,MF_STRING,IDM_LAYERED,"&Opacity...");
            AppendMenu(d_s_menu,MF_SEPARATOR,0,0);
            AppendMenu(d_s_menu,MF_STRING,IDM_S_STATE_BAR,"&Hide State bar");
            AppendMenu(d_s_menu,MF_STRING,IDM_ONTOP,"Always on To&p\tCtrl+Shift+T");
            AppendMenu(d_s_menu,MF_STRING,IDM_HIDE_WINDOW,"H&ide Window to taskbar\tCtrl+T");
            AppendMenu(d_menu,MF_POPUP,PtrToUint(d_s_menu),"&Appearence");
       }
        d_s_menu=CreateMenu();
        AppendMenu(d_s_menu,MF_STRING|MF_DISABLED,IDM_EDUNDO,"Undo\tCtrl+Z");
        AppendMenu(d_s_menu,MF_SEPARATOR,0,0);
        AppendMenu(d_s_menu,MF_STRING,IDM_SELECT_ALL,"Select All\tCtrl+A");
        AppendMenu(d_s_menu,MF_STRING|MF_DISABLED,IDM_EDCUT,"Cut\tCtrl+X");
        AppendMenu(d_s_menu,MF_STRING|MF_DISABLED,IDM_EDCOPY,"Copy\tCtrl+C");
        AppendMenu(d_s_menu,MF_STRING,IDM_EDPASTE,"Paste\tCtrl+V");
        AppendMenu(d_s_menu,MF_STRING|MF_DISABLED,IDM_EDDEL,"Delete\tDel");
        AppendMenu(d_s_menu,MF_SEPARATOR,0,0);
        if(fileExist(getMutexLocation()))
            AppendMenu(d_s_menu,MF_CHECKED,IDM_MUTEX,"Only One instance");
        else
            AppendMenu(d_s_menu,MF_UNCHECKED,IDM_MUTEX,"Only One instance");
        AppendMenu(d_s_menu,MF_STRING|MF_DISABLED,IDM_SEARCH,"&Find");
        AppendMenu(d_s_menu,MF_STRING,IDM_GO_TO,"&Go to...");
        sub_menu=CreateMenu( );
        {/**<Sub menu of Zoom*/
        AppendMenu(sub_menu,MF_STRING,IDM_ZOOM_25,"25%");
        AppendMenu(sub_menu,MF_STRING,IDM_ZOOM_50,"50%");
        AppendMenu(sub_menu,MF_STRING,IDM_ZOOM_75,"75%");
        AppendMenu(sub_menu,MF_STRING,IDM_ZOOM_100,"100%");
        AppendMenu(sub_menu,MF_STRING,IDM_ZOOM_125,"125%");
        AppendMenu(sub_menu,MF_STRING,IDM_ZOOM_150,"150%");
        AppendMenu(sub_menu,MF_STRING,IDM_ZOOM_200,"200%");
        }
        AppendMenu(d_s_menu,MF_STRING|MF_POPUP,(UINT_PTR)sub_menu,"&Zoom");
        AppendMenu(d_menu,MF_POPUP,PtrToUint(d_s_menu),"&Edit");

        d_s_menu=CreateMenu();
        AppendMenu(d_s_menu,MF_STRING,IDM_COMP_GCC,"&run(with gcc)\tCtrl+Shift+R");
        AppendMenu(d_s_menu,MF_STRING,IDM_TEST,"Start");
        AppendMenu(d_s_menu,MF_STRING,IDM_SHOW_CONSOLE,"Show C&onsole");
        AppendMenu(d_menu,MF_POPUP,PtrToUint(d_s_menu),"&Development");

        d_s_menu=CreateMenu();
        AppendMenu(d_s_menu,MF_STRING,IDM_VIEW,"&Take ScreenShot...\tAlt+V");
        AppendMenu(d_s_menu,MF_STRING,IDM_ABOUT,"&About\tF2");
        AppendMenu(d_s_menu,MF_SEPARATOR,0,0);
        AppendMenu(d_s_menu,MF_STRING,IDM_CONTACT_US,"Contact Us...");
        AppendMenu(d_menu,MF_POPUP,PtrToUint(d_s_menu),"&Other");

        AppendMenu(d_menu,MF_SEPARATOR,0,0);
        AppendMenu(d_menu,MF_HELP|MF_DISABLED,0,"My Script");
        CheckMenuRadioItem(d_menu,IDM_ZOOM_25,IDM_ZOOM_200,IDM_ZOOM_100,MF_BYCOMMAND);
        SetMenu(hwnd,d_menu);
        UpdateWindow(hwnd);
        SetFocus(GetDlgItem(hwnd,IDM_TEXT));
       }
      {/**<Lecture des fichiers passes en commande s'ils existent*/
       /**<Parse Command line argument*/
         char** cline=NULL;
         int c_nb=0;
         cline=(char**)CommandLineToArgvW((LPCWSTR)*commande_line,&c_nb);
          /**/
          if(strlen(cline[0]) > 3)
          {
              HCURSOR prevouis_Cursor=GetCursor();
              SetCursor(LoadCursor(NULL,IDC_WAIT));
              parse(hwnd,cline,caption,Setfile);
              SetCursor(prevouis_Cursor);
          }
          c_nb--;
         while(c_nb >= 0)
        {
            GlobalFree(cline[c_nb]);
            c_nb--;
        }
        GlobalFree(cline);
       }
      /**<cas ou la derniere session s'est terminee brusquement*/
       last_session(hwnd,Setfile);
      /**<Set default Opacity of windows*/
       SetLayeredWindowAttributes(hwnd,0,Info.opacity*255/100,LWA_ALPHA);
       if(Info.state_bar==TRUE)show_state_bar(hwnd,&S_State_bar);
       else hide_state_bar(hwnd,&S_State_bar); //S_State_bar==Info.state_bar
       return 0;
    }
   /*Gestion de l'auto hide de la status bar
    case WM_MOUSEMOVE:
    {
        int yPos = (int)HIWORD(lParam);  // vertical position of cursor
        if(  (yPos >= ((int)height - 30)) && (yPos <(int)height))
        {
             show_state_bar(hwnd,&S_State_bar);
        }else{
             hide_state_bar(hwnd,&S_State_bar);
        }
        break;
    }*/
    case WM_CLOSE:
    {
       if(change==TRUE)
            {
                int result=MessageBox(hwnd,"Save changes?","Notification",MB_YESNOCANCEL|MB_ICONINFORMATION);
                switch(result)
                {
                case IDYES:
                    save(hwnd);
                    DeleteObject(hfont);
                    KillTimer(hwnd,ID_TIMER);
                    DestroyWindow(hwnd);
                    break;
                case IDNO:
                    DeleteObject(hfont);
                    KillTimer(hwnd,ID_TIMER);
                    DestroyWindow(hwnd);
                    break;
                default:
                    break;
                }
            }
            else
            {
                DeleteObject(hfont);
                KillTimer(hwnd,ID_TIMER);
                DestroyWindow(hwnd);
            }
        break;
    }
    case MY_WM_NOTIFYICON:
    {
        if(lParam == WM_LBUTTONUP)
        {
            ShowWindow(hwnd,SW_SHOW);
            Shell_NotifyIcon(NIM_DELETE,&TrayIcon);
        }
        if(lParam == WM_RBUTTONUP)
        {
            GlobalFree(paths);
            HMENU hmenu;
            HMENU hpopup;
            POINT pos;
            GetCursorPos(&pos);
            hmenu=LoadMenu(hinst,"IDR_MENU");
            hpopup=GetSubMenu(hmenu,0);
            SetForegroundWindow(hwnd);
            TrackPopupMenuEx(hpopup,0,pos.x,pos.y,hwnd,NULL);
            DestroyMenu(hmenu);
        }
        return 0;
    }
    case WM_HSCROLL:
    {
      if((HWND)lParam == GetDlgItem(hwnd,ID_TRACKBAR))
      {
        if((LOWORD(wParam)==TB_ENDTRACK) || (LOWORD(wParam)==TB_THUMBTRACK))
        {
            SendMessage((HWND)lParam,TBM_CLEARSEL,TRUE,0);
            trackbar_pos=SendMessage((HWND)lParam, TBM_GETPOS, 0, 0);
            SendMessage((HWND)lParam, TBM_SETSEL,(WPARAM)TRUE,(LPARAM)MAKELONG(30,trackbar_pos));
            char tmp[5];
            sprintf(tmp,"%d%%",trackbar_pos);
            SendMessage(GetDlgItem(hwnd,IDM_TRACKBAR_TEXT),WM_SETTEXT,0,(LPARAM)tmp);
            SetLayeredWindowAttributes(hwnd,0,trackbar_pos*255/100,LWA_ALPHA);
        }
       }
        break;
    }
    case WM_COMMAND:
    {
     if(SendMessage(GetDlgItem(hwnd,IDM_TEXT),EM_GETMODIFY,0,0) != 0)
      {
                change=TRUE;
                SendMessage(GetDlgItem(hwnd,IDM_TEXT),EM_SETMODIFY,FALSE,0);
                EnableWindow(GetDlgItem(hwnd,IDM_SAVE),TRUE);//Reactivation du button "save"
                if(Setfile[1]==TRUE){EnableWindow(GetDlgItem(hwnd,IDCANCEL),TRUE);}//Reactivation du button "Cancel"
                ModifyMenu(d_menu,IDM_SAVE,MF_BYCOMMAND|MF_ENABLED,IDM_SAVE,"&Save Changes\tCtrl+S"); //Reactivation de "save changes" dans les menus
                ModifyMenu(d_menu,IDM_SAVE_AS,MF_BYCOMMAND|MF_ENABLED,IDM_SAVE_AS,"Save &As...\tCtrl+Shift+S"); //Reactivation de "save As" dans les menus

                if(Setfile[0]==FALSE)
                {
                    SetDlgItemTextA(hwnd,IDM_FILENAME,">New file*");
                }
                else
                {
                    if(caption[(int)strlen(caption) - 1] != '*')
                    {
                        strcat(caption,"*\0");
                        SendMessage(hwnd,WM_SETTEXT,0,(LPARAM)(LPSTR)caption);
                        SendMessage(GetDlgItem(hwnd,IDM_FILENAME),WM_SETTEXT,0,(LPARAM)(LPSTR)caption);
                    }
                }
        }
     switch(LOWORD(wParam))
      {
      case IDM_ONTOP:
        {
            SetWindowPos(hwnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
            ModifyMenu(GetMenu(hwnd),IDM_ONTOP,MF_STRING|MF_CHECKED,IDM_NOTOP,"Always on top");
            break;

        }
        case IDM_NOTOP:
        {
            SetWindowPos(hwnd,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
            ModifyMenu(GetMenu(hwnd),IDM_NOTOP,MF_STRING|MF_UNCHECKED,IDM_ONTOP,"Always on top");
            break;

        }
      case IDM_MUTEX:
        {
            if(fileExist(getMutexLocation()))
            {
                DeleteFile(getMutexLocation());
                CheckMenuItem(GetMenu(hwnd),IDM_MUTEX,MF_UNCHECKED);
            }
            else
            {
                FILE* f = fopen(getMutexLocation(),"a");
                fclose(f);
                CheckMenuItem(GetMenu(hwnd),IDM_MUTEX,MF_CHECKED);
            }
            break;

        }
       case IDM_RENAME:
        {
            DialogBox(hinst,(LPCTSTR)IDD_RENAME_DIALOG,hwnd,(DLGPROC)renameProc);
            break;
        }
        case IDM_TEST:
           {
               RECT tmprect;
       GetClientRect(hwnd,&tmprect);
       tmprect.bottom-=100;
      tmprect.left+=200;
      tmprect.right=tmprect.left+30;
      tmprect.top=tmprect.bottom-30;
      DrawFrameControl(GetDC(hwnd),&tmprect,DFC_BUTTON,DFCS_BUTTONPUSH|DFCS_ADJUSTRECT);


                /**<Test directory changes*/
            /**
            note=FindFirstChangeNotification("C:\\Users\\Isaac\\Desktop\\new",TRUE,FILE_NOTIFY_CHANGE_FILE_NAME);
            if(note!=INVALID_HANDLE_VALUE)
            {
                MessageBox(NULL,"***Wactching Notification:ok****","Note",MB_OK|MB_ICONINFORMATION);
            }
            note_state=WaitForSingleObject(note,INFINITE);
            if(note_state==WAIT_OBJECT_0)
            {
                MessageBox(NULL,"***Wactching Notification:change detected****","Note",MB_OK|MB_ICONINFORMATION);
            }
            else if(note_state==WAIT_TIMEOUT){}
            else{}
            FindCloseChangeNotification(note);
            */

            break;
           }
        case IDM_SELECT_ALL:
        {
          SendDlgItemMessage(hwnd,IDM_TEXT,EM_SETSEL,0,-1); //selectionner tout le texte dans le control edit.
          break;
        }
        case IDM_EDUNDO:
        {
            SendMessage(GetDlgItem(hwnd,IDM_TEXT),WM_UNDO,0,0);
            break;
        }
        case IDM_EDPASTE:
        {
            SendMessage(GetDlgItem(hwnd,IDM_TEXT),WM_PASTE,0,0);
            break;
        }
        case IDM_EDCUT:
        {
            SendMessage(GetDlgItem(hwnd,IDM_TEXT),WM_CUT,0,0);
            break;
        }
        case IDM_EDCOPY:
        {
            SendMessage(GetDlgItem(hwnd,IDM_TEXT),WM_COPY,0,0);
            break;
        }
        case IDM_EDDEL:
        {
            SendMessage(GetDlgItem(hwnd,IDM_TEXT),WM_CLEAR,0,0);
            break;
        }
        case IDM_TRACKBAR_OK:
        {
            ShowWindow(GetDlgItem(hwnd,IDM_TRACKBAR_OK),SW_HIDE);
            ShowWindow(GetDlgItem(hwnd,ID_TRACKBAR),SW_HIDE);
            ShowWindow(GetDlgItem(hwnd,IDM_TRACKBAR_TEXT),SW_HIDE);
            break;
        }
        case IDM_LAYERED:
        {
            ShowWindow(GetDlgItem(hwnd,IDM_TRACKBAR_OK),SW_SHOW);
            ShowWindow(GetDlgItem(hwnd,IDM_TRACKBAR_TEXT),SW_SHOW);
            temp=GetDlgItem(hwnd,ID_TRACKBAR);
            ShowWindow(temp,SW_SHOW);
            SetFocus(temp);
            break;
        }
        case IDM_RESET_COLOR:
        {
            (&default_color)->text_color_idm_text = DEFAULT_COLOR;
            (&default_color)->bck_color_idm_text = DEFAULT_BKCOLOR;
            (&default_color)->bck_color_s_idm_text = DEFAULT_BKCOLOR_S;
            DeleteObject(he);
            he=CreateSolidBrush((default_color.bck_color_s_idm_text));
            SetFocus(NULL);
            SetFocus(GetDlgItem(hwnd,IDM_TEXT));
            break;
        }
        case IDM_HIDE_BCK_SECOND:
        {
            DeleteObject(he);
            he=CreateSolidBrush((default_color.bck_color_idm_text));
            SetFocus(NULL);
            SetFocus(GetDlgItem(hwnd,IDM_TEXT));
            ModifyMenu(GetMenu(hwnd),IDM_HIDE_BCK_SECOND,MF_STRING,IDM_SHOW_BCK_SECOND,"&Enable");
            EnableMenuItem(GetMenu(hwnd),IDM_BCK_S_COLOR,MF_DISABLED);
            EnableMenuItem(GetMenu(hwnd),IDM_BCK_S_IMAGE,MF_DISABLED);
            break;
        }
        case IDM_BCK_S_IMAGE:
        {
            if((fname = getFileName(hwnd)) != NULL )
            {
                DeleteObject(he);
                he=CreatePatternBrush(LoadBitmap(NULL,(LPCSTR)fname));
                GlobalFree(fname);
                SetFocus(GetDlgItem(hwnd,IDM_TEXT));
            }
            break;
        }
        case IDM_SHOW_BCK_SECOND:
        {
            DeleteObject(he);
            he=CreateSolidBrush((default_color.bck_color_s_idm_text));
            SetFocus(NULL);
            SetFocus(GetDlgItem(hwnd,IDM_TEXT));
            ModifyMenu(GetMenu(hwnd),IDM_SHOW_BCK_SECOND,MF_STRING,IDM_HIDE_BCK_SECOND,"&Disable");
            EnableMenuItem(GetMenu(hwnd),IDM_BCK_S_COLOR,MF_ENABLED);
            EnableMenuItem(GetMenu(hwnd),IDM_BCK_S_IMAGE,MF_ENABLED);
            break;
        }
        case IDM_DELETE_LINE:
        {
            temp=GetDlgItem(hwnd,IDM_TEXT);
            int line=SendMessage(temp,EM_LINEFROMCHAR,-1,0);;
            int pre=SendMessage(temp,EM_LINEINDEX,line,0);
            int next=SendMessage(temp,EM_LINEINDEX,line+1,0);
            SendMessage(temp,EM_SETSEL,(WPARAM)(pre),(LPARAM)(next));
            SendMessage(temp,EM_REPLACESEL,(WPARAM)TRUE,(LPARAM)" ");
            change =true;
        }
        case IDM_COPY_LINE:
        {
            temp=GetDlgItem(hwnd,IDM_TEXT);
            int previous=SendMessage(temp,EM_LINEINDEX,-1,0);//get position of beginning of the current line
            int l=SendMessage(temp,EM_LINEFROMCHAR,-1,0);
            int pos=SendMessage(temp,EM_LINEINDEX,l,0);
            int len=SendMessage(temp,EM_LINELENGTH,(WPARAM)pos,0);
            LPSTR line=(LPSTR)LocalAlloc(GPTR,(len+3)*sizeof(char));
            sprintf(line,"%d",(len));
            SendMessage(temp,EM_GETLINE,(WPARAM)l,(LPARAM)line);
            strcat(line,"\r\r\n");//Insert soft-linebreak characters
            pos=SendMessage(temp,EM_LINEINDEX,(l+1),0);
            SendMessage(temp,EM_SETSEL,(WPARAM)(previous),(LPARAM)(previous));
            SendMessage(temp,EM_REPLACESEL,(WPARAM)TRUE,(LPARAM)line);
            GlobalFree(line);
            change=TRUE;
            break;
        }
        case IDM_HIDE_WINDOW:
        {
            ShowWindow(hwnd,SW_HIDE);
            TrayIcon.cbSize=sizeof(NOTIFYICONDATA);
            TrayIcon.hWnd=hwnd;
            TrayIcon.uID=0;
            TrayIcon.hIcon=LoadIcon(hinst,MAKEINTRESOURCE(SCP_ICON));
            //TrayIcon.hIcon=CreateSmallIcon(hwnd);
            TrayIcon.uCallbackMessage = MY_WM_NOTIFYICON;
            TrayIcon.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP;
            if(Setfile[0]==TRUE)strcpy(TrayIcon.szTip,caption);
            else strcpy(TrayIcon.szTip,"Text Editor");
            Shell_NotifyIcon(NIM_ADD,&TrayIcon);
            Shell_NotifyIcon(NIM_MODIFY,&TrayIcon);
            FreeConsole();
            break;
        }
        case IDM_HIDE_CONSOLE:
        {
           ShowWindow(GetConsoleWindow(),SW_HIDE);

           ModifyMenu(GetMenu(hwnd),IDM_HIDE_CONSOLE,MF_STRING,IDM_SHOW_CONSOLE,"Show C&onsole");
           break;
        }
        case IDM_SHOW_CONSOLE:
        {
           ShowWindow(GetConsoleWindow(),SW_SHOW);

           ModifyMenu(GetMenu(hwnd),IDM_SHOW_CONSOLE,MF_STRING,IDM_HIDE_CONSOLE,"Hide C&onsole");
           break;
        }
        case IDM_GO_TO:
        {
            int result=DialogBox(hinst,(LPCTSTR)IDD_GO_TO,hwnd,(DLGPROC)GotoProc);
            goto_function(hwnd,result);
            break;
        }
        case IDM_SEARCH:/**<Incomplet*/
        {
            ZeroMemory(&fr, sizeof(FINDREPLACE));
            fr.lStructSize = sizeof(FINDREPLACE);
            fr.hInstance=hinst;
            fr.hwndOwner = hwnd;
            fr.lpstrFindWhat = szFindWhat;
            fr.wFindWhatLen = 100;
            fr.Flags = 0;

            hdlg=FindText(&fr);
            SetForegroundWindow(hdlg);
            break;
        }
        case IDM_CAPTION:
        {
            temp=GetDlgItem(hwnd,IDM_CAPTION);
            if(HIWORD(wParam)==CBN_DROPDOWN)
            {
                int len=0,m=0,nb=0;
                nb=SendMessage(temp,CB_GETCOUNT,0,0);
                if (nb!=CB_ERR)
                {
                    nb--;
                   while(nb>=0)
                   {
                      len=SendMessage(temp,CB_GETLBTEXTLEN,nb,0);
                      if(len > m) m=len;
                      nb--;
                   }
                }
                MoveWindow(temp,0,0,m*10,10,1);
                ShowWindow(temp,SW_SHOW);
            }
            else if(HIWORD(wParam)==CBN_CLOSEUP)
            {
                MoveWindow(temp,0,-5,20,10,1);
                ShowWindow(temp,SW_SHOW);
            }
            else if(HIWORD(wParam)==CBN_SELENDOK)
            {
                int i =SendMessage(temp,CB_GETCURSEL,0,0);
                char tmp[_MAX_PATH];
                SendMessage(temp,CB_GETLBTEXT,(WPARAM)0,(LPARAM)tmp);
                if(strcmp(caption,tmp)!=0)
                {
                   if(change==FALSE){
                                        if(read_drop_file(hwnd,Setfile,tmp))strcpy(caption,tmp);
                                    }
                   else{
                        if(MessageBox(hwnd,"Do you want to save changes?","Note",MB_YESNO|MB_ICONQUESTION) == IDYES) save(hwnd);
                         change=FALSE;
                         if(read_drop_file(hwnd,Setfile,tmp))strcpy(caption,tmp);
                        }
                }
            }
            else{}
           break;
        }
        case IDM_CONTACT_US:
        {
          ShellExecute(hwnd,"open","mailto:0.my.script.1@gmail.com?subject=Text Editor:Report&body=write body email here",NULL,NULL,SW_SHOWNORMAL);
          break;
        }
        case IDM_S_STATE_BAR:
        {
        SetFocus(GetDlgItem(hwnd,IDM_TEXT));
         if(S_State_bar==TRUE)
         {
            hide_state_bar(hwnd,&S_State_bar);
         }
         else{show_state_bar(hwnd,&S_State_bar);}
         break;
        }
        case IDM_CLOSE:
        {
        int confirm =1;
        if(Setfile[0]==TRUE)
         {
            if(change==TRUE)
            {
                int result=MessageBox(hwnd,"Save changes?","Notification",MB_YESNOCANCEL|MB_ICONINFORMATION);
                switch(result)
                {
                case IDYES:
                    save(hwnd);
                    break;
                case IDNO:
                    break;
                default:
                    confirm = 0;
                    break;
                }
            }
            if(confirm==1)
            {
                int i = remove_from_recent_file(hwnd,NULL);
                //SendMessage(GetDlgItem(hwnd,IDM_CAPTION),CB_GETCURSEL,0,0);
                if(i >= 0)
                {
                    char tmp[MAX_PATH];
                    SendMessage(GetDlgItem(hwnd,IDM_CAPTION),CB_GETLBTEXT,(WPARAM)i,(LPARAM)tmp);
                    if(read_drop_file(hwnd,Setfile,tmp))strcpy(caption,tmp);
                }
                else {
                    initialise(hwnd,&change,Setfile,&d_menu);
                }
            }
        }
        break;
        }
        case IDM_RESTORE:
        {
                ShowWindow(hwnd,SW_SHOW);
                Shell_NotifyIcon(NIM_DELETE,&TrayIcon);
                break;
        }
        case IDM_QUIT:
        {
            if(change==TRUE)
            {
                int tmp_quit=MessageBox(hwnd,"Save changes?","Notification",MB_YESNOCANCEL|MB_ICONINFORMATION);
                switch(tmp_quit)
                {
                case IDYES:
                    save(hwnd);
                    DeleteObject(hfont);
                    KillTimer(hwnd,ID_TIMER);
                    DestroyWindow(hwnd);
                    break;
                case IDNO:
                    DeleteObject(hfont);
                    KillTimer(hwnd,ID_TIMER);
                    DestroyWindow(hwnd);
                    break;
                default:
                    break;
                }
            }
            else
            {
                DeleteObject(hfont);
                KillTimer(hwnd,ID_TIMER);
                DestroyWindow(hwnd);
            }
            break;
        }
        case IDM_NEW_FILE:
        {
            strcpy(caption,">New file*");
            SetDlgItemText(hwnd,IDM_FILENAME,caption);
            SendMessage(hwnd,WM_SETTEXT,0,(LPARAM)(LPSTR)caption);
            change=TRUE;
            SetDlgItemText(hwnd,IDM_TEXT,"");
            SendMessage(GetDlgItem(hwnd,IDM_TEXT),EM_SETMODIFY,FALSE,0);
            EnableWindow(GetDlgItem(hwnd,IDM_SAVE),TRUE);//Reactivation du button "save"
            ModifyMenu(d_menu,IDM_SAVE,MF_BYCOMMAND|MF_ENABLED,IDM_SAVE,"&Save Changes"); //Reactivation de "save changes" dans les menus
            ModifyMenu(d_menu,IDM_SAVE_AS,MF_BYCOMMAND|MF_ENABLED,IDM_SAVE_AS,"Save &As..."); //Reactivation de "save As" dans les menus
            Setfile[0]=TRUE;
            SetFocus(GetDlgItem(hwnd,IDM_TEXT));
            SendDlgItemMessage(hwnd,IDM_CAPTION,CB_SETCURSEL,-1,0);
            break;
        }
        case IDM_ABOUT:
        {
            DialogBox(hinst,(LPCTSTR)IDD_ABOUT,hwnd,(DLGPROC)DialProcAbout);
            break;
        }
        case IDM_LIRE:
        {
            if(change==TRUE)
            {
                switch(MessageBox(hwnd,"Save changes?","Notification",MB_YESNOCANCEL|MB_ICONINFORMATION))
                {
                case IDYES:
                    save_as(hwnd);
                    read(hwnd,Setfile);
                    break;
                case IDNO:
                    read(hwnd,Setfile);
                    break;
                default:
                    return TRUE;
                    break;
                }
                return TRUE;
            }
            read(hwnd,Setfile);
            change=FALSE;
            break;
        }
        case IDM_SAVE_AS:
        {
            if(save_as(hwnd))
            {
                change=FALSE;
                Setfile[0]=TRUE;
            }
            else change=TRUE;
            break;
        }
        case IDM_SAVE:
        {
            if(save(hwnd))
            {
                change=FALSE;
                Setfile[0]=TRUE;
            }else change=TRUE;
            break;
        }
        case IDCANCEL:
        {
           if(MessageBox(hwnd,"Do you want to cancel all changes?","Note",MB_YESNO|MB_ICONWARNING)== IDYES )
           {
               if(backup_file(caption,RESTORE_FILE))
               {
                   read_drop_file(hwnd,Setfile,caption);
               }
               change=FALSE;
           }
           return TRUE;
        }
        {/**<Zoom option*/
        case IDM_ZOOM_25:
        {
                modify_size(hwnd,FALSE,25);
                CheckMenuRadioItem(GetMenu(hwnd),IDM_ZOOM_25,IDM_ZOOM_200,IDM_ZOOM_25,MF_BYCOMMAND);
                break;
        }
        case IDM_ZOOM_50:
        {
                modify_size(hwnd,FALSE,50);
                CheckMenuRadioItem(GetMenu(hwnd),IDM_ZOOM_25,IDM_ZOOM_200,IDM_ZOOM_50,MF_BYCOMMAND);
                break;
        }
        case IDM_ZOOM_75:
        {
                modify_size(hwnd,FALSE,75);
                CheckMenuRadioItem(GetMenu(hwnd),IDM_ZOOM_25,IDM_ZOOM_200,IDM_ZOOM_75,MF_BYCOMMAND);
                break;
        }
        case IDM_ZOOM_100:
        {
                modify_size(hwnd,TRUE,100);
                CheckMenuRadioItem(GetMenu(hwnd),IDM_ZOOM_25,IDM_ZOOM_200,IDM_ZOOM_100,MF_BYCOMMAND);
                break;
        }
        case IDM_ZOOM_125:
        {
                modify_size(hwnd,FALSE,125);
                CheckMenuRadioItem(GetMenu(hwnd),IDM_ZOOM_25,IDM_ZOOM_200,IDM_ZOOM_125,MF_BYCOMMAND);
                break;
        }case IDM_ZOOM_150:
        {
                modify_size(hwnd,FALSE,150);
                CheckMenuRadioItem(GetMenu(hwnd),IDM_ZOOM_25,IDM_ZOOM_200,IDM_ZOOM_150,MF_BYCOMMAND);
                break;
        }
        case IDM_ZOOM_200:
        {
                modify_size(hwnd,FALSE,200);
                CheckMenuRadioItem(GetMenu(hwnd),IDM_ZOOM_25,IDM_ZOOM_200,IDM_ZOOM_200,MF_BYCOMMAND);
                break;
        }
        }
        case IDM_FONT:/**<Change font*/
        {
            if(setfont(hwnd,&hfont,&default_color.text_color_idm_text)==TRUE)
            {
                SendDlgItemMessage(hwnd,IDM_TEXT,WM_SETFONT,(WPARAM)(hfont),TRUE);
                //SendDlgItemMessage(hwnd,IDM_FILENAME,WM_SETFONT,(WPARAM)(hfont),TRUE);
                SendDlgItemMessage(hwnd,IDM_LIRE,WM_SETFONT,(WPARAM)(hfont),TRUE);
                SendDlgItemMessage(hwnd,IDM_SAVE,WM_SETFONT,(WPARAM)(hfont),TRUE);
                SendDlgItemMessage(hwnd,IDCANCEL,WM_SETFONT,(WPARAM)(hfont),TRUE);
                SendDlgItemMessage(hwnd,IDM_CAPTION,WM_SETFONT,(WPARAM)(hfont),TRUE);
                UpdateWindow(hwnd);
            }
            break;
        }
        case IDM_BK_COLOR:/**<Change text color and background colors*/
        {
            set_bkcolor(hwnd,&(default_color.bck_color_idm_text));
            SetFocus(GetDlgItem(hwnd,IDM_TEXT));
            break;
        }
        case IDM_TXT_COLOR:
        {
            set_bkcolor(hwnd,&(default_color.text_color_idm_text));
            SetFocus(GetDlgItem(hwnd,IDM_TEXT));
            break;
        }
        case IDM_BCK_S_COLOR:
        {
            set_bkcolor(hwnd,&(default_color.bck_color_s_idm_text));
            DeleteObject(he);
            he=CreateSolidBrush(default_color.bck_color_s_idm_text);
            SetFocus(GetDlgItem(hwnd,IDM_TEXT));
            break;
        }
        case IDM_VIEW:
        {
            saveBitmap(hwnd,hinst);
            if(MessageBox(hwnd,"Do you want display the screenshot now?","Note",MB_YESNO|MB_ICONQUESTION) == IDYES)
            {
                //DialogBox(hinst,(LPCTSTR)MAKEINTRESOURCE(IDD_VIEW),NULL,(DLGPROC)DialView);
                showMessageError(hwnd,"***Partie non developpe*****");
            }
            break;
        }
        case IDM_COMP_GCC:
        {
            if(change==TRUE)
            {
                if(!save(hwnd)) return TRUE;
                else{
                    change=FALSE;
                    Setfile[0]=TRUE;
                }
            }
            strcpy(commande,"gcc -o test.exe  ");
            system("color 02");
            strcat(commande,caption);
            system(commande);
            if(fileExist("test.exe"))
            {
                       ZeroMemory(&info,sizeof(info));
                       info.cbSize=sizeof(info);
                       info.hwnd=NULL;
                       info.nShow=SW_SHOWNORMAL;
                       info.fMask=SEE_MASK_NOCLOSEPROCESS;
                       info.lpVerb=NULL;
                       info.lpFile=".\\test.exe";
                       if (ShellExecuteExA(&info))
                            {
                                WaitForSingleObject(info.hProcess,INFINITE);
                                system("del test.exe");
                            }
            }else{
                            ShowWindow(GetConsoleWindow(),SW_SHOW);
                            ModifyMenu(GetMenu(hwnd),IDM_SHOW_CONSOLE,MF_STRING,IDM_HIDE_CONSOLE,"Hide C&onsole");
                 }

             break;
        }
        default:
            break;
        }
        break;
    }
    case WM_DROPFILES:
    {

        UINT i=0;
        UINT nb=GetDroppedFilesPaths((HDROP)wParam,paths); //nombre de fichiers dropper
        switch(nb)
        {
        case UNKNOWN_ERROR:
            showMessageError(hwnd,"Something were wrong");
            break;
        case ERROR_MEMORY:
            showMessageError(hwnd,"Memory access denied");
            break;
        default:
            if(change==TRUE)
            {
                switch(MessageBox(hwnd,"Save changes?","Notification",MB_YESNOCANCEL|MB_ICONINFORMATION))
                {
                case IDYES :
                    save(hwnd);
                    break;
                case IDCANCEL:
                    for(i=0;i<nb;i++){GlobalFree(*paths[i]);}
                    GlobalFree(*paths);
                    /****Terminated drag process****/
                    DragFinish((HDROP)wParam);
                    return TRUE;
                    break;
                default:
                    break;
                }
            }
            for(i=1;i<nb;i++){
              update_recent_file(hwnd,paths[0][i]);
            }
            read_drop_file(hwnd,Setfile,paths[0][0]);
            strcpy(caption,paths[0][0]);
          /***Liberation de memoire***/
            for(i=0;i<nb;i++){GlobalFree(paths[0][i]);}
             GlobalFree(paths[0]);
             UpdateWindow(hwnd);
            break;
        }

        /****Terminated drag process****/
        DragFinish((HDROP)wParam);
        return TRUE;
        break;
    }
    case WM_DESTROY:
    {
        DestroyMenu(GetMenu(hwnd));
        DeleteObject(hbdialog);
        DeleteObject(he);
        DeleteObject(hfont);
        DeleteObject(font);
        DeleteObject(hscrool);
        GlobalFree(paths);
        backup_file(NULL,DEL_BACKUP);
        GlobalFree(fname);
        ShowWindow(GetConsoleWindow(),SW_SHOW);
        PostQuitMessage (WM_QUIT);
        break;
    }
    case WM_POWERBROADCAST:
    {
        if((DWORD)wParam == PBT_APMBATTERYLOW)
        {
            if(caption[strlen(caption)-1]=='*')
            {
                    if(Setfile[0]==TRUE)
                    {
                                caption[strlen(caption)-1]=='\0';
                                SetWindowText(GetDlgItem(hwnd,IDM_FILENAME),caption);
                                save(hwnd);
                    }
                    else{
                        char ctext[_MAX_PATH];
                        GetTempPathA(_MAX_PATH,ctext);
                        strcat(ctext,LAST_FILE_NAME);
                        SetWindowText(GetDlgItem(hwnd,IDM_FILENAME),ctext);
                        save(hwnd);
                        change=FALSE;
                        }
            }
            PostQuitMessage(0);
        }
        else if((DWORD)wParam == PBT_APMQUERYSUSPEND) {return TRUE;}
        return TRUE;
    }
    case WM_SIZE:
    {
        if((int) LOWORD(lParam) < 400){width = 400;flag = TRUE;}
        else {width = LOWORD(lParam);} //Use to initialise  position of dialog box
        height = HIWORD(lParam);
        /**Update FILENAME button**/
        temp=GetDlgItem(hwnd,IDM_FILENAME);
        MoveWindow(temp,20,0,width-20,23,TRUE);
        SetFocus(temp);
        ShowWindow(temp,SW_HIDE);
        ShowWindow(temp,SW_SHOW);
        /**Update CANCEL button**/
        temp=GetDlgItem(hwnd,IDCANCEL);
        MoveWindow(temp,width-60,22,60,23,TRUE);
        ShowWindow(temp,SW_SHOWNORMAL);
        /**Update TEXT button**/
        temp=GetDlgItem(hwnd,IDM_TEXT);
        if(S_State_bar==TRUE)
        {
            MoveWindow(temp,5,50,width-6,height-75,FALSE);
        }
        else{
            MoveWindow(temp,5,50,width-6,height-52,FALSE);
        }
        ShowWindow(temp,SW_SHOWNORMAL);
        SetFocus(temp);
        if(S_State_bar==TRUE)
        {
        /**Update status bar if it's displayed**/
           size_change=TRUE;
           SendMessage(statusbar, WM_SIZE, 0, 0);
         }
         /******End Update*******/
        /* if(width == 400 && flag){
            MoveWindow(hwnd, position.x+300, position.y+100, 402, height, false);
         }*/
         if(flag == false){ClientToScreen(hwnd,&position);}
         break;
    }
   /* case WM_MOVE:
    {
        yWindows = (int) HIWORD(lParam); // recupere la position de la fenetre au moment du deplacement
        break;
    }*/
    case WM_CTLCOLORSTATIC:
    {
        SetBkColor((HDC)wParam,(default_color.static_bck_color));
        SetTextColor((HDC)wParam,(default_color.static_text_color));
        return (BOOL)PtrToInt(hbstatic);
    }
    case  WM_CTLCOLORSCROLLBAR:
    {
        SetBkColor((HDC)wParam,RGB(60,250,240));
        return (BOOL)PtrToInt(hscrool);
    }
    case WM_CTLCOLOREDIT:
    {
            SetTextColor((HDC)wParam,(default_color.text_color_idm_text));
            SetBkColor((HDC)wParam,(default_color.bck_color_idm_text));
            return (BOOL)PtrToInt(he);
    }
    case WM_CTLCOLORDLG:
        return  (INT_PTR)hbdialog;
    default:
        return DefWindowProc (hwnd, message, wParam, lParam);
    }
  return 0;
}

HICON CreateSmallIcon( HWND hWnd ) //A revoir
{
    static TCHAR *szText = TEXT ( "100" );
    HDC hdc, hdcMem;
    HBITMAP hBitmap = NULL;
    HBITMAP hOldBitMap = NULL;
    HBITMAP hBitmapMask = NULL;
    ICONINFO iconInfo;
    HFONT hFont;
    HICON hIcon;

    hdc = GetDC ( hWnd );
    hdcMem = CreateCompatibleDC ( hdc );
    hBitmap = CreateCompatibleBitmap ( hdc, 16, 16 );
    hBitmapMask = CreateCompatibleBitmap ( hdc, 16, 16 );
    ReleaseDC ( hWnd, hdc );
    hOldBitMap = (HBITMAP) SelectObject ( hdcMem, hBitmap );
    PatBlt ( hdcMem, 0, 0, 16, 16, WHITENESS );

    // Draw percentage
    hFont = CreateFont (12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    TEXT ("Arial"));
    hFont = (HFONT) SelectObject ( hdcMem, hFont );
    TextOut ( hdcMem, 0, 0, szText, lstrlen (szText) );

    SelectObject ( hdc, hOldBitMap );
    hOldBitMap = NULL;

    iconInfo.fIcon = TRUE;
    iconInfo.xHotspot = 0;
    iconInfo.yHotspot = 0;
    iconInfo.hbmMask = hBitmapMask;
    iconInfo.hbmColor = hBitmap;

    hIcon = CreateIconIndirect ( &iconInfo );

    DeleteObject ( SelectObject ( hdcMem, hFont ) );
    DeleteDC ( hdcMem );
    DeleteDC ( hdc );
    DeleteObject ( hBitmap );
    DeleteObject ( hBitmapMask );

    return hIcon;
}

BOOL APIENTRY renameProc(HWND hwnd ,UINT message, WPARAM wParam, LPARAM lParam)
{
    RECT rc;
    float x, y, reg_width, reg_height;
    switch(message)
    {
    case WM_INITDIALOG:
        /*GetClientRect(hwnd,&rc);
        reg_width = rc.right - rc.left;
        reg_height= rc.bottom - rc.top;
        x=(GetSystemMetrics(SM_CXSCREEN) - reg_width)/2;
        y=(GetSystemMetrics(SM_CYSCREEN) - reg_height)/2;
        MoveWindow(hwnd,x,y,reg_width,reg_height,true);
        //initialise control of path name
        //initialise control of filename*/
        break;
    case WM_COMMAND:
        if(LOWORD(wParam) == IDOK)
        {

            //function
        }
        else
        {

            //another function
        }
        break;
    }
    return TRUE;
}

LRESULT CALLBACK edit_control_proc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LPSTR text=NULL;
    short taille=0,line_index,prv_line_index,i=0,position,char_sel,character_copied=0;
    switch(message)
    {
        case WM_PRINTCLIENT:
        case WM_PAINT:
        {
             CallWindowProc(default_edit_control_proc,hwnd,message,wParam,lParam);

             /*****Display Cue Banner Text if IDM_TEXT don't contain any text**/
            int textLength = GetWindowTextLength(hwnd);
            if (textLength == 0 && GetFocus() != hwnd)
            {
                HDC hdc = (message == WM_PRINTCLIENT)
                ? reinterpret_cast<HDC>(wParam)
                : GetDCEx(hwnd, NULL, DCX_INTERSECTUPDATE|DCX_CACHE|DCX_CLIPCHILDREN | DCX_CLIPSIBLINGS);

                HFONT font =(HFONT)SendMessage(hwnd,WM_GETFONT,0,0);
                RECT editRect;
                GetClientRect(hwnd,&editRect);
                editRect.top+=2;
                editRect.left+=2;
                SelectObject(hdc, font);
                SetTextColor(hdc, GetSysColor(COLOR_GRAYTEXT));
                //SetBkMode(hdc, TRANSPARENT);
                SetBkColor(hdc,default_color.bck_color_idm_text);
                //DrawText(hdc, cueBannerText, int(strlen(cueBannerText)), &editRect, DT_TOP|DT_LEFT|DT_NOPREFIX|DT_NOCLIP);
                TextOut(hdc,2,2,cueBannerText,int(strlen(cueBannerText)));
                ReleaseDC(hwnd, hdc);
                //DeleteObject(font);
            }

         break;
        }
        case WM_CREATE:
            nb_tab=0;
            CallWindowProc(default_edit_control_proc,hwnd,message,wParam,lParam);
            break;
        case WM_SETFOCUS:
        {
             CallWindowProc(default_edit_control_proc,hwnd,message,wParam,lParam);
            /* GetCaretPos(&pt);
             POINT pt;
             CreateCaret(hwnd,LoadBitmap(hinst,MAKEINTRESOURCE(SCP_CARET)),0,0);
             SetCaretPos(pt.x,pt.y);
             ShowCaret(hwnd);
             */
             break;
        }
        case WM_UNICHAR:
        case WM_CHAR:
        {
            switch(wParam)
            {
                case ':':
                case '{':
                    first_char = (wParam);
                    CallWindowProc(default_edit_control_proc,hwnd,message,wParam,lParam);
                    break;
                case '}':
                    nb_tab= (nb_tab<1)?0:nb_tab-1;
                    CallWindowProc(default_edit_control_proc,hwnd,message,wParam,lParam);
                    break;
                case 0x08: //back space
                {
                    prv_line_index=SendMessage(hwnd,EM_LINEFROMCHAR,-1,0);
                    prv_line_index = (prv_line_index > 0)?prv_line_index:0;
                    CallWindowProc(default_edit_control_proc,hwnd,message,wParam,lParam);
                    line_index=SendMessage(hwnd,EM_LINEFROMCHAR,-1,0);
                    line_index = (line_index > 0)?line_index:0;
                    char_sel=HIWORD(SendMessage(hwnd,EM_GETSEL,0,0));
                    position=SendMessage(hwnd,EM_LINEINDEX,line_index,0);
                    taille=SendMessage(hwnd,EM_LINELENGTH,position,0);
                    if(taille>0)
                    {
                        text=(LPSTR)LocalAlloc(GPTR,(taille+1) * sizeof(char));
                        sprintf(text,"%d",(taille*sizeof(char)));
                        character_copied = SendMessage(hwnd,EM_GETLINE,(WPARAM)line_index,(LPARAM)text);
                        if(character_copied>0)
                        {
                            int a=char_sel- position-1;

                            if(text[a]=='{' || text[a]==':')nb_tab=(nb_tab>0)?(nb_tab-1):0;
                            else if(text[a]=='}')nb_tab++;
                        }
                        else{
                                char buf[120];
                                sprintf(buf,"Error detected on \" edit_control_proc()>WM_LBUTTONDOWN\" \n Prevouis index=%d Current index=%d,position=%d,taille= %d, character_copied =%d", prv_line_index,line_index,position,taille,character_copied);
                                showMessageError(hwnd,buf);
                                report_error(hwnd,0,0,"Error detected on \" edit_control_proc()>WM_LBUTTONDOWN\"");
                                SetFocus(hwnd);
                            }
                    LocalFree(text);
                   }
                   first_char=' ';
                    break;
                 }
                 case 0x0D://enter
                 {
                    CallWindowProc(default_edit_control_proc,hwnd,message,wParam,lParam);
                    if(first_char == ':' || first_char == '{')
                    {
                        nb_tab++;
                        first_char = ' ';
                    }
                    for(short i=nb_tab;i>0;i--)SendMessage(hwnd, WM_CHAR, 0x09, 0);
                    break;
                }
                default:
                    first_char=' ';
                    CallWindowProc(default_edit_control_proc,hwnd,message,wParam,lParam);
                   break;
            }
            break;
        }
        case WM_LBUTTONDOWN:
        {//Complete
            CallWindowProc(default_edit_control_proc,hwnd,message,wParam,lParam);
            DWORD index[0];
            SendMessage(hwnd,EM_GETSEL,(WPARAM)&(index[0]),(LPARAM)&(index[1]));
            if(index[0] != index[1])
            {
                POINT pos;
                GetCursorPos(&pos);
                DWORD curIndex =  SendMessage(hwnd,EM_CHARFROMPOS,(WPARAM)0,(LPARAM)MAKELPARAM(pos.x,pos.y));
                printf("\nStart=%d , Cur=%d , End=%d\n",index[0],curIndex,index[1]);
                if(index[0]<= curIndex && curIndex <= index[1])
                {

                }
            }else{
            prv_line_index=SendMessage(hwnd,EM_LINEFROMCHAR,-1,0);
            CallWindowProc(default_edit_control_proc,hwnd,message,wParam,lParam);
            line_index=SendMessage(hwnd,EM_LINEFROMCHAR,-1,0);
            char_sel=(int)HIWORD(SendMessage(hwnd,EM_GETSEL,0,0));
            position=(int)SendMessage(hwnd,EM_LINEINDEX,line_index,0);
            taille=(int)SendMessage(hwnd,EM_LINELENGTH,position,0);
            if(taille>0)
            {
                text=(LPSTR)LocalAlloc(GPTR,(taille+1) * sizeof(char));
                sprintf(text,"%d",(taille*sizeof(char)));
                character_copied = SendMessage(hwnd,EM_GETLINE,(WPARAM)line_index,(LPARAM)text);
                        if(character_copied>0)
                        {
                            int a=char_sel- position-1;

                            if(text[a]=='{' || text[a]==':')nb_tab=(nb_tab>0)?(nb_tab-1):0;
                            else if(text[a]=='}')nb_tab++;
                        }
                        else{
                                char buf[120];
                                sprintf(buf,"Error detected on \" edit_control_proc()>WM_LBUTTONDOWN\" \n Prevouis index=%d Current index=%d,position=%d taille=%d character_copied =%d", prv_line_index,line_index,position,taille,character_copied);
                                showMessageError(hwnd,buf);
                                report_error(hwnd,0,0,"Error detected on \" edit_control_proc()>WM_LBUTTONDOWN\"");
                                SetFocus(hwnd);
                            }
                LocalFree(text);
            }
            }
            break;
        }
        case WM_RBUTTONUP:
        {
            HWND parent = GetParent(hwnd);
            HMENU menu = GetSubMenu(GetMenu(parent),2);
            POINT pos;
            GetCursorPos(&pos);
            SetForegroundWindow(hwnd);
            TrackPopupMenuEx(menu,0,pos.x,pos.y,parent,NULL);
            break;
        }
        default:
              CallWindowProc(default_edit_control_proc,hwnd,message,wParam,lParam);
            break;
    }
    //return FALSE; //this line cause some bugs.
}

BOOL APIENTRY DialProcAbout(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    switch (message)
    {
   case WM_CTLCOLORSTATIC:
        SetBkColor((HDC)wParam,(default_color.static_bck_color));
        return (BOOL)PtrToInt(hbstatic);
   case WM_CTLCOLORDLG:
        return  (INT_PTR)PtrToInt(hbdialog);
    case WM_COMMAND:
        if(LOWORD(wParam)==IDCANCEL||LOWORD(wParam)==IDOK)
        {
            EndDialog(hwnd,1);
        }
        return  TRUE;
    default:
        return FALSE;
    }
    return FALSE;
}

BOOL APIENTRY GotoProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    UINT uid=LOWORD(wParam);
    LPSTR ctext=NULL;
    int dsize;
    int result=0;
    switch (message)
    {
    case WM_CTLCOLORDLG:
        return  (INT_PTR)hbdialog;
    case WM_INITDIALOG:
        SendMessage(hwnd,WM_SETICON,(WPARAM)ICON_SMALL,(LPARAM)(HICON)LoadIcon(hinst,MAKEINTRESOURCE(SCP_ICON)));
        RECT rc,rc2;
        GetClientRect(hwnd,&rc);
        GetClientRect(GetParent(hwnd),&rc2);
        POINT pt;
        pt.x = rc2.left;
        pt.y = rc2.top - GetSystemMetrics(SM_CYMENU) - GetSystemMetrics(SM_CYSMCAPTION);
        ClientToScreen(GetParent(hwnd),&pt);
        MoveWindow(hwnd, pt.x + ( (rc2.right - rc2.left) - (rc.right - rc.left))/2 , pt.y + ( (rc2.bottom - rc2.top) - (rc.bottom - rc.top))/2, 230, 100,false);
        return TRUE;
    case WM_COMMAND:
        if(uid==IDCANCEL)
        {
            EndDialog(hwnd,0);
        }
        else if(uid==IDOK)
        {
            HWND hedit=GetDlgItem(hwnd,GO_TO_EDIT);
            dsize=(int)GetWindowTextLength(hedit);
            if(dsize > 0)
            {
                ctext=(LPSTR)GlobalAlloc(GPTR,dsize+1);
                dsize++;
                if(ctext != NULL)
                {
                   GetWindowText(hedit,ctext,dsize);
                   result=String_to_Int(ctext);
                   GlobalFree(ctext);
                }
                else{showMessageError(hwnd,"Some error was occured");}
            }
             SetLastError(result); //return value to the parent window
             EndDialog(hwnd,result);
         }
         else{}
        return  TRUE;
    default:
        return FALSE;
    }
    return FALSE;
}

BOOL  APIENTRY DialView(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    PAINTSTRUCT paint;
    switch (message)
    {
    case WM_INITDIALOG:
        //SendMessage(hwnd,WM_SETCURSOR,(WPARAM)MAKEINTRESOURCE(SCP_CUR),0);
        CaptureImage(hwnd,GetParent(hwnd),hinst);
        //KillTimer(hwnd,ID_TIMER);
        //SetTimer(hwnd,ID_TIMER,500,NULL);
        break;
    case WM_TIMER:
        PostMessage(hwnd,WM_PAINT,0,0);
        return 0;
    case WM_PAINT:
    {
        //HDC hdc=BeginPaint(hwnd,&paint);
        CaptureImage(hwnd,NULL,hinst);
        EndPaint(hwnd,&paint);
        UpdateWindow(hwnd);
        break;
    }
    case WM_COMMAND:
        if(LOWORD(wParam)==IDCANCEL||LOWORD(wParam)==IDOK)
        {
            EndDialog(hwnd,1);
        }
        return  TRUE;
    case WM_DESTROY:
    {
       // KillTimer(hwnd,ID_TIMER);
        EndDialog(hwnd,1);
    }
    default:
        return FALSE;
    }
    return FALSE;
}

BOOL  APIENTRY DialReg(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam)
{
    LPSTR name=NULL;
    HWND hedit;
    DWORD fsize=0;
    RECT rc;
    float x, y, reg_width, reg_height;
    int i;//9066
    switch (message)
    {
    case WM_INITDIALOG:
        SetDlgItemUrl(hwnd,LINK_REG,"Get registration's key");
        GetClientRect(hwnd,&rc);
        reg_width = rc.right - rc.left;
        reg_height= rc.bottom - rc.top;
        x=(GetSystemMetrics(SM_CXSCREEN) - reg_width)/2;
        y=(GetSystemMetrics(SM_CYSCREEN) - reg_height)/2;
        MoveWindow(hwnd,x,y,reg_width,reg_height,true);
        SendMessage(GetDlgItem(hwnd,PASS_REG),EM_SETPASSWORDCHAR,'$',0);
        break;
    case WM_COMMAND:
        if(LOWORD(wParam)==IDOK)
        {
            hedit=GetDlgItem(hwnd,NAME_REG);
            fsize=GetWindowTextLength(hedit);
            fsize ++;
            name=(LPSTR)GlobalAlloc(GPTR,fsize);
            if(name==NULL)
            {
                showMessageError(hwnd,"Something were wrong.\n Please try again later...");
            }
            else{GetDlgItemText(hwnd,NAME_REG,name,fsize);
                  if(!strcmp(name,"root"))
                  {
                    GlobalFree(name);
                    name=NULL;
                    fsize=0;
                    hedit=GetDlgItem(hwnd,PASS_REG);
                    fsize=GetWindowTextLength(hedit);
                    fsize ++;
                    name=(LPSTR)GlobalAlloc(GPTR,fsize);
                    if(name==NULL)
                    {
                        showMessageError(hwnd," Something were wrong.\n Please try again later...");
                    }
                    else{
                            GetDlgItemText(hwnd,PASS_REG,name,fsize);
                            if(!strcmp(name,"root") )
                            {
                             char ftext[_MAX_PATH];
                             GetTempPathA(_MAX_PATH,ftext);
                             strcat(ftext,"reg.text.editor.tmp");
                             HANDLE file=CreateFile(ftext,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NOT_CONTENT_INDEXED|FILE_ATTRIBUTE_HIDDEN,NULL);
                             if(file==INVALID_HANDLE_VALUE)
                             {
                                 showMessageError(hwnd,"Failed to save key reistration\nPlease try again..");
                                 EndDialog(hwnd,1);
                                 PostQuitMessage(WM_QUIT);
                             }
                             else{
                                    DWORD read,dwsize=(DWORD)27;
                                    WriteFile(file,"ACDEF-AAAFF-ACCD-ABBC-ABCD",(dwsize-1),&read,NULL);
                                    CloseHandle(file);
                                    MessageBox(hwnd,TEXT("Registration succeed.\n Thank's.Let's enjoy..."),TEXT("Notification"),MB_OK|MB_ICONINFORMATION);
                                    EndDialog(hwnd,1);
                                 }

                            }
                            else showMessageError(hwnd,"Error:Password is wrong.\n Please get good keys...");

                            GlobalFree(name);
                         }
                  }
                  else showMessageError(hwnd,"Error:User's name is wrong.\n Please get good keys...");
                 }
           GlobalFree(name);
        }
        else{ }
        return  TRUE;
    case WM_CLOSE:
    {
        EndDialog(hwnd,1);
        PostQuitMessage(WM_QUIT);
    }
    default:
        return FALSE;
    }
    return FALSE;
}

LPSTR getFileName(HWND hwnd)
{
    LPSTR fname = (LPSTR)GlobalAlloc(GPTR, MAX_PATH);
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrTitle ="Select Bmp file";
    ofn.lpstrFilter = "Bmp Files (*.bmp)\0*.bmp\0";//
    ofn.lpstrFile = fname;
    ofn.hInstance=hinst;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY|OFN_CREATEPROMPT;
    ofn.lpstrDefExt = "bmp";
    InitCommonControls();
    if(!GetOpenFileName(&ofn))
    {
        GlobalFree(fname);
        fname = NULL;
    }
    return fname;
}

int test_state_bar =0;
void update_status_bar(HWND statusbar,HWND hwnd)
{
        HWND temp=GetDlgItem(hwnd,IDM_TEXT);
        if(GetFocus()==temp)
        {
            char text[256];
            if(size_change==TRUE)
            {
                RECT rc;
                GetWindowRect(hwnd,&rc);
                int width=rc.right-rc.left;
                int statuswidth[]= {160,160+width-780,160+width-700,160+width-550+20,180+width-440,-1};
                //int statuswidth[]= {180,180+width-780,180+width-700,180+width-550,180+width-440,-1};
                SendMessage(statusbar,SB_SETPARTS,6,(LPARAM)statuswidth);
                size_change=FALSE;

                if(test_state_bar == 0)
                {
                    LPSTR computer_name=(LPSTR)LocalAlloc(GPTR,2*MAX_COMPUTERNAME_LENGTH + 1);
                    LPSTR user_name=(LPSTR)LocalAlloc(GPTR,MAX_COMPUTERNAME_LENGTH+1);
                    DWORD dwsize=MAX_COMPUTERNAME_LENGTH + 1 ;
                    GetComputerName(computer_name,&dwsize);
                    dwsize=MAX_COMPUTERNAME_LENGTH + 1 ;
                    GetUserName(user_name,&dwsize);
                    strcat(computer_name,">>");
                    strcat(computer_name,user_name);
                    LocalFree(user_name);
                    SendMessage(statusbar,SB_SETTEXT,0,(LPARAM)computer_name);
                    LocalFree(computer_name);
                    sprintf(text,"[-Ver:1.2.4-]-[--%s %s--]-",__DATE__,__TIME__);
                    SendMessage(statusbar,SB_SETTEXT,5,(LPARAM)text);
                    test_state_bar =1;
                }
            }

            sprintf(text,"Lines:%d",(int)SendMessage(GetDlgItem(hwnd,IDM_TEXT),EM_GETLINECOUNT,0,0));
            SendMessage(statusbar,SB_SETTEXT,2,(LPARAM)text);
            int position=HIWORD(SendMessage(temp,EM_GETSEL,0,0));
            int col=position - SendMessage(temp,EM_LINEINDEX,-1,0);
            int l=SendMessage(temp,EM_LINEFROMCHAR,-1,0)+1;

            if(col >= 0)sprintf(text,"L:%d      C:%d     P:%d",l,col,position);
            else sprintf(text,"L:%d      C:NaN     P:%d",l,position);

            SendMessage(statusbar,SB_SETTEXT,3,(LPARAM)text);
            ShowWindow(statusbar,SW_SHOW);
        }
}

BOOL save (HWND hwnd)
{
    BOOL result=FALSE;
    HWND hedit;
    HANDLE hfile;
   // static char fname[MAX_PATH];
    char initial[]=">New file*";
    LPSTR ftext=NULL;
    static DWORD read;
    static DWORD fsize;
    int i=0,test=0;
    hedit=GetDlgItem(hwnd,IDM_FILENAME);
    fsize=GetWindowTextLength(hedit);
    fsize++;
    ftext=(LPSTR)GlobalAlloc(GPTR,fsize+1);
    if(ftext==NULL)
    {
        showMessageError(hwnd, "Error:Something were wrong.\n Please try again later..." );
    }
    else
    {
        GetDlgItemText(hwnd,IDM_FILENAME,ftext,fsize);
        /**Veririe si c'est un nouveau fichier: dans ce cas ftext=initial **/
        //Retoune 1 si ce n'est pas le cas
        while( (i<=9) && (test==0))
        {
            if(ftext[i]!=initial[i])
            {
                test=1;
            }
            i++;
        }
        if(test==1)
        {
            ftext[strlen(ftext)-1]='\0';
            strcpy(caption,ftext);
            hfile=CreateFile(ftext,GENERIC_WRITE|GENERIC_READ,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
            if(hfile == INVALID_HANDLE_VALUE)
            {
                if(fileExist(ftext))
                {
                    MessageBox(hwnd,TEXT("Error:Access denied..."),TEXT("Error"),MB_OK|MB_ICONWARNING);
                }else{
                showMessageError(hwnd,"Error:Opening of file failed.\n Please try again later...");
                }
            }
            else
            {
                hedit=GetDlgItem(hwnd,IDM_TEXT);
                fsize=GetWindowTextLength(hedit);
                fsize++;
                if(fsize > 0)
                {
                    ftext=(LPSTR)GlobalAlloc(GPTR,fsize+1);
                    if(ftext==NULL)
                    {
                        showMessageError(hwnd,"Error:Something were wrong.\n Please try again later...");
                        strcat(caption,"*/0");
                    }
                    else
                    {
                        UnlockFile(hfile,0,0,0,0);
                        GetDlgItemText(hwnd,IDM_TEXT,ftext,fsize);
                        ftext[fsize]='\0';
                        WriteFile(hfile,ftext,(fsize-1),&read,NULL);
                        GlobalFree(ftext);
                        result=TRUE;
                        SendMessage(GetDlgItem(hwnd,IDM_FILENAME),WM_SETTEXT,0,(LPARAM)(LPSTR)caption);//Set default program's name
                        SendMessage(hwnd,WM_SETTEXT,0,(LPARAM)(LPSTR)caption);//Set default program's name
                        EnableWindow(GetDlgItem(hwnd,IDM_SAVE),FALSE);//Desactivation du button "save"
                        EnableMenuItem(GetMenu(hwnd),IDM_SAVE,TRUE);
                        EnableMenuItem(GetMenu(hwnd),IDM_SAVE_AS,TRUE);

                    }
                }
                else {
                        strcat(caption,"*/0");
                        showMessageError(hwnd, "Error:Something were wrong.\n Please try again later..." );
                     }
                CloseHandle(hfile);
            }
        }
        else{result=save_as(hwnd);}
    }
 return result;
}

BOOL save_as (HWND hwnd)
{
    BOOL result=FALSE;
    OPENFILENAME ofn;
    static HWND hedit;
    static HANDLE hfile;
    static char fname[MAX_PATH]="New file";
    LPSTR ftext=NULL;
    static DWORD read;
    static DWORD fsize;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = fname;
    ofn.hInstance=hinst;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER |OFN_PATHMUSTEXIST| OFN_OVERWRITEPROMPT;
    ofn.lpstrDefExt = "txt";//default extension
    ofn.lpstrTitle ="TextEditor Save";//title of Save As dialog
    if(GetSaveFileName(&ofn)==0)
    {
        DWORD result=CommDlgExtendedError();
        manage_error(hwnd,result,C_OPENFILENAME);
    }
    else
    {
        hfile=CreateFile(ofn.lpstrFile,GENERIC_WRITE|GENERIC_READ,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
        if(hfile==INVALID_HANDLE_VALUE)
        {
            if(fileExist(ofn.lpstrFile))
                {
                   showMessageError(hwnd,"Error:Access denied...");
                }else{
                   showMessageError(hwnd,"Error:specified file has invalid\n\tRessayer..." );
                }
        }
        else
        {
            hedit=GetDlgItem(hwnd,IDM_TEXT);
            fsize=GetWindowTextLength(hedit);
            fsize++;
            if(fsize > 0)
            {
                ftext=(LPSTR)GlobalAlloc(GPTR,fsize+1);
                if(ftext!=NULL)
                {
                    GetDlgItemText(hwnd,IDM_TEXT,ftext,fsize);
                    ftext[fsize]='\0';
                    WriteFile(hfile,ftext,(fsize-1),&read,NULL);
                    strcpy(caption,fname);
                    SendMessage(GetDlgItem(hwnd,IDM_FILENAME),WM_SETTEXT,0,(LPARAM)(LPSTR)caption);//Set default program's name
                    SendMessage(hwnd,WM_SETTEXT,0,(LPARAM)(LPSTR)caption);//Set default program's name
                    GlobalFree(ftext);
                    result=TRUE;
                    EnableWindow(GetDlgItem(hwnd,IDM_SAVE),FALSE);//Desactivation du button "save"
                    EnableMenuItem(GetMenu(hwnd),IDM_SAVE,TRUE);
                    EnableMenuItem(GetMenu(hwnd),IDM_SAVE_AS,TRUE);
                    update_recent_file(hwnd,fname);
                }
            }
         CloseHandle(hfile);
        }
    }
    return result;
}


char* getMutexLocation()
{
  GetTempPathA(_MAX_PATH,mutex);/**<met le chemin absolu de %temp% dans la variable ctext*/
  return strcat(mutex,"mutex.text.editor.ms\0");


}
/*void SCRIPTPARAM loadParam()
{

}*/

void read (HWND hwnd,BOOL* Setfile)
{
    OPENFILENAME ofn;
    static char fname[MAX_PATH]=" ";
    LPSTR ftext=NULL;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrTitle ="TextEditor Open";
    ofn.lpstrFilter = "ScriptEdit Files (.ms)\0*.ms\0All Files (*.*)\0*.*\0";//
    ofn.lpstrFile = fname;
    ofn.hInstance=hinst;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY|OFN_CREATEPROMPT;
    ofn.lpstrDefExt = "ms";
    InitCommonControls();
    if(!GetOpenFileName(&ofn))
    {
        DWORD result=CommDlgExtendedError();
        manage_error(hwnd,result,C_OPENFILENAME);
    }
    else
    {
        ftext = SE_ReadFile(hwnd,fname);
        if(ftext!=NULL)
        {
            SetDlgItemText(hwnd,IDM_TEXT,ftext);
            SendDlgItemMessage(hwnd,IDM_TEXT,EM_FMTLINES,TRUE,0);
            strcpy(caption,fname);
            SendMessage(hwnd,WM_SETTEXT,0,(LPARAM)(LPSTR)caption);
            update_recent_file(hwnd,fname);
            SetDlgItemText(hwnd,IDM_FILENAME,fname);
            UpdateWindow(hwnd);
            Setfile[0]=TRUE;
            GlobalFree(ftext);
            if(!backup_file(fname,BACKUP))
                    {
                        Setfile[1]=FALSE;
                        EnableWindow(GetDlgItem(hwnd,IDCANCEL),FALSE);/**<Desactivation du button "Cancel"*/
                    }
            else{
                Setfile[1]=TRUE;
               EnableWindow(GetDlgItem(hwnd,IDCANCEL),TRUE);
            }
        }
    }
}
