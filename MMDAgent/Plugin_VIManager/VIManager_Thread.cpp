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
#include "VIManager.h"
#include "VIManager_Logger.h"
#include "VIManager_Thread.h"

/* VIManager_Event_initialize: initialize input message buffer */
static void VIManager_Event_initialize(VIManager_Event *e, const char *type, const char *args)
{
   if (type != NULL)
      e->type = MMDAgent_strdup(type);
   else
      e->type = NULL;
   if (args != NULL)
      e->args = MMDAgent_strdup(args);
   else
      e->args = NULL;
   e->next = NULL;
}

/* VIManager_Event_clear: free input message buffer */
static void VIManager_Event_clear(VIManager_Event *e)
{
   if (e->type != NULL)
      free(e->type);
   if (e->args != NULL)
      free(e->args);
   VIManager_Event_initialize(e, NULL, NULL);
}

/* VIManager_EventQueue_initialize: initialize queue */
static void VIManager_EventQueue_initialize(VIManager_EventQueue *q)
{
   q->head = NULL;
   q->tail = NULL;
}

/* VIManager_EventQueue_clear: free queue */
static void VIManager_EventQueue_clear(VIManager_EventQueue *q)
{
   VIManager_Event *tmp1, *tmp2;

   for (tmp1 = q->head; tmp1 != NULL; tmp1 = tmp2) {
      tmp2 = tmp1->next;
      VIManager_Event_clear(tmp1);
      free(tmp1);
   }
   VIManager_EventQueue_initialize(q);
}

/* VIManager_EventQueue_enqueue: enqueue */
static void VIManager_EventQueue_enqueue(VIManager_EventQueue *q, const char *type, const char *args)
{
   if (q->tail == NULL) {
      q->tail = (VIManager_Event *) calloc(1, sizeof (VIManager_Event));
      VIManager_Event_initialize(q->tail, type, args);
      q->head = q->tail;
   } else {
      q->tail->next = (VIManager_Event *) calloc(1, sizeof (VIManager_Event));
      VIManager_Event_initialize(q->tail->next, type, args);
      q->tail = q->tail->next;
   }
}

/* VIManager_EventQueue_dequeue: dequeue */
static int VIManager_EventQueue_dequeue(VIManager_EventQueue *q, char *type, char *args)
{
   VIManager_Event *tmp;

   if (q->head == NULL) {
      if (type != NULL)
         strcpy(type, "");
      if (args != NULL)
         strcpy(type, "");
      return 0;
   }
   if (type != NULL)
      strcpy(type, q->head->type);
   if (args != NULL)
      strcpy(args, q->head->args);
   tmp = q->head->next;
   VIManager_Event_clear(q->head);
   free(q->head);
   q->head = tmp;
   if (tmp == NULL)
      q->tail = NULL;
   return 1;
}

/* main_thread: main thread */
static unsigned __stdcall main_thread(void *param)
{
   VIManager_Thread *vimanager_thread = (VIManager_Thread *) param;

   vimanager_thread->stateTransition();
   _endthreadex(0);
   return 0;
}

/* VIManager_Thread::initialize: initialize thread */
void VIManager_Thread::initialize()
{
   m_sub = NULL;

   m_mmdagent = NULL;

   m_stop = false;

   m_threadHandle = 0;
   m_queueMutex = 0;
   m_transEvent = 0;

   VIManager_EventQueue_initialize(&eventQueue);
}

/* VIManager_Thread::clear: free thread */
void VIManager_Thread::clear()
{
   VIManager_Link *tmp1, *tmp2;

   m_stop = true;
   if(m_transEvent)
      SetEvent(m_transEvent);

   /* stop thread & close mutex */
   if (m_threadHandle != 0) {
      if (WaitForSingleObject(m_threadHandle, VIMANAGERTHREAD_WAITMS) != WAIT_OBJECT_0)
         MessageBoxA(NULL, "Error: cannot stop VIManager thread.", "Error", MB_OK);
      CloseHandle(m_threadHandle);
   }
   if (m_queueMutex)
      CloseHandle(m_queueMutex);
   if (m_transEvent)
      CloseHandle(m_transEvent);

   /* free */
   VIManager_EventQueue_clear(&eventQueue);

   for(tmp1 = m_sub; tmp1 != NULL; tmp1 = tmp2) {
      tmp2 = tmp1->next;
      delete tmp1;
   }

   initialize();
}

/* VIManager_Thread::VIManager_Thread: thread constructor */
VIManager_Thread::VIManager_Thread()
{
   initialize();
}

/* VIManager_Thread::~VIManager_Thread: thread destructor */
VIManager_Thread::~VIManager_Thread()
{
   clear();
}

/* VIManager_Thread::loadAndStart: load FST and start thread */
void VIManager_Thread::loadAndStart(MMDAgent *mmdagent, const char *file)
{
   WIN32_FIND_DATA data;
   HANDLE find;
   char buf[VIMANAGER_MAXBUFLEN];
   char dir[VIMANAGER_MAXBUFLEN];
   char fst[VIMANAGER_MAXBUFLEN];
   int i, j, len;
   VIManager_Link *l, *last;

   if(mmdagent == NULL)
      return;

   /* load FST for VIManager */
   if (m_vim.load(file) == 0)
      return;

   /* setup logger */
   m_logger.setup(mmdagent);

   m_mmdagent = mmdagent;

   /* get dir and fst */
   len = MMDAgent_strlen(file);
   strcpy(dir, file);
   for(i = len - 1; i >= 0; i--) {
      dir[i] = '\0';
      if(file[i] == '\\')
         break;
   }
   for(j = 0, i++; i <= len; i++, j++) {
      fst[j] = file[i];
   }

   /* load sub fst */
   sprintf(buf, "%s*", file);
   find = FindFirstFileA(buf, &data);
   if(find != INVALID_HANDLE_VALUE) {
      do {
         sprintf(buf, "%s\\%s", dir, data.cFileName);
         if(MMDAgent_strequal(data.cFileName, fst) == false) {
            l = new VIManager_Link;
            l->next = NULL;
            if(l->vim.load(buf) == 0) {
               delete l;
            } else {
               if(m_sub == NULL)
                  m_sub = l;
               else
                  last->next = l;
               last = l;
            }
         }
      } while(FindNextFileA(find, &data));
      FindClose(find);
   }

   /* create mutex */
   m_queueMutex = CreateMutex(NULL, false, NULL);
   if (m_queueMutex == 0) {
      MessageBoxA(NULL, "Error: cannot create buffer mutex for VIManager.", "Error", MB_OK);
      return;
   }
   m_transEvent = CreateEvent(NULL, true, false, NULL);
   if (m_transEvent == 0) {
      MessageBoxA(NULL, "Error: cannot create transition event for VIManager.", "Error", MB_OK);
      return;
   }

   /* thread start */
   m_threadHandle = (HANDLE)_beginthreadex(NULL, 0, main_thread, this, 0, NULL);
   if (m_threadHandle == 0) {
      MessageBoxA(NULL, "Error: cannot start VIManager thread.", "Error", MB_OK);
      return;
   }
}

/* VIManager_Thread::isRunning: check running */
bool VIManager_Thread::isRunning()
{
   if (m_threadHandle == 0 || m_stop == true)
      return false;
   return true;
}

/* VIManager_Thread::stopAndRelease: stop thread and release */
void VIManager_Thread::stopAndRelease()
{
   clear();
}

/* VIManager_Thread::enqueueBuffer: enqueue buffer to check */
void VIManager_Thread::enqueueBuffer(const char *type, const char *args)
{
   /* wait buffer */
   if (WaitForSingleObject(m_queueMutex, INFINITE) != WAIT_OBJECT_0)
      MessageBoxA(NULL, "Error: cannot wait buffer mutex for VIManager.", "Error", MB_OK);

   /* save event */
   VIManager_EventQueue_enqueue(&eventQueue, type, args);

   /* release buffer */
   ReleaseMutex(m_queueMutex);

   /* start state transition thread */
   SetEvent(m_transEvent);
}

/* VIManager_Thread::stateTransition: thread loop for VIManager */
void VIManager_Thread::stateTransition()
{
   char itype[VIMANAGER_MAXBUFLEN];
   char iargs[VIMANAGER_MAXBUFLEN];
   char otype[VIMANAGER_MAXBUFLEN];
   char oargs[VIMANAGER_MAXBUFLEN];
   int remain = 1;
   VIManager_Link *l;

   /* first epsilon step */
   while (m_logger.setTransition(m_vim.transition(VIMANAGER_EPSILON, NULL, otype, oargs)) == true) {
      if (MMDAgent_strequal(otype, VIMANAGER_EPSILON) == false)
         m_mmdagent->sendCommandMessage(otype, oargs);
   }

   for(l = m_sub; l != NULL; l = l->next) {
      while (l->vim.transition(VIMANAGER_EPSILON, NULL, otype, oargs) != NULL) {
         if (MMDAgent_strequal(otype, VIMANAGER_EPSILON) == false)
            m_mmdagent->sendCommandMessage(otype, oargs);
      }
   }

   while(m_stop == false) {
      /* wait transition event */
      if (WaitForSingleObject(m_transEvent, INFINITE) != WAIT_OBJECT_0)
         MessageBoxA(NULL, "Error: cannot wait transition event for VIManager.", "Error", MB_OK);
      if(m_stop) return;
      ResetEvent(m_transEvent);

      do {
         /* wait queue access */
         if (WaitForSingleObject(m_queueMutex, INFINITE) != WAIT_OBJECT_0)
            MessageBoxA(NULL, "Error: cannot wait buffer mutex for VIManager.", "Error", MB_OK);
         if(m_stop) return;
         /* load input message */
         remain = VIManager_EventQueue_dequeue(&eventQueue, itype, iargs);
         ReleaseMutex(m_queueMutex);

         if (remain == 0)
            break;

         /* state transition with input symbol */
         m_logger.setTransition(m_vim.transition(itype, iargs, otype, oargs));
         if (MMDAgent_strequal(otype, VIMANAGER_EPSILON) == false)
            m_mmdagent->sendCommandMessage(otype, oargs);

         /* state transition with epsilon */
         while (m_logger.setTransition(m_vim.transition(VIMANAGER_EPSILON, NULL, otype, oargs)) == true) {
            if (MMDAgent_strequal(otype, VIMANAGER_EPSILON) == false)
               m_mmdagent->sendCommandMessage(otype, oargs);
         }

         for(l = m_sub; l != NULL; l = l->next) {
            l->vim.transition(itype, iargs, otype, oargs);
            if (MMDAgent_strequal(otype, VIMANAGER_EPSILON) == false)
               m_mmdagent->sendCommandMessage(otype, oargs);

            /* state transition with epsilon */
            while (l->vim.transition(VIMANAGER_EPSILON, NULL, otype, oargs) != NULL) {
               if (MMDAgent_strequal(otype, VIMANAGER_EPSILON) == false)
                  m_mmdagent->sendCommandMessage(otype, oargs);
            }
         }
      } while (remain);
   }
}

/* VIManager_Thread::renderLog: render log message */
void VIManager_Thread::renderLog()
{
   m_logger.render(m_vim.getCurrentState());
}
