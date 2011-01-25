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

#define UTILS_DIRSEPARATOR '\\'
#define UTILS_MAXCHARBYTE 3

/* MMDAgent_getcharsize: get character size */
char MMDAgent_getcharsize(const char *str);

/* MMDAgent_strequal: string matching */
bool MMDAgent_strequal(const char *str1, const char *str2);

/* MMDAgent_strtailmatch: match tail string */
bool MMDAgent_strtailmatch(const char *str1, const char *str2);

/* MMDAgent_strdup: strdup */
char *MMDAgent_strdup(const char *str);

/* MMDAgent_intdup: integer type strdup */
char *MMDAgent_intdup(const int digit);

/* MMDAgent_dirdup: get directory from file path */
char *MMDAgent_dirdup(const char *file);

/* MMDAgent_strtok: strtok */
char *MMDAgent_strtok(char *str, const char *pat, char **save);

/* MMDAgent_str2bool: convert string to boolean */
bool MMDAgent_str2bool(const char *str);

/* MMDAgent_str2int: convert string to integer */
int MMDAgent_str2int(const char *str);

/* MMDAgent_str2float: convert string to float */
float MMDAgent_str2float(const char *str);

/* MMDAgent_str2ivec: convert string to integer vector */
bool MMDAgent_str2ivec(const char *str, int *vec, const int size);

/* MMDAgent_str2fvec: convert string to float vector */
bool MMDAgent_str2fvec(const char *str, float *vec, const int size);

/* MMDAgent_str2pos: get position from string */
bool MMDAgent_str2pos(const char *str, btVector3 *pos);

/* MMDAgent_str2rot: get rotation from string */
bool MMDAgent_str2rot(const char *str, btQuaternion *rot);

/* MMDAgent_fgettoken: get token from file pointer */
int MMDAgent_fgettoken(FILE *fp, char *buff);
