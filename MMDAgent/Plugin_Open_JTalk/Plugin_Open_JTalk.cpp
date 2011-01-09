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

#include <stdlib.h>

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

#include "MMDFiles.h"
#include "Option.h"
#include "MMDAgent.h"

/* global variables */

#define PLUGINOPENJTALK_NAME    L"Open_JTalk"
#define PLUGINOPENJTALK_NTHREAD 2

#define PLUGINOPENJTALK_STARTCOMMAND L"SYNTH_START"
#define PLUGINOPENJTALK_STOPCOMMAND  L"SYNTH_STOP"

int open_jtalk_thread_index;
Open_JTalk_Thread open_jtalk_thread_list[PLUGINOPENJTALK_NTHREAD];

/* startSynthesis: start to synthesize speech */
static bool startSynthesis(char *val)
{
   /* check */
   if (open_jtalk_thread_list[open_jtalk_thread_index].isStarted() == false || val == NULL || strlen(val) <= 0)
      return false;

   /* synthesis */
   open_jtalk_thread_list[open_jtalk_thread_index].setSynthParameter(val);

   /* change thread */
   open_jtalk_thread_index++;
   if (open_jtalk_thread_index >= PLUGINOPENJTALK_NTHREAD)
      open_jtalk_thread_index = 0;

   return true;
}

/* extWindowCreate: load models and start thread */
void __stdcall extWindowCreate(MMDAgent *m, HWND hWnd)
{
   int i;
   size_t size;
   wchar_t buff[OPENJTALK_MAXBUFLEN];
   char dic_dir[OPENJTALK_MAXBUFLEN];
   char config[OPENJTALK_MAXBUFLEN];
   wchar_t current_dir[OPENJTALK_MAXBUFLEN];

   open_jtalk_thread_index = 0;

   /* get dictionary directory name */
   wcscpy(buff, m->getAppDirName());
   wcscat(buff, L"\\");
   wcscat(buff, PLUGINOPENJTALK_NAME);
   wcstombs_s(&size, dic_dir, OPENJTALK_MAXBUFLEN, buff, _TRUNCATE);

   /* get config file */
   wcscpy(buff, m->getConfigFileName());
   wcstombs_s(&size, config, OPENJTALK_MAXBUFLEN, buff, _TRUNCATE);

   /* save current directory and move directory */
   GetCurrentDirectory(OPENJTALK_MAXBUFLEN, current_dir);
   SetCurrentDirectory(m->getConfigDirName());

   /* load */
   if (size > 5) {
      config[size-5] = '.';
      config[size-4] = 'o';
      config[size-3] = 'j';
      config[size-2] = 't';
      for (i = 0; i < PLUGINOPENJTALK_NTHREAD; i++)
         open_jtalk_thread_list[i].loadAndStart(hWnd, WM_MMDAGENT_EVENT, WM_MMDAGENT_COMMAND, dic_dir, config);
   }

   /* move directory */
   SetCurrentDirectory(current_dir);
}

/* extWindowProc: catch message */
void __stdcall extWindowProc(MMDAgent *m, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   int i;
   wchar_t *buf1;
   wchar_t *buf2;
   char buf3[OPENJTALK_MAXBUFLEN];
   size_t len3;

   if (open_jtalk_thread_list[open_jtalk_thread_index].isStarted()) {
      if (message == WM_MMDAGENT_COMMAND) {
         buf1 = (wchar_t *) wParam;
         buf2 = (wchar_t *) lParam;
         if (buf1 != NULL) {
            if (wcscmp(buf1, PLUGINOPENJTALK_STARTCOMMAND) == 0 && buf2 != NULL) {
               /* SYNTH_START command */
               wcstombs_s(&len3, buf3, OPENJTALK_MAXBUFLEN, buf2, _TRUNCATE);
               if (len3 > 0) {
                  open_jtalk_thread_list[open_jtalk_thread_index].setSynthParameter(buf3);
                  open_jtalk_thread_index++;
                  if (open_jtalk_thread_index >= PLUGINOPENJTALK_NTHREAD)
                     open_jtalk_thread_index = 0;
               }
            } else if (wcscmp(buf1, PLUGINOPENJTALK_STOPCOMMAND) == 0) {
               /* SYNTH_STOP command */
               for (i = 0; i < PLUGINOPENJTALK_NTHREAD; i++)
                  open_jtalk_thread_list[i].stop();
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
