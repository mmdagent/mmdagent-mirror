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

#ifndef __mmdagent_eventcommanddef_h__
#define __mmdagent_eventcommanddef_h__

/* command names */
#define MMDAGENT_COMMAND_MODEL_ADD      L"MODEL_ADD"
#define MMDAGENT_COMMAND_MODEL_DELETE   L"MODEL_DELETE"
#define MMDAGENT_COMMAND_MODEL_CHANGE   L"MODEL_CHANGE"
#define MMDAGENT_COMMAND_MOTION_ADD     L"MOTION_ADD"
#define MMDAGENT_COMMAND_MOTION_DELETE  L"MOTION_DELETE"
#define MMDAGENT_COMMAND_MOTION_CHANGE  L"MOTION_CHANGE"
#define MMDAGENT_COMMAND_MOVE_START     L"MOVE_START"
#define MMDAGENT_COMMAND_MOVE_STOP      L"MOVE_STOP"
#define MMDAGENT_COMMAND_TURN_START     L"TURN_START"
#define MMDAGENT_COMMAND_TURN_STOP      L"TURN_STOP"
#define MMDAGENT_COMMAND_ROTATE_START   L"ROTATE_START"
#define MMDAGENT_COMMAND_ROTATE_STOP    L"ROTATE_STOP"
#define MMDAGENT_COMMAND_SOUND_START    L"SOUND_START"
#define MMDAGENT_COMMAND_SOUND_STOP     L"SOUND_STOP"
#define MMDAGENT_COMMAND_STAGE          L"STAGE"
#define MMDAGENT_COMMAND_FLOOR          L"FLOOR"
#define MMDAGENT_COMMAND_BACKGROUND     L"BACKGROUND"
#define MMDAGENT_COMMAND_LIGHTCOLOR     L"LIGHTCOLOR"
#define MMDAGENT_COMMAND_LIGHTDIRECTION L"LIGHTDIRECTION"
#define MMDAGENT_COMMAND_LIPSYNC_START  L"LIPSYNC_START"
#define MMDAGENT_COMMAND_LIPSYNC_STOP   L"LIPSYNC_STOP"

/* event names */
#define MMDAGENT_EVENT_MODEL_ADD      L"MODEL_EVENT_ADD"
#define MMDAGENT_EVENT_MODEL_DELETE   L"MODEL_EVENT_DELETE"
#define MMDAGENT_EVENT_MODEL_CHANGE   L"MODEL_EVENT_CHANGE"
#define MMDAGENT_EVENT_MOTION_ADD     L"MOTION_EVENT_ADD"
#define MMDAGENT_EVENT_MOTION_DELETE  L"MOTION_EVENT_DELETE"
#define MMDAGENT_EVENT_MOTION_CHANGE  L"MOTION_EVENT_CHANGE"
#define MMDAGENT_EVENT_MOTION_LOOP    L"MOTION_EVENT_LOOP"
#define MMDAGENT_EVENT_MOVE_START     L"MOVE_EVENT_START"
#define MMDAGENT_EVENT_MOVE_STOP      L"MOVE_EVENT_STOP"
#define MMDAGENT_EVENT_TURN_START     L"TURN_EVENT_START"
#define MMDAGENT_EVENT_TURN_STOP      L"TURN_EVENT_STOP"
#define MMDAGENT_EVENT_ROTATE_START   L"ROTATE_EVENT_START"
#define MMDAGENT_EVENT_ROTATE_STOP    L"ROTATE_EVENT_STOP"
#define MMDAGENT_EVENT_SOUND_START    L"SOUND_EVENT_START"
#define MMDAGENT_EVENT_SOUND_STOP     L"SOUND_EVENT_STOP"
#define MMDAGENT_EVENT_STAGE          L"STAGE"
#define MMDAGENT_EVENT_FLOOR          L"FLOOR"
#define MMDAGENT_EVENT_BACKGROUND     L"BACKGROUND"
#define MMDAGENT_EVENT_LIGHTCOLOR     L"LIGHTCOLOR"
#define MMDAGENT_EVENT_LIGHTDIRECTION L"LIGHTDIRECTION"
#define MMDAGENT_EVENT_LIPSYNC_START  L"LIPSYNC_EVENT_START"
#define MMDAGENT_EVENT_LIPSYNC_STOP   L"LIPSYNC_EVENT_STOP"
#define MMDAGENT_EVENT_KEY            L"KEY"

#endif /* __mmdagent_eventcommanddef_h__ */
