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
#include "LipSync.h"
#include "utils.h"

#define MAXPNUM 2048 /* maximum number of phoneme sequence */

/* LipSync::initialize: initialize LipSync */
void LipSync::initialize()
{
   int i;

   for (i = 0; i < 6; i++) {
      m_lipDef[i] = NULL;
      m_table[i] = NULL;
   }
   m_faceName = NULL;
}

/* LipSync::clear: free LipSync */
void LipSync::clear()
{
   int i;
   LipDef *l, *tmp;

   for (i = 0; i < 6; i++) {
      l = m_lipDef[i];
      while (l) {
         tmp = l->next;
         free(l->name);
         delete l ;
         l = tmp;
      }
      if (m_table[i])
         free(m_table[i]);
   }
   if (m_faceName) {
      for (i = 0; i < m_numFaces; i++)
         free(m_faceName[i]);
      free(m_faceName);
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

/* LipSync::setup: initialize and setup LipSync */
bool LipSync::setup(PMDModel *pmd)
{
   int i, j, k, count;
   LipDef *lf;
   bool exist[6];
   char faceNames[6][20] = {"発音「あ」", "発音「い」", "発音「う」", "発音「え」", "発音「お」", "発音「ん」" };
   char buf[LIPSYNC_MAXBUFLEN];
   FILE *fp;
   char linebuf[256];
   float rate;
   char *p, *psave, *q, *pp;
   int n;
   bool ret = true;

   clear();

   for (i = 0; i < 6; i++)
      exist[i] = false;

   /* check for primary face definition */
   count = 0;
   for (i = 0; i < 6; i++) {
      if (pmd->getFace(faceNames[i])) {
         lf = new LipDef;
         lf->name = strdup(faceNames[i]);
         lf->rate = 1.0f;
         lf->next = NULL;
         m_lipDef[i] = lf;
         exist[i] = true;
         count++;
      }
   }
   if (count >= 1 && count <= 5)
      for (i = 0; i < 6; i++)
         if (!exist[i])
            g_logger.mbslog("! Warning: face \"%s\" not in model", faceNames[i]);

   /* read lip.txt if any */
   sprintf(buf, "%s\\Lip.txt", pmd->getModelDir());
   fp = fopen(buf, "r");
   if (fp) {
      while (fgets(linebuf, 256, fp)) {
         if (linebuf[0] == '\n' || linebuf[0] == '\r' || linebuf[0] == '#') continue;
         q = strchr(linebuf, '=');
         if (!q) {
            g_logger.mbslog("! Error: LipSync: wrong format in lip.txt: %s", linebuf);
            continue;
         }
         *q = '\0';
         for (n = 0; n < 6; n++)
            if (strcmp(faceNames[n], linebuf) == 0)
               break;
         if (n >= 6) {
            *q = '=';
            g_logger.mbslog("! Error: LipSync: wrong format in \"Lip.txt\": %s", linebuf);
            continue;
         }
         if (exist[n]) {
            /* primary face definitioin already exist! */
            g_logger.mbslog("! Warning: LipSync: \"%s\" already defined", faceNames[n]);
            continue;
         }
         for (p = strtok_s(q + 1, "+\r\n", &psave); p; p = strtok_s(NULL, "+\r\n", &psave)) {
            pp = strchr(p, '*');
            if (pp) {
               *pp = '\0';
               if (!pmd->getFace(p)) {
                  g_logger.mbslog("! Error: LipSync: face \"%s\" not exist for \"%s\"", p, faceNames[n]);
                  continue;
               }
               rate = (float) atof(pp + 1);
            } else {
               if (!pmd->getFace(p)) {
                  g_logger.mbslog("! Error: LipSync: face \"%s\" not exist for \"%s\"", p, faceNames[n]);
                  continue;
               }
               rate = 1.0f;
            }
            lf = new LipDef;
            lf->name = strdup(p);
            lf->rate = rate;
            lf->next = m_lipDef[n];
            m_lipDef[n] = lf;
            if (pp)
               *pp = '*';
         }
         exist[n] = true;
      }
      fclose(fp);
   }

   for (i = 0; i < 6; i++) {
      if (!exist[i]) {
         g_logger.mbslog("! Warning: LipSync: \"%s\" not in model and Lip.txt, apply default", faceNames[i]);
         switch (i) {
         case 0:
            if (pmd->getFace("あ")) {
               lf = new LipDef;
               lf->name = strdup("あ");
               lf->rate = 0.5f;
               lf->next = m_lipDef[i];
               m_lipDef[i] = lf;
               exist[0] = true;
            }
            break;
         case 1:
            if (pmd->getFace("い")) {
               lf = new LipDef;
               lf->name = strdup("い");
               lf->rate = 0.4f;
               lf->next = m_lipDef[i];
               m_lipDef[i] = lf;
               exist[1] = true;
            }
            break;
         case 2:
            if (pmd->getFace("う")) {
               lf = new LipDef;
               lf->name = strdup("う");
               lf->rate = 1.0f;
               lf->next = m_lipDef[i];
               m_lipDef[i] = lf;
               exist[2] = true;
            }
            break;
         case 3:
            if (pmd->getFace("あ")) {
               lf = new LipDef;
               lf->name = strdup("あ");
               lf->rate = 0.1f;
               lf->next = m_lipDef[i];
               m_lipDef[i] = lf;
               if (pmd->getFace("い")) {
                  lf = new LipDef;
                  lf->name = strdup("い");
                  lf->rate = 0.6f;
                  lf->next = m_lipDef[i];
                  m_lipDef[i] = lf;
                  if (pmd->getFace("う")) {
                     lf = new LipDef;
                     lf->name = strdup("う");
                     lf->rate = 0.2f;
                     lf->next = m_lipDef[i];
                     m_lipDef[i] = lf;
                     exist[3] = true;
                  }
               }
            }
            break;
         case 4:
            if (pmd->getFace("お")) {
               lf = new LipDef;
               lf->name = strdup("お");
               lf->rate = 1.0f;
               lf->next = m_lipDef[i];
               m_lipDef[i] = lf;
               exist[4] = true;
            }
            break;
         case 5:
            exist[5] = true;
            break;
         }
      }
   }

   for (i = 0; i < 6; i++) {
      if (!exist[i]) {
         g_logger.mbslog("! Error: LipSync: failed to initialize \"%s\"", faceNames[i]);
         ret = false;
      }
   }
   if (ret == false)
      return false;

   /* count number of faces and make face list */
   m_numFaces = 0;
   for (i = 0; i < 6; i++)
      for (lf = m_lipDef[i]; lf; lf = lf->next)
         m_numFaces++;
   m_faceName = (char **) malloc(sizeof(char *) * m_numFaces);
   m_numFaces = 0;
   for (i = 0; i < 6; i++) {
      for (lf = m_lipDef[i]; lf; lf = lf->next) {
         for (k = 0; k < m_numFaces; k++)
            if (strcmp(m_faceName[k], lf->name) == 0)
               break;
         if (k >= m_numFaces) {
            m_faceName[m_numFaces] = strdup(lf->name);
            m_numFaces++;
         }
      }
   }

   /* set up tables */
   for (i = 0; i < 6; i++) {
      m_table[i] = (float *) malloc(sizeof(float) * m_numFaces);
      for (j = 0; j < m_numFaces; j++)
         m_table[i][j] = 0.0f;
   }
   for (i = 0; i < 6; i++) {
      for (lf = m_lipDef[i]; lf; lf = lf->next) {
         for (j = 0; j < m_numFaces; j++) {
            if (strcmp(m_faceName[j], lf->name) == 0) {
               m_table[i][j] = lf->rate;
               break;
            }
         }
      }
   }

   return true;
}

#define MARGIN_TRIGGER 2.0f /* start margin frame from closed lip */
#define MARGIN_HEAD 2.0f    /* frame of closed lip */
#define MARGIN_TAIL 10.0f   /* frame of vowel lip */

#define C_DUR_DEFAULT 2.0f
#define V_DUR_DEFAULT 4.0f
#define M_DUR_DEFAULT 2.0f
#define SIL_DUR_DEFAULT 5.0f

/* lip information per phoneme */
struct P2L {
   char pname[10]; /* phoneme name */
   int type;       /* type */
   int vid;        /* vowel ID */
   float rate;     /* opened lip rate */
};

enum {
   T_V,  /* vowel */
   T_C,  /* consonant */
   T_M,  /* bilabial consonant */
   T_Q,  /* sokuon */
   T_SIL /* silent */
};

/* phoneme table */
static P2L p2l[] = {
   {"A", T_V, 0, 1.0f},
   {"I", T_V, 1, 1.0f},
   {"U", T_V, 2, 1.0f},
   {"E", T_V, 3, 1.0f},
   {"O", T_V, 4, 1.0f},
   {"a", T_V, 0, 1.0f},
   {"i", T_V, 1, 1.0f},
   {"u", T_V, 2, 1.0f},
   {"e", T_V, 3, 1.0f},
   {"o", T_V, 4, 1.0f},
   {"a:", T_V, 0, 1.0f},
   {"i:", T_V, 1, 1.0f},
   {"u:", T_V, 2, 1.0f},
   {"e:", T_V, 3, 1.0f},
   {"o:", T_V, 4, 1.0f},
   {"k", T_C, -1, 0.8f},
   {"g", T_C, -1, 0.8f},
   {"s", T_C, -1, 0.25f},
   {"sh", T_C, -1, 0.7f},
   {"z", T_C, -1, 0.4f},
   {"j", T_C, -1, 0.3f},
   {"t", T_C, -1, 0.4f},
   {"ch", T_C, 0, 0.2f},
   {"ts", T_C, 2, 0.3f},
   {"d", T_C, -1, 0.3f},
   {"n", T_C, -1, 0.3f},
   {"h", T_C, -1, 1.0f},
   {"f", T_C, 2, 0.5f},
   {"b", T_M, -1, 0.0f},
   {"p", T_M, -1, 0.0f},
   {"m", T_C, -1, 0.0f},
   {"y", T_C, 1, 0.7f},
   {"r", T_C, -1, 0.5f},
   {"w", T_C, 2, 0.5f},
   {"v", T_M, -1, 0.0f},
   {"N", T_V, 5, 1.0f},
   {"q", T_Q, -1, 0.0f},
   {"cl", T_Q, -1, 0.0f},
   {"ky", T_C, -1, 0.8f},
   {"gy", T_C, -1, 0.8f},
   {"zy", T_C, -1, 0.4f},
   {"dy", T_C, -1, 0.3f},
   {"ty", T_C, -1, 0.3f},
   {"ny", T_C, -1, 0.3f},
   {"hy", T_C, -1, 1.0f},
   {"by", T_M, -1, 0.0f},
   {"py", T_M, -1, 0.0f},
   {"my", T_M, -1, 0.0f},
   {"ry", T_C, -1, 0.5f},
   {"sp", T_M, -1, 0.0f},
   {"pau", T_M, -1, 0.0f},
   {"silE", T_SIL, -1, 0.0f},
   {"silB", T_SIL, -1, 0.0f},
   {"sil", T_SIL, -1, 0.0f},
   {"", -1, -1, 0.0f}
};

/* getVidRate: get vowel ID and blend rate */
static bool getVidRate(int id, int *vid, float *rate, int *pseq, int seqlen)
{
   int v;
   float r;

   if (id >= seqlen) return false;

   switch (p2l[pseq[id]].type) {
   case T_V:
      /* use registered vowel ID and blend rate */
      *vid = p2l[pseq[id]].vid;
      *rate = p2l[pseq[id]].rate;
      break;
   case T_C:
      /* consonant */
      *vid = -1;
      if (p2l[pseq[id]].rate == 0.0f) {
         /* closed lip */
         *vid = 0;
         *rate = 0.0f;
      } else {
         if (p2l[pseq[id]].vid != -1) {
            /* vowel * blend rate */
            *vid = p2l[pseq[id]].vid;
            *rate = p2l[pseq[id]].rate;
         } else {
            /* use next vowel */
            if (id < seqlen - 1) {
               if (p2l[pseq[id+1]].type == T_V) {
                  *vid = p2l[pseq[id+1]].vid;
                  *rate = p2l[pseq[id]].rate;
               }
            }
         }
      }
      if (*vid == -1) {
         /* closed lip */
         *vid = 0;
         *rate = 0.2f;
      }
      break;
   case T_Q:
      /* store next phoneme */
      if (getVidRate(id + 1, &v, &r, pseq, seqlen)) {
         *vid = v;
         *rate = r * 0.9f;
      } else {
         /* end */
         *vid = 0;
         *rate = 0.0f;
      }
      break;
   case T_M:
   case T_SIL:
      /* silent of closed lip */
      *vid = 0;
      *rate = 0.0f;
      break;
   }

   return true;
}

/* KeyFrame: work area */
typedef struct _KeyFrame {
   float frame;
   float rate;
   int type;
   struct _KeyFrame *next;
} KeyFrame;

/* LipSync::createMotion: create motion from phoneme sequence */
bool LipSync::createMotion(char *seq, unsigned char **rawData, unsigned long *rawSize)
{
   int i, j;

   int pseq[MAXPNUM];
   float dur[MAXPNUM];
   char *buf1;
   KeyFrame **keyframe;
   bool ret = true;
   int seqlen;
   char *p, *p_save, *q;
   size_t len;
   int vid;
   KeyFrame *kf;
   float rate;
   float currentFrame = MARGIN_TRIGGER;
   KeyFrame *ktmp;
   unsigned char *data;
   int totalKeyNum = 0;

   /* for VMD */
   VMDFile_Header *header;
   unsigned long *numBoneKeyFrames;
   unsigned long *numFaceKeyFrames;
   VMDFile_FaceFrame *face;

   /* initialize */
   (*rawData) = NULL;
   (*rawSize) = 0;

   /* analysis phoneme sequence */
   if (seq == NULL)
      return false;
   len = strlen(seq);
   if (len <= 0)
      return false;
   buf1 = (char *) malloc(sizeof(char) * (len + 1));
   strcpy(buf1, seq);
   buf1[len] = '\0';
   seqlen = 0;
   for (p = strtok_s(buf1, LIPSYNC_SEPARATOR, &p_save); p; p = strtok_s(NULL, LIPSYNC_SEPARATOR, &p_save)) {
      if (seqlen >= MAXPNUM) {
         g_logger.mbslog("! Error: LipSync: too long phone sequence (>%d)", MAXPNUM);
         ret = false;
         break;
      }
      q = p;
      while ((*q < '0' || *q > '9') && *q != '\0') q++;
      if (*q == '\0')
         dur[seqlen] = -1.0f;
      else
         dur[seqlen] = (float) atof(q) * 0.03f; /* convert ms to frame */
      *q = '\0';
      pseq[seqlen] = -1;
      j = 0;
      while (p2l[j].type != -1) {
         if (!strcmp(p2l[j].pname, p)) {
            pseq[seqlen] = j;
            break;
         }
         j++;
      }
      if (pseq[seqlen] == -1) {
         g_logger.mbslog("! Error: LipSync: unknown phone \"%S\"", p);
         ret = false;
         continue;
      }
      if (dur[seqlen] == -1.0f) {
         switch (p2l[pseq[seqlen]].type) {
         case T_V:
            dur[seqlen] = V_DUR_DEFAULT;
            break;
         case T_C:
            dur[seqlen] = C_DUR_DEFAULT;
            break;
         case T_M:
            dur[seqlen] = M_DUR_DEFAULT;
            break;
         case T_Q:
            dur[seqlen] = V_DUR_DEFAULT;
            break;
         case T_SIL:
            dur[seqlen] = SIL_DUR_DEFAULT;
            break;
         }
      }
      if (dur[seqlen] < 1.0f) {
         g_logger.mbslog("!Warning: LipSync: \"%S\" shorter than a frame (33.3msec), padded to 1 frame", p);
         dur[seqlen] = 1.0f;
      }
      seqlen++;
   }
   if (ret == false) {
      free(buf1);
      return false;
   }

   /* create key frame */
   keyframe = (KeyFrame **) malloc(sizeof(KeyFrame *) * m_numFaces);
   for (i = 0; i < m_numFaces; i++)
      keyframe[i] = NULL;
   for (i = 0; i < seqlen; i++) {
      if (getVidRate(i, &vid, &rate, pseq, seqlen)) {
         for (j = 0; j < m_numFaces; j++) {
            kf = (KeyFrame *) malloc(sizeof(KeyFrame));
            kf->frame = currentFrame;
            kf->rate = m_table[vid][j] * rate;
            kf->type = p2l[pseq[i]].type;
            kf->next = keyframe[j];
            keyframe[j] = kf;
         }
      }
      currentFrame += dur[i];
   }
   for (i = 0; i < m_numFaces; i++) {
      kf = (KeyFrame *) malloc(sizeof(KeyFrame));
      kf->frame = currentFrame;
      kf->rate = 0.0f;
      kf->type = T_V;
      kf->next = keyframe[i];
      keyframe[i] = kf;
   }

   /* insert frame of opened and closed lip */
   for (i = 0; i < m_numFaces; i++) {
      for (kf = keyframe[i]; kf; kf = kf->next) {
         if (kf->next == NULL) continue;
         if ((kf->next->type == T_M || kf->next->type == T_SIL)
               && (kf->type != T_M && kf->next->type != T_SIL)
               && (kf->frame - kf->next->frame) > MARGIN_HEAD) {
            ktmp = (KeyFrame *) malloc(sizeof(KeyFrame));
            ktmp->frame = kf->frame - MARGIN_TRIGGER;
            ktmp->rate = 0.0f;
            ktmp->next = keyframe[i];
            keyframe[i] = ktmp;
         } else if (kf->next->type == T_V && (kf->frame - kf->next->frame) > MARGIN_TAIL) {
            ktmp = (KeyFrame *) malloc(sizeof(KeyFrame));
            ktmp->frame = kf->frame - MARGIN_TRIGGER;
            ktmp->rate = kf->next->rate * 0.8f;
            ktmp->next = keyframe[i];
            keyframe[i] = ktmp;
         }
      }
   }

   /* convert to VMD data */
   totalKeyNum = 0;
   for (i = 0; i < m_numFaces; i++)
      for (kf = keyframe[i]; kf; kf = kf->next)
         totalKeyNum++;

   /* create memories */
   *rawSize = sizeof(VMDFile_Header) + sizeof(unsigned long) + sizeof(unsigned long)
              + sizeof(VMDFile_FaceFrame) * totalKeyNum;
   *rawData = (unsigned char *) malloc(*rawSize);

   data = *rawData;
   /* header */
   header = (VMDFile_Header *) data;
   strncpy(header->header, "Vocaloid Motion Data 0002", 30);
   data += sizeof(VMDFile_Header);
   /* number of key frame for bone */
   numBoneKeyFrames = (unsigned long *) data;
   *numBoneKeyFrames = 0;
   data += sizeof(unsigned long);
   /* number of key frame for expression */
   numFaceKeyFrames = (unsigned long *) data;
   *numFaceKeyFrames = totalKeyNum;
   data += sizeof(unsigned long);
   /* set key frame */
   for (i = 0; i < m_numFaces; i++) {
      for (kf = keyframe[i]; kf; kf = kf->next) {
         face = (VMDFile_FaceFrame *) data;
         strncpy(face->faceName, m_faceName[i], VMD_BONE_FACE_NAME_LEN);
         face->keyFrame = (unsigned long) (kf->frame + 0.5f);
         face->weight = kf->rate;
         data += sizeof(VMDFile_FaceFrame);
      }
   }

   /* release */
   for (i = 0; i < m_numFaces; i++) {
      kf = keyframe[i];
      while (kf) {
         ktmp = kf->next;
         free(kf);
         kf = ktmp;
      }
   }
   free(keyframe);

   return true;
}

