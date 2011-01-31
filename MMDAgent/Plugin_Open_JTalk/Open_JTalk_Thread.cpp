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

#include <locale.h>
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

/* main_thread: main thread */
static unsigned __stdcall main_thread(void *param)
{
   Open_JTalk_Thread *open_jtalk_thread = (Open_JTalk_Thread *) param;

   open_jtalk_thread->start();

   _endthreadex(0);
   return 0;
}

/* Open_JTalk_Thread::initialize: initialize thread */
void Open_JTalk_Thread::initialize()
{
   m_window = 0;
   m_event = 0;
   m_command = 0;

   m_threadHandle = 0;
   m_bufferMutex = 0;
   m_synthEvent = 0;

   m_speaking = false;
   m_kill = false;

   m_charaBuff = NULL;
   m_styleBuff = NULL;
   m_textBuff = NULL;

   m_numModels = 0;
   m_modelNames = NULL;
   m_numStyles = 0;
   m_styleNames = NULL;
}

/* Open_JTalk_Thread::clear: free thread */
void Open_JTalk_Thread::clear()
{
   int i;

   stop();
   m_kill = true;

   /* stop to wait */
   if (m_synthEvent)
      SetEvent(m_synthEvent);

   if(m_threadHandle != 0) {
      if (WaitForSingleObject(m_threadHandle, OPENJTALKTHREAD_WAITMS) != WAIT_OBJECT_0)
         MessageBoxA(NULL, "ERROR: Cannot stop Open JTalk thread.", "Error", MB_OK);
      CloseHandle(m_threadHandle);
   }
   if (m_bufferMutex)
      CloseHandle(m_bufferMutex);
   if (m_synthEvent)
      CloseHandle(m_synthEvent);

   if(m_charaBuff) free(m_charaBuff);
   if(m_styleBuff) free(m_styleBuff);
   if(m_textBuff) free(m_textBuff);

   /* free model names */
   if (m_numModels > 0) {
      for (i = 0; i < m_numModels; i++)
         free(m_modelNames[i]);
      free(m_modelNames);
   }

   /* free style names */
   if (m_numStyles > 0) {
      for (i = 0; i < m_numStyles; i++)
         free(m_styleNames[i]);
      free(m_styleNames);
   }

   initialize();
}

/* Open_JTalk_Thread::Open_JTalk_Thread: thread constructor */
Open_JTalk_Thread::Open_JTalk_Thread()
{
   initialize();
}

/* Open_JTalk_Thread::~Open_JTalk_Thread: thread destructor */
Open_JTalk_Thread::~Open_JTalk_Thread()
{
   clear();
}

/* Open_JTalk_Thread::loadAndStart: load models and start thread */
void Open_JTalk_Thread::loadAndStart(HWND window, UINT event, UINT command, char *dicDir, char *config)
{
   int i, j, k;
   char buff[OPENJTALK_MAXBUFLEN];
   FILE *fp;
   bool err = false;

   double *weights;

   /* load config file */
   fp = fopen(config, "r");
   if (fp == NULL)
      return;

   /* number of speakers */
   i = MMDAgent_fgettoken(fp, buff);
   if (i <= 0) {
      MessageBoxA(NULL, "ERROR: Cannot load the number of models in config file of Open JTalk.", "Error", MB_OK);
      fclose(fp);
      clear();
      return;
   }
   m_numModels = MMDAgent_str2int(buff);
   if (m_numModels <= 0) {
      MessageBoxA(NULL, "ERROR: The number of models must be positive value.", "Error", MB_OK);
      fclose(fp);
      clear();
      return;
   }

   /* model directory names */
   m_modelNames = (char **) malloc(sizeof(char *) * m_numModels);
   for (i = 0; i < m_numModels; i++) {
      j = MMDAgent_fgettoken(fp, buff);
      if (j <= 0)
         err = true;
      m_modelNames[i] = MMDAgent_strdup(buff);
   }
   if (err) {
      MessageBoxA(NULL, "ERROR: Cannot load model directory names in config file of Open JTalk.", "Error", MB_OK);
      fclose(fp);
      clear();
      return;
   }

   /* number of speaking styles */
   i = MMDAgent_fgettoken(fp, buff);
   if (i <= 0) {
      MessageBoxA(NULL, "ERROR: Cannot load the number of speaking styles in config file of Open JTalk.", "Error", MB_OK);
      fclose(fp);
      clear();
      return;
   }
   m_numStyles = MMDAgent_str2int(buff);
   if (m_numStyles <= 0) {
      MessageBoxA(NULL, "ERROR: The number of speaking styles must be positive value.", "Error", MB_OK);
      m_numStyles = 0;
      fclose(fp);
      clear();
      return;
   }

   /* style names and weights */
   m_styleNames = (char **) calloc(m_numStyles, sizeof(char *));
   weights = (double *) calloc((3 * m_numModels + 4) * m_numStyles, sizeof(double));
   for (i = 0; i < m_numStyles; i++) {
      j = MMDAgent_fgettoken(fp, buff);
      if(j <= 0)
         err = true;
      m_styleNames[i] = MMDAgent_strdup(buff);
      for (j = 0; j < 3 * m_numModels + 4; j++) {
         k = MMDAgent_fgettoken(fp, buff);
         if (k <= 0)
            err = true;
         weights[(3 * m_numModels + 4) * i + j] = MMDAgent_str2float(buff);
      }
   }
   fclose(fp);
   if(err) {
      MessageBoxA(NULL, "ERROR: Cannot load style definitions in config file of Open JTalk.", "Error", MB_OK);
      free(weights);
      clear();
      return;
   }

   /* load models for TTS */
   if (m_openJTalk.load(dicDir, m_modelNames, m_numModels, weights, m_numStyles) != true) {
      MessageBoxA(NULL, "ERROR: Cannot initialize Open JTalk.", "Error", MB_OK);
      free(weights);
      clear();
      return;
   }
   setlocale(LC_CTYPE, "japanese");

   free(weights);

   m_window = window;
   m_event = event;
   m_command = command;

   /* create mutex */
   m_bufferMutex = CreateMutex(NULL, false, NULL);
   if (m_bufferMutex == 0) {
      MessageBoxA(NULL, "ERROR: Cannot create mutex.", "Error", MB_OK);
      clear();
      return;
   }

   /* create event */
   m_synthEvent = CreateEvent(NULL, true, false, NULL);
   if (m_synthEvent == 0) {
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

/* Open_JTalk_Thread::stopAndRelease: stop thread and free Open JTalk */
void Open_JTalk_Thread::stopAndRelease()
{
   clear();
}

/* Open_JTalk_Thread::start: main thread loop for TTS */
void Open_JTalk_Thread::start()
{
   char lip[OPENJTALK_MAXBUFLEN];
   char *chara, *style, *text;
   int index;

   /* check */
   if(m_threadHandle == 0 || m_bufferMutex == 0 || m_synthEvent == 0)
      return;

   while (m_kill == false) {
      /* wait event */
      if (WaitForSingleObject(m_synthEvent, INFINITE) != WAIT_OBJECT_0) {
         MessageBoxA(NULL, "ERROR: Cannot wait event.", "Error", MB_OK);
         return;
      }
      ResetEvent(m_synthEvent);

      if(m_kill == false) {
         /* wait text */
         if (WaitForSingleObject(m_bufferMutex, INFINITE) != WAIT_OBJECT_0) {
            MessageBoxA(NULL, "ERROR: Cannot wait buffer.", "Error", MB_OK);
            return;
         }
         chara = MMDAgent_strdup(m_charaBuff);
         style = MMDAgent_strdup(m_styleBuff);
         text  = MMDAgent_strdup(m_textBuff);
         ReleaseMutex(m_bufferMutex);

         /* search style index */
         for (index = 0; index < m_numStyles; index++)
            if (MMDAgent_strequal(m_styleNames[index], style))
               break;
         if (index >= m_numStyles) /* unknown style */
            index = 0;

         /* send SYNTH_EVENT_STOP */
         sendStartEventMessage(chara);

         /* synthesize */
         m_openJTalk.setStyle(index);
         m_openJTalk.prepare(text);
         m_openJTalk.getPhonemeSequence(lip);
         if (MMDAgent_strlen(lip) > 0) {
            sendLipCommandMessage(chara, lip);
            m_openJTalk.synthesis();
         }

         /* send SYNTH_EVENT_STOP */
         sendStopEventMessage(chara);

         free(chara);
         free(style);
         free(text);
      }
      m_speaking = false;
   }
}

/* Open_JTalk_Thread::isRunning: check running */
bool Open_JTalk_Thread::isRunning()
{
   if (m_threadHandle == 0 || m_kill == true)
      return false;
   return true;
}

/* Open_JTalk_Thread::isSpeaking: check speaking */
bool Open_JTalk_Thread::isSpeaking()
{
   return m_speaking;
}

/* checkCharacter: check speaking character */
bool Open_JTalk_Thread::checkCharacter(char *chara)
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
   ret = MMDAgent_strequal(m_charaBuff, chara);

   /* release buffer mutex */
   ReleaseMutex(m_bufferMutex);

   return ret;
}

/* Open_JTalk_Thread::synthesis: start synthesis */
void Open_JTalk_Thread::synthesis(char *chara, char *style, char *text)
{
   /* check */
   if(isRunning() == false || m_bufferMutex == 0 || m_synthEvent == 0)
      return;
   if(MMDAgent_strlen(chara) <= 0 || MMDAgent_strlen(style) <= 0 || MMDAgent_strlen(text) <= 0)
      return;

   /* wait buffer mutex */
   if (WaitForSingleObject(m_bufferMutex, INFINITE) != WAIT_OBJECT_0) {
      MessageBoxA(NULL, "ERROR: Cannot wait buffer.", "Error", MB_OK);
      return;
   }

   /* save character name, speaking style, and text */
   if(m_charaBuff) free(m_charaBuff);
   if(m_styleBuff) free(m_styleBuff);
   if(m_textBuff) free(m_textBuff);
   m_charaBuff = MMDAgent_strdup(chara);
   m_styleBuff = MMDAgent_strdup(style);
   m_textBuff = MMDAgent_strdup(text);
   m_speaking = true;

   /* release buffer mutex */
   ReleaseMutex(m_bufferMutex);

   /* start synthesis thread */
   SetEvent(m_synthEvent);
}

/* Open_JTalk_Thread::stop: barge-in function */
void Open_JTalk_Thread::stop()
{
   if(isRunning())
      m_openJTalk.stop();
}

/* Open_JTalk_Thread::sendStartEventMessage: send start event message to MMDAgent */
void Open_JTalk_Thread::sendStartEventMessage(char *str)
{
   ::PostMessage(m_window, m_event, (WPARAM) MMDAgent_strdup(OPENJTALKTHREAD_EVENTSTART), (LPARAM) MMDAgent_strdup(str));
}

/* Open_JTalk_Thread::sendStopEventMessage: send stop event message to MMDAgent */
void Open_JTalk_Thread::sendStopEventMessage(char *str)
{
   ::PostMessage(m_window, m_event, (WPARAM) MMDAgent_strdup(OPENJTALKTHREAD_EVENTSTOP), (LPARAM) MMDAgent_strdup(str));
}

/* Open_JTalk_Thread::sendLipCommandMessage: send lipsync command message to MMDAgent */
void Open_JTalk_Thread::sendLipCommandMessage(char *chara, char *lip)
{
   char *mes1;
   char *mes2;

   if(chara == NULL || lip == NULL) return;

   mes1 = MMDAgent_strdup(OPENJTALKTHREAD_COMMANDLIP);
   mes2 = (char *) malloc(sizeof(char) * (MMDAgent_strlen(chara) + 1 + MMDAgent_strlen(lip) + 1));
   sprintf(mes2, "%s|%s", chara, lip);

   ::PostMessage(m_window, m_command, (WPARAM) mes1, (LPARAM) mes2);
}
