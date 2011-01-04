/* ----------------------------------------------------------------- */
/*           Toolkit for Building Voice Interaction Systems          */
/*           MMDAgent developed by MMDAgent Project Team             */
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

#include "Option.h"
#include "MMDAgent.h"
#include "utils.h"
#include "MMDAgent_command.h"

#define MMDAGENT_MAXNUMCOMMAND    10
#define MMDAGENT_MAXCOMMANDBUFLEN 1024
#define MMDAGENT_MAXLIPSYNCBUFLEN MMDAGENT_MAXCOMMANDBUFLEN

/* arg2floatArray: parse float array from string */
static bool arg2floatArray(float *dst, int len, wchar_t *arg)
{
   int n;
   wchar_t *buf, *p, *psave;

   buf = (wchar_t *) malloc(sizeof(wchar_t) * (wcslen(arg) + 1));
   wcscpy(buf, arg);
   n = 0;
   for (p = wcstok_s(buf, L"(,)", &psave); p ; p = wcstok_s(NULL, L"(,)", &psave)) {
      if (n < len)
         dst[n] = (float) _wtof(p);
      n++;
   }
   free(buf);

   if (n != len)
      return false;
   else
      return true;
}

/* arg2pos: get position from string */
static bool arg2pos(btVector3 *dst, wchar_t *arg)
{
   float f[3];

   if (arg2floatArray(f, 3, arg) == false)
      return false;

   dst->setZero();
   dst->setValue(f[0], f[1], f[2]);

   return true;
}

/* arg2rot: get rotation from string */
static bool arg2rot(btQuaternion *dst, wchar_t *arg)
{
   float angle[3];

   if (arg2floatArray(angle, 3, arg) == false)
      return false;

   dst->setEulerZYX(RAD(angle[2]), RAD(angle[1]), RAD(angle[0]));

   return true;
}

/* MMDAgent::command: decode command string from client and call process */
bool MMDAgent::command(wchar_t *command, wchar_t *wstr)
{
   static wchar_t warg[MMDAGENT_MAXNUMCOMMAND][MMDAGENT_MAXCOMMANDBUFLEN]; /* static buffer */
   int num = 0;
   wchar_t *buf;

   wchar_t *tmpStr1, *tmpStr2;
   bool tmpBool1, tmpBool2, tmpBool3, tmpBool4;
   float tmpFloat;
   btVector3 tmpPos;
   btQuaternion tmpRot;
   float tmpFloatList[3];

   /* divide string into arguments */
   if (wstr == NULL) {
      g_logger.log( L"<%s>", command);
      num = 0;
   } else {
      g_logger.log( L"<%s|%s>", command, wstr );
      buf = (wchar_t *) malloc(sizeof(wchar_t) * (wcslen(wstr) + 1));
      wcscpy(buf, wstr);
      for (tmpStr1 = wcstokWithDoubleQuotation(buf, L"|", &tmpStr2); tmpStr1; tmpStr1 = wcstokWithDoubleQuotation(NULL, L"|", &tmpStr2)) {
         if (num >= MMDAGENT_MAXNUMCOMMAND) {
            g_logger.log(L"! Error: too many argument in command %s: %s", command, wstr);
            free(buf);
            return false;
         }
         wcsncpy(warg[num], tmpStr1, MMDAGENT_MAXCOMMANDBUFLEN);
         warg[num][MMDAGENT_MAXCOMMANDBUFLEN - 1] = L'\0';
         num++;
      }
      free(buf);
   }

   if (wcscmp(command, MMDAGENT_COMMAND_MODEL_ADD) == 0) {
      tmpStr1 = NULL;
      tmpStr2 = NULL;
      if (num < 2 || num > 6) {
         g_logger.log(L"! Error: %s: wrong number of arguments", command);
         return false;
      }
      if (num >= 3) {
         if (arg2pos(&tmpPos, warg[2]) == false) {
            g_logger.log(L"! Error: %s: not a position string: %s", command, warg[2]);
            return false;
         }
      } else {
         tmpPos = btVector3(0.0, 0.0, 0.0);
      }
      if (num >= 4) {
         if (arg2rot(&tmpRot, warg[3]) == false) {
            g_logger.log(L"! Error: %s: not a rotation string: %s", command, warg[3]);
            return false;
         }
      } else {
         tmpRot.setEulerZYX(0.0, 0.0, 0.0);
      }
      if (num >= 5) {
         tmpStr1 = warg[4];
      }
      if (num >= 6) {
         tmpStr2 = warg[5];
      }
      return addModel(warg[0], warg[1], &tmpPos, &tmpRot, tmpStr1, tmpStr2);
   } else if (wcscmp(command, MMDAGENT_COMMAND_MODEL_CHANGE) == 0) {
      /* change model */
      if (num != 2) {
         g_logger.log(L"! Error: %s: wrong number of arguments", command);
         return false;
      }
      return changeModel(warg[0], warg[1]);
   } else if (wcscmp(command, MMDAGENT_COMMAND_MODEL_DELETE) == 0) {
      /* delete model */
      if (num != 1) {
         g_logger.log(L"! Error: %s: wrong number of arguments", command);
         return false;
      }
      return deleteModel(warg[0]);
   } else if (wcscmp(command, MMDAGENT_COMMAND_MOTION_ADD) == 0) {
      /* add motion */
      tmpBool1 = true; /* full */
      tmpBool2 = true; /* once */
      tmpBool3 = true; /* enableSmooth */
      tmpBool4 = true; /* enableRePos */
      if (num < 3 || num > 7) {
         g_logger.log(L"! Error: %s: too few arguments", command);
         return false;
      }
      if (num >= 4) {
         if (wcscmp(warg[3], L"FULL") == 0) {
            tmpBool1 = true;
         } else if (wcscmp(warg[3], L"PART") == 0) {
            tmpBool1 = false;
         } else {
            g_logger.log(L"! Error: %s: 4th argument should be \"FULL\" or \"PART\"", command);
            return false;
         }
      }
      if (num >= 5) {
         if (wcscmp(warg[4], L"ONCE") == 0) {
            tmpBool2 = true;
         } else if (wcscmp(warg[4], L"LOOP") == 0) {
            tmpBool2 = false;
         } else {
            g_logger.log(L"! Error: %s: 5th argument should be \"ONCE\" or \"LOOP\"", command);
            return false;
         }
      }
      if (num >= 6) {
         if (wcscmp(warg[5], L"ON") == 0) {
            tmpBool3 = true;
         } else if (wcscmp(warg[5], L"OFF") == 0) {
            tmpBool3 = false;
         } else {
            g_logger.log(L"! Error: %s: 6th argument should be \"ON\" or \"OFF\"", command);
            return false;
         }
      }
      if (num >= 7) {
         if (wcscmp(warg[6], L"ON") == 0) {
            tmpBool4 = true;
         } else if (wcscmp(warg[6], L"OFF") == 0) {
            tmpBool4 = false;
         } else {
            g_logger.log(L"! Error: %s: 7th argument should be \"ON\" or \"OFF\"", command);
            return false;
         }
      }
      return addMotion(warg[0], warg[1] , warg[2], tmpBool1, tmpBool2, tmpBool3, tmpBool4);
   } else if (wcscmp(command, MMDAGENT_COMMAND_MOTION_CHANGE) == 0) {
      /* change motion */
      if (num != 3) {
         g_logger.log(L"! Error: %s: wrong number of arguments", command);
         return false;
      }
      return changeMotion(warg[0], warg[1], warg[2]);
   } else if (wcscmp(command, MMDAGENT_COMMAND_MOTION_DELETE) == 0) {
      /* delete motion */
      if (num != 2) {
         g_logger.log(L"! Error: %s: wrong number of arguments", command);
         return false;
      }
      return deleteMotion(warg[0], warg[1]);
   } else if (wcscmp(command, MMDAGENT_COMMAND_MOVE_START) == 0) {
      /* start moving */
      tmpBool1 = false;
      tmpFloat = -1.0;
      if (num < 2 || num > 4) {
         g_logger.log(L"! Error: %s: wrong number of arguments", command);
         return false;
      }
      if (arg2pos(&tmpPos, warg[1]) == false) {
         g_logger.log(L"! Error: %s: not a position string: %s", command, warg[1]);
         return false;
      }
      if (num >= 3) {
         if (wcscmp(warg[2], L"LOCAL") == 0) {
            tmpBool1 = true;
         } else if (wcscmp(warg[2], L"GLOBAL") == 0) {
            tmpBool1 = false;
         } else {
            g_logger.log(L"! Error: %s: 3rd argument should be \"GLOBAL\" or \"LOCAL\"", command);
            return false;
         }
      }
      if (num >= 4)
         tmpFloat = (float) _wtof(warg[3]);
      return startMove(warg[0], &tmpPos, tmpBool1, tmpFloat);
   } else if (wcscmp(command, MMDAGENT_COMMAND_MOVE_STOP) == 0) {
      /* stop moving */
      if (num != 1) {
         g_logger.log(L"! Error: %s: wrong number of arguments", command);
         return false;
      }
      return stopMove(warg[0]);
   } else if (wcscmp(command, MMDAGENT_COMMAND_ROTATE_START) == 0) {
      /* start rotation */
      tmpBool1 = false;
      tmpFloat = -1.0;
      if (num < 2 || num > 4) {
         g_logger.log(L"! Error: %s: wrong number of arguments", command);
         return false;
      }
      if (arg2rot(&tmpRot, warg[1]) == false) {
         g_logger.log(L"! Error: %s: not a rotation string: %s", command, warg[1]);
         return false;
      }
      if (num >= 3) {
         if (wcscmp(warg[2], L"LOCAL") == 0) {
            tmpBool1 = true;
         } else if (wcscmp(warg[2], L"GLOBAL") == 0) {
            tmpBool1 = false;
         } else {
            g_logger.log(L"! Error: %s: 3rd argument should be \"GLOBAL\" or \"LOCAL\"", command);
            return false;
         }
      }
      if (num >= 4)
         tmpFloat = (float) _wtof(warg[3]);
      return startRotation(warg[0], &tmpRot, tmpBool1, tmpFloat);
   } else if (wcscmp(command, MMDAGENT_COMMAND_ROTATE_STOP) == 0) {
      /* stop rotation */
      if (num != 1) {
         g_logger.log(L"! Error: %s: wrong number of arguments", command);
         return false;
      }
      return stopRotation(warg[0]);
   } else if (wcscmp(command, MMDAGENT_COMMAND_TURN_START) == 0) {
      /* turn start */
      tmpBool1 = false;
      tmpFloat = -1.0;
      if (num < 2 || num > 4) {
         g_logger.log(L"! Error: %s: wrong number of arguments", command);
         return false;
      }
      if (arg2pos(&tmpPos, warg[1]) == false) {
         g_logger.log(L"! Error: %s: not a position string: %s", command, warg[1]);
         return false;
      }
      if (num >= 3) {
         if (wcscmp(warg[2], L"LOCAL") == 0) {
            tmpBool1 = true;
         } else if (wcscmp(warg[2], L"GLOBAL") == 0) {
            tmpBool1 = false;
         } else {
            g_logger.log(L"! Error: %s: 3rd argument should be \"GLOBAL\" or \"LOCAL\"", command);
            return false;
         }
      }
      if (num >= 4)
         tmpFloat = (float) _wtof(warg[3]);
      return startTurn(warg[0], &tmpPos, tmpBool1, tmpFloat);
   } else if (wcscmp(command, MMDAGENT_COMMAND_TURN_STOP) == 0) {
      /* stop turn */
      if (num != 1) {
         g_logger.log(L"! Error: %s: wrong number of arguments", command);
         return false;
      }
      return stopTurn(warg[0]);
   } else if (wcscmp(command, MMDAGENT_COMMAND_SOUND_START) == 0) {
      /* start sound */
      if (num != 2) {
         g_logger.log(L"! Error: %s: wrong number of arguments", command);
         return false;
      }
      startSound(warg[0], warg[1], true);
   } else if (wcscmp(command, MMDAGENT_COMMAND_SOUND_STOP) == 0) {
      /* stop sound */
      if (num < 1) {
         g_logger.log(L"! Error: %s: wrong number of arguments", command);
         return false;
      }
      stopSound(warg[0]);
   } else if (wcscmp(command, MMDAGENT_COMMAND_STAGE) == 0) {
      /* change stage */
      if (num != 1) {
         g_logger.log(L"! Error: %s: wrong number of arguments", command);
         return false;
      }
      /* pmd or bitmap */
      tmpStr1 = wcsstr(warg[0], L",");
      if (tmpStr1 == NULL) {
         return setStage(warg[0]);
      } else {
         (*tmpStr1) = L'\0';
         tmpStr1++;
         if (setFloor(warg[0]) == true && setBackground(tmpStr1) == true)
            return true;
         else
            return false;
      }
   } else if (wcscmp(command, MMDAGENT_COMMAND_LIGHTCOLOR) == 0) {
      /* change light color */
      if (num != 1) {
         g_logger.log(L"! Error: %s: wrong number of arguments", command);
         return false;
      }
      if (arg2floatArray(tmpFloatList, 3, warg[0]) == false) {
         g_logger.log(L"! Error: %s: not \"R,G,B\" value: %s", command, warg[0]);
         return false;
      }
      return changeLightColor(tmpFloatList[0], tmpFloatList[1], tmpFloatList[2]);
   } else if (wcscmp(command, MMDAGENT_COMMAND_LIGHTDIRECTION) == 0) {
      /* change light direction */
      if (num != 1) {
         g_logger.log(L"! Error: %s: wrong number of arguments", command);
         return false;
      }
      if (arg2floatArray(tmpFloatList, 3, warg[0]) == false) {
         g_logger.log(L"! Error: %s: not \"x,y,z\" value: %s", command, warg[0]);
         return false;
      }
      return changeLightDirection(tmpFloatList[0], tmpFloatList[1], tmpFloatList[2]);
   } else if (wcscmp(command, MMDAGENT_COMMAND_LIPSYNC_START) == 0) {
      /* start lip sync */
      if (num != 2) {
         g_logger.log(L"! Error: %s: wrong number of arguments", command);
         return false;
      }
      return startLipSync(warg[0], warg[1]);
   } else if (wcscmp(command, MMDAGENT_COMMAND_LIPSYNC_STOP) == 0) {
      /* stop lip sync */
      if (num != 1) {
         g_logger.log(L"! Error: %s: wrong number of arguments", command);
         return false;
      }
      return stopLipSync(warg[0]);
   }
   return true;
}

/* MMDAgent::addMotion: add motion */
bool MMDAgent::addMotion(wchar_t *modelAlias, wchar_t *motionAlias, wchar_t *fileName, bool full, bool once, bool enableSmooth, bool enableRePos)
{
   int i;
   bool find;
   int id;
   VMD *vmd;
   MotionPlayer *motionPlayer;
   wchar_t *name;

   /* motion file */
   vmd = m_motion.loadFromFile(fileName);
   if (vmd == NULL) {
      g_logger.log(L"! Error: addMotion: failed to load %s.", fileName);
      return false;
   }

   /* ID */
   id = findModelAlias(modelAlias);
   if (id < 0) {
      g_logger.log(L"! Error: addMotion: not found %s.", modelAlias);
      return false;
   }

   /* alias */
   if (motionAlias && wcslen(motionAlias) > 0) {
      /* check the same alias */
      name = _wcsdup(motionAlias);
      for (motionPlayer = m_model[id].getMotionManager()->getMotionPlayerList(); motionPlayer; motionPlayer = motionPlayer->next) {
         if (motionPlayer->active && wcscmp(motionPlayer->name, name) == 0) {
            g_logger.log(L"! Error: addMotion: motion alias \"%s\" is already used.", name);
            free(name);
            return false;
         }
      }
   } else {
      /* if motion alias is not specified, unused digit is used */
      for(i = 0;; i++) {
         find = false;
         name = (wchar_t *) malloc(sizeof(wchar_t) * (getNumDigit(i) + 1));
         wsprintf(name, L"%d", i);
         for (motionPlayer = m_model[id].getMotionManager()->getMotionPlayerList(); motionPlayer; motionPlayer = motionPlayer->next) {
            if (motionPlayer->active && wcscmp(motionPlayer->name, name) == 0) {
               find = true;
               break;
            }
         }
         if(find == false)
            break;
         free(name);
      }
   }

   /* start motion */
   if (m_model[id].startMotion(vmd, name, full, once, enableSmooth, enableRePos) == false) {
      free(name);
      return false;
   }

   sendEventMessage(MMDAGENT_EVENT_MOTION_ADD, L"%s|%s", modelAlias, name);
   free(name);
   return true;
}

/* MMDAgent::changeMotion: change motion */
bool MMDAgent::changeMotion(wchar_t *modelAlias, wchar_t *motionAlias, wchar_t *fileName)
{
   int id;
   VMD *vmd, *old = NULL;
   MotionPlayer *motionPlayer;

   /* ID */
   id = findModelAlias(modelAlias);
   if (id < 0) {
      g_logger.log(L"! Error: changeMotion: not found %s.", modelAlias);
      return false;
   }

   /* check */
   if (!motionAlias) {
      g_logger.log(L"! Error: changeMotion: not specified %s.", motionAlias);
      return false;
   }

   /* motion file */
   vmd = m_motion.loadFromFile(fileName);
   if (vmd == NULL) {
      g_logger.log(L"! Error: changeMotion: failed to load %s.", fileName);
      return false;
   }

   /* get motion before change */
   for (motionPlayer = m_model[id].getMotionManager()->getMotionPlayerList(); motionPlayer; motionPlayer = motionPlayer->next) {
      if (motionPlayer->active && wcscmp(motionPlayer->name, motionAlias) == 0) {
         old = motionPlayer->vmd;
         break;
      }
   }
   if(old == NULL) {
      g_logger.log(L"! Error: changeMotion: motion alias \"%s\"is not found.", motionAlias);
      m_motion.unload(vmd);
      return false;
   }

   /* change motion */
   if (m_model[id].swapMotion(vmd, motionAlias) == false) {
      g_logger.log(L"! Error: changeMotion: motion alias \"%s\"is not found.", motionAlias);
      m_motion.unload(vmd);
      return false;
   }

   /* unload old motion from motion stocker */
   m_motion.unload(old);

   /* send event message */
   sendEventMessage(MMDAGENT_EVENT_MOTION_CHANGE, L"%s|%s", modelAlias, motionAlias);
   return true;
}

/* MMDAgent::deleteMotion: delete motion */
bool MMDAgent::deleteMotion(wchar_t *modelAlias, wchar_t *motionAlias)
{
   int id;

   /* ID */
   id = findModelAlias(modelAlias);
   if (id < 0) {
      g_logger.log(L"! Error: deleteMotion: not found %s.", modelAlias);
      return false;
   }

   /* delete motion */
   if (m_model[id].getMotionManager()->deleteMotion(motionAlias) == false) {
      g_logger.log(L"! Error: deleteMotion: motion alias \"%s\"is not found.", motionAlias);
      return false;
   }

   /* don't send event message */
   return true;
}

/* MMDAgent::addModel: add model */
bool MMDAgent::addModel(wchar_t *modelAlias, wchar_t *fileName, btVector3 *pos, btQuaternion *rot, wchar_t *baseModelAlias, wchar_t *baseBoneName)
{
   int i;
   int id;
   int baseID;
   wchar_t *name;
   btVector3 offsetPos = btVector3(0.0f, 0.0f, 0.0f);
   btQuaternion offsetRot = btQuaternion(0.0f, 0.0f, 0.0f, 1.0f);
   bool forcedPosition = false;
   PMDBone *assignBone = NULL;
   PMDObject *assignObject = NULL;
   float *l = m_option.getLightDirection();
   btVector3 light = btVector3(l[0], l[1], l[2]);

   /* set */
   if (pos)
      offsetPos = (*pos);
   if (rot)
      offsetRot = (*rot);
   if (pos || rot)
      forcedPosition = true;
   if (baseModelAlias) {
      baseID = findModelAlias(baseModelAlias);
      if (baseID < 0) {
         g_logger.log(L"!Error: addModel: model alias \"%s\" is not found", baseModelAlias);
         return false;
      }
      if (baseBoneName) {
         assignBone = m_model[baseID].getPMDModel()->getBone(baseBoneName);
      } else {
         assignBone = m_model[baseID].getPMDModel()->getCenterBone();
      }
      if (assignBone == NULL) {
         if (baseBoneName)
            g_logger.log(L"!Error: addModel: bone \"%s\" is not exist on %s.", baseBoneName, baseModelAlias);
         else
            g_logger.log(L"!Error: addModel: cannot assign to bone of %s.", baseModelAlias);
         return false;
      }
      assignObject = &m_model[baseID];
   }

   /* ID */
   id = getNewModelId();
   if (id == -1) {
      g_logger.log(L"! Error: addModel: too many models.");
      return false;
   }

   /* determine name */
   if (modelAlias && wcslen(modelAlias) > 0) {
      /* check the same alias */
      name = _wcsdup(modelAlias);
      if (findModelAlias(name) >= 0) {
         g_logger.log(L"! Error: addModel: model alias \"%s\" is already used.", name);
         free(name);
         return false;
      }
   } else {
      /* if model alias is not specified, unused digit is used */
      for(i = 0;; i++) {
         name = (wchar_t *) malloc(sizeof(wchar_t) * (getNumDigit(i) + 1));
         wsprintf(name, L"%d", i);
         if (findModelAlias(name) >= 0)
            free(name);
         else
            break;
      }
   }

   /* add model */
   if (!m_model[id].load(fileName, &offsetPos, &offsetRot, forcedPosition, assignBone, assignObject, &m_bullet, m_systex, m_option.getUseCartoonRendering(), m_option.getCartoonEdgeWidth(), &light)) {
      g_logger.log(L"! Error: addModel: failed to load %s.", fileName);
      m_model[id].deleteModel();
      free(name);
      return false;
   }

   /* initialize motion manager */
   m_model[id].resetMotionManager();
   m_model[id].setAlias(name);

   /* send event message */
   sendEventMessage(MMDAGENT_EVENT_MODEL_ADD, L"%s", name);
   free(name);
   return true;
}

/* MMDAgent::changeModel: change model */
bool MMDAgent::changeModel(wchar_t *modelAlias, wchar_t *fileName)
{
   int i;
   int id;
   MotionPlayer *motionPlayer;
   double currentFrame;
   float *l = m_option.getLightDirection();
   btVector3 light = btVector3(l[0], l[1], l[2]);

   /* ID */
   id = findModelAlias(modelAlias);
   if (id < 0) {
      g_logger.log(L"! Error: changeModel: not found %s.", modelAlias);
      return false;
   }

   /* load model */
   if (!m_model[id].load(fileName, NULL, NULL, false, NULL, NULL, &m_bullet, m_systex, m_option.getUseCartoonRendering(), m_option.getCartoonEdgeWidth(), &light)) {
      g_logger.log(L"! Error: changeModel: failed to load model %s.", fileName);
      return false;
   }

   /* update motion manager */
   if (m_model[id].getMotionManager()) {
      for (motionPlayer = m_model[id].getMotionManager()->getMotionPlayerList(); motionPlayer; motionPlayer = motionPlayer->next) {
         if (motionPlayer->active) {
            currentFrame = motionPlayer->mc.getCurrentFrame();
            m_model[id].getMotionManager()->startMotionSub(motionPlayer->vmd, motionPlayer);
            motionPlayer->mc.setCurrentFrame(currentFrame);
         }
      }
   }

   /* set alias */
   m_model[id].setAlias(modelAlias);

   /* delete accessories  */
   for (i = 0; i < m_numModel; i++)
      if (m_model[i].isEnable() && m_model[i].getAssignedModel() == &(m_model[id]))
         deleteModel(m_model[i].getAlias());

   /* send message */
   sendEventMessage(MMDAGENT_EVENT_MODEL_CHANGE, L"%s", modelAlias);
   return true;
}

/* MMDAgent::deleteModel: delete model */
bool MMDAgent::deleteModel(wchar_t *modelAlias)
{
   int i;
   int id;

   /* ID */
   id = findModelAlias(modelAlias);
   if (id < 0) {
      /* wrong alias */
      g_logger.log(L"Error: deleteModel: not found %s.", modelAlias);
      return false;
   }

   /* delete accessories  */
   for (i = 0; i < m_numModel; i++)
      if (m_model[i].isEnable() && m_model[i].getAssignedModel() == &(m_model[id]))
         deleteModel(m_model[i].getAlias());

   /* set frame from now to disappear */
   m_model[id].startDisappear();

   /* send event message */
   sendEventMessage(MMDAGENT_EVENT_MODEL_DELETE, L"%s", modelAlias);
   return true;
}

/* MMDAgent::setFloor: set floor image */
bool MMDAgent::setFloor(wchar_t *fileName)
{
   /* load floor */
   if (m_stage->loadFloor(fileName, &m_bullet) == false) {
      g_logger.log(L"Error: setFloor: cannot set floor %s.", fileName);
      return false;
   }

   /* send event message */
   sendEventMessage(MMDAGENT_EVENT_FLOOR, L"%s", fileName);
   return true;
}

/* MMDAgent::setBackground: set background image */
bool MMDAgent::setBackground(wchar_t *fileName)
{
   /* load background */
   if (m_stage->loadBackground(fileName, &m_bullet) == false) {
      g_logger.log(L"Error: setBackground: cannot set background %s.", fileName);
      return false;
   }

   /* send event message */
   sendEventMessage(MMDAGENT_EVENT_BACKGROUND, L"%s", fileName);
   return true;
}

/* MMDAgent::setStage: set stage */
bool MMDAgent::setStage(wchar_t *fileName)
{
   if (m_stage->loadStagePMD(fileName, &m_bullet, m_systex) == false) {
      g_logger.log(L"Error: setStage: cannot set stage %s.", fileName);
      return false;
   }

   sendEventMessage(MMDAGENT_EVENT_STAGE, L"%s", fileName);
   return true;
}

/* MMDAgent::startSound: start sound */
bool MMDAgent::startSound(wchar_t *soundAlias, wchar_t *fileName, bool adjust)
{
   m_audio.close(soundAlias);
   if (m_audio.play(fileName, soundAlias) == false) {
      m_timer.adjustStop();
      g_logger.log(L"Error: startSound: cannot start sound %s.", fileName);
      return false;
   }

   /* start timer to adjust audio */
   if (adjust) {
      m_timer.adjustSetTarget((double) m_option.getMotionAdjustFrame() * 0.03);
      m_timer.adjustStart();
   }

   /* send event message */
   sendEventMessage(MMDAGENT_EVENT_SOUND_START, L"%s", soundAlias);
   return true;
}

/* MMDAgent::stopSound: stop sound */
bool MMDAgent::stopSound(wchar_t *soundAlias)
{
   m_audio.close(soundAlias);

   /* stop timer */
   m_timer.adjustStop();

   /* don't send event message */
   return true;
}

/* MMDAgent::changeLightDirection: change light direction */
bool MMDAgent::changeLightDirection(float x, float y, float z)
{
   float f[4];

   f[0] = x;
   f[1] = y;
   f[2] = z;
   f[3] = 0.0f;
   m_option.setLightDirection(f);
   updateLight();

   /* send event message */
   sendEventMessage( MMDAGENT_EVENT_LIGHTDIRECTION, L"%.2f,%.2f,%.2f", x, y, z);
   return true;
}

/* MMDAgent::changeLightColor: change light color */
bool MMDAgent::changeLightColor(float r, float g, float b)
{
   float f[3];

   f[0] = r;
   f[1] = g;
   f[2] = b;
   m_option.setLightColor(f);
   updateLight();

   /* send event message */
   sendEventMessage(MMDAGENT_EVENT_LIGHTCOLOR, L"%.2f,%.2f,%.2f", r, g, b);
   return true;
}

/* MMDAgent::startMove: start moving */
bool MMDAgent::startMove(wchar_t *modelAlias, btVector3 *pos, bool local, float speed)
{
   int id;
   btVector3 currentPos;
   btQuaternion currentRot;
   btVector3 targetPos;
   btTransform tr;

   /* ID */
   id = findModelAlias(modelAlias);
   if (id < 0) {
      g_logger.log(L"! Error: startMove: not found %s.", modelAlias);
      return false;
   }

   /* set */
   m_model[id].getPosition(currentPos);
   targetPos = (*pos);

   /* local or global */
   if (local) {
      m_model[id].getRotation(currentRot);
      tr = btTransform(currentRot, currentPos);
      targetPos = tr * targetPos;
   }

   /* not need to start */
   if (currentPos == targetPos)
      return true;

   m_model[id].setMoveSpeed(speed);
   m_model[id].setPosition(targetPos);
   sendEventMessage(MMDAGENT_EVENT_MOVE_START, L"%s", modelAlias);

   return true;
}

/* MMDAgent::stopMove: stop moving */
bool MMDAgent::stopMove(wchar_t *modelAlias)
{
   int id;
   btVector3 targetPos;
   btVector3 currentPos;

   /* ID */
   id = findModelAlias(modelAlias);
   if (id < 0) {
      g_logger.log(L"! Error: stopMove: not found %s.", modelAlias);
      return false;
   }

   /* set */
   targetPos = (*(m_model[id].getPMDModel()->getRootBone()->getOffset()));
   m_model[id].getPosition(currentPos);

   /* not need to stop */
   if (currentPos == targetPos)
      return true;

   m_model[id].setPosition(targetPos);
   sendEventMessage(MMDAGENT_EVENT_MOVE_STOP, L"%s", modelAlias);

   return true;
}

/* MMDAgent::startRotation: start rotation */
bool MMDAgent::startRotation(wchar_t *modelAlias, btQuaternion *rot, bool local, float speed)
{
   int id;
   btQuaternion targetRot;
   btQuaternion currentRot;

   id = findModelAlias(modelAlias);
   if (id < 0) {
      g_logger.log(L"! Error: startRotation: not found %s", modelAlias);
      return false;
   }

   /* set */
   m_model[id].getRotation(currentRot);
   targetRot = (*rot);

   /* local or global */
   if (local)
      targetRot += currentRot;

   /* not need to start */
   if (currentRot == targetRot)
      return true;

   m_model[id].setSpinSpeed(speed);
   m_model[id].setRotation(targetRot);
   sendEventMessage(MMDAGENT_EVENT_ROTATE_START, L"%s", modelAlias);

   return true;
}

/* MMDAgent::stopRotation: stop rotation */
bool MMDAgent::stopRotation(wchar_t *modelAlias)
{
   int id;
   btQuaternion currentRot;
   btQuaternion targetRot;

   id = findModelAlias(modelAlias);
   if (id < 0) {
      g_logger.log(L"! Error: stopRotation: not found %s", modelAlias);
      return false;
   }

   /* set */
   targetRot = (*(m_model[id].getPMDModel()->getRootBone()->getCurrentRotation()));
   m_model[id].getRotation(currentRot);

   /* not need to rotate */
   if (currentRot == targetRot)
      return true;

   m_model[id].setRotation(targetRot);
   sendEventMessage(MMDAGENT_EVENT_ROTATE_STOP, L"%s", modelAlias);

   return true;
}

/* MMDAgent::startTurn: start turn */
bool MMDAgent::startTurn(wchar_t *modelAlias, btVector3 *pos, bool local, float speed)
{
   int id;
   btVector3 currentPos;
   btQuaternion currentRot;
   btVector3 targetPos;
   btQuaternion targetRot;
   btTransform tr;

   btVector3 diffPos;
   float z, rad;

   /* ID */
   id = findModelAlias(modelAlias);
   if (id < 0) {
      g_logger.log(L"! Error: startTurn: not found %s.", modelAlias);
      return false;
   }

   /* set */
   targetPos = (*pos);
   m_model[id].getRotation(currentRot);

   /* local or global */
   if (local) {
      m_model[id].getPosition(currentPos);
      tr = btTransform(currentRot, currentPos);
      targetPos = tr * targetPos;
   }

   /* get vector from current position to target position */
   diffPos = (*(m_model[id].getPMDModel()->getRootBone()->getOffset()));
   diffPos = targetPos - diffPos;
   diffPos.normalize();

   z = diffPos.z();
   if (z > 1.0f) z = 1.0f;
   if (z < -1.0f) z = -1.0f;
   rad = acosf(z);
   if (diffPos.x() < 0.0f) rad = -rad;
   targetRot.setEulerZYX(0, rad, 0);

   /* not need to turn */
   if (currentRot == targetRot)
      return true;

   m_model[id].setSpinSpeed(speed);
   m_model[id].setRotation(targetRot);
   m_model[id].setTurningFlag(true);
   sendEventMessage(MMDAGENT_EVENT_TURN_START, L"%s", modelAlias);

   return true;
}

/* MMDAgent::stopTurn: stop turn */
bool MMDAgent::stopTurn(wchar_t *modelAlias)
{
   int id;
   btQuaternion currentRot;
   btQuaternion targetRot;

   id = findModelAlias(modelAlias);
   if (id < 0) {
      g_logger.log(L"! Error: stopTurn: not found %s.", modelAlias);
      return false;
   }

   /* not need to stop turn */
   if (!m_model[id].isTurning())
      return true;

   /* set */
   targetRot = (*(m_model[id].getPMDModel()->getRootBone()->getCurrentRotation()));
   m_model[id].getRotation(currentRot);

   /* not need to turn */
   if (currentRot == targetRot)
      return true;

   m_model[id].setRotation(targetRot);
   sendEventMessage(MMDAGENT_EVENT_TURN_STOP, L"%s", modelAlias);

   return true;
}

/* MMDAgent::startLipSync: start lip sync */
bool MMDAgent::startLipSync(wchar_t *modelAlias, wchar_t *seq)
{
   int id;
   size_t len;
   char buf[8192];
   unsigned char *vmdData;
   unsigned long vmdSize;
   VMD *vmd;
   bool find = false;
   MotionPlayer *motionPlayer;

   /* ID */
   id = findModelAlias(modelAlias);
   if (id < 0) {
      g_logger.log(L"! Error: startLipSync not found %s.", modelAlias);
      return false;
   }

   /* create motion */
   wcstombs_s(&len, buf, MMDAGENT_MAXLIPSYNCBUFLEN, seq, _TRUNCATE);
   if(m_model[id].getLipSync()->createMotion(buf, &vmdData, &vmdSize) == false) {
      g_logger.log(L"! Error: startLipSync: cannot create lip motion.");
      return false;
   }
   vmd = m_motion.loadFromData(vmdData, vmdSize);
   free(vmdData);

   /* search running lip motion */
   for (motionPlayer = m_model[id].getMotionManager()->getMotionPlayerList(); motionPlayer; motionPlayer = motionPlayer->next) {
      if (motionPlayer->active && wcscmp(motionPlayer->name, LIPSYNC_MOTION_NAME) == 0) {
         find = true;
         break;
      }
   }

   /* start lip sync */
   if(find == true) {
      if (m_model[id].swapMotion(vmd, LIPSYNC_MOTION_NAME) == false) {
         g_logger.log(L"! Error: startLipSync: cannot start lip sync.");
         m_motion.unload(vmd);
         return false;
      }
      sendEventMessage(MMDAGENT_EVENT_LIPSYNC_STOP, L"%s", modelAlias);
   } else {
      if (m_model[id].startMotion(vmd, LIPSYNC_MOTION_NAME, false, true, true, true) == false) {
         g_logger.log(L"! Error: startLipSync: cannot start lip sync.");
         m_motion.unload(vmd);
         return false;
      }
   }

   /* send event message */
   sendEventMessage(MMDAGENT_EVENT_LIPSYNC_START, L"%s", modelAlias);
   return true;
}

/* MMDAgent::stopLipSync: stop lip sync */
bool MMDAgent::stopLipSync(wchar_t *modelAlias)
{
   int id;

   /* ID */
   id = findModelAlias(modelAlias);
   if (id < 0) {
      g_logger.log(L"! Error: stopLipSync: not found %s.", modelAlias);
      return false;
   }

   /* stop lip sync */
   if (m_model[id].getMotionManager()->deleteMotion(LIPSYNC_MOTION_NAME) == false) {
      g_logger.log(L"! Error: stopLipSync: lipsync motion not found");
      return false;
   }

   /* don't send message */
   return true;
}
