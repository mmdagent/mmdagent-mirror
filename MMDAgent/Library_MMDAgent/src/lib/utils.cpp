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

#include "MMDAgent.h"
#include "utils.h"

LogText g_logger;

/* getDirectory: get directory from file path */
bool getDirectory(const char *file, char *dir)
{

   int i, len;
   bool found = false;
   char ch;

   if (file == NULL)
      return false;
   len = strlen(file);
   if (len <= 0)
      return false;

   for (i = len; i >= 0; i--) {
      ch = file[i];
      if (found == true) {
         dir[i] = ch;
      } else {
         if (ch == UTILS_DIRSEPARATOR)
            found = true;
         dir[i] = '\0';
      }
   }

   if (dir[0] == '\0')
      strcpy(dir, ".");
   return true;
}

/* strtokWithPattern: strtok with given pattern */
static char *strtokWithPattern(char *str, char *delim, char left_paren, char right_paren, char **strsave)
{
   char *p;
   char *from;
   char c;

   if (str != NULL) {
      *strsave = str;
   }

   /* find start point */
   p = *strsave;
   while (*p != L'\0' && strchr(delim, *p))
      p++;
   if (*p == L'\0')
      return NULL; /* no token left */

   /* copy to ret_buf until end point is found */
   c = *p;
   if (c == left_paren) {
      p++;
      if (*p == '\0') return NULL;
      from = p;
      while ((c = *p) != '\0' &&
             ((c != right_paren) || (*(p + 1) != '\0' && !strchr(delim, *(p + 1)))))
         p++;
      /* if quotation not terminated, allow the rest as one token */
   } else {
      from = p;
      while ((c = *p) != '\0' && (!strchr(delim, c)))
         p++;
   }
   if (*p != '\0') {
      *p = '\0';
      p++;
   }
   *strsave = p;

   return from;
}

/* strtokWithDoubleQuotation: strtok with double quotation */
char *strtokWithDoubleQuotation(char *str, char *delim, char **strsave)
{
   return strtokWithPattern(str, delim, '\"', '\"', strsave);
}

/* hasExtension: check extension */
bool hasExtension(char *file, char *ext)
{
   int i;
   int len1;
   int len2;
   char c1;
   char c2;

   if(file == NULL || ext == NULL)
      return false;

   len1 = strlen(file);
   len2 = strlen(ext);
   if(len1 <= len2)
      return false;

   for(i = 1; i <= len2; i++) {
      c1 = tolower(file[len1 - i]);
      c2 = tolower(ext[len2 - i]);
      if(c1 != c2)
         return false;
   }

   return true;
}

/* getNumDigit: get number of digit */
int getNumDigit(int in)
{
   int out = 0;

   if(in == 0)
      return 1;

   if(in < 0) {
      out = 1;
      in *= -1;
   }
   for (; in != 0; out++)
      in /= 10;

   return out;
}
