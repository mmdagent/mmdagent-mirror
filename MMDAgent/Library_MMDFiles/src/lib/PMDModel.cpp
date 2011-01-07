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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "GLee.h"
#include "Define.h"
#include "PMDModel.h"

/* getDir: get directory from file path */
static bool getDir(const wchar_t *filePath, wchar_t *dirName, wchar_t *baseName, int bufLen)
{
   int i, len;
   bool found = false;
   wchar_t ch;

   dirName[0] = L'\0';
   baseName[0] = L'\0';

   if (filePath == NULL)
      return false;
   len = wcslen(filePath);
   if (len <= 0 || bufLen < len)
      return false;

   for (i = len; i >= 0; i--) {
      ch = filePath[i];
      if (found == false && (ch == L'\\' || ch == L'/' || ch == L':')) {
         found = true;
         wcsncpy(baseName, &(filePath[i+1]), len - i);
         dirName[i+1] = L'\0';
      }
      if (found)
         dirName[i] = filePath[i];
   }

   if (dirName[0] == L'\0')
      return false;
   else
      return true;
}

/* PMDModel::initialize: initialize PMDModel */
void PMDModel::initialize()
{
   m_name = NULL;
   m_comment = NULL;
   m_bulletPhysics = NULL;

   m_vertexList = NULL;
   m_normalList = NULL;
   m_texCoordList = NULL;
   m_bone1List = NULL;
   m_bone2List = NULL;
   m_boneWeight1 = NULL;
   m_noEdgeFlag = NULL;
   m_surfaceList = NULL;
   m_material = NULL;
   m_boneList = NULL;
   m_IKList = NULL;
   m_faceList = NULL;
   m_rigidBodyList = NULL;
   m_constraintList = NULL;

   m_boneSkinningTrans = NULL;
   m_skinnedVertexList = NULL;
   m_skinnedNormalList = NULL;
   m_toonTexCoordList = NULL;
   m_edgeVertexList = NULL;
   m_surfaceListForEdge = NULL;
   m_toonTexCoordListForShadowMap = NULL;
   m_rotateBoneIDList = NULL;
   m_IKSimulated = NULL;

   /* initial values for variables that should be kept at model change */
   m_enableSimulation = true;
   m_toon = false;
   m_globalAlpha = 1.0f;
   m_edgeOffset = 0.03f;
   m_selfShadowDrawing = false;
   m_selfShadowDensityCoef = 0.0f;
   m_edgeColor[0] = 0.0f;
   m_edgeColor[1] = 0.0f;
   m_edgeColor[2] = 0.0f;
   m_edgeColor[3] = 1.0f;
   m_rootBone.reset();
}

/* PMDModel::clear: free PMDModel */
void PMDModel::clear()
{
   int i;

   if (m_vertexList) {
      delete [] m_vertexList;
      m_vertexList = NULL;
   }
   if (m_normalList) {
      delete [] m_normalList;
      m_normalList = NULL;
   }
   if (m_texCoordList) {
      free(m_texCoordList);
      m_texCoordList = NULL;
   }
   if (m_bone1List) {
      free(m_bone1List);
      m_bone1List = NULL;
   }
   if (m_bone2List) {
      free(m_bone2List);
      m_bone2List = NULL;
   }
   if (m_boneWeight1) {
      free(m_boneWeight1);
      m_boneWeight1 = NULL;
   }
   if (m_noEdgeFlag) {
      free(m_noEdgeFlag);
      m_noEdgeFlag = NULL;
   }
   if (m_surfaceList) {
      free(m_surfaceList);
      m_surfaceList = NULL;
   }
   if (m_material) {
      delete [] m_material;
      m_material = NULL;
   }
   m_textureLoader.release();
   if (m_boneList) {
      delete [] m_boneList;
      m_boneList = NULL;
   }
   if (m_IKList) {
      delete [] m_IKList;
      m_IKList = NULL;
   }
   if (m_faceList) {
      delete [] m_faceList;
      m_faceList = NULL;
   }
   if (m_constraintList) {
      delete [] m_constraintList;
      m_constraintList = NULL;
   }
   if (m_rigidBodyList) {
      delete [] m_rigidBodyList;
      m_rigidBodyList = NULL;
   }

   if (m_boneSkinningTrans) {
      delete [] m_boneSkinningTrans;
      m_boneSkinningTrans = NULL;
   }
   if (m_skinnedVertexList) {
      delete [] m_skinnedVertexList;
      m_skinnedVertexList = NULL;
   }
   if (m_skinnedNormalList) {
      delete [] m_skinnedNormalList;
      m_skinnedNormalList = NULL;
   }
   if (m_toonTexCoordList) {
      free(m_toonTexCoordList);
      m_toonTexCoordList = NULL;
   }
   if (m_edgeVertexList) {
      delete [] m_edgeVertexList;
      m_edgeVertexList = NULL;
   }
   if (m_surfaceListForEdge) {
      free(m_surfaceListForEdge);
      m_surfaceListForEdge = NULL;
   }
   if (m_toonTexCoordListForShadowMap) {
      free(m_toonTexCoordListForShadowMap);
      m_toonTexCoordListForShadowMap = NULL;
   }
   if (m_rotateBoneIDList) {
      free(m_rotateBoneIDList);
      m_rotateBoneIDList = NULL;
   }
   if (m_IKSimulated) {
      free(m_IKSimulated);
      m_IKSimulated = NULL;
   }
   if(m_comment) {
      free(m_comment);
      m_comment = NULL;
   }
   if(m_name) {
      free(m_name);
      m_name = NULL;
   }

   for (i = 0; i < SYSTEMTEXTURE_NUMFILES; i++)
      m_localToonTexture[i].release();
   m_name2bone.release();
   m_name2face.release();
}

/* PMDModel::PMDModel: constructor */
PMDModel::PMDModel()
{
   initialize();
}

/* PMDModel::~PMDModel: destructor */
PMDModel::~PMDModel()
{
   clear();
}

/* PMDModel::setPhysicsEngine: set Bullet Physics */
void PMDModel::setPhysicsEngine(BulletPhysics *engine)
{
   m_bulletPhysics = engine;
}

/* PMDModel::load: load from file name (wide char) */
bool PMDModel::load(const wchar_t *filePath, SystemTexture *systex)
{
   FILE *fp;
   fpos_t size;
   unsigned char *data;
   wchar_t dirName[MAX_PATH], baseName[MAX_PATH], currentDir[MAX_PATH];
   bool ret;

   /* get model dir and open file */
   if (getDir(filePath, dirName, baseName, MAX_PATH)) {
      GetCurrentDirectory(MAX_PATH, currentDir);
      SetCurrentDirectory(dirName);
      fp = _wfopen(baseName, L"rb");
   } else {
      /* path contains no directory, just open it */
      fp = _wfopen(filePath, L"rb");
   }

   if (!fp) {
      if (dirName[0] != L'\0')
         SetCurrentDirectory(currentDir);
      return false;
   }

   /* get file size */
   fseek(fp, 0, SEEK_END);
   fgetpos(fp, &size);

   /* allocate memory for reading data */
   data = (unsigned char *) malloc((size_t) size);

   /* read all data */
   fseek(fp, 0, SEEK_SET);
   fread(data, 1, (size_t) size, fp);

   /* close file */
   fclose(fp);

   /* initialize and load from the data memories */
   ret = parse(data, (unsigned long) size, systex);

   /* release memory for reading */
   free(data);

   /* memorize the model directory (= current) */
   GetCurrentDirectory(MAX_PATH, m_modelDir);

   /* restore the current dir */
   if (dirName[0] != L'\0')
      SetCurrentDirectory(currentDir);

   return ret;
}

/* PMDModel::getBone: find bone data by name (multi-byte char) */
PMDBone *PMDModel::getBone(char *name)
{
   PMDBone *match = (PMDBone *) m_name2bone.findNearest(name);

   if (match && strcmp(match->getName(), name) == 0)
      return match;
   else
      return NULL;
}

/* PMDModel::getBone: find bone data by name (wide char) */
PMDBone *PMDModel::getBone(wchar_t *name)
{
   size_t converted;
   char buf[PMD_FILE_NAME_LEN+2];
   PMDBone *match;

   wcstombs_s(&converted, buf, PMD_FILE_NAME_LEN + 1, name, _TRUNCATE);
   match = (PMDBone *) m_name2bone.findNearest(buf);
   if (match && strcmp(match->getName(), buf) == 0)
      return match;
   else
      return NULL;
}

/* PMDModel::getFace: find face data by name (multi-byte char) */
PMDFace *PMDModel::getFace(char *name)
{
   PMDFace *match = (PMDFace *) m_name2face.findNearest(name);

   if (match && strcmp(match->getName(), name) == 0)
      return match;
   else
      return NULL;
}

/* PMDModel::getFace: find face data by name (wide char) */
PMDFace *PMDModel::getFace(wchar_t *name)
{
   size_t converted;
   char buf[PMD_FILE_NAME_LEN+2];
   PMDFace *match;

   wcstombs_s(&converted, buf, PMD_FILE_NAME_LEN + 1, name, _TRUNCATE);
   match = (PMDFace *) m_name2face.findNearest(buf);
   if (match && strcmp(match->getName(), buf) == 0)
      return match;
   else
      return NULL;
}

/* PMDModel::getChildBoneList: return list of child bones, in decent order */
int PMDModel::getChildBoneList(PMDBone **bone, unsigned short boneNum, PMDBone **childBoneList, unsigned short childBoneNumMax)
{
   unsigned short i, j;
   PMDBone *b;
   int n, k, l;
   bool updated;
   int iFrom, iTo;

   for (i = 0, n = 0; i < boneNum; i++) {
      b = bone[i];
      for (j = 0; j < m_numBone; j++) {
         if (m_boneList[j].getParentBone() == b) {
            if (n >= childBoneNumMax)
               return -1;
            childBoneList[n] = &(m_boneList[j]);
            n++;
         }
      }
   }

   updated = true;
   iFrom = 0;
   iTo = n;
   while (updated) {
      updated = false;
      for (k = iFrom; k < iTo; k++) {
         b = childBoneList[k];
         for (j = 0; j < m_numBone; j++) {
            if (m_boneList[j].getParentBone() == b) {
               for (l = 0; l < n; l++)
                  if (childBoneList[l] == &(m_boneList[j]))
                     break;
               if (l < n)
                  continue;
               if (n >= childBoneNumMax)
                  return -1;
               childBoneList[n] = &(m_boneList[j]);
               n++;
               updated = true;
            }
         }
      }
      iFrom = iTo;
      iTo = n;
   }

   return n;
}

/* PMDModel::setPhysicsControl switch bone control by physics simulation */
void PMDModel::setPhysicsControl(bool flag)
{
   unsigned long i;

   m_enableSimulation = flag;
   /* when true, align all rigid bodies to corresponding bone by setting Kinematics flag */
   /* when false, all rigid bodies will have their own motion states according to the model definition */
   for (i = 0; i < m_numRigidBody; i++)
      m_rigidBodyList[i].setKinematic(!flag);
}

/* PMDModel::release: free PMDModel */
void PMDModel::release()
{
   clear();
}

/* PMDModel:;setEdgeThin: set edge offset */
void PMDModel::setEdgeThin(float thin)
{
   m_edgeOffset = thin * 0.03f;
}

/* PMDModel:;setToonFlag: set toon rendering flag */
void PMDModel::setToonFlag(bool flag)
{
   m_toon = flag;
}

/* PMDModel::getToonFlag: return true when enable toon rendering */
bool PMDModel::getToonFlag()
{
   return m_toon;
}

/* PMDModel::setSelfShadowDrawing: set self shadow drawing flag */
void PMDModel::setSelfShadowDrawing(bool flag)
{
   m_selfShadowDrawing = flag;
}

/* PMDModel::setEdgeColor: set edge color */
void PMDModel::setEdgeColor(float col[4])
{
   int i;

   for (i = 0; i < 4; i++)
      m_edgeColor[i] = col[i];
}

/* PMDModel::setGlobalAlpha: set global alpha value */
void PMDModel::setGlobalAlpha(float alpha)
{
   m_globalAlpha = alpha;
}

/* PMDModel::getRootBone: get root bone */
PMDBone *PMDModel::getRootBone()
{
   return &m_rootBone;
}

/* PMDModel::getCenterBone: get center bone */
PMDBone *PMDModel::getCenterBone()
{
   return m_centerBone;
}

/* PMDModel::getName: get name */
char *PMDModel::getName()
{
   return m_name;
}

/* PMDModel::getNumVertex: get number of vertics */
unsigned long PMDModel::getNumVertex()
{
   return m_numVertex;
}

/* PMDModel::getNumSurface: get number of surface definitions */
unsigned long PMDModel::getNumSurface()
{
   return m_numSurface;
}

/* PMDModel::getNumMaterial: get number of material definitions */
unsigned long PMDModel::getNumMaterial()
{
   return m_numMaterial;
}

/* PMDModel::getNumBone: get number of bones */
unsigned short PMDModel::getNumBone()
{
   return m_numBone;
}

/* PMDModel::getNumIK: get number of IK chains */
unsigned short PMDModel::getNumIK()
{
   return m_numIK;
}

/* PMDModel::getNumFace: get number of faces */
unsigned short PMDModel::getNumFace()
{
   return m_numFace;
}

/* PMDModel::getNumRigidBody: get number of rigid bodies */
unsigned long PMDModel::getNumRigidBody()
{
   return m_numRigidBody;
}

/* PMDModel::getNumConstraint: get number of constraints */
unsigned long PMDModel::getNumConstraint()
{
   return m_numConstraint;
}

/* PMDModel::getErrorTextureList: get error texture list */
void PMDModel::getErrorTextureList(char *buf, int maxLen)
{
   m_textureLoader.getErrorTextureString(buf, maxLen);
}

/* PMDModel::getMaxHeight: get max height */
float PMDModel::getMaxHeight()
{
   return m_maxHeight;
}

/* PMDModel::getComment: get comment of PMD */
char *PMDModel::getComment()
{
   return m_comment;
}

/* PMDModel::getModelDir: get model directory */
wchar_t *PMDModel::getModelDir()
{
   return m_modelDir;
}
