/* ----------------------------------------------------------------- */
/*           The Toolkit for Building Voice Interaction Systems      */
/*           "MMDAgent" developed by MMDAgent Project Team           */
/*           http://www.mmdagent.jp/                                 */
/* ----------------------------------------------------------------- */
/*                                                                   */
/*  Copyright (c) 2009-2015  Nagoya Institute of Technology          */
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
#include "VIManager.h"
#include "VIManager_Logger.h"
#include "VIManager_Thread.h"

/* VIManager_Logger::initialize: initialize logger */
void VIManager_Logger::initialize()
{
   int i;

   m_mmdagent = NULL;

   for(i = 0; i < VIMANAGERLOGGER_TEXTHEIGHT + 1; i++)
      m_history[i] = NULL;

   memset(&m_elem, 0, sizeof(FTGLTextDrawElements));
}

/* VIManager_Logger::clear: free logger */
void VIManager_Logger::clear()
{
   if(m_elem.vertices)
      free(m_elem.vertices);
   if(m_elem.texcoords)
      free(m_elem.texcoords);
   if(m_elem.indices)
      free(m_elem.indices);

   initialize();
}

/* VIManager_Logger::VIManager_Logger: constructor */
VIManager_Logger::VIManager_Logger()
{
   initialize();
}

/* VIManager_Logger::~VIManager_Logger: destructor */
VIManager_Logger::~VIManager_Logger()
{
   clear();
}

/* VIManager_Logger::setup: setup logger */
void VIManager_Logger::setup(MMDAgent *mmdagent)
{
   m_mmdagent = mmdagent;
}

/* VIManager_Logger::setTransition: store state transition */
bool VIManager_Logger::setTransition(VIManager_Arc *arc)
{
   int i;
   VIManager_Arc *tmp1, *tmp2;

   if(arc == NULL)
      return false;

   tmp2 = arc;
   for(i = 0; i < VIMANAGERLOGGER_TEXTHEIGHT + 1; i++) {
      tmp1 = m_history[i];
      m_history[i] = tmp2;
      tmp2 = tmp1;
      if(tmp2 == NULL)
         break;
   }

   return true;
}

/* VIManager_Logger::drawArc: render arc */
void VIManager_Logger::addArcToElement(unsigned int from, VIManager_Arc *arc, float x, float y)
{
   int i, j;
   char buf1[MMDAGENT_MAXBUFLEN], buf2[MMDAGENT_MAXBUFLEN];

   strcpy(buf1, arc->input_event_type);
   for(i = 0; i < arc->input_event_args.size; i++) {
      strcat(buf1, "|");
      for(j = 0; j < arc->input_event_args.argc[i]; j++) {
         if(j != 0)
            strcat(buf1, ",");
         strcat(buf1, arc->input_event_args.args[i][j]);
      }
   }
   if (MMDAgent_strlen(arc->output_command_args) > 0)
      sprintf(buf2, "%d %d %s %s|%s", from, arc->next_state->number, buf1, arc->output_command_type, arc->output_command_args);
   else
      sprintf(buf2, "%d %d %s %s", from, arc->next_state->number, buf1, arc->output_command_type);
   if (arc->variable_action) {
      strcat(buf2, " ");
      strcat(buf2, arc->variable_action);
   }

   m_mmdagent->getTextureFont()->getTextDrawElements(buf2, &m_elem, m_elem.textLen, x, y, 0.0f);
}

/* VIManager_Logger::drawVariable: render variable */
void VIManager_Logger::addVariableToElement(VIManager_Variable *v, float x, float y)
{
   char buf[MMDAGENT_MAXBUFLEN];

   sprintf(buf, "$%s=%s", v->name, v->value);
   m_mmdagent->getTextureFont()->getTextDrawElements(buf, &m_elem, m_elem.textLen, x, y, 0.0f);
}

/* VIManager_Logger::render: render log */
void VIManager_Logger::render(VIManager_State *currentState, VIManager_VList *currentVariableList)
{
#ifndef VIMANAGER_DONTRENDERDEBUG
   int i;
   VIManager_Arc *arc;
   VIManager_Arc *prevArc;
   VIManager_Variable *v;
   size_t indexLen[3];
   GLfloat vertices[12];
   GLubyte indices[] = { 0, 1, 2, 0, 2, 3 };


   if(m_mmdagent == NULL || currentState == NULL)
      return;

   /* reset rendering data for text */
   m_elem.textLen = 0;
   m_elem.numIndices = 0;

   /* prepare text drawing element */
   for (i = 0; i < VIMANAGERLOGGER_TEXTHEIGHT; i++) {
      arc = m_history[i];
      if (arc == NULL) break;
      prevArc = m_history[i + 1];
      addArcToElement(prevArc == NULL ? VIMANAGER_STARTSTATE : prevArc->next_state->number, arc, 0.0f, VIMANAGERLOGGER_LINESTEP * i);
   }
   indexLen[0] = m_elem.numIndices;
   for (i = 0, arc = currentState->arc_list.head; i < VIMANAGERLOGGER_TEXTHEIGHT && arc != NULL; i++, arc = arc->next) {
      addArcToElement(currentState->number, arc, 0.0f, - VIMANAGERLOGGER_LINESTEP * i);
   }
   indexLen[1] = m_elem.numIndices - indexLen[0];
   if (currentVariableList != NULL) {
      for (v = currentVariableList->head; v != NULL; i++, v = v->next) {
         addVariableToElement(v, 0.0f, -VIMANAGERLOGGER_LINESTEP * i);
      }
   }
   indexLen[2] = m_elem.numIndices - (indexLen[0] + indexLen[1]);

   /* start of drawing */
   glDisable(GL_CULL_FACE);
   glDisable(GL_LIGHTING);
   glEnable(GL_TEXTURE_2D);
   glActiveTexture(GL_TEXTURE0);
   glClientActiveTexture(GL_TEXTURE0);
   glEnableClientState(GL_VERTEX_ARRAY);

   /* show the history */
   glPushMatrix();
   glTranslatef(VIMANAGERLOGGER_POSITION1);
   glRotatef(VIMANAGERLOGGER_ROTATE1);
   glScalef(VIMANAGERLOGGER_SCALE, VIMANAGERLOGGER_SCALE, VIMANAGERLOGGER_SCALE);
   glColor4f(VIMANAGERLOGGER_BGCOLOR1);
   glBindTexture(GL_TEXTURE_2D, 0);
   vertices[0] = 0;
   vertices[1] = 0;
   vertices[2] = 0;
   vertices[3] = VIMANAGERLOGGER_WIDTH1;
   vertices[4] = 0;
   vertices[5] = 0;
   vertices[6] = VIMANAGERLOGGER_WIDTH1;
   vertices[7] = VIMANAGERLOGGER_HEIGHT1;
   vertices[8] = 0;
   vertices[9] = 0;
   vertices[10] = VIMANAGERLOGGER_HEIGHT1;
   vertices[11] = 0;
   glVertexPointer(3, GL_FLOAT, 0, vertices);
   glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glVertexPointer(3, GL_FLOAT, 0, m_elem.vertices);
   glTexCoordPointer(2, GL_FLOAT, 0, m_elem.texcoords);
   glTranslatef(0.5f, VIMANAGERLOGGER_LINESTEP * 0.7f, 0.01f);
   glColor4f(VIMANAGERLOGGER_TEXTCOLOR1);
   glBindTexture(GL_TEXTURE_2D, m_mmdagent->getTextureFont()->getTextureID());
   glDrawElements(GL_TRIANGLES, indexLen[0], GL_UNSIGNED_INT, (const GLvoid *) m_elem.indices);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glPopMatrix();

   /* show the future */
   glPushMatrix();
   glTranslatef(VIMANAGERLOGGER_POSITION2);
   glRotatef(VIMANAGERLOGGER_ROTATE2);
   glScalef(VIMANAGERLOGGER_SCALE, VIMANAGERLOGGER_SCALE, VIMANAGERLOGGER_SCALE);
   glColor4f(VIMANAGERLOGGER_BGCOLOR2);
   glBindTexture(GL_TEXTURE_2D, 0);
   vertices[0] = 0;
   vertices[1] = 0;
   vertices[2] = 0;
   vertices[3] = VIMANAGERLOGGER_WIDTH2;
   vertices[4] = 0;
   vertices[5] = 0;
   vertices[6] = VIMANAGERLOGGER_WIDTH2;
   vertices[7] = VIMANAGERLOGGER_HEIGHT2;
   vertices[8] = 0;
   vertices[9] = 0;
   vertices[10] = VIMANAGERLOGGER_HEIGHT2;
   vertices[11] = 0;
   glVertexPointer(3, GL_FLOAT, 0, vertices);
   glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indices);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glVertexPointer(3, GL_FLOAT, 0, m_elem.vertices);
   glTexCoordPointer(2, GL_FLOAT, 0, m_elem.texcoords);
   glTranslatef(0.5f, VIMANAGERLOGGER_HEIGHT2 - VIMANAGERLOGGER_LINESTEP * 1.2f, 0.01f);
   glColor4f(VIMANAGERLOGGER_TEXTCOLOR2);
   glBindTexture(GL_TEXTURE_2D, m_mmdagent->getTextureFont()->getTextureID());
   glDrawElements(GL_TRIANGLES, indexLen[1], GL_UNSIGNED_INT, (const GLvoid *) & (m_elem.indices[indexLen[0]]));
   /* draw variables */
   if (currentVariableList != NULL) {
      glColor4f(VIMANAGERLOGGER_TEXTCOLOR3);
      glDrawElements(GL_TRIANGLES, indexLen[2], GL_UNSIGNED_INT, (const GLvoid *) & (m_elem.indices[indexLen[0] + indexLen[1]]));
   }
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
   glPopMatrix();

   /* end of draw */
   glDisableClientState(GL_VERTEX_ARRAY);
   glDisable(GL_TEXTURE_2D);
   glEnable(GL_LIGHTING);
   glEnable(GL_CULL_FACE);
#endif /* !VIMANAGER_DONTRENDERDEBUG */
}
