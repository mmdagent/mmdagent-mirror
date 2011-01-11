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

#include <process.h>
#include <locale.h>

#include "julius/juliuslib.h"
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
   WORD_ID *word;
   int word_num;
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
   word = s->word;
   word_num = s->word_num;
   strcpy(str, "");
   first = 1;
   for (i = 0; i < word_num; i++) {
      if (strlen(r->lm->winfo->woutput[word[i]]) > 0) {
         if (first == 0)
            strcat(str, ",");
         strncat(str, r->lm->winfo->woutput[word[i]], JULIUSTHREAD_MAXBUFLEN);
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
unsigned __stdcall main_thread(void *param)
{
   Recog *recog;
   int ret;

   recog = (Recog *) param;
   ret = j_recognize_stream(recog);
   _endthreadex(ret);
   return(ret);
}

/* Julius_Thread::initialize: initialize thread */
void Julius_Thread::initialize()
{
   m_jconf = NULL;
   m_recog = NULL;

   m_window = NULL;
   m_event = NULL;

   m_opened = false;
   m_threadHandle = NULL;
}

/* Julius_Thread::clear: free thread */
void Julius_Thread::clear()
{
   if (m_recog == NULL)
      return;

   if (m_opened == true)
      j_close_stream(m_recog);
   if (m_threadHandle != NULL)
      CloseHandle(m_threadHandle);
   if (m_recog)
      j_recog_free(m_recog);
   if (m_jconf)
      j_jconf_free(m_jconf);

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
bool Julius_Thread::loadAndStart(HWND param1, UINT param2)
{
   _locale_t model_locale;
   char buff[JULIUSTHREAD_MAXBUFLEN];

   /* reset */
   clear();

   /* set locale */
   model_locale = _create_locale(LC_CTYPE, JULIUSTHREAD_LOCALE);

   /* set latency */
   _snprintf(buff, JULIUSTHREAD_MAXBUFLEN - 1, "%d", JULIUSTHREAD_LATENCY);
   _putenv_s("PA_MIN_LATENCY_MSEC", buff);
   _putenv_s("LATENCY_MSEC", buff);

   /* load config file */
   m_jconf = j_config_load_file_new(JULIUSTHREAD_NAME);
   if (m_jconf == NULL)
      return false;

   /* create instance */
   m_recog = j_create_instance_from_jconf(m_jconf);
   if (m_recog == NULL) {
      MessageBox(NULL, L"ERROR: Cannot create Julius instance.", L"Error", MB_OK);
      return false;
   }

   /* register callback functions */
   callback_add(m_recog, CALLBACK_EVENT_RECOGNITION_BEGIN, ::callback_recog_begin, this);
   callback_add(m_recog, CALLBACK_RESULT, ::callback_result_final, this);
   if (!j_adin_init(m_recog))
      return false;

   if (j_open_stream(m_recog, NULL) != 0) {
      MessageBox(NULL, L"ERROR: Cannot open recognition stream.", L"Error", MB_OK);
      return false;
   }

   m_window = param1;
   m_event = param2;

   /* create recognition thread */
   m_threadHandle = (HANDLE) _beginthreadex(NULL, 0, main_thread, m_recog, 0, NULL);
   if (m_threadHandle == 0) {
      j_close_stream(m_recog);
      MessageBox(NULL, L"ERROR: Cannot start Julius thread.", L"Error", MB_OK);
      return false;
   }

   m_opened = true;
   return true;
}

/* Julius_Thread::sendMessage: send message to MMDAgent */
void Julius_Thread::sendMessage(char *str1, char *str2)
{
   char *mes1, *mes2;

   if(str1 == NULL)
      return;

   mes1 = strdup(str1);
   if(str2 != NULL)
      mes2 = strdup(str2);
   else
      mes2 = strdup("");

   ::PostMessage(m_window, m_event, (WPARAM) mes1, (LPARAM) mes2);
}
