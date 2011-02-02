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

/* Dl_initialize: initialize dll */
void Dll_initialize(Dll *d)
{
   d->name = NULL;
   d->handle = NULL;
   d->enable = false;

   d->appStart = NULL;
   d->appEnd = NULL;
   d->procCommand = NULL;
   d->procEvent = NULL;
   d->update = NULL;
   d->render = NULL;

   d->next = NULL;
}

/* Dll_clear: free dll */
void Dll_clear(Dll *d)
{
   if (d->handle)
      ::FreeLibrary(d->handle);
   if (d->name)
      free(d->name);

   Dll_initialize(d);
}

/* Dll_load: load dll */
bool Dll_load(Dll *d, const char *dir, const char *file)
{
   char *buf;

   if(d == NULL || dir == NULL || file == NULL) return false;
   Dll_clear(d);

   /* open */
   buf = (char *) malloc(sizeof(char) * (MMDAgent_strlen(dir) + 1 + MMDAgent_strlen(file) + 1));
   sprintf(buf, "%s%c%s", dir, PLUGIN_DIRSEPARATOR, file);
   d->handle = ::LoadLibraryExA(buf, NULL, 0);
   free(buf);
   if (!d->handle)
      return false;

   /* set function pointers */
   d->appStart = (void (__stdcall *)(MMDAgent *)) ::GetProcAddress(d->handle, "extAppStart");
   d->appEnd = (void (__stdcall *)(MMDAgent *)) ::GetProcAddress(d->handle, "extAppEnd");
   d->procCommand = (void (__stdcall *)(MMDAgent *, const char *, const char *)) ::GetProcAddress(d->handle, "extProcCommand");
   d->procEvent = (void (__stdcall *)(MMDAgent *, const char *, const char *)) ::GetProcAddress(d->handle, "extProcEvent");
   d->update = (void (__stdcall *)(MMDAgent *, double)) ::GetProcAddress(d->handle, "extUpdate");
   d->render = (void (__stdcall *)(MMDAgent *)) ::GetProcAddress(d->handle, "extRender");

   if (d->appStart || d->appEnd || d->procCommand || d->procEvent || d->update || d->render) {
      /* save file name */
      d->name = MMDAgent_strdup(file);
      d->enable = true;
      return true;
   } else {
      /* if none, exit */
      Dll_clear(d);
      return false;
   }
}

/* Plugin::initialize: initialize plugin list */
void Plugin::initialize()
{
   m_head = NULL;
   m_tail = NULL;
}

/* Plugin::clear: free plugin list */
void Plugin::clear()
{
   Dll *d1, *d2;

   for (d1 = m_head; d1; d1 = d2) {
      d2 = d1->next;
      Dll_clear(d1);
      free(d1);
   }

   initialize();
}

/* Plugin::Plugin: constructor */
Plugin::Plugin()
{
   initialize();
}

/* Plugin::~Plugin: destructor */
Plugin::~Plugin()
{
   clear();
}

/* Plugin::load: load all DLLs in a directory */
bool Plugin::load(const char *dir)
{
   WIN32_FIND_DATAA findData;
   HANDLE hFind;
   char *buf;
   bool ret = false;
   Dll *d;

   if(dir == NULL) return false;
   buf = (char *) malloc(sizeof(char) * (MMDAgent_strlen(dir) + 1 + MMDAgent_strlen(PLUGIN_DYNAMICLIBS) + 1));
   sprintf(buf, "%s%c%s", dir, PLUGIN_DIRSEPARATOR, PLUGIN_DYNAMICLIBS);

   /* search file */
   hFind = FindFirstFileA(buf, &findData);
   if (hFind == INVALID_HANDLE_VALUE) {
      free(buf);
      return ret;
   }

   /* add */
   do {
      d = (Dll *) malloc(sizeof(Dll));
      Dll_initialize(d);
      if (Dll_load(d, dir, findData.cFileName) == false) {
         free(d);
      } else {
         if (m_tail == NULL)
            m_head = d;
         else
            m_tail->next = d;
         m_tail = d;
         ret = true;
      }
   } while (FindNextFileA(hFind, &findData));

   /* end */
   FindClose(hFind);
   free(buf);

   return ret;
}

/* Plugin::execAppStart: run when application is start */
void Plugin::execAppStart(MMDAgent *mmdagent)
{
   Dll *d;

   for (d = m_head; d; d = d->next)
      if (d->enable && d->appStart)
         d->appStart(mmdagent);
}

/* Plugin::execAppEnd: run when application is end */
void Plugin::execAppEnd(MMDAgent *mmdagent)
{
   Dll *d;

   for (d = m_head; d; d = d->next)
      if (d->enable && d->appEnd)
         d->appEnd(mmdagent);
}

/* Plugin::execProcCommand: process command message */
void Plugin::execProcCommand(MMDAgent *mmdagent, const char *type, const char *args)
{
   Dll *d;

   for (d = m_head; d; d = d->next)
      if (d->enable && d->procCommand)
         d->procCommand(mmdagent, type, args);
}

/* Plugin::execProcEvent: process event message */
void Plugin::execProcEvent(MMDAgent *mmdagent, const char *type, const char *args)
{
   Dll *d;

   for (d = m_head; d; d = d->next)
      if (d->enable && d->procEvent)
         d->procEvent(mmdagent, type, args);
}

/* Plugin::execUpdate: run when motion is updated */
void Plugin::execUpdate(MMDAgent *mmdagent, double deltaFrame)
{
   Dll *d;

   for (d = m_head; d; d = d->next)
      if (d->enable && d->update)
         d->update(mmdagent, deltaFrame);
}

/* Plugin::execRender: run when scene is rendered */
void Plugin::execRender(MMDAgent *mmdagent)
{
   Dll *d;

   for (d = m_head; d; d = d->next)
      if (d->enable && d->render)
         d->render(mmdagent);
}
