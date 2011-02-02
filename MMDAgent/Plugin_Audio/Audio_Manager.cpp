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
#include "Audio_Thread.h"
#include "Audio_Manager.h"

/* Audio_Event_initialize: initialize input message buffer */
static void Audio_Event_initialize(Audio_Event *e, const char *str)
{
   if (str != NULL)
      e->event = MMDAgent_strdup(str);
   else
      e->event = NULL;
   e->next = NULL;
}

/* Audio_Event_clear: free input message buffer */
static void Audio_Event_clear(Audio_Event *e)
{
   if (e->event != NULL)
      free(e->event);
   Audio_Event_initialize(e, NULL);
}

/* Audio_EventQueue_initialize: initialize queue */
static void Audio_EventQueue_initialize(Audio_EventQueue *q)
{
   q->head = NULL;
   q->tail = NULL;
}

/* Audio_EventQueue_clear: free queue */
static void Audio_EventQueue_clear(Audio_EventQueue *q)
{
   Audio_Event *tmp1, *tmp2;

   for (tmp1 = q->head; tmp1 != NULL; tmp1 = tmp2) {
      tmp2 = tmp1->next;
      Audio_Event_clear(tmp1);
      free(tmp1);
   }
   Audio_EventQueue_initialize(q);
}

/* Audio_EventQueue_enqueue: enqueue */
static void Audio_EventQueue_enqueue(Audio_EventQueue *q, const char *str)
{
   if(MMDAgent_strlen(str) <= 0)
      return;

   if (q->tail == NULL) {
      q->tail = (Audio_Event *) calloc(1, sizeof (Audio_Event));
      Audio_Event_initialize(q->tail, str);
      q->head = q->tail;
   } else {
      q->tail->next = (Audio_Event *) calloc(1, sizeof (Audio_Event));
      Audio_Event_initialize(q->tail->next, str);
      q->tail = q->tail->next;
   }
}

/* Audio_EventQueue_dequeue: dequeue */
static void Audio_EventQueue_dequeue(Audio_EventQueue *q, char **str)
{
   Audio_Event *tmp;

   if (q->head == NULL) {
      *str = NULL;
      return;
   }
   *str = MMDAgent_strdup(q->head->event);

   tmp = q->head->next;
   Audio_Event_clear(q->head);
   free(q->head);
   q->head = tmp;
   if (tmp == NULL)
      q->tail = NULL;
}

/* main_thread: main thread */
static unsigned __stdcall main_thread(void *param)
{
   Audio_Manager *audio_manager = (Audio_Manager *) param;

   audio_manager->start();

   _endthreadex(0);
   return 0;
}

/* Audio_Manager::initialize: initialize */
void Audio_Manager::initialize()
{
   m_list = NULL;

   m_window = 0;
   m_event = 0;

   m_threadHandle = 0;
   m_bufferMutex = 0;
   m_playingEvent = 0;

   m_kill = false;

   Audio_EventQueue_initialize(&m_bufferQueue);
}

/* Audio_Manager::clear: clear */
void Audio_Manager::clear()
{
   Audio_Link *tmp1, *tmp2;

   m_kill = true;

   /* stop and release all thread */
   for(tmp1 = m_list; tmp1; tmp1 = tmp2) {
      tmp2 = tmp1->next;
      tmp1->audio_thread.stopAndRelease();
      delete tmp1;
   }

   /* wait */
   if(m_playingEvent)
      SetEvent(m_playingEvent);

   if(m_threadHandle != 0) {
      if(WaitForSingleObject(m_threadHandle, AUDIOMANAGER_WAITMS) != WAIT_OBJECT_0)
         MessageBoxA(NULL, "Error: cannot stop audio thread.", "Error", MB_OK);
      CloseHandle(m_threadHandle);
   }
   if(m_bufferMutex)
      CloseHandle(m_bufferMutex);
   if(m_playingEvent)
      CloseHandle(m_playingEvent);

   Audio_EventQueue_clear(&m_bufferQueue);

   initialize();
}

/* Audio_Manager::Audio_Manager: constructor */
Audio_Manager::Audio_Manager()
{
   initialize();
}

/* Audio_Manager::~Audio_Manager: destructor */
Audio_Manager::~Audio_Manager()
{
   clear();
}

/* Audio_Manager::setupAndStart: setup and start thread */
void Audio_Manager::setupAndStart(HWND hWnd, UINT event)
{
   clear();

   m_window = hWnd;
   m_event = event;

   if(m_window == 0 || m_event == 0) {
      clear();
      return;
   }

   m_threadHandle = (HANDLE) _beginthreadex(NULL, 0, main_thread, this, 0, NULL);
   if(m_threadHandle == 0) {
      MessageBoxA(NULL, "Error: cannot start audio thread.", "Error", MB_OK);
      clear();
   }
}

/* Audio_Manager::stopAndRelease: stop and release thread */
void Audio_Manager::stopAndRelease()
{
   clear();
}

/* Audio_Manager::start: main loop */
void Audio_Manager::start()
{
   int i;
   Audio_Link *link;
   char *buff, *save;
   char *alias, *file;

   /* check */
   if(m_threadHandle == 0 || m_bufferMutex != 0 || m_playingEvent != 0) return;

   /* create buffer mutex */
   m_bufferMutex = CreateMutex(NULL, false, NULL);
   if(m_bufferMutex == 0) {
      MessageBoxA(NULL, "Error: cannot create buffer mutex for audio.", "Error", MB_OK);
      return;
   }

   /* create playing event */
   m_playingEvent = CreateEvent(NULL, true, false, NULL);
   if(m_playingEvent == 0) {
      MessageBoxA(NULL, "Error: cannot create playing event for audio.", "Error", MB_OK);
      return;
   }

   /* create initial threads */
   for(i = 0; i < AUDIOMANAGER_INITIALNTHREAD; i++) {
      link = new Audio_Link;
      link->audio_thread.setupAndStart(m_window, m_event);
      link->next = m_list;
      m_list = link;
   }

   while(m_kill == false) {
      /* wait playing event */
      if(WaitForSingleObject(m_playingEvent, INFINITE) != WAIT_OBJECT_0) {
         MessageBoxA(NULL, "Error: cannot wait playing event for audio.", "Error", MB_OK);
         return;
      }
      ResetEvent(m_playingEvent);

      while(m_kill == false) {
         /* wait buffer mutex */
         if(WaitForSingleObject(m_bufferMutex, INFINITE) != WAIT_OBJECT_0) {
            MessageBoxA(NULL, "Error: cannot wait buffer mutex for audio.", "Error", MB_OK);
            ReleaseMutex(m_bufferMutex);
            return;
         }
         Audio_EventQueue_dequeue(&m_bufferQueue, &buff); /* get buffer */
         ReleaseMutex(m_bufferMutex);

         if(buff == NULL) break;

         alias = MMDAgent_strtok(buff, "|", &save);
         file = MMDAgent_strtok(NULL, "|", &save);

         if(alias != NULL && file != NULL) {
            /* check alias */
            for(i = 0, link = m_list; link; link = link->next, i++)
               if(link->audio_thread.checkAlias(alias))
                  break;
            if(link) {
               link->audio_thread.stop(); /* if the same alias is playing, stop immediately */
            } else {
               for(i = 0, link = m_list; link; link = link->next, i++)
                  if(link->audio_thread.isRunning() && link->audio_thread.isPlaying() == false)
                     break;
               if(link == NULL) {
                  link = new Audio_Link;
                  link->audio_thread.setupAndStart(m_window, m_event);
                  link->next = m_list;
                  m_list = link;
               }
            }
            /* set */
            link->audio_thread.play(alias, file);
         }

         free(buff); /* free buffer */
      }
   }
}

/* Audio_Manager::isRunning: check running */
bool Audio_Manager::isRunning()
{
   if(m_threadHandle == 0 || m_kill == true)
      return false;
   else
      return true;
}

/* Audio_Manager::play: start playing */
void Audio_Manager::play(const char *str)
{
   /* check */
   if(MMDAgent_strlen(str) <= 0 || m_bufferMutex == 0 || m_playingEvent == 0)
      return;

   /* wait buffer mutex */
   if(WaitForSingleObject(m_bufferMutex, INFINITE) != WAIT_OBJECT_0) {
      MessageBoxA(NULL, "Error: cannot wait buffer mutex for audio.", "Error", MB_OK);
      return;
   }

   /* enqueue alias and file name */
   Audio_EventQueue_enqueue(&m_bufferQueue, str);

   /* release buffer mutex */
   ReleaseMutex(m_bufferMutex);

   /* start playing event */
   SetEvent(m_playingEvent);
}

/* Audio_Manager::stop: stop playing */
void Audio_Manager::stop(const char *str)
{
   Audio_Link *link;

   for(link = m_list; link; link = link->next) {
      if(link->audio_thread.checkAlias(str)) {
         link->audio_thread.stop();
         return;
      }
   }
}
