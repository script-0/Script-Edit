#ifndef __MAIN_H__
#define __MAIN_H__

/*  To use this exported function of dll, include this header
 *  in your project.
 */

#ifdef BUILD_DLL
    #define DLL_EXPORT __declspec(dllexport)
#else
    #define DLL_EXPORT __declspec(dllimport)
#endif

#include "resource.h"
#include "coordonnees.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define LAST_FILE_NAME "last_file.t"

DLL_EXPORT void showMessageError(HWND,char*);
DLL_EXPORT BOOL load_initial_info(INITIAL_INFO* info);
DLL_EXPORT void parse(HWND hwnd,char** cline,char* caption,BOOL* Setfile);
DLL_EXPORT int test_filename(const char* c_path);
DLL_EXPORT void last_session(HWND hwnd,BOOL* Setfile);
DLL_EXPORT BOOL backup_file(const char* fname,BOOL flag);
DLL_EXPORT UINT GetDroppedFilesPaths(HDROP hdrop,char*** path);
DLL_EXPORT BOOL read_drop_file(HWND hwnd ,BOOL* Setfile,char* fname);
DLL_EXPORT void set_bkcolor(HWND hwnd,COLORREF* bkcolor);
DLL_EXPORT void font_copy(CHAR* font, const char* value);
DLL_EXPORT void initialise(HWND hwnd, BOOL* change,BOOL* Setfile,HMENU* d_menu);
DLL_EXPORT void hide_state_bar(HWND hwnd,BOOL* state);
DLL_EXPORT void show_state_bar(HWND hwnd,BOOL* state);
DLL_EXPORT int remove_from_recent_file(HWND hwnd, const char* path);
DLL_EXPORT void update_recent_file(HWND hwnd_window,const char* path);
DLL_EXPORT int String_to_Int(const LPSTR ctext);
DLL_EXPORT void report_error(HWND hwnd,DWORD result,int flag,char* message_error);
DLL_EXPORT void manage_error(HWND hwnd,DWORD result,int flag);
DLL_EXPORT BOOL fileExist(const TCHAR* file);
DLL_EXPORT void read_error(HWND hwnd,DWORD dwError);
DLL_EXPORT LPSTR SE_ReadFile(HWND hwnd,char* fname);

#ifdef __cplusplus
}
#endif

#endif // __MAIN_H__
