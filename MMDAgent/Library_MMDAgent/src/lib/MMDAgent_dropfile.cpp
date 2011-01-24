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
#include <shellapi.h>

#include "MMDAgent.h"
#include "utils.h"

/* MMDAgent::procDropFileMessage: process file drops message */
void MMDAgent::procDropFileMessage(char *file, int x, int y)
{
   int i;

   int dropAllowedModelID;
   int targetModelID;

   /* for motion */
   MotionPlayer *motionPlayer;

   /* for mp3 */
   int len;
   char *buf;
   FILE *fp;

   if (hasExtension(file, ".vmd")) {
      dropAllowedModelID = -1;
      targetModelID = -1;
      if (m_keyCtrl) {
         /* if Ctrl-key, start motion on all models */
         targetModelID = MODEL_ALL;
      } else if (m_doubleClicked && m_selectedModel != -1 && m_model[m_selectedModel].allowMotionFileDrop()) {
         targetModelID = m_selectedModel;
      } else {
         targetModelID = m_render->pickModel(this, x, y, &dropAllowedModelID); /* model ID in curpor position */
         if (targetModelID == -1)
            targetModelID = dropAllowedModelID;
      }
      if (targetModelID == -1) {
         m_logger.log("Warning: vmd file dropped but no model exit at the point");
      } else {
         /* pause timer to skip file loading time */
         m_timer.pause();
         if (m_keyShift) { /* if Shift-key, insert motion */
            if (targetModelID == MODEL_ALL) {
               /* all model */
               for (i = 0; i < m_numModel; i++) {
                  if (m_model[i].isEnable() && m_model[i].allowMotionFileDrop())
                     addMotion(m_model[i].getAlias(), NULL, file, false, true, true, true);
               }
            } else {
               /* target model */
               addMotion(m_model[targetModelID].getAlias(), NULL, file, false, true, true, true);
            }
         } else {
            /* change base motion */
            if (targetModelID == MODEL_ALL) {
               /* all model */
               for (i = 0; i < m_numModel; i++) {
                  if (m_model[i].isEnable() && m_model[i].allowMotionFileDrop()) {
                     for (motionPlayer = m_model[i].getMotionManager()->getMotionPlayerList(); motionPlayer; motionPlayer = motionPlayer->next) {
                        if (motionPlayer->active && strcmp(motionPlayer->name, "base") == 0) {
                           changeMotion(m_model[i].getAlias(), "base", file); /* if 'base' motion is already used, change motion */
                           break;
                        }
                     }
                     if (!motionPlayer)
                        addMotion(m_model[i].getAlias(), "base", file, true, false, true, true);
                  }
               }
            } else {
               /* target model */
               for (motionPlayer = m_model[targetModelID].getMotionManager()->getMotionPlayerList(); motionPlayer; motionPlayer = motionPlayer->next) {
                  if (motionPlayer->active && strcmp(motionPlayer->name, "base") == 0) {
                     changeMotion(m_model[targetModelID].getAlias(), "base", file); /* if 'base' motion is already used, change motion */
                     break;
                  }
               }
               if (!motionPlayer)
                  addMotion(m_model[targetModelID].getAlias(), "base", file, true, false, true, true);
            }
         }
         /* check mp3 */
         len = strlen(file);
         if (len >= 5) {
            buf = strdup(file);
            buf[len-3] = 'm';
            buf[len-2] = 'p';
            buf[len-1] = '3';
            fp = fopen(buf, "rb");
            if (fp) {
               /* start audio */
               fclose(fp);
               sendCommandMessage(MMDAGENT_COMMAND_SOUND_START, "%s|%s", "audio", buf);
            }
            free(buf);
         }
         /* resume timer */
         m_timer.resume();
      }
   } else if (hasExtension(file, ".xpmd")) {
      /* load stage */
      setStage(file);
   } else if (hasExtension(file, ".pmd")) {
      /* drop model */
      if (m_keyCtrl) {
         /* if Ctrl-key, add model */
         addModel(NULL, file, NULL, NULL, NULL, NULL);
      } else {
         /* change model */
         if (m_doubleClicked && m_selectedModel != -1) /* already selected */
            targetModelID = m_selectedModel;
         else
            targetModelID = m_render->pickModel(this, x, y, &dropAllowedModelID);
         if (targetModelID == -1) {
            m_logger.log("Warning: pmd file dropped but no model at the point");
         } else {
            changeModel(m_model[targetModelID].getAlias(), file);
         }
      }
   } else if (hasExtension(file, ".bmp") || hasExtension(file, ".tga") || hasExtension(file, ".png")) {
      if (m_keyCtrl)
         setFloor(file); /* change floor with Ctrl-key */
      else
         setBackground(file); /* change background without Ctrl-key */
   }
}
