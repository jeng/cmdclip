/* pushclip, Copyright (c) 2003-2021 Jeremy English <jhe@jeremyenglish.org>
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

#define BUFSIZE 4096 
#define TITLE_BUFSIZE 1024

//https://docs.microsoft.com/en-us/troubleshoot/windows-server/performance/obtain-console-window-handle
HWND GetConsoleHwnd(void) {
    HWND hwndFound;         // This is what is returned to the caller.
    char pszNewWindowTitle[TITLE_BUFSIZE]; // Contains fabricated WindowTitle.
    char pszOldWindowTitle[TITLE_BUFSIZE]; // Contains original WindowTitle.

    // Fetch current window title.
    GetConsoleTitle(pszOldWindowTitle, TITLE_BUFSIZE);

    // Format a "unique" NewWindowTitle.
    wsprintf(pszNewWindowTitle,"%d/%d",
            GetTickCount(),
            GetCurrentProcessId());

    // Change current window title.
    SetConsoleTitle(pszNewWindowTitle);

    // Ensure window title has been updated.
    Sleep(40);

    // Look for NewWindowTitle.
    hwndFound=FindWindow(NULL, pszNewWindowTitle);

    // Restore original window title.
    SetConsoleTitle(pszOldWindowTitle);

    return(hwndFound);
}

void ErrorMessage(const char *msg){
    #define ERROR_CODE 10
    HANDLE output = GetStdHandle(STD_ERROR_HANDLE);
    char errorCode[ERROR_CODE] = {0};
    wsprintf(errorCode, " %d\n", GetLastError());
    if (output == INVALID_HANDLE_VALUE){
        return;
    }
    WriteFile(output, msg, strlen(msg), NULL, NULL);
    WriteFile(output, errorCode, strlen(errorCode), NULL, NULL);
    CloseHandle(output); 
}

int main(int argc, char **argv){
    HGLOBAL globalBuffer;
    int currentBufferIdx = 0;
    int currentBufferSize = BUFSIZE;
    bool haveData = false;

    HANDLE input = GetStdHandle(STD_INPUT_HANDLE);

    if (input == INVALID_HANDLE_VALUE){
        ErrorMessage("Could not get the input handle");
        return -1;
    }

    HWND cmdHwnd = GetConsoleHwnd();

    int res = OpenClipboard(cmdHwnd);
    if (res <= 0){
        ErrorMessage("Could not open the clipboard");
        return -1;
    }

    EmptyClipboard();

    globalBuffer = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, (currentBufferSize + 1) * sizeof(TCHAR));

    for(;;){
        LPSTR buffer;
        DWORD bytesread;

        if (globalBuffer == NULL){
            ErrorMessage("Failed to get the memory");
            CloseClipboard();
            return -1;
        }

        buffer = (LPTSTR)GlobalLock(globalBuffer);

        if (buffer == NULL){
            ErrorMessage("Failed to lock the global object");
            CloseClipboard();
            return -1;
        }

        if(ReadFile(input, &(buffer[currentBufferIdx]), BUFSIZE, &bytesread, NULL)){
            GlobalUnlock(globalBuffer);

            haveData = true;

            if (bytesread == BUFSIZE){
                currentBufferSize += BUFSIZE;
                currentBufferIdx += BUFSIZE;
                globalBuffer = GlobalReAlloc(globalBuffer, (currentBufferSize + 1) * sizeof(TCHAR), GMEM_MOVEABLE | GMEM_ZEROINIT);
                if (globalBuffer == NULL){
                    ErrorMessage("Could not reallocate the buffer");
                    CloseClipboard();
                    return -1;
                }
            }
        }
        else {
            GlobalUnlock(globalBuffer);
            break;
        }
    }


    if (haveData && globalBuffer != NULL){
        HANDLE res = SetClipboardData(CF_TEXT, globalBuffer);
        if (res == NULL){

            //Once ownership is transferred we cannot free the global buffer.
            //We only free the global buffer if SetClipboardData fails or if we
            //do not have any data.

            GlobalFree(globalBuffer);
            ErrorMessage("Push failed");
        }
    } 
    else if (globalBuffer != NULL){
        GlobalFree(globalBuffer);
    }

    CloseClipboard();
    CloseHandle(input);
}

