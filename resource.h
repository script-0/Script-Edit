#ifndef _RESOURCE_H_
#define _RESOURCE_H_
struct INITIAL_INFO{
                    DWORD x0;
                    DWORD y0;
                    DWORD width;
                    DWORD height;
                    DWORD opacity;
                    BOOL state_bar;/**<state of visibility of state bar*/
                    BOOL state_bck_color;/**<State of "Disable Background color"*/
                    };/**<Structure witch contain all initial informations need to load app*/
typedef struct INITIAL_INFO INITIAL_INFO;
#define SCP_ICON 1000
#define SCP_CUR 1001
#define SCP_LOGO 10010
#define IDD_VIEW 1002
#define ID_TIMER 1003
#define ID_STATUSBAR 1004
#define ID_ACCEL 1005
#define IDC_STATIC 1006
#define IDD_REGISTRATION 1007
#define IDD_GO_TO 1008
#define ID_TRACKBAR 1009
#define IDD_RENAME_DIALOG 1010


#define IDM_QUIT 1
#define IDM_RESTORE 3
#define IDM_ABOUT 4
#define IDM_NEW 5
#define IDM_OTHER 6
#define IDM_NEW_FILE 7
#define IDM_LABEL 8
#define IDM_COPYRIGHTS 9
#define IDD_ABOUT 10
#define IDM_TEXT 11
#define IDM_LIRE 12
#define IDM_FILENAME 13
#define IDM_SAVE 14
#define IDM_SAVE_AS 15
#define IDM_FONT 16
#define IDM_BK_COLOR 17
#define IDM_BCK_S_COLOR 18
#define IDM_VIEW 19
#define IDM_CAPTION 20
#define IDC_REG  21
#define NAME_REG 22
#define PASS_REG 23
#define LINK_REG 24
#define IDM_COMP_GCC 25
#define IDM_TXT_COLOR 26
#define IDM_CLOSE 27
#define IDM_S_STATE_BAR 28
#define IDM_CONTACT_US 29
#define IDM_HIDE 30
#define IDM_RECENT_FILE 31 //31-41 (10 recent files)
#define IDM_SEARCH 42
#define IDM_GO_TO 43
#define GO_TO_EDIT 44
#define IDM_HIDE_CONSOLE 45
#define IDM_SHOW_CONSOLE 46
#define IDM_HIDE_WINDOW 47
#define IDM_COPY_LINE 48
#define IDM_ZOOM 49
#define IDM_ZOOM_25 50
#define IDM_ZOOM_50 51
#define IDM_ZOOM_75 52
#define IDM_ZOOM_100 53
#define IDM_ZOOM_125 54
#define IDM_ZOOM_150 55
#define IDM_ZOOM_200 56
#define IDM_CONTACT_US_FCK 57
#define IDM_CONTACT_US_MAIL 58
#define IDM_CONTACT_US_WTP 59
#define IDM_HIDE_BCK_SECOND 60
#define IDM_SHOW_BCK_SECOND 61
#define IDM_LAYERED 62
#define IDM_TRACKBAR_OK 63
#define IDM_TRACKBAR_TEXT 64
#define IDM_SETMARGIN 65

#define IDM_EDUNDO 66
#define IDM_EDCOPY IDM_EDUNDO+1
#define IDM_EDCUT IDM_EDUNDO+2
#define IDM_EDPASTE IDM_EDUNDO+3
#define IDM_EDDEL IDM_EDUNDO+4
#define IDM_SELECT_ALL IDM_EDUNDO+5 //66+5
#define IDM_TEST 72
#define IDC_OLD_NAME_LABEL IDM_TEST+1
#define IDC_OLD_NAME_TEXT IDM_TEST+2
#define IDC_NEW_NAME_LABEL IDM_TEST+3
#define IDC_NEW_NAME_TEXT IDM_TEST+4
#define IDC_OLD_NAME_OK IDM_TEST+5
#define IDM_BCK_S_IMAGE IDM_TEST+6
#define IDM_RESET_COLOR IDM_TEST+7
#define IDM_RENAME IDM_TEST+8
#define IDM_MUTEX IDM_TEST+9
#define IDM_DELETE_LINE IDM_TEST+10
#define IDM_ONTOP IDM_TEST+11
#define IDM_NOTOP IDM_TEST+12

#define C_OPENFILENAME 2000
#define C_SAVEFILENAME 2001
#define C_FINDTEXT 2002
#define C_REPLACETEXT 2003
#define BACKUP 2004
#define DEL_BACKUP 2005
#define RESTORE_FILE 2006

#define X_TEXT 5
#define Y_TEXT 5+14
#define  TEXT_WIDTH 380
#define TEXT_HEIGHT 30

#define ERROR_MEMORY -1
#define ERROR_MULTIPLE_SELECTION  -2
#define UNKNOWN_ERROR -3

#define STATUS_WIDTH 0
#define STATUS_HEIGHT 0
#define X_STATUS 20
#define Y_STATUS 20

#define NO_PATH 0
#define NO_EXT -1
#define NO_PATH_NO_EXT -2

#define TRACKBAR_WD 200
#define X_TRACKBAR 50
#define X_TRACKBAR_EDIT X_TRACKBAR+TRACKBAR_WD+2
#define TRACKBAR_EDIT_WD 40
#define X_TRACKBAR_OK  X_TRACKBAR_EDIT+TRACKBAR_EDIT_WD+2

#define X_RENAME_DIALOG 0
#define Y_RENAME_DIALOG 0
#define WIDTH_RENAME_DIALOG 0
#define HEIGHT_RENAME_DIALOG 0

#define X_OLD_NAME_LABEL 0
#define Y_OLD_NAME_LABEL 0
#define WIDTH_OLD_NAME_LABEL 0
#define HEIGHT_OLD_NAME_LABEL 0

#define X_OLD_NAME_TEXT 0
#define Y_OLD_NAME_TEXT 0
#define WIDTH_OLD_NAME_TEXT 0
#define HEIGHT_OLD_NAME_TEXT 0

#define X_NEW_NAME_LABEL 0
#define Y_NEW_NAME_LABEL 0
#define WIDTH_NEW_NAME_LABEL 0
#define HEIGHT_NEW_NAME_LABEL 0

#define X_NEW_NAME_TEXT 0
#define Y_NEW_NAME_TEXT 0
#define WIDTH_NEW_NAME_TEXT 0
#define HEIGHT_NEW_NAME_TEXT 0

#define  X_OLD_NAME_OK 0
#define Y_OLD_NAME_OK 0
#define WIDTH_OLD_NAME_OK 0
#define HEIGHT_OLD_NAME_OK 0

#endif // _RESOURCE
