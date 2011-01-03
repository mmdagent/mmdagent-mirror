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

#ifndef __mmdagent_screen_h__
#define __mmdagent_screen_h__

/* DeviceContextInfo: device context information */
typedef struct _DeviceContextInfo {
   HDC hDC;               /* device context */
   HGLRC hGLRC;           /* rendering context */
   bool csaa;             /* true if CSAA is used */
   int multiSampleNum;    /* number of samples for FSAA (MSAA) */
   int multiSampleCovNum; /* coverage of CSAA */
   int multiSampleColNum; /* color value of CSAA */
   int pixelformat;       /* format of pixel (normal) */
   int pixelformat_ARB;   /* format of pixel (extension) */
} DeviceContextInfo;

/* Screen: screen window */
class Screen
{
private:

   DeviceContextInfo m_dcinfo; /* device context information */

   bool m_vsync;                  /* true if vertical sync. */
   bool m_fullscreen;             /* true if full screen mode */
   RECT m_originalWindowSize;     /* window size before full screen mode */
   bool m_mouseDisp;              /* true if mouse is showed */
   double m_mouseActiveLeftFrame; /* store display frame after moving mouse */

   /* extensionSupported: check if an OpenGL Extension is supported */
   bool extensionSupported(const char *pszExtensionName);

   /* getPixelFormat: get pixel format */
   void getPixelFormat(DeviceContextInfo *info);

   /* setUpPixelFormat: setup pixel format */
   void setUpPixelFormat(DeviceContextInfo *info);

   /* getPixelFormatARB: get pixel format */
   void getPixelFormatARB(DeviceContextInfo *info, int max_sample_num, int max_sample_col_num);

   /* initDC: get device context */
   void initDC(HWND hWnd, int pixelFormat);

   /* setVSync: set vertical sync. */
   void setVSync(bool flag);

   /* initialize: initialize screen */
   void initialize();

   /* clear: free screen */
   void clear();

public:

   /* Screen: constructor */
   Screen();

   /* Screen: destructor */
   ~Screen();

   /* createWindow: create window */
   HWND createWindow(int *size, HINSTANCE hInstance, TCHAR *szTitle, TCHAR *szWindowClass, int max_sample_num, int max_sample_col_num, bool topMost);

   /* swapBuffers: swap buffers */
   void swapBuffers();

   /* toggleVSync: toggle vertical sync. flag */
   void toggleVSync();

   /* setFullScreen: set fullscreen */
   void setFullScreen(HWND hWnd);

   /* exitFullScreen: exit fullscreen */
   void exitFullScreen(HWND hWnd);

   /* setMouseActiveTime: set mouse active time */
   void setMouseActiveTime(double frame);

   /* updateMouseActiveTime: update mouse active time */
   void updateMouseActiveTime(double deltaFrame);

   /* getInfoString: get screen information */
   char * getInfoString(char *buf, size_t buflen);

   /* getDC: get device context */
   HDC getDC();
};

#endif /* __mmdagent_screen_h__ */
