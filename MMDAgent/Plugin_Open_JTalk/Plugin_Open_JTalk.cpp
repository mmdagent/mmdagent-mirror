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

#include "mecab.h"
#include "njd.h"
#include "jpcommon.h"
#include "HTS_engine.h"

#include "text2mecab.h"
#include "mecab2njd.h"
#include "njd2jpcommon.h"

#include "njd_set_pronunciation.h"
#include "njd_set_digit.h"
#include "njd_set_accent_phrase.h"
#include "njd_set_accent_type.h"
#include "njd_set_unvoiced_vowel.h"
#include "njd_set_long_vowel.h"

#include "Open_JTalk.h"
#include "Open_JTalk_Thread.h"

#include "MMDAgent.h"

/* global variables */

#define PLUGINOPENJTALK_NAME         "Open_JTalk"
#define PLUGINOPENJTALK_DIRSEPARATOR '\\'
#define PLUGINOPENJTALK_NTHREAD      2

#define PLUGINOPENJTALK_STARTCOMMAND "SYNTH_START"
#define PLUGINOPENJTALK_STOPCOMMAND  "SYNTH_STOP"

int open_jtalk_thread_index;
Open_JTalk_Thread open_jtalk_thread_list[PLUGINOPENJTALK_NTHREAD];

/* extWindowCreate: load models and start thread */
void __stdcall extWindowCreate(MMDAgent *m, HWND hWnd)
{
   int i;
   size_t size;
   char dic_dir[OPENJTALK_MAXBUFLEN];
   char *config;
   char current_dir[OPENJTALK_MAXBUFLEN];

   open_jtalk_thread_index = 0;

   /* get dictionary directory name */
   sprintf(dic_dir, "%s%c%s", m->getAppDirName(), PLUGINOPENJTALK_DIRSEPARATOR, PLUGINOPENJTALK_NAME);

   /* get config file */
   config = strdup(m->getConfigFileName());
   size = strlen(config);

   /* save current directory and move directory */
   GetCurrentDirectoryA(OPENJTALK_MAXBUFLEN, current_dir);
   SetCurrentDirectoryA(m->getConfigDirName());

   /* load */
   if (size > 4) {
      config[size-4] = '.';
      config[size-3] = 'o';
      config[size-2] = 'j';
      config[size-1] = 't';
      for (i = 0; i < PLUGINOPENJTALK_NTHREAD; i++)
         open_jtalk_thread_list[i].loadAndStart(hWnd, WM_MMDAGENT_EVENT, WM_MMDAGENT_COMMAND, dic_dir, config);
   }

   /* move directory */
   SetCurrentDirectoryA(current_dir);

   free(config);
}

/* extWindowProc: catch message */
void __stdcall extWindowProc(MMDAgent *m, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   int i;
   char *buf1;
   char *buf2;

   if (open_jtalk_thread_list[open_jtalk_thread_index].isRunning()) {
      if (message == WM_MMDAGENT_COMMAND) {
         buf1 = (char *) wParam;
         buf2 = (char *) lParam;
         if (buf1 != NULL) {
            if (strcmp(buf1, PLUGINOPENJTALK_STARTCOMMAND) == 0 && buf2 != NULL) {
               /* SYNTH_START command */
               open_jtalk_thread_list[open_jtalk_thread_index].setSynthParameter(buf2);
               open_jtalk_thread_index++;
               if (open_jtalk_thread_index >= PLUGINOPENJTALK_NTHREAD)
                  open_jtalk_thread_index = 0;
            } else if (strcmp(buf1, PLUGINOPENJTALK_STOPCOMMAND) == 0) {
               /* SYNTH_STOP command */
               for (i = 0; i < PLUGINOPENJTALK_NTHREAD; i++)
                  open_jtalk_thread_list[i].stop();
            }
         }
      }
   }
}

/* extAppEnd: stop and free thread */
void __stdcall extAppEnd(MMDAgent *m)
{
   int i;

   for(i = 0; i < PLUGINOPENJTALK_NTHREAD; i++)
      open_jtalk_thread_list[i].stopAndRelease();
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
