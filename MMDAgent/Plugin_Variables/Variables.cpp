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

#include <time.h>

#include "MMDAgent.h"
#include "Variables.h"

/* Variables::sendEventMessage: send event message to MMDAgent */
void Variables::sendEventMessage(const char *mes1, const char *mes2)
{
   if(mes1 == NULL || mes2 == NULL)
      return;

   ::PostMessage(m_window, m_event, (WPARAM) MMDAgent_strdup(mes1), (LPARAM) MMDAgent_strdup(mes2));
}

/* Variables::initialize: initialize */
void Variables::initialize()
{
   m_head = NULL;
   m_tail = NULL;

   m_window = 0;
   m_event = 0;

   srand((unsigned) time(NULL));
}

/* Variables::clear: free */
void Variables::clear()
{
   Value *tmp1, *tmp2;

   for(tmp1 = m_head; tmp1 ; tmp1 = tmp2) {
      tmp2 = tmp1->next;
      free(tmp1->name);
      free(tmp1);
   }

   initialize();
}

/* Variables::Variables: thread constructor */
Variables::Variables()
{
   initialize();
}

/* Variables::~Variables: thread destructor */
Variables::~Variables()
{
   clear();
}

/* Variables::setup: setup variables */
void Variables::setup(HWND hWnd, UINT event)
{
   if(hWnd == 0 || event == 0)
      return;

   m_window = hWnd;
   m_event = event;
}

/* Variables::set: set value */
void Variables::set(const char *alias, const char *str)
{
   Value *val;

   char *buff, *p, *q, *save;
   float max, min, tmp;

   if(MMDAgent_strlen(alias) <= 0) return;

   /* check the same alias */
   for(val = m_head; val; val = val->next) {
      if(MMDAgent_strequal(val->name, alias))
         break;
   }

   /* push */
   if(val == NULL) {
      val = (Value *) malloc(sizeof(Value));
      val->name = MMDAgent_strdup(alias);
      val->next = NULL;
      if(m_tail == NULL) {
         m_head = val;
         val->prev = NULL;
      } else {
         m_tail->next = val;
         val->prev = m_tail;
      }
      m_tail = val;
   }

   /* set value */
   buff = MMDAgent_strdup(str);
   p = MMDAgent_strtok(buff, ",", &save);
   q = MMDAgent_strtok(NULL, ",", &save);
   if(q == NULL) {
      val->val = MMDAgent_str2float(str);
   } else {
      /* set random value */
      min = MMDAgent_str2float(q);
      max = MMDAgent_str2float(p);
      if(max < min) {
         tmp = max;
         max = min;
         min = tmp;
      }
      val->val = min + (max - min) * (rand() - 0.0f) * (1.0f / (RAND_MAX - 0.0f)); /* 0.0f is RAND_MIN */
   }
   if(buff)
      free(buff);

   sendEventMessage(VARIABLES_VALUESETEVENT, alias); /* send message */
}

/* Variables::unset: unset */
void Variables::unset(const char *alias)
{
   Value *tmp1, *tmp2;

   for(tmp1 = m_head; tmp1; tmp1 = tmp2) {
      tmp2 = tmp1->next;
      if(MMDAgent_strequal(tmp1->name, alias)) {
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
               m_tail = tmp1->prev;
               tmp1->prev->next = NULL;
            } else {
               tmp1->next->prev = tmp1->prev;
               tmp1->prev->next = tmp1->prev;
            }
         }
         sendEventMessage(VARIABLES_VALUEUNSETEVENT, tmp1->name); /* send message */
         free(tmp1->name);
         free(tmp1);
         break;
      }
   }
}

/* Variables::evaluate: evaluate value */
void Variables::evaluate(const char *alias, const char *mode, const char *str)
{
   Value *val;
   float f1, f2;
   char buff[VARIABLES_MAXBUFLEN];
   bool ret;

   /* get value 1 */
   for(val = m_head; val; val = val->next) {
      if(MMDAgent_strequal(val->name, alias) == true) {
         f1 = val->val;
         break;
      }
   }
   if(val == NULL)
      return;

   /* get value 2 */
   f2 = MMDAgent_str2float(str);

   /* evaluate */
   if(MMDAgent_strequal(mode, VARIABLES_EQ) == true) {
      if(f1 == f2)
         ret = true;
      else
         ret = false;
   } else if(MMDAgent_strequal(mode, VARIABLES_NE) == true) {
      if(f1 != f2)
         ret = true;
      else
         ret = false;
   } else if(MMDAgent_strequal(mode, VARIABLES_LE) == true) {
      if(f1 <= f2)
         ret = true;
      else
         ret = false;
   } else if(MMDAgent_strequal(mode, VARIABLES_LT) == true) {
      if(f1 < f2)
         ret = true;
      else
         ret = false;
   } else if(MMDAgent_strequal(mode, VARIABLES_GE) == true) {
      if(f1 >= f2)
         ret = true;
      else
         ret = false;
   } else if(MMDAgent_strequal(mode, VARIABLES_GT) == true) {
      if(f1 > f2)
         ret = true;
      else
         ret = false;
   } else {
      /* unknown mode */
      return;
   }

   if(ret == true)
      sprintf(buff, "%s|%s|%s|%s", alias, mode, str, VARIABLES_TRUE);
   else
      sprintf(buff, "%s|%s|%s|%s", alias, mode, str, VARIABLES_FALSE);

   sendEventMessage(VARIABLES_VALUEEVALEVENT, buff);
}
