#ifndef _FONCTIONS1_
#define _FONCTIONS1_

#ifndef __STRING_H__
    #include <string.h>
    #define __STRING_H__
#endif // __STRING_H__


void modify_size(HWND hwnd,BOOL flag,DWORD dsize)//DLL-getobject fail
{
    HFONT font=(HFONT)SendDlgItemMessage(hwnd,IDM_TEXT,WM_GETFONT,0,0);
    LOGFONT logfont;
    if(SUCCEEDED(GetObject(font,sizeof(LOGFONT),&logfont)))
    {
        LONG l_size=(LONG)(MulDiv(14*dsize/100,GetDeviceCaps(GetDC(hwnd),LOGPIXELSX),72));
        logfont.lfHeight=l_size;
        font = CreateFontIndirect(&logfont);
        SendDlgItemMessage(hwnd,IDM_TEXT,WM_SETFONT,(WPARAM)(font),TRUE);
    }
    else{
            showMessageError(hwnd,"\tWe are sorry!\nAn error occurs. Please reload the application.");
        }
}

BOOL setfont(HWND hwnd,HFONT* hfont,COLORREF* color)//DLL-Getdevicecaps fail
{
    InitCommonControls();
    CHOOSEFONT cf= {sizeof(CHOOSEFONT)};
    LOGFONT lf;
    GetObject(*hfont,sizeof(LOGFONT),&lf);//convert Hfont to Logfont
    cf.Flags=CF_EFFECTS|CF_INITTOLOGFONTSTRUCT|CF_SCREENFONTS;
    cf.lpTemplateName="TextEditor Font";
    cf.hwndOwner=hwnd;
    cf.rgbColors=*color;//default text color
    lf.lfUnderline=FALSE;// Underline c  haracter
    lf.lfOutPrecision=OUT_DEVICE_PRECIS;
    lf.lfPitchAndFamily=FF_DECORATIVE;
    font_copy(lf.lfFaceName,"Lucida Calligraphy");//font Color
    lf.lfHeight=(LONG)(MulDiv(10,GetDeviceCaps(GetDC(hwnd),LOGPIXELSX),72));//size: 10
    cf.lpLogFont=&lf;

    if(ChooseFont(&cf))
    {
        DeleteObject(*hfont);
        *hfont = CreateFontIndirect(&lf);
        if(!hfont)
        {
                showMessageError(hwnd,"\tWe are sorry!\nFont Creating Failed. Please try again later.");
                return FALSE;
        }
        *color=cf.rgbColors;
    }
    return TRUE;
}

void goto_function(HWND hwnd,int result)
{
            if(result != 0)
            {
                HWND hedit=GetDlgItem(hwnd,IDM_TEXT);
                int umax=(int)SendMessage(hedit,EM_GETLINECOUNT,0,0);
                if(umax < result)
                {
                    showMessageError(hwnd,"\tLine not found.\n");
                }
                else{
                        result--;
                        SendMessage(hedit,EM_LINESCROLL,(LPARAM)0,(LPARAM)(result-(int)SendMessage(hedit,EM_GETFIRSTVISIBLELINE,0,0)));
                        result=(int)SendMessage(hedit,EM_LINEINDEX,result,0);
                        SendMessage(hedit,EM_SETSEL,(WPARAM)result,(LPARAM)(result+5));
                }
                SetFocus(hedit);
            }
}

/**Name: isRegistred
 * Objectif: verifie si l'utilisateur s'est enregistre( verifie la presence de la Cles:"ACDEF-AAAFF-ACCD-ABBC-ABCD"
             dans le fichier:%temp%//reg.text.editor.tmp
 * Entree: Le HWND de la fenetre principale
 * Sortie: TRUE s'il s'est deja enregistre et FALSE sinon.En cas d'erreur on ferme la fenetre( POSTQUITMESSAGE(0) )
 */
BOOL isRegistred(HWND hwnd )
{
  BOOL result=FALSE;
  char ctext[_MAX_PATH];
  GetTempPathA(_MAX_PATH,ctext);/**<met le chemin absolu de %temp% dans la variable ctext*/
  strcat(ctext,"reg.text.editor.tmp\0");
  if(fileExist(ctext))
  {
      HANDLE hfile=CreateFile(ctext,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
      if(hfile==INVALID_HANDLE_VALUE)
        {
            showMessageError(hwnd,"\t\tWe are sorry!\n    Verification of your registration keys failed.\nPlease try again or contact us for resolve this issue.");
            report_error(hwnd,0,0,"Verification of registration keys failed");
            PostQuitMessage(0);
        }
        else
        {
            DWORD fsize=GetFileSize(hfile,NULL);
            if(fsize == 0xFFFFFFFF)
            {
                showMessageError(hwnd,"\t\tWe are sorry!\n    Verification of your registration keys failed.\nPlease try again or contact us for resolve this issue.");
                report_error(hwnd,0,0,"Verification of registration keys failed");
                PostQuitMessage(0);
            }
            else
            {
                LPSTR ftext=(LPSTR)GlobalAlloc(GPTR,fsize+1);
                if(ftext==NULL)
                {
                    showMessageError(hwnd,"\t\tWe are sorry!\n    Verification of your registration keys failed.\nPlease try again or contact us for resolve this issue.");
                    report_error(hwnd,0,0,"Verification of registration keys failed");
                    PostQuitMessage(0);
                }
                else
                {
                    DWORD dwread;
                    if(!ReadFile(hfile,ftext,fsize,&dwread,NULL))
                    {
                       MessageBox(hwnd,"\t\tWe are sorry!\n    Verification of your registration keys failed.\nPlease try again or contact us for resolve this issue.","We are sorry! --My Script--",MB_OK|MB_ICONERROR);
                       report_error(hwnd,0,0,"Verification of registration keys failed");
                       PostQuitMessage(0);
                    }
                    else{
                            ftext[fsize]='\0';
                            result = strcmp(ftext,"ACDEF-AAAFF-ACCD-ABBC-ABCD")==0;/**<La cles :"ACDEF-AAAFF-ACCD-ABBC-ABCD"*/
                        }
                    GlobalFree(ftext);
                }
            }
        CloseHandle(hfile);
        }
    }
  return result;
  }
#endif // _FONCTIONS1_
