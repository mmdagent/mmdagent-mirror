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
#include <time.h>

#include "CountDown_Thread.h"

/* main_thread: main thread */
static unsigned __stdcall main_thread(void *param)
{
   CountDown_Thread *cdt = (CountDown_Thread *) param;

   while (cdt->isRunning())
      cdt->check();

   _endthreadex(0);
   return 0;
}

/* CountDown_Thread::initialize: initialize thread */
void CountDown_Thread::initialize()
{
   m_head = NULL;
   m_tail = NULL;

   m_window = 0;
   m_event = 0;

   m_threadHandle = 0;
   m_mutex = 0;

   m_stop = false;
}

/* CountDown_Thread::clear: free thread */
void CountDown_Thread::clear()
{
   CountDown *tmp1, *tmp2;

   m_stop = true;

   /* wait end of thread */
   if(m_threadHandle != 0) {
      if (WaitForSingleObject(m_threadHandle, INFINITE) != WAIT_OBJECT_0)
         MessageBoxA(NULL, "ERROR: Cannot wait thread end.", "Error", MB_OK);
      CloseHandle(m_threadHandle);
   }

   if(m_mutex)
      CloseHandle(m_mutex);

   for(tmp1 = m_head; tmp1 ; tmp1 = tmp2) {
      tmp2 = tmp1->next;
      free(tmp1->name);
      free(tmp1);
   }

   initialize();
}

/* CountDown_Thread::CountDown_Thread: thread constructor */
CountDown_Thread::CountDown_Thread()
{
   initialize();
}

/* CountDown_Thread::~CountDown_Thread: thread destructor */
CountDown_Thread::~CountDown_Thread()
{
   clear();
}

/* CountDown_Thread::loadAndStart: load variables and start thread */
void CountDown_Thread::loadAndStart(HWND param1, UINT param2)
{
   m_window = param1;
   m_event = param2;

   m_mutex = CreateMutex(NULL, false, NULL);
   if(m_mutex == 0) {
      MessageBoxA(NULL, "ERROR: Cannot create mutex.", "Error", MB_OK);
      return;
   }

   /* thread start */
   m_threadHandle = (HANDLE) _beginthreadex(NULL, 0, main_thread, this, 0, NULL);
   if (m_threadHandle == 0) {
      MessageBoxA(NULL, "ERROR: Cannot start CountDown thread.", "Error", MB_OK);
      return;
   }
}

/* CountDown_Thead::check: check timers */
void CountDown_Thread::check()
{
   CountDown *tmp1, *tmp2;
   int now;

   /* wait */
   if(WaitForSingleObject(m_mutex, INFINITE) != WAIT_OBJECT_0)
      MessageBoxA(NULL, "ERROR: Cannot wait count down.", "Error", MB_OK);

   now = clock() / CLOCKS_PER_SEC;

   for(tmp1 = m_head; tmp1; tmp1 = tmp2) {
      tmp2 = tmp1->next;
      if(tmp1->goal <= now) {
         if(tmp1 == m_head) {
            if(tmp1 == m_tail) {
               m_head = NULL;
               m_tail = NULL;
            } else {
               m_head = tmp1->next;
               tmp1->next->prev = NULL;
            }
         } else {
            if(tmp1 == m_tail) {
               tmp1->prev->next = NULL;
               m_tail = tmp1->prev;
            } else {
               tmp1->prev->next = tmp1->next;
               tmp1->next->prev = tmp1->prev;
            }
         }
         sendStopEventMessage(tmp1->name); /* send message */
         free(tmp1->name);
         free(tmp1);
      }
   }

   /* release */
   ReleaseMutex(m_mutex);

   Sleep(COUNTDOWNTHREAD_SLEEPMS);
}

/* CountDown_Thread::isRunning: check running */
bool CountDown_Thread::isRunning()
{
   if (m_threadHandle == 0 || m_stop == true)
      return false;
   return true;
}

/* CountDown_Thread::stopAndRelease: stop thread and free Open JTalk */
void CountDown_Thread::stopAndRelease()
{
   clear();
}

/* CountDown_Thread::set: set timer */
void CountDown_Thread::set(char *alias, int sec)
{
   CountDown *countDown;
   int now;

   if(alias == NULL || strlen(alias) <= 0) return;

   /* wait */
   if(WaitForSingleObject(m_mutex, INFINITE) != WAIT_OBJECT_0)
      MessageBoxA(NULL, "ERROR: Cannot wait count down.", "Error", MB_OK);

   now = (clock() / CLOCKS_PER_SEC);

   /* check the same alias */
   for(countDown = m_head; countDown; countDown = countDown->next) {
      if(strcmp(countDown->name, alias) == 0) {
         countDown->goal = now + sec;
         return;
      }
   }

   /* push timer */
   countDown = (CountDown *) malloc(sizeof(CountDown));
   countDown->name = strdup(alias);
   countDown->goal = now + sec;
   countDown->next = NULL;
   if(m_tail == NULL) {
      m_head = countDown;
      countDown->prev = NULL;
   } else {
      m_tail->next = countDown;
      countDown->prev = m_tail;
   }
   m_tail = countDown;

   sendStartEventMessage(countDown->name); /* send message */

   /* release */
   ReleaseMutex(m_mutex);
}

/* CountDown_Thread::unset: unset timer */
void CountDown_Thread::unset(char *alias)
{
   CountDown *tmp1, *tmp2;

   /* wait */
   if(WaitForSingleObject(m_mutex, INFINITE) != WAIT_OBJECT_0)
      MessageBoxA(NULL, "ERROR: Cannot wait count down.", "Error", MB_OK);

   for(tmp1 = m_head; tmp1; tmp1 = tmp2) {
      tmp2 = tmp1->next;
      if(strcmp(tmp1->name, alias) == 0) {
         if(tmp1 == m_head) {
            if(tmp1 == m_tail) {
            } else {
               m_head = tmp1->next;
               tmp1->next->prev = NULL;
            }
         } else {
            if(tmp1 == m_tail) {
               m_tail = tmp1->prev;
               tmp1->prev->next = NULL;
            } else {
               tmp1->next->prev = tmp1->prev;
               tmp1->prev->next = tmp1->prev;
            }
         }
         sendStopEventMessage(tmp1->name); /* send message */
         free(tmp1->name);
         free(tmp1);
         break;
      }
   }

   /* release */
   ReleaseMutex(m_mutex);
}

/* CountDown_Thread::sendStartEventMessage: send start event message to MMDAgent */
void CountDown_Thread::sendStartEventMessage(char *str)
{
   char *mes1;
   char *mes2;

   if(str == NULL)
      return;

   mes1 = strdup(COUNTDOWNTHREAD_TIMERSTARTEVENT);
   mes2 = strdup(str);
   ::PostMessage(m_window, m_event, (WPARAM) mes1, (LPARAM) mes2);
}

/* CountDown_Thread::sendStopEventMessage: send stop event message to MMDAgent */
void CountDown_Thread::sendStopEventMessage(char *str)
{
   char *mes1;
   char *mes2;

   if(str == NULL)
      return;

   mes1 = strdup(COUNTDOWNTHREAD_TIMERSTOPEVENT);
   mes2 = strdup(str);

   ::PostMessage(m_window, m_event, (WPARAM) mes1, (LPARAM) mes2);
}
