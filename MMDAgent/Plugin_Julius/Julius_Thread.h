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

#define JULIUSTHREAD_MAXBUFLEN 2048

#define JULIUSTHREAD_NAME    "jconf.txt"
#define JULIUSTHREAD_LOCALE  "Japanese_Japan.20932" /* EUC-JP */
#define JULIUSTHREAD_LATENCY 50
#define JULIUSTHREAD_WAITMS  100000                 /* 100 sec */

#define JULIUSTHREAD_EVENTSTART "RECOG_EVENT_START"
#define JULIUSTHREAD_EVENTSTOP  "RECOG_EVENT_STOP"

/* Julius_Thead: thread for Julius */
class Julius_Thread
{
private :

   Jconf *m_jconf; /* configuration parameter data */
   Recog *m_recog; /* engine instance */

   HWND m_window; /* window handle to post message */
   UINT m_event;  /* message type to post message */

   HANDLE m_threadHandle; /* thread handle */

   /* initialize: initialize thread */
   void initialize();

   /* clear: free thread */
   void clear();

public :

   /* Julius_Thread: thread constructor */
   Julius_Thread();

   /* ~Julius_Thread: thread destructor  */
   ~Julius_Thread();

   /* loadAndStart: load models and start thread */
   bool loadAndStart(HWND param1, UINT param2);

   /* stopAndRlease: stop thread and release julius */
   void stopAndRelease();

   /* sendMessage: send message to MMDAgent */
   void sendMessage(char *str1, char *str2);
};
