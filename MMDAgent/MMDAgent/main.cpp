/* ----------------------------------------------------------------- */
/*           The Toolkit for Building Voice Interaction Systems      */
/*           "MMDAgent" developed by MMDAgent Project Team           */
/*           http://www.mmdagent.jp/                                 */
/* ----------------------------------------------------------------- */
/*                                                                   */
/*  Copyright (c) 2009-2010  Nagoya Institute of Technology          */
/*                           Department of Computer Science          */
/*                                                                   */
/* All rights reserved.                                              */
/*                                                                   */
/* Redistribution and use in source and binary forms, with or        */
/* without modification, are permitted provided that the following   */
/* conditions are met:                                               */
/*                                                                   */
/* - Redistributions of source code must retain the above copyright  */
/*   notice, this list of conditions and the following disclaimer.   */
/* - Redistributions in binary form must reproduce the above         */
/*   copyright notice, this list of conditions and the following     */
/*   disclaimer in the documentation and/or other materials provided */
/*   with the distribution.                                          */
/* - Neither the name of the MMDAgent project team nor the names of  */
/*   its contributors may be used to endorse or promote products     */
/*   derived from this software without specific prior written       */
/*   permission.                                                     */
/*                                                                   */
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND            */
/* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,       */
/* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF          */
/* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE          */
/* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS */
/* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,          */
/* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED   */
/* TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     */
/* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON */
/* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,   */
/* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY    */
/* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE           */
/* POSSIBILITY OF SUCH DAMAGE.                                       */
/* ----------------------------------------------------------------- */

/* headers */

#include <windows.h>
#include <locale.h>
#include <tchar.h>

#include "TextRenderer.h"
#include "resource.h"
#define GLOBAL_VARIABLE_DEFINE
#include "MMDAgent.h"
#include "UserOption.h"
#include "utils.h"

#define MAIN_MAXBUFLEN 1024

MMDAgent mmdagent;
bool isRunning;

HWND windowHandleCommandDialogBox; /* window handle of command dialog box */

/* copyrightDialogBox: show copyright */
static INT_PTR CALLBACK copyrightDialogBox(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
   UNREFERENCED_PARAMETER(lParam);

   switch (message) {
   case WM_INITDIALOG:
      return (INT_PTR) true;
   case WM_COMMAND:
      if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
         EndDialog(hDlg, LOWORD(wParam));
         return (INT_PTR) true;
      }
      break;
   }
   return (INT_PTR) false;
}

/* commandDialogBox: send command by hand */
static INT_PTR CALLBACK commandDialogBox(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
   char buf[MAIN_MAXBUFLEN];
   wchar_t wbuf[MAIN_MAXBUFLEN];
   size_t len;
   wchar_t *p;

   switch (message) {
   case WM_INITDIALOG:
      strcpy(buf, "");
      windowHandleCommandDialogBox = hDlg;
      return (INT_PTR) true;
   case WM_COMMAND:
      switch (LOWORD(wParam)) {
      case IDCANCEL:
         DestroyWindow(hDlg);
         windowHandleCommandDialogBox = 0;
         return (INT_PTR) true;
      case IDOK:
         GetDlgItemTextA(hDlg, IDC_EDIT, buf, MAIN_MAXBUFLEN);
         /* execute */
         mbstowcs_s(&len, wbuf, MAIN_MAXBUFLEN, buf, _TRUNCATE);
         if (len > 0) {
            p = wcschr(wbuf, L'|');
            if (p) {
               *p = L'\0';
               mmdagent.sendCommandMessage(wbuf, L"%s", p + 1);
            } else {
               mmdagent.sendCommandMessage(wbuf, NULL);
            }
         }
         SetDlgItemTextA(hDlg, IDC_EDIT, "");
         return (INT_PTR) true;
      }
      break;
   case WM_CLOSE:
      DestroyWindow(hDlg);
      windowHandleCommandDialogBox = 0;
      return (INT_PTR) true;
   case WM_DESTROY:
      windowHandleCommandDialogBox = 0;
      return (INT_PTR) true;
   }
   return (INT_PTR) false;
}


/* procMessage: process all message */
LRESULT CALLBACK procMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   bool hitAgent;
   bool hitThis;

   /* process message for MMDAgent */
   hitAgent = mmdagent.procMessage(hWnd, message, wParam, lParam);

   hitThis = true;
   switch (message) {
   case WM_COMMAND:
      switch (LOWORD(wParam)) {
      case IDM_ABOUT:
         DialogBox(mmdagent.getInstance(), MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, copyrightDialogBox);
         break;
      case IDM_TEST_COMMAND:
         windowHandleCommandDialogBox = CreateDialogParam(mmdagent.getInstance(), MAKEINTRESOURCE(IDD_COMMANDBOX), hWnd, commandDialogBox, 0);
         break;
      case IDM_EXIT:
         DestroyWindow(hWnd);
         break;
      default:
         return DefWindowProc(hWnd, message, wParam, lParam);
      }
      break;
   case WM_PAINT:
      mmdagent.renderScene(hWnd);
      ValidateRect(hWnd, NULL);
      break;
   case WM_DESTROY:
      isRunning = false;
      mmdagent.release();
      PostQuitMessage(0);
      break;
   default:
      hitThis = false;
   }

   if (hitThis == false && hitAgent == false)
      return DefWindowProc(hWnd, message, wParam, lParam);

   return 0;
}

/* _tWinMain: main function */
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
   MSG msg;
   HACCEL hAccelTable;
   TCHAR szTitle[MAIN_MAXBUFLEN];
   TCHAR szWindowClass[MAIN_MAXBUFLEN];

   int argc;
   wchar_t **argv;

   WNDCLASSEX wcex;
   HWND hWnd;

   UNREFERENCED_PARAMETER(hPrevInstance);
   UNREFERENCED_PARAMETER(lpCmdLine);

   LoadString(hInstance, IDS_APP_TITLE, szTitle, MAIN_MAXBUFLEN);
   LoadString(hInstance, IDC_MMDAGENT, szWindowClass, MAIN_MAXBUFLEN);

   /* register window */
   wcex.cbSize = sizeof(WNDCLASSEX);
   wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
   wcex.lpfnWndProc = procMessage;
   wcex.cbClsExtra = 0;
   wcex.cbWndExtra = 0;
   wcex.hInstance = hInstance;
   wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MMDAGENT));
   wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
   wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
   wcex.lpszMenuName = 0;
   wcex.lpszClassName = szWindowClass;
   wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
   RegisterClassEx(&wcex);

   /* create window */
   argv = CommandLineToArgvW(GetCommandLine(), &argc);
   hWnd = mmdagent.setup(hInstance, szTitle, szWindowClass, argc, argv);
   if (!hWnd)
      return false;
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MMDAGENT));

   /* initialize variables */
   isRunning = true;
   windowHandleCommandDialogBox = 0;

   while (isRunning) {
      if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
         if (!GetMessage(&msg, NULL, 0, 0)) break;
         if (windowHandleCommandDialogBox && IsDialogMessage(windowHandleCommandDialogBox, &msg)) continue;
         TranslateAccelerator(msg.hwnd, hAccelTable, &msg);
         TranslateMessage(&msg);
         DispatchMessage(&msg);
      } else {
         mmdagent.updateScene();
      }
   }

   return (int) msg.wParam;
}
