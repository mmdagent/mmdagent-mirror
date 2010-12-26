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

/* for DLL */

#ifndef WINVER
#define WINVER 0x0600
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0410
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0700
#endif

#define WIN32_LEAN_AND_MEAN

/* headers */

#include <windows.h>
#include <locale.h>

#include "VIManager.h"
#include "VIManager_Thread.h"

#include "MMDAgent.h"

/* global variance */

VIManager_Thread vimanager_thread;

/* extWindowCreate: load FST and start thread */
void __stdcall extWindowCreate(MMDAgent *m, HWND hWnd)
{
   wchar_t *buf1;
   char buf2[VIMANAGER_MAXBUFLEN];
   size_t len2;

   setlocale(LC_CTYPE, "japanese");

   buf1 = m->getConfigFileName();
   wcstombs_s(&len2, buf2, VIMANAGER_MAXBUFLEN, buf1, _TRUNCATE);
   if (len2 > 5) {
      buf2[len2-5] = '.';
      buf2[len2-4] = 'f';
      buf2[len2-3] = 's';
      buf2[len2-2] = 't';
      vimanager_thread.loadAndStart(hWnd, WM_MMDAGENT_COMMAND, buf2);
   }
}

/* extWindowProc: catch dialog message */
void __stdcall extWindowProc(MMDAgent *m, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   wchar_t *mes1;
   wchar_t *mes2;
   char buf1[VIMANAGER_MAXBUFLEN];
   char buf2[VIMANAGER_MAXBUFLEN];
   size_t len1;
   size_t len2;

   if (vimanager_thread.isStarted()) {
      if (message == WM_MMDAGENT_EVENT) {
         mes1 = (wchar_t *) wParam;
         mes2 = (wchar_t *) lParam;
         if (mes1 != NULL && wcslen(mes1) > 0) {
            wcstombs_s(&len1, buf1, VIMANAGER_MAXBUFLEN, mes1, _TRUNCATE);
            if (len1 > 0) {
               if (mes2 != NULL && wcslen(mes2) > 0)
                  wcstombs_s(&len2, buf2, VIMANAGER_MAXBUFLEN, mes2, _TRUNCATE);
               else
                  strcpy(buf2, "");
               vimanager_thread.enqueueBuffer(buf1, buf2); /* enqueue */
            }
         }
      }
   }
}

/* DllMain: main for DLL */
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
   switch (ul_reason_for_call) {
   case DLL_PROCESS_ATTACH:
   case DLL_THREAD_ATTACH:
   case DLL_THREAD_DETACH:
   case DLL_PROCESS_DETACH:
      break;
   }
   return true;
}
