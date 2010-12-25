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

#include "TextRenderer.h"
#include "utils.h"

/* getDirName: get directory name from file name */
void getDirName(wchar_t *wszDir, const wchar_t *wszPath)
{
   int i;
   wchar_t ch;
   bool found = FALSE;

   wszDir[0] = L'\0';
   for (i = wcslen(wszPath); i >= 0; i--) {
      ch = wszPath[i];
      if (found) {
         wszDir[i] = wszPath[i];
      } else if (ch == L'\\' || ch == L'/' || ch == L':') {
         found = TRUE;
         wszDir[i+1] = L'\0';
         wszDir[i] = wszPath[i];
      }
   }
}

/* getFullDirName: get directory name from file name (full path) */
void getFullDirName(wchar_t *wszDir, const wchar_t *wszPath)
{
   wchar_t fbuf[MAX_PATH];

   if (wcschr(wszPath, L':')) {
      getDirName(wszDir, wszPath);
   } else {
      GetCurrentDirectory(MAX_PATH, fbuf);
      wcsncat_s(fbuf, MAX_PATH, L"\\", _TRUNCATE);
      wcsncat_s(fbuf, MAX_PATH, wszPath, _TRUNCATE);
      getDirName(wszDir, fbuf);
   }
}

/* wcstokWithPattern: wcstok with given pattern */
static wchar_t *wcstokWithPattern(wchar_t *str, wchar_t *delim, wchar_t left_paren, wchar_t right_paren, int mode, wchar_t **strsave)
{
   wchar_t *p;
   wchar_t *from;
   wchar_t c;

   if (str != NULL) {
      *strsave = str;
   }

   /* find start point */
   p = *strsave;
   while (*p != L'\0' && wcschr(delim, *p)) p++;
   if (*p == L'\0') return NULL; /* no token left */

   /* if mode == 1, exit here */
   if (mode == 1) {
      *strsave = p;
      return p;
   }

   /* copy to ret_buf until end point is found */
   c = *p;
   if (c == left_paren) {
      p++;
      if (*p == L'\0') return NULL;
      from = p;
      while ((c = *p) != L'\0' &&
             ((c != right_paren) || (*(p + 1) != L'\0' && !wcschr(delim, *(p + 1))))) p++;
      /* if quotation not terminated, allow the rest as one token */
   } else {
      from = p;
      while ((c = *p) != L'\0' && (!wcschr(delim, c))) p++;
   }
   if (*p != '\0') {
      *p = '\0';
      p++;
   }
   *strsave = p;

   return from;
}

/* wcstokWithDoubleQuotation: wcstok with double quotation */
wchar_t *wcstokWithDoubleQuotation(wchar_t *str, wchar_t *delim, wchar_t **strsave)
{
   return wcstokWithPattern(str, delim, L'\"', L'\"', 0, strsave);
}

/* hasSuffix: check suffix of a file */
bool hasSuffix(wchar_t *fileName, wchar_t *suffix)
{
   size_t len1 = wcslen(fileName);
   size_t len2 = wcslen(suffix);
   size_t pos = len2;

   while (pos > 0) {
      if (fileName[len1 - pos] != suffix[len2 - pos] && fileName[len1 - pos] != towupper(suffix[len2 - pos]))
         break;
      pos--;
   }
   if (pos == 0)
      return true;

   return false;
}

/* getNumDigit: get number of digit */
int getNumDigit(int in)
{
   int out;

   for (out = 0; in != 0; out++)
      in /= 10;

   return out;
}
