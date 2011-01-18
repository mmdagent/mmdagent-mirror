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

/* getTokenFromFp: get token from file pointer */
static int getTokenFromFp(FILE *fp, char *buff)
{
   int i;
   char c;

   c = fgetc(fp);
   printf("%ld\n", ftell(fp));
   if(c == EOF) {
      buff[0] = '\0';
      return 0;
   }

   if(c == '#') {
      for(c = fgetc(fp); c != EOF; c = fgetc(fp))
         if(c == '\n')
            return getTokenFromFp(fp, buff);
      buff[0] = '\0';
      return 0;
   }

   if(c == ' ' || c == '\t' || c == '\r' || c == '\n')
      return getTokenFromFp(fp, buff);

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

/* LipSync::initialize: initialize lipsync */
void LipSync::initialize()
{
   m_numMotion = 0;
   m_motion = NULL;

   m_numPhone = 0;
   m_phone = NULL;
   m_blendRate = NULL;
}

/* LipSync::clear: free lipsync */
void LipSync::clear()
{
   int i;

   if(m_motion != NULL) {
      for(i = 0; i < m_numMotion; i++)
         free(m_motion[i]);
      free(m_motion);
   }
   if(m_phone != NULL) {
      for(i = 0; i < m_numPhone; i++)
         free(m_phone[i]);
      free(m_phone);
   }
   if(m_blendRate != NULL) {
      for(i = 0; i < m_numPhone; i++)
         free(m_blendRate[i]);
      free(m_blendRate);
   }

   initialize();
}

/* LipSync::LipSync: constructor */
LipSync::LipSync()
{
   initialize();
}

/* LipSync::~LipSync: destructor */
LipSync::~LipSync()
{
   clear();
}

/* LipSync::load: initialize and load lip setting */
bool LipSync::load(char *file)
{
   int i, j;
   FILE *fp;
   int len;
   char buff[LIPSYNC_MAXBUFLEN];
   bool err = false;

   fp = fopen(file, "r");
   if(fp == NULL)
      return false;

   /* number of expression */
   len = getTokenFromFp(fp, buff);
   if(len <= 0) {
      fclose(fp);
      return false;
   }
   m_numMotion = atoi(buff);
   if(m_numMotion <= 0) {
      fclose(fp);
      clear();
      return false;
   }

   /* motion name */
   m_motion = (char **) malloc(sizeof(char *) * m_numMotion);
   for(i = 0; i < m_numMotion; i++) {
      len = getTokenFromFp(fp, buff);
      if(len <= 0) err = true;
      m_motion[i] = strdup(buff);
   }
   if(err == true) {
      fclose(fp);
      clear();
      return false;
   }

   /* number of phone */
   len = getTokenFromFp(fp, buff);
   if(len <= 0) {
      fclose(fp);
      clear();
      return false;
   }
   m_numPhone = atoi(buff);
   if(m_numPhone <= 0) {
      fclose(fp);
      clear();
      return false;
   }

   /* phone name, type, and blend rate */
   m_phone = (char **) malloc(sizeof(char *) * m_numPhone);
   m_blendRate = (float **) malloc(sizeof(float *) * m_numPhone);
   for(i = 0; i < m_numPhone; i++) {
      len = getTokenFromFp(fp, buff);
      if(len <= 0) err = true;
      m_phone[i] = strdup(buff);
      m_blendRate[i] = (float *) malloc(sizeof(float) * m_numMotion);
      for(j = 0; j < m_numMotion; j++) {
         len = getTokenFromFp(fp, buff);
         if(len <= 0) err = true;
         m_blendRate[i][j] = (float) atof(buff);
         if(m_blendRate[i][j] < 0.0f) err = true;
      }
   }
   if(err == true) {
      fclose(fp);
      clear();
      return false;
   }

   fclose(fp);
   return true;
}

/* LipSync::createMotion: create motion from phoneme sequence */
bool LipSync::createMotion(char *str, unsigned char **rawData, unsigned long *rawSize)
{
   int i, j, k;
   int len;
   char *buf, *p;

   LipKeyFrame *head, *tail, *tmp1, *tmp2;
   float f, diff;

   int totalNumKey;
   unsigned long currentFrame;
   unsigned char *data;
   VMDFile_Header *header;
   unsigned long *numBoneKeyFrames;
   unsigned long *numFaceKeyFrames;
   VMDFile_FaceFrame *face;

   /* check */
   if(str == NULL || m_numMotion <= 0 || m_numPhone <= 0)
      return false;

   /* initialize */
   (*rawData) = NULL;
   (*rawSize) = 0;

   /* get phone index and duration */
   buf = strdup(str);
   head = NULL;
   tail = NULL;
   diff = 0.0f;
   for(i = 0, k = 0, p = strtok(buf, LIPSYNC_SEPARATOR); p; i++, p = strtok(NULL, LIPSYNC_SEPARATOR)) {
      if(i % 2 == 0) {
         for(j = 0; j < m_numPhone; j++) {
            if(strcmp(m_phone[j], p) == 0) {
               k = j;
               break;
            }
         }
         if(m_numPhone <= j)
            k = 0;
      } else {
         tmp1 = (LipKeyFrame *) malloc(sizeof(LipKeyFrame));
         tmp1->phone = k;
         f = 0.03f * (float) atof(p) + diff; /* convert ms to frame */
         tmp1->duration = (int) (f + 0.5);
         if(tmp1->duration < 1)
            tmp1->duration = 1;
         diff = f - tmp1->duration;
         tmp1->rate = 1.0f;
         tmp1->next = NULL;
         if(head == NULL)
            head = tmp1;
         else
            tail->next = tmp1;
         tail = tmp1;
      }
   }

   /* add final closed lip */
   tmp1 = (LipKeyFrame *) malloc(sizeof(LipKeyFrame));
   tmp1->phone = 0;
   tmp1->duration = 1;
   tmp1->rate = 0.0f;
   tmp1->next = NULL;
   if(head == NULL)
      head = tmp1;
   else
      tail->next = tmp1;
   tail = tmp1;

   /* insert interpolation lip motion */
   for(tmp1 = head; tmp1; tmp1 = tmp1->next) {
      if(tmp1->next && tmp1->duration > LIPSYNC_INTERPOLATIONMARGIN) {
         tmp2 = (LipKeyFrame *) malloc(sizeof(LipKeyFrame));
         tmp2->phone = tmp1->phone;
         tmp2->duration = LIPSYNC_INTERPOLATIONMARGIN;
         tmp2->rate = tmp1->rate * LIPSYNC_INTERPOLATIONRATE;
         tmp2->next = tmp1->next;
         tmp1->duration -= LIPSYNC_INTERPOLATIONMARGIN;
         tmp1->next = tmp2;
         tmp2 = tmp1;
      }
   }

   /* count length of key frame */
   len = 0;
   for(tmp1 = head; tmp1; tmp1 = tmp1->next)
      len++;

   totalNumKey = m_numMotion * len;

   /* create memories */
   (*rawSize) = sizeof(VMDFile_Header) + sizeof(unsigned long) + sizeof(unsigned long) + sizeof(VMDFile_FaceFrame) * totalNumKey;
   i = (*rawSize);
   i = sizeof(unsigned char) * (*rawSize);
   (*rawData) = (unsigned char *) malloc(i);

   data = (*rawData);
   /* header */
   header = (VMDFile_Header *) data;
   strncpy(header->header, "Vocaloid Motion Data 0002", 30);
   data += sizeof(VMDFile_Header);
   /* number of key frame for bone */
   numBoneKeyFrames = (unsigned long *) data;
   (*numBoneKeyFrames) = 0;
   data += sizeof(unsigned long);
   /* number of key frame for expression */
   numFaceKeyFrames = (unsigned long *) data;
   (*numFaceKeyFrames) = totalNumKey;
   data += sizeof(unsigned long);
   /* set key frame */
   for (i = 0; i < m_numMotion; i++) {
      currentFrame = 0;
      for(tmp1 = head; tmp1; tmp1 = tmp1->next) {
         face = (VMDFile_FaceFrame *) data;
         strncpy(face->name, m_motion[i], 15);
         face->keyFrame = currentFrame;
         face->weight = m_blendRate[tmp1->phone][i] * tmp1->rate;
         data += sizeof(VMDFile_FaceFrame);
         currentFrame += tmp1->duration;
      }
   }

   /* free */
   free(buf);
   for(tmp1 = head; tmp1; tmp1 = tmp2) {
      tmp2 = tmp1->next;
      free(tmp1);
   }
   return true;
}
