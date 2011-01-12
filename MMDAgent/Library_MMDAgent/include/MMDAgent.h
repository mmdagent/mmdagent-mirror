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
#include "Timer.h"
#include "PMDObject.h"
#include "MotionStocker.h"
#include "Stage.h"
#include "Plugin.h"
#include "TextRenderer.h"

#define MAXMODEL  20    /* maximum number of model */
#define MODEL_ALL 65535 /* alias for all model */
#define MMDAGENT_MAXDISPSTRLEN 1024

#define MMDAGENT_MAXBUFLEN    1024
#define MMDAGENT_DIRSEPARATOR '\\'

#define MMDAGENT_SYSDATADIR "AppData"
#define MMDAGENT_PLUGINDIR  "Plugins"

#define WM_MMDAGENT_EVENT   (WM_USER + 1) /* window index for output event message */
#define WM_MMDAGENT_COMMAND (WM_USER + 2) /* window index for input command message */

/* MMDAgent: MMDAgent class */
class MMDAgent
{
private:

   char *m_configFileName; /* config file name */
   char *m_configDirName;  /* directory name of config file */
   char *m_appDirName;     /* directory name of application data */

   HWND m_hWnd;       /* window handle */
   HINSTANCE m_hInst; /* application instance */

   Option m_option;         /* user options */
   BulletPhysics m_bullet;  /* Bullet Physics */
   Screen *m_screen;        /* screen */
   Stage *m_stage;          /* stage */
   SystemTexture *m_systex; /* system texture */
   Render *m_render;        /* render */
   Timer m_timer;           /* timer */
   TextRenderer m_text;     /* text render */
   PluginList m_plugin;     /* plugins */

   PMDObject m_model[MAXMODEL]; /* models */
   short m_numModel;            /* number of models */
   MotionStocker m_motion;      /* motions */

   bool m_keyCtrl;           /* true if Ctrl-key is on */
   bool m_keyShift;          /* true if Shift-key is on */
   int m_selectedModel;      /* model ID selected by mouse */
   bool m_doubleClicked;     /* true if double clicked */
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
   bool addMotion(char *modelAlias, char *motionAlias, char *fileName, bool full, bool once, bool enableSmooth, bool enableRePos);

   /* changeMotion: change motion */
   bool changeMotion(char *modelAlias, char *motionAlias, char *fileName);

   /* deleteMotion: delete motion */
   bool deleteMotion(char *modelAlias, char *motionAlias);

   /* addModel: add model */
   bool addModel(char *modelAlias, char *fileName, btVector3 *pos, btQuaternion *rot, char *baseModelAlias, char *baseBoneName);

   /* changeModel: change model */
   bool changeModel(char *modelAlias, char *fileName);

   /* deleteModel: delete model */
   bool deleteModel(char *modelAlias);

   /* setFloor: set floor image */
   bool setFloor(char *fileName);

   /* setBackground: set background image */
   bool setBackground(char *fileName);

   /* setStage: set stage */
   bool setStage(char *fileName);

   /* changeLightDirection: change light direction */
   bool changeLightDirection(float x, float y, float z);

   /* changeLightColor: change light color */
   bool changeLightColor(float r, float g, float b);

   /* startMove: start moving */
   bool startMove(char *modelAlias, btVector3 *pos, bool local, float speed);

   /* stopMove: stop moving */
   bool stopMove(char *modelAlias);

   /* startRotation: start rotation */
   bool startRotation(char *modelAlias, btQuaternion *rot, bool local, float spped);

   /* stopRotation: stop rotation */
   bool stopRotation(char *modelAlias);

   /* startTurn: start turn */
   bool startTurn(char *modelAlias, btVector3 *pos, bool local, float speed);

   /* stopTurn: stop turn */
   bool stopTurn(char *modelAlias);

   /* startLipSync: start lip sync */
   bool startLipSync(char *modelAlias, char *seq);

   /* stopLipSync: stop lip sync */
   bool stopLipSync(char *modelAlias);

   /* command: decode command string from client and call process */
   bool command(char *command, char *wstr);

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
   void sendCommandMessage(char *type, const char *format, ...);

   /* sendEventMessage: send event message */
   void sendEventMessage(char *type, const char *format, ...);

   /* setup: initialize and setup MMDAgent */
   HWND setup(HINSTANCE hInstance, TCHAR *szTitle, TCHAR *szWindowClass, int argc, char **argv);

   void procWindowCreateMessage(HWND hWnd);
   void procWindowDestroyMessage();
   void procMouseLeftButtonDoubleClickMessage(int x, int y);
   void procMouseLeftButtonDownMessage(int x, int y, bool withCtrl, bool withShift);
   void procMouseLeftButtonUpMessage();
   void procMouseWheel(bool zoomup, bool withCtrl, bool withShift);
   void procMouseMove(int x, int y, bool withCtrl, bool withShift);
   void procMouseRightButtonDownMessage();

   void procFullScreenMessage();
   void procInfoStringMessage();
   void procVSyncMessage();
   void procShadowMappingMessage();
   void procShadowMappingOrderMessage();
   void procDisplayRigidBodyMessage();
   void procDisplayWireMessage();
   void procDisplayBoneMessage();
   void procCartoonEdgeMessage(bool plus);
   void procTimeAdjustMessage(bool plus);
   void procHorizontalRotateMessage(bool right);
   void procVerticalRotateMessage(bool up);
   void procHorizontalMoveMessage(bool right);
   void procVerticalMoveMessage(bool up);
   void procDeleteModelMessage();
   void procPhysicsMessage();
   void procDisplayLogMessage();
   void procWindowSizeMessage(int x, int y);
   void procCommandMessage(char *mes1, char *mes2);
   void procEventMessage(char *mes1, char *mes2);

   void procPluginMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

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

   /* getWindowHandler: get window handle */
   HWND getWindowHandler();

   /* getInstance: get application instance */
   HINSTANCE getInstance();

   /* findModelAlias: find a model with the specified alias */
   int findModelAlias(char *alias);

   /* procDropFileMessage: handle file drops */
   void procDropFileMessage(HWND hWnd, WPARAM wParam, LPARAM lParam);

   /* getConfigFileName: get config file name for plugin */
   inline char *getConfigFileName() {
      return m_configFileName;
   }

   /* getConfigDirName: get directory of config file for plugin */
   inline char *getConfigDirName() {
      return m_configDirName;
   }

   /* getAppDirName: get application directory name for plugin */
   inline char *getAppDirName() {
      return m_appDirName;
   }
};

#endif /* __mmdagent_h__ */
