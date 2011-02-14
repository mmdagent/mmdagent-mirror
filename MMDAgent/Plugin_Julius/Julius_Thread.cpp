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
#include <locale.h>

#include "MMDAgent.h"
#include "julius/juliuslib.h"
#include "Julius_Logger.h"
#include "Julius_Thread.h"

/* callback_recog_begin: callback for beginning of recognition */
static void callback_recog_begin(Recog *recog, void *data)
{
   Julius_Thread *j = (Julius_Thread *) data;
   j->sendMessage(JULIUSTHREAD_EVENTSTART, NULL);
}

/* callback_result_final: callback for recognitional result */
static void callback_result_final(Recog *recog, void *data)
{
   int i;
   int first;
   Sentence *s;
   RecogProcess *r;
   static char str[JULIUSTHREAD_MAXBUFLEN];
   size_t size = 0;
   Julius_Thread *j = (Julius_Thread *) data;

   _locale_t locale;
   static wchar_t wstr[JULIUSTHREAD_MAXBUFLEN];

   static char sjisbuf[JULIUSTHREAD_MAXBUFLEN];

   /* get status */
   r = recog->process_list;
   if (!r->live)
      return;
   if (r->result.status < 0)
      return;

   s = &(r->result.sent[0]);
   strcpy(str, "");
   first = 1;
   for (i = 0; i < s->word_num; i++) {
      if (MMDAgent_strlen(r->lm->winfo->woutput[s->word[i]]) > 0) {
         if (first == 0)
            strcat(str, ",");
         strncat(str, r->lm->winfo->woutput[s->word[i]], JULIUSTHREAD_MAXBUFLEN);
         if (first == 1)
            first = 0;
      }
   }

   if (first == 0) {
      /* euc-jp -> wide char */
      locale = _create_locale(LC_CTYPE, JULIUSTHREAD_LOCALE);
      if (locale)
         _mbstowcs_s_l(&size, wstr, JULIUSTHREAD_MAXBUFLEN, str, _TRUNCATE, locale);
      else
         mbstowcs_s(&size, wstr, JULIUSTHREAD_MAXBUFLEN, str, _TRUNCATE);

      /* wide char -> sjis */
      wcstombs_s(&size, sjisbuf, JULIUSTHREAD_MAXBUFLEN, wstr, _TRUNCATE);

      j->sendMessage(JULIUSTHREAD_EVENTSTOP, sjisbuf);
   }
}

/* main_thread: main thread */
static unsigned __stdcall main_thread(void *param)
{
   Julius_Thread *julius_thread = (Julius_Thread *) param;

   julius_thread->start();

   _endthreadex(0);
   return 0;
}

/* Julius_Thread::initialize: initialize thread */
void Julius_Thread::initialize()
{
   m_jconf = NULL;
   m_recog = NULL;

   m_window = 0;
   m_event = 0;

   m_threadHandle = 0;

   m_languageModel = NULL;
   m_dictionary = NULL;
   m_acousticModel = NULL;
   m_triphoneList = NULL;
   m_configFile = NULL;
   m_userDictionary = NULL;
}

/* Julius_Thread::clear: free thread */
void Julius_Thread::clear()
{
   if(m_threadHandle != 0) {
      if(m_recog)
         j_close_stream(m_recog);
      if(WaitForSingleObject(m_threadHandle, JULIUSTHREAD_WAITMS) != WAIT_OBJECT_0)
         MessageBoxA(NULL, "Error: cannot stop Julius thread.", "Error", MB_OK);
      CloseHandle(m_threadHandle);
   }
   if (m_recog) {
      j_recog_free(m_recog); /* jconf is also released in j_recog_free */
   } else if (m_jconf) {
      j_jconf_free(m_jconf);
   }

   if(m_languageModel != NULL)
      free(m_languageModel);
   if(m_dictionary != NULL)
      free(m_dictionary);
   if(m_acousticModel != NULL)
      free(m_acousticModel);
   if(m_triphoneList != NULL)
      free(m_triphoneList);
   if(m_configFile != NULL)
      free(m_configFile);
   if(m_userDictionary != NULL)
      free(m_userDictionary);

   initialize();
}

/* Julius_Thread::Julius_Thread: thread constructor */
Julius_Thread::Julius_Thread()
{
   initialize();
}

/* Julius_Thread::~Julius_Thread: thread destructor */
Julius_Thread::~Julius_Thread()
{
   clear();
}

/* Julius_Thread::loadAndStart: load models and start thread */
void Julius_Thread::loadAndStart(HWND window, UINT event, const char *languageModel, const char *dictionary, const char *acousticModel, const char *triphoneList, const char *configFile, const char *userDictionary)
{
   /* reset */
   clear();

   m_window = window;
   m_event = event;

   m_languageModel = MMDAgent_strdup(languageModel);
   m_dictionary = MMDAgent_strdup(dictionary);
   m_acousticModel = MMDAgent_strdup(acousticModel);
   m_triphoneList = MMDAgent_strdup(triphoneList);
   m_configFile = MMDAgent_strdup(configFile);
   m_userDictionary = MMDAgent_strdup(userDictionary);

   if(m_window == 0 || m_event == 0 || m_languageModel == NULL || m_dictionary == NULL || m_acousticModel == NULL || m_triphoneList == NULL || m_configFile == NULL) {
      clear();
      return;
   }

   /* create recognition thread */
   m_threadHandle = (HANDLE) _beginthreadex(NULL, 0, main_thread, this, 0, NULL);
   if (m_threadHandle == 0) {
      MessageBoxA(NULL, "Error: cannot start Julius thread.", "Error", MB_OK);
      clear();
   }
}

/* Julius_Thread::stopAndRelease: stop thread and release julius */
void Julius_Thread::stopAndRelease()
{
   clear();
}

/* Julius_Thread::start: main loop */
void Julius_Thread::start()
{
   char buff[JULIUSTHREAD_MAXBUFLEN];
   FILE *fp;

   if(m_jconf != NULL || m_recog != NULL || m_window == 0 || m_event == 0 || m_threadHandle == 0 || m_languageModel == 0 || m_dictionary == 0 || m_acousticModel == 0 || m_triphoneList == 0 || m_configFile == 0) return;

   /* set latency */
   _snprintf(buff, JULIUSTHREAD_MAXBUFLEN - 1, "%d", JULIUSTHREAD_LATENCY);
   _putenv_s("PA_MIN_LATENCY_MSEC", buff);
   _putenv_s("LATENCY_MSEC", buff);

   /* load models */
   sprintf(buff, "-d \"%s\"", m_languageModel);
   m_jconf = j_config_load_string_new(buff);
   if (m_jconf == NULL) {
      MessageBoxA(NULL, "Error: cannot load language model for Julius.", "Error", MB_OK);
      return;
   }
   sprintf(buff, "-v \"%s\"", m_dictionary);
   if(j_config_load_string(m_jconf, buff) < 0) {
      MessageBoxA(NULL, "Error: cannot load system dictionary for Julius.", "Error", MB_OK);
      return;
   }
   sprintf(buff, "-h \"%s\"", m_acousticModel);
   if(j_config_load_string(m_jconf, buff) < 0) {
      MessageBoxA(NULL, "Error: cannot load acoustic model for Julius.", "Error", MB_OK);
      return;
   }
   sprintf(buff, "-hlist \"%s\"", m_triphoneList);
   if(j_config_load_string(m_jconf, buff) < 0) {
      MessageBoxA(NULL, "Error: cannot load triphone list for Julius.", "Error", MB_OK);
      return;
   }

   /* load config file */
   if(j_config_load_file(m_jconf, m_configFile) < 0) {
      MessageBoxA(NULL, "Error: cannot load config file for Julius.", "Error", MB_OK);
      return;
   }

   /* load user dictionary */
   fp = fopen(m_userDictionary, "r");
   if(fp != NULL) {
      fclose(fp);
      j_add_dict(m_jconf->lm_root, m_userDictionary);
   }

   /* create instance */
   m_recog = j_create_instance_from_jconf(m_jconf);
   if (m_recog == NULL) {
      MessageBoxA(NULL, "Error: cannot create Julius instance.", "Error", MB_OK);
      return;
   }

   /* register callback functions */
   callback_add(m_recog, CALLBACK_EVENT_RECOGNITION_BEGIN, callback_recog_begin, this);
   callback_add(m_recog, CALLBACK_RESULT, callback_result_final, this);
   if (!j_adin_init(m_recog)) {
      MessageBoxA(NULL, "Error: cannot create Julius instance.", "Error", MB_OK);
      return;
   }

   if (j_open_stream(m_recog, NULL) != 0) {
      MessageBoxA(NULL, "Error: cannot open recognition stream.", "Error", MB_OK);
      return;
   }

   /* start logger */
   m_logger.start(m_recog);

   /* start recognize */
   j_recognize_stream(m_recog);
}

/* Julius_Thread::pause: pause recognition process */
void Julius_Thread::pause()
{
   if(m_recog != NULL)
      j_request_pause(m_recog);
}

/* Julius_Thread::resume: resume recognition process */
void Julius_Thread::resume()
{
   if(m_recog != NULL)
      j_request_resume(m_recog);
}

/* Julius_Thread::sendMessage: send message to MMDAgent */
void Julius_Thread::sendMessage(const char *str1, const char *str2)
{
   char *mes1, *mes2;

   if(str1 == NULL)
      return;

   mes1 = MMDAgent_strdup(str1);
   if(str2 != NULL)
      mes2 = MMDAgent_strdup(str2);
   else
      mes2 = MMDAgent_strdup("");

   ::PostMessage(m_window, m_event, (WPARAM) mes1, (LPARAM) mes2);
}

/* Julius_Thread::getLogActiveFlag: get active flag of logger */
bool Julius_Thread::getLogActiveFlag()
{
   return m_logger.getActiveFlag();
}

/* Julius_Thread::setLogActiveFlag: set active flag of logger */
void Julius_Thread::setLogActiveFlag(bool b)
{
   m_logger.setActiveFlag(b);
}

/* Julius_Thread::updateLog: update log view per step */
void Julius_Thread::updateLog(double deltaFrame)
{
   m_logger.update(deltaFrame);
}

/* Julius_Thread::renderLog: render log view */
void Julius_Thread::renderLog()
{
   m_logger.render();
}
