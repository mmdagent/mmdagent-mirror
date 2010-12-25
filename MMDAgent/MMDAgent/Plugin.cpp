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

#include <windows.h>

#include "TextRenderer.h"
#include "UserOption.h"
#include "Plugin.h"
#include "utils.h"

/* Plugin_initialize: initialize Plugin */
void Plugin_initialize(Plugin *p)
{
   p->name = NULL;
   p->handle = NULL;
   p->enable = false;

   p->appStart = NULL;
   p->appEnd = NULL;
   p->windowCreate = NULL;
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
bool Plugin_load(Plugin *p, const wchar_t *dllDirName, const wchar_t *dllFileName)
{
   wchar_t *buf;

   Plugin_clear(p);

   /* open DLL */
   buf = (wchar_t *) malloc(sizeof(wchar_t) * (wcslen(dllDirName) + 1 + wcslen(dllFileName) + 1));
   wcscpy(buf, dllDirName);
   wcscat(buf, L"\\");
   wcscat(buf, dllFileName);
   p->handle = ::LoadLibraryEx(buf, NULL, 0);
   if (!p->handle) {
      g_logger.log(L"! Error: Plugin: failed to load library \"%s\"", dllFileName);
      free(buf);
      return false;
   }
   free(buf);

   /* set function pointers */
   p->appStart = (void (__stdcall *)(MMDAgent *)) ::GetProcAddress(p->handle, "extAppStart");
   p->appEnd = (void (__stdcall *)(MMDAgent *)) ::GetProcAddress(p->handle, "extAppEnd");
   p->windowCreate = (void (__stdcall *)(MMDAgent *, HWND)) ::GetProcAddress(p->handle, "extWindowCreate");
   p->windowProc = (void (__stdcall *)(MMDAgent *, HWND, UINT, WPARAM, LPARAM)) ::GetProcAddress(p->handle, "extWindowProc");
   p->update = (void (__stdcall *)(MMDAgent *, double)) ::GetProcAddress(p->handle, "extUpdate");
   p->render = (void (__stdcall *)(MMDAgent *)) ::GetProcAddress(p->handle, "extRender");

   if (p->appStart || p->appEnd || p->windowCreate || p->windowProc || p->update || p->render) {
      /* save file name */
      g_logger.log(L"Plugin: \"%s\" loaded", dllFileName);
      p->name = (wchar_t *) malloc(sizeof(wchar_t) * (wcslen(dllFileName) + 1));
      wcscpy(p->name, dllFileName);
      p->enable = true;
      return true;
   } else {
      /* if none, exit */
      g_logger.log(L"Plugin: \"%s\" has no ext function, skipped", dllFileName);
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
bool PluginList::load(const wchar_t *dirName)
{
   WIN32_FIND_DATA findData;
   HANDLE hFind;
   wchar_t *buf;
   bool ret = false;
   Plugin *p;

   buf = (wchar_t *) malloc(sizeof(wchar_t) * (wcslen(dirName) + 7));
   wcscpy(buf, dirName);
   wcscat(buf, L"\\*.dll");

   /* search file */
   hFind = FindFirstFile(buf, &findData);
   if (hFind == INVALID_HANDLE_VALUE) {
      g_logger.log(L"! Error: Plugin: unable to open dir \"%s\"", dirName);
      free(buf);
      return ret;
   }

   /* add DLL */
   do {
      p = (Plugin *) malloc(sizeof(Plugin));
      Plugin_initialize(p);
      if (Plugin_load(p, dirName, findData.cFileName) == false) {
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
   } while (FindNextFile(hFind, &findData));

   /* end */
   FindClose(hFind);
   free(buf);

   return ret;
}

/* PluginList::addPlugin: add internal plugin */
bool PluginList::addPlugin(wchar_t *name, void (__stdcall *appStart)(MMDAgent *), void (__stdcall *appEnd)(MMDAgent *), void (__stdcall *windowCreate)(MMDAgent *, HWND), void (__stdcall *windowProc)(MMDAgent *, HWND, UINT, WPARAM, LPARAM), void (__stdcall *update)(MMDAgent *, double), void (__stdcall *render)(MMDAgent *))
{
   Plugin *p;

   /* check */
   if (name == NULL)
      return false;
   if (wcslen(name) <= 0)
      return false;
   if (!appStart && !appEnd && !windowCreate && !windowProc && !update && !render)
      return false;

   /* create */
   p = (Plugin *) malloc(sizeof(Plugin));
   Plugin_initialize(p);
   p->name = (wchar_t *) malloc(sizeof(wchar_t) * (wcslen(name) + 1));
   wcscpy(p->name, name);
   p->appStart = appStart;
   p->appEnd = appEnd;
   p->windowCreate = windowCreate;
   p->windowProc = windowProc;
   p->update = update;
   p->render = render;
   p->enable = true;

   /* add */
   if (m_tail == NULL)
      m_head = p;
   else
      m_tail->next = p;
   m_tail = p;
   m_numPlugin++;

   return true;
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

/* PluginList:execWindowCreate: run when window is created */
void PluginList::execWindowCreate(MMDAgent *mmdagent, HWND hWnd)
{
   Plugin *p;

   for (p = m_head; p; p = p->next)
      if (p->enable && p->windowCreate)
         p->windowCreate(mmdagent, hWnd);
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
