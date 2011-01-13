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

/* headers */

#include "MMDAgent.h"
#include "utils.h"

/* findPlane: calculate plane */
static void findPlane(GLfloat plane[4], TileTexture *t)
{
   GLfloat vec0x, vec0y, vec0z, vec1x, vec1y, vec1z;

   /* need 2 vectors to find cross product */
   vec0x = t->getSize(2, 0) - t->getSize(1, 0);
   vec0y = t->getSize(2, 1) - t->getSize(1, 1);
   vec0z = t->getSize(2, 2) - t->getSize(1, 2);

   vec1x = t->getSize(3, 0) - t->getSize(1, 0);
   vec1y = t->getSize(3, 1) - t->getSize(1, 1);
   vec1z = t->getSize(3, 2) - t->getSize(1, 2);

   /* find cross product to get A, B, and C of plane equation */
   plane[0] =   vec0y * vec1z - vec0z * vec1y;
   plane[1] = -(vec0x * vec1z - vec0z * vec1x);
   plane[2] =   vec0x * vec1y - vec0y * vec1x;
   plane[3] = -(plane[0] * t->getSize(1, 0) + plane[1] * t->getSize(1, 1) + plane[2] * t->getSize(1, 2));
}

/* shadowMatrix: calculate shadow projection matrix */
static void shadowMatrix(GLfloat shadowMat[4][4], GLfloat groundplane[4], GLfloat lightpos[4])
{
   GLfloat dot;

   /* find dot product between light position vector and ground plane normal */
   dot = groundplane[0] * lightpos[0] +
         groundplane[1] * lightpos[1] +
         groundplane[2] * lightpos[2] +
         groundplane[3] * lightpos[3];

   shadowMat[0][0] = dot - lightpos[0] * groundplane[0];
   shadowMat[1][0] = 0.f - lightpos[0] * groundplane[1];
   shadowMat[2][0] = 0.f - lightpos[0] * groundplane[2];
   shadowMat[3][0] = 0.f - lightpos[0] * groundplane[3];

   shadowMat[0][1] = 0.f - lightpos[1] * groundplane[0];
   shadowMat[1][1] = dot - lightpos[1] * groundplane[1];
   shadowMat[2][1] = 0.f - lightpos[1] * groundplane[2];
   shadowMat[3][1] = 0.f - lightpos[1] * groundplane[3];

   shadowMat[0][2] = 0.f - lightpos[2] * groundplane[0];
   shadowMat[1][2] = 0.f - lightpos[2] * groundplane[1];
   shadowMat[2][2] = dot - lightpos[2] * groundplane[2];
   shadowMat[3][2] = 0.f - lightpos[2] * groundplane[3];

   shadowMat[0][3] = 0.f - lightpos[3] * groundplane[0];
   shadowMat[1][3] = 0.f - lightpos[3] * groundplane[1];
   shadowMat[2][3] = 0.f - lightpos[3] * groundplane[2];
   shadowMat[3][3] = dot - lightpos[3] * groundplane[3];
}

/* TileTexture::resetDisplayList: reset display list */
void TileTexture::resetDisplayList()
{
   if (m_listIndexValid) {
      glDeleteLists(m_listIndex, 1);
      m_listIndexValid = false;
   }
}

/* TileTexture::initialize: initialize texture */
void TileTexture::initialize()
{
   int i, j;

   m_isLoaded = false;
   m_listIndex = 0;
   m_listIndexValid = false;

   for (i = 0; i < 4; i++)
      for (j = 0; j < 3; j++)
         m_vertices[i][j] = 0.0f;
   m_numx = 1.0f;
   m_numy = 1.0f;
}

/* TileTexture::clear: free texture */
void TileTexture::clear()
{
   if (m_isLoaded)
      m_texture.release();
   initialize();
}

/* TileTexture::TileTexture: constructor */
TileTexture::TileTexture()
{
   initialize();
}

/* TileTexture: destructor */
TileTexture::~TileTexture()
{
   clear();
}

/* TileTexture::load: load a texture from file name (wide char) */
bool TileTexture::load(char *file)
{
   if (file == NULL)
      return false;
   if (strlen(file) <= 0)
      return false;

   if(m_texture.load(file) == false)
      return false;

   m_isLoaded = true;
   resetDisplayList();

   return true;
}

/* TileTexture::render: render the textures */
void TileTexture::render(bool cullFace, const float normal[3])
{
   static const GLfloat color[] = { 0.65f, 0.65f, 0.65f, 1.0f };

   if (m_isLoaded == false) return;

   if (m_listIndexValid) {
      /* call display list */
      glCallList(m_listIndex);
      return;
   }

   /* create display list  */
   m_listIndex = glGenLists(1); /* get display list index */
   glNewList(m_listIndex, GL_COMPILE);

   /* register rendering command */
   if (!cullFace)
      glDisable(GL_CULL_FACE);

   glEnable(GL_TEXTURE_2D);
   glPushMatrix();
   glNormal3f(normal[0], normal[1], normal[2]);
   glBindTexture(GL_TEXTURE_2D, m_texture.getID());
   glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
   glBegin(GL_QUADS);
   glTexCoord2f(0.0, m_numy);
   glVertex3fv(m_vertices[0]);
   glTexCoord2f(m_numx, m_numy);
   glVertex3fv(m_vertices[1]);
   glTexCoord2f(m_numx, 0.0);
   glVertex3fv(m_vertices[2]);
   glTexCoord2f(0.0, 0.0);
   glVertex3fv(m_vertices[3]);
   glEnd();
   glPopMatrix();
   glDisable(GL_TEXTURE_2D);

   if (!cullFace)
      glEnable(GL_CULL_FACE);

   /* end of regist */
   glEndList();
   m_listIndexValid = true;
}

/* TileTexture::getSize: get texture size */
GLfloat TileTexture::getSize(int i, int j)
{
   return m_vertices[i][j];
}

/* TileTexture::setSize: set texture size */
void TileTexture::setSize(float v00, float v01, float v02,
                          float v10, float v11, float v12,
                          float v20, float v21, float v22,
                          float v30, float v31, float v32,
                          float numx, float numy)
{
   m_vertices[0][0] = v00;
   m_vertices[0][1] = v01;
   m_vertices[0][2] = v02;
   m_vertices[1][0] = v10;
   m_vertices[1][1] = v11;
   m_vertices[1][2] = v12;
   m_vertices[2][0] = v20;
   m_vertices[2][1] = v21;
   m_vertices[2][2] = v22;
   m_vertices[3][0] = v30;
   m_vertices[3][1] = v31;
   m_vertices[3][2] = v32;

   m_numx = numx;
   m_numy = numy;

   resetDisplayList();
}

/* Stage::makeFloorBody: create a rigid body for floor */
void Stage::makeFloorBody(float width, float depth)
{
   btVector3 localInertia(0, 0, 0);
   btScalar mass = 0.0f;
   btCollisionShape *colShape;
   btTransform startTransform;
   btDefaultMotionState* myMotionState;

   releaseFloorBody();

   colShape = new btBoxShape(btVector3(width, 10.0f, depth)); /* <- can free memory ? */
   if (mass != 0.0f)
      colShape->calculateLocalInertia(mass, localInertia);
   startTransform.setIdentity();
   startTransform.setOrigin(btVector3(0.0f, -9.99f, 0.0f));
   myMotionState = new btDefaultMotionState(startTransform);
   btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
   rbInfo.m_linearDamping = 0.5f;
   rbInfo.m_angularDamping = 0.5f;
   rbInfo.m_restitution = 0.8f;
   rbInfo.m_friction = 0.5f;
   m_floorBody = new btRigidBody(rbInfo);

   if (m_bullet)
      m_bullet->getWorld()->addRigidBody(m_floorBody);
}

/* Stage::releaseFloorBody: release rigid body for floor */
void Stage::releaseFloorBody()
{
   if (m_floorBody) {
      if (m_floorBody->getMotionState())
         delete m_floorBody->getMotionState();
      if (m_bullet)
         m_bullet->getWorld()->removeCollisionObject(m_floorBody);
      delete m_floorBody;
      m_floorBody = NULL;
   }
}

/* Stage::initialize: initialize stage */
void Stage::initialize()
{
   int i, j;

   m_hasPMD = false;
   m_listIndexPMD = 0;
   m_listIndexPMDValid = false;
   m_bullet = NULL;
   m_floorBody = NULL;
   for (i = 0; i < 4 ; i++)
      for (j = 0; j < 4; j++)
         m_floorShadow[i][j] = 0.0f;
}

/* Stage::clear: free stage */
void Stage::clear()
{
   releaseFloorBody();
   initialize();
}

/* Stage::Stage: constructor */
Stage::Stage()
{
   initialize();
}

/* Stage::~Stage: destructor */
Stage::~Stage()
{
   clear();
}

/* Stage::setSize: set size of floor and background */
void Stage::setSize(float *size, float numx, float numy)
{
   m_floor.setSize(-size[0], 0.0f, size[1],
                   size[0], 0.0f, size[1],
                   size[0], 0.0f, -size[1],
                   -size[0], 0.0f, -size[1],
                   numx, numy);
   m_background.setSize(-size[0], 0.0f, -size[1],
                        size[0], 0.0f, -size[1],
                        size[0], size[2], -size[1],
                        -size[0], size[2], -size[1],
                        numx, numy);
   makeFloorBody(size[0], size[1]);
}

/* Stage::loadFloor: load floor image */
bool Stage::loadFloor(char *file, BulletPhysics *bullet)
{
   if (m_bullet == NULL)
      m_bullet = bullet;

   if(m_floor.load(file) == false) {
      g_logger.log("! Error: Stage: unable to load floor \"%s\"", file);
      return false;
   }
   if (m_hasPMD) {
      m_pmd.release();
      m_hasPMD = false;
   }

   return true;
}

/* Stage::loadBackground: load background image */
bool Stage::loadBackground(char *file, BulletPhysics *bullet)
{
   if (m_bullet == NULL)
      m_bullet = bullet;

   if(m_background.load(file) == false) {
      g_logger.log("! Error: Stage: unable to load background \"%s\"", file);
      return false;
   }
   if (m_hasPMD) {
      m_pmd.release();
      m_hasPMD = false;
   }

   return true;
}

/* Stage::loadStagePMD: load stage pmd */
bool Stage::loadStagePMD(char *file, BulletPhysics *bullet, SystemTexture *systex)
{
   if (m_bullet == NULL)
      m_bullet = bullet;

   if(m_pmd.load(file, m_bullet, systex) == false) {
      g_logger.log("! Error: Stage: unable to load stage PMD \"%s\"", file);
      return false;
   }
   m_pmd.setToonFlag(false);
   m_pmd.updateSkin();
   m_hasPMD = true;
   if (m_listIndexPMDValid) {
      glDeleteLists(m_listIndexPMD, 1);
      m_listIndexPMDValid = false;
   }

   return true;
}

/* Stage::renderFloor: render the floor */
void Stage::renderFloor()
{
   const float normal[3] = {0.0f, 1.0f, 0.0f};

   if (m_hasPMD)
      renderPMD();
   else
      m_floor.render(false, normal);
}

/* Stage::renderBackground: render the background */
void Stage::renderBackground()
{
   const float normal[3] = {0.0f, 0.0f, 1.0f};

   if (!m_hasPMD)
      m_background.render(true, normal);
}

/* Stage::renderPMD: render the stage pmd */
void Stage::renderPMD()
{
   if (m_listIndexPMDValid) {
      glCallList(m_listIndexPMD);
      return;
   }

   m_listIndexPMD = glGenLists(1);
   glNewList(m_listIndexPMD, GL_COMPILE);
   glPushMatrix();
   m_pmd.renderModel();
   glPopMatrix();
   glEndList();
   m_listIndexPMDValid = true;
}

/* Stage::updateShadowMatrix: update shadow projection matrix */
void Stage::updateShadowMatrix(float lightDirection[4])
{
   GLfloat floorPlane[4];

   findPlane(floorPlane, &m_floor);
   shadowMatrix(m_floorShadow, floorPlane, lightDirection);
}

/* Stage::getShadowMatrix: get shadow projection matrix */
GLfloat *Stage::getShadowMatrix()
{
   return (GLfloat *) m_floorShadow;
}

