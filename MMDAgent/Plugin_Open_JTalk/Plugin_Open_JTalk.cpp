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

#include "MMDAgent.h"

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
#include "Open_JTalk_Manager.h"

/* definitions */

#define PLUGINOPENJTALK_NAME         "Open_JTalk"
#define PLUGINOPENJTALK_DIRSEPARATOR '\\'

#define PLUGINOPENJTALK_STARTCOMMAND "SYNTH_START"
#define PLUGINOPENJTALK_STOPCOMMAND  "SYNTH_STOP"

/* global variables */

static Open_JTalk_Manager open_jtalk_manager;
static bool enable;

/* extAppStart: load models and start thread */
void __stdcall extAppStart(MMDAgent *m)
{
   int len;
   char dic_dir[OPENJTALK_MAXBUFLEN];
   char *config;

   /* get dictionary directory name */
   sprintf(dic_dir, "%s%c%s", m->getAppDirName(), PLUGINOPENJTALK_DIRSEPARATOR, PLUGINOPENJTALK_NAME);

   /* get config file */
   config = MMDAgent_strdup(m->getConfigFileName());
   len = MMDAgent_strlen(config);

   /* load */
   if (len > 4) {
      config[len-4] = '.';
      config[len-3] = 'o';
      config[len-2] = 'j';
      config[len-1] = 't';
      open_jtalk_manager.loadAndStart(m->getWindowHandler(), WM_MMDAGENT_EVENT, WM_MMDAGENT_COMMAND, dic_dir, config);
   }

   if(config)
      free(config);

   enable = true;
   ::PostMessage(m->getWindowHandler(), WM_MMDAGENT_EVENT, (WPARAM) MMDAgent_strdup(MMDAGENT_EVENT_PLUGINENABLE), (LPARAM) MMDAgent_strdup(PLUGINOPENJTALK_NAME));
}

/* extWindowProc: process message */
void __stdcall extWindowProc(MMDAgent *m, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   char *mes1;
   char *mes2;

   if(enable == true) {
      if(message == WM_MMDAGENT_COMMAND) {
         mes1 = (char *) wParam;
         mes2 = (char *) lParam;
         if(MMDAgent_strequal(mes1, MMDAGENT_COMMAND_PLUGINDISABLE)) {
            if(MMDAgent_strequal(mes2, PLUGINOPENJTALK_NAME)) {
               enable = false;
               ::PostMessage(hWnd, WM_MMDAGENT_EVENT, (WPARAM) MMDAgent_strdup(MMDAGENT_EVENT_PLUGINDISABLE), (LPARAM) MMDAgent_strdup(PLUGINOPENJTALK_NAME));
            }
         } else if (open_jtalk_manager.isRunning()) {
            if (MMDAgent_strequal(mes1, PLUGINOPENJTALK_STARTCOMMAND)) {
               open_jtalk_manager.synthesis(mes2);
            } else if (MMDAgent_strequal(mes1, PLUGINOPENJTALK_STOPCOMMAND)) {
               open_jtalk_manager.stop(mes2);
            }
         }
      }
   } else {
      if(message == WM_MMDAGENT_COMMAND) {
         mes1 = (char *) wParam;
         mes2 = (char *) lParam;
         if(MMDAgent_strequal(mes1, MMDAGENT_COMMAND_PLUGINENABLE)) {
            if(MMDAgent_strequal(mes2, PLUGINOPENJTALK_NAME)) {
               enable = true;
               ::PostMessage(hWnd, WM_MMDAGENT_EVENT, (WPARAM) MMDAgent_strdup(MMDAGENT_EVENT_PLUGINENABLE), (LPARAM) MMDAgent_strdup(PLUGINOPENJTALK_NAME));
            }
         }
      }
   }
}

/* extAppEnd: stop and free thread */
void __stdcall extAppEnd(MMDAgent *m)
{
   open_jtalk_manager.stopAndRelease();
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
