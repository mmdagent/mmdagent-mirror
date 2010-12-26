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
#include "UserOption.h"

/* MMDAgent::dropFile: handle file drops */
void MMDAgent::dropFile(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
   int i, j;

   RECT rc;
   POINT pos;

   int dropAllowedModelID;
   int targetModelID;

   wchar_t droppedFileName[MAX_PATH];
   HDROP hDrop = (HDROP) wParam;
   int fileNum;

   /* for motion */
   MotionPlayer *motionPlayer;

   /* for mp3 */
   int len;
   wchar_t *buf;
   FILE *fp;

   /* get mouse position */
   if (!GetWindowRect(hWnd, &rc)) return;
   if (!GetCursorPos(&pos)) return;
   pos.x -= rc.left;
   pos.y -= rc.top;

   /* get dropped file list */
   fileNum = DragQueryFile(hDrop, -1, droppedFileName, MAX_PATH);

   for (i = 0; i < fileNum; i++) {
      DragQueryFile(hDrop, i, droppedFileName, MAX_PATH);

      if (hasSuffix(droppedFileName, L"vmd")) {
         dropAllowedModelID = -1;
         targetModelID = -1;
         if (m_keyCtrl) {
            /* if Ctrl-key, start motion on all models */
            targetModelID = MODEL_ALL;
         } else if (m_doubleClicked && m_selectedModel != -1) {
            /* if already selected */
            targetModelID = m_selectedModel;
            if (m_model[m_selectedModel].allowMotionFileDrop()) {
               targetModelID = m_selectedModel;
            } else {
               targetModelID = m_render->pickModel(this, pos.x, pos.y, &dropAllowedModelID); /* model ID in curpor position */
               if (targetModelID == -1)
                  targetModelID = dropAllowedModelID;
            }
         } else {
            /* others */
            targetModelID = m_render->pickModel(this, pos.x, pos.y, &dropAllowedModelID); /* model ID in curpor position */
            if (targetModelID == -1)
               targetModelID = dropAllowedModelID;
            if (targetModelID == -1) {
               for (j = 0; j < m_numModel; j++) {
                  if (m_model[j].isEnable() && m_model[j].allowMotionFileDrop()) {
                     targetModelID = j;
                     break;
                  }
               }
            }
         }
         if (targetModelID == -1) {
            g_logger.log(L"Warning: vmd file dropped but no model exit at the point");
         } else {
            /* pause timer to skip file loading time */
            m_timer.pause();
            if (m_keyShift) { /* if Shift-key, insert motion */
               if (targetModelID == MODEL_ALL) {
                  /* all model */
                  for (j = 0; j < m_numModel; j++) {
                     if (m_model[j].isEnable() && m_model[j].allowMotionFileDrop())
                        addMotion(m_model[j].getAlias(), L"insert", droppedFileName, false, true, true, true);
                  }
               } else {
                  /* target model */
                  addMotion(m_model[targetModelID].getAlias(), L"insert", droppedFileName, false, true, true, true);
               }
            } else {
               /* change base motion */
               if (targetModelID == MODEL_ALL) {
                  /* all model */
                  for (j = 0; j < m_numModel; j++) {
                     if (m_model[j].isEnable() && m_model[j].allowMotionFileDrop()) {
                        for (motionPlayer = m_model[j].getMotionManager()->getMotionPlayerList(); motionPlayer; motionPlayer = motionPlayer->next) {
                           if (motionPlayer->active && wcscmp(motionPlayer->name, L"base") == 0) {
                              changeMotion(m_model[j].getAlias(), L"base", droppedFileName);
                              break;
                           }
                        }
                        if (!motionPlayer)
                           addMotion(m_model[j].getAlias(), L"base", droppedFileName, true, false, true, true);
                     }
                  }
               } else {
                  /* target model */
                  for (motionPlayer = m_model[targetModelID].getMotionManager()->getMotionPlayerList(); motionPlayer; motionPlayer = motionPlayer->next) {
                     if (motionPlayer->active && wcscmp(motionPlayer->name, L"base") == 0) {
                        changeMotion(m_model[targetModelID].getAlias(), L"base", droppedFileName);
                        break;
                     }
                  }
                  if (!motionPlayer)
                     addMotion(m_model[targetModelID].getAlias(), L"base", droppedFileName, true, false, true, true);
               }
            }
            /* check mp3 */
            len = wcslen(droppedFileName);
            if (len >= 5) {
               buf = (wchar_t *) malloc(sizeof(wchar_t) * (len + 1));
               wcscpy(buf, droppedFileName);
               buf[len-3] = L'm';
               buf[len-2] = L'p';
               buf[len-1] = L'3';
               fp = _wfopen(buf, L"rb");
               if (fp) {
                  /* start audio */
                  fclose(fp);
                  stopSound(L"audio");
                  startSound(L"audio", buf, true);
               } else {
                  /* stop audio */
                  stopSound(L"audio");
               }
               free(buf);
            }
            /* resume timer */
            m_timer.resume();
         }
      } else if (hasSuffix(droppedFileName, L"xpmd")) {
         /* load stage */
         setStage(droppedFileName);
      } else if (hasSuffix(droppedFileName, L"pmd")) {
         /* drop model */
         if (m_keyCtrl) {
            /* if Ctrl-key, add model */
            addModel(NULL, droppedFileName, NULL, NULL, NULL, NULL);
         } else {
            /* change model */
            if (m_doubleClicked && m_selectedModel != -1) /* already selected */
               targetModelID = m_selectedModel;
            else
               targetModelID = m_render->pickModel(this, pos.x, pos.y, &dropAllowedModelID);
            if (targetModelID == -1) {
               g_logger.log(L"Warning: pmd file dropped but no model at the point");
            } else {
               changeModel(m_model[targetModelID].getAlias(), droppedFileName);
            }
         }
      } else if (hasSuffix(droppedFileName, L"bmp") || hasSuffix(droppedFileName, L"tga") || hasSuffix(droppedFileName, L"png")) {
         if (m_keyCtrl)
            setFloor(droppedFileName); /* change floor with Ctrl-key */
         else
            setBackground(droppedFileName); /* change background without Ctrl-key */
      }
   }
}
