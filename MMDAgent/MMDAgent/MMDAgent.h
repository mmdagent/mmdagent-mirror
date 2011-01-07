/* ----------------------------------------------------------------- */
/*           Toolkit for Building Voice Interaction Systems          */
/*           MMDAgent developed by MMDAgent Project Team             */
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

#ifndef __mmdagent_h__
#define __mmdagent_h__

#include <stdarg.h>

#include "Screen.h"
#include "Render.h"
#include "BulletPhysics.h"
#include "Timer.h"
#include "PMDObject.h"
#include "MotionStocker.h"
#include "Stage.h"
#include "Audio.h"
#include "Plugin.h"
#include "TextRenderer.h"

#define MAXMODEL  20    /* maximum number of model */
#define MODEL_ALL 65535 /* alias for all model */
#define MMDAGENT_MAXDISPSTRLEN 1024

#define MMDAGENT_MAXBUFLEN 1024

#define MMDAGENT_SYSDATADIR L"AppData"
#define MMDAGENT_PLUGINDIR  L"Plugins"

#define WM_MMDAGENT_EVENT   (WM_USER + 1) /* window index for output event message */
#define WM_MMDAGENT_COMMAND (WM_USER + 2) /* window index for input command message */

/* MMDAgent: MMDAgent class */
class MMDAgent
{
private:

   wchar_t m_configFileName[MAX_PATH]; /* config file name */
   wchar_t m_configDirName[MAX_PATH];  /* directory name of config file */
   wchar_t m_appDirName[MAX_PATH];     /* directory name of application data */

   HWND m_hWnd;       /* window handle */
   HINSTANCE m_hInst; /* application instance */

   Option m_option;         /* user options */
   BulletPhysics m_bullet;  /* Bullet Physics */
   Screen *m_screen;        /* screen */
   Stage *m_stage;          /* stage */
   SystemTexture *m_systex; /* system texture */
   Render *m_render;        /* render */
   Timer m_timer;           /* timer */
   Audio m_audio;           /* audio */
   TextRenderer m_text;     /* text render */
   PluginList m_plugin;     /* plugins */

   PMDObject m_model[MAXMODEL]; /* models */
   short m_numModel;            /* number of models */
   MotionStocker m_motion;      /* motions */

   bool m_keyCtrl;                 /* true if Ctrl-key is on */
   bool m_keyShift;                /* true if Shift-key is on */
   int m_selectedModel;            /* model ID selected by mouse */
   bool m_doubleClicked;           /* true if double clicked */
   POINT m_mousepos;
   bool m_leftButtonPressed;

   bool m_enablePhysicsSimulation; /* true if physics simulation is on */
   bool m_dispLog;                 /* true if log window is shown */
   bool m_dispBulletBodyFlag;      /* true if bullet body is shown */
   bool m_dispModelDebug;          /* true if model debugger is on */

   double m_dispFrameAdjust;         /* display time for audio time adjustment */
   double m_dispFrameCue;            /* display time for audio time difference */
   double m_dispModelMove[MAXMODEL]; /* display time for model position when model is moving */

   /* getNewModelId: return new model ID */
   int getNewModelId();

   /* removeRelatedModels: delete a model */
   void removeRelatedModels(int modelId);

   /* updateWindowSize: change OpenGL campus size to current window size */
   void updateWindowSize(HWND hWnd);

   /* updateLight: update light */
   void updateLight();

   /* addMotion: add motion */
   bool addMotion(wchar_t *modelAlias, char *motionAlias, wchar_t *fileName, bool full, bool once, bool enableSmooth, bool enableRePos);

   /* changeMotion: change motion */
   bool changeMotion(wchar_t *modelAlias, char *motionAlias, wchar_t *fileName);

   /* deleteMotion: delete motion */
   bool deleteMotion(wchar_t *modelAlias, char *motionAlias);

   /* addModel: add model */
   bool addModel(wchar_t *modelAlias, wchar_t *fileName, btVector3 *pos, btQuaternion *rot, wchar_t *baseModelAlias, wchar_t *baseBoneName);

   /* changeModel: change model */
   bool changeModel(wchar_t *modelAlias, wchar_t *fileName);

   /* deleteModel: delete model */
   bool deleteModel(wchar_t *modelAlias);

   /* setFloor: set floor image */
   bool setFloor(wchar_t *fileName);

   /* setBackground: set background image */
   bool setBackground(wchar_t *fileName);

   /* setStage: set stage */
   bool setStage(wchar_t *fileName);

   /* startSound: start sound */
   bool startSound(wchar_t *soundAlias, wchar_t *fileName, bool adjust);

   /* stopSound: stop sound */
   bool stopSound(wchar_t *soundAlias);

   /* changeLightDirection: change light direction */
   bool changeLightDirection(float x, float y, float z);

   /* changeLightColor: change light color */
   bool changeLightColor(float r, float g, float b);

   /* startMove: start moving */
   bool startMove(wchar_t *modelAlias, btVector3 *pos, bool local, float speed);

   /* stopMove: stop moving */
   bool stopMove(wchar_t *modelAlias);

   /* startRotation: start rotation */
   bool startRotation(wchar_t *modelAlias, btQuaternion *rot, bool local, float spped);

   /* stopRotation: stop rotation */
   bool stopRotation(wchar_t *modelAlias);

   /* startTurn: start turn */
   bool startTurn(wchar_t *modelAlias, btVector3 *pos, bool local, float speed);

   /* stopTurn: stop turn */
   bool stopTurn(wchar_t *modelAlias);

   /* startLipSync: start lip sync */
   bool startLipSync(wchar_t *modelAlias, wchar_t *seq);

   /* stopLipSync: stop lip sync */
   bool stopLipSync(wchar_t *modelAlias);

   /* initialize: initialize MMDAgent */
   void initialize();

   /* clear: free MMDAgent */
   void clear();

public:

   /* MMDAgent: constructor */
   MMDAgent();

   /* ~MMDAgent: destructor */
   ~MMDAgent();

   /* release: free MMDAgent */
   void release();

   /* updateScene: update the whole scene */
   void updateScene();

   /* renderScene: render the whole scene */
   void renderScene(HWND hWnd);

   /* sendCommandMessage: send command message */
   void sendCommandMessage(wchar_t *commandType, const wchar_t *argFormat, ...);

   /* sendEventMessage: send event message */
   void sendEventMessage(wchar_t *eventType, const wchar_t *argFormat, ...);

   /* setup: initialize and setup MMDAgent */
   HWND setup(HINSTANCE hInstance, TCHAR *szTitle, TCHAR *szWindowClass, int argc, wchar_t **argv);

   /* procMessage: process message */
   bool procMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

   /* getMoelList: get model list */
   PMDObject *getModelList();

   /* getNumModel: get number of models */
   short getNumModel();

   Option *getOption();

   /* getRender: get render */
   Render *getRender();

   /* getStage: get stage */
   Stage *getStage();

   /* getTimer: get timer */
   Timer *getTimer();

   /* getAudio: get audio */
   Audio *getAudio();

   /* getWindowHandler: get window handle */
   HWND getWindowHandler();

   /* getInstance: get application instance */
   HINSTANCE getInstance();

   /* findModelAlias: find a model with the specified alias */
   int findModelAlias(wchar_t *alias);

   /* command: decode command string from client and call process */
   bool command(wchar_t *command, wchar_t *wstr);

   /* dropFile: handle file drops */
   void dropFile(HWND hWnd, WPARAM wParam, LPARAM lParam);

   /* getConfigFileName: get config file name for plugin */
   inline wchar_t *getConfigFileName() {
      return m_configFileName;
   }

   /* getConfigDirName: get directory of config file for plugin */
   inline wchar_t *getConfigDirName() {
      return m_configDirName;
   }

   /* getAppDirName: get application directory name for plugin */
   inline wchar_t *getAppDirName() {
      return m_appDirName;
   }
};

#endif /* __mmdagent_h__ */
