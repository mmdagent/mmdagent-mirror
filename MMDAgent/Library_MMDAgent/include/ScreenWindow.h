/* ----------------------------------------------------------------- */
/*           The Toolkit for Building Voice Interaction Systems      */
/*           "MMDAgent" developed by MMDAgent Project Team           */
/*           http://www.mmdagent.jp/                                 */
/* ----------------------------------------------------------------- */
/*                                                                   */
/*  Copyright (c) 2009-2013  Nagoya Institute of Technology          */
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

/* ScreenWindow: screen window */
class ScreenWindow
{
private:
   bool m_enable;

   bool m_vsync;                     /* true if vertical sync. */
   unsigned char m_numMultiSampling; /* number of multi-sampling */
   bool m_showMouse;                 /* true if mouse is showed */
   double m_mouseActiveLeftFrame;    /* store display frame after moving mouse */
   bool m_fullScreen;                /* true if full screen mode */

   /* initialize: initialize screen */
   void initialize();

   /* clear: free screen */
   void clear();

public:

   /* ScreenWindow: constructor */
   ScreenWindow();

   /* ScreenWindow: destructor */
   ~ScreenWindow();

   /* setup: create window */
   bool setup(const int *size, const char *title, int maxNumSampling);

   /* swapBuffers: swap buffers */
   void swapBuffers();

   /* toggleVSync: toggle vertical sync. flag */
   void toggleVSync();

   /* getVSync: get vertical sync. flag */
   bool getVSync();

   /* getNumMultiSampling: get number of multi-sampling */
   int getNumMultiSampling();

   /* setMouseActiveTime: set mouse active time */
   void setMouseActiveTime(double frame);

   /* updateMouseActiveTime: update mouse active time */
   void updateMouseActiveTime(double frame);

   /* setFullScreen: set fullscreen */
   void setFullScreen();

   /* exitFullScreen: exit fullscreen */
   void exitFullScreen();
};
