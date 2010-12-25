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
#include "PMDFile.h"
#include "PTree.h"
#include "btBulletDynamicsCommon.h"
#include "VMD.h"
#include "MotionStocker.h"
#include "utils.h"

/* MotionStocker::initialize: initialize MotionStocker */
void MotionStocker::initialize()
{
   int i;

   for (i = 0; i < VMDGRIDSIZE; i++)
      m_vmdFileName[i] = NULL;
   m_num = 0;
   m_current = 0;
}

/* MotionStocker::clear: free MotionStocker */
void MotionStocker::clear()
{
   int i;

   for (i = 0; i < VMDGRIDSIZE; i++)
      if (m_vmdFileName[i])
         free(m_vmdFileName[i]);

   initialize();
}

/* MotionStocker::MotionStocker: constructor */
MotionStocker::MotionStocker()
{
   initialize();
}

/* MotionStocker::~MotionStocker: destructor */
MotionStocker::~MotionStocker()
{
   clear();
}

/* MotionStocker::load: load a model or return cached one */
VMD * MotionStocker::load(wchar_t *fileName)
{
   VMD *ret;
   int i;
   wchar_t buf[VMDGRID_MAXBUFLEN];

   for (i = m_current - 1; i >= 0; i--) {
      if (wcscmp(m_vmdFileName[i], fileName) == 0) {
#if 0
         g_logger.log(L"Motion \"%s\" in cache", fileName);
#endif
         return &(m_vmdList[i]);
      }
   }
   for (i = m_num - 1; i >= m_current; i--) {
      if (wcscmp(m_vmdFileName[i], fileName) == 0) {
#if 0
         g_logger.log(L"Motion \"%s\" in cache", fileName);
#endif
         return &(m_vmdList[i]);
      }
   }
   if (m_vmdList[m_current].load(fileName) == false) {
      g_logger.log(L"! Error: failed to load vmd file: %s", fileName);
      return NULL;
   }
#if 0
   g_logger.log(L"Motion \"%s\" loaded", fileName);
#endif
   if (m_vmdFileName[m_current])
      free(m_vmdFileName[m_current]);
   m_vmdFileName[m_current] = (wchar_t *) malloc(sizeof(wchar_t) * (wcslen(fileName) + 1));
   wcscpy(m_vmdFileName[m_current], fileName);
   ret = &(m_vmdList[m_current]);
   m_current++;
   if (m_num < m_current)
      m_num = m_current;
   if (m_current >= VMDGRIDSIZE)
      m_current = 0;

#if 0
   _snwprintf(buf, VMDGRID_MAXBUFLEN, L"- %d frames (%.1f sec), %d bones, %d faces\n- total %d key frames for %s",
              (int) ret->getMaxFrame(), ret->getMaxFrame() / 30.0f, ret->getNumBoneKind(), ret->getNumFaceKind(), ret->getTotalKeyFrame(), ret->getNameW());
   g_logger.log(buf);
#endif

   return ret;
}
