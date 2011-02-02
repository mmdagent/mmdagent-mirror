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

#include <windows.h>
#include <process.h>

#include "MMDAgent.h"
#include "Audio_Thread.h"

/* main_thread: main thread */
static unsigned __stdcall main_thread(void *param)
{
   Audio_Thread *audio_thread = (Audio_Thread *) param;

   audio_thread->start();

   _endthreadex(0);
   return 0;
}

/* Audio_Thread::initialize: initialize thread */
void Audio_Thread::initialize()
{
   m_window = 0;
   m_event = 0;

   m_threadHandle = 0;
   m_bufferMutex = 0;
   m_playingEvent = 0;

   m_playing = false;
   m_kill = false;

   m_alias = NULL;
   m_file = NULL;
}

/* Audio_Thread::clear: free thread */
void Audio_Thread::clear()
{
   stop();
   m_kill = true;

   /* wait */
   if (m_playingEvent)
      SetEvent(m_playingEvent);

   if(m_threadHandle != 0) {
      if (WaitForSingleObject(m_threadHandle, AUDIOTHREAD_WAITMS) != WAIT_OBJECT_0)
         MessageBoxA(NULL, "ERROR: Cannot stop Audio thread.", "Error", MB_OK);
      CloseHandle(m_threadHandle);
   }
   if (m_bufferMutex)
      CloseHandle(m_bufferMutex);
   if (m_playingEvent)
      CloseHandle(m_playingEvent);

   if(m_alias) free(m_alias);
   if(m_file) free(m_file);

   initialize();
}

/* Audio_Thread::Audio_Thread: thread constructor */
Audio_Thread::Audio_Thread()
{
   initialize();
}

/* Audio_Thread::~Audio_Thread: thread destructor */
Audio_Thread::~Audio_Thread()
{
   clear();
}

/* Audio_Thread::setupAndStart: setup and start thread */
void Audio_Thread::setupAndStart(HWND window, UINT event)
{
   m_window = window;
   m_event = event;

   /* create mutex */
   m_bufferMutex = CreateMutex(NULL, false, NULL);
   if (m_bufferMutex == 0) {
      MessageBoxA(NULL, "ERROR: Cannot create mutex.", "Error", MB_OK);
      clear();
      return;
   }

   /* create event */
   m_playingEvent = CreateEvent(NULL, true, false, NULL);
   if (m_playingEvent == 0) {
      MessageBoxA(NULL, "ERROR: Cannot create event.", "Error", MB_OK);
      clear();
      return;
   }

   /* thread start */
   m_threadHandle = (HANDLE)_beginthreadex(NULL, 0, main_thread, this, 0, NULL);
   if (m_threadHandle == 0) {
      MessageBoxA(NULL, "ERROR: Cannot start Open JTalk thread.", "Error", MB_OK);
      clear();
      return;
   }
}

/* Audio_Thread::stopAndRelease: stop thread and free Open JTalk */
void Audio_Thread::stopAndRelease()
{
   clear();
}

/* Audio_Thread::start: main thread loop for TTS */
void Audio_Thread::start()
{
   char buf[AUDIOTHREAD_MAXBUFLEN];
   char ret[AUDIOTHREAD_MAXBUFLEN];
   char *alias, *file;

   /* check */
   if(m_threadHandle == 0 || m_bufferMutex == 0 || m_playingEvent == 0)
      return;

   while (m_kill == false) {
      /* wait event */
      if (WaitForSingleObject(m_playingEvent, INFINITE) != WAIT_OBJECT_0) {
         MessageBoxA(NULL, "ERROR: Cannot wait event.", "Error", MB_OK);
         return;
      }
      ResetEvent(m_playingEvent);

      if(m_kill == false) {
         /* wait text */
         if (WaitForSingleObject(m_bufferMutex, INFINITE) != WAIT_OBJECT_0) {
            MessageBoxA(NULL, "ERROR: Cannot wait buffer.", "Error", MB_OK);
            return;
         }
         alias = MMDAgent_strdup(m_alias);
         file = MMDAgent_strdup(m_file);
         m_playing = true;
         ReleaseMutex(m_bufferMutex);

         /* wait */
         sprintf(buf, "open \"%s\" alias _%s wait", file, alias);
         if (mciSendStringA(buf, NULL, 0, 0) != 0) {
            if(alias) free(alias);
            if(file) free(file);
            continue;
         }

         /* enqueue */
         sprintf(buf, "cue _%s output wait", alias);
         if (mciSendStringA(buf, NULL, 0, 0) != 0) {
            sprintf(buf, "close _%s wait", alias);
            mciSendStringA(buf, NULL, 0, 0);
            if(alias) free(alias);
            if(file) free(file);
            continue;
         }

         /* start */
         sprintf(buf, "play _%s", alias);
         if (mciSendStringA(buf, NULL, 0, 0) != 0) {
            sprintf(buf, "close _%s wait", alias);
            mciSendStringA(buf, NULL, 0, 0);
            if(alias) free(alias);
            if(file) free(file);
            continue;
         }

         /* send SOUND_EVENT_START */
         sendStartEventMessage(alias);

         do {
            /* check user's stop */
            if(m_playing == false) {
               sprintf(buf, "stop _%s wait", alias);
               mciSendStringA(buf, NULL, 0, NULL);
               break;
            }
            Sleep(AUDIOTHREAD_SLEEPMS);
            /* check end of sound */
            sprintf(buf, "status _%s mode wait", alias);
            mciSendStringA(buf, ret, sizeof(ret), NULL);
         } while(MMDAgent_strequal(ret, "playing") == true);

         /* send SOUND_EVENT_STOP */
         sendStopEventMessage(alias);

         sprintf(buf, "close _%s wait", alias);
         mciSendStringA(buf, NULL, 0, NULL);

         if(alias) free(alias);
         if(file) free(file);
         m_playing = false;
      }
   }
}

/* Audio_Thread::isRunning: check running */
bool Audio_Thread::isRunning()
{
   if (m_threadHandle == 0 || m_kill == true)
      return false;
   return true;
}

/* Audio_Thread::isPlaying: check playing */
bool Audio_Thread::isPlaying()
{
   return m_playing;
}

/* checkAlias: check playing alias */
bool Audio_Thread::checkAlias(char *alias)
{
   bool ret;

   /* check */
   if(isRunning() == false || m_bufferMutex == 0)
      return false;

   /* wait buffer mutex */
   if (WaitForSingleObject(m_bufferMutex, INFINITE) != WAIT_OBJECT_0) {
      MessageBoxA(NULL, "ERROR: Cannot wait buffer.", "Error", MB_OK);
      return false;
   }

   /* save character name, speaking style, and text */
   ret = MMDAgent_strequal(m_alias, alias);

   /* release buffer mutex */
   ReleaseMutex(m_bufferMutex);

   return ret;
}

/* Audio_Thread::play: start playing */
void Audio_Thread::play(char *alias, char *file)
{
   /* check */
   if(isRunning() == false || m_bufferMutex == 0 || m_playingEvent == 0)
      return;
   if(MMDAgent_strlen(alias) <= 0 || MMDAgent_strlen(file) <= 0)
      return;

   /* wait buffer mutex */
   if (WaitForSingleObject(m_bufferMutex, INFINITE) != WAIT_OBJECT_0) {
      MessageBoxA(NULL, "ERROR: Cannot wait buffer.", "Error", MB_OK);
      return;
   }

   /* save character name, speaking style, and text */
   if(m_alias) free(m_alias);
   if(m_file) free(m_file);
   m_alias = MMDAgent_strdup(alias);
   m_file = MMDAgent_strdup(file);

   /* release buffer mutex */
   ReleaseMutex(m_bufferMutex);

   /* start playing thread */
   SetEvent(m_playingEvent);
}

/* Audio_Thread::stop: stop playing */
void Audio_Thread::stop()
{
   if(isRunning())
      m_playing = false;
}

/* Audio_Thread::sendStartEventMessage: send start event message to MMDAgent */
void Audio_Thread::sendStartEventMessage(char *str)
{
   ::PostMessage(m_window, m_event, (WPARAM) MMDAgent_strdup(AUDIOTHREAD_EVENTSTART), (LPARAM) MMDAgent_strdup(str));
}

/* Audio_Thread::sendStopEventMessage: send stop event message to MMDAgent */
void Audio_Thread::sendStopEventMessage(char *str)
{
   ::PostMessage(m_window, m_event, (WPARAM) MMDAgent_strdup(AUDIOTHREAD_EVENTSTOP), (LPARAM) MMDAgent_strdup(str));
}
