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
#include "Screen.h"
#include "GLee.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

#define GL_COVERAGE_SAMPLES_NV  0x80A9
#define GL_COLOR_SAMPLES_NV     0x8E20
#define WGL_COVERAGE_SAMPLES_NV 0x2042
#define WGL_COLOR_SAMPLES_NV    0x20B9

#define DUMMY_CLASS_NAME L"MMD_AGENT_DUMMY"

#define IDI_MMDAGENT  107 /* should be removed */

/* dummyWndProc: dummy proc function */
static LRESULT CALLBACK dummyWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

/* dummyMyRegisterClass: dummy register function */
static ATOM dummyMyRegisterClass(HINSTANCE hInstance)
{
   WNDCLASSEX wcex;

   wcex.cbSize = sizeof(WNDCLASSEX);

   wcex.style = CS_HREDRAW | CS_VREDRAW;
   wcex.lpfnWndProc = dummyWndProc;
   wcex.cbClsExtra = 0;
   wcex.cbWndExtra = 0;
   wcex.hInstance = hInstance;
   wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MMDAGENT));
   wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
   wcex.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
   wcex.lpszMenuName = 0;
   wcex.lpszClassName = DUMMY_CLASS_NAME;
   wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_MMDAGENT));

   return RegisterClassEx(&wcex);
}

/* Screen::initialize: initialize screen */
void Screen::initialize()
{
   m_vsync = true;
   m_fullscreen = false;
   m_mouseDisp = true;
   m_mouseActiveLeftFrame = 0.0;
}

/* Screen::clear: free screen */
void Screen::clear()
{
   initialize();
}

/* Screen::Screen: constructor */
Screen::Screen()
{
   initialize();
}

/* Screen::Screen: destructor */
Screen::~Screen()
{
   clear();
}

/* Screen::extensionSupported: check if an OpenGL Extension is supported */
bool Screen::extensionSupported(const char *pszExtensionName)
{
   static const char *pszGLExtensions = 0;
   static const char *pszWGLExtensions = 0;

   if (!pszGLExtensions)
      pszGLExtensions = reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS));

   if (!pszWGLExtensions) {
      typedef const char *(WINAPI * PFNWGLGETEXTENSIONSSTRINGARBPROC)(HDC);

      PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB =
         reinterpret_cast<PFNWGLGETEXTENSIONSSTRINGARBPROC>(wglGetProcAddress("wglGetExtensionsStringARB"));

      if (wglGetExtensionsStringARB)
         pszWGLExtensions = wglGetExtensionsStringARB(wglGetCurrentDC());
   }

   if (!strstr(pszGLExtensions, pszExtensionName))
      if (!strstr(pszWGLExtensions, pszExtensionName))
         return false;

   return true;
}

/* Screen::initDC: get device context */
void Screen::initDC(HWND hWnd, int pixelFormat)
{
   /* get default device context of window */
   m_dcinfo.hDC = ::GetDC(hWnd);

   if (pixelFormat == -1) {
      /* initialize information */
      m_dcinfo.csaa = false;
      m_dcinfo.multiSampleNum = 0;
      m_dcinfo.multiSampleColNum = 0;
      m_dcinfo.pixelformat = -1;
      m_dcinfo.pixelformat_ARB = -1;
      /* get pixel format */
      getPixelFormat(&m_dcinfo);
   } else {
      /* initialize by given pixel format */
      m_dcinfo.pixelformat = pixelFormat;
      m_dcinfo.pixelformat_ARB = pixelFormat;
   }

   /* create rendering context by setup pixel format */
   if (m_dcinfo.pixelformat != -1)
      setUpPixelFormat(&m_dcinfo);
}

/* Screen::getPixelFormat: get pixel format */
void Screen::getPixelFormat(DeviceContextInfo *info)
{
   static PIXELFORMATDESCRIPTOR pfd = {
      sizeof(PIXELFORMATDESCRIPTOR), /* structure size */
      1, /* version of OpenGL */
      PFD_DRAW_TO_WINDOW |
      PFD_DOUBLEBUFFER | /* double buffer */
      PFD_SUPPORT_OPENGL, /* use OpenGL */
      PFD_TYPE_RGBA, /* color data of pixel */
      32, /* number of bit for color */
      0, 0, 0, 0, 0, 0, 0, 0, /* bits of RGBA color */
      0, 0, 0, 0, 0, /* bits of accum. buffer per pixel */
      32, /* bits of debs buffer per pixel */
      2, /* bits of stencil buffer for shadow */
      0, /* bits of sub buffer */
      PFD_MAIN_PLANE, /* layer type */
      0,
      0,
      0,
      0
   };
   info->pixelformat = ::ChoosePixelFormat(info->hDC, &pfd);

   if (info->pixelformat == 0) /* error */
      info->pixelformat = -1;
}

/* Screen::setUpPixelFormat: setup pixel format */
void Screen::setUpPixelFormat(DeviceContextInfo *info)
{
   static PIXELFORMATDESCRIPTOR pfd;

   if (info->pixelformat == -1) return;

   /* get information from pixel format */
   ::DescribePixelFormat(info->hDC, info->pixelformat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

   /* set pixel format */
   if (!::SetPixelFormat(info->hDC, info->pixelformat, &pfd))
      info->pixelformat = -1; /* error */

   /* create rendering contest */
   info->hGLRC = (info->pixelformat != -1) ? wglCreateContext(info->hDC) : NULL;
}

/* Screen::getPixelFormatARB: get pixel format */
void Screen::getPixelFormatARB(DeviceContextInfo *info, int max_sample_num, int max_sample_col_num)
{
   int i, j;
   HDC hdc = info->hDC;

   /* get pixel format (extension) */
   static GLfloat fAttribs[] = { 0.0f, 0.0f };
   int iAttribs[] = {
      WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
      WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
      WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
      WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
      WGL_COLOR_BITS_ARB, 24, /* 24 */
      WGL_ALPHA_BITS_ARB, 8, /* 8 */
      WGL_DEPTH_BITS_ARB, 24, /* 24 */
      WGL_STENCIL_BITS_ARB, 8, /* 8 for shadow */
      WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
      WGL_SAMPLE_BUFFERS_ARB, 1,
      0, 0, 0, 0, /* WGL_SAMPLES_ARB */
      0, 0
   };
   UINT numFormats;
   bool found;
   int offset;

   info->multiSampleNum = 0;

   if (info->pixelformat == -1) return;

   /* initialize */
   wglMakeCurrent(hdc, info->hGLRC);

   /* get pixel format setting */
   offset = 20; /* index of WGL_SAMPLES_ARB */

   /* if CSAA is supported, use it */
   if (extensionSupported("GL_NV_multisample_coverage") && extensionSupported("WGL_NV_multisample_coverage") && max_sample_col_num > 0)
      info->csaa = true;
   else
      info->csaa = false;
   /* coverage limit is 16 */
   if (max_sample_num > 16)
      max_sample_num = 16;
   if (info->csaa) {
      iAttribs[offset] = WGL_COVERAGE_SAMPLES_NV;
      iAttribs[offset+2] = WGL_COLOR_SAMPLES_NV;
      /* CSAA color limit is 8 */
      if (max_sample_col_num > 8)
         max_sample_col_num = 8;
   } else {
      iAttribs[offset] = WGL_SAMPLES_ARB;
      iAttribs[offset+2] = 0;
   }

   offset++;
   found = false;
   info->multiSampleNum = 0;
   info->multiSampleColNum = 0;

   if (info->csaa) {
      for (i = max_sample_num; i > 0; i /= 2) {
         for (j = max_sample_col_num; j > 0; j /= 2) {
            if (j > i)
               continue;
            iAttribs[offset] = i;
            iAttribs[offset+2] = j;
            if (wglChoosePixelFormatARB(hdc, iAttribs, fAttribs, 1, &(info->pixelformat_ARB), &numFormats) && numFormats >= 1) {
               found = true;
               info->multiSampleNum = i;
               info->multiSampleColNum = j;
               i = 0;
               break;
            }
         }
      }
   } else {
      for (i = max_sample_num; i > 0; i /= 2) {
         iAttribs[offset] = i;
         if (wglChoosePixelFormatARB(hdc, iAttribs, fAttribs, 1, &(info->pixelformat_ARB), &numFormats) && numFormats >= 1) {
            found = true;
            info->multiSampleNum = i;
            break;
         }
      }
   }

   /* release */
   wglMakeCurrent(hdc, NULL);
   wglDeleteContext(info->hGLRC);
   info->hGLRC = NULL;
   if (!found) {
      info->pixelformat_ARB = -1;
      info->multiSampleNum = 0;
      info->multiSampleColNum = 0;
      /* use default pixel format */
   } else {
      /* remake */
      info->pixelformat = info->pixelformat_ARB;
   }
}

/* Screen::createWindow: create window */
HWND Screen::createWindow(int *size, HINSTANCE hInstance, TCHAR *szTitle, TCHAR *szWindowClass, int max_sample_num, int max_sample_col_num, bool topMost)
{
   HWND hWnd;
   int currentPixelFormat;

   /* register dummy class for FSAA querying */
   dummyMyRegisterClass(hInstance);

   /* make dummy window */
   hWnd = CreateWindowEx(
             (topMost ? WS_EX_TOPMOST : 0),
             max_sample_num > 0 ? DUMMY_CLASS_NAME : szWindowClass,
             szTitle,
             WS_VISIBLE | WS_OVERLAPPEDWINDOW,
             CW_USEDEFAULT,
             0,
             size[0],
             size[1],
             NULL,
             NULL,
             hInstance,
             NULL
          );

   if (!hWnd)
      return hWnd;

   /* prepare initial device context (if FSAA is used, dummy) */
   initDC(hWnd, -1);

   if (max_sample_num > 0) {

      /* get pixel format (extension) */
      getPixelFormatARB(&m_dcinfo, max_sample_num, max_sample_col_num);
      /* save parameter */
      currentPixelFormat = m_dcinfo.pixelformat;
      /* close winodw */
      DestroyWindow(hWnd);
      /* re-make window */
      hWnd = CreateWindowEx(
                (topMost ? WS_EX_TOPMOST : 0),
                szWindowClass,
                szTitle,
                WS_VISIBLE | WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT,
                0,
                size[0],
                size[1],
                NULL,
                NULL,
                hInstance,
                NULL
             );
      if (!hWnd)
         return hWnd;

      /* prepare initialize device context using pixel format */
      initDC(hWnd, currentPixelFormat);

      /* set rendering target of OpenGL */
      wglMakeCurrent(m_dcinfo.hDC, m_dcinfo.hGLRC);

      /* enable multi-sampling of OpenGL */
      if (m_dcinfo.multiSampleNum > 0)
         glEnable(GL_MULTISAMPLE);
   }

   /* if CSAA, get parameters again */
   if (m_dcinfo.multiSampleColNum > 0) {
      glGetIntegerv(GL_SAMPLES_ARB, &(m_dcinfo.multiSampleNum));
      glGetIntegerv(GL_COLOR_SAMPLES_NV, &(m_dcinfo.multiSampleColNum));
      glGetIntegerv(GL_COVERAGE_SAMPLES_NV, &(m_dcinfo.multiSampleCovNum));
   }

   /* ser rendering target of OpenGL */
   wglMakeCurrent(m_dcinfo.hDC, m_dcinfo.hGLRC);

   /* set vertical sync. */
   setVSync(m_vsync);

   return hWnd;
}

/* Screen::swapBuffers: swap buffers */
void Screen::swapBuffers()
{
   SwapBuffers(m_dcinfo.hDC);
}

/* Screen::setVSync: set vertical sync. */
void Screen::setVSync(bool flag)
{
   wglSwapIntervalEXT(flag ? 1 : 0);
   m_vsync = flag;
}

/* Screen::toggleVSync: toggle vertical sync. flag */
void Screen::toggleVSync()
{
   setVSync(!m_vsync);
}

/* Screen::setFullScreen: set fullscreen */
void Screen::setFullScreen(HWND hWnd)
{
   DWORD dwStyle;
   RECT rc;

   /* save window size before full screen */
   GetWindowRect(hWnd, &(m_originalWindowSize));

   /* remove window appendage */
   dwStyle = GetWindowLong(hWnd, GWL_STYLE);
   dwStyle &= ~WS_CAPTION;
   dwStyle &= ~WS_SYSMENU;
   dwStyle &= ~WS_MINIMIZEBOX;
   dwStyle &= ~WS_THICKFRAME;
   SetWindowLong(hWnd, GWL_STYLE, dwStyle);

   /* set full screen */
   GetWindowRect(GetDesktopWindow(), &rc);
   SetWindowPos(hWnd, NULL, 0, 0, rc.right, rc.bottom, SWP_SHOWWINDOW);

   /* mouse focus */
   SetFocus(hWnd);

   /* disable mouse cursor */
   if (m_mouseDisp) {
      ShowCursor(false);
      m_mouseDisp = false;
   }

   /* save */
   m_fullscreen = true;
}

/* Screen::exitFullScreen: exit fullscreen */
void Screen::exitFullScreen(HWND hWnd)
{
   DWORD dwStyle;

   /* reset window setting */
   dwStyle = GetWindowLong(hWnd, GWL_STYLE);
   dwStyle |= WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_THICKFRAME;
   SetWindowLong(hWnd, GWL_STYLE, dwStyle);

   /* reset window size */
   SetWindowPos(hWnd, NULL,
                m_originalWindowSize.left, m_originalWindowSize.top,
                m_originalWindowSize.right - m_originalWindowSize.left,
                m_originalWindowSize.bottom - m_originalWindowSize.top,
                SWP_SHOWWINDOW);

   /* enable mouse cursor */
   if (! m_mouseDisp) {
      ShowCursor(true);
      m_mouseDisp = true;
   }

   /* save */
   m_fullscreen = false;
}

/* Screen::setMouseActiveTime: set mouse active time */
void Screen::setMouseActiveTime(double frame)
{
   m_mouseActiveLeftFrame = frame;
   if (m_fullscreen) {
      /* if full screen, disable mouse cursor */
      if (! m_mouseDisp) {
         ShowCursor(true);
         m_mouseDisp = true;
      }
   }
}

/* Screen::updateMouseActiveTime: update mouse active time */
void Screen::updateMouseActiveTime(double deltaFrame)
{
   if (m_mouseActiveLeftFrame == 0.0)
      return;
   m_mouseActiveLeftFrame -= deltaFrame;
   if (m_mouseActiveLeftFrame <= 0.0) {
      m_mouseActiveLeftFrame = 0.0;
      if (m_fullscreen) {
         /* if full screen, disable mouse cursor */
         if (m_mouseDisp) {
            ShowCursor(false);
            m_mouseDisp = false;
         }
      }
   }
}

/* Screen::getInfoString: get screen information */
char * Screen::getInfoString(char *buf, size_t buflen)
{
   char ch[3];

   if (m_dcinfo.multiSampleColNum > 0) {
      /* CSAA */
      if (m_dcinfo.multiSampleColNum == m_dcinfo.multiSampleCovNum) {
         /* equivalent to normal MSAA */
         _snprintf(buf, buflen, "%dx MSAA (%d,%d)", m_dcinfo.multiSampleNum, m_dcinfo.multiSampleColNum, m_dcinfo.multiSampleCovNum);
      } else {
         switch (m_dcinfo.multiSampleColNum) {
         case 16:
            strcpy(ch, "HQ");
            break;
         case 8:
            strcpy(ch, "Q");
            break;
         case 4:
            strcpy(ch, "");
            break;
         default:
            strcpy(ch, "L");
            break;
         }
         _snprintf(buf, buflen, "%dx%s CSAA (%d,%d)", m_dcinfo.multiSampleNum, ch, m_dcinfo.multiSampleColNum, m_dcinfo.multiSampleCovNum);
      }
   } else {
      /* MSAA */
      if (m_dcinfo.multiSampleNum > 0)
         _snprintf(buf, buflen, "%dx MSAA", m_dcinfo.multiSampleNum);
      else
         _snprintf(buf, buflen, "No AA");
   }
   return buf;
}

/* Screen::getDC: get device context */
HDC Screen::getDC()
{
   return m_dcinfo.hDC;
}
