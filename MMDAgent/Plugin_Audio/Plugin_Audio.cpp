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
#include <MMSystem.h>

#include "MMDAgent.h"
#include "Audio.h"

/* global variables */

Audio audio;

/* extWindowCreate: initialize audio */
void __stdcall extWindowCreate(MMDAgent *m, HWND hWnd)
{
   audio.setup(hWnd);
}

/* extWindowProc: process message */
void __stdcall extWindowProc(MMDAgent * m, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   int i;
   char *mes1, *mes2;
   char *buf, *p, *q;
   char *aliasA, *fileA;
   wchar_t *aliasW, *fileW;
   size_t len;

   if(message == MM_MCINOTIFY) {
      /* audio stop event */
      aliasA = audio.getFinishedAlias(wParam, lParam);
      if (aliasA) {
         audio.stop(aliasA);
         mes1 = strdup(MMDAGENTCOMMAND_SOUNDEVENTSTOP);
         mes2 = strdup(aliasA);
         ::PostMessage(hWnd, WM_MMDAGENT_EVENT, (WPARAM) mes1, (LPARAM) mes2);
      }
   } else if(message == WM_MMDAGENT_COMMAND) {
      mes1 = (char *) wParam;
      mes2 = (char *) lParam;
      if(mes1 != NULL) {
         if(strcmp(mes1, MMDAGENT_COMMAND_SOUND_START) == 0) {
            /* audio start command */
            if(mes2 != NULL) {
               buf = _strdup(mes2);
               for(i = 0, p = strtok_s(buf, "|", &q); p; i++, p = strtok_s(NULL, "|", &q)) {
                  if(i == 0)
                     aliasA = p;
                  else if(i == 1)
                     fileA = p;
               }
               if(i == 2) {
                  audio.stop(aliasA);
                  if(audio.play(aliasA, fileA) == true)
                     ::PostMessage(hWnd, WM_MMDAGENT_EVENT, (WPARAM) _strdup(MMDAGENTCOMMAND_SOUNDEVENTSTART), (LPARAM) _strdup(aliasA));
               }
               free(buf);
            }
         } else if(strcmp(mes1, MMDAGENT_COMMAND_SOUND_STOP) == 0) {
            /* audio stop command */
            if(mes2 != NULL) 
               audio.stop(mes2);
            
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
