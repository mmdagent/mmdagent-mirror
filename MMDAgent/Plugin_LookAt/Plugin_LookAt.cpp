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
#include "BoneController.h"

/* definitions */

#define PLUGINLOOKAT_NAME "LookAt"

/* global variables */

static BoneController headController[MMDAGENT_MAXNMODEL];
static BoneController eyeController[MMDAGENT_MAXNMODEL];
static bool enable;

/* setHeadController: set bone controller to head */
static void setHeadController(BoneController *controller, PMDModel *model)
{
   char *bone[] = {"“ª"};
   controller->setup(model, bone, 1, 0.150f, 0.008f, 0.0f, 0.0f, 1.0f, 20.0f, 60.0f, 0.0f, -45.0f, -60.0f, 0.0f, 0.0f, -1.0f, 0.0f);
}

/* setEyeController: set eye controller to eyes */
static void setEyeController(BoneController *controller, PMDModel *model)
{
   char *bone[] = {"‰E–Ú", "¶–Ú"};
   controller->setup(model, bone, 2, 0.180f, 0.008f, 0.0f, 0.0f, 1.0f, 5.0f, 5.0f, 0.0f, -5.0f, -5.0f, 0.0f, 0.0f, 0.0f, 0.0f);
}

/* changeLookAt: switch LookAt */
static void changeLookAt(PMDObject *objs, const int num, HWND hWnd)
{
   int i;

   for(i = 0; i < num; i++) {
      if(objs[i].isEnable() == true) {
         if(enable == true) {
            headController[i].setEnableFlag(false);
            eyeController[i].setEnableFlag(false);
         } else {
            headController[i].setEnableFlag(true);
            eyeController[i].setEnableFlag(true);
         }
      }
   }
   enable = !enable;
   if(enable)
      ::PostMessage(hWnd, WM_MMDAGENT_EVENT, (WPARAM) MMDAgent_strdup(MMDAGENT_EVENT_PLUGINENABLE), (LPARAM) MMDAgent_strdup(PLUGINLOOKAT_NAME));
   else
      ::PostMessage(hWnd, WM_MMDAGENT_EVENT, (WPARAM) MMDAgent_strdup(MMDAGENT_EVENT_PLUGINDISABLE), (LPARAM) MMDAgent_strdup(PLUGINLOOKAT_NAME));
}

/* extAppStart: initialize controller */
void __stdcall extAppStart(MMDAgent *m)
{
   enable = false;
}

/* extWindowProc: process message */
void __stdcall extWindowProc(MMDAgent *mmdagent, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   int id;
   char *mes1, *mes2;
   char *p, *buf, *save;
   PMDObject *objs;

   if(message == WM_MMDAGENT_COMMAND) {
      objs = mmdagent->getModelList();
      mes1 = (char *) wParam;
      mes2 = (char *) lParam;
      if(MMDAgent_strequal(mes1, MMDAGENT_COMMAND_PLUGINENABLE)) {
         if(MMDAgent_strequal(mes2, PLUGINLOOKAT_NAME) && enable == false)
            changeLookAt(objs, mmdagent->getNumModel(), hWnd);
      } else if(MMDAgent_strequal(mes1, MMDAGENT_COMMAND_PLUGINDISABLE)) {
         if(MMDAgent_strequal(mes2, PLUGINLOOKAT_NAME) && enable == true)
            changeLookAt(objs, mmdagent->getNumModel(), hWnd);
      }
   } else if(message == WM_MMDAGENT_EVENT) {
      objs = mmdagent->getModelList();
      mes1 = (char *) wParam;
      mes2 = (char *) lParam;
      if(MMDAgent_strequal(mes1, MMDAGENT_EVENT_KEY)) {
         if(mes2 != NULL && MMDAgent_strequal(mes2, "L")) {
            changeLookAt(objs, mmdagent->getNumModel(), hWnd);
         }
      } else if(MMDAgent_strequal(mes1, MMDAGENT_EVENT_MODELCHANGE) || MMDAgent_strequal(mes1, MMDAGENT_EVENT_MODELADD)) {
         buf = MMDAgent_strdup(mes2);
         p = MMDAgent_strtok(buf, "|", &save);
         if(p) {
            id = mmdagent->findModelAlias(p);
            if(id != -1) {
               setHeadController(&headController[id], objs[id].getPMDModel());
               setEyeController(&eyeController[id], objs[id].getPMDModel());
            }
         }
         if(buf != NULL)
            free(buf);
      }
   }
}

/* extUpdate: update motions */
void __stdcall extUpdate(MMDAgent *mmdagent, double deltaFrame)
{
   int i;
   float rate;
   PMDObject *objs;
   btVector3 targetPos, pointPos;
   HWND hWnd = mmdagent->getWindowHandler();
   RECT rc;
   POINT pos;

   /* set target position */
   if (!GetWindowRect(hWnd, &rc)) return;
   if (!GetCursorPos(&pos)) return;
   pos.x -= (rc.left + rc.right) / 2;
   pos.y -= (rc.top + rc.bottom) / 2;
   rate = 100.0f / (float)(rc.right - rc.left);
   pointPos.setValue(pos.x * rate, -pos.y * rate, 0.0f);
   mmdagent->getScreenPointPosition(&targetPos, &pointPos);

   /* calculate direction of all controlled bones */
   objs = mmdagent->getModelList();
   for (i = 0; i < mmdagent->getNumModel(); i++) {
      if (objs[i].isEnable() == true) {
         headController[i].update(&targetPos, (float) deltaFrame);
         eyeController[i].update(&targetPos, (float) deltaFrame);
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
