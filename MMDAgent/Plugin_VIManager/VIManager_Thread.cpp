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
#include <process.h>

#include "VIManager.h"
#include "VIManager_Thread.h"

/* VIManager_Event_initialize: initialize input message buffer */
void VIManager_Event_initialize(VIManager_Event *e, char *type, char *args)
{
   if (type != NULL)
      e->type = _strdup(type);
   else
      e->type = NULL;
   if (args != NULL)
      e->args = _strdup(args);
   else
      e->args = NULL;
   e->next = NULL;
}

/* VIManager_Event_clear: free input message buffer */
void VIManager_Event_clear(VIManager_Event *e)
{
   if (e->type != NULL)
      free(e->type);
   if (e->args != NULL)
      free(e->args);
   VIManager_Event_initialize(e, NULL, NULL);
}

/* VIManager_EventQueue_initialize: initialize queue */
void VIManager_EventQueue_initialize(VIManager_EventQueue *q)
{
   q->head = NULL;
   q->tail = NULL;
}

/* VIManager_EventQueue_clear: free queue */
void VIManager_EventQueue_clear(VIManager_EventQueue *q)
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
void VIManager_EventQueue_enqueue(VIManager_EventQueue *q, char *type, char *args)
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
int VIManager_EventQueue_dequeue(VIManager_EventQueue *q, char *type, char *args)
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
   m_window = NULL;
   m_command = NULL;

   m_stop = false;

   m_threadHandle = 0;
   m_queueMutex = 0;
   m_transEvent = 0;

   VIManager_EventQueue_initialize(&eventQueue);
}

/* VIManager_Thread::clear: free thread */
void VIManager_Thread::clear()
{
   m_stop = true;
   if(m_transEvent)
      SetEvent(m_transEvent);

   /* stop thread & close mutex */
   if (m_threadHandle != 0) {
      if (WaitForSingleObject(m_threadHandle, INFINITE) != WAIT_OBJECT_0)
         MessageBox(NULL, L"ERROR: Cannot wait thread end.", L"Error", MB_OK);
      CloseHandle(m_threadHandle);
   }
   if (m_queueMutex)
      CloseHandle(m_queueMutex);
   if (m_transEvent)
      CloseHandle(m_transEvent);

   /* free */
   VIManager_EventQueue_clear(&eventQueue);

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
void VIManager_Thread::loadAndStart(HWND window, UINT command, char *fn)
{
   /* load FST for VIManager */
   if (m_vim.load(fn) == 0)
      return;

   m_window = window;
   m_command = command;

   /* create mutex */
   m_queueMutex = CreateMutex(NULL, false, NULL);
   if (m_queueMutex == 0) {
      MessageBox(NULL, L"ERROR: Cannot create mutex.", L"Error", MB_OK);
      return;
   }
   m_transEvent = CreateEvent(NULL, true, false, NULL);
   if (m_transEvent == 0) {
      MessageBox(NULL, L"ERROR: Cannot create event.", L"Error", MB_OK);
      return;
   }

   /* thread start */
   m_threadHandle = (HANDLE)_beginthreadex(NULL, 0, main_thread, this, 0, NULL);
   if (m_threadHandle == 0) {
      MessageBox(NULL, L"ERROR: Cannot start VIManager thread.", L"Error", MB_OK);
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
void VIManager_Thread::enqueueBuffer(char *type, char *args)
{
   /* wait buffer */
   if (WaitForSingleObject(m_queueMutex, INFINITE) != WAIT_OBJECT_0)
      MessageBox(NULL, L"ERROR: Cannot wait buffer.", L"Error", MB_OK);

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

   /* first epsilon step */
   while (m_vim.transition(VIMANAGER_EPSILON, NULL, otype, oargs)) {
      if (strcmp(otype, VIMANAGER_EPSILON) != 0)
         sendMessage(otype, oargs);
   }

   while(m_stop == false) {
      /* wait transition event */
      if (WaitForSingleObject(m_transEvent, INFINITE) != WAIT_OBJECT_0)
         MessageBox(NULL, L"ERROR: Cannot wait event.", L"Error", MB_OK);
      if(m_stop) return;
      ResetEvent(m_transEvent);

      do {
         /* wait queue access */
         if (WaitForSingleObject(m_queueMutex, INFINITE) != WAIT_OBJECT_0)
            MessageBox(NULL, L"ERROR: Cannot wait buffer.", L"Error", MB_OK);
         if(m_stop) return;
         /* load input message */
         remain = VIManager_EventQueue_dequeue(&eventQueue, itype, iargs);
         ReleaseMutex(m_queueMutex);

         if (remain == 0)
            break;

         /* state transition with input symbol */
         m_vim.transition(itype, iargs, otype, oargs);
         if (strcmp(otype, VIMANAGER_EPSILON) != 0)
            sendMessage(otype, oargs);

         /* state transition with epsilon */
         while (m_vim.transition(VIMANAGER_EPSILON, NULL, otype, oargs)) {
            if (strcmp(otype, VIMANAGER_EPSILON) != 0)
               sendMessage(otype, oargs);
         }
      } while (remain);
   }
}

/* VIManager_Thread::sendMessage: send message to MMDAgent */
void VIManager_Thread::sendMessage(char *str1, char *str2)
{
   char *mes1, *mes2;

   if(str1 == NULL)
      return;

   mes1 = strdup(str1);
   if(str2 != NULL)
      mes2 = strdup(str2);
   else
      mes2 = strdup("");

   ::PostMessage(m_window, m_command, (WPARAM) mes1, (LPARAM) mes2);
}
