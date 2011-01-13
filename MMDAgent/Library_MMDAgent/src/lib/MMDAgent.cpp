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

#include "MMDFiles.h"

#include "Option.h"
#include "MMDAgent.h"
#include "utils.h"
#include "MMDAgent_command.h"

/* MMDAgent::initialize: initialize MMDAgent */
void MMDAgent::initialize()
{
   int i;

   m_configFileName = NULL;
   m_configDirName = NULL;
   m_appDirName = NULL;

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

   if(m_configFileName)
      free(m_configFileName);
   if(m_configDirName)
      free(m_configDirName);
   if(m_appDirName)
      free(m_appDirName);
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

   stepmax = m_option.getBulletFps();
   stepFrame = 30.0 / m_option.getBulletFps();
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
                  if (strcmp(motionPlayer->name, LIPSYNC_MOTIONNAME) == 0)
                     sendEventMessage(MMDAGENTCOMMAND_LIPSYNCEVENTSTOP, "%s", m_model[i].getAlias());
                  else {
                     sendEventMessage(MMDAGENTCOMMAND_MOTIONEVENTDELETE, "%s|%s", m_model[i].getAlias(), motionPlayer->name);
                  }
                  /* unload from motion stocker */
                  m_motion.unload(motionPlayer->vmd);
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
   if(m_option.getUseShadowMapping())
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
            sendEventMessage(MMDAGENTCOMMAND_MOVEEVENTSTOP, "%s", m_model[i].getAlias());
         if (m_model[i].updateModelRootRotation(fps)) {
            if (m_model[i].isTurning()) {
               sendEventMessage(MMDAGENTCOMMAND_TURNEVENTSTOP, "%s", m_model[i].getAlias());
               m_model[i].setTurningFlag(false);
            } else {
               sendEventMessage(MMDAGENTCOMMAND_ROTATEEVENTSTOP, "%s", m_model[i].getAlias());
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
   if (m_option.getShowFps()) {
      _snprintf(buff, MMDAGENT_MAXDISPSTRLEN, "%5.1ffps ", m_timer.getFps());
      m_screen->getInfoString(&(buff[9]), MMDAGENT_MAXDISPSTRLEN - 9);
      glDisable(GL_LIGHTING);
      glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
      glPushMatrix();
      glRasterPos3fv(m_option.getFpsPosition());
      m_text.drawAsciiStringBitmap(buff);
      glPopMatrix();
      glEnable(GL_LIGHTING);
   }

   /* show adjustment time for audio */
   if (m_dispFrameAdjust > 0.0) {
      if (m_option.getMotionAdjustFrame() > 0)
         _snprintf(buff, MMDAGENT_MAXDISPSTRLEN, "%d msec advance", m_option.getMotionAdjustFrame());
      else
         _snprintf(buff, MMDAGENT_MAXDISPSTRLEN, "%d msec delay", m_option.getMotionAdjustFrame());
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
   float *f;
   btVector3 l;

   /* udpate OpenGL light */
   m_render->updateLighting(m_option.getUseCartoonRendering(), m_option.getUseMMDLikeCartoon(), m_option.getLightDirection(), m_option.getLightIntensity(), m_option.getLightColor());
   /* update shadow matrix */
   f = m_option.getLightDirection();
   m_stage->updateShadowMatrix(f);
   /* update vector for cartoon */
   l = btVector3(f[0], f[1], f[2]);
   for (i = 0; i < m_numModel; i++)
      if (m_model[i].isEnable() == true)
         m_model[i].setLightForToon(&l);
}

/* MMDAgent::sendCommandMessage: send command message */
void MMDAgent::sendCommandMessage(char *type, const char *format, ...)
{
   va_list argv;
   char *buf1, *buf2;

   buf1 = strdup(type);

   if (format == NULL) {
      ::PostMessage(m_hWnd, WM_MMDAGENT_COMMAND, (WPARAM) buf1, (LPARAM) NULL);
      return;
   }

   buf2 = (char *) malloc(sizeof(char) * MMDAGENT_MAXBUFLEN);

   va_start(argv, format);
   vsnprintf(buf2, MMDAGENT_MAXBUFLEN, format, argv);
   va_end(argv);

   ::PostMessage(m_hWnd, WM_MMDAGENT_COMMAND, (WPARAM) buf1, (LPARAM) buf2);
}

/* MMDAgent::sendEventMessage: send event message */
void MMDAgent::sendEventMessage(char *type, const char *format, ...)
{
   va_list argv;
   char *buf1, *buf2;

   buf1 = strdup(type);

   if (format == NULL) {
      ::PostMessage(m_hWnd, WM_MMDAGENT_EVENT, (WPARAM) buf1, (LPARAM) NULL);
      return;
   }

   buf2 = (char *) malloc(sizeof(char) * MMDAGENT_MAXBUFLEN);

   va_start(argv, format);
   vsnprintf(buf2, MMDAGENT_MAXBUFLEN, format, argv);
   va_end(argv);

   ::PostMessage(m_hWnd, WM_MMDAGENT_EVENT, (WPARAM) buf1, (LPARAM) buf2);
}

/* MMDAgent::setup: initialize and setup MMDAgent */
HWND MMDAgent::setup(HINSTANCE hInstance, TCHAR *szTitle, TCHAR *szWindowClass, int argc, char **argv)
{
   int i;
   size_t len;
   char buff[MMDAGENT_MAXBUFLEN];

   char *binaryFileName;
   char *binaryDirName;

   if(argc < 1 || strlen(argv[0]) <= 0)
      return 0;

   /* get binary file name */
   binaryFileName = strdup(argv[0]);

   /* get binary directory name */
   binaryDirName = (char *) malloc(sizeof(char) * (strlen(binaryFileName) + 1));
   getDirectory(binaryFileName, binaryDirName);

   m_hInst = hInstance;

   /* set timer precision to 2ms */
   m_timer.setPrecision(2);

   /* set local to japan */
   setlocale(LC_CTYPE, "jpn");

   /* get application directory */
   if(m_appDirName)
      free(m_appDirName);
   m_appDirName = (char *) malloc(sizeof(char) * (strlen(binaryDirName) + 1 + strlen(MMDAGENT_SYSDATADIR) + 1));
   sprintf(m_appDirName, "%s%c%s", binaryDirName, MMDAGENT_DIRSEPARATOR, MMDAGENT_SYSDATADIR);

   /* get default config file name */
   strcpy(buff, binaryFileName);
   len = strlen(buff);
   if (len > 4) {
      buff[len-4] = '.';
      buff[len-3] = 'm';
      buff[len-2] = 'd';
      buff[len-1] = 'f';

      /* load default config file */
      if(m_option.load(buff)) {
         if(m_configFileName)
            free(m_configFileName);
         m_configFileName = strdup(buff);
      }
   }

   /* load additional config file name */
   for (i = 1; i < argc; i++) {
      if (hasExtension(argv[i], ".mdf")) {
         if (m_option.load(argv[i])) {
            if(m_configFileName)
               free(m_configFileName);
            m_configFileName = strdup(argv[i]);
         }
      }
   }

   /* get config directory name */
   if(m_configDirName)
      free(m_configDirName);
   if(m_configFileName == NULL) {
      m_configFileName = strdup(binaryFileName);
      m_configDirName = strdup(binaryDirName);
   } else {
      m_configDirName = (char *) malloc(sizeof(char) * (strlen(m_configFileName) + 1));
      getDirectory(m_configFileName, m_configDirName);
   }

   /* load and start plugins */
   sprintf(buff, "%s%c%s", binaryDirName, MMDAGENT_DIRSEPARATOR, MMDAGENT_PLUGINDIR);
   m_plugin.load(buff);
   m_plugin.execAppStart(this);

   /* initialize logger */
   g_logger.setup(m_option.getLogSize(), m_option.getLogPosition(), m_option.getLogScale());

   /* create components */
   m_screen = new Screen;
   m_stage = new Stage;
   m_systex = new SystemTexture;
   m_render = new Render;

   /* create window */
   m_hWnd = m_screen->createWindow(m_option.getWindowSize(), hInstance, szTitle, szWindowClass, m_option.getMaxMultiSampling(), m_option.getMaxMultiSamplingColor(), m_option.getTopMost());
   if (!m_hWnd)
      return m_hWnd;

   /* allow drag and drop */
   DragAcceptFiles(m_hWnd, true);

   /* initialize bullet phisics */
   m_bullet.setup(m_option.getBulletFps());

   /* load toon textures from system directory */
   if (m_systex->load(m_appDirName) == false)
      return false;

   /* initialize render */
   if (m_render->setup(m_option.getWindowSize(), m_option.getCampusColor(), m_option.getUseShadowMapping(), m_option.getShadowMappingTextureSize(), m_option.getShadowMappingLightFirst()) == false)
      return false;

   /* initialize text render */
   m_text.setup(m_screen->getDC());

   /* load model from arguments */
   for (i = 1; i < argc; i++)
      if (hasExtension(argv[i], ".pmd"))
         addModel(NULL, argv[i], NULL, NULL, NULL, NULL);

   /* set stage size */
   m_stage->setSize(m_option.getStageSize(), 1.0f, 1.0f);

   /* set full screen */
   if (m_option.getFullScreen())
      m_screen->setFullScreen(m_hWnd);

   /* set mouse enable timer */
   m_screen->setMouseActiveTime(45.0f);

   /* update light */
   updateLight();

   /* start timer */
   m_timer.startSystem();

   SetCurrentDirectoryA(m_configDirName);

   free(binaryFileName);
   free(binaryDirName);
   return m_hWnd;
}

void MMDAgent::procWindowCreateMessage(HWND hWnd)
{
   m_plugin.execWindowCreate(this, hWnd);
}

void MMDAgent::procWindowDestroyMessage()
{
   m_plugin.execAppEnd(this);
   release();
}

void MMDAgent::procMouseLeftButtonDoubleClickMessage(int x, int y)
{
   /* double click */
   m_mousepos.x = x;
   m_mousepos.y = y;
   /* store model ID */
   m_selectedModel = m_render->pickModel(this, x, y, NULL);
   /* make model highlight */
   m_render->highlightModel(this, m_selectedModel);
   m_doubleClicked = true;
}

void MMDAgent::procMouseLeftButtonDownMessage(int x, int y, bool withCtrl, bool withShift)
{
   /* start hold */
   m_mousepos.x = x;
   m_mousepos.y = y;
   m_leftButtonPressed = true;
   m_doubleClicked = false;
   /* store model ID */
   m_selectedModel = m_render->pickModel(this, x, y, NULL);
   if (withCtrl == true && withShift == false) /* with Ctrl-key */
      m_render->highlightModel(this, m_selectedModel);
}

void MMDAgent::procMouseLeftButtonUpMessage()
{
   /* if highlight, trun off */
   if (!m_doubleClicked)
      m_render->highlightModel(this, -1);
   /* end of hold */
   m_leftButtonPressed = false;
}

void MMDAgent::procMouseWheel(bool zoomup, bool withCtrl, bool withShift)
{
   float tmp1, tmp2;

   /* zoom */
   tmp1 = m_option.getScaleStep();
   tmp2 = m_render->getScale();
   if (withCtrl) /* faster */
      tmp1 = (tmp1 - 1.0f) * 5.0f + 1.0f;
   else if (withShift) /* slower */
      tmp1 = (tmp1 - 1.0f) * 0.2f + 1.0f;
   if (zoomup)
      tmp2 *= tmp1;
   else
      tmp2 /= tmp1;
   m_render->setScale(tmp2);
}

void MMDAgent::procMouseMove(int x, int y, bool withCtrl, bool withShift)
{
   float *f;
   float tmp1, tmp2, tmp3;
   LONG r1; /* should be renamed */
   LONG r2;
   btVector3 v;
   btMatrix3x3 bm;

   /* store Ctrl-key and Shift-key state for drag and drop */
   m_keyCtrl = withCtrl;
   m_keyShift = withShift;
   /* left-button is dragged in window */
   if (m_leftButtonPressed) {
      r1 = x;
      r2 = y;
      r1 -= m_mousepos.x;
      r2 -= m_mousepos.y;
      if (r1 > 32767) r1 -= 65536;
      if (r1 < -32768) r1 += 65536;
      if (r2 > 32767) r2 -= 65536;
      if (r2 < -32768) r2 += 65536;
      if (withShift && withCtrl) {
         /* if Shift- and Ctrl-key, rotate light direction */
         f = m_option.getLightDirection();
         v = btVector3(f[0], f[1], f[2]);
         bm = btMatrix3x3(btQuaternion(0, r2 * 0.007f, 0) * btQuaternion(r1 * 0.007f, 0, 0));
         v = bm * v;
         changeLightDirection(v.x(), v.y(), v.z());
      } else if (withShift) {
         /* if Shift-key, translate display */
         tmp1 = r1 / (float) m_render->getWidth();
         tmp2 = - r2 / (float) m_render->getHeight();
         tmp3 = 20.0f;
         tmp1 = (float) (tmp1 * (tmp3 - RENDER_VIEWPOINT_CAMERA_Z) / RENDER_VIEWPOINT_FRUSTUM_NEAR);
         tmp2 = (float) (tmp2 * (tmp3 - RENDER_VIEWPOINT_CAMERA_Z) / RENDER_VIEWPOINT_FRUSTUM_NEAR);
         tmp1 /= m_render->getScale();
         tmp2 /= m_render->getScale();
         tmp3 = 0.0f;
         m_render->translate(tmp1, tmp2, tmp3);
      } else if (withCtrl) {
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
      m_mousepos.x = x;
      m_mousepos.y = y;
      /* forced update motion */
      updateScene();
   } else if (m_mousepos.x != x || m_mousepos.y != y) {
      /* set mouse enable timer */
      m_screen->setMouseActiveTime(45.0f);
   }
}

void MMDAgent::procMouseRightButtonDownMessage()
{
   m_screen->setMouseActiveTime(45.0f);
}

void MMDAgent::procFullScreenMessage()
{
   if (m_option.getFullScreen() == true) {
      m_screen->exitFullScreen(m_hWnd);
      m_option.setFullScreen(false);
   } else {
      m_screen->setFullScreen(m_hWnd);
      m_option.setFullScreen(true);
   }
   updateWindowSize(m_hWnd);
}

void MMDAgent::procInfoStringMessage()
{
   if(m_option.getShowFps() == true)
      m_option.setShowFps(false);
   else
      m_option.setShowFps(true);
}

void MMDAgent::procVSyncMessage()
{
   m_screen->toggleVSync();
}

void MMDAgent::procShadowMappingMessage()
{
   if(m_option.getUseShadowMapping() == true) {
      m_option.setUseShadowMapping(false);
      m_render->setShadowMapping(false, m_option.getShadowMappingTextureSize(), m_option.getShadowMappingLightFirst());
   } else {
      m_option.setUseShadowMapping(true);
      m_render->setShadowMapping(true, m_option.getShadowMappingTextureSize(), m_option.getShadowMappingLightFirst());
   }
}

void MMDAgent::procShadowMappingOrderMessage()
{
   if(m_option.getShadowMappingLightFirst() == true)
      m_option.setShadowMappingLightFirst(false);
   else
      m_option.setShadowMappingLightFirst(true);
}

void MMDAgent::procDisplayRigidBodyMessage()
{
   m_dispBulletBodyFlag = !m_dispBulletBodyFlag;
}

void MMDAgent::procDisplayWireMessage()
{
   GLint polygonMode[2];

   glGetIntegerv(GL_POLYGON_MODE, polygonMode);
   if (polygonMode[1] == GL_LINE)
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   else
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

void MMDAgent::procDisplayBoneMessage()
{
   m_dispModelDebug = !m_dispModelDebug;
}

void MMDAgent::procCartoonEdgeMessage(bool plus)
{
   int i;

   if(plus)
      m_option.setCartoonEdgeWidth(m_option.getCartoonEdgeWidth() * m_option.getCartoonEdgeStep());
   else
      m_option.setCartoonEdgeWidth(m_option.getCartoonEdgeWidth() / m_option.getCartoonEdgeStep());
   for (i = 0; i < m_numModel; i++)
      m_model[i].getPMDModel()->setEdgeThin(m_option.getCartoonEdgeWidth());
}

void MMDAgent::procTimeAdjustMessage(bool plus)
{
   if(plus)
      m_option.setMotionAdjustFrame(m_option.getMotionAdjustFrame() + 10); /* todo: 10 -> option */
   else
      m_option.setMotionAdjustFrame(m_option.getMotionAdjustFrame() - 10);
   m_timer.adjustSetTarget((double) m_option.getMotionAdjustFrame() * 0.03);
   m_dispFrameAdjust = 90.0;
}

void MMDAgent::procHorizontalRotateMessage(bool right)
{
   if(right)
      m_render->rotate(m_option.getRotateStep(), 0.0f, 0.0f);
   else
      m_render->rotate(-m_option.getRotateStep(), 0.0f, 0.0f);
}

void MMDAgent::procVerticalRotateMessage(bool up)
{
   if(up)
      m_render->rotate(0.0f, -m_option.getRotateStep(), 0.0f);
   else
      m_render->rotate(0.0f, m_option.getRotateStep(), 0.0f);
}

void MMDAgent::procHorizontalMoveMessage(bool right)
{
   if(right)
      m_render->translate(m_option.getTranslateStep(), 0.0f, 0.0f);
   else
      m_render->translate(-m_option.getTranslateStep(), 0.0f, 0.0f);
}

void MMDAgent::procVerticalMoveMessage(bool up)
{
   if(up)
      m_render->translate(0.0f, m_option.getTranslateStep(), 0.0f);
   else
      m_render->translate(0.0f, -m_option.getTranslateStep(), 0.0f);
}

void MMDAgent::procDeleteModelMessage()
{
   if (m_doubleClicked) {
      deleteModel(m_model[m_selectedModel].getAlias());
      m_doubleClicked = false;
   }
}

void MMDAgent::procPhysicsMessage()
{
   int i;

   m_enablePhysicsSimulation = !m_enablePhysicsSimulation;
   for (i = 0; i < m_numModel; i++)
      m_model[i].getPMDModel()->setPhysicsControl(m_enablePhysicsSimulation);
}

void MMDAgent::procDisplayLogMessage()
{
   m_dispLog = !m_dispLog;
}

void MMDAgent::procWindowSizeMessage(int x, int y)
{
   m_render->setSize(x, y);
}

void MMDAgent::procCommandMessage(char *mes1, char *mes2)
{
   /* check command and free strings */
   command(mes1, mes2);
   if (mes1 != NULL)
      free(mes1);
   if (mes2 != NULL)
      free(mes2);
}

void MMDAgent::procEventMessage(char *mes1, char *mes2)
{
   /* free strings */
   if (mes1 != NULL) {
      if (mes2 != NULL && strlen(mes2) > 0)
         g_logger.log("[%s|%s]", mes1, mes2);
      else
         g_logger.log("[%s]", mes1);
   }
   if (mes1 != NULL)
      free(mes1);
   if (mes2 != NULL)
      free(mes2);
}

void MMDAgent::procPluginMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   m_plugin.execWindowProc(this, hWnd, message, wParam, lParam);
}

#if 0
case MM_MCINOTIFY:
/* audio stop */
aliasName = m_audio.getFinishedAlias(wParam, lParam);
if (aliasName)
{
   char mbsbuf[MMDAGENT_MAXBUFLEN];
   size_t len;
   wcstombs_s(&len, mbsbuf, MMDAGENT_MAXBUFLEN, aliasName, _TRUNCATE);
   sendEventMessage(MMDAGENTCOMMAND_SOUNDEVENTSTOP, "%s", mbsbuf);
   m_audio.close(mbsbuf);
}
break;
#endif

/* MMDAgent::findModelAlias: find a model with the specified alias */
int MMDAgent::findModelAlias(char *alias)
{
   int i;

   for (i = 0; i < m_numModel; i++)
      if (m_model[i].isEnable() && strcmp(m_model[i].getAlias(), alias) == 0)
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

Option *MMDAgent::getOption()
{
   return &m_option;
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
