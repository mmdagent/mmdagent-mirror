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

#ifndef __mmdagent_plugin_h__
#define __mmdagent_plugin_h__

#define PLUGIN_DIRSEPARATOR '\\'
#define PLUGIN_DYNAMICLIBS  "*.dll"

class MMDAgent;

/* Plugin: plugin for MMDAgent */
typedef struct _Plugin {
   char *name;
   HINSTANCE handle;
   bool enable;

   void (__stdcall *appStart)(MMDAgent *mmdagent);
   void (__stdcall *appEnd)(MMDAgent *mmdagent);
   void (__stdcall *windowCreate)(MMDAgent *mmdagent, HWND hWnd);
   void (__stdcall *windowProc)(MMDAgent *mmdagent, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
   void (__stdcall *update)(MMDAgent *mmdagent, double deltaFrame);
   void (__stdcall *render)(MMDAgent *mmdagent);

   struct _Plugin *next;
} Plugin;

/* Plugin_initialize: initialize plugin */
void Plugin_initialize(Plugin *p);

/* Plugin_clear: free plugin */
void Plugin_clear(Plugin *p);

/* Plugin_load: load DLL */
bool Plugin_load(Plugin *p, const char *dllDirName, const char *dllFileName);

/* PluginList: plugin list */
class PluginList
{
private:

   Plugin *m_head;
   Plugin *m_tail;
   int m_numPlugin;

   /* initialize: initialize plugin list */
   void initialize();

   /* clear: free plugin list */
   void clear();

public:

   /* PluginList: constructor */
   PluginList();

   /* ~PluginList: destructor */
   ~PluginList();

   /* load: load all DLLs in a directory */
   bool load(const char *dir);

   /* addPlugin: add internal plugin */
   bool addPlugin(char *name, void (__stdcall *appStart)(MMDAgent *), void (__stdcall *appEnd)(MMDAgent *), void (__stdcall *windowCreate)(MMDAgent *, HWND), void (__stdcall *windowProc)(MMDAgent *, HWND, UINT, WPARAM, LPARAM), void (__stdcall *update)(MMDAgent *, double), void (__stdcall *render)(MMDAgent *));

   /* execAppStart: run when application is start */
   void execAppStart(MMDAgent *mmdagent);

   /* execAppEnd: run when application is end */
   void execAppEnd(MMDAgent *mmdagent);

   /* execWindowCreate: run when window is created */
   void execWindowCreate(MMDAgent *mmdagent, HWND hWnd);

   /* execWindowProc: receive window message */
   void execWindowProc(MMDAgent *mmdagent, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

   /* execUpdate: */
   void execUpdate(MMDAgent *mmdagent, double deltaFrame);

   /* execRender: */
   void execRender(MMDAgent *mmdagent);
};

#endif /* __mmdagent_plugin_h__ */
