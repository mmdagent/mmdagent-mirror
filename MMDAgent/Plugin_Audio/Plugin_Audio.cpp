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

#include "Audio_Thread.h"
#include "Audio_Manager.h"

/* definitions */

#define PLUGINAUDIO_NAME         "Audio"
#define PLUGINAUDIO_DEFAULTALIAS "audio"

#define PLUGINAUDIO_STARTCOMMAND "SOUND_START"
#define PLUGINAUDIO_STOPCOMMAND  "SOUND_STOP"

/* global variables */

static Audio_Manager audio_manager;
static bool enable;

/* extAppStart: setup and start thread */
void __stdcall extAppStart(MMDAgent *mmdagent)
{
   audio_manager.setupAndStart(mmdagent->getWindowHandler(), WM_MMDAGENT_EVENT);

   enable = true;
   ::PostMessage(mmdagent->getWindowHandler(), WM_MMDAGENT_EVENT, (WPARAM) MMDAgent_strdup(MMDAGENT_EVENT_PLUGINENABLE), (LPARAM) MMDAgent_strdup(PLUGINAUDIO_NAME));
}

/* extProcCommand: process command message */
void __stdcall extProcCommand(MMDAgent *mmdagent, const char *type, const char *args)
{
   if(enable == true) {
      if(MMDAgent_strequal(type, MMDAGENT_COMMAND_PLUGINDISABLE)) {
         if(MMDAgent_strequal(args, PLUGINAUDIO_NAME)) {
            enable = false;
            ::PostMessage(mmdagent->getWindowHandler(), WM_MMDAGENT_EVENT, (WPARAM) MMDAgent_strdup(MMDAGENT_EVENT_PLUGINDISABLE), (LPARAM) MMDAgent_strdup(PLUGINAUDIO_NAME));
         }
      } else if (audio_manager.isRunning()) {
         if (MMDAgent_strequal(type, PLUGINAUDIO_STARTCOMMAND)) {
            audio_manager.play(args);
         } else if (MMDAgent_strequal(type, PLUGINAUDIO_STOPCOMMAND)) {
            audio_manager.stop(args);
         }
      }
   } else {
      if(MMDAgent_strequal(type, MMDAGENT_COMMAND_PLUGINENABLE)) {
         if(MMDAgent_strequal(args, PLUGINAUDIO_NAME)) {
            enable = true;
            ::PostMessage(mmdagent->getWindowHandler(), WM_MMDAGENT_EVENT, (WPARAM) MMDAgent_strdup(MMDAGENT_EVENT_PLUGINENABLE), (LPARAM) MMDAgent_strdup(PLUGINAUDIO_NAME));
         }
      }
   }
}

/* extProcEvent: process event message */
void __stdcall extProcEvent(MMDAgent *mmdagent, const char *type, const char *args)
{
   int i;
   char *buf, *p, *q;

   if(enable == true) {
      if(MMDAgent_strequal(type, MMDAGENT_EVENT_DRAGANDDROP)) {
         buf = MMDAgent_strdup(args);
         p = MMDAgent_strtok(buf, "|", &q);
         if(MMDAgent_strtailmatch(p, ".vmd")) {
            i = MMDAgent_strlen(p);
            p[i-4] = '.';
            p[i-3] = 'm';
            p[i-2] = 'p';
            p[i-1] = '3';
            audio_manager.stop(PLUGINAUDIO_DEFAULTALIAS);
            q = (char *) malloc(sizeof(char) * (strlen(PLUGINAUDIO_DEFAULTALIAS) + 1 + strlen(p) + 1));
            sprintf(q, "%s|%s", PLUGINAUDIO_DEFAULTALIAS, p);
            audio_manager.play(q);
            free(q);
         }
         if(buf)
            free(buf);
      }
   }
}

/* extAppEnd: stop and free thread */
void __stdcall extAppEnd(MMDAgent *mmdagent)
{
   audio_manager.stopAndRelease();
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
