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

#ifndef __mmdagent_lipsync_h__
#define __mmdagent_lipsync_h__

#include "PMDModel.h"
#include "VMD.h"

#define LIPSYNC_MAXBUFLEN  1024
#define LIPSYNC_MOTIONNAME "LipSync" /* motion name of lip sync */

#define LIPSYNC_SEPARATOR ","

/* LibDef: lip definition */
typedef struct _LipDef {
   char *name; /* expression name */
   float rate; /* blend rate */
   struct _LipDef *next;
} LipDef;

/* LipSync: lip sync */
class LipSync
{
private:

   LipDef *m_lipDef[6]; /* list of blend rate */
   int m_numFaces;      /* number of expression */
   char **m_faceName;   /* name list of expression */
   float *m_table[6];   /* table of phoneme ID and blend rate*/

   /* initialize: initialize LipSync */
   void initialize();

   /* clear: free LipSync */
   void clear();

public:

   /* LipSync: constructor */
   LipSync();

   /* ~LipSync: destructor */
   ~LipSync();

   /* setup: initialize and setup LipSync */
   bool setup(PMDModel *pmd);

   /* composeMotion: create motion from phoneme sequence */
   bool createMotion(char *lipSequence, unsigned char **vmdData, unsigned long *vmdSize);
};

#endif /* __mmdagent_lipsync_h__ */
