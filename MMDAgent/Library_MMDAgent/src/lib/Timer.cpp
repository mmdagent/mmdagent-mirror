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

#include <windows.h>

#include "MMDAgent.h"

/* Timer::diffTime: return time difference between two counts */
DWORD Timer::diffTime(DWORD now, DWORD past)
{
   if (past > now)
      return (DWORD)(0x100000000 + now - past); /* timer overflow between now and past */
   else
      return now - past;
}

/* Timer::initialize: initialize timer */
void Timer::initialize()
{
   m_prec = 0;

   m_systemStartTime = 0;
   m_lastUpdateFrameSystem = 0.0;
   m_pauseTime = 0;

   m_fps = 0.0f;
   m_fpsStartTime = 0;
   m_fpsCount = 0;

   m_targetAdjustmentFrame = 0.0;
   m_currentAdjustmentFrame = 0.0;
   m_enableAdjustment = false;

   m_userStartTime = 0;
}

/* Timer::clear: free timer */
void Timer::clear()
{
   if (m_prec > 0)
      timeEndPeriod(m_prec); /* reset precision of timer */
   initialize();
}

/* Timer::Timer: constructor */
Timer::Timer()
{
   initialize();
}

/* Timer::~Timer: destructor */
Timer::~Timer()
{
   clear();
}

/* Timer::setup: initialize and start timer */
void Timer::setup(UINT msec)
{
   if (m_prec != msec) {
      if (m_prec > 0)
         timeEndPeriod(m_prec);
      m_prec = msec;
      timeBeginPeriod(m_prec);
   }

   /* get system start time */
   m_systemStartTime = timeGetTime();
   /* reset number of frames from last getTimeInterval function */
   m_lastUpdateFrameSystem = 0.0;
   /* reset start time of fps count */
   m_fpsStartTime = m_systemStartTime;
   /* reset count of calling countFrame function */
   m_fpsCount = 0;
}

/* Timer::getFrameInterval: return time interval from last call */
double Timer::getFrameInterval()
{
   DWORD currentTime;
   double currentFrameSystem;
   double intervalFrame;

   /* get current msec */
   currentTime = timeGetTime();
   /* calculate time from system start time */
   currentFrameSystem = diffTime(currentTime, m_systemStartTime) * 0.03;
   /* get number of frames from last calling */
   intervalFrame = currentFrameSystem - m_lastUpdateFrameSystem;
   /* save number of frames for next calling */
   m_lastUpdateFrameSystem = currentFrameSystem;
   return intervalFrame;
}

/* Timer::pause: pause timer */
void Timer::pause()
{
   m_pauseTime = timeGetTime();
}

/* Timer::resume: resume timer */
void Timer::resume()
{
   m_lastUpdateFrameSystem += diffTime(timeGetTime(), m_pauseTime) * 0.03;
}

/* Timer::start: start user timer */
void Timer::start()
{
   m_userStartTime = timeGetTime();
}

/* Timer::ellapsed: return ellapsed time since last call of start() */
int Timer::ellapsed()
{
   return((int)diffTime(timeGetTime(), m_userStartTime));
}

/* Timer::countFrame: increment frame count for FPS calculation */
void Timer::countFrame()
{
   DWORD t;

   /* increment count */
   m_fpsCount++;

   /* update fps per second */
   t = timeGetTime();
   if (t - m_fpsStartTime >= 1000) {
      /* calculate fps */
      m_fps = 1000.0f * (float) m_fpsCount / (float)(t - m_fpsStartTime);
      /* reset counter */
      m_fpsStartTime = t;
      m_fpsCount = 0;
   }
}

/* Timer::getFps: get fps */
float Timer::getFps()
{
   return m_fps;
}

/* Timer::setTargetAdjustmentFrame: set target frame to sync music */
void Timer::setTargetAdjustmentFrame(double frame)
{
   m_targetAdjustmentFrame = frame;
}

/* Timer::startAdjustment: start to sync music */
void Timer::startAdjustment()
{
   m_currentAdjustmentFrame = 0.0;
   m_enableAdjustment = true;
}

/* Timer::stopAdjustment: stop to sync music */
void Timer::stopAdjustment()
{
   m_enableAdjustment = false;
}

/* Timer::getCurrentAdjustmentFrame: get current frame to sync music */
double Timer::getCurrentAdjustmentFrame()
{
   return m_currentAdjustmentFrame;
}

/* Timer::getAdditionalFrame: get number of additional frames to sync music */
double Timer::getAdditionalFrame(double baseProcFrame)
{
   double mstep = 0.0;

   if (m_enableAdjustment == false)
      return 0.0;

   if (m_targetAdjustmentFrame > m_currentAdjustmentFrame) {
      /* x2 (max=10ms) */
      if (baseProcFrame > 0.3) {
         mstep = 0.3;
      } else {
         mstep = baseProcFrame;
      }
      if (m_currentAdjustmentFrame + mstep > m_targetAdjustmentFrame) {
         mstep = m_targetAdjustmentFrame - m_currentAdjustmentFrame;
         m_currentAdjustmentFrame = m_targetAdjustmentFrame;
      } else {
         m_currentAdjustmentFrame += mstep;
      }
   }
   if (m_targetAdjustmentFrame < m_currentAdjustmentFrame) {
      /* /2 (max=5ms) */
      if (baseProcFrame > 0.3) {
         mstep = -0.15;
      } else {
         mstep = - (baseProcFrame * 0.5);
      }
      if (m_currentAdjustmentFrame + mstep < m_targetAdjustmentFrame) {
         mstep = m_targetAdjustmentFrame - m_currentAdjustmentFrame;
         m_currentAdjustmentFrame = m_targetAdjustmentFrame;
      } else {
         m_currentAdjustmentFrame += mstep;
      }
   }

   return mstep;
}
