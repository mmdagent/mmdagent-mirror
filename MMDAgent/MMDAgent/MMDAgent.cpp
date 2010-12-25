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

/* headers */

#include <windows.h>
#include <locale.h>

#include "resource.h"
#include "MMDAgent.h"
#include "utils.h"
#include "UserOption.h"
#include "MMDAgent_command.h"

extern void initializeLookAt(MMDAgent *mmdagent, PluginList *pluginList);

/* MMDAgent::initialize: initialize MMDAgent */
void MMDAgent::initialize()
{
   int i;

   wcscpy(m_configFileName, L"");
   wcscpy(m_configDirName, L"");
   wcscpy(m_appDirName, L"");

   m_hWnd = 0;
   m_hInst = 0;

   m_screen = NULL;
   m_stage = NULL;
   m_systex = NULL;
   m_render = NULL;

   m_numModel = 0;

   m_keyCtrl = false;
   m_keyShift = false;
   m_selectedModel = -1;
   m_doubleClicked = false;
   m_mousepos.x = 0;
   m_mousepos.y = 0;
   m_leftButtonPressed = false;

   m_enablePhysicsSimulation = true;
   m_dispLog = false;
   m_dispBulletBodyFlag = false;
   m_dispModelDebug = false;

   m_dispFrameAdjust = 0.0;
   m_dispFrameCue = 0.0;
   for (i = 0; i < MAXMODEL; i++)
      m_dispModelMove[i] = 0.0;
}

/* MMDAgent::clear: free MMDAgent */
void MMDAgent::clear()
{
   int i;

   if (m_render)
      delete m_render;
   if (m_systex)
      delete m_systex;
   if (m_stage)
      delete m_stage;
   if (m_screen)
      delete m_screen;
   for (i = 0; i < m_numModel; i++) {
      if (m_model[i].isEnable())
         m_model[i].release();
   }

   initialize();
}

/* MMDAgent::MMDAgent: constructor */
MMDAgent::MMDAgent()
{
   initialize();
}

/* MMDAgent::~MMDAgent: destructor */
MMDAgent::~MMDAgent()
{
   release();
}

/* MMDAgent::release: free MMDAgent */
void MMDAgent::release()
{
   clear();
}

/* MMDAgent::getNewModelId: return new model ID */
int MMDAgent::getNewModelId()
{
   int i;

   for (i = 0; i < m_numModel; i++)
      if (m_model[i].isEnable() == false)
         return i; /* re-use it */

   if (m_numModel >= MAXMODEL)
      return -1; /* no more room */

   i = m_numModel;
   m_numModel++;

   m_model[i].setEnableFlag(false); /* model is not loaded yet */
   return i;
}

/* MMDAgent::removeRelatedModels: delete a model */
void MMDAgent::removeRelatedModels(int modelId)
{
   int i;

   /* remove assigned accessories */
   for (i = 0; i < m_numModel; i++)
      if (m_model[i].isEnable() == true && m_model[i].getAssignedModel() == &(m_model[modelId]))
         removeRelatedModels(i);

   /* remove model */
   m_model[modelId].deleteModel();
}

/* MMDAgent::updateScene: update the whole scene */
void MMDAgent::updateScene()
{
   int i, ite;
   double intervalFrame;
   int stepmax;
   double stepFrame;
   double restFrame;
   double procFrame;
   double adjustFrame;
   MotionPlayer *motionPlayer;

   /* get frame interval */
   intervalFrame = m_timer.getFrameInterval();

   stepmax = opt[CONF_BULLET_FPS].i;
   stepFrame = 30.0 / opt[CONF_BULLET_FPS].i;
   restFrame = intervalFrame;

   for (ite = 0; ite < stepmax; ite++) {
      if (restFrame <= stepFrame) {
         /* break */
         procFrame = restFrame;
         ite = stepmax;
      } else {
         /* add stepFrame */
         procFrame = stepFrame;
         restFrame -= stepFrame;
      }
      /* calculate adjustment time for audio */
      adjustFrame = m_timer.getAdditionalFrame(procFrame);
      if (adjustFrame != 0.0)
         m_dispFrameCue = 90.0;
      /* update motion */
      for (i = 0; i < m_numModel; i++) {
         if (m_model[i].isEnable() == false) continue;
         /* update root bone */
         m_model[i].updateRootBone();
         if (m_model[i].updateMotion(procFrame + adjustFrame)) {
            /* search end of motion */
            for (motionPlayer = m_model[i].getMotionManager()->getMotionPlayerList(); motionPlayer; motionPlayer = motionPlayer->next) {
               if (motionPlayer->statusFlag == MOTION_STATUS_DELETED) {
                  /* send event message */
                  if (wcscmp(motionPlayer->name, LIPSYNC_MOTION_NAME) == 0)
                     sendEventMessage(MMDAGENT_EVENT_LIPSYNC_STOP, L"%s", m_model[i].getAlias());
                  else
                     sendEventMessage(MMDAGENT_EVENT_MOTION_DELETE, L"%s|%s", m_model[i].getAlias(), motionPlayer->name);
               }
            }
         }
         /* update alpha for appear or disappear */
         if (m_model[i].updateAlpha(procFrame + adjustFrame))
            removeRelatedModels(i); /* remove model and accessories */
      }
      /* execute plugin */
      m_plugin.execUpdate(this, procFrame + adjustFrame);
      /* update bullet physics */
      m_bullet.update((float) procFrame);
   }
   /* update after simulation */
   for (i = 0; i < m_numModel; i++)
      if (m_model[i].isEnable() == true)
         m_model[i].updateAfterSimulation(m_enablePhysicsSimulation);

   /* calculate rendering range for shadow mapping */
   if (m_render->getShadowMapping())
      m_render->updateDepthTextureViewParam(m_model, m_numModel);

   /* decrement each indicator */
   if (m_dispFrameAdjust > 0.0)
      m_dispFrameAdjust -= intervalFrame;
   if (m_dispFrameCue > 0.0)
      m_dispFrameCue -= intervalFrame;
   for (i = 0; i < m_numModel; i++) {
      if (m_model[i].isEnable()) {
         if (m_model[i].isMoving()) {
            m_dispModelMove[i] = 15.0;
         } else if (m_dispModelMove[i] > 0.0) {
            m_dispModelMove[i] -= intervalFrame;
         }
      }
   }

   /* decrement mouse active time */
   m_screen->updateMouseActiveTime(intervalFrame);

   InvalidateRect(m_hWnd, NULL, false);
}


/* MMDAgent::renderScene: render the whole scene */
void MMDAgent::renderScene(HWND hWnd)
{
   int i;
   char buff[MMDAGENT_MAXDISPSTRLEN];
   btVector3 pos;
   float fps;

   if (!m_hWnd) return;

   /* update model position and rotation */
   fps = m_timer.getFps();
   for (i = 0; i < m_numModel; i++) {
      if (m_model[i].isEnable() == true) {
         if (m_model[i].updateModelRootOffset(fps))
            sendEventMessage(MMDAGENT_EVENT_MOVE_STOP, L"%s", m_model[i].getAlias());
         if (m_model[i].updateModelRootRotation(fps)) {
            if (m_model[i].isTurning()) {
               sendEventMessage(MMDAGENT_EVENT_TURN_STOP, L"%s", m_model[i].getAlias());
               m_model[i].setTurningFlag(false);
            } else {
               sendEventMessage(MMDAGENT_EVENT_ROTATE_STOP, L"%s", m_model[i].getAlias());
            }
         }
      }
   }

   /* render scene */
   m_render->render(this);

   /* show debug display */
   if (m_dispModelDebug)
      for (i = 0; i < m_numModel; i++)
         if (m_model[i].isEnable() == true)
            m_model[i].renderDebug(&m_text);

   /* show bullet body */
   if (m_dispBulletBodyFlag)
      m_bullet.debugDisplay();

   /* show log window */
   if (m_dispLog)
      g_logger.render(&m_text);

   /* count fps */
   m_timer.countFrame();

   /* show fps */
   if (opt[CONF_FPS_ENABLED].b) {
      _snprintf(buff, MMDAGENT_MAXDISPSTRLEN, "%5.1ffps ", m_timer.getFps());
      m_screen->getInfoString(&(buff[9]), MMDAGENT_MAXDISPSTRLEN - 9);
      glDisable(GL_LIGHTING);
      glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
      glPushMatrix();
      glRasterPos3fv(opt[CONF_FPS_POS].fv);
      m_text.drawAsciiStringBitmap(buff);
      glPopMatrix();
      glEnable(GL_LIGHTING);
   }

   /* show adjustment time for audio */
   if (m_dispFrameAdjust > 0.0) {
      if (opt[CONF_MOTION_ADJUST].i > 0)
         _snprintf(buff, MMDAGENT_MAXDISPSTRLEN, "%d msec advance", opt[CONF_MOTION_ADJUST].i);
      else
         _snprintf(buff, MMDAGENT_MAXDISPSTRLEN, "%d msec delay", opt[CONF_MOTION_ADJUST].i);
      glDisable(GL_LIGHTING);
      glColor3f(1.0f, 0.0f, 0.0f);
      glPushMatrix();
      glWindowPos2f(5.0f, 5.0f);
      m_text.drawAsciiStringBitmap(buff);
      glPopMatrix();
      glEnable(GL_LIGHTING);
   }

   /* show adjustment time per model */
   if (m_dispFrameCue > 0.0) {
      _snprintf(buff, MMDAGENT_MAXDISPSTRLEN, "Cuing Motion: %+d", (int)(m_timer.adjustGetCurrent() / 0.03));
      glDisable(GL_LIGHTING);
      glColor3f(0.0f, 1.0f, 0.0f);
      glPushMatrix();
      glWindowPos2f(160.0f, 5.0f);
      m_text.drawAsciiStringBitmap(buff);
      glPopMatrix();
      glEnable(GL_LIGHTING);
   }

   /* show model position */
   strcpy(buff, "");
   for (i = 0; i < m_numModel; i++) {
      if (m_model[i].isEnable() == true && m_dispModelMove[i] > 0.0) {
         m_model[i].getPosition(pos);
         _snprintf(buff, MMDAGENT_MAXDISPSTRLEN, "%s (%.1f, %.1f, %.1f)", buff, pos.x(), pos.y(), pos.z());
      }
   }
   if (strlen(buff) > 0) {
      glDisable(GL_LIGHTING);
      glColor3f(1.0f, 0.0f, 0.0f);
      glPushMatrix();
      glWindowPos2f(5.0f, 5.0f);
      m_text.drawAsciiStringBitmap(buff);
      glPopMatrix();
      glEnable(GL_LIGHTING);
   }

   /* show model comments and error */
   for (i = 0; i < m_numModel; i++) {
      if (m_model[i].isEnable() == true) {
         glPushMatrix();
         m_model[i].renderComment(&m_text);
         m_model[i].renderError(&m_text);
         glPopMatrix();
      }
   }

   /* execute plugin */
   m_plugin.execRender(this);

   /* swap buffer */
   m_screen->swapBuffers();
}

/* MMDAgent::updateWindowSize: change OpenGL campus size to current window size */
void MMDAgent::updateWindowSize(HWND hWnd)
{
   RECT rc;

   GetWindowRect(hWnd, &rc);
   m_render->setSize(rc.right - rc.left, rc.bottom - rc.top);
}


/* MMDAgent::updateLight: update light */
void MMDAgent::updateLight()
{
   int i;
   btVector3 l;

   /* udpate OpenGL light */
   m_render->updateLighting();
   /* update shadow matrix */
   m_stage->updateShadowMatrix(opt[CONF_LIGHT_DIRECTION_FROM].fv);
   /* update vector for cartoon */
   l = btVector3(opt[CONF_LIGHT_DIRECTION_FROM].fv[0], opt[CONF_LIGHT_DIRECTION_FROM].fv[1], opt[CONF_LIGHT_DIRECTION_FROM].fv[2]);
   for (i = 0; i < m_numModel; i++)
      if (m_model[i].isEnable() == true)
         m_model[i].setLightForToon(&l);
}

/* MMDAgent::sendCommandMessage: send command message */
void MMDAgent::sendCommandMessage(wchar_t *commandType, const wchar_t *argFormat, ...)
{
   int len;
   va_list argv;
   wchar_t *buf1, *buf2;

   buf1 = (wchar_t *) malloc(sizeof(wchar_t) * (wcslen(commandType) + 1));
   wcscpy(buf1, commandType);

   if (argFormat == NULL) {
      ::PostMessage(m_hWnd, WM_MMDAGENT_COMMAND, (WPARAM) buf1, (LPARAM) NULL);
      return;
   }

   va_start(argv, argFormat);
   len = _vscwprintf(argFormat, argv);
   buf2 = (wchar_t *) malloc(sizeof(wchar_t) * (len + 1));
   vswprintf(buf2, len + 1, argFormat, argv);
   va_end(argv);

   ::PostMessage(m_hWnd, WM_MMDAGENT_COMMAND, (WPARAM) buf1, (LPARAM) buf2);
}

/* MMDAgent::sendEventMessage: send event message */
void MMDAgent::sendEventMessage(wchar_t *eventType, const wchar_t *argFormat, ...)
{
   int len;
   va_list argv;
   wchar_t *buf1, *buf2;

   buf1 = (wchar_t *) malloc(sizeof(wchar_t) * (wcslen(eventType) + 1));
   wcscpy(buf1, eventType);

   if (argFormat == NULL) {
      ::PostMessage(m_hWnd, WM_MMDAGENT_EVENT, (WPARAM) buf1, (LPARAM) NULL);
      return;
   }

   va_start(argv, argFormat);
   len = _vscwprintf(argFormat, argv);
   buf2 = (wchar_t *) malloc(sizeof(wchar_t) * (len + 1));
   vswprintf(buf2, len + 1, argFormat, argv);
   va_end(argv);

   ::PostMessage(m_hWnd, WM_MMDAGENT_EVENT, (WPARAM) buf1, (LPARAM) buf2);
}

/* MMDAgent::setup: initialize and setup MMDAgent */
HWND MMDAgent::setup(HINSTANCE hInstance, TCHAR *szTitle, TCHAR *szWindowClass, int argc, wchar_t **argv)
{
   int i;
   size_t len;
   wchar_t binaryDirName[MAX_PATH]; /* path of MMDAgent.exe */
   wchar_t fbuf[MAX_PATH];
   wchar_t startDirName[MAX_PATH];

   m_hInst = hInstance;

   /* initialize logger */
   g_logger.setup((int) (opt[CONF_LOG_WINDOW].fv[0]), (int) (opt[CONF_LOG_WINDOW].fv[1]), opt[CONF_LOG_WINDOW].fv[2], opt[CONF_LOG_WINDOW].fv[3], opt[CONF_LOG_WINDOW].fv[4], opt[CONF_LOG_WINDOW].fv[5]);

   /* set timer precision to 2ms */
   m_timer.setPrecision(2);

   /* set local to japan */
   setlocale(LC_CTYPE, "jpn");

   /* get start directory */
   GetCurrentDirectory(MAX_PATH, startDirName);

   /* get path of MMDAgent.exe */
   GetModuleFileName(NULL, fbuf, MAX_PATH);
   getFullDirName(binaryDirName, fbuf);

   /* get default config file name */
   len = wcslen(fbuf);
   if (len > 4) {
      fbuf[len-4] = L'.';
      fbuf[len-3] = L'm';
      fbuf[len-2] = L'd';
      fbuf[len-1] = L'f';

      /* load default config file */
      if (config_load(fbuf, 1))
         wcscpy(m_configFileName, fbuf);
   }
   wcscpy(m_configDirName, binaryDirName);

   /* load additional config file name */
   for (i = 1; i < argc; i++) {
      if (hasSuffix(argv[i], L"mdf")) {
         if (config_load(argv[i], 2)) {
            wcscpy(m_configFileName, argv[i]);
            getFullDirName(m_configDirName, argv[i]);
            break; /* load only one config file */
         }
      }
   }

   /* get application directory */
   wcsncpy(m_appDirName, binaryDirName, MAX_PATH);
   wcsncat_s(m_appDirName, MAX_PATH, opt[CONF_SYSTEM_DATA_DIR].s, _TRUNCATE);
   g_logger.log(L"system_data_dir: %s", m_appDirName);

   /* get plugin directory */
   wcsncpy(fbuf, binaryDirName, MAX_PATH);
   wcsncat_s(fbuf, MAX_PATH, opt[CONF_PLUGIN_DIR].s, _TRUNCATE);
   g_logger.log(L"searching plugins at %s", fbuf);

   /* load and start plugins */
   m_plugin.load(fbuf);
   m_plugin.execAppStart(this);

   /* create components */
   m_screen = new Screen;
   m_stage = new Stage;
   m_systex = new SystemTexture;
   m_render = new Render;

   /* create window */
   m_hWnd = m_screen->createWindow(opt[CONF_WINDOW_WIDTH].i, opt[CONF_WINDOW_HEIGHT].i, hInstance, szTitle, szWindowClass);
   if (!m_hWnd)
      return m_hWnd;

   /* allow drag and drop */
   DragAcceptFiles(m_hWnd, true);

   /* initialize bullet phisics */
   m_bullet.setup(opt[CONF_BULLET_FPS].i);
   g_logger.log(L"Physics simulation step: 1/%d sec.", opt[CONF_BULLET_FPS].i);

   /* initialize audio */
   m_audio.setup(m_hWnd);

   /* load toon textures from system directory */
   char appDirNameA[1024];
   size_t appDirNameASize;
   wcstombs_s(&appDirNameASize, appDirNameA, 1024, m_appDirName, _TRUNCATE);
   if (appDirNameASize <= 0)
      return false;
   if (m_systex->load(appDirNameA) == false)
      return false;

   /* initialize render */
   if (m_render->setup(opt[CONF_WINDOW_WIDTH].i, opt[CONF_WINDOW_HEIGHT].i) == false)
      return false;

   /* initialize text render */
   m_text.setup(m_screen->getDC(), L"Arial Unicode MS");

   /* set model positions from config file */
   wchar_t buf[CONF_STR_LEN_MAX];
   wchar_t *p;
   btVector3 pos;
   pos.setZero();
   int c = 0;
   wcsncpy(buf, opt[CONF_INIT_MODEL_LOCATION].s, CONF_STR_LEN_MAX);
   i = 0;
   for (p = wcstok(buf, L", ()\t\r\n"); p; p = wcstok(NULL, L", ()\t\r\n")) {
      switch (c) {
      case 0:
         pos.setX(btScalar(_wtof(p)));
         break;
      case 1:
         pos.setY(btScalar(_wtof(p)));
         break;
      case 2:
         pos.setZ(btScalar(_wtof(p)));
         break;
      }
      if (c < 2) {
         c++;
      } else {
         c = 0;
         if (i >= MAXMODEL) break;
         m_model[i++].setPosition(pos);
      }
   }

   /* load models from config file */
   switch (opt[CONF_MODEL_FILE].src) {
   case 0:
      SetCurrentDirectory(binaryDirName);
      break;
   case 1:
      SetCurrentDirectory(binaryDirName);
      break;
   case 2:
      SetCurrentDirectory(m_configDirName);
      break;
   }
   wcsncpy(buf, opt[CONF_MODEL_FILE].s, CONF_STR_LEN_MAX);
   wchar_t *psave;
   i = 0;
   for (p = wcstokWithDoubleQuotation(buf, L",\t\r\n", &psave); p; p = wcstokWithDoubleQuotation(NULL, L",\t\r\n", &psave)) {
      if (i >= MAXMODEL) break;
      m_model[i].getPosition(pos);
      if (addModel(NULL, p, &pos, NULL, NULL, NULL)) i++;
   }
   /* re-set current directory */
   SetCurrentDirectory(startDirName);

   /* overwrite alias from config file */
   i = 0;
   wcsncpy(buf, opt[CONF_INIT_MODEL_ALIAS].s, CONF_STR_LEN_MAX);
   for (p = wcstok(buf, L",\t\r\n"); p; p = wcstok(NULL, L",\t\r\n")) {
      if (i >= MAXMODEL) break;
      wcsncpy(m_model[i].getAlias(), p, 50);
      i++;
   }

   /* load model from arguments */
   for (i = 1; i < argc; i++)
      if (hasSuffix(argv[i], L"pmd"))
         addModel(NULL, argv[i], NULL, NULL, NULL, NULL);

   /* load motion from config file */
   switch (opt[CONF_INIT_MOTION_FILE].src) {
   case 0:
      SetCurrentDirectory(binaryDirName);
      break;
   case 1:
      SetCurrentDirectory(binaryDirName);
      break;
   case 2:
      SetCurrentDirectory(m_configDirName);
      break;
   }
   wcsncpy(buf, opt[CONF_INIT_MOTION_FILE].s, CONF_STR_LEN_MAX);
   i = 0;
   for (p = wcstok(buf, L", \t\r\n"); p; p = wcstok(NULL, L", \t\r\n")) {
      while (i < m_numModel && m_model[i].isEnable() == false)
         i++;
      if (i >= m_numModel)
         break;
      addMotion(m_model[i].getAlias(), NULL, p, true, true, true, true);
      i++;
   }
   /* re-set current directory */
   SetCurrentDirectory(startDirName);

   /* load stage */
   bool stage_flag = false;
   m_stage->setSize(opt[CONF_FLOOR_WIDTH].f, (opt[CONF_FLOOR_DEPTH].f == 0.0f) ? opt[CONF_FLOOR_WIDTH].f : opt[CONF_FLOOR_DEPTH].f, opt[CONF_BACK_HEIGHT].f , 1.0f, 1.0f);
   switch (opt[CONF_FLOOR_BMP].src) {
   case 0:
      SetCurrentDirectory(binaryDirName);
      break;
   case 1:
      SetCurrentDirectory(binaryDirName);
      break;
   case 2:
      SetCurrentDirectory(m_configDirName);
      break;
   }
   if (hasSuffix(opt[CONF_FLOOR_BMP].s, L"pmd")) {
      m_stage->loadStagePMD(opt[CONF_FLOOR_BMP].s , &m_bullet, m_systex);
      stage_flag = true;
   } else {
      m_stage->loadFloor(opt[CONF_FLOOR_BMP].s, &m_bullet);
   }
   /* re-set current directory */
   SetCurrentDirectory(startDirName);

   /* load background and floor */
   switch (opt[CONF_BACK_BMP].src) {
   case 0:
      SetCurrentDirectory(binaryDirName);
      break;
   case 1:
      SetCurrentDirectory(binaryDirName);
      break;
   case 2:
      SetCurrentDirectory(m_configDirName);
      break;
   }
   if (hasSuffix(opt[CONF_BACK_BMP].s, L"pmd")) {
      m_stage->loadStagePMD(opt[CONF_BACK_BMP].s, &m_bullet, m_systex);
   } else if (stage_flag == false) {
      m_stage->loadBackground(opt[CONF_BACK_BMP].s, &m_bullet);
   }
   /* re-set current directory */
   SetCurrentDirectory(startDirName);

   /* set full screen */
   if (opt[CONF_FULLSCREEN_ENABLED].b)
      m_screen->setFullScreen(m_hWnd);

   /* set mouse enable timer */
   m_screen->setMouseActiveTime(45.0f);

   /* if music file is specified, start sound */
   if (wcslen(opt[CONF_MUSIC_FILE].s) > 0) {
      switch (opt[CONF_MUSIC_FILE].src) {
      case 0:
         SetCurrentDirectory(binaryDirName);
         break;
      case 1:
         SetCurrentDirectory(binaryDirName);
         break;
      case 2:
         SetCurrentDirectory(m_configDirName);
         break;
      }
      startSound(L"audio", opt[CONF_MUSIC_FILE].s, true);
   }

   /* update light */
   updateLight();

   initializeLookAt(this, &m_plugin);

   /* start timer */
   m_timer.startSystem();

   return m_hWnd;
}

/* MMDAgent::procMessage: process message */
bool MMDAgent::procMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   int wmId, wmEvent;
   bool hit_mouse = true;
   bool hit_window = true;
   int i;
   float s;
   wchar_t *aliasName;
   GLint polygonMode[2];

   /* execute plugin functions */
   m_plugin.execWindowProc(this, hWnd, message, wParam, lParam);
   switch (message) {
   case WM_CREATE:
      m_plugin.execWindowCreate(this, hWnd);
      break;
   case WM_DESTROY:
      m_plugin.execAppEnd(this);
      break;
   }

   /* process mouse events */
   float tmp1, tmp2, tmp3;
   LONG r1;
   LONG r2;
   btVector3 v;
   btMatrix3x3 bm;

   switch (message) {
   case WM_LBUTTONDBLCLK:
      /* double click */
      m_mousepos.x = LOWORD(lParam);
      m_mousepos.y = HIWORD(lParam);
      /* store model ID */
      m_selectedModel = m_render->pickModel(this, LOWORD(lParam), HIWORD(lParam), NULL);
      /* make model highlight */
      m_render->highlightModel(this, m_selectedModel);
      m_doubleClicked = true;
      if (m_selectedModel != -1)
         g_logger.log(L"DoubleClick on %s", m_model[m_selectedModel].getAlias());
      break;
   case WM_LBUTTONDOWN:
      /* start hold */
      m_mousepos.x = LOWORD(lParam);
      m_mousepos.y = HIWORD(lParam);
      SetCapture(hWnd);
      m_leftButtonPressed = true;
      m_doubleClicked = false;
      /* store model ID */
      m_selectedModel = m_render->pickModel(this, LOWORD(lParam), HIWORD(lParam), NULL);
      if ((wParam & MK_CONTROL) && (!(wParam & MK_SHIFT))) /* with Ctrl-key */
         m_render->highlightModel(this, m_selectedModel);
      break;
   case WM_LBUTTONUP:
      /* if highlight, trun off */
      if (!m_doubleClicked)
         m_render->highlightModel(this, -1);
      /* end of hold */
      m_leftButtonPressed = false;
      ReleaseCapture();
      break;
   case WM_MOUSEWHEEL:
      /* zoom */
      tmp1 = opt[CONF_UI_SCALE_STEP].f;
      tmp2 = m_render->getScale();
      if (wParam & MK_SHIFT) /* faster */
         tmp1 = (tmp1 - 1.0f) * 5.0f + 1.0f;
      else if (wParam & MK_CONTROL) /* slower */
         tmp1 = (tmp1 - 1.0f) * 0.2f + 1.0f;
      if ((short) HIWORD(wParam) > 0)
         tmp2 *= tmp1;
      else
         tmp2 /= tmp1;
      m_render->setScale(tmp2);
      InvalidateRect(hWnd, NULL, false);
      break;
   case WM_MOUSEMOVE:
      /* store Ctrl-key and Shift-key state for drag and drop */
      m_keyCtrl = (wParam & MK_CONTROL) ? true : false;
      m_keyShift = (wParam & MK_SHIFT) ? true : false;
      /* left-button is dragged in window */
      if (m_leftButtonPressed) {
         r1 = LOWORD(lParam);
         r2 = HIWORD(lParam);
         r1 -= m_mousepos.x;
         r2 -= m_mousepos.y;
         if (r1 > 32767) r1 -= 65536;
         if (r1 < -32768) r1 += 65536;
         if (r2 > 32767) r2 -= 65536;
         if (r2 < -32768) r2 += 65536;
         if (wParam & MK_SHIFT && wParam & MK_CONTROL) {
            /* if Shift- and Ctrl-key, rotate light direction */
            v = btVector3(opt[CONF_LIGHT_DIRECTION_FROM].fv[0], opt[CONF_LIGHT_DIRECTION_FROM].fv[1], opt[CONF_LIGHT_DIRECTION_FROM].fv[2]);
            bm = btMatrix3x3(btQuaternion(0, r2 * 0.007f, 0) * btQuaternion(r1 * 0.007f, 0, 0));
            v = bm * v;
            changeLightDirection(v.x(), v.y(), v.z());
         } else if (wParam & MK_SHIFT) {
            /* if Shift-key, translate display */
            tmp1 = r1 / (float) m_render->getWidth();
            tmp2 = - r2 / (float) m_render->getHeight();
            tmp3 = 20.0f;
            tmp1 = (float) (tmp1 * (tmp3 - opt[CONF_VIEWPOINT_CAMERA_Z].f) / opt[CONF_VIEWPOINT_FRUSTUM_NEAR].f);
            tmp2 = (float) (tmp2 * (tmp3 - opt[CONF_VIEWPOINT_CAMERA_Z].f) / opt[CONF_VIEWPOINT_FRUSTUM_NEAR].f);
            tmp1 /= m_render->getScale();
            tmp2 /= m_render->getScale();
            tmp3 = 0.0f;
            m_render->translate(tmp1, tmp2, tmp3);
         } else if (wParam & MK_CONTROL) {
            /* if Ctrl-key, move model */
            if (m_selectedModel != -1) {
               m_render->highlightModel(this, m_selectedModel);
               btVector3 pos;
               m_model[m_selectedModel].getPosition(pos);
               pos.setX(pos.x() + r1 / 20.0f);
               pos.setZ(pos.z() + r2 / 20.0f);
               m_model[m_selectedModel].setPosition(pos);
               m_model[m_selectedModel].setMoveSpeed(-1.0f);
            }
         } else {
            /* if no key, rotate display */
            m_render->rotate(r1 * 0.007f, r2 * 0.007f, 0.0f);
         }
         m_mousepos.x = LOWORD(lParam);
         m_mousepos.y = HIWORD(lParam);
         /* forced update motion */
         updateScene();
      } else if (m_mousepos.x != LOWORD(lParam) || m_mousepos.y != HIWORD(lParam)) {
         /* set mouse enable timer */
         m_screen->setMouseActiveTime(45.0f);
      }
      break;
   case WM_RBUTTONDOWN:
      /* set mouse enable timer */
      m_screen->setMouseActiveTime(45.0f);
      break;
   default:
      hit_mouse = false;
      break;
   }

   switch (message) {
   case WM_COMMAND:
      wmId = LOWORD(wParam);
      wmEvent = HIWORD(wParam);
      switch (wmId) {
      case IDM_TOGGLE_FULLSCREEN:
         if (opt[CONF_FULLSCREEN_ENABLED].b) {
            m_screen->exitFullScreen(hWnd);
            opt[CONF_FULLSCREEN_ENABLED].b = false;
         } else {
            m_screen->setFullScreen(hWnd);
            opt[CONF_FULLSCREEN_ENABLED].b = true;
         }
         updateWindowSize(hWnd);
         break;
      case IDM_TOGGLE_INFOSTRING:
         opt[CONF_FPS_ENABLED].b = !opt[CONF_FPS_ENABLED].b;
         break;
      case IDM_TOGGLE_VSYNC:
         m_screen->toggleVSync();
         break;
      case IDM_TOGGLE_SHADOWMAP:
         m_render->setShadowMapping(!m_render->getShadowMapping());
         break;
      case IDM_TOGGLE_SHADOWMAP_ORDER:
         m_render->setShadowMapOrder(!m_render->getShadowMapOrder());
         break;
      case IDM_TOGGLE_DISP_RIGIDBODY:
         m_dispBulletBodyFlag = !m_dispBulletBodyFlag;
         break;
      case IDM_TOGGLE_DISP_WIRE:
         glGetIntegerv(GL_POLYGON_MODE, polygonMode);
         if (polygonMode[1] == GL_LINE)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
         else
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
         break;
      case IDM_TOGGLE_DISP_BONE:
         m_dispModelDebug = !m_dispModelDebug;
         break;
      case IDM_CARTOON_EDGE_PLUS:
         opt[CONF_CARTOON_EDGE_WIDTH].f *= opt[CONF_UI_CARTOON_EDGE_STEP].f;
         for (i = 0; i < m_numModel; i++)
            m_model[i].getPMDModel()->setEdgeThin(opt[CONF_CARTOON_EDGE_WIDTH].f);
         break;
      case IDM_CARTOON_EDGE_MINUS:
         opt[CONF_CARTOON_EDGE_WIDTH].f /= opt[CONF_UI_CARTOON_EDGE_STEP].f;
         for (i = 0; i < m_numModel; i++)
            m_model[i].getPMDModel()->setEdgeThin(opt[CONF_CARTOON_EDGE_WIDTH].f);
         break;
      case IDM_TIME_PLUS:
         opt[CONF_MOTION_ADJUST].i += 10;
         m_timer.adjustSetTarget((double) opt[CONF_MOTION_ADJUST].i * 0.03);
         m_dispFrameAdjust = 90.0;
         break;
      case IDM_TIME_MINUS:
         opt[CONF_MOTION_ADJUST].i -= 10;
         m_timer.adjustSetTarget((double) opt[CONF_MOTION_ADJUST].i * 0.03);
         m_dispFrameAdjust = 90.0;
         break;
      case IDM_ZOOM_IN:
         s = m_render->getScale();
         s *= opt[CONF_UI_SCALE_STEP].f;
         m_render->setScale(s);
         InvalidateRect(hWnd, NULL, false);
         break;
      case IDM_ZOOM_OUT:
         s = m_render->getScale();
         s /= opt[CONF_UI_SCALE_STEP].f;
         m_render->setScale(s);
         InvalidateRect(hWnd, NULL, false);
         break;
      case IDM_ROTATE_LEFT:
         m_render->rotate(-opt[CONF_UI_ROTATE_STEP].f, 0.0f, 0.0f);
         break;
      case IDM_ROTATE_RIGHT:
         m_render->rotate(opt[CONF_UI_ROTATE_STEP].f, 0.0f, 0.0f);
         break;
      case IDM_ROTATE_UP:
         m_render->rotate(0.0f, -opt[CONF_UI_ROTATE_STEP].f, 0.0f);
         break;
      case IDM_ROTATE_DOWN:
         m_render->rotate(0.0f, opt[CONF_UI_ROTATE_STEP].f, 0.0f);
         break;
      case IDM_MOVE_LEFT:
         m_render->translate(-opt[CONF_UI_TRANSLATE_STEP].f, 0.0f, 0.0f); /* move left */
         break;
      case IDM_MOVE_RIGHT:
         m_render->translate(opt[CONF_UI_TRANSLATE_STEP].f, 0.0f, 0.0f); /* move right */
         break;
      case IDM_MOVE_UP:
         m_render->translate(0.0f, opt[CONF_UI_TRANSLATE_STEP].f, 0.0f);
         break;
      case IDM_MOVE_DOWN:
         m_render->translate(0.0f, -opt[CONF_UI_TRANSLATE_STEP].f, 0.0f);
         break;
      case IDM_DELETE_MODEL:
         if (m_doubleClicked) {
            deleteModel(m_model[m_selectedModel].getAlias());
            m_doubleClicked = false;
         }
         break;
      case IDM_TOGGLE_PHYSICS:
         m_enablePhysicsSimulation = !m_enablePhysicsSimulation;
         for (i = 0; i < m_numModel; i++)
            m_model[i].getPMDModel()->setPhysicsControl(m_enablePhysicsSimulation);
         if (m_enablePhysicsSimulation)
            g_logger.log(L"Physics simulation enabled");
         else
            g_logger.log(L"Physics simulation disabled");
         break;
      case IDM_TOGGLE_DISP_LOG:
         m_dispLog = !m_dispLog;
         break;
      }
      break;
   case WM_SIZE:
      /* change window size */
      updateWindowSize(hWnd);
      break;
   case WM_MOVE:
      /* forced update motion */
      updateScene();
      break;
   case WM_DROPFILES:
      dropFile(hWnd, wParam, lParam);
      break;
   case MM_MCINOTIFY:
      /* audio stop */
      aliasName = m_audio.getFinishedAlias(wParam, lParam);
      if (aliasName) {
         sendEventMessage(MMDAGENT_EVENT_SOUND_STOP, L"%s", aliasName);
         m_audio.close(aliasName);
      }
      break;
   case WM_CHAR:
      sendEventMessage(MMDAGENT_EVENT_KEY, L"%C", wParam);
      break;
   case WM_MMDAGENT_COMMAND:
      /* check command and free strings */
      command((wchar_t *) wParam, (wchar_t *) lParam);
      if ((wchar_t *) wParam != NULL)
         free((wchar_t *) wParam);
      if ((wchar_t *) lParam != NULL)
         free((wchar_t *) lParam);
      break;
   case WM_MMDAGENT_EVENT:
      /* free strings */
      if ((wchar_t *) wParam != NULL) {
         if ((wchar_t *) lParam != NULL)
            g_logger.log(L"[%s|%s]", (wchar_t *) wParam, (wchar_t *) lParam);
         else
            g_logger.log(L"[%s]", (wchar_t *) wParam);
      }
      if ((wchar_t *) wParam != NULL)
         free((wchar_t *) wParam);
      if ((wchar_t *) lParam != NULL)
         free((wchar_t *) lParam);
      break;
   default:
      hit_window = false;
      break;
   }

   if (hit_window == true || hit_mouse == true)
      return true;
   else
      return false;
}

/* MMDAgent::findModelAlias: find a model with the specified alias */
int MMDAgent::findModelAlias(wchar_t *alias)
{
   int i;

   for (i = 0; i < m_numModel; i++)
      if (m_model[i].isEnable() && wcscmp(m_model[i].getAlias(), alias) == 0)
         return i;

   return -1;
}

/* MMDAgent::getMoelList: get model list */
PMDObject *MMDAgent::getModelList()
{
   return &(m_model[0]);
}

/* MMDAgent::getNumModel: */
short MMDAgent::getNumModel()
{
   return m_numModel;
}

/* MMDAgent::getRender: get render */
Render *MMDAgent::getRender()
{
   return m_render;
}

/* MMDAgent::getStage: get stage */
Stage *MMDAgent::getStage()
{
   return m_stage;
}

/* MMDAgent::getTimer: get timer */
Timer *MMDAgent::getTimer()
{
   return &m_timer;
}

/* MMDAgent::getAudio: get audio */
Audio *MMDAgent::getAudio()
{
   return &m_audio;
}

/* MMDAgent::getWindowHandler: get window handle */
HWND MMDAgent::getWindowHandler()
{
   return m_hWnd;
}

/* MMDAgent::getInstance: get instance */
HINSTANCE MMDAgent::getInstance()
{
   return m_hInst;
}
