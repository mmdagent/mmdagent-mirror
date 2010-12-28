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
#include "GLee.h"

/* TextRenderer::initialize: initialize text renderer */
void TextRenderer::initialize()
{
   m_hDC = NULL;
   m_outlineFont = NULL;
   m_outlineFontID = 0;
   m_bitmapFontID = 0;
   m_idNum = 0;
   m_current = 0;
}

/* TextRenderer::clear: clear text renderer */
void TextRenderer::clear()
{
   int i;

   for (i = 0; i < m_idNum; i++)
      glDeleteLists(m_codeList[i].id + m_outlineFontID, 1);
   if (m_bitmapFontID != 0)
      glDeleteLists(m_bitmapFontID, TEXTRENDERER_ASCIISIZE);
   if (m_outlineFontID != 0)
      glDeleteLists(m_outlineFontID, TEXTRENDERER_ASCIISIZE);
   if (m_outlineFont)
      DeleteObject(m_outlineFont);

   initialize();
}

/* TextRenderer::TextRenderer: constructor */
TextRenderer::TextRenderer()
{
   initialize();
}

/* TextRender:: destructor */
TextRenderer::~TextRenderer()
{
   clear();
}

/* TextRenderer::setup: initialize and setup text renderer */
void TextRenderer::setup(HDC hDC, wchar_t *fontName)
{
   HGDIOBJ oldfont;
   GLYPHMETRICSFLOAT gmf[TEXTRENDERER_ASCIISIZE];

   clear();

   /* store device context */
   m_hDC = hDC;

   /* set TEXTRENDERER_ASCIISIZE bitmap font for ASCII */
   m_bitmapFontID = glGenLists(TEXTRENDERER_ASCIISIZE);
   if (m_bitmapFontID == 0)
      return;

   /* get system font */
   oldfont = SelectObject(m_hDC, GetStockObject(SYSTEM_FONT));
   if (!oldfont) {
      glDeleteLists(m_bitmapFontID, TEXTRENDERER_ASCIISIZE);
      m_bitmapFontID = 0;
      return;
   }
   if (wglUseFontBitmaps(m_hDC, 0, TEXTRENDERER_ASCIISIZE, m_bitmapFontID) == false) {
      glDeleteLists(m_bitmapFontID, TEXTRENDERER_ASCIISIZE);
      m_bitmapFontID = 0;
      return;
   }
   SelectObject(m_hDC, oldfont);

   /* set TEXTRENDERER_ASCIISIZE outline font for ASCII */
   m_outlineFontID = glGenLists(TEXTRENDERER_ASCIISIZE);
   if (m_outlineFontID == 0)
      return;

   /* get outline font */
   m_outlineFont = CreateFontW(30, 0, 0, 0, FW_NORMAL, false, false, false,
                               DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                               fontName);
   if (!m_outlineFont) {
      glDeleteLists(m_outlineFontID, TEXTRENDERER_ASCIISIZE);
      m_outlineFontID = 0;
      return;
   }

   oldfont = SelectObject(m_hDC, m_outlineFont);
   if (!oldfont) {
      glDeleteLists(m_outlineFontID, TEXTRENDERER_ASCIISIZE);
      m_outlineFontID = 0;
      return;
   }
   if (wglUseFontOutlinesW(m_hDC, 0, TEXTRENDERER_ASCIISIZE, m_outlineFontID, 0.0f, 0.1f, WGL_FONT_POLYGONS, gmf) == false) {
      glDeleteLists(m_outlineFontID, TEXTRENDERER_ASCIISIZE);
      m_outlineFontID = 0;
      return;
   }
   SelectObject(m_hDC, oldfont);
}

/* TextRenderer::drawAsciiStringBitmap: draw ascii string (bitmap, multi-byte char) */
void TextRenderer::drawAsciiStringBitmap(char *str)
{
   GLsizei size = strlen(str);

   glListBase(m_bitmapFontID);
   glCallLists(size, GL_UNSIGNED_BYTE, (const GLvoid*) str);
}

/* TextRenderer::drawAsciiStringBitmap: draw ascii string (bitmap, wide char) */
void TextRenderer::drawAsciiStringBitmap(wchar_t *wstr)
{
   size_t converted;
   char buf[TEXTRENDERER_MAXBUFLEN];
   GLsizei size;

   wcstombs_s(&converted, buf, TEXTRENDERER_MAXBUFLEN, wstr, _TRUNCATE);
   size = strlen(buf);
   glListBase(m_bitmapFontID);
   glCallLists(size, GL_UNSIGNED_BYTE, (const GLvoid*) buf);
}

/* TextRenderer::drawAsciiStringOutline: draw ascii string (outline, multi-byte char) */
void TextRenderer::drawAsciiStringOutline(char *str)
{
   GLsizei size = strlen(str);

   glListBase(m_outlineFontID);
   glCallLists(size, GL_UNSIGNED_BYTE, (const GLvoid*) str);
   glFrontFace(GL_CCW);
}

/* TextRenderer::drawAsciiStringOutline: draw ascii string (outline, wide char) */
void TextRenderer::drawAsciiStringOutline(wchar_t *wstr)
{
   size_t converted;
   char buf[TEXTRENDERER_MAXBUFLEN];
   GLsizei size;

   wcstombs_s(&converted, buf, TEXTRENDERER_MAXBUFLEN, wstr, _TRUNCATE);
   size = strlen(buf);
   glListBase(m_outlineFontID);
   glCallLists(size, GL_UNSIGNED_BYTE, (const GLvoid*) buf);
   glFrontFace(GL_CCW);
}

/* TextRenderer::getDisplayListArrayOfString: get array of display list indices Draw any string (outline, wide char, slow) */
int TextRenderer::getDisplayListArrayOfString(wchar_t *wstr, unsigned int *idList, int maxlen)
{
   size_t i;
   int n = 0;
   unsigned int id;
   int j;
   char buf[10];
   size_t len = wcslen(wstr);
   GLYPHMETRICSFLOAT gmf;
   HGDIOBJ oldfont;

   for (i = 0; i < len; i++) {
      if (n >= maxlen) return maxlen; /* overflow */
      if (iswascii(wstr[i])) {
         /* ascii, use display list whose id number equals to ascii code */
         wctomb(buf, wstr[i]);
         idList[n] = (unsigned int) buf[0];
         n++;
      } else {
         /* non-ascii look for already allocated display lists */
         /* search from latest to oldest */
         for (j = m_current; j >= 0; j--) {
            if (m_codeList[j].code == wstr[i]) {
               id = m_codeList[j].id;
               break;
            }
         }
         if (j < 0) {
            for (j = m_idNum - 1; j >= m_current; j--) {
               if (m_codeList[j].code == wstr[i])
                  id = m_codeList[j].id;
               break;
            }
            if (j < m_current) {
               /* not found, assign new display id */
               id = glGenLists(1);
               if (id == 0)
                  continue;
               /* get font outline to the display list */
               oldfont = SelectObject(m_hDC, m_outlineFont);
               if (!oldfont)
                  return -1;
               if (wglUseFontOutlinesW(m_hDC, wstr[i], 1, id, 0.0f, 0.1f, WGL_FONT_POLYGONS, &gmf) == false)
                  continue;
               SelectObject(m_hDC, oldfont);
               /* make id as relative to base id */
               id -= m_outlineFontID;
               /* move latest pointer forward and store */
               m_current++;
               if (m_current >= m_idNum) {
                  /* if room, extend max, else, pointer will be cycled */
                  if (m_idNum < TEXTRENDERER_MAXNCHAR)
                     m_idNum++;
                  else
                     m_current = 0;
               }
               m_codeList[m_current].code = wstr[i];
               m_codeList[m_current].id = id;
            }
         }
         idList[n] = id;
         n++;
      }
   }
   return n;
}

/* TextRenderer::renderSispayListArrayOfString: render the obtained array of display lists for a string */
void TextRenderer::renderDisplayListArrayOfString(unsigned int *idList, int n)
{
   glListBase(m_outlineFontID);
   glCallLists((GLsizei) n, GL_UNSIGNED_INT, (const GLvoid*) idList);
   glFrontFace(GL_CCW);
}

/* TextRenderer::drawString: draw any string (outline, wide char, slow) */
void TextRenderer::drawString(wchar_t *wstr)
{
   unsigned int idList[TEXTRENDERER_MAXBUFLEN];
   int n;

   n = getDisplayListArrayOfString(wstr, idList, TEXTRENDERER_MAXBUFLEN);

   if (n != -1) renderDisplayListArrayOfString(idList, n);
}
