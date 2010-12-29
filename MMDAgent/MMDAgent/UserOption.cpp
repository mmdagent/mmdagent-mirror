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

#pragma once

#include <windows.h>

#include "MMDAgent.h"

#define __MMDAGENT_CONFIG_MAIN__
#include "UserOption.h"
#include "utils.h"

CONF_OPT opt[] = {
   {
      L"Initial model file names (*.pmd)",
      CONF_MODEL_FILE, "model_file", CT_STR, 0, 0, 0.0f, {}, 0, L"", 0
   },
   {
      L"Initial motion file names (*.vmd)",
      CONF_INIT_MOTION_FILE, "initial_motion_file", CT_STR, 0, 0, 0.0f, {}, 0, L"", 0
   },
   {
      L"Initial model position (0.0,0.0,0.0)",
      CONF_INIT_MODEL_LOCATION, "initial_model_position", CT_STR, 0, 0, 0.0f, {}, 0, L"(0.0,0.0,0.0)", 0
   },
   {
      L"Initial model alias",
      CONF_INIT_MODEL_ALIAS, "initial_model_alias", CT_STR, 0, 0, 0.0f, {}, 0, L"", 0
   },
   {
      L"Stage file(s) (*.xpmd, *.png, *.tga, *.bmp)",
      CONF_STAGE, "stage", CT_STR, 0, 0, 0.0f, {}, 0, L"", 0
   },
   {
      L"Initial music file (*.wav, *.mp3)",
      CONF_MUSIC_FILE, "music_file", CT_STR, 0, 0, 0.0f, {}, 0, L"", 0
   },
   {
      L"Application data directory from MMDAgent.exe [dir]",
      CONF_SYSTEM_DATA_DIR, "system_data_dir", CT_STR, 0, 0, 0.0f, {}, 0, L"AppData", 0
   },
   {
      L"Plugin data directory from MMDAgent.exe [dir]",
      CONF_PLUGIN_DIR, "plugin_dir", CT_STR, 0, 0, 0.0f, {}, 0, L"Plugins", 0
   },
   {
      L"Cartoon rendering flag [true|false]",
      CONF_CARTOON_ENABLED, "cartoon_rendering", CT_BOOL, true, 0, 0.0f, {}, 0, L"", 0
   },
   {
      L"Use MMDAgent original cartoon [true|false]",
      CONF_CARTOON_ORIGINAL, "cartoon_original", CT_BOOL, false, 0, 0.0f, {}, 0, L"", 0
   },
   {
      L"Display FPS [true|false]",
      CONF_FPS_ENABLED, "display_fps", CT_BOOL, true, 0, 0.0f, {}, 0, L"", 0
   },
   {
      L"Maximum value of anti-aliasing [0|2|4|8|16]",
      CONF_OPENGL_MULTISAMPLE_MAXNUM, "opengl_multisample_maxnum", CT_INT, 0, 4, 0.0f, {}, 0, L"", 0
   },
   {
      L"Maximum number of color for OpenGV (If NVIDIA, force MSAA)",
      CONF_OPENGL_MULTISAMPLE_COLOR_MAXNUM, "opengl_multisample_color_maxnum", CT_INT, 0, 4, 0.0f, {}, 0, L"", 0
   },
   {
      L"Floor width [float]",
      CONF_FLOOR_WIDTH, "floor_width", CT_FLOAT, 0, 0, 25.0f, {}, 0, L"", 0
   },
   {
      L"Floor depth [float]",
      CONF_FLOOR_DEPTH, "floor_depth", CT_FLOAT, 0, 0, 0.0f, {}, 0, L"", 0
   },
   {
      L"Number of tiles for floor [int]",
      CONF_FLOOR_PATTERN_NUM, "floor_pattern_num", CT_FLOAT, 0, 0, 4.0f, {}, 0, L"", 0
   },
   {
      L"Background height [float]",
      CONF_BACK_HEIGHT, "back_height", CT_FLOAT, 0, 0, 40.0f, {}, 0, L"", 0
   },
   {
      L"Position of FPS (x, y, z)",
      CONF_FPS_POS, "display_fps_pos", CT_ARRAY, 0, 0, 0.0f, { -2.5f, 22.0f, 3.0f}, 3, L"", 0
   },
   {
      L"Log panel setting (width, height, x, y, z, scale)",
      CONF_LOG_WINDOW, "log_window_param", CT_ARRAY, 0, 0, 0.0f, {80.0f, 30.0f, -17.5f, 3.0f, -15.0f, 1.0f}, 6, L"", 0
   },
   {
      L"Window width [pixel]",
      CONF_WINDOW_WIDTH, "window_width", CT_INT, 0, 640, 0.0f, {}, 0, 0, L"", 0
   },
   {
      L"Window height [pixel]",
      CONF_WINDOW_HEIGHT, "window_height", CT_INT, 0, 550, 0.0f, {}, 0, 0, L"", 0
   },
   {
      L"Top most [true/false]",
      CONF_TOPMOST_ENABLED, "top_most", CT_BOOL, false, 0, 0.0f, {}, 0, L"", 0
   },
   {
      L"Full screen [true/false]",
      CONF_FULLSCREEN_ENABLED, "fullscreen", CT_BOOL, false, 0, 0.0f, {}, 0, L"", 0
   },
   {
      L"Use shadow mapping [true/false]",
      CONF_SHADOW_MAPPING, "shadow_mapping", CT_BOOL, false, 0, 0.0f, {}, 0, L"", 0
   },
   {
      L"Depth texture size for shadow mapping [int]",
      CONF_SHADOW_TEXTURE_SIZE, "shadow_texture_size", CT_INT, 0, 1024, 0.0f, {}, 0, L"", 0
   },
   {
      L"Self shadow rate for shadow mapping [float]",
      CONF_SHADOW_SELF_DENSITY, "shadow_self_density", CT_FLOAT, 0, 0, 1.0f, {}, 0, L"", 0
   },
   {
      L"Floor shadow rate (0.0-1.0) [float]",
      CONF_SHADOW_DENSITY, "shadow_density", CT_FLOAT, 0, 0, 0.5f, {}, 0, L"", 0
   },
   {
      L"Give priority to texture alpha for shadow mapping [true/false]",
      CONF_SHADOW_MAPPING_LIGHT_FIRST, "shadow_mapping_light_first", CT_BOOL, true, 0, 0.0f, {}, 0, L"", 0
   },
   {
      L"Zoom [float]",
      CONF_MODEL_SCALE, "model_scale", CT_FLOAT, 0, 0, 1.0f, {}, 0, L"", 0
   },
   {
      L"Initiali model position (x, y, z)",
      CONF_MODEL_POS, "model_pos", CT_ARRAY, 0, 0, 0.0f, {0.0f, 4.0f, 0.0f}, 3, L"", 0
   },
   {
      L"Initial model rotation (w, x, y, z)",
      CONF_MODEL_ROT, "model_rot", CT_ARRAY, 0, 0, 0.0f, {1.0f, 0.0f, 0.0f, 0.0f}, 4, L"", 0
   },
   {
      L"Viewpoint to near frustum [float]",
      CONF_VIEWPOINT_FRUSTUM_NEAR, "viewport_frustum_near", CT_FLOAT, 0, 0, 5.0f, {}, 0, L"", 0
   },
   {
      L"Viewpoint to far frustum [float]",
      CONF_VIEWPOINT_FRUSTUM_FAR, "viewport_frustum_far", CT_FLOAT, 0, 0, 2000.0f, {}, 0, L"", 0
   },
   {
      L"Viewpoint from model position [float]",
      CONF_VIEWPOINT_CAMERA_Z, "viewport_camera_z", CT_FLOAT, 0, 0, -100.0f, {}, 0, L"", 0
   },
   {
      L"Viewpoint of model offset [float]",
      CONF_VIEWPOINT_Y_OFFSET, "viewport_y_offset", CT_FLOAT, 0, 0, -13.0f, {}, 0, L"", 0
   },
   {
      L"Light direction",
      CONF_LIGHT_DIRECTION_FROM, "light_direction_from", CT_ARRAY, 0, 0, 0.0f, {0.5f, 1.0f, 0.5f, 0.0f}, 4, L"", 0
   },
   {
      L"Light power (diffuse) (0.0-1.0)",
      CONF_LIGHT_INTENSITY, "light_intensity", CT_FLOAT, 0, 0, 0.6f, {}, 0, L"", 0
   },
   {
      L"Light power (ambient) (0.0-1.0)",
      CONF_LIGHT_INTENSITY_AMBIENT, "light_intensity_ambient", CT_FLOAT, 0, 0, 1.0f, {}, 0, L"", 0
   },
   {
      L"Light color (0.0-1.0)",
      CONF_LIGHT_COLOR, "light_color", CT_ARRAY, 0, 0, 0.0f, {1.0f, 1.0f, 1.0f}, 3, L"", 0
   },
   {
      L"Background color (0.0～1.0) [r, g, b]",
      CONF_BACKGROUND_COLOR, "background_color", CT_ARRAY, 0, 0, 0.0f, {0.0f, 0.0f, 0.2f}, 3, L"", 0
   },
   {
      L"Edge width for catroon (> 0.0) [float]",
      CONF_CARTOON_EDGE_WIDTH, "cartoon_edge_width", CT_FLOAT, 0, 0, 1.0f, {}, 0, L"", 0
   },
   {
      L"Rotation step [float]",
      CONF_UI_ROTATE_STEP, "ui_rotate_step", CT_FLOAT, 0, 0, 0.08726646256f, {}, 0, L"", 0
   },
   {
      L"Translate step [float]",
      CONF_UI_TRANSLATE_STEP, "ui_translate_step", CT_FLOAT, 0, 0, 0.5f, {}, 0, L"", 0
   },
   {
      L"Scale step [float]",
      CONF_UI_SCALE_STEP, "ui_scale_step", CT_FLOAT, 0, 0, 1.05f, {}, 0, L"", 0
   },
   {
      L"Cartoon edge step [float]",
      CONF_UI_CARTOON_EDGE_STEP, "ui_cartoon_edge_step", CT_FLOAT, 0, 0, 1.2f, {}, 0, L"", 0
   },
   {
      L"Frame interval [float]",
      CONF_FRAME_MAX_INTERVAL,	"frame_max_interval",	CT_FLOAT, 0, 0, 10.0f, {}, 0, L"", 0
   },
   {
      L"Motion adjust frame [int]",
      CONF_MOTION_ADJUST, "motion_adjust_msec", CT_INT, 0, 0, 0.0f, {}, 0, L"", 0
   },
   {
      L"FPS for Bullet Physics]",
      CONF_BULLET_FPS, "bullet_fps", CT_INT, 0, 120, 0.0f, {}, 0, L"", 0
   },
};

/* save_config: save config file */
bool config_save(wchar_t *wszFileName)
{
   int i;
   FILE *fp;
   static char str[512];
   char str2[CONF_STR_LEN_MAX];
   size_t strSize;
   char sbuf[256];
   size_t converted = 0;

   fp = _wfopen(wszFileName, L"w");
   if (!fp)
      return false;

   for (i = 0; i < CONF_NUM; i++) {
      wcstombs(sbuf, opt[i].desc, 256);
      _snprintf(str, 512, "# %s\n", sbuf);
      strSize = strlen(str);
      if (fwrite(str, sizeof(char), strSize, fp) != strSize) {
         fclose(fp);
         return false;
      }
      switch (opt[i].type) {
      case CT_BOOL:
         _snprintf(str, 512, "%s=%s\n", opt[i].name, (opt[i].b) ? "true" : "false");
         break;
      case CT_INT:
         _snprintf(str, 512, "%s=%d\n", opt[i].name, opt[i].i);
         break;
      case CT_FLOAT:
         _snprintf(str, 512, "%s=%f\n", opt[i].name, opt[i].f);
         break;
      case CT_STR:
         _snprintf(str, 512, "%s=%S\n", opt[i].name, opt[i].s);
         break;
      case CT_ARRAY:
         _snprintf(str, 512, "%s=", opt[i].name);
         for (int j = 0; j < opt[i].fvnum; j++) {
            _snprintf(str2, CONF_STR_LEN_MAX, (j == 0) ? "%f" : ", %f", opt[i].fv[j]);
            strcat(str, str2);
         }
         strcat(str, "\n");
         break;
      }
      strSize = strlen(str);
      if (fwrite(str, sizeof(char), strSize, fp) != strSize) {
         fclose(fp);
         return false;
      }
   }
   fclose(fp);

   return true;
}

/* config_load: load config file */
bool config_load(wchar_t *wszFileName, int source_id)
{
   FILE *fp;
   char buf[256];
   wchar_t wbuf[256];
   wchar_t *p, *q;
   int id, v;
   int len;

   fp = _wfopen(wszFileName, L"r");
   if (!fp) {
      g_logger.log( L"! Warning: UserOption: file \"%s\" not found, skipped", wszFileName);
      return false;
   }
   while (fgetws(wbuf, 256, fp)) {
      p = &(wbuf[wcslen(wbuf)-1]);
      while (p >= &(wbuf[0]) && (*p == L'\n' || *p == L'\r')) {
         *p = L'\0';
         p--;
      }
      p = &(wbuf[0]);
      if (*p == L'#') continue;
      while (*p != L'=' && *p != L'\0') p++;
      if (*p == L'\0') continue;
      *p = L'\0';
      p++;

      /* if "include", call this function */
      if ( wcscmp(wbuf, L"include") == 0 )
         if ( config_load(p, source_id) == false )
            g_logger.log( L"! Warning: UserOption: terminate loading \"%s\", continuing...", p );

      wcstombs(buf, wbuf, 256);
      for (id = 0; id < CONF_NUM; id++)
         if (!strcmp(opt[id].name, buf))
            break;
      if (id >= CONF_NUM) {
         g_logger.log( L"! Error: UserOption: unknown variable \"%s\", skipped", wbuf);
         continue;
      }
      switch (opt[id].type) {
      case CT_BOOL:
         if (p[0] == L't' || p[0] == L'T' || p[0] == L'Y' || p[0] == L'y') {
            opt[id].b = true;
         } else if (p[0] == L'f' || p[0] == L'F' || p[0] == L'N' || p[0] == L'n') {
            opt[id].b = false;
         } else {
            g_logger.log(L"! Error: UserOption: value of \"%s\" shoule be either \"true\" or \"false\"", wbuf);
            return false;
         }
         break;
      case CT_INT:
         opt[id].i = _wtoi(p);
         break;
      case CT_FLOAT:
         opt[id].f = (float)_wtof(p);
         break;
      case CT_STR:
         if (*p == L'"') p++;
         len = wcslen(p);
         if (p[len-1] == L'"') p[len-1] = L'\0';
         wcsncpy(opt[id].s, p, CONF_STR_LEN_MAX);
         break;
      case CT_ARRAY:
         v = 0;
         for (q = wcstok(p, L", \t\r\n"); q; q = wcstok(NULL, L", \t\r\n")) {
            opt[id].fv[v++] = (float) _wtof(q);
         }
         break;
      }

      opt[id].src = source_id;
   }
   fclose(fp);
   g_logger.log( L"UserOption: loaded config file \"%s\"", wszFileName);

   return true;
}


