#ifndef __COLOR_H__
#define __COLOR_H__

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif

#ifndef __WINDOWS_H__
    #define __WINDOWS_H__
    #include <windows.h>
#endif // __WINDOWS_H__

#define DEFAULT_COLOR RGB(0,128,192) /**<default text color for IDM_TEXT control edit*/
#define DEFAULT_BKCOLOR RGB(30,30,30) /**<default first background color for control edit IDM_TEXT*/
#define DEFAULT_BKCOLOR_S RGB(92,92,92)  /**<default second background color for control edit IDM_TEXT*/
#define DEFAULT_BRUSH_DIALOG_BOX CreateSolidBrush(RGB(64,128,128)) /**default brush for background of dialog box*/
#define DEFAULT_BRUSH_SCROOL_BAR CreateSolidBrush(RGB(60,250,240)) /**default brush for background of scroll bar*/
#define DEFAULT_BRUSH_BKCOLOR_S CreateSolidBrush(DEFAULT_BKCOLOR_S) /**default brush for disabled background of control edit IDM_TEXT*/
#define DEFAULT_BKCOLOR_STATIC_CONTROL RGB(84,189,189) /**default background color for static control*/
#define DEFAULT_COLOR_STATIC_CONTROL RGB(200,200,200) /**default text color for static control*/
#define DEFAULT_BRUSH_STATIC_CONTROL CreateSolidBrush(DEFAULT_BKCOLOR_STATIC_CONTROL)/**default brush for background of static control*/
#define DEFAULT_TEXT_COLOR_RICH_EDIT RGB(255,255,255)
#define DEFAULT_BCK_COLOR_RICH_EDIT RGB(100,100,100)

typedef struct APP_COLOR{
                            COLORREF text_color_idm_text;/**<text color for IDM_TEXT control edit*/
                            COLORREF bck_color_idm_text;/**<enabled background color for control edit IDM_TEXT*/
                            COLORREF bck_color_s_idm_text;/**<disabled background color  for control edit IDM_TEXT*/
                            COLORREF static_bck_color;/**< background color for static control*/
                            COLORREF static_text_color;/**< text color for static control*/
                            COLORREF text_color_file_name;/**<text color for IDM_FILENAME rich edit*/
                            COLORREF bck_color_file_name;/**<background color for IDM_FILENAME rich edit*/
                            } APP_COLOR;

#ifdef __cplusplus
extern "C"
{
#endif

DLL_EXPORT BOOL LoadColor(APP_COLOR*);

#ifdef __cplusplus
}
#endif

#endif // __COLOR_H__
