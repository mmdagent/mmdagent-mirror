/* ----------------------------------------------------------------- */
/*           The Toolkit for Building Voice Interaction Systems      */
/*           "MMDAgent" developed by MMDAgent Project Team           */
/*           http://www.mmdagent.jp/                                 */
/* ----------------------------------------------------------------- */
/*                                                                   */
/*  Copyright (c) 2009-2015  Nagoya Institute of Technology          */
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

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif /* __APPLE__ */

#if !defined(_WIN32) && !defined(__APPLE__) && !defined(__ANDROID__)
#include <iconv.h>
#include <unistd.h>
#endif /* !_WIN32 && !__APPLE__ && !__ANDROID__ */

#ifdef _WIN32
#include <windows.h>
#endif /* _WIN32 */

#ifdef __ANDROID__
#include <stdio.h>
#include <sys/sysconf.h>
#include "UTF8Table.h"
#endif /* __ANDROID__ */

#include "MMDFiles.h"

/* MMDFiles_charsize: number of character byte */
static const unsigned char MMDFiles_charsize[] = {
   1, 0x01, 0x7F,
   2, 0xC2, 0xDF,
   3, 0xE0, 0xEF,
   4, 0xF0, 0xF7,
   5, 0xF8, 0xFB,
   6, 0xFC, 0xFD,
   0, 0, 0
};

/* MMDFiles_getcharsize: get character size */
unsigned char MMDFiles_getcharsize(const char *str)
{
   unsigned char i;

   if(str == NULL || *str == '\0')
      return 0;
   for(i = 0; MMDFiles_charsize[i] > 0; i += 3)
      if((char) MMDFiles_charsize[i + 1] <= *str && *str <= (char) MMDFiles_charsize[i + 2])
         return MMDFiles_charsize[i];
   return 0;
}

/* MMDFiles_dirseparator: check directory separator */
bool MMDFiles_dirseparator(char c)
{
   int i;
   const char list[] = {MMDFILESUTILS_DIRSEPARATORS, 0};

   for(i = 0; list[i] != 0; i++) {
      if(c == list[i])
         return true;
   }

   return false;
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

/* MMDFiles_strheadmatch: match head string */
bool MMDFiles_strheadmatch(const char *str1, const char *str2)
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
   if(strncmp(str1, str2, len2) == 0)
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
   if(strcmp(&str1[len1 - len2], str2) == 0)
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

#if defined(_WIN32)
static char *MMDFiles_strdup_with_conversion(const char *str, UINT from, UINT to)
{
   int result;
   size_t size;
   char *buff;
   int wideCharSize;
   WCHAR *wideCharStr;

   if(str == NULL)
      return NULL;

   result = MultiByteToWideChar(from, 0, (LPCSTR) str, -1, NULL, 0);
   if(result <= 0) {
      return NULL;
   }
   wideCharSize = result;

   wideCharStr = (WCHAR *) malloc(sizeof(WCHAR) * (wideCharSize + 1));
   if(wideCharStr == NULL) {
      return NULL;
   }

   result = MultiByteToWideChar( from, 0, (LPCSTR) str, -1, (LPWSTR) wideCharStr, wideCharSize);
   if(result != wideCharSize) {
      free(wideCharStr);
      return NULL;
   }

   result = WideCharToMultiByte(to, 0, (LPCWSTR) wideCharStr, -1, NULL, 0, NULL, NULL );
   if(result <= 0) {
      free(wideCharStr);
      return NULL;
   }
   size = (size_t) result;

   buff = (char *) malloc(sizeof(char) * (size + 1));
   if(buff == NULL) {
      free(wideCharStr);
      return NULL;
   }

   result = WideCharToMultiByte(to, 0, (LPCWSTR) wideCharStr, -1, (LPSTR) buff, size, NULL, NULL);
   if((size_t) result != size) {
      free(wideCharStr);
      free(buff);
      return NULL;
   }

   free(wideCharStr);

   return buff;
}

#elif defined(__APPLE__)
static char *MMDFiles_strdup_with_conversion(const char *str, CFStringEncoding from, CFStringEncoding to)
{
   size_t i, size;
   char *inBuff, *outBuff;
   size_t inLen, outLen;
   CFStringRef cfs;

   if(str == NULL)
      return NULL;

   cfs = CFStringCreateWithCString(NULL, str, from);
   outLen = CFStringGetMaximumSizeForEncoding(CFStringGetLength(cfs), to) + 1;
   outBuff = (char *) malloc(outLen);
   CFStringGetCString(cfs, outBuff, outLen, to);
   CFRelease(cfs);

   return outBuff;
}
#elif defined(__ANDROID__)
static char *MMDFiles_strdup_with_conversion(const char *str, bool systemToPmd)
{
   size_t len;
   const char *p;
   const char **f;
   const char **t;
   const char **from = &UTF8Table[0];
   const char **to = &UTF8Table[1];
   char *buff;

   len = MMDFiles_strlen(str);
   if(len <= 0) {
      return NULL;
   }

   buff = (char *) calloc(len * MMDFILESUTILS_MAXCHARBYTE, sizeof(char));
   if(buff == NULL) {
      return NULL;
   }

   if(!systemToPmd) {
      from = &UTF8Table[1];
      to = &UTF8Table[0];
   }

   for(p = str; *p != '\0';) {
      f = from;
      t = to;
      while(f != NULL) {
         if(MMDFiles_strheadmatch(p, *f)) {
            strcat(buff, *t);
            break;
         }
         f += 2;
         t += 2;
      }
      if(f == NULL) {
         p++;
      } else {
         p += strlen(*f);
      }
   }

   return buff;
}
#else
static char *MMDFiles_strdup_with_conversion(const char *str, const char *from, const char *to)
{
   iconv_t ic;
   char *inBuff, *outBuff;
   char *inFile, *outFile;
   size_t inLen, outLen;

   inLen = MMDFiles_strlen(str);
   if(inLen <= 0)
      return NULL;
   outLen = inLen * MMDFILESUTILS_MAXCHARBYTE;

   ic = iconv_open(to, from);
   if(ic < 0)
      return NULL;

   inBuff = inFile = MMDFiles_strdup(str);
   outBuff = outFile = (char *) calloc(outLen, sizeof(char));

   /* convert muli-byte char */
   if(iconv(ic, &inFile, &inLen, &outFile, &outLen) >= 0) {
      outFile = '\0';
   } else {
      strcpy(outBuff, "");
   }

   iconv_close(ic);

   free(inBuff);
   return outBuff;
}
#endif

/* MMDFiles_strdup_from_sjis_to_utf8: strdup with conversion from sjis to utf8 */
char *MMDFiles_strdup_from_sjis_to_utf8(const char *str)
{
#if defined(_WIN32)
   return MMDFiles_strdup_with_conversion(str, CP_ACP, CP_UTF8);
#elif defined(__APPLE__)
   return MMDFiles_strdup_with_conversion(str, kCFStringEncodingDOSJapanese, kCFStringEncodingUTF8);
#elif defined(__ANDROID__)
   return MMDFiles_strdup_with_conversion(str, false);
#else
   return MMDFiles_strdup_with_conversion(str, "CP932", "UTF8");
#endif /* !_WIN32 && !__APPLE__ && !__ANDROID__ */
}

/* MMDFiles_strdup_from_utf8_to_sjis: strdup with conversion from utf8 to sjis */
char *MMDFiles_strdup_from_utf8_to_sjis(const char *str)
{
#if defined(_WIN32)
   return MMDFiles_strdup_with_conversion(str, CP_UTF8, CP_ACP);
#elif defined(__APPLE__)
   return MMDFiles_strdup_with_conversion(str, kCFStringEncodingUTF8, kCFStringEncodingDOSJapanese);
#elif defined(__ANDROID__)
   return MMDFiles_strdup_with_conversion(str, true);
#else
   return MMDFiles_strdup_with_conversion(str, "UTF8", "CP932");
#endif
}

/* MMDFiles_pathdup_from_application_to_system_locale: convert charset from application to system locale */
char *MMDFiles_pathdup_from_application_to_system_locale(const char *str)
{
   size_t i, size, inLen;
   char *inBuff, *outBuff;

   if(str == NULL)
      return NULL;

   inBuff = MMDFiles_strdup(str);
   if(inBuff == NULL)
      return NULL;
   inLen = strlen(inBuff);

   /* convert directory separator */
   for(i = 0; i < inLen; i += size) {
      size = MMDFiles_getcharsize(&inBuff[i]);
      if(size == 1 && MMDFiles_dirseparator(inBuff[i]) == true)
         inBuff[i] = MMDFILESUTILS_SYSTEMDIRSEPARATOR;
   }

   outBuff = MMDFiles_strdup_from_application_to_system_locale(inBuff);
   free(inBuff);
   return outBuff;
}

/* MMDFiles_pathdup_from_system_locale_to_application: convert path charset from system locale to application */
char *MMDFiles_pathdup_from_system_locale_to_application(const char *str)
{
   size_t i, size, outLen;
   char *outBuff;

   if(str == NULL)
      return NULL;

   outBuff = MMDFiles_strdup_from_system_locale_to_application(str);
   if(outBuff == NULL)
      return NULL;

   outLen = strlen(outBuff);

   /* convert directory separator */
   for(i = 0; i < outLen; i += size) {
      size = MMDFiles_getcharsize(&outBuff[i]);
      if(size == 1 && MMDFiles_dirseparator(outBuff[i]) == true)
         outBuff[i] = MMDFILES_DIRSEPARATOR;
   }

   return outBuff;
}

/* MMDFiles_strdup_from_application_to_system_locale: convert charset from application to system locale */
char *MMDFiles_strdup_from_application_to_system_locale(const char *str)
{
   if(str == NULL)
      return NULL;

#if defined(_WIN32)
   return MMDFiles_strdup_with_conversion(str, CP_UTF8, CP_ACP);
#elif !defined(__APPLE__) && !defined(__ANDROID__)
   return MMDFiles_strdup_with_conversion(str, "UTF-8", "");
#else
   return MMDFiles_strdup(str);
#endif
}

/* MMDFiles_strdup_from_system_locale_to_application: convert charset from system locale to application */
char *MMDFiles_strdup_from_system_locale_to_application(const char *str)
{
   if(str == NULL)
      return NULL;

#if defined(_WIN32)
   return MMDFiles_strdup_with_conversion(str, CP_ACP, CP_UTF8);
#elif !defined(__APPLE__) && !defined(__ANDROID__)
   return MMDFiles_strdup_with_conversion(str, "", "UTF-8");
#else
   return MMDFiles_strdup(str);
#endif
}

/* MMDFiles_dirname: get directory name from path */
char *MMDFiles_dirname(const char *file)
{
   int i, len, index = -1;
   char size;
   char *dir;

   len = MMDFiles_strlen(file);

   for(i = 0; i < len; i += size) {
      size = MMDFiles_getcharsize(&file[i]);
      if(size == 1 && MMDFiles_dirseparator(file[i]) == true)
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

/* MMDFiles_basename: get file name from path */
char *MMDFiles_basename(const char *file)
{
   int i, len, index = -1;
   char size;
   char *base;

   len = MMDFiles_strlen(file);

   for(i = 0; i < len; i += size) {
      size = MMDFiles_getcharsize(&file[i]);
      if(size == 1 && MMDFiles_dirseparator(file[i]) == true)
         index = i;
   }

   if(index >= 0) {
      base = (char *) malloc(sizeof(char) * (len - index));
      strncpy(base, &file[index + 1], len - index - 1);
      base[len - index - 1] = '\0';
   } else {
      base = MMDFiles_strdup(file);
   }

   return base;
}

/* MMDFiles_fopen: get file pointer */
FILE *MMDFiles_fopen(const char *file, const char *mode)
{
   char *path;
   FILE *fp;

   if(file == NULL || mode == NULL)
      return NULL;

   path = MMDFiles_pathdup_from_application_to_system_locale(file);
   if(path == NULL)
      return NULL;

   fp = fopen(path, mode);
   free(path);

   // remove BOM
   if(fp != NULL && strcmp(mode, "r") == 0) {
      char c1, c2, c3;
      c1 = fgetc(fp);
      c2 = fgetc(fp);
      c3 = fgetc(fp);
      if(c1 == '\xEF' && c2 == '\xBB' && c3 == '\xBF')
         return fp;
      fseek(fp, 0, SEEK_SET);
   }

   return fp;
}

/* MMDFiles_getfsize: get file size */
size_t MMDFiles_getfsize(const char *file)
{
   FILE *fp;
   fpos_t size;

   fp = MMDFiles_fopen(file, "rb");
   if (!fp)
      return 0;

   fseek(fp, 0, SEEK_END);
   fgetpos(fp, &size);
   fseek(fp, 0, SEEK_SET);
   fclose(fp);

#if defined(_WIN32) || defined(__APPLE__) || defined(__ANDROID__)
   return (size_t) size;
#else
   return (size_t) size.__pos;
#endif /* _WIN32 || __APPLE__ || __ANDROID__ */
}

/* MMDFiles_getpagesize: get memory page size */
unsigned int MMDFiles_getpagesize()
{
#if __APPLE__
   return (unsigned int) 4096;
#elif defined(_WIN32)
   SYSTEM_INFO sysinfo;
   GetSystemInfo(&sysinfo);
   return (unsigned int) sysinfo.dwPageSize;
#else
   return (unsigned int) sysconf(_SC_PAGE_SIZE);
#endif
}
