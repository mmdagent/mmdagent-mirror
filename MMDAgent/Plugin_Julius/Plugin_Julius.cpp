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

#include <windows.h>
#include <locale.h>

#include "julius/juliuslib.h"
#include "Julius_Logger.h"
#include "Julius_Thread.h"
#include "MMDAgent.h"

/* definitions */

#define PLUGINJULIUS_NAME         "Julius"
#define PLUGINJULIUS_DIRSEPARATOR '\\'

#define PLUGINJULIUS_LANGUAGEMODEL "lang_m\\web.60k.8-8.bingramv5.gz"
#define PLUGINJULIUS_DICTIONARY    "lang_m\\web.60k.htkdic"
#define PLUGINJULIUS_ACOUSTICMODEL "phone_m\\clustered.mmf.16mix.all.julius.binhmm"
#define PLUGINJULIUS_TRIPHONELIST  "phone_m\\tri_tied.list.bin"
#define PLUGINJULIUS_CONFIGFILE    "jconf.txt"

/* global variables */

static Julius_Thread julius_thread;
static bool enable;

/* extAppStart: load models and start thread */
void __stdcall extAppStart(MMDAgent *mmdagent)
{
   int len;
   char languageModel[JULIUSTHREAD_MAXBUFLEN];
   char dictionary[JULIUSTHREAD_MAXBUFLEN];
   char acousticModel[JULIUSTHREAD_MAXBUFLEN];
   char triphoneList[JULIUSTHREAD_MAXBUFLEN];
   char configFile[JULIUSTHREAD_MAXBUFLEN];
   char userDictionary[JULIUSTHREAD_MAXBUFLEN];

   /* set model file names */
   sprintf(languageModel, "%s%c%s%c%s", mmdagent->getAppDirName(), PLUGINJULIUS_DIRSEPARATOR, PLUGINJULIUS_NAME, PLUGINJULIUS_DIRSEPARATOR, PLUGINJULIUS_LANGUAGEMODEL);
   sprintf(dictionary, "%s%c%s%c%s", mmdagent->getAppDirName(), PLUGINJULIUS_DIRSEPARATOR, PLUGINJULIUS_NAME, PLUGINJULIUS_DIRSEPARATOR, PLUGINJULIUS_DICTIONARY);
   sprintf(acousticModel, "%s%c%s%c%s", mmdagent->getAppDirName(), PLUGINJULIUS_DIRSEPARATOR, PLUGINJULIUS_NAME, PLUGINJULIUS_DIRSEPARATOR, PLUGINJULIUS_ACOUSTICMODEL);
   sprintf(triphoneList, "%s%c%s%c%s", mmdagent->getAppDirName(), PLUGINJULIUS_DIRSEPARATOR, PLUGINJULIUS_NAME, PLUGINJULIUS_DIRSEPARATOR, PLUGINJULIUS_TRIPHONELIST);
   sprintf(configFile, "%s%c%s%c%s", mmdagent->getAppDirName(), PLUGINJULIUS_DIRSEPARATOR, PLUGINJULIUS_NAME, PLUGINJULIUS_DIRSEPARATOR, PLUGINJULIUS_CONFIGFILE);

   /* user dictionary */
   strcpy(userDictionary, mmdagent->getConfigFileName());
   len = MMDAgent_strlen(userDictionary);
   if(len > 4) {
      userDictionary[len-4] = '.';
      userDictionary[len-3] = 'd';
      userDictionary[len-2] = 'i';
      userDictionary[len-1] = 'c';
   } else {
      strcpy(userDictionary, "");
   }

   /* load models and start thread */
   julius_thread.loadAndStart(mmdagent->getWindowHandler(), WM_MMDAGENT_EVENT, languageModel, dictionary, acousticModel, triphoneList, configFile, userDictionary);

   enable = true;
   ::PostMessage(mmdagent->getWindowHandler(), WM_MMDAGENT_EVENT, (WPARAM) MMDAgent_strdup(MMDAGENT_EVENT_PLUGINENABLE), (LPARAM) MMDAgent_strdup(PLUGINJULIUS_NAME));
}

/* extProcCommand: process command message */
void __stdcall extProcCommand(MMDAgent *mmdagent, const char *type, const char *args)
{
   if(MMDAgent_strequal(type, MMDAGENT_COMMAND_PLUGINDISABLE)) {
      if(MMDAgent_strequal(args, PLUGINJULIUS_NAME) && enable == true) {
         julius_thread.pause();
         enable = false;
         ::PostMessage(mmdagent->getWindowHandler(), WM_MMDAGENT_EVENT, (WPARAM) MMDAgent_strdup(MMDAGENT_EVENT_PLUGINDISABLE), (LPARAM) MMDAgent_strdup(PLUGINJULIUS_NAME));
      }
   } else if(MMDAgent_strequal(type, MMDAGENT_COMMAND_PLUGINENABLE)) {
      if(MMDAgent_strequal(args, PLUGINJULIUS_NAME) && enable == false) {
         julius_thread.resume();
         enable = true;
         ::PostMessage(mmdagent->getWindowHandler(), WM_MMDAGENT_EVENT, (WPARAM) MMDAgent_strdup(MMDAGENT_EVENT_PLUGINENABLE), (LPARAM) MMDAgent_strdup(PLUGINJULIUS_NAME));
      }
   }
}

/* extProcEvent: process event message */
void __stdcall extProcEvent(MMDAgent *mmdagent, const char *type, const char *args)
{
   if(MMDAgent_strequal(type, MMDAGENT_EVENT_KEY)) {
      if(MMDAgent_strequal(args, "J")) {
         if(julius_thread.getLogActiveFlag() == true)
            julius_thread.setLogActiveFlag(false);
         else
            julius_thread.setLogActiveFlag(true);
      }
   }
}

/* extAppEnd: stop thread and free julius */
void __stdcall extAppEnd(MMDAgent *mmdagent)
{
   julius_thread.stopAndRelease();
   enable = false;
}

/* extUpdate: update log view */
void __stdcall extUpdate(MMDAgent *mmdagent, double deltaFrame)
{
   julius_thread.updateLog(deltaFrame);
}

/* extRender: render log view when debug display mode */
void __stdcall extRender(MMDAgent *mmdagent)
{
   julius_thread.renderLog();
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
