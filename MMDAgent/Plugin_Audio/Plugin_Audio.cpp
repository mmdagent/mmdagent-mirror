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

#define PLUGINAUDIO_COMMANDSTART "SOUND_START"
#define PLUGINAUDIO_COMMANDSTOP  "SOUND_STOP"
#define PLUGINAUDIO_EVENTSTART   "SOUND_EVENT_START"
#define PLUGINAUDIO_EVENTSTOP    "SOUND_EVENT_STOP"

Audio audio;

/* extAppStart: initialize audio */
void __stdcall extAppStart(MMDAgent *m)
{
   audio.setup(m->getWindowHandler());
}

/* extWindowProc: process message */
void __stdcall extWindowProc(MMDAgent * m, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   int i;
   char *mes1, *mes2;
   char *buf, *p, *q;
   char *alias, *file;

   if(message == MM_MCINOTIFY) {
      /* audio stop event */
      alias = audio.getFinishedAlias(wParam, lParam);
      if (alias) {
         audio.stop(alias);
         ::PostMessage(hWnd, WM_MMDAGENT_EVENT, (WPARAM) MMDAgent_strdup(PLUGINAUDIO_EVENTSTOP), (LPARAM) MMDAgent_strdup(alias));
      }
   } else if(message == WM_MMDAGENT_COMMAND) {
      mes1 = (char *) wParam;
      mes2 = (char *) lParam;
      if(mes1 != NULL) {
         if(strcmp(mes1, PLUGINAUDIO_COMMANDSTART) == 0) {
            /* audio start command */
            buf = MMDAgent_strdup(mes2);
            for(i = 0, p = MMDAgent_strtok(buf, "|", &q); p; i++, p = MMDAgent_strtok(NULL, "|", &q)) {
               if(i == 0)
                  alias = p;
               else if(i == 1)
                  file = p;
            }
            if(i == 2) {
               audio.stop(alias);
               if(audio.play(alias, file) == true)
                  ::PostMessage(hWnd, WM_MMDAGENT_EVENT, (WPARAM) MMDAgent_strdup(PLUGINAUDIO_EVENTSTART), (LPARAM) MMDAgent_strdup(alias));
            }
            if(mes2 != NULL)
               free(buf);
         } else if(strcmp(mes1, PLUGINAUDIO_COMMANDSTOP) == 0) {
            /* audio stop command */
            if(mes2 != NULL)
               audio.stop(mes2);
         }
      }
   } else if(message == WM_MMDAGENT_EVENT) {
      mes1 = (char *) wParam;
      mes2 = (char *) lParam;
      if(MMDAgent_strequal(mes1, MMDAGENT_EVENT_DRAGANDDROP)) {
         buf = MMDAgent_strdup(mes2);
         p = MMDAgent_strtok(buf, "|", &q);
         if(MMDAgent_strtailmatch(p, ".vmd")) {
            i = MMDAgent_strlen(p);
            p[i-4] = '.';
            p[i-3] = 'm';
            p[i-2] = 'p';
            p[i-1] = '3';
            audio.stop("audio");
            if(audio.play("audio", p) == true)
               ::PostMessage(hWnd, WM_MMDAGENT_EVENT, (WPARAM) MMDAgent_strdup(PLUGINAUDIO_EVENTSTART), (LPARAM) MMDAgent_strdup("audio"));
         }
         if(buf)
            free(buf);
      }
   }
}

/* extAppEnd: release audio */
void __stdcall extAppEnd(MMDAgent *m)
{
   audio.release();
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
