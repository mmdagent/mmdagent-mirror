/* ----------------------------------------------------------------- */
/*           The Toolkit for Building Voice Interaction Systems      */
/*           "MMDAgent" developed by MMDAgent Project Team           */
/*           http://www.mmdagent.jp/                                 */
/* ----------------------------------------------------------------- */
/*                                                                   */
/*  Copyright (c) 2009-2014  Nagoya Institute of Technology          */
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

#define MOTIONCONTROLLER_BONESTARTMARGINFRAME 20.0 /* frame lengths for bone motion smoothing at loop head */
#define MOTIONCONTROLLER_FACESTARTMARGINFRAME 6.0  /* frame lengths for face motion smoothing at loop head */

#define MOTIONCONTROLLER_CENTERBONENAME "\x83\x5a\x83\x93\x83\x5e\x81\x5b" /* center */

/* MotionControllerBoneElement: motion control element for bone */
typedef struct _MotionControllerBoneElement {
   PMDBone *bone;         /* bone to be controlled */
   BoneMotion *motion;    /* bone motion to be played */
   btVector3 pos;         /* calculated position */
   btQuaternion rot;      /* calculated rotation */
   btVector3 snapPos;     /* snapped position at beginning of motion for smoothing */
   btQuaternion snapRot;  /* snapped rotation at beginning of motion for smoothing */
   unsigned long lastKey; /* last key frame number */
   bool looped;           /* true if the stored end-of-motion position/rotation should be applied as keyframs at first frame */
} MotionControllerBoneElement;

/* MotionControllerFaceElement: Motion control element for face */
typedef struct _MotionControllerFaceElement {
   PMDFace *face;         /* face to be managed */
   FaceMotion *motion;    /* face motion to be played */
   float weight;          /* calculated weight */
   float snapWeight;      /* snapped face weight at beginning of motion for smoothing */
   unsigned long lastKey; /* last key frame number */
   bool looped;           /* true if the stored end-of-motion weight should be applied as keyframs at first frame */
} MotionControllerFaceElement;

/* MotionControllerSwitchElement: Motion control element for switches */
typedef struct _MotionControllerSwitchElement {
   PMDModel *pmd;           /* pmd model to be switched */
   SwitchMotion *motion;    /* switch motion to be played */
   SwitchKeyFrame *current; /* current key frame to be applied */
   unsigned long lastKey;   /* last key frame number */
} MotionControllerSwitchElement;

/* MotionController: motion controller class, to handle one motion to a list of bones and faces */
class MotionController
{
private:

   float m_maxFrame; /* maximum key frame */

   unsigned long m_numBoneCtrl;                 /* number of bone control list */
   MotionControllerBoneElement *m_boneCtrlList; /* bone control list */
   unsigned long m_numFaceCtrl;                 /* number of face control list */
   MotionControllerFaceElement *m_faceCtrlList; /* face control list */
   MotionControllerSwitchElement *m_switchCtrl; /* model switch control */

   /* values determined by the given PMD and VMD */
   bool m_hasCenterBoneMotion; /* true if the motion has more than 1 key frames for center bone and need re-location */

   /* configurations */
   float m_boneBlendRate;     /* if != 1.0f, the resulting bone location will be blended upon the current bone position at this rate, else override it */
   float m_faceBlendRate;     /* if != 1.0f, the resulting face weight will be blended upon the current face at this rate, else override it */
   bool m_ignoreSingleMotion; /* if true, motions with only one key frame for the first frame will be discarded for inserting motions */

   /* internal work area */
   double m_currentFrame;      /* current frame */
   double m_previousFrame;     /* current frame at last call to advance() */
   double m_noBoneSmearFrame;  /* remaining frames for bone motion smoothing at loop head */
   double m_noFaceSmearFrame;  /* remaining frames for face motion smoothing at loop head */

   /* internal work area for initial pose snapshot */
   bool m_overrideFirst; /* when true, the initial bone pos/rot and face weights in the motion at the first frame will be replaced by the runtime pose snapshot */

   /* calcBoneAt: calculate bone pos/rot at the given frame */
   void calcBoneAt(MotionControllerBoneElement *mc, float absFrame);

   /* calcFaceAt: calculate face weight at the given frame */
   void calcFaceAt(MotionControllerFaceElement *mc, float absFrame);

   /* calcSwitchAt: calculate switches at the given frame */
   void calcSwitchAt(MotionControllerSwitchElement *mc, float frameNow);

   /* control: set bone position/rotation and face weights according to the motion to the specified frame */
   void control(float frameNow);

   /* takeSnap: take a snap shot of bones/faces for motion smoothing at beginning of a motion */
   void takeSnap(btVector3 *centerPos);

   /* setLoopedFlags: set flag if the stored end-of-motion position/rotation/weight should be applied at first frame */
   void setLoopedFlags(bool flag);

   /* initialize: initialize controller */
   void initialize();

   /* clear: free controller */
   void clear();

public:

   /* MotionController: constructor */
   MotionController();

   /* ~MotionController: destructor */
   ~MotionController();

   /* setup: initialize and set up controller */
   void setup(PMDModel *model, VMD *motions);

   /* reset: reset values */
   void reset();

   /* advance: advance motion controller by the given frame, return true when reached end */
   bool advance(double deltaFrame);

   /* rewind: rewind motion controller to the given frame */
   void rewind(float targetFrame, float frame);

   /* setOverrideFirst: should be called at the first frame, to tell controller to take snapshot */
   void setOverrideFirst(btVector3 *centerPos);

   /* setBoneBlendRate: set bone blend rate */
   void setBoneBlendRate(float rate);

   /* setFaceBlendRate: set face blend rate */
   void setFaceBlendRate(float rate);

   /* setIgnoreSingleMotion: set insert motion flag */
   void setIgnoreSingleMotion(bool val);

   /* hasCenter: return true if the motion has more than 1 key frames for center bone */
   bool hasCenter();

   /* getMaxFrame: get max frame */
   float getMaxFrame();

   /* getCurrentFrame: get current frame */
   double getCurrentFrame();

   /* setCurrentFrame: set current frame */
   void setCurrentFrame(double frame);

   /* getPreviousFrame: get previous frame */
   double getPreviousFrame();

   /* setPreviousFrame: set previous frame */
   void setPreviousFrame(double frame);

   /* getNumBoneCtrl: get number of bone controller */
   unsigned long getNumBoneCtrl();

   /* getBoneCtrlList: get list of bone controller */
   MotionControllerBoneElement *getBoneCtrlList();
};
