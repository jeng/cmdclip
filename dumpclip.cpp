/* dumpclip, Copyright (c) 2003-2021 Jeremy English <jhe@jeremyenglish.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  No representations are made about the suitability of this
 * software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 */

#include "windows.h"
#include "winuser.h"

void ErrorMessage(const char *msg){
    #define ERROR_CODE 10
    HANDLE output = GetStdHandle(STD_ERROR_HANDLE);
    char errorCode[ERROR_CODE] = {0};
    wsprintf(errorCode, "%d\n", GetLastError());
    if (output == INVALID_HANDLE_VALUE){
        return;
    }
    WriteFile(output, msg, strlen(msg), NULL, NULL);
    WriteFile(output, errorCode, strlen(errorCode), NULL, NULL);
    CloseHandle(output); 
}

void WriteData(const char *msg){
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    if (output == INVALID_HANDLE_VALUE){
        return;
    }
    WriteFile(output, msg, strlen(msg), NULL, NULL);
    CloseHandle(output); 
}

int main(int argc, char **argv){
    int res = OpenClipboard(NULL);
    if (res > 0){
        HGLOBAL hglb = GetClipboardData(CF_TEXT);
        if (hglb != NULL){
            LPTSTR lptstr = (LPTSTR)GlobalLock(hglb);
            if(lptstr != NULL){
                WriteData(lptstr);
                GlobalUnlock(hglb);
            }
        }
    }
    else {
        ErrorMessage("Could not open the clipboard");
    }
    CloseClipboard();
}
