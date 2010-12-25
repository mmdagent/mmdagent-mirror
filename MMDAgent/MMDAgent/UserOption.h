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

#ifndef __mmdagent_useroption_h__
#define __mmdagent_useroption_h__

#define CONF_STR_LEN_MAX 256

enum {
   CT_BOOL,
   CT_INT,
   CT_FLOAT,
   CT_ARRAY,
   CT_STR
};

typedef struct {
   wchar_t desc[80];
   unsigned short id;
   char * name;
   unsigned char type;
   bool b;
   int i;
   float f;
   float fv[6];
   unsigned char fvnum;
   wchar_t s[CONF_STR_LEN_MAX];
   int src;
} CONF_OPT;


enum {
   CONF_MODEL_FILE,
   CONF_INIT_MOTION_FILE,
   CONF_INIT_MODEL_LOCATION,
   CONF_INIT_MODEL_ALIAS,
   CONF_FLOOR_BMP,
   CONF_BACK_BMP,
   CONF_MUSIC_FILE,
   CONF_SYSTEM_DATA_DIR,
   CONF_PLUGIN_DIR,

   CONF_CARTOON_ENABLED,
   CONF_CARTOON_ORIGINAL,
   CONF_FPS_ENABLED,
   CONF_OPENGL_MULTISAMPLE_MAXNUM,
   CONF_OPENGL_MULTISAMPLE_COLOR_MAXNUM,

   CONF_FLOOR_WIDTH,
   CONF_FLOOR_DEPTH,
   CONF_FLOOR_PATTERN_NUM,
   CONF_BACK_HEIGHT,
   CONF_FPS_POS,
   CONF_LOG_WINDOW,

   CONF_WINDOW_WIDTH,
   CONF_WINDOW_HEIGHT,
   CONF_TOPMOST_ENABLED,
   CONF_FULLSCREEN_ENABLED,

   CONF_SHADOW_MAPPING,
   CONF_SHADOW_TEXTURE_SIZE,
   CONF_SHADOW_SELF_DENSITY,
   CONF_SHADOW_DENSITY,
   CONF_SHADOW_MAPPING_LIGHT_FIRST,

   CONF_MODEL_SCALE,
   CONF_MODEL_POS,
   CONF_MODEL_ROT,
   CONF_VIEWPOINT_FRUSTUM_NEAR,
   CONF_VIEWPOINT_FRUSTUM_FAR,
   CONF_VIEWPOINT_CAMERA_Z,
   CONF_VIEWPOINT_Y_OFFSET,
   CONF_LIGHT_DIRECTION_FROM,
   CONF_LIGHT_INTENSITY,
   CONF_LIGHT_INTENSITY_AMBIENT,
   CONF_LIGHT_COLOR,
   CONF_BACKGROUND_COLOR,
   CONF_CARTOON_EDGE_WIDTH,
   CONF_UI_ROTATE_STEP,
   CONF_UI_TRANSLATE_STEP,
   CONF_UI_SCALE_STEP,
   CONF_UI_CARTOON_EDGE_STEP,
   CONF_FRAME_MAX_INTERVAL,
   CONF_MOTION_ADJUST,
   CONF_BULLET_FPS,

   CONF_NUM
};

#ifndef __MMDAGENT_CONFIG_MAIN__
extern CONF_OPT opt[];
#endif

bool config_save(wchar_t *wszFileName);
bool config_load(wchar_t *wszFileName, int src_id);

#endif /* __mmdagent_useroption_h__ */
