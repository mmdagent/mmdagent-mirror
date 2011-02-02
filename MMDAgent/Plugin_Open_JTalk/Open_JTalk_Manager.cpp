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

/* headers */

#include <process.h>

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

/* Open_JTalk_Event_initialize: initialize input message buffer */
static void Open_JTalk_Event_initialize(Open_JTalk_Event *e, const char *str)
{
   if (str != NULL)
      e->event = MMDAgent_strdup(str);
   else
      e->event = NULL;
   e->next = NULL;
}

/* Open_JTalk_Event_clear: free input message buffer */
static void Open_JTalk_Event_clear(Open_JTalk_Event *e)
{
   if (e->event != NULL)
      free(e->event);
   Open_JTalk_Event_initialize(e, NULL);
}

/* Open_JTalk_EventQueue_initialize: initialize queue */
static void Open_JTalk_EventQueue_initialize(Open_JTalk_EventQueue *q)
{
   q->head = NULL;
   q->tail = NULL;
}

/* Open_JTalk_EventQueue_clear: free queue */
static void Open_JTalk_EventQueue_clear(Open_JTalk_EventQueue *q)
{
   Open_JTalk_Event *tmp1, *tmp2;

   for (tmp1 = q->head; tmp1 != NULL; tmp1 = tmp2) {
      tmp2 = tmp1->next;
      Open_JTalk_Event_clear(tmp1);
      free(tmp1);
   }
   Open_JTalk_EventQueue_initialize(q);
}

/* Open_JTalk_EventQueue_enqueue: enqueue */
static void Open_JTalk_EventQueue_enqueue(Open_JTalk_EventQueue *q, const char *str)
{
   if(MMDAgent_strlen(str) <= 0)
      return;

   if (q->tail == NULL) {
      q->tail = (Open_JTalk_Event *) calloc(1, sizeof (Open_JTalk_Event));
      Open_JTalk_Event_initialize(q->tail, str);
      q->head = q->tail;
   } else {
      q->tail->next = (Open_JTalk_Event *) calloc(1, sizeof (Open_JTalk_Event));
      Open_JTalk_Event_initialize(q->tail->next, str);
      q->tail = q->tail->next;
   }
}

/* Open_JTalk_EventQueue_dequeue: dequeue */
static void Open_JTalk_EventQueue_dequeue(Open_JTalk_EventQueue *q, char **str)
{
   Open_JTalk_Event *tmp;

   if (q->head == NULL) {
      *str = NULL;
      return;
   }
   *str = MMDAgent_strdup(q->head->event);

   tmp = q->head->next;
   Open_JTalk_Event_clear(q->head);
   free(q->head);
   q->head = tmp;
   if (tmp == NULL)
      q->tail = NULL;
}

/* main_thread: main thread */
static unsigned __stdcall main_thread(void *param)
{
   Open_JTalk_Manager *open_jtalk_manager = (Open_JTalk_Manager *) param;

   open_jtalk_manager->start();

   _endthreadex(0);
   return 0;
}

/* Open_JTalk_Manager::initialize: initialize */
void Open_JTalk_Manager::initialize()
{
   m_list = NULL;

   m_window = 0;
   m_event = 0;
   m_command = 0;

   m_threadHandle = 0;
   m_bufferMutex = 0;
   m_synthEvent = 0;

   m_dicDir = NULL;
   m_config = NULL;

   m_kill = false;

   Open_JTalk_EventQueue_initialize(&m_bufferQueue);
}

/* Open_JTalk_Manager::clear clear */
void Open_JTalk_Manager::clear()
{
   Open_JTalk_Link *tmp1, *tmp2;

   m_kill = true;

   /* stop and release all thread */
   for(tmp1 = m_list; tmp1; tmp1 = tmp2) {
      tmp2 = tmp1->next;
      tmp1->open_jtalk_thread.stopAndRelease();
      delete tmp1;
   }

   /* wait */
   if(m_synthEvent)
      SetEvent(m_synthEvent);

   if(m_threadHandle != 0) {
      if(WaitForSingleObject(m_threadHandle, OPENJTALKMANAGER_WAITMS) != WAIT_OBJECT_0)
         MessageBoxA(NULL, "Error: cannot stop Open JTalk thread.", "Error", MB_OK);
      CloseHandle(m_threadHandle);
   }
   if(m_bufferMutex)
      CloseHandle(m_bufferMutex);
   if(m_synthEvent)
      CloseHandle(m_synthEvent);

   if(m_dicDir)
      free(m_dicDir);
   if(m_config)
      free(m_config);

   Open_JTalk_EventQueue_clear(&m_bufferQueue);

   initialize();
}

/* Open_JTalk_Manager::Open_JTalk_Manager: constructor */
Open_JTalk_Manager::Open_JTalk_Manager()
{
   initialize();
}

/* Open_JTalk_Manager::~Open_JTalk_Manager: destructor */
Open_JTalk_Manager::~Open_JTalk_Manager()
{
   clear();
}

/* Open_JTalk_Manager::loadAndStart: load and start thread */
void Open_JTalk_Manager::loadAndStart(HWND hWnd, UINT event, UINT command, const char *dicDir, const char *config)
{
   clear();

   m_window = hWnd;
   m_event = event;
   m_command = command;
   m_dicDir = MMDAgent_strdup(dicDir);
   m_config = MMDAgent_strdup(config);

   if(m_window == 0 || m_event == 0 || m_command == 0 || m_dicDir == NULL || m_config == NULL) {
      clear();
      return;
   }

   m_threadHandle = (HANDLE) _beginthreadex(NULL, 0, main_thread, this, 0, NULL);
   if(m_threadHandle == 0) {
      MessageBoxA(NULL, "Error: cannot start Open JTalk thread.", "Error", MB_OK);
      clear();
   }
}

/* Open_JTalk_Manager::stopAndRelease: stop and release thread */
void Open_JTalk_Manager::stopAndRelease()
{
   clear();
}

/* Open_JTalk_Manager::start: main loop */
void Open_JTalk_Manager::start()
{
   int i;
   Open_JTalk_Link *link;
   char *buff, *save;
   char *chara, *style, *text;

   /* check */
   if(m_threadHandle == 0 || m_bufferMutex != 0 || m_synthEvent != 0) return;

   /* create buffer mutex */
   m_bufferMutex = CreateMutex(NULL, false, NULL);
   if(m_bufferMutex == 0) {
      MessageBoxA(NULL, "Error: cannot create buffer mutex for Open JTalk.", "Error", MB_OK);
      return;
   }

   /* create synthesis event */
   m_synthEvent = CreateEvent(NULL, true, false, NULL);
   if(m_synthEvent == 0) {
      MessageBoxA(NULL, "Error: cannot create synthesis event for Open JTalk.", "Error", MB_OK);
      return;
   }

   /* create initial threads */
   for(i = 0; i < OPENJTALKMANAGER_INITIALNTHREAD; i++) {
      link = new Open_JTalk_Link;
      link->open_jtalk_thread.loadAndStart(m_window, m_event, m_command, m_dicDir, m_config);
      link->next = m_list;
      m_list = link;
   }

   while(m_kill == false) {
      /* wait synthesis event */
      if(WaitForSingleObject(m_synthEvent, INFINITE) != WAIT_OBJECT_0) {
         MessageBoxA(NULL, "Error: cannot wait synthesis event for Open JTalk.", "Error", MB_OK);
         return;
      }
      ResetEvent(m_synthEvent);

      while(m_kill == false) {
         /* wait buffer mutex */
         if(WaitForSingleObject(m_bufferMutex, INFINITE) != WAIT_OBJECT_0) {
            MessageBoxA(NULL, "Error: cannot wait buffer mutex for Open JTalk.", "Error", MB_OK);
            ReleaseMutex(m_bufferMutex);
            return;
         }
         Open_JTalk_EventQueue_dequeue(&m_bufferQueue, &buff); /* get buffer */
         ReleaseMutex(m_bufferMutex);

         if(buff == NULL) break;

         chara = MMDAgent_strtok(buff, "|", &save);
         style = MMDAgent_strtok(NULL, "|", &save);
         text = MMDAgent_strtok(NULL, "|", &save);

         if(chara != NULL && style != NULL || text != NULL) {
            /* check character */
            for(i = 0, link = m_list; link; link = link->next, i++)
               if(link->open_jtalk_thread.checkCharacter(chara))
                  break;
            if(link) {
               link->open_jtalk_thread.stop(); /* if the same character is speaking, stop immediately */
            } else {
               for(i = 0, link = m_list; link; link = link->next, i++)
                  if(link->open_jtalk_thread.isRunning() && link->open_jtalk_thread.isSpeaking() == false)
                     break;
               if(link == NULL) {
                  link = new Open_JTalk_Link;
                  link->open_jtalk_thread.loadAndStart(m_window, m_event, m_command, m_dicDir, m_config);
                  link->next = m_list;
                  m_list = link;
               }
            }
            /* set */
            link->open_jtalk_thread.synthesis(chara, style, text);
         }

         free(buff); /* free buffer */
      }
   }
}

/* Open_JTalk_Manager::isRunning: check running */
bool Open_JTalk_Manager::isRunning()
{
   if(m_threadHandle == 0 || m_kill == true)
      return false;
   else
      return true;
}

/* Open_JTalk_Manager::synthesis: start synthesis */
void Open_JTalk_Manager::synthesis(const char *str)
{
   /* check */
   if(MMDAgent_strlen(str) <= 0 || m_bufferMutex == 0 || m_synthEvent == 0)
      return;

   /* wait buffer mutex */
   if(WaitForSingleObject(m_bufferMutex, INFINITE) != WAIT_OBJECT_0) {
      MessageBoxA(NULL, "Error: cannot wait buffer mutex for Open JTalk.", "Error", MB_OK);
      return;
   }

   /* enqueue character name, speaking style, and text */
   Open_JTalk_EventQueue_enqueue(&m_bufferQueue, str);

   /* release buffer mutex */
   ReleaseMutex(m_bufferMutex);

   /* start synthesis event */
   SetEvent(m_synthEvent);
}

/* Open_JTalk_Manager::stop: stop synthesis */
void Open_JTalk_Manager::stop(const char *str)
{
   Open_JTalk_Link *link;

   for(link = m_list; link; link = link->next) {
      if(link->open_jtalk_thread.checkCharacter(str)) {
         link->open_jtalk_thread.stop();
         return;
      }
   }
}
