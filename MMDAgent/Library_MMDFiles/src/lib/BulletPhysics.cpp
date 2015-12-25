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

#include "MMDFiles.h"
#include "BulletCollision/CollisionShapes/btShapeHull.h"

/* BulletPhysics::initialize: initialize BulletPhysics */
void BulletPhysics::initialize()
{
   m_collisionConfig = NULL;
   m_dispatcher = NULL;
   m_overlappingPairCache = NULL;
   m_solver = NULL;
   m_world = NULL;

   m_fps = 0;

   m_boxListEnabled = false;
   m_sphereListEnabled = false;
   m_sphereVertexInitialized = false;
}

/* BulletPhysics::clear: free BulletPhysics */
void BulletPhysics::clear()
{
   int i, numObject;
   btCollisionObject *obj;
   btRigidBody *body;

   if (m_world) {
      /* release remaining objects within the world */
      numObject = m_world->getNumCollisionObjects();
      for (i = 0; i < numObject; i++) {
         obj = m_world->getCollisionObjectArray()[i];
         body = btRigidBody::upcast(obj);
         if (body && body->getMotionState())
            delete body->getMotionState();
         m_world->removeCollisionObject(obj);
         delete obj;
      }
   }
   if (m_world)
      delete m_world;
   if (m_solver)
      delete m_solver;
   if (m_overlappingPairCache)
      delete m_overlappingPairCache;
   if (m_dispatcher)
      delete m_dispatcher;
   if (m_collisionConfig)
      delete m_collisionConfig;

   initialize();
}

/* BulletPhysics::BulletPhysics: constructor */
BulletPhysics::BulletPhysics()
{
   initialize();
}

/* BulletPhysics::~BulletPhysics: destructor */
BulletPhysics::~BulletPhysics()
{
   clear();
}

/* BulletPhysics::setup: initialize and setup BulletPhysics */
void BulletPhysics::setup(int simulationFps, float gravityFactor)
{
   float dist = 400.0f;

   clear();

   /* store values */
   m_fps = simulationFps;
   m_subStep = btScalar(1.0f / m_fps);

   /* make a collision configuration */
   m_collisionConfig = new btDefaultCollisionConfiguration();

   /* make a collision dispatcher from the configuration for sequenciall processing */
   m_dispatcher = new btCollisionDispatcher(m_collisionConfig);

   /* set broadphase */
   m_overlappingPairCache = new btAxisSweep3(btVector3(btScalar(-dist), btScalar(-dist), btScalar(-dist)), btVector3(btScalar(dist), btScalar(dist), btScalar(dist)), 1024);

   /* make a sequencial constraint solver */
   m_solver = new btSequentialImpulseConstraintSolver();

   /* create simulation world */
   m_world = new btDiscreteDynamicsWorld(m_dispatcher, m_overlappingPairCache, m_solver, m_collisionConfig);

   /* set default gravity */
   /* some tweak for the simulation to match that of MikuMikuDance */
   m_world->setGravity(btVector3(btScalar(0.0f), btScalar(-9.8f * gravityFactor), btScalar(0.0f)));

   /* a weird configuration to use 120Hz simulation */
   /* change the number of constraint solving iteration to be inversely propotional to simulation rate */
   /* is this a bug of bulletphysics? */
   m_world->getSolverInfo().m_numIterations = (int) (10 * 60 / m_fps);
}

/* BulletPhysics::update: step the simulation world forward */
void BulletPhysics::update(float deltaFrame)
{
   btScalar sec = deltaFrame / 30.0f; /* convert frame to second */

   if (sec > 1.0) {
      /* long pause, just move ahead at one step */
      m_world->stepSimulation(sec, 1, sec);
   } else {
      /* progress by (1.0/fps) sub step */
      m_world->stepSimulation(sec, m_fps, m_subStep);
   }
}

/* BulletPhysics::getWorld: get simulation world */
btDiscreteDynamicsWorld *BulletPhysics::getWorld()
{
   return m_world;
}

/* BulletPhysics::drawCube: draw a cube */
void BulletPhysics::drawCube()
{
#ifndef MMDFILES_DONTRENDERDEBUG
   static const GLfloat vertices[8][3] = {
      { -0.5f, -0.5f, -0.5f },
      { 0.5f, -0.5f, -0.5f },
      { 0.5f, 0.5f, -0.5f },
      { -0.5f, 0.5f, -0.5f },
      { -0.5f, -0.5f, 0.5f },
      { 0.5f, -0.5f, 0.5f },
      { 0.5f, 0.5f, 0.5f },
      { -0.5f, 0.5f, 0.5f }
   };
   static const GLubyte indices[] = {
      0, 1, 1, 2, 2, 3, 3, 0,
      4, 5, 5, 6, 6, 7, 7, 4,
      0, 4, 1, 5, 2, 6, 3, 7
   };
   glVertexPointer(3, GL_FLOAT, 0, vertices);
   glDrawElements(GL_LINES, 24, GL_UNSIGNED_BYTE, indices);
#endif /* !MMDFILES_DONTRENDERDEBUG */
}

/* BulletPhysics::drawSphere: draw a sphere */
void BulletPhysics::drawSphere()
{
#ifndef MMDFILES_DONTRENDERDEBUG
   static bool initialized = false;

   if (m_sphereVertexInitialized == false) {
      int i, j;
      double lat1, z1, zr1, lng, x, y;
      int c;
      c = 0;
      m_sphereVertices[c++] = 0.0f;
      m_sphereVertices[c++] = 0.0f;
      m_sphereVertices[c++] = -1.0f;
      for (i = 1; i < BULLETPHYSICS_SPHERELATS; i++) {
         lat1 = BULLETPHYSICS_PI * (-0.5 + (double)i / BULLETPHYSICS_SPHERELATS);
         z1 = sin(lat1);
         zr1 = cos(lat1);
         for (j = 0; j < BULLETPHYSICS_SPHERELONGS; j++) {
            lng = 2 * BULLETPHYSICS_PI * (double)(j - 1) / BULLETPHYSICS_SPHERELONGS;
            x = cos(lng);
            y = sin(lng);
            m_sphereVertices[c++] = (GLfloat)(x * zr1);
            m_sphereVertices[c++] = (GLfloat)(y * zr1);
            m_sphereVertices[c++] = (GLfloat)z1;
         }
      }
      m_sphereVertices[c++] = 0.0f;
      m_sphereVertices[c++] = 0.0f;
      m_sphereVertices[c++] = 1.0f;
      c = 0;
      for (i = 1; i < BULLETPHYSICS_SPHEREVERTEXNUM - 1; i++)
         m_sphereIndices[c++] = i;
      for (j = 0; j < BULLETPHYSICS_SPHERELONGS / 2; j++) {
         m_sphereIndices[c++] = BULLETPHYSICS_SPHEREVERTEXNUM - 1;
         for (i = BULLETPHYSICS_SPHERELATS - 1; i >= 1; i--)
            m_sphereIndices[c++] = 1 + BULLETPHYSICS_SPHERELONGS * (i - 1) + j;
         m_sphereIndices[c++] = 0;
         for (i = 1; i < BULLETPHYSICS_SPHERELATS; i++)
            m_sphereIndices[c++] = 1 + BULLETPHYSICS_SPHERELONGS * (i - 1) + j + (BULLETPHYSICS_SPHERELONGS / 2);
      }
      m_sphereIndices[c++] = BULLETPHYSICS_SPHEREVERTEXNUM - 1;
      m_sphereVertexInitialized = true;
   }
   glVertexPointer(3, GL_FLOAT, 0, m_sphereVertices);
   glDrawElements(GL_LINE_STRIP, BULLETPHYSICS_SPHEREINDEXNUM, GL_UNSIGNED_BYTE, m_sphereIndices);
#endif /* !MMDFILES_DONTRENDERDEBUG */
}

/* BulletPhysics::drawConvex: draw a convex shape */
void BulletPhysics::drawConvex(btConvexShape* shape)
{
#ifndef MMDFILES_DONTRENDERDEBUG
   btShapeHull *hull = new btShapeHull(shape);
   hull->buildHull(shape->getMargin());
   if (hull->numTriangles() > 0) {
      glVertexPointer(3, GL_FLOAT, sizeof(btVector3), hull->getVertexPointer());
      glDrawElements(GL_TRIANGLES, hull->numIndices(), GL_UNSIGNED_INT, hull->getIndexPointer());
   }
   delete hull;
#endif /* !MMDFILES_DONTRENDERDEBUG */
}

/* debugDisplay: render rigid bodies */
void BulletPhysics::debugDisplay()
{
#ifndef MMDFILES_DONTRENDERDEBUG
   int i;
   GLfloat color[] = {0.8f, 0.8f, 0.0f, 1.0f};
   GLint polygonMode[2];
   btRigidBody* body;
   btScalar m[16];
   btCollisionShape* shape;
   btVector3 halfExtent;
   const btSphereShape* sphereShape;
   float radius;
   int numObjects = m_world->getNumCollisionObjects();

   /* draw in wire frame */
   glGetIntegerv(GL_POLYGON_MODE, polygonMode);
   if (polygonMode[1] != GL_LINE)
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   glDisable(GL_TEXTURE_2D);
   glEnableClientState(GL_VERTEX_ARRAY);
   for (i = 0; i < numObjects; i++) {
      body = btRigidBody::upcast(m_world->getCollisionObjectArray()[i]);
      /* set color */
      if (body->getFlags() & BULLETPHYSICS_RIGIDBODYFLAGB) {
         color[0] = 0.3f;
         color[1] = 0.7f;
         color[2] = 0.0f;
      } else if (body->getFlags() & BULLETPHYSICS_RIGIDBODYFLAGP) {
         color[0] = 0.8f;
         color[1] = 0.4f;
         color[2] = 0.1f;
      } else if (body->getFlags() & BULLETPHYSICS_RIGIDBODYFLAGA) {
         color[0] = 0.8f;
         color[1] = 0.8f;
         color[2] = 0.0f;
      } else {
         color[0] = 0.3f;
         color[1] = 0.8f;
         color[2] = 0.0f;
         break;
      }
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
      /* draw */
      body->getWorldTransform().getOpenGLMatrix(m);
      shape = body->getCollisionShape();
      glPushMatrix();
      glMultMatrixf(m);
      switch (shape->getShapeType()) {
      case BOX_SHAPE_PROXYTYPE: {
         const btBoxShape* boxShape = static_cast<const btBoxShape*>(shape);
         halfExtent = boxShape->getHalfExtentsWithMargin();
         glScalef(2.0f * halfExtent[0], 2.0f * halfExtent[1], 2.0f * halfExtent[2]);
         drawCube();
         break;
      }
      case SPHERE_SHAPE_PROXYTYPE: {
         sphereShape = static_cast<const btSphereShape*>(shape);
         radius = sphereShape->getMargin(); /* radius doesn't include the margin, so draw with margin */
         glScalef(radius, radius, radius);
         drawSphere();
         break;
      }
      default:
         if (shape->isConvex()) {
            drawConvex((btConvexShape*)shape);
         }
         break;
      }
      glPopMatrix();
   }
   glDisableClientState(GL_VERTEX_ARRAY);
   if (polygonMode[1] != GL_LINE) {
      glPolygonMode(GL_FRONT_AND_BACK, polygonMode[1]);
   }
#endif /* !MMDFILES_DONTRENDERDEBUG */
}
