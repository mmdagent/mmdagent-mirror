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

#include "MMDFiles.h"

/* MMDFiles_charsize: number of character byte */
static const unsigned char MMDFiles_charsize[] = {
   1, 0x01, 0x7F, /* control and ASCII */
   1, 0xA1, 0xDF, /* 1 byte char */
   2, 0x81, 0x9F, /* 2 byte char */
   2, 0xE0, 0xEF, /* 2 byte char */
   -1, -1, -1
};

/* MMDFiles_getcharsize: get character size */
unsigned char MMDFiles_getcharsize(const char *str)
{
   unsigned char i;

   if(str == NULL || *str == '\0')
      return 0;
   for(i = 0; MMDFiles_charsize[i] > 0; i += 3)
      if((char) MMDFiles_charsize[i+1] <= *str && *str <= (char) MMDFiles_charsize[i+2])
         return MMDFiles_charsize[i];
   return 0;
}

/* MMDFiles_strequal: string matching */
bool MMDFiles_strequal(const char *str1, const char *str2)
{
   if(str1 == NULL || str2 == NULL)
      return false;
   else if(str1 == str2)
      return true;
   else if(strcmp(str1, str2) == 0)
      return true;
   else
      return false;
}

/* MMDFiles_strtailmatch: match tail string */
bool MMDFiles_strtailmatch(const char *str1, const char *str2)
{
   int len1, len2;

   if(str1 == NULL || str2 == NULL)
      return false;
   if(str1 == str2)
      return true;
   len1 = strlen(str1);
   len2 = strlen(str2);
   if(len1 < len2)
      return false;
   if(strcmp(&str1[len1-len2], str2) == 0)
      return true;
   else
      return false;
}

/* MMDFiles_strlen: strlen */
int MMDFiles_strlen(const char *str)
{
   if(str == NULL)
      return 0;
   else
      return strlen(str);
}

/* MMDFiles_strdup: strdup */
char *MMDFiles_strdup(const char *str)
{
   char *buf;

   if(str == NULL)
      return NULL;
   buf = (char *) malloc(sizeof(char) * (strlen(str) + 1));
   strcpy(buf, str);

   return buf;
}

/* MMDFiles_dirdup: get directory from file path */
char *MMDFiles_dirdup(const char *file)
{
   int i, len, index = -1;
   char size;
   char *dir;

   len = MMDFiles_strlen(file);

   for(i = 0; i < len; i += size) {
      size = MMDFiles_getcharsize(&file[i]);
      if(size == 1 && file[i] == MMDFILES_DIRSEPARATOR)
         index = i;
   }

   if(index >= 0) {
      dir = (char *) malloc(sizeof(char *) * (index + 1));
      strncpy(dir, file, index);
      dir[index] = '\0';
   } else {
      dir = MMDFiles_strdup(".");
   }

   return dir;
}

/* MMDFiles_getfsize: get file size */
size_t MMDFiles_getfsize(const char *file)
{
   FILE *fp;
   fpos_t size;

   fp = fopen(file, "rb");
   if (!fp)
      return 0;

   fseek(fp, 0, SEEK_END);
   fgetpos(fp, &size);
   fseek(fp, 0, SEEK_SET);
   fclose(fp);

   return (size_t) size;
}
