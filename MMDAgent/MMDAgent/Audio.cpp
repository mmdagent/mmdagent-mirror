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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "TextRenderer.h"
#include "Audio.h"
#include "utils.h"

/* Audio::initialize: initialize audio */
void Audio::initialize()
{
   int i;

   m_window = 0;
   m_needToClose = false;

   for (i = 0; i < AUDIO_MAXNSOUND; i++) {
      m_fileName[i][0] = L'\0';
      m_id[i] = -1;
   }
   m_current = 0;
}

/* Audio::clear: free audio */
void Audio::clear()
{
   if (m_needToClose)
      mciSendString(L"close all", NULL, 0, NULL);
   initialize();
}

/* Audio::Audio: constructor */
Audio::Audio()
{
   initialize();
}

/* Audio::~Audio: destructor */
Audio::~Audio()
{
   clear();
}

/* Audio::setup: initialize audio with window handle */
void Audio::setup(HWND hWnd)
{
   m_window = hWnd;
}

/* Audio::play: play audio */
bool Audio::play(char *file, char *alias)
{
   int i, j;
   wchar_t buf[AUDIO_MAXBUFLEN];

   /* wait */
   size_t len;
   wchar_t fileWcs[AUDIO_MAXBUFLEN];
   wchar_t aliasWcs[AUDIO_MAXBUFLEN];
   mbstowcs_s(&len, fileWcs, AUDIO_MAXBUFLEN, file, _TRUNCATE);
   mbstowcs_s(&len, aliasWcs, AUDIO_MAXBUFLEN, alias, _TRUNCATE);
   _snwprintf(buf, AUDIO_MAXBUFLEN, L"open \"%s\" alias %s wait", fileWcs, aliasWcs);
   if (mciSendString(buf, NULL, 0, NULL) != 0) {
      g_logger.log("! Error: Audio: failed to open \"%s\" as alias \"%s\"", file, alias);
      return false;
   }

   /* check */
   for (i = 0, j = -1; i < AUDIO_MAXNSOUND; i++) {
      if (m_fileName[i][0] == L'\0' && j == -1)
         j = i;
      if (wcsncmp(m_fileName[i], aliasWcs, AUDIO_MAXBUFLEN) == 0) {
         m_id[i] = mciGetDeviceID(aliasWcs);
         break;
      }
   }
   if (i >= AUDIO_MAXNSOUND) {
      if (j == -1) {
         g_logger.log("! Error: Audio: too many musics at a time! (%d)", AUDIO_MAXNSOUND);
         close(alias);
         return false;
      }
      m_id[j] = mciGetDeviceID(aliasWcs);
      wcsncpy(m_fileName[j], aliasWcs, AUDIO_MAXBUFLEN);
   }

   /* enqueue */
   _snwprintf(buf, AUDIO_MAXBUFLEN, L"cue %s output wait", aliasWcs);
   if (mciSendString(buf, NULL, 0, NULL) != 0) {
      g_logger.log("! Error: Audio: failed to cue \"%s\" in mci", alias);
      return false;
   }

   /* start */
   _snwprintf(buf, AUDIO_MAXBUFLEN, L"play %s notify", aliasWcs);
   if (mciSendString(buf, NULL, 0, m_window) != 0) {
      g_logger.log("! Error: Audio: failed to start playing \"%s\" as alias \"%s\"", file, alias);
      return false;
   }
   m_needToClose = true; /* set flag to close */

   return true;
}

/* Audio::close: close audio */
void Audio::close(char *alias)
{
   int i;
   wchar_t buf[AUDIO_MAXBUFLEN];

   /* send close command */
   wchar_t wcsbuf[AUDIO_MAXBUFLEN];
   size_t len;
   mbstowcs_s(&len, wcsbuf, AUDIO_MAXBUFLEN, alias, _TRUNCATE);
   _snwprintf(buf, AUDIO_MAXBUFLEN, L"close %s", wcsbuf);
   mciSendString(buf, NULL, 0, NULL);

   /* remove file name */
   for (i = 0; i < AUDIO_MAXNSOUND; i++) {
      if (wcsncmp(m_fileName[i], wcsbuf, AUDIO_MAXBUFLEN) == 0) {
         m_fileName[i][0] = L'\0';
         break;
      }
   }
}

/* Audio::getFinishedAlias: return finished audio alias */
wchar_t *Audio::getFinishedAlias(WPARAM wParam, LPARAM lParam)
{
   int i;

   switch (wParam) {
   case MCI_NOTIFY_SUCCESSFUL: /* end of playing */
      for (i = 0; i < AUDIO_MAXNSOUND; i++) {
         if (lParam == m_id[i])
            return m_fileName[i];
      }
      break;
   }
   return NULL;
}
