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

/* headers */

#include <windows.h>

#include "MMDAgent.h"
#include "Plugin.h"
#include "MMDAgent_command.h"
#include "utils.h"

#define MAXBONEPERCONTROL 10

/* BoneControlDef: bone control definition */
struct BoneControlDef {
   char name[21];                        /* name */
   char boneName[MAXBONEPERCONTROL][21]; /* target bone name */
   unsigned short boneNum;               /* number of target name */
   float rateOn;                         /* speed rate when switch is on */
   float rateOff;                        /* speed rate when switch is off */
   btVector3 baseVector;                 /* normalized base vector */
   btVector3 LowerAngularLimit;          /* lower angular limit */
   btVector3 UpperAngularLimit;          /* upper angular limit */
   btVector3 adjustPosition;             /* offset to adjust target position */

   BoneControlDef() {
      name[0] = '\0';
   }
};

/* BoneController: control bone */
struct BoneController {
   BoneControlDef *def;
   unsigned short boneNum;
   PMDBone *bone[MAXBONEPERCONTROL];
   PMDBone **childBoneList;
   unsigned short childBoneNum;
   bool enabled;
   btQuaternion rot[MAXBONEPERCONTROL];
   float fadingRate;

   BoneController() {
      childBoneList = NULL;
   }

   ~BoneController() {
      if (childBoneList) free(childBoneList);
   }

   void initialize(PMDModel *pmd, BoneControlDef *defArg) {
      if (defArg) def = defArg;

      /* get list of child bones to be updated */
      if (childBoneList) {
         free(childBoneList);
         childBoneList = NULL;
      }

      if (! def) {
         boneNum = 0;
         childBoneNum = 0;
         reset();
         return;
      }

      int n = 0;
      /* look for target bones */
      for (int i = 0; i < def->boneNum; i++) {
         PMDBone *b = pmd->getBone(def->boneName[i]);
         if (b) bone[n++] = b;
      }
      boneNum = n;

      childBoneNum = pmd->getNumBone();
      if (childBoneNum > 0) {
         childBoneList = (PMDBone **) malloc(sizeof(PMDBone *) * childBoneNum);
         int ret = pmd->getChildBoneList(bone, boneNum, childBoneList, childBoneNum);
         if (ret < 0) {
            free(childBoneList);
            childBoneList = NULL;
            childBoneNum = 0;
         } else {
            childBoneNum = ret;
            /* remove simulated bones */
            int n = 0;
            for (int i = 0; i < childBoneNum; i++) {
               if (! childBoneList[i]->isSimulated()) {
                  if (n != i) childBoneList[n] = childBoneList[i];
                  n++;
               }
            }
            childBoneNum = n;
         }
      }

      reset();
   }
   void reset() {
      enabled = false;
      fadingRate = 0.0f;
   }
   void enable() {
      enabled = true;
      for (int i = 0; i < boneNum; i++)
         rot[i] = (*(bone[i]->getCurrentRotation()));
   }
   void disable() {
      enabled = false;
      fadingRate = 1.0f;
   }
   void update(btVector3 *lookAtPos, float deltaFrame) {
      if (enabled) {
         /* increase rate */
         float rate = def->rateOn * deltaFrame;
         if (rate > 1.0f) rate = 1.0f;
         if (rate < 0.0f) rate = 0.0f;
         /* set offset of target position */
         btVector3 v = *lookAtPos + def->adjustPosition;
         for (int i = 0; i < boneNum; i++) {
            /* calculate rotation to target position */
            btVector3 localDest = bone[i]->getTransform()->inverse() * v;
            localDest.normalize();
            float dot = def->baseVector.dot(localDest);
            if (dot > 1.0f) continue;
            float angle = acosf(dot);
            btVector3 axis = def->baseVector.cross(localDest);
            if (axis.length2() < 0.0000001f) continue;
            axis.normalize();
            btQuaternion targetRot = btQuaternion(axis, btScalar(angle));
            /* set limit of rotation */
            btScalar x, y, z;
            btMatrix3x3 mat;
            mat.setRotation(targetRot);
            mat.getEulerZYX(z, y, x);
            if (x > def->UpperAngularLimit.x()) x = def->UpperAngularLimit.x();
            if (y > def->UpperAngularLimit.y()) y = def->UpperAngularLimit.y();
            if (z > def->UpperAngularLimit.z()) z = def->UpperAngularLimit.z();
            if (x < def->LowerAngularLimit.x()) x = def->LowerAngularLimit.x();
            if (y < def->LowerAngularLimit.y()) y = def->LowerAngularLimit.y();
            if (z < def->LowerAngularLimit.z()) z = def->LowerAngularLimit.z();
            targetRot.setEulerZYX(z, y, x);
            /* slerp from current rotation to target rotation */
            rot[i] = rot[i].slerp(targetRot, rate);
            /* set result to current rotation */
            bone[i]->setCurrentRotation(&rot[i]);
         }
         /* update bone transform matrices */
         for (int i = 0; i < boneNum; i++) bone[i]->update();
         for (int i = 0; i < childBoneNum; i++) childBoneList[i]->update();
      } else {
         btQuaternion tmpRot;
         /* spin target bone slowly */
         if (fadingRate > 0.0f) {
            /* decrement rate */
            fadingRate -= def->rateOff * deltaFrame;
            if (fadingRate < 0.0f) fadingRate = 0.0f;
            /* rate multiplication for bone rotation */
            for (int i = 0; i < boneNum; i++) {
               tmpRot = (*(bone[i]->getCurrentRotation()));
               rot[i] = tmpRot.slerp(rot[i], fadingRate);
               bone[i]->setCurrentRotation(&rot[i]);
            }
            /* update bone transform matrices */
            for (int i = 0; i < boneNum; i++) bone[i]->update();
            for (int i = 0; i < childBoneNum; i++) childBoneList[i]->update();
         }
      }
   }
};

static BoneControlDef g_controlDef[4]; /* bone controll definitions */
static BoneController g_controllers[MAXMODEL][4]; /* controller */
static int g_defNum = 0;

/* createDefaultControlDefinitions: create controller definitions */
static void createDefaultControlDefinitions()
{
   BoneControlDef *def;

   g_defNum = 0;

   /* neck */
   def = &g_controlDef[0];
   strncpy(def->name, "UŒüŽñ", 20);
   def->boneNum = 1;
   strncpy(def->boneName[0], "“ª", 20);
   def->baseVector = btVector3(0.0, 0.0, 1.0);
   def->rateOn = 0.150f;
   def->rateOff = 0.008f;
   def->LowerAngularLimit = btVector3(RAD(-45.0f), RAD(-60.0f), RAD(0.0f));
   def->UpperAngularLimit = btVector3(RAD(20.0f), RAD(60.0f), RAD(0.0f));
   def->adjustPosition = btVector3(0.0f, -1.0f, 0.0f);
   /* eye */
   def = &g_controlDef[1];
   strncpy(def->name, "UŒü–Ú", 20);
   def->boneNum = 2;
   strncpy(def->boneName[0], "‰E–Ú", 20);
   strncpy(def->boneName[1], "¶–Ú", 20);
   def->baseVector = btVector3(0.0, 0.0, 1.0);
   def->rateOn = 0.180f;
   def->rateOff = 0.008f;
   def->LowerAngularLimit = btVector3(RAD(-5.0f), RAD(-5.0f), RAD(0.0f));
   def->UpperAngularLimit = btVector3(RAD(5.0f), RAD(5.0f), RAD(0.0f));
   def->adjustPosition = btVector3(0.0f, 0.0f, 0.0f);

   g_defNum = 2;
}

/* execLookAt: update motion */
static void __stdcall execLookAt(MMDAgent *mmdagent, double deltaFrame)
{
   btVector3 targetPos, pointPos;

   /* set view target */
   HWND hWnd = mmdagent->getWindowHandler();
   RECT rc;
   POINT pos;
   if (!GetWindowRect(hWnd, &rc)) return;
   if (!GetCursorPos(&pos)) return;
   pos.x -= (rc.left + rc.right) / 2;
   pos.y -= (rc.top + rc.bottom) / 2;
   float rate = 100.0f / (float)(rc.right - rc.left);
   float x = pos.x * rate;
   float y = - pos.y * rate;
   pointPos.setValue(x, y, 0.0f);
   mmdagent->getRender()->getScreenPointPosition(&targetPos, &pointPos);

   /* calculate direction of all controlled bones */
   PMDObject *objs = mmdagent->getModelList();
   for (int i = 0; i < mmdagent->getNumModel(); i++)
      if (objs[i].isEnable() == true)
         for (int j = 0; j < g_defNum; j++)
            g_controllers[i][j].update(&targetPos, (float) deltaFrame);
}

/* execProc: listen event message */
static void __stdcall execProc(MMDAgent *mmdagent, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   PMDObject *objs = mmdagent->getModelList();

   switch (message) {
   case WM_KEYDOWN:
      if (wParam == 'L') {
         for (int i = 0; i < mmdagent->getNumModel(); i++) {
            for (int j = 0; j < g_defNum; j++) {
               if (g_controllers[i][j].enabled) g_controllers[i][j].disable();
               else g_controllers[i][j].enable();
            }
         }
         g_logger.log(L"BoneControl: toggled");
      }
      break;
   case WM_MMDAGENT_EVENT:
      wchar_t *str = (wchar_t *)wParam;
      if (wcsncmp(str, MMDAGENT_EVENT_MODEL_CHANGE, wcslen(MMDAGENT_EVENT_MODEL_CHANGE)) == 0 || wcsncmp(str, MMDAGENT_EVENT_MODEL_ADD, wcslen(MMDAGENT_EVENT_MODEL_ADD)) == 0) {
         wchar_t *buf = (wchar_t *)malloc(sizeof(wchar_t) * (wcslen((wchar_t *)lParam) + 1));
         wchar_t *p, *psave;
         int c = 0;
         int id;
         wcscpy(buf, (wchar_t *)lParam);
         for (p = wcstok_s(buf, L"|", &psave); p; p = wcstok_s(NULL, L"|", &psave)) {
            if (c == 0) {
               id = mmdagent->findModelAlias(p);
               if (id != -1) {
                  for (int j = 0; j < g_defNum; j++) {
                     g_controllers[id][j].initialize(objs[id].getPMDModel(), &(g_controlDef[j]));
                  }
               }
            }
            c++;
         }
         free(buf);
      }
      break;
   }
}

/* initializeLookAt: initialize LookAt */
void initializeLookAt(MMDAgent *mmdagent, PluginList *pluginList)
{
   /* initialize controller */
   createDefaultControlDefinitions();

   PMDObject *objs = mmdagent->getModelList();
   for (int i = 0; i < mmdagent->getNumModel(); i++)
      if (objs[i].isEnable() == true)
         for (int j = 0; j < g_defNum; j++)
            g_controllers[i][j].initialize(objs[i].getPMDModel(), &(g_controlDef[j]));

   /* register */
   if (pluginList->addPlugin(L"Internal: LookUp", NULL, NULL, NULL, execProc, execLookAt, NULL) == false)
      g_logger.log(L"! Error: BoneControl: cannot load internal plugin");
}
