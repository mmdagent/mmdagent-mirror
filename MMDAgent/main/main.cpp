/* ----------------------------------------------------------------- */
/*           The Toolkit for Building Voice Interaction Systems      */
/*           "MMDAgent" developed by MMDAgent Project Team           */
/*           http://www.mmdagent.jp/                                 */
/* ----------------------------------------------------------------- */
/*                                                                   */
/*  Copyright (c) 2009-2011  Nagoya Institute of Technology          */
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

#include <locale.h>
#include <windows.h>

#include "resource.h"
#include "MMDAgent.h"

#define MAIN_MAXBUFLEN 1024

MMDAgent mmdagent;

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
   char *p;

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
         if (strlen(buf) > 0) {
            p = strchr(buf, '|');
            if (p) {
               *p = '\0';
               mmdagent.sendCommandMessage(buf, "%s", p + 1);
            } else {
               mmdagent.sendCommandMessage(buf, NULL);
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
   bool hit = true;

   /* process message for MMDAgent */
   switch (message) {
   case WM_CREATE:
      break;
   case WM_LBUTTONDBLCLK:
      mmdagent.procMouseLeftButtonDoubleClickMessage(LOWORD(lParam), HIWORD(lParam));
      break;
   case WM_LBUTTONDOWN:
      SetCapture(hWnd);
      mmdagent.procMouseLeftButtonDownMessage(LOWORD(lParam), HIWORD(lParam), wParam & MK_CONTROL ? true : false, wParam & MK_SHIFT ? true : false);
      break;
   case WM_LBUTTONUP:
      ReleaseCapture();
      mmdagent.procMouseLeftButtonUpMessage();
      break;
   case WM_MOUSEWHEEL:
      mmdagent.procMouseWheelMessage((short) HIWORD(wParam) > 0 ? true : false, wParam & MK_CONTROL ? true : false, wParam & MK_SHIFT ? true : false);
      InvalidateRect(hWnd, NULL, false);
      break;
   case WM_MOUSEMOVE:
      mmdagent.procMouseMoveMessage(LOWORD(lParam), HIWORD(lParam), wParam & MK_CONTROL ? true : false, wParam & MK_SHIFT ? true : false);
      break;
   case WM_RBUTTONDOWN:
      mmdagent.procMouseRightButtonDownMessage();
      break;
   case WM_COMMAND:
      switch (LOWORD(wParam)) {
      case IDM_TOGGLE_FULLSCREEN:
         mmdagent.procFullScreenMessage();
         break;
      case IDM_TOGGLE_INFOSTRING:
         mmdagent.procInfoStringMessage();
         break;
      case IDM_TOGGLE_VSYNC:
         mmdagent.procVSyncMessage();
         break;
      case IDM_TOGGLE_SHADOWMAP:
         mmdagent.procShadowMappingMessage();
         break;
      case IDM_TOGGLE_SHADOWMAP_ORDER:
         mmdagent.procShadowMappingOrderMessage();
         break;
      case IDM_TOGGLE_DISP_RIGIDBODY:
         mmdagent.procDisplayRigidBodyMessage();
         break;
      case IDM_TOGGLE_DISP_WIRE:
         mmdagent.procDisplayWireMessage();
         break;
      case IDM_TOGGLE_DISP_BONE:
         mmdagent.procDisplayBoneMessage();
         break;
      case IDM_CARTOON_EDGE_PLUS:
         mmdagent.procCartoonEdgeMessage(true);
         break;
      case IDM_CARTOON_EDGE_MINUS:
         mmdagent.procCartoonEdgeMessage(false);
         break;
      case IDM_TIME_PLUS:
         mmdagent.procTimeAdjustMessage(true);
         break;
      case IDM_TIME_MINUS:
         mmdagent.procTimeAdjustMessage(false);
         break;
      case IDM_ZOOM_IN:
         mmdagent.procMouseWheelMessage(true, false, false);
         InvalidateRect(hWnd, NULL, false);
         break;
      case IDM_ZOOM_OUT:
         mmdagent.procMouseWheelMessage(false, false, false);
         InvalidateRect(hWnd, NULL, false);
         break;
      case IDM_ROTATE_LEFT:
         mmdagent.procHorizontalRotateMessage(false);
         break;
      case IDM_ROTATE_RIGHT:
         mmdagent.procHorizontalRotateMessage(true);
         break;
      case IDM_ROTATE_UP:
         mmdagent.procVerticalRotateMessage(true);
         break;
      case IDM_ROTATE_DOWN:
         mmdagent.procVerticalRotateMessage(false);
         break;
      case IDM_MOVE_LEFT:
         mmdagent.procHorizontalMoveMessage(false);
         break;
      case IDM_MOVE_RIGHT:
         mmdagent.procHorizontalMoveMessage(true);
         break;
      case IDM_MOVE_UP:
         mmdagent.procVerticalMoveMessage(true);
         break;
      case IDM_MOVE_DOWN:
         mmdagent.procVerticalMoveMessage(false);
         break;
      case IDM_DELETE_MODEL:
         mmdagent.procDeleteModelMessage();
         break;
      case IDM_TOGGLE_PHYSICS:
         mmdagent.procPhysicsMessage();
         break;
      case IDM_TOGGLE_DISP_LOG:
         mmdagent.procDisplayLogMessage();
         break;
      case IDM_TOGGLE_HOLD:
         mmdagent.procHoldMessage();
         break;
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
         hit = false;
      }
      break;
   case WM_PAINT:
      mmdagent.renderScene();
      ValidateRect(hWnd, NULL);
      break;
   case WM_SIZE: {
      RECT rc;
      GetClientRect(hWnd, &rc);
      mmdagent.procWindowSizeMessage(rc.right - rc.left, rc.bottom - rc.top);
   }
   break;
   case WM_MOVE:
      mmdagent.updateScene();
      break;
   case WM_DROPFILES: {
      RECT rc;
      POINT pos;
      char file[MMDAGENT_MAXBUFLEN];
      int i, num;
      /* get mouse position */
      if(!GetWindowRect(hWnd, &rc)) break;
      if(!GetCursorPos(&pos)) break;
      pos.x -= rc.left;
      pos.y -= rc.top;
      /* num */
      num = DragQueryFileA((HDROP) wParam, -1, file, MMDAGENT_MAXBUFLEN);
      for(i = 0; i < num; i++) {
         DragQueryFileA((HDROP) wParam, i, file, MMDAGENT_MAXBUFLEN);
         mmdagent.procDropFileMessage(file, pos.x, pos.y);
      }
   }
   break;
   case WM_CHAR:
      mmdagent.procKeyMessage((char) wParam);
      break;
   case WM_MMDAGENT_COMMAND:
      mmdagent.procCommandMessage((char *) wParam, (char *) lParam);
      break;
   case WM_MMDAGENT_EVENT:
      mmdagent.procEventMessage((char *) wParam, (char *) lParam);
      break;
   case WM_MMDAGENT_LOG:
      mmdagent.procLogMessage((char *) wParam);
      break;
   case WM_DESTROY:
      mmdagent.procWindowDestroyMessage();
      PostQuitMessage(0);
      break;
   default:
      hit = false;
   }

   if (hit == false)
      return DefWindowProc(hWnd, message, wParam, lParam);

   return 0;
}

/* WinMain: main function */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
   setlocale(LC_CTYPE, "japanese");

   MSG msg;
   HACCEL hAccelTable;
   char szTitle[MAIN_MAXBUFLEN];
   char szWindowClass[MAIN_MAXBUFLEN];

   int i;
   size_t len;
   int argc;
   wchar_t **wargv;
   char **argv;

   WNDCLASSEXA wcex;
   HWND hWnd;

   UNREFERENCED_PARAMETER(hPrevInstance);
   UNREFERENCED_PARAMETER(lpCmdLine);

   LoadStringA(hInstance, IDS_APP_TITLE, szTitle, MAIN_MAXBUFLEN);
   LoadStringA(hInstance, IDC_MMDAGENT, szWindowClass, MAIN_MAXBUFLEN);

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
   RegisterClassExA(&wcex);

   /* create window */
   wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
   if(argc < 1) return false;
   argv = (char **) malloc(sizeof(char *) * argc);
   for(i = 0; i < argc; i++) {
      argv[i] = (char *) malloc(sizeof(char) * MAIN_MAXBUFLEN);
      wcstombs_s(&len, argv[i], MAIN_MAXBUFLEN, wargv[i], _TRUNCATE);
   }
   hWnd = mmdagent.setup(hInstance, szTitle, szWindowClass, argc, argv);
   for(i = 0; i < argc; i++)
      free(argv[i]);
   free(argv);
   if (hWnd == 0) {
      MessageBoxA(NULL, "Error: cannot execute MMDAgent.", "Error", MB_OK);
      return 0;
   }
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MMDAGENT));

   /* initialize variables */
   windowHandleCommandDialogBox = 0;

   while(true) {
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
