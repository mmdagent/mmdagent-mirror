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

/* PMDModel::parse: initialize and load from data memories */
bool PMDModel::parse(const unsigned char *data, unsigned long size, BulletPhysics *bullet, SystemTexture *systex, const char *dir)
{
   const unsigned char *start = data;
   FILE *fp;
   bool ret = true;
   unsigned int i;
   btQuaternion defaultRot(btScalar(0.0f), btScalar(0.0f), btScalar(0.0f), btScalar(1.0f));

   char sjisBuff[257];

   PMDFile_Header *fileHeader;
   PMDFile_Vertex *fileVertex;
   PMDFile_Material *fileMaterial;
   PMDFile_Bone *fileBone;
   PMDFile_IK *fileIK;
   PMDFile_Face *fileFace;

   unsigned char numFaceDisp;
   unsigned char numBoneFrameDisp;
   unsigned int numBoneDisp;

   char buf[MMDFILES_MAXBUFLEN]; /* for toon texture */

   unsigned char englishNameExist;
   char *exToonBMPName;

   btVector3 tmpVector;
   PMDFile_RigidBody *fileRigidBody;
   PMDFile_Constraint *fileConstraint;

   unsigned short j, k, l;
   float f;
   unsigned int surfaceFrom, surfaceTo;
   char *name;
   PMDBone *bMatch;

   /* clear memory */
   clear();
   m_hasSingleSphereMap = false;
   m_hasMultipleSphereMap = false;
   m_baseFace = NULL;
   m_centerBone = NULL;

   /* reset root bone's rotation */
   m_rootBone.setCurrentRotation(&defaultRot);
   m_rootBone.update();

   /* set Bullet Physics */
   m_bulletPhysics = bullet;

   /* reset toon texture IDs by system default textures */
   for (j = 0; j < SYSTEMTEXTURE_NUMFILES; j++)
      m_toonTextureID[j] = systex->getTextureID(j);

   /* header */
   fileHeader = (PMDFile_Header *) data;
   if (fileHeader->magic[0] != 'P' || fileHeader->magic[1] != 'm' || fileHeader->magic[2] != 'd')
      return false;
   if (fileHeader->version != 1.0f)
      return false;
   /* name */
   strncpy(sjisBuff, fileHeader->name, 20);
   sjisBuff[20] = '\0';
   m_name = MMDFiles_strdup_from_sjis_to_utf8(sjisBuff);
   /* comment */
   strncpy(sjisBuff, fileHeader->comment, 256);
   sjisBuff[256] = '\0';
   m_comment = MMDFiles_strdup_from_sjis_to_utf8(sjisBuff);
   data += sizeof(PMDFile_Header);

   /* vertex data and bone weights */
   /* relocate as separated list for later OpenGL calls */
   m_numVertex = *((unsigned int *) data);
   data += sizeof(unsigned int);
   m_vertexList = new btVector3[m_numVertex];
   m_normalList = new btVector3[m_numVertex];
   m_texCoordList = (TexCoord *) malloc(sizeof(TexCoord) * m_numVertex);
   m_bone1List = (short *) malloc(sizeof(short) * m_numVertex);
   m_bone2List = (short *) malloc(sizeof(short) * m_numVertex);
   m_boneWeight1 = (float *) malloc(sizeof(float) * m_numVertex);
   m_noEdgeFlag = (bool *) malloc(sizeof(bool) * m_numVertex);
   fileVertex = (PMDFile_Vertex *) data;
   for (i = 0; i < m_numVertex; i++) {
      m_vertexList[i].setValue(btScalar(fileVertex[i].pos[0]), btScalar(fileVertex[i].pos[1]), btScalar(fileVertex[i].pos[2]));
      m_normalList[i].setValue(btScalar(fileVertex[i].normal[0]), btScalar(fileVertex[i].normal[1]), btScalar(fileVertex[i].normal[2]));
      m_texCoordList[i].u = fileVertex[i].uv[0];
      m_texCoordList[i].v = fileVertex[i].uv[1];
      m_bone1List[i] = fileVertex[i].boneID[0];
      m_bone2List[i] = fileVertex[i].boneID[1];
      m_boneWeight1[i] = fileVertex[i].boneWeight1 * 0.01f;
      m_noEdgeFlag[i] = fileVertex[i].noEdgeFlag ? true : false;
   }
   data += sizeof(PMDFile_Vertex) * m_numVertex;

   /* surface data, 3 vertex indices for each */
   m_numSurface = *((unsigned int *) data);
   data += sizeof(unsigned int);
   m_surfaceList = (unsigned short *) malloc(sizeof(unsigned short) * m_numSurface);
   memcpy(m_surfaceList, data, sizeof(unsigned short) * m_numSurface);
   data += sizeof(unsigned short) * m_numSurface;

   /* material data (color, texture, toon parameter, edge flag) */
   m_numMaterial = *((unsigned int *) data);
   data += sizeof(unsigned int);
   m_material = new PMDMaterial[m_numMaterial];
   fileMaterial = (PMDFile_Material *) data;
   surfaceFrom = 0;
   for (i = 0; i < m_numMaterial; i++) {
      if (!m_material[i].setup(&fileMaterial[i], &m_textureLoader, dir, surfaceFrom, m_vertexList, m_surfaceList)) {
         /* ret = false; */
      }
      surfaceFrom += m_material[i].getNumSurface();
   }
   data += sizeof(PMDFile_Material) * m_numMaterial;

   /* bone data */
   m_numBone = *((unsigned short *) data);
   data += sizeof(unsigned short);
   m_boneList = new PMDBone[m_numBone];
   fileBone = (PMDFile_Bone *) data;
   for (i = 0; i < m_numBone; i++) {
      if (!m_boneList[i].setup(&(fileBone[i]), m_boneList, m_numBone, &m_rootBone))
         ret = false;
      if (MMDFiles_strequal(m_boneList[i].getName(), PMDMODEL_CENTERBONENAME) == true && m_centerBone == NULL)
         m_centerBone = &(m_boneList[i]);
   }
   if (!m_centerBone && m_numBone >= 1) {
      /* if no bone is named "center," use the first bone as center */
      m_centerBone = &(m_boneList[0]);
   }
   data += sizeof(PMDFile_Bone) * m_numBone;
   /* make ordered bone list */
   if (m_numBone > 0) {
      m_orderedBoneList = (PMDBone **) malloc(sizeof(PMDBone *) * m_numBone);
      k = 0;
      for (j = 0; j < m_numBone; j++) {
         if (fileBone[j].parentBoneID == -1)
            m_orderedBoneList[k++] = &(m_boneList[j]);
      }
      l = k;
      for (j = 0; j < m_numBone; j++) {
         if (fileBone[j].parentBoneID != -1)
            m_orderedBoneList[l++] = &(m_boneList[j]);
      }
      do {
         i = 0;
         for (j = k; j < m_numBone; j++) {
            for (l = 0; l < j; l++) {
               if (m_orderedBoneList[l] == m_orderedBoneList[j]->getParentBone())
                  break;
            }
            if (l >= j) {
               bMatch = m_orderedBoneList[j];
               if (j < m_numBone - 1)
                  memmove(&m_orderedBoneList[j], &m_orderedBoneList[j + 1], sizeof(PMDBone *) * (m_numBone - 1 - j));
               m_orderedBoneList[m_numBone - 1] = bMatch;
               i = 1;
            }
         }
      } while (i != 0);
   }
   /* calculate bone offset after all bone positions are loaded */
   for (i = 0; i < m_numBone; i++)
      m_boneList[i].computeOffset();

   /* IK data */
   m_numIK = *((unsigned short *) data);
   data += sizeof(unsigned short);
   if (m_numIK > 0) {
      m_IKList = new PMDIK[m_numIK];
      for (i = 0; i < m_numIK; i++) {
         fileIK = (PMDFile_IK *) data;
         data += sizeof(PMDFile_IK);
         m_IKList[i].setup(fileIK, (short *)data, m_boneList);
         data += sizeof(short) * fileIK->numLink;
      }
   }

   /* face data */
   m_numFace = *((unsigned short *) data);
   data += sizeof(unsigned short);
   if (m_numFace > 0) {
      m_faceList = new PMDFace[m_numFace];
      for (i = 0; i < m_numFace; i++) {
         fileFace = (PMDFile_Face *)data;
         data += sizeof(PMDFile_Face);
         m_faceList[i].setup(fileFace, (PMDFile_Face_Vertex *) data);
         data += sizeof(PMDFile_Face_Vertex) * fileFace->numVertex;
      }
      m_baseFace = &(m_faceList[0]); /* store base face */
      /* convert base-relative index to the index of original vertices */
      for (i = 1; i < m_numFace; i++)
         m_faceList[i].convertIndex(m_baseFace);
   }

   /* display names (skip) */
   /* indices for faces which should be displayed in "face" region */
   numFaceDisp = *((unsigned char *) data);
   data += sizeof(unsigned char) + sizeof(unsigned short) * numFaceDisp;
   /* bone frame names */
   numBoneFrameDisp = *((unsigned char *) data);
   data += sizeof(unsigned char) + 50 * numBoneFrameDisp;
   /* indices for bones which should be displayed in each bone region */
   numBoneDisp = *((unsigned int *) data);
   data += sizeof(unsigned int) + (sizeof(short) + sizeof(unsigned char)) * numBoneDisp;

   /* end of base format */
   /* check for remaining data */
   if ((unsigned long) data - (unsigned long) start >= size) {
      /* no extension data remains */
      m_numRigidBody = 0;
      m_numConstraint = 0;
      /* assign default toon textures for toon shading */
      for (j = 0; j <= 10; j++) {
         if (j == 0)
            sprintf(buf, "%s%ctoon0.bmp", dir, MMDFILES_DIRSEPARATOR);
         else
            sprintf(buf, "%s%ctoon%02d.bmp", dir, MMDFILES_DIRSEPARATOR, j);
         /* if "toon??.bmp" exist at the same directory as PMD file, use it */
         /* if not exist or failed to read, use system default toon textures */
         fp = MMDFiles_fopen(buf, "rb");
         if (fp != NULL) {
            fclose(fp);
            if (m_localToonTexture[j].load(buf) == true)
               m_toonTextureID[j] = m_localToonTexture[j].getID();
         }
      }
   } else {
      /* English display names (skip) */
      englishNameExist = *((unsigned char *) data);
      data += sizeof(unsigned char);
      if (englishNameExist != 0) {
         /* model name and comments in English */
         data += 20 + 256;
         /* bone names in English */
         data += 20 * m_numBone;
         /* face names in English */
         if (m_numFace > 0) data += 20 * (m_numFace - 1); /* "base" not included in English list */
         /* bone frame names in English */
         data += 50 * numBoneFrameDisp;
      }

      /* toon texture file list (replace toon01.bmp - toon10.bmp) */
      /* the "toon0.bmp" should be loaded separatedly */
      sprintf(buf, "%s%ctoon0.bmp", dir, MMDFILES_DIRSEPARATOR);
      fp = MMDFiles_fopen(buf, "rb");
      if (fp != NULL) {
         fclose(fp);
         if (m_localToonTexture[0].load(buf) == true)
            m_toonTextureID[0] = m_localToonTexture[0].getID();
      }
      for (i = 1; i <= 10; i++) {
         exToonBMPName = (char *) data;
         sprintf(buf, "%s%c%s", dir, MMDFILES_DIRSEPARATOR, exToonBMPName);
         fp = MMDFiles_fopen(buf, "rb");
         if (fp != NULL) {
            fclose(fp);
            if (m_localToonTexture[i].load(buf) == true)
               m_toonTextureID[i] = m_localToonTexture[i].getID();
         }
         data += 100;
      }

      /* check for remaining data */
      if ((unsigned long) data - (unsigned long) start >= size) {
         /* no rigid body / constraint data exist */
         m_numRigidBody = 0;
         m_numConstraint = 0;
      } else if (!m_bulletPhysics) {
         /* check if we have given a bulletphysics engine */
         m_numRigidBody = 0;
         m_numConstraint = 0;
      } else {
         /* get model offset */
         m_rootBone.getOffset(&tmpVector);
         /* update bone matrix to apply root bone offset to bone position */
         for (i = 0; i < m_numBone; i++)
            m_orderedBoneList[i]->update();

         /* Bullet Physics rigidbody data */
         m_numRigidBody = *((unsigned int *) data);
         data += sizeof(unsigned int);
         if (m_numRigidBody > 0) {
            m_rigidBodyList = new PMDRigidBody[m_numRigidBody];
            fileRigidBody = (PMDFile_RigidBody *) data;
            for (i = 0; i < m_numRigidBody; i++) {
               if (! m_rigidBodyList[i].setup(&fileRigidBody[i], (fileRigidBody[i].boneID == 0xFFFF) ? & (m_boneList[0]) : & (m_boneList[fileRigidBody[i].boneID])))
                  ret = false;
               m_rigidBodyList[i].joinWorld(m_bulletPhysics->getWorld());
               /* flag the bones under simulation in order to skip IK solving for those bones */
               if (fileRigidBody[i].type != 0 && fileRigidBody[i].boneID != 0xFFFF)
                  m_boneList[fileRigidBody[i].boneID].setSimulatedFlag(true);
            }
            data += sizeof(PMDFile_RigidBody) * m_numRigidBody;
         }

         /* BulletPhysics constraint data */
         m_numConstraint = *((unsigned int *) data);
         data += sizeof(unsigned int);
         if (m_numConstraint > 0) {
            m_constraintList = new PMDConstraint[m_numConstraint];
            fileConstraint = (PMDFile_Constraint *) data;
            for (i = 0; i < m_numConstraint; i++) {
               if (!m_constraintList[i].setup(&fileConstraint[i], m_rigidBodyList, &tmpVector)) /* apply model offset */
                  ret = false;
               m_constraintList[i].joinWorld(m_bulletPhysics->getWorld());
            }
            data += sizeof(PMDFile_Constraint) * m_numConstraint;
         }
      }
   }

   if (ret == false) return false;

#ifdef MMDFILES_CONVERTCOORDINATESYSTEM
   /* left-handed system: PMD, DirectX */
   /* right-handed system: OpenGL, bulletphysics */
   /* convert the left-handed vertices to right-handed system */
   /* 1. vectors should be (x, y, -z) */
   /* 2. rotation should be (-rx, -ry, z) */
   /* 3. surfaces should be reversed */
   /* reverse Z value on vertices */
   for (i = 0; i < m_numVertex; i++) {
      m_vertexList[i].setZ(-m_vertexList[i].z());
      m_normalList[i].setZ(-m_normalList[i].z());
   }
   /* reverse surface, swapping vartex order [0] and [1] in a triangle surface */
   for (i = 0; i < m_numSurface; i += 3) {
      j = m_surfaceList[i];
      m_surfaceList[i] = m_surfaceList[i + 1];
      m_surfaceList[i + 1] = j;
   }
#endif /* MMDFILES_CONVERTCOORDINATESYSTEM */

   /* prepare work area */
   /* transforms for skinning */
   m_boneSkinningTrans = new btTransform[m_numBone];
   /* surface list to be rendered at edge drawing (skip non-edge materials) */
   m_numSurfaceForEdge = 0;
   for (i = 0; i < m_numMaterial; i++)
      if (m_material[i].getEdgeFlag())
         m_numSurfaceForEdge += m_material[i].getNumSurface();

   /* prepare vertex buffers as work area */

   /* the fist buffer contains dynamic data: vertex, normal, toon coodinates, edge vertex */
   glGenBuffers(1, &m_vboBufDynamic);
   glBindBuffer(GL_ARRAY_BUFFER, m_vboBufDynamic);
   m_vboBufDynamicLen = (sizeof(btVector3) * 3 + sizeof(TexCoord)) * m_numVertex;
   glBufferData(GL_ARRAY_BUFFER, m_vboBufDynamicLen, NULL, GL_STATIC_DRAW);
   /* store initial values and set offset for each part */
   m_vboOffsetVertex = 0;
   glBufferSubData(GL_ARRAY_BUFFER, (GLintptr) m_vboOffsetVertex, sizeof(btVector3) * m_numVertex, m_vertexList);
   m_vboOffsetNormal = m_vboOffsetVertex + sizeof(btVector3) * m_numVertex;
   glBufferSubData(GL_ARRAY_BUFFER, (GLintptr) m_vboOffsetNormal, sizeof(btVector3) * m_numVertex, m_normalList);
   m_vboOffsetToon = m_vboOffsetNormal + sizeof(btVector3) * m_numVertex;
   m_vboOffsetEdge = m_vboOffsetToon + sizeof(TexCoord) * m_numVertex;

   /* the second buffer contains static data: texture coordinates */
   glGenBuffers(1, &m_vboBufStatic);
   glBindBuffer(GL_ARRAY_BUFFER, m_vboBufStatic);
   glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoord) * m_numVertex, m_texCoordList, GL_STATIC_DRAW);

   /* the third buffer contains static element data: original and for edge */
   glGenBuffers(1, &m_vboBufElement);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboBufElement);
   if (m_numSurfaceForEdge != 0) {
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * (m_numSurface + m_numSurfaceForEdge), NULL, GL_STATIC_DRAW);
      m_vboOffsetSurfaceForEdge = sizeof(unsigned short) * m_numSurface;
   }  else {
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * m_numSurface, NULL, GL_STATIC_DRAW);
      m_vboOffsetSurfaceForEdge = 0;
   }
   glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (GLintptr) NULL, sizeof(unsigned short) * m_numSurface, m_surfaceList);
   if (m_numSurfaceForEdge != 0) {
      surfaceFrom = 0;
      surfaceTo = m_vboOffsetSurfaceForEdge;
      for (i = 0; i < m_numMaterial; i++) {
         if (m_material[i].getEdgeFlag()) {
            glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, (GLintptr) surfaceTo, sizeof(unsigned short) * m_material[i].getNumSurface(), &(m_surfaceList[surfaceFrom]));
            surfaceTo += sizeof(unsigned short) * m_material[i].getNumSurface();
         }
         surfaceFrom += m_material[i].getNumSurface();
      }
   }
   /* unbind buffer */
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

   /* initialize material order */
   m_materialRenderOrder = (unsigned int *) malloc(sizeof(unsigned int) * m_numMaterial);
   m_materialDistance = (MaterialDistanceData *) malloc(sizeof(MaterialDistanceData) * m_numMaterial);
   for (i = 0; i < m_numMaterial; i++)
      m_materialRenderOrder[i] = i;
   /* check if spheremap is used (single or multiple) */
   for (i = 0; i < m_numMaterial; i++) {
      if (m_material[i].hasSingleSphereMap())
         m_hasSingleSphereMap = true;
      if (m_material[i].hasMultipleSphereMap())
         m_hasMultipleSphereMap = true;
   }

   /* make index of rotation-subjective bones (type == UNDER_ROTATE) and subjective bones */
   if (m_numBone > 0) {
      m_rotateBoneIDList = (unsigned short *) malloc(sizeof(unsigned short) * m_numBone);
      for (j = 0; j < m_numBone; j++) {
         if (m_boneList[j].getType() == UNDER_ROTATE)
            m_rotateBoneIDList[m_numRotateBone++] = j;
      }
      if(m_numRotateBone > 0) {
         do {
            i = 0;
            for (j = 0; j < m_numBone; j++) {
               for (k = 0; k < m_numRotateBone; k++) {
                  if (m_rotateBoneIDList[k] == j)
                     break;
               }
               if (k >= m_numRotateBone) {
                  for (k = 0; k < m_numRotateBone; k++) {
                     if (&(m_boneList[m_rotateBoneIDList[k]]) == m_boneList[j].getParentBone()) {
                        m_rotateBoneIDList[m_numRotateBone++] = j;
                        i = 1;
                        break;
                     }
                  }
               }
            }
         } while(i == 1);
      }
   }

   /* check if some IK solvers can be disabled since the bones are simulated by physics */
   if (m_numIK > 0) {
      m_IKSimulated = (bool *) malloc(sizeof(bool) * m_numIK);
      for (j = 0; j < m_numIK; j++) {
         /* this IK will be disabled when the leaf bone is controlled by physics simulation */
         m_IKSimulated[j] = m_IKList[j].isSimulated();
      }
   }

   /* mark motion independency for each bone */
   for (j = 0; j < m_numBone; j++)
      m_boneList[j].setMotionIndependency();

   /* make vertex assigned bone max distance */
   if (m_numBone > 0) {
      m_maxDistanceFromVertexAssignedBone = (float *) malloc(sizeof(float) * m_numBone);
      for (j = 0; j < m_numBone; j++)
         m_maxDistanceFromVertexAssignedBone[j] = -1.0f;
      for (i = 0; i < m_numVertex; i++) {
         if (m_boneWeight1[i] >= PMDMODEL_MINBONEWEIGHT) {
            m_boneList[m_bone1List[i]].getOriginPosition(&tmpVector);
            f = tmpVector.distance2(m_vertexList[i]);
            if (m_maxDistanceFromVertexAssignedBone[m_bone1List[i]] < f)
               m_maxDistanceFromVertexAssignedBone[m_bone1List[i]] = f;
         }
         if (m_boneWeight1[i] <= 1.0f - PMDMODEL_MINBONEWEIGHT) {
            m_boneList[m_bone2List[i]].getOriginPosition(&tmpVector);
            f = tmpVector.distance2(m_vertexList[i]);
            if (m_maxDistanceFromVertexAssignedBone[m_bone2List[i]] < f)
               m_maxDistanceFromVertexAssignedBone[m_bone2List[i]] = f;
         }
      }
      for (j = 0; j < m_numBone; j++)
         if (m_maxDistanceFromVertexAssignedBone[j] != -1.0f)
            m_maxDistanceFromVertexAssignedBone[j] = sqrtf(m_maxDistanceFromVertexAssignedBone[j]);
   }

   /* build name->entity index for fast lookup */
   for (j = 0; j < m_numBone; j++) {
      name = m_boneList[j].getName();
      m_name2bone.add(name, strlen(name), &(m_boneList[j]));
   }
   for (j = 0; j < m_numFace; j++) {
      name = m_faceList[j].getName();
      m_name2face.add(name, strlen(name), &(m_faceList[j]));
   }

   /* get maximum height */
   if (m_numVertex > 0) {
      m_maxHeight = m_vertexList[0].y();
      for (i = 1; i < m_numVertex; i++)
         if (m_maxHeight < m_vertexList[i].y())
            m_maxHeight = m_vertexList[i].y();
   }

   /* get bounding sphere step */
   m_boundingSphereStep = m_numVertex / PMDMODEL_BOUNDINGSPHEREPOINTS;
   if (m_boundingSphereStep < PMDMODEL_BOUNDINGSPHEREPOINTSMIN) m_boundingSphereStep = PMDMODEL_BOUNDINGSPHEREPOINTSMIN;
   if (m_boundingSphereStep > PMDMODEL_BOUNDINGSPHEREPOINTSMAX) m_boundingSphereStep = PMDMODEL_BOUNDINGSPHEREPOINTSMAX;

   /* simulation is currently off, so change bone status */
   if (!m_enableSimulation)
      setPhysicsControl(false);

   return true;
}
