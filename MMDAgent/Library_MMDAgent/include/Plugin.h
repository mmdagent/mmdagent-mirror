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

/* definitions */

#define PLUGIN_DIRSEPARATOR '\\'
#define PLUGIN_DYNAMICLIBS  "*.dll"

/* Dll: dll for MMDAgent */
typedef struct _Dll {
   char *name;
   HINSTANCE handle;
   bool enable;

   void (__stdcall *appStart)(MMDAgent *mmdagent);
   void (__stdcall *appEnd)(MMDAgent *mmdagent);
   void (__stdcall *windowProc)(MMDAgent *mmdagent, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
   void (__stdcall *update)(MMDAgent *mmdagent, double deltaFrame);
   void (__stdcall *render)(MMDAgent *mmdagent);

   struct _Dll *next;
} Dll;

/* Dll_initialize: initialize dll */
void Dll_initialize(Dll *d);

/* Dll_clear: free plugin */
void Dll_clear(Dll *d);

/* Dll_load: load dll */
bool Dll_load(Dll *d, const char *dir, const char *file);

/* Plugin: plugin list */
class Plugin
{
private:

   Dll *m_head;
   Dll *m_tail;
   int m_numPlugin;

   /* initialize: initialize plugin list */
   void initialize();

   /* clear: free plugin list */
   void clear();

public:

   /* Plugin: constructor */
   Plugin();

   /* ~Plugin: destructor */
   ~Plugin();

   /* load: load all DLLs in a directory */
   bool load(const char *dir);

   /* execAppStart: run when application is start */
   void execAppStart(MMDAgent *mmdagent);

   /* execAppEnd: run when application is end */
   void execAppEnd(MMDAgent *mmdagent);

   /* execWindowProc: receive window message */
   void execWindowProc(MMDAgent *mmdagent, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

   /* execUpdate: */
   void execUpdate(MMDAgent *mmdagent, double deltaFrame);

   /* execRender: */
   void execRender(MMDAgent *mmdagent);
};
