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

#include "MMDAgent.h"
#include "BoneController.h"

/* global variables */

bool enable;
BoneController neckController[MMDAGENT_MAXNMODEL];
BoneController eyeController[MMDAGENT_MAXNMODEL];

static void setNeckController(BoneController *controller, PMDModel *model)
{
   char *bone[] = {"“ª"};
   controller->setup(model, bone, 1, 0.150f, 0.008f, 0.0f, 0.0f, 1.0f, 20.0f, 60.0f, 0.0f, -45.0f, -60.0f, 0.0f, 0.0f, -1.0f, 0.0f);
}

static void setEyeController(BoneController *controller, PMDModel *model)
{
   char *bone[] = {"‰E–Ú", "¶–Ú"};
   controller->setup(model, bone, 2, 0.180f, 0.008f, 0.0f, 0.0f, 1.0f, 5.0f, 5.0f, 0.0f, -5.0f, -5.0f, 0.0f, 0.0f, 0.0f, 0.0f);
}

/* extWindowCreate: initialize controller */
void __stdcall extWindowCreate(MMDAgent *m, HWND hWnd)
{
   enable = false;
}

/* extWindowProc: process message */
void __stdcall extWindowProc(MMDAgent *mmdagent, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   int i;
   int id;
   char *mes1, *mes2;
   char *p, *buf;
   PMDObject *objs;

   if(message == WM_MMDAGENT_EVENT) {
      objs = mmdagent->getModelList();
      mes1 = (char *) wParam;
      mes2 = (char *) lParam;
      if(mes1 != NULL) {
         if(strcmp(mes1, "KEY") == 0) {
            if(mes2 != NULL && strcmp(mes2, "L") == 0) {
               for(i = 0; i < mmdagent->getNumModel(); i++) {
                  if(objs[i].isEnable() == true) {
                     if(enable == true) {
                        neckController[i].setEnableFlag(false);
                        eyeController[i].setEnableFlag(false);
                     } else {
                        neckController[i].setEnableFlag(true);
                        eyeController[i].setEnableFlag(true);
                     }
                  }
               }
               enable = !enable;
            }
         } else if(strcmp(mes1, MMDAGENTCOMMAND_MODELEVENTCHANGE) == 0 || strcmp(mes1, MMDAGENTCOMMAND_MODELEVENTADD) == 0) {
            if(mes2 != NULL) {
               buf = strdup(mes2);
               for(i = 0, p = strtok(buf, "|"); p; i++, p = strtok(NULL, "|")) {
                  if(i == 0) {
                     id = mmdagent->findModelAlias(p);
                     if(id != -1) {
                        setNeckController(&neckController[id], objs[id].getPMDModel());
                        setEyeController(&eyeController[id], objs[id].getPMDModel());
                     }
                  }
               }
               free(buf);
            }
         }
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

   /* set target position */
   HWND hWnd = mmdagent->getWindowHandler();
   RECT rc;
   POINT pos;
   if (!GetWindowRect(hWnd, &rc)) return;
   if (!GetCursorPos(&pos)) return;
   pos.x -= (rc.left + rc.right) / 2;
   pos.y -= (rc.top + rc.bottom) / 2;
   rate = 100.0f / (float)(rc.right - rc.left);
   pointPos.setValue(pos.x * rate, -pos.y * rate, 0.0f);
   mmdagent->getRender()->getScreenPointPosition(&targetPos, &pointPos);

   /* calculate direction of all controlled bones */
   objs = mmdagent->getModelList();
   for (i = 0; i < mmdagent->getNumModel(); i++) {
      if (objs[i].isEnable() == true) {
         neckController[i].update(&targetPos, (float) deltaFrame);
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
