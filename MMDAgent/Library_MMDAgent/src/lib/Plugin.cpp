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

#include "MMDAgent.h"

/* Plugin_initialize: initialize Plugin */
void Plugin_initialize(Plugin *p)
{
   p->name = NULL;
   p->handle = NULL;
   p->enable = false;

   p->appStart = NULL;
   p->appEnd = NULL;
   p->windowProc = NULL;
   p->update = NULL;
   p->render = NULL;

   p->next = NULL;
}

/* Plugin_clear: free Plugin */
void Plugin_clear(Plugin *p)
{
   if (p->handle)
      ::FreeLibrary(p->handle);
   if (p->name)
      free(p->name);

   Plugin_initialize(p);
}

/* Plugin_load: load DLL */
bool Plugin_load(Plugin *p, const char *dllDirName, const char *dllFileName)
{
   char *buf;

   if(p == NULL || dllDirName == NULL || dllFileName == NULL) return false;
   Plugin_clear(p);

   /* open DLL */
   buf = (char *) malloc(sizeof(char) * (MMDAgent_strlen(dllDirName) + 1 + MMDAgent_strlen(dllFileName) + 1));
   sprintf(buf, "%s\\%s", dllDirName, dllFileName);
   p->handle = ::LoadLibraryExA(buf, NULL, 0);
   free(buf);
   if (!p->handle)
      return false;

   /* set function pointers */
   p->appStart = (void (__stdcall *)(MMDAgent *)) ::GetProcAddress(p->handle, "extAppStart");
   p->appEnd = (void (__stdcall *)(MMDAgent *)) ::GetProcAddress(p->handle, "extAppEnd");
   p->windowProc = (void (__stdcall *)(MMDAgent *, HWND, UINT, WPARAM, LPARAM)) ::GetProcAddress(p->handle, "extWindowProc");
   p->update = (void (__stdcall *)(MMDAgent *, double)) ::GetProcAddress(p->handle, "extUpdate");
   p->render = (void (__stdcall *)(MMDAgent *)) ::GetProcAddress(p->handle, "extRender");

   if (p->appStart || p->appEnd || p->windowProc || p->update || p->render) {
      /* save file name */
      p->name = MMDAgent_strdup(dllFileName);
      p->enable = true;
      return true;
   } else {
      /* if none, exit */
      Plugin_clear(p);
      return false;
   }
}

/* PluginList::initialize: initialize plugin list */
void PluginList::initialize()
{
   m_head = NULL;
   m_tail = NULL;
   m_numPlugin = 0;
}

/* PluginList::clear: free plugin list */
void PluginList::clear()
{
   Plugin *p1, *p2;

   for (p1 = m_head; p1; p1 = p2) {
      p2 = p1->next;
      Plugin_clear(p1);
      free(p1);
   }
   initialize();
}

/* PluginList::PluginList: constructor */
PluginList::PluginList()
{
   initialize();
}

/* PluginList::~PluginList: destructor */
PluginList::~PluginList()
{
   clear();
}

/* PluginList::load: load all DLLs in a directory */
bool PluginList::load(const char *dir)
{
   WIN32_FIND_DATAA findData;
   HANDLE hFind;
   char *buf;
   bool ret = false;
   Plugin *p;

   if(dir == NULL) return false;
   buf = (char *) malloc(sizeof(char) * (MMDAgent_strlen(dir) + 1 + MMDAgent_strlen(PLUGIN_DYNAMICLIBS) + 1));
   sprintf(buf, "%s%c%s", dir, PLUGIN_DIRSEPARATOR, PLUGIN_DYNAMICLIBS);

   /* search file */
   hFind = FindFirstFileA(buf, &findData);
   if (hFind == INVALID_HANDLE_VALUE) {
      free(buf);
      return ret;
   }

   /* add DLL */
   do {
      p = (Plugin *) malloc(sizeof(Plugin));
      Plugin_initialize(p);
      if (Plugin_load(p, dir, findData.cFileName) == false) {
         free(p);
      } else {
         if (m_tail == NULL)
            m_head = p;
         else
            m_tail->next = p;
         m_tail = p;
         m_numPlugin++;
         ret = true;
      }
   } while (FindNextFileA(hFind, &findData));

   /* end */
   FindClose(hFind);
   free(buf);

   return ret;
}

/* PluginList::execAppStart: run when application is start */
void PluginList::execAppStart(MMDAgent *mmdagent)
{
   Plugin *p;

   for (p = m_head; p; p = p->next)
      if (p->enable && p->appStart)
         p->appStart(mmdagent);
}

/* PluginList::execAppEnd: run when application is end */
void PluginList::execAppEnd(MMDAgent *mmdagent)
{
   Plugin *p;

   for (p = m_head; p; p = p->next)
      if (p->enable && p->appEnd)
         p->appEnd(mmdagent);
}

/* PluginList::execWindowProc: receive window message */
void PluginList::execWindowProc(MMDAgent *mmdagent, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   Plugin *p;

   for (p = m_head; p; p = p->next)
      if (p->enable && p->windowProc)
         p->windowProc(mmdagent, hWnd, message, wParam, lParam);
}

/* PluginList::execUpdate: */
void PluginList::execUpdate(MMDAgent *mmdagent, double deltaFrame)
{
   Plugin *p;

   for (p = m_head; p; p = p->next)
      if (p->enable && p->update)
         p->update(mmdagent, deltaFrame);
}

/* PluginList::execRender: */
void PluginList::execRender(MMDAgent *mmdagent)
{
   Plugin *p;

   for (p = m_head; p; p = p->next)
      if (p->enable && p->render)
         p->render(mmdagent);
}
