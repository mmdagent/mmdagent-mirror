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

#include "MMDAgent.h"
#include "utils.h"

#define MMDAGENT_MAXNUMCOMMAND    10
#define MMDAGENT_MAXCOMMANDBUFLEN 1024
#define MMDAGENT_MAXLIPSYNCBUFLEN MMDAGENT_MAXCOMMANDBUFLEN

/* MMDAgent::command: decode command string from client and call process */
bool MMDAgent::command(char *command, char *str)
{
   static char argv[MMDAGENT_MAXNUMCOMMAND][MMDAGENT_MAXCOMMANDBUFLEN]; /* static buffer */
   int num = 0;
   char *buf;

   char *str1, *str2;
   bool bool1, bool2, bool3, bool4;
   float f;
   btVector3 pos;
   btQuaternion rot;
   float fvec[3];

   /* divide string into arguments */
   if (MMDAgent_strlen(str) <= 0) {
      m_logger->log("<%s>", command);
      num = 0;
   } else {
      m_logger->log("<%s|%s>", command, str);
      buf = MMDAgent_strdup(str);
      for (str1 = MMDAgent_strtok(buf, "|", &str2); str1; str1 = MMDAgent_strtok(NULL, "|", &str2)) {
         if (num >= MMDAGENT_MAXNUMCOMMAND) {
            m_logger->log("! Error: too many argument in command %s: %s", command, str);
            free(buf);
            return false;
         }
         strncpy(argv[num], str1, MMDAGENT_MAXCOMMANDBUFLEN);
         argv[num][MMDAGENT_MAXCOMMANDBUFLEN - 1] = '\0';
         num++;
      }
      free(buf);
   }

   if (MMDAgent_strequal(command, MMDAGENT_COMMAND_MODEL_ADD)) {
      str1 = NULL;
      str2 = NULL;
      if (num < 2 || num > 6) {
         m_logger->log("! Error: %s: wrong number of arguments", command);
         return false;
      }
      if (num >= 3) {
         if (MMDAgent_str2pos(argv[2], &pos) == false) {
            m_logger->log("! Error: %s: not a position string: %s", command, argv[2]);
            return false;
         }
      } else {
         pos = btVector3(0.0, 0.0, 0.0);
      }
      if (num >= 4) {
         if (MMDAgent_str2rot(argv[3], &rot) == false) {
            m_logger->log("! Error: %s: not a rotation string: %s", command, argv[3]);
            return false;
         }
      } else {
         rot.setEulerZYX(0.0, 0.0, 0.0);
      }
      if (num >= 5) {
         str1 = argv[4];
      }
      if (num >= 6) {
         str2 = argv[5];
      }
      return addModel(argv[0], argv[1], &pos, &rot, str1, str2);
   } else if (MMDAgent_strequal(command, MMDAGENT_COMMAND_MODEL_CHANGE)) {
      /* change model */
      if (num != 2) {
         m_logger->log("! Error: %s: wrong number of arguments", command);
         return false;
      }
      return changeModel(argv[0], argv[1]);
   } else if (MMDAgent_strequal(command, MMDAGENT_COMMAND_MODEL_DELETE)) {
      /* delete model */
      if (num != 1) {
         m_logger->log("! Error: %s: wrong number of arguments", command);
         return false;
      }
      return deleteModel(argv[0]);
   } else if (MMDAgent_strequal(command, MMDAGENT_COMMAND_MOTION_ADD)) {
      /* add motion */
      bool1 = true; /* full */
      bool2 = true; /* once */
      bool3 = true; /* enableSmooth */
      bool4 = true; /* enableRePos */
      if (num < 3 || num > 7) {
         m_logger->log("! Error: %s: too few arguments", command);
         return false;
      }
      if (num >= 4) {
         if (MMDAgent_strequal(argv[3], "FULL")) {
            bool1 = true;
         } else if (MMDAgent_strequal(argv[3], "PART")) {
            bool1 = false;
         } else {
            m_logger->log("! Error: %s: 4th argument should be \"FULL\" or \"PART\"", command);
            return false;
         }
      }
      if (num >= 5) {
         if (MMDAgent_strequal(argv[4], "ONCE")) {
            bool2 = true;
         } else if (MMDAgent_strequal(argv[4], "LOOP")) {
            bool2 = false;
         } else {
            m_logger->log("! Error: %s: 5th argument should be \"ONCE\" or \"LOOP\"", command);
            return false;
         }
      }
      if (num >= 6) {
         if (MMDAgent_strequal(argv[5], "ON")) {
            bool3 = true;
         } else if (MMDAgent_strequal(argv[5], "OFF")) {
            bool3 = false;
         } else {
            m_logger->log("! Error: %s: 6th argument should be \"ON\" or \"OFF\"", command);
            return false;
         }
      }
      if (num >= 7) {
         if (MMDAgent_strequal(argv[6], "ON")) {
            bool4 = true;
         } else if (MMDAgent_strequal(argv[6], "OFF")) {
            bool4 = false;
         } else {
            m_logger->log("! Error: %s: 7th argument should be \"ON\" or \"OFF\"", command);
            return false;
         }
      }
      return addMotion(argv[0], argv[1], argv[2], bool1, bool2, bool3, bool4);
   } else if (MMDAgent_strequal(command, MMDAGENT_COMMAND_MOTION_CHANGE)) {
      /* change motion */
      if (num != 3) {
         m_logger->log("! Error: %s: wrong number of arguments", command);
         return false;
      }
      return changeMotion(argv[0], argv[1], argv[2]);
   } else if (MMDAgent_strequal(command, MMDAGENT_COMMAND_MOTION_DELETE)) {
      /* delete motion */
      if (num != 2) {
         m_logger->log("! Error: %s: wrong number of arguments", command);
         return false;
      }
      return deleteMotion(argv[0], argv[1]);
   } else if (MMDAgent_strequal(command, MMDAGENT_COMMAND_MOVE_START)) {
      /* start moving */
      bool1 = false;
      f = -1.0;
      if (num < 2 || num > 4) {
         m_logger->log("! Error: %s: wrong number of arguments", command);
         return false;
      }
      if (MMDAgent_str2pos(argv[1], &pos) == false) {
         m_logger->log("! Error: %s: not a position string: %s", command, argv[1]);
         return false;
      }
      if (num >= 3) {
         if (MMDAgent_strequal(argv[2], "LOCAL")) {
            bool1 = true;
         } else if (MMDAgent_strequal(argv[2], "GLOBAL")) {
            bool1 = false;
         } else {
            m_logger->log("! Error: %s: 3rd argument should be \"GLOBAL\" or \"LOCAL\"", command);
            return false;
         }
      }
      if (num >= 4)
         f = MMDAgent_str2float(argv[3]);
      return startMove(argv[0], &pos, bool1, f);
   } else if (MMDAgent_strequal(command, MMDAGENT_COMMAND_MOVE_STOP)) {
      /* stop moving */
      if (num != 1) {
         m_logger->log("! Error: %s: wrong number of arguments", command);
         return false;
      }
      return stopMove(argv[0]);
   } else if (MMDAgent_strequal(command, MMDAGENT_COMMAND_ROTATE_START)) {
      /* start rotation */
      bool1 = false;
      f = -1.0;
      if (num < 2 || num > 4) {
         m_logger->log("! Error: %s: wrong number of arguments", command);
         return false;
      }
      if (MMDAgent_str2rot(argv[1], &rot) == false) {
         m_logger->log("! Error: %s: not a rotation string: %s", command, argv[1]);
         return false;
      }
      if (num >= 3) {
         if (MMDAgent_strequal(argv[2], "LOCAL")) {
            bool1 = true;
         } else if (MMDAgent_strequal(argv[2], "GLOBAL")) {
            bool1 = false;
         } else {
            m_logger->log("! Error: %s: 3rd argument should be \"GLOBAL\" or \"LOCAL\"", command);
            return false;
         }
      }
      if (num >= 4)
         f = MMDAgent_str2float(argv[3]);
      return startRotation(argv[0], &rot, bool1, f);
   } else if (MMDAgent_strequal(command, MMDAGENT_COMMAND_ROTATE_STOP)) {
      /* stop rotation */
      if (num != 1) {
         m_logger->log("! Error: %s: wrong number of arguments", command);
         return false;
      }
      return stopRotation(argv[0]);
   } else if (MMDAgent_strequal(command, MMDAGENT_COMMAND_TURN_START)) {
      /* turn start */
      bool1 = false;
      f = -1.0;
      if (num < 2 || num > 4) {
         m_logger->log("! Error: %s: wrong number of arguments", command);
         return false;
      }
      if (MMDAgent_str2pos(argv[1], &pos) == false) {
         m_logger->log("! Error: %s: not a position string: %s", command, argv[1]);
         return false;
      }
      if (num >= 3) {
         if (MMDAgent_strequal(argv[2], "LOCAL")) {
            bool1 = true;
         } else if (MMDAgent_strequal(argv[2], "GLOBAL")) {
            bool1 = false;
         } else {
            m_logger->log("! Error: %s: 3rd argument should be \"GLOBAL\" or \"LOCAL\"", command);
            return false;
         }
      }
      if (num >= 4)
         f = MMDAgent_str2float(argv[3]);
      return startTurn(argv[0], &pos, bool1, f);
   } else if (MMDAgent_strequal(command, MMDAGENT_COMMAND_TURN_STOP)) {
      /* stop turn */
      if (num != 1) {
         m_logger->log("! Error: %s: wrong number of arguments", command);
         return false;
      }
      return stopTurn(argv[0]);
   } else if (MMDAgent_strequal(command, MMDAGENT_COMMAND_STAGE)) {
      /* change stage */
      if (num != 1) {
         m_logger->log("! Error: %s: wrong number of arguments", command);
         return false;
      }
      /* pmd or bitmap */
      str1 = strstr(argv[0], ",");
      if (str1 == NULL) {
         return setStage(argv[0]);
      } else {
         (*str1) = '\0';
         str1++;
         if (setFloor(argv[0]) == true && setBackground(str1) == true)
            return true;
         else
            return false;
      }
   } else if (MMDAgent_strequal(command, MMDAGENT_COMMAND_LIGHTCOLOR)) {
      /* change light color */
      if (num != 1) {
         m_logger->log("! Error: %s: wrong number of arguments", command);
         return false;
      }
      if (MMDAgent_str2fvec(argv[0], fvec, 3) == false) {
         m_logger->log("! Error: %s: not \"R,G,B\" value: %s", command, argv[0]);
         return false;
      }
      return changeLightColor(fvec[0], fvec[1], fvec[2]);
   } else if (MMDAgent_strequal(command, MMDAGENT_COMMAND_LIGHTDIRECTION)) {
      /* change light direction */
      if (num != 1) {
         m_logger->log("! Error: %s: wrong number of arguments", command);
         return false;
      }
      if (MMDAgent_str2fvec(argv[0], fvec, 3) == false) {
         m_logger->log("! Error: %s: not \"x,y,z\" value: %s", command, argv[0]);
         return false;
      }
      return changeLightDirection(fvec[0], fvec[1], fvec[2]);
   } else if (MMDAgent_strequal(command, MMDAGENT_COMMAND_LIPSYNC_START)) {
      /* start lip sync */
      if (num != 2) {
         m_logger->log("! Error: %s: wrong number of arguments", command);
         return false;
      }
      return startLipSync(argv[0], argv[1]);
   } else if (MMDAgent_strequal(command, MMDAGENT_COMMAND_LIPSYNC_STOP)) {
      /* stop lip sync */
      if (num != 1) {
         m_logger->log("! Error: %s: wrong number of arguments", command);
         return false;
      }
      return stopLipSync(argv[0]);
   }
   return true;
}

/* MMDAgent::addMotion: add motion */
bool MMDAgent::addMotion(char *modelAlias, char *motionAlias, char *fileName, bool full, bool once, bool enableSmooth, bool enableRePos)
{
   int i;
   bool find;
   int id;
   VMD *vmd;
   MotionPlayer *motionPlayer;
   char *name;

   /* motion file */
   vmd = m_motion->loadFromFile(fileName);
   if (vmd == NULL) {
      m_logger->log("! Error: addMotion: failed to load %s.", fileName);
      return false;
   }

   /* ID */
   id = findModelAlias(modelAlias);
   if (id < 0) {
      m_logger->log("! Error: addMotion: not found %s.", modelAlias);
      return false;
   }

   /* alias */
   if (MMDAgent_strlen(motionAlias) > 0) {
      /* check the same alias */
      name = MMDAgent_strdup(motionAlias);
      for (motionPlayer = m_model[id].getMotionManager()->getMotionPlayerList(); motionPlayer; motionPlayer = motionPlayer->next) {
         if (motionPlayer->active && MMDAgent_strequal(motionPlayer->name, name)) {
            m_logger->log("! Error: addMotion: motion alias \"%s\" is already used.", name);
            free(name);
            return false;
         }
      }
   } else {
      /* if motion alias is not specified, unused digit is used */
      for(i = 0;; i++) {
         find = false;
         name = MMDAgent_intdup(i);
         for (motionPlayer = m_model[id].getMotionManager()->getMotionPlayerList(); motionPlayer; motionPlayer = motionPlayer->next) {
            if (motionPlayer->active && MMDAgent_strequal(motionPlayer->name, name)) {
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

   sendEventMessage(MMDAGENTCOMMAND_EVENTMOTIONADD, "%s|%s", modelAlias, name);
   free(name);
   return true;
}

/* MMDAgent::changeMotion: change motion */
bool MMDAgent::changeMotion(char *modelAlias, char *motionAlias, char *fileName)
{
   int id;
   VMD *vmd, *old = NULL;
   MotionPlayer *motionPlayer;

   /* ID */
   id = findModelAlias(modelAlias);
   if (id < 0) {
      m_logger->log("! Error: changeMotion: not found %s.", modelAlias);
      return false;
   }

   /* check */
   if (!motionAlias) {
      m_logger->log("! Error: changeMotion: not specified %s.", motionAlias);
      return false;
   }

   /* motion file */
   vmd = m_motion->loadFromFile(fileName);
   if (vmd == NULL) {
      m_logger->log("! Error: changeMotion: failed to load %s.", fileName);
      return false;
   }

   /* get motion before change */
   for (motionPlayer = m_model[id].getMotionManager()->getMotionPlayerList(); motionPlayer; motionPlayer = motionPlayer->next) {
      if (motionPlayer->active && MMDAgent_strequal(motionPlayer->name, motionAlias)) {
         old = motionPlayer->vmd;
         break;
      }
   }
   if(old == NULL) {
      m_logger->log("! Error: changeMotion: motion alias \"%s\"is not found.", motionAlias);
      m_motion->unload(vmd);
      return false;
   }

   /* change motion */
   if (m_model[id].swapMotion(vmd, motionAlias) == false) {
      m_logger->log("! Error: changeMotion: motion alias \"%s\"is not found.", motionAlias);
      m_motion->unload(vmd);
      return false;
   }

   /* unload old motion from motion stocker */
   m_motion->unload(old);

   /* send event message */
   sendEventMessage(MMDAGENTCOMMAND_MOTIONEVENTCHANGE, "%s|%s", modelAlias, motionAlias);
   return true;
}

/* MMDAgent::deleteMotion: delete motion */
bool MMDAgent::deleteMotion(char *modelAlias, char *motionAlias)
{
   int id;

   /* ID */
   id = findModelAlias(modelAlias);
   if (id < 0) {
      m_logger->log("! Error: deleteMotion: not found %s.", modelAlias);
      return false;
   }

   /* delete motion */
   if (m_model[id].getMotionManager()->deleteMotion(motionAlias) == false) {
      m_logger->log("! Error: deleteMotion: motion alias \"%s\"is not found.", motionAlias);
      return false;
   }

   /* don't send event message */
   return true;
}

/* MMDAgent::addModel: add model */
bool MMDAgent::addModel(char *modelAlias, char *fileName, btVector3 *pos, btQuaternion *rot, char *baseModelAlias, char *baseBoneName)
{
   int i;
   int id;
   int baseID;
   char *name;
   btVector3 offsetPos = btVector3(0.0f, 0.0f, 0.0f);
   btQuaternion offsetRot = btQuaternion(0.0f, 0.0f, 0.0f, 1.0f);
   bool forcedPosition = false;
   PMDBone *assignBone = NULL;
   PMDObject *assignObject = NULL;
   float *l = m_option->getLightDirection();
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
         m_logger->log("!Error: addModel: model alias \"%s\" is not found", baseModelAlias);
         return false;
      }
      if (baseBoneName) {
         assignBone = m_model[baseID].getPMDModel()->getBone(baseBoneName);
      } else {
         assignBone = m_model[baseID].getPMDModel()->getCenterBone();
      }
      if (assignBone == NULL) {
         if (baseBoneName)
            m_logger->log("!Error: addModel: bone \"%s\" is not exist on %s.", baseBoneName, baseModelAlias);
         else
            m_logger->log("!Error: addModel: cannot assign to bone of %s.", baseModelAlias);
         return false;
      }
      assignObject = &m_model[baseID];
   }

   /* ID */
   id = getNewModelId();
   if (id == -1) {
      m_logger->log("! Error: addModel: too many models.");
      return false;
   }

   /* determine name */
   if (MMDAgent_strlen(modelAlias) > 0) {
      /* check the same alias */
      name = MMDAgent_strdup(modelAlias);
      if (findModelAlias(name) >= 0) {
         m_logger->log("! Error: addModel: model alias \"%s\" is already used.", name);
         free(name);
         return false;
      }
   } else {
      /* if model alias is not specified, unused digit is used */
      for(i = 0;; i++) {
         name = MMDAgent_intdup(i);
         if (findModelAlias(name) >= 0)
            free(name);
         else
            break;
      }
   }

   /* add model */
   if (!m_model[id].load(fileName, name, &offsetPos, &offsetRot, forcedPosition, assignBone, assignObject, m_bullet, m_systex, m_lipSync, m_option->getUseCartoonRendering(), m_option->getCartoonEdgeWidth(), &light, m_option->getDisplayCommentFrame())) {
      m_logger->log("! Error: addModel: failed to load %s.", fileName);
      m_model[id].deleteModel();
      free(name);
      return false;
   }

   /* initialize motion manager */
   m_model[id].resetMotionManager();

   /* send event message */
   sendEventMessage(MMDAGENTCOMMAND_MODELEVENTADD, "%s", name);
   free(name);
   return true;
}

/* MMDAgent::changeModel: change model */
bool MMDAgent::changeModel(char *modelAlias, char *fileName)
{
   int i;
   int id;
   MotionPlayer *motionPlayer;
   double currentFrame;
   double previousFrame;
   float *l = m_option->getLightDirection();
   btVector3 light = btVector3(l[0], l[1], l[2]);

   /* ID */
   id = findModelAlias(modelAlias);
   if (id < 0) {
      m_logger->log("! Error: changeModel: not found %s.", modelAlias);
      return false;
   }

   /* load model */
   if (!m_model[id].load(fileName, modelAlias, NULL, NULL, false, NULL, NULL, m_bullet, m_systex, m_lipSync, m_option->getUseCartoonRendering(), m_option->getCartoonEdgeWidth(), &light, m_option->getDisplayCommentFrame())) {
      m_logger->log("! Error: changeModel: failed to load model %s.", fileName);
      return false;
   }

   /* update motion manager */
   if (m_model[id].getMotionManager()) {
      for (motionPlayer = m_model[id].getMotionManager()->getMotionPlayerList(); motionPlayer; motionPlayer = motionPlayer->next) {
         if (motionPlayer->active) {
            currentFrame = motionPlayer->mc.getCurrentFrame();
            previousFrame = motionPlayer->mc.getPreviousFrame();
            m_model[id].getMotionManager()->startMotionSub(motionPlayer->vmd, motionPlayer);
            motionPlayer->mc.setCurrentFrame(currentFrame);
            motionPlayer->mc.setPreviousFrame(previousFrame);
         }
      }
   }

   /* delete accessories immediately*/
   for (i = 0; i < m_numModel; i++)
      if (m_model[i].isEnable() && m_model[i].getAssignedModel() == &(m_model[id]))
         removeRelatedModels(i);

   /* send message */
   sendEventMessage(MMDAGENTCOMMAND_MODELEVENTCHANGE, "%s", modelAlias);
   return true;
}

/* MMDAgent::deleteModel: delete model */
bool MMDAgent::deleteModel(char *modelAlias)
{
   int i;
   int id;

   /* ID */
   id = findModelAlias(modelAlias);
   if (id < 0) {
      /* wrong alias */
      m_logger->log("Error: deleteModel: not found %s.", modelAlias);
      return false;
   }

   /* delete accessories  */
   for (i = 0; i < m_numModel; i++)
      if (m_model[i].isEnable() && m_model[i].getAssignedModel() == &(m_model[id]))
         deleteModel(m_model[i].getAlias());

   /* set frame from now to disappear */
   m_model[id].startDisappear();

   /* send event message */
   return true;
}

/* MMDAgent::setFloor: set floor image */
bool MMDAgent::setFloor(char *fileName)
{
   /* load floor */
   if (m_stage->loadFloor(fileName, m_bullet) == false) {
      m_logger->log("Error: setFloor: cannot set floor %s.", fileName);
      return false;
   }

   /* send event message */
   sendEventMessage(MMDAGENTCOMMAND_FLOOR, "%s", fileName);
   return true;
}

/* MMDAgent::setBackground: set background image */
bool MMDAgent::setBackground(char *fileName)
{
   /* load background */
   if (m_stage->loadBackground(fileName, m_bullet) == false) {
      m_logger->log("Error: setBackground: cannot set background %s.", fileName);
      return false;
   }

   /* send event message */
   sendEventMessage(MMDAGENTCOMMAND_BACKGROUND, "%s", fileName);
   return true;
}

/* MMDAgent::setStage: set stage */
bool MMDAgent::setStage(char *fileName)
{
   if (m_stage->loadStagePMD(fileName, m_bullet, m_systex) == false) {
      m_logger->log("Error: setStage: cannot set stage %s.", fileName);
      return false;
   }

   sendEventMessage(MMDAGENTCOMMAND_STAGE, "%s", fileName);
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
   m_option->setLightDirection(f);
   updateLight();

   /* send event message */
   sendEventMessage(MMDAGENTCOMMAND_EVENTLIGHTDIRECTION, "%.2f,%.2f,%.2f", x, y, z);
   return true;
}

/* MMDAgent::changeLightColor: change light color */
bool MMDAgent::changeLightColor(float r, float g, float b)
{
   float f[3];

   f[0] = r;
   f[1] = g;
   f[2] = b;
   m_option->setLightColor(f);
   updateLight();

   /* send event message */
   sendEventMessage(MMDAGENTCOMMAND_EVENTLIGHTCOLOR, "%.2f,%.2f,%.2f", r, g, b);
   return true;
}

/* MMDAgent::startMove: start moving */
bool MMDAgent::startMove(char *modelAlias, btVector3 *pos, bool local, float speed)
{
   int id;
   btVector3 currentPos;
   btQuaternion currentRot;
   btVector3 targetPos;
   btTransform tr;

   /* ID */
   id = findModelAlias(modelAlias);
   if (id < 0) {
      m_logger->log("! Error: startMove: not found %s.", modelAlias);
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
   sendEventMessage(MMDAGENTCOMMAND_MOVEEVENTSTART, "%s", modelAlias);
   return true;
}

/* MMDAgent::stopMove: stop moving */
bool MMDAgent::stopMove(char *modelAlias)
{
   int id;
   btVector3 targetPos;
   btVector3 currentPos;

   /* ID */
   id = findModelAlias(modelAlias);
   if (id < 0) {
      m_logger->log("! Error: stopMove: not found %s.", modelAlias);
      return false;
   }

   /* set */
   targetPos = (*(m_model[id].getPMDModel()->getRootBone()->getOffset()));
   m_model[id].getPosition(currentPos);

   /* not need to stop */
   if (currentPos == targetPos)
      return true;

   m_model[id].setPosition(targetPos);
   sendEventMessage(MMDAGENTCOMMAND_MOVEEVENTSTOP, "%s", modelAlias);
   return true;
}

/* MMDAgent::startRotation: start rotation */
bool MMDAgent::startRotation(char *modelAlias, btQuaternion *rot, bool local, float speed)
{
   int id;
   btQuaternion targetRot;
   btQuaternion currentRot;

   id = findModelAlias(modelAlias);
   if (id < 0) {
      m_logger->log("! Error: startRotation: not found %s", modelAlias);
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
   sendEventMessage(MMDAGENTCOMMAND_ROTATEEVENTSTART, "%s", modelAlias);
   return true;
}

/* MMDAgent::stopRotation: stop rotation */
bool MMDAgent::stopRotation(char *modelAlias)
{
   int id;
   btQuaternion currentRot;
   btQuaternion targetRot;

   id = findModelAlias(modelAlias);
   if (id < 0) {
      m_logger->log("! Error: stopRotation: not found %s", modelAlias);
      return false;
   }

   /* set */
   targetRot = (*(m_model[id].getPMDModel()->getRootBone()->getCurrentRotation()));
   m_model[id].getRotation(currentRot);

   /* not need to rotate */
   if (currentRot == targetRot)
      return true;

   m_model[id].setRotation(targetRot);
   sendEventMessage(MMDAGENTCOMMAND_ROTATEEVENTSTOP, "%s", modelAlias);
   return true;
}

/* MMDAgent::startTurn: start turn */
bool MMDAgent::startTurn(char *modelAlias, btVector3 *pos, bool local, float speed)
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
      m_logger->log("! Error: startTurn: not found %s.", modelAlias);
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
   sendEventMessage(MMDAGENTCOMMAND_TURNEVENTSTART, "%s", modelAlias);
   return true;
}

/* MMDAgent::stopTurn: stop turn */
bool MMDAgent::stopTurn(char *modelAlias)
{
   int id;
   btQuaternion currentRot;
   btQuaternion targetRot;

   id = findModelAlias(modelAlias);
   if (id < 0) {
      m_logger->log("! Error: stopTurn: not found %s.", modelAlias);
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
   sendEventMessage(MMDAGENTCOMMAND_TURNEVENTSTOP, "%s", modelAlias);

   return true;
}

/* MMDAgent::startLipSync: start lip sync */
bool MMDAgent::startLipSync(char *modelAlias, char *seq)
{
   int id;
   unsigned char *vmdData;
   unsigned long vmdSize;
   VMD *vmd;
   bool find = false;
   MotionPlayer *motionPlayer;

   /* ID */
   id = findModelAlias(modelAlias);
   if (id < 0) {
      m_logger->log("! Error: startLipSync not found %s.", modelAlias);
      return false;
   }

   /* create motion */
   if(m_model[id].createLipSyncMotion(seq, &vmdData, &vmdSize) == false) {
      m_logger->log("! Error: startLipSync: cannot create lip motion.");
      return false;
   }
   vmd = m_motion->loadFromData(vmdData, vmdSize);
   free(vmdData);

   /* search running lip motion */
   for (motionPlayer = m_model[id].getMotionManager()->getMotionPlayerList(); motionPlayer; motionPlayer = motionPlayer->next) {
      if (motionPlayer->active && MMDAgent_strequal(motionPlayer->name, LIPSYNC_MOTIONNAME)) {
         find = true;
         break;
      }
   }

   /* start lip sync */
   if(find == true) {
      if (m_model[id].swapMotion(vmd, LIPSYNC_MOTIONNAME) == false) {
         m_logger->log("! Error: startLipSync: cannot start lip sync.");
         m_motion->unload(vmd);
         return false;
      }
      sendEventMessage(MMDAGENTCOMMAND_LIPSYNCEVENTSTOP, "%s", modelAlias);
   } else {
      if (m_model[id].startMotion(vmd, LIPSYNC_MOTIONNAME, false, true, true, true) == false) {
         m_logger->log("! Error: startLipSync: cannot start lip sync.");
         m_motion->unload(vmd);
         return false;
      }
   }

   /* send event message */
   sendEventMessage(MMDAGENTCOMMAND_LIPSYNCEVENTSTART, "%s", modelAlias);
   return true;
}

/* MMDAgent::stopLipSync: stop lip sync */
bool MMDAgent::stopLipSync(char *modelAlias)
{
   int id;

   /* ID */
   id = findModelAlias(modelAlias);
   if (id < 0) {
      m_logger->log("! Error: stopLipSync: not found %s.", modelAlias);
      return false;
   }

   /* stop lip sync */
   if (m_model[id].getMotionManager()->deleteMotion(LIPSYNC_MOTIONNAME) == false) {
      m_logger->log("! Error: stopLipSync: lipsync motion not found");
      return false;
   }

   /* don't send message */
   return true;
}
