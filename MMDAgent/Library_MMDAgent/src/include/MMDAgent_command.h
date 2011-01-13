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

/* command names */
#define MMDAGENT_COMMAND_MODEL_ADD      "MODEL_ADD"
#define MMDAGENT_COMMAND_MODEL_DELETE   "MODEL_DELETE"
#define MMDAGENT_COMMAND_MODEL_CHANGE   "MODEL_CHANGE"
#define MMDAGENT_COMMAND_MOTION_ADD     "MOTION_ADD"
#define MMDAGENT_COMMAND_MOTION_DELETE  "MOTION_DELETE"
#define MMDAGENT_COMMAND_MOTION_CHANGE  "MOTION_CHANGE"
#define MMDAGENT_COMMAND_MOVE_START     "MOVE_START"
#define MMDAGENT_COMMAND_MOVE_STOP      "MOVE_STOP"
#define MMDAGENT_COMMAND_TURN_START     "TURN_START"
#define MMDAGENT_COMMAND_TURN_STOP      "TURN_STOP"
#define MMDAGENT_COMMAND_ROTATE_START   "ROTATE_START"
#define MMDAGENT_COMMAND_ROTATE_STOP    "ROTATE_STOP"
#define MMDAGENT_COMMAND_SOUND_START    "SOUND_START"
#define MMDAGENT_COMMAND_SOUND_STOP     "SOUND_STOP"
#define MMDAGENT_COMMAND_STAGE          "STAGE"
#define MMDAGENT_COMMAND_FLOOR          "FLOOR"
#define MMDAGENT_COMMAND_BACKGROUND     "BACKGROUND"
#define MMDAGENT_COMMAND_LIGHTCOLOR     "LIGHTCOLOR"
#define MMDAGENT_COMMAND_LIGHTDIRECTION "LIGHTDIRECTION"
#define MMDAGENT_COMMAND_LIPSYNC_START  "LIPSYNC_START"
#define MMDAGENT_COMMAND_LIPSYNC_STOP   "LIPSYNC_STOP"

/* event names */
#define MMDAGENTCOMMAND_MODELEVENTADD       "MODEL_EVENT_ADD"
#define MMDAGENTCOMMAND_MODELEVENTDELETE    "MODEL_EVENT_DELETE"
#define MMDAGENTCOMMAND_MODELEVENTCHANGE    "MODEL_EVENT_CHANGE"
#define MMDAGENTCOMMAND_EVENTMOTIONADD      "MOTION_EVENT_ADD"
#define MMDAGENTCOMMAND_MOTIONEVENTDELETE   "MOTION_EVENT_DELETE"
#define MMDAGENTCOMMAND_MOTIONEVENTCHANGE   "MOTION_EVENT_CHANGE"
#define MMDAGENTCOMMAND_MOVEEVENTSTART      "MOVE_EVENT_START"
#define MMDAGENTCOMMAND_MOVEEVENTSTOP       "MOVE_EVENT_STOP"
#define MMDAGENTCOMMAND_TURNEVENTSTART      "TURN_EVENT_START"
#define MMDAGENTCOMMAND_TURNEVENTSTOP       "TURN_EVENT_STOP"
#define MMDAGENTCOMMAND_ROTATEEVENTSTART    "ROTATE_EVENT_START"
#define MMDAGENTCOMMAND_ROTATEEVENTSTOP     "ROTATE_EVENT_STOP"
#define MMDAGENTCOMMAND_SOUNDEVENTSTART     "SOUND_EVENT_START"
#define MMDAGENTCOMMAND_SOUNDEVENTSTOP      "SOUND_EVENT_STOP"
#define MMDAGENTCOMMAND_STAGE               "STAGE"
#define MMDAGENTCOMMAND_FLOOR               "FLOOR"
#define MMDAGENTCOMMAND_BACKGROUND          "BACKGROUND"
#define MMDAGENTCOMMAND_EVENTLIGHTCOLOR     "LIGHTCOLOR"
#define MMDAGENTCOMMAND_EVENTLIGHTDIRECTION "LIGHTDIRECTION"
#define MMDAGENTCOMMAND_LIPSYNCEVENTSTART   "LIPSYNC_EVENT_START"
#define MMDAGENTCOMMAND_LIPSYNCEVENTSTOP    "LIPSYNC_EVENT_STOP"
#define MMDAGENTCOMMAND_KEY                 "KEY"
