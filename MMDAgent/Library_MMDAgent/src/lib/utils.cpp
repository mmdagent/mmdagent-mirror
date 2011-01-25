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

/* MMDAgent_charsize: number of character byte */
static const unsigned char MMDAgent_charsize[] = {
   1, 0x01, 0x7F, /* control and ASCII */
   1, 0xA1, 0xDF, /* 1 byte char */
   2, 0x81, 0x9F, /* 2 byte char */
   2, 0xE0, 0xEF, /* 2 byte char */
   -1, -1, -1
};

/* MMDAgent_getcharsize: get character size */
char MMDAgent_getcharsize(const char *str)
{
   char i, size = -1;

   if(str == NULL || *str == '\0')
      return 0;
   for(i = 0; MMDAgent_charsize[i] > 0; i += 3)
      if((char) MMDAgent_charsize[i+1] <= *str && *str <= (char) MMDAgent_charsize[i+2])
         return MMDAgent_charsize[i];
   return 0;
}

/* MMDAgent_strequal: string matching */
bool MMDAgent_strequal(const char *str1, const char *str2)
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

/* MMDAgent_strtailmatch: match tail string */
bool MMDAgent_strtailmatch(const char *str1, const char *str2)
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

/* MMDAgent_strdup: strdup */
char *MMDAgent_strdup(const char *str)
{
   char *buf;

   if(str == NULL)
      return NULL;
   buf = (char *) malloc(sizeof(char) * (strlen(str) + 1));
   strcpy(buf, str);
   return buf;
}

/* MMDAgent_intdup: integer type strdup */
char *MMDAgent_intdup(const int digit)
{
   int i, size;
   char *p;

   if(digit == 0) {
      size = 2;
   } else {
      if(digit < 0) {
         size = 2;
         i = -digit;
      } else {
         size = 1;
         i = digit;
      }
      for (; i != 0; size++)
         i /= 10;
   }

   p = (char *) malloc(sizeof(char) * size);
   sprintf(p, "%d", digit);
   return p;
}

/* MMDAgent_dirdup: get directory from file path */
char *MMDAgent_dirdup(const char *file)
{
   int i, len;
   char *buf;

   if (file == NULL)
      return NULL;
   len = strlen(file);
   if (len <= 0)
      return NULL;

   for (i = len - 1; i >= 0; i--) {
      if(file[i] == UTILS_DIRSEPARATOR)
         break;
   }
   if(i < 0) return NULL;

   buf = (char *) malloc(sizeof(char) * (i + 1));
   strncpy(buf, file, i);
   buf[i] = '\0';

   return buf;
}

/* MMDAgent_strtok: strtok */
char *MMDAgent_strtok(char *str, const char *pat, char **save)
{
   char *s, *e, *p;
   const char *q;
   char mbc1[UTILS_MAXCHARBYTE];
   char mbc2[UTILS_MAXCHARBYTE];
   int find;
   int step = 0;
   char i, size;

   if(str != NULL)
      p = str;
   else if(save != NULL)
      p = *save;
   else
      return NULL;
   while(*p != '\0') {
      if(step == 0)
         s = p;
      if(step == 1)
         e = p;
      size = MMDAgent_getcharsize(p);
      for(i = 0; i < size; i++) {
         mbc1[i] = *p;
         if(*p == '\0') {
            i = 0;
            break;
         }
         p++;
      }
      mbc1[i] = '\0';
      /* search */
      find = 0;
      q = pat;
      while(*q != '\0') {
         size = MMDAgent_getcharsize(q);
         for(i = 0; i < size; i++) {
            mbc2[i] = *q;
            if(*q == '\0') {
               i = 0;
               break;
            }
            q++;
         }
         mbc2[i] = '\0';
         if(strcmp(mbc1, mbc2) == 0) {
            find = 1;
            break;
         }
      }
      /* check */
      if(step == 0) {
         if(find == 0)
            step = 1;
      } else {
         if(find == 1) {
            *e = '\0';
            *save = p;
            return s;
         } else if(*p == '\0') {
            *save = p;
            return s;
         }
      }
   }

   *save = p;
   return NULL;
}

/* MMDAgent_str2bool: convert string to boolean */
bool MMDAgent_str2bool(const char *str)
{
   if(str == NULL)
      return false;
   else if(strcmp(str, "true") == 0)
      return true;
   else
      return false;
}

/* MMDAgent_str2int: convert string to integer */
int MMDAgent_str2int(const char *str)
{
   if(str == NULL)
      return 0;
   return atoi(str);
}

/* MMDAgent_str2float: convert string to float */
float MMDAgent_str2float(const char *str)
{
   if(str == NULL)
      return 0.0f;
   return (float) atof(str);
}

/* MMDAgent_str2ivec: convert string to integer vector */
bool MMDAgent_str2ivec(const char *str, int *vec, const int size)
{
   int i = 0;
   char *buff, *p, *save;

   if(str == NULL)
      return false;
   buff = MMDAgent_strdup(str);
   for(p = MMDAgent_strtok(buff, ",", &save); p && i < size; p = MMDAgent_strtok(NULL, ",", &save))
      vec[i++] = atoi(p);
   free(buff);
   if(i == size)
      return true;
   else
      return false;
}

/* MMDAgent_str2fvec: convert string to float vector */
bool MMDAgent_str2fvec(const char *str, float *vec, const int size)
{
   int i = 0;
   char *buff, *p, *save;

   if(str == NULL)
      return false;
   buff = MMDAgent_strdup(str);
   for(p = MMDAgent_strtok(buff, ",", &save); p && i < size; p = MMDAgent_strtok(NULL, ",", &save))
      vec[i++] = (float) atof(p);
   free(buff);
   if(i == size)
      return true;
   else
      return false;
}

/* MMDAgent_str2pos: get position from string */
bool MMDAgent_str2pos(const char *str, btVector3 *pos)
{
   float vec[3];

   if (MMDAgent_str2fvec(str, vec, 3) == false)
      return false;

   pos->setValue(vec[0], vec[1], vec[2]);

   return true;
}

/* MMDAgent_str2rot: get rotation from string */
bool MMDAgent_str2rot(const char *str, btQuaternion *rot)
{
   float vec[3];

   if (MMDAgent_str2fvec(str, vec, 3) == false)
      return false;

   rot->setEulerZYX(MMDFILES_RAD(vec[2]), MMDFILES_RAD(vec[1]), MMDFILES_RAD(vec[0]));

   return true;
}

/* MMDAgent_fgettoken: get token from file pointer */
int MMDAgent_fgettoken(FILE *fp, char *buff)
{
   int i;
   char c;

   c = fgetc(fp);
   if(c == EOF) {
      buff[0] = '\0';
      return 0;
   }

   if(c == '#') {
      for(c = fgetc(fp); c != EOF; c = fgetc(fp))
         if(c == '\n')
            return MMDAgent_fgettoken(fp, buff);
      buff[0] = '\0';
      return 0;
   }

   if(c == ' ' || c == '\t' || c == '\r' || c == '\n')
      return MMDAgent_fgettoken(fp, buff);

   buff[0] = c;
   for(i = 1, c = fgetc(fp); c != EOF && c != '#' && c != ' ' && c != '\t' && c != '\r' && c != '\n'; c = fgetc(fp))
      buff[i++] = c;
   buff[i] = '\0';

   if(c == '#')
      fseek(fp, -1, SEEK_CUR);
   if(c == EOF)
      fseek(fp, 0, SEEK_END);

   return i;
}
