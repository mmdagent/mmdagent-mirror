/* ----------------------------------------------------------------- */
/*           The Japanese TTS System "Open JTalk"                    */
/*           developed by HTS Working Group                          */
/*           http://open-jtalk.sourceforge.net/                      */
/* ----------------------------------------------------------------- */
/*                                                                   */
/*  Copyright (c) 2008-2015  Nagoya Institute of Technology          */
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
/* - Neither the name of the HTS working group nor the names of its  */
/*   contributors may be used to endorse or promote products derived */
/*   from this software without specific prior written permission.   */
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

#ifndef NJD_SET_PRONUNCIATION_RULE_H
#define NJD_SET_PRONUNCIATION_RULE_H

#ifdef __cplusplus
#define NJD_SET_PRONUNCIATION_RULE_H_START extern "C" {
#define NJD_SET_PRONUNCIATION_RULE_H_END   }
#else
#define NJD_SET_PRONUNCIATION_RULE_H_START
#define NJD_SET_PRONUNCIATION_RULE_H_END
#endif                          /* __CPLUSPLUS */

NJD_SET_PRONUNCIATION_RULE_H_START;

static const char *njd_set_pronunciation_list[] = {
   "\xe3\x83\xb4\xe3\x83\xa7", "\xe3\x83\xb4\xe3\x83\xa7", "1",
   "\xe3\x83\xb4\xe3\x83\xa5", "\xe3\x83\xb4\xe3\x83\xa5", "1",
   "\xe3\x83\xb4\xe3\x83\xa3", "\xe3\x83\xb4\xe3\x83\xa3", "1",
   "\xe3\x83\xb4\xe3\x82\xa9", "\xe3\x83\xb4\xe3\x82\xa9", "1",
   "\xe3\x83\xb4\xe3\x82\xa7", "\xe3\x83\xb4\xe3\x82\xa7", "1",
   "\xe3\x83\xb4\xe3\x82\xa3", "\xe3\x83\xb4\xe3\x82\xa3", "1",
   "\xe3\x83\xb4\xe3\x82\xa1", "\xe3\x83\xb4\xe3\x82\xa1", "1",
   "\xe3\x83\xb4", "\xe3\x83\xb4", "1",
   "\xe3\x83\xb3", "\xe3\x83\xb3", "1",
   "\xe3\x83\xb2", "\xe3\x83\xb2", "1",
   "\xe3\x83\xb1", "\xe3\x83\xb1", "1",
   "\xe3\x83\xb0", "\xe3\x83\xb0", "1",
   "\xe3\x83\xaf", "\xe3\x83\xaf", "1",
   "\xe3\x83\xad", "\xe3\x83\xad", "1",
   "\xe3\x83\xac", "\xe3\x83\xac", "1",
   "\xe3\x83\xab", "\xe3\x83\xab", "1",
   "\xe3\x83\xaa\xe3\x83\xa7", "\xe3\x83\xaa\xe3\x83\xa7", "1",
   "\xe3\x83\xaa\xe3\x83\xa5", "\xe3\x83\xaa\xe3\x83\xa5", "1",
   "\xe3\x83\xaa\xe3\x83\xa3", "\xe3\x83\xaa\xe3\x83\xa3", "1",
   "\xe3\x83\xaa\xe3\x82\xa7", "\xe3\x83\xaa\xe3\x82\xa7", "1",
   "\xe3\x83\xaa", "\xe3\x83\xaa", "1",
   "\xe3\x83\xa9", "\xe3\x83\xa9", "1",
   "\xe3\x83\xa8", "\xe3\x83\xa8", "1",
   "\xe3\x83\xa7", "\xe3\x83\xa7", "1",
   "\xe3\x83\xa6", "\xe3\x83\xa6", "1",
   "\xe3\x83\xa5", "\xe3\x83\xa5", "1",
   "\xe3\x83\xa4", "\xe3\x83\xa4", "1",
   "\xe3\x83\xa3", "\xe3\x83\xa3", "1",
   "\xe3\x83\xa2", "\xe3\x83\xa2", "1",
   "\xe3\x83\xa1", "\xe3\x83\xa1", "1",
   "\xe3\x83\xa0", "\xe3\x83\xa0", "1",
   "\xe3\x83\x9f\xe3\x83\xa7", "\xe3\x83\x9f\xe3\x83\xa7", "1",
   "\xe3\x83\x9f\xe3\x83\xa5", "\xe3\x83\x9f\xe3\x83\xa5", "1",
   "\xe3\x83\x9f\xe3\x83\xa3", "\xe3\x83\x9f\xe3\x83\xa3", "1",
   "\xe3\x83\x9f\xe3\x82\xa7", "\xe3\x83\x9f\xe3\x82\xa7", "1",
   "\xe3\x83\x9f", "\xe3\x83\x9f", "1",
   "\xe3\x83\x9e", "\xe3\x83\x9e", "1",
   "\xe3\x83\x9d", "\xe3\x83\x9d", "1",
   "\xe3\x83\x9c", "\xe3\x83\x9c", "1",
   "\xe3\x83\x9b", "\xe3\x83\x9b", "1",
   "\xe3\x83\x9a", "\xe3\x83\x9a", "1",
   "\xe3\x83\x99", "\xe3\x83\x99", "1",
   "\xe3\x83\x98", "\xe3\x83\x98", "1",
   "\xe3\x83\x97", "\xe3\x83\x97", "1",
   "\xe3\x83\x96", "\xe3\x83\x96", "1",
   "\xe3\x83\x95\xe3\x82\xa9", "\xe3\x83\x95\xe3\x82\xa9", "1",
   "\xe3\x83\x95\xe3\x82\xa7", "\xe3\x83\x95\xe3\x82\xa7", "1",
   "\xe3\x83\x95\xe3\x82\xa3", "\xe3\x83\x95\xe3\x82\xa3", "1",
   "\xe3\x83\x95\xe3\x82\xa1", "\xe3\x83\x95\xe3\x82\xa1", "1",
   "\xe3\x83\x95", "\xe3\x83\x95", "1",
   "\xe3\x83\x94\xe3\x83\xa7", "\xe3\x83\x94\xe3\x83\xa7", "1",
   "\xe3\x83\x94\xe3\x83\xa5", "\xe3\x83\x94\xe3\x83\xa5", "1",
   "\xe3\x83\x94\xe3\x83\xa3", "\xe3\x83\x94\xe3\x83\xa3", "1",
   "\xe3\x83\x94\xe3\x82\xa7", "\xe3\x83\x94\xe3\x82\xa7", "1",
   "\xe3\x83\x94", "\xe3\x83\x94", "1",
   "\xe3\x83\x93\xe3\x83\xa7", "\xe3\x83\x93\xe3\x83\xa7", "1",
   "\xe3\x83\x93\xe3\x83\xa5", "\xe3\x83\x93\xe3\x83\xa5", "1",
   "\xe3\x83\x93\xe3\x83\xa3", "\xe3\x83\x93\xe3\x83\xa3", "1",
   "\xe3\x83\x93\xe3\x82\xa7", "\xe3\x83\x93\xe3\x82\xa7", "1",
   "\xe3\x83\x93", "\xe3\x83\x93", "1",
   "\xe3\x83\x92\xe3\x83\xa7", "\xe3\x83\x92\xe3\x83\xa7", "1",
   "\xe3\x83\x92\xe3\x83\xa5", "\xe3\x83\x92\xe3\x83\xa5", "1",
   "\xe3\x83\x92\xe3\x83\xa3", "\xe3\x83\x92\xe3\x83\xa3", "1",
   "\xe3\x83\x92\xe3\x82\xa7", "\xe3\x83\x92\xe3\x82\xa7", "1",
   "\xe3\x83\x92", "\xe3\x83\x92", "1",
   "\xe3\x83\x91", "\xe3\x83\x91", "1",
   "\xe3\x83\x90", "\xe3\x83\x90", "1",
   "\xe3\x83\x8f", "\xe3\x83\x8f", "1",
   "\xe3\x83\x8e", "\xe3\x83\x8e", "1",
   "\xe3\x83\x8d", "\xe3\x83\x8d", "1",
   "\xe3\x83\x8c", "\xe3\x83\x8c", "1",
   "\xe3\x83\x8b\xe3\x83\xa7", "\xe3\x83\x8b\xe3\x83\xa7", "1",
   "\xe3\x83\x8b\xe3\x83\xa5", "\xe3\x83\x8b\xe3\x83\xa5", "1",
   "\xe3\x83\x8b\xe3\x83\xa3", "\xe3\x83\x8b\xe3\x83\xa3", "1",
   "\xe3\x83\x8b\xe3\x82\xa7", "\xe3\x83\x8b\xe3\x82\xa7", "1",
   "\xe3\x83\x8b", "\xe3\x83\x8b", "1",
   "\xe3\x83\x8a", "\xe3\x83\x8a", "1",
   "\xe3\x83\x89\xe3\x82\xa5", "\xe3\x83\x89\xe3\x82\xa5", "1",
   "\xe3\x83\x89", "\xe3\x83\x89", "1",
   "\xe3\x83\x88\xe3\x82\xa5", "\xe3\x83\x88\xe3\x82\xa5", "1",
   "\xe3\x83\x88", "\xe3\x83\x88", "1",
   "\xe3\x83\x87\xe3\x83\xa7", "\xe3\x83\x87\xe3\x83\xa7", "1",
   "\xe3\x83\x87\xe3\x83\xa5", "\xe3\x83\x87\xe3\x83\xa5", "1",
   "\xe3\x83\x87\xe3\x83\xa3", "\xe3\x83\x87\xe3\x83\xa3", "1",
   "\xe3\x83\x87\xe3\x82\xa3", "\xe3\x83\x87\xe3\x82\xa3", "1",
   "\xe3\x83\x87", "\xe3\x83\x87", "1",
   "\xe3\x83\x86\xe3\x83\xa7", "\xe3\x83\x86\xe3\x83\xa7", "1",
   "\xe3\x83\x86\xe3\x83\xa5", "\xe3\x83\x86\xe3\x83\xa5", "1",
   "\xe3\x83\x86\xe3\x83\xa3", "\xe3\x83\x86\xe3\x83\xa3", "1",
   "\xe3\x83\x86\xe3\x82\xa3", "\xe3\x83\x86\xe3\x82\xa3", "1",
   "\xe3\x83\x86", "\xe3\x83\x86", "1",
   "\xe3\x83\x85", "\xe3\x83\x85", "1",
   "\xe3\x83\x84\xe3\x82\xa9", "\xe3\x83\x84\xe3\x82\xa9", "1",
   "\xe3\x83\x84\xe3\x82\xa7", "\xe3\x83\x84\xe3\x82\xa7", "1",
   "\xe3\x83\x84\xe3\x82\xa3", "\xe3\x83\x84\xe3\x82\xa3", "1",
   "\xe3\x83\x84\xe3\x82\xa1", "\xe3\x83\x84\xe3\x82\xa1", "1",
   "\xe3\x83\x84", "\xe3\x83\x84", "1",
   "\xe3\x83\x83", "\xe3\x83\x83", "1",
   "\xe3\x83\x82", "\xe3\x83\x82", "1",
   "\xe3\x83\x81\xe3\x83\xa7", "\xe3\x83\x81\xe3\x83\xa7", "1",
   "\xe3\x83\x81\xe3\x83\xa5", "\xe3\x83\x81\xe3\x83\xa5", "1",
   "\xe3\x83\x81\xe3\x83\xa3", "\xe3\x83\x81\xe3\x83\xa3", "1",
   "\xe3\x83\x81\xe3\x82\xa7", "\xe3\x83\x81\xe3\x82\xa7", "1",
   "\xe3\x83\x81", "\xe3\x83\x81", "1",
   "\xe3\x83\x80", "\xe3\x83\x80", "1",
   "\xe3\x82\xbf", "\xe3\x82\xbf", "1",
   "\xe3\x82\xbe", "\xe3\x82\xbe", "1",
   "\xe3\x82\xbd", "\xe3\x82\xbd", "1",
   "\xe3\x82\xbc", "\xe3\x82\xbc", "1",
   "\xe3\x82\xbb", "\xe3\x82\xbb", "1",
   "\xe3\x82\xba\xe3\x82\xa3", "\xe3\x82\xba\xe3\x82\xa3", "1",
   "\xe3\x82\xba", "\xe3\x82\xba", "1",
   "\xe3\x82\xb9\xe3\x82\xa3", "\xe3\x82\xb9\xe3\x82\xa3", "1",
   "\xe3\x82\xb9", "\xe3\x82\xb9", "1",
   "\xe3\x82\xb8\xe3\x83\xa7", "\xe3\x82\xb8\xe3\x83\xa7", "1",
   "\xe3\x82\xb8\xe3\x83\xa5", "\xe3\x82\xb8\xe3\x83\xa5", "1",
   "\xe3\x82\xb8\xe3\x83\xa3", "\xe3\x82\xb8\xe3\x83\xa3", "1",
   "\xe3\x82\xb8\xe3\x82\xa7", "\xe3\x82\xb8\xe3\x82\xa7", "1",
   "\xe3\x82\xb8", "\xe3\x82\xb8", "1",
   "\xe3\x82\xb7\xe3\x83\xa7", "\xe3\x82\xb7\xe3\x83\xa7", "1",
   "\xe3\x82\xb7\xe3\x83\xa5", "\xe3\x82\xb7\xe3\x83\xa5", "1",
   "\xe3\x82\xb7\xe3\x83\xa3", "\xe3\x82\xb7\xe3\x83\xa3", "1",
   "\xe3\x82\xb7\xe3\x82\xa7", "\xe3\x82\xb7\xe3\x82\xa7", "1",
   "\xe3\x82\xb7", "\xe3\x82\xb7", "1",
   "\xe3\x82\xb6", "\xe3\x82\xb6", "1",
   "\xe3\x82\xb5", "\xe3\x82\xb5", "1",
   "\xe3\x82\xb4", "\xe3\x82\xb4", "1",
   "\xe3\x82\xb3", "\xe3\x82\xb3", "1",
   "\xe3\x82\xb2", "\xe3\x82\xb2", "1",
   "\xe3\x82\xb1", "\xe3\x82\xb1", "1",
   "\xe3\x82\xb0", "\xe3\x82\xb0", "1",
   "\xe3\x82\xaf", "\xe3\x82\xaf", "1",
   "\xe3\x82\xae\xe3\x83\xa7", "\xe3\x82\xae\xe3\x83\xa7", "1",
   "\xe3\x82\xae\xe3\x83\xa5", "\xe3\x82\xae\xe3\x83\xa5", "1",
   "\xe3\x82\xae\xe3\x83\xa3", "\xe3\x82\xae\xe3\x83\xa3", "1",
   "\xe3\x82\xae\xe3\x82\xa7", "\xe3\x82\xae\xe3\x82\xa7", "1",
   "\xe3\x82\xae", "\xe3\x82\xae", "1",
   "\xe3\x82\xad\xe3\x83\xa7", "\xe3\x82\xad\xe3\x83\xa7", "1",
   "\xe3\x82\xad\xe3\x83\xa5", "\xe3\x82\xad\xe3\x83\xa5", "1",
   "\xe3\x82\xad\xe3\x83\xa3", "\xe3\x82\xad\xe3\x83\xa3", "1",
   "\xe3\x82\xad\xe3\x82\xa7", "\xe3\x82\xad\xe3\x82\xa7", "1",
   "\xe3\x82\xad", "\xe3\x82\xad", "1",
   "\xe3\x82\xac", "\xe3\x82\xac", "1",
   "\xe3\x82\xab", "\xe3\x82\xab", "1",
   "\xe3\x82\xaa", "\xe3\x82\xaa", "1",
   "\xe3\x82\xa9", "\xe3\x82\xa9", "1",
   "\xe3\x82\xa8", "\xe3\x82\xa8", "1",
   "\xe3\x82\xa7", "\xe3\x82\xa7", "1",
   "\xe3\x82\xa6\xe3\x82\xa9", "\xe3\x82\xa6\xe3\x82\xa9", "1",
   "\xe3\x82\xa6\xe3\x82\xa7", "\xe3\x82\xa6\xe3\x82\xa7", "1",
   "\xe3\x82\xa6\xe3\x82\xa3", "\xe3\x82\xa6\xe3\x82\xa3", "1",
   "\xe3\x82\xa6", "\xe3\x82\xa6", "1",
   "\xe3\x82\xa5", "\xe3\x82\xa5", "1",
   "\xe3\x82\xa4\xe3\x82\xa7", "\xe3\x82\xa4\xe3\x82\xa7", "1",
   "\xe3\x82\xa4", "\xe3\x82\xa4", "1",
   "\xe3\x82\xa3", "\xe3\x82\xa3", "1",
   "\xe3\x82\xa2", "\xe3\x82\xa2", "1",
   "\xe3\x82\xa1", "\xe3\x82\xa1", "1",
   "\xe3\x82\x94\xe3\x82\x87", "\xe3\x83\xb4\xe3\x83\xa7", "1",
   "\xe3\x82\x94\xe3\x82\x85", "\xe3\x83\xb4\xe3\x83\xa5", "1",
   "\xe3\x82\x94\xe3\x82\x83", "\xe3\x83\xb4\xe3\x83\xa3", "1",
   "\xe3\x82\x94\xe3\x81\x89", "\xe3\x83\xb4\xe3\x82\xa9", "1",
   "\xe3\x82\x94\xe3\x81\x87", "\xe3\x83\xb4\xe3\x82\xa7", "1",
   "\xe3\x82\x94\xe3\x81\x83", "\xe3\x83\xb4\xe3\x82\xa3", "1",
   "\xe3\x82\x94\xe3\x81\x81", "\xe3\x83\xb4\xe3\x82\xa1", "1",
   "\xe3\x82\x94", "\xe3\x83\xb4", "1",
   "\xe3\x82\x93", "\xe3\x83\xb3", "1",
   "\xe3\x82\x92", "\xe3\x83\xb2", "1",
   "\xe3\x82\x91", "\xe3\x83\xb1", "1",
   "\xe3\x82\x90", "\xe3\x83\xb0", "1",
   "\xe3\x82\x8f", "\xe3\x83\xaf", "1",
   "\xe3\x82\x8d", "\xe3\x83\xad", "1",
   "\xe3\x82\x8c", "\xe3\x83\xac", "1",
   "\xe3\x82\x8b", "\xe3\x83\xab", "1",
   "\xe3\x82\x8a\xe3\x82\x87", "\xe3\x83\xaa\xe3\x83\xa7", "1",
   "\xe3\x82\x8a\xe3\x82\x85", "\xe3\x83\xaa\xe3\x83\xa5", "1",
   "\xe3\x82\x8a\xe3\x82\x83", "\xe3\x83\xaa\xe3\x83\xa3", "1",
   "\xe3\x82\x8a\xe3\x81\x87", "\xe3\x83\xaa\xe3\x82\xa7", "1",
   "\xe3\x82\x8a", "\xe3\x83\xaa", "1",
   "\xe3\x82\x89", "\xe3\x83\xa9", "1",
   "\xe3\x82\x88", "\xe3\x83\xa8", "1",
   "\xe3\x82\x87", "\xe3\x83\xa7", "1",
   "\xe3\x82\x86", "\xe3\x83\xa6", "1",
   "\xe3\x82\x85", "\xe3\x83\xa5", "1",
   "\xe3\x82\x84", "\xe3\x83\xa4", "1",
   "\xe3\x82\x83", "\xe3\x83\xa3", "1",
   "\xe3\x82\x82", "\xe3\x83\xa2", "1",
   "\xe3\x82\x81", "\xe3\x83\xa1", "1",
   "\xe3\x82\x80", "\xe3\x83\xa0", "1",
   "\xe3\x81\xbf\xe3\x82\x87", "\xe3\x83\x9f\xe3\x83\xa7", "1",
   "\xe3\x81\xbf\xe3\x82\x85", "\xe3\x83\x9f\xe3\x83\xa5", "1",
   "\xe3\x81\xbf\xe3\x82\x83", "\xe3\x83\x9f\xe3\x83\xa3", "1",
   "\xe3\x81\xbf\xe3\x81\x87", "\xe3\x83\x9f\xe3\x82\xa7", "1",
   "\xe3\x81\xbf", "\xe3\x83\x9f", "1",
   "\xe3\x81\xbe", "\xe3\x83\x9e", "1",
   "\xe3\x81\xbd", "\xe3\x83\x9d", "1",
   "\xe3\x81\xbc", "\xe3\x83\x9c", "1",
   "\xe3\x81\xbb", "\xe3\x83\x9b", "1",
   "\xe3\x81\xba", "\xe3\x83\x9a", "1",
   "\xe3\x81\xb9", "\xe3\x83\x99", "1",
   "\xe3\x81\xb8", "\xe3\x83\x98", "1",
   "\xe3\x81\xb7", "\xe3\x83\x97", "1",
   "\xe3\x81\xb6", "\xe3\x83\x96", "1",
   "\xe3\x81\xb5\xe3\x81\x89", "\xe3\x83\x95\xe3\x82\xa9", "1",
   "\xe3\x81\xb5\xe3\x81\x87", "\xe3\x83\x95\xe3\x82\xa7", "1",
   "\xe3\x81\xb5\xe3\x81\x83", "\xe3\x83\x95\xe3\x82\xa3", "1",
   "\xe3\x81\xb5\xe3\x81\x81", "\xe3\x83\x95\xe3\x82\xa1", "1",
   "\xe3\x81\xb5", "\xe3\x83\x95", "1",
   "\xe3\x81\xb4\xe3\x82\x87", "\xe3\x83\x94\xe3\x83\xa7", "1",
   "\xe3\x81\xb4\xe3\x82\x85", "\xe3\x83\x94\xe3\x83\xa5", "1",
   "\xe3\x81\xb4\xe3\x82\x83", "\xe3\x83\x94\xe3\x83\xa3", "1",
   "\xe3\x81\xb4\xe3\x81\x87", "\xe3\x83\x94\xe3\x82\xa7", "1",
   "\xe3\x81\xb4", "\xe3\x83\x94", "1",
   "\xe3\x81\xb3\xe3\x82\x87", "\xe3\x83\x93\xe3\x83\xa7", "1",
   "\xe3\x81\xb3\xe3\x82\x85", "\xe3\x83\x93\xe3\x83\xa5", "1",
   "\xe3\x81\xb3\xe3\x82\x83", "\xe3\x83\x93\xe3\x83\xa3", "1",
   "\xe3\x81\xb3\xe3\x81\x87", "\xe3\x83\x93\xe3\x82\xa7", "1",
   "\xe3\x81\xb3", "\xe3\x83\x93", "1",
   "\xe3\x81\xb2\xe3\x82\x87", "\xe3\x83\x92\xe3\x83\xa7", "1",
   "\xe3\x81\xb2\xe3\x82\x85", "\xe3\x83\x92\xe3\x83\xa5", "1",
   "\xe3\x81\xb2\xe3\x82\x83", "\xe3\x83\x92\xe3\x83\xa3", "1",
   "\xe3\x81\xb2\xe3\x81\x87", "\xe3\x83\x92\xe3\x82\xa7", "1",
   "\xe3\x81\xb2", "\xe3\x83\x92", "1",
   "\xe3\x81\xb1", "\xe3\x83\x91", "1",
   "\xe3\x81\xb0", "\xe3\x83\x90", "1",
   "\xe3\x81\xaf", "\xe3\x83\x8f", "1",
   "\xe3\x81\xae", "\xe3\x83\x8e", "1",
   "\xe3\x81\xad", "\xe3\x83\x8d", "1",
   "\xe3\x81\xac", "\xe3\x83\x8c", "1",
   "\xe3\x81\xab\xe3\x82\x87", "\xe3\x83\x8b\xe3\x83\xa7", "1",
   "\xe3\x81\xab\xe3\x82\x85", "\xe3\x83\x8b\xe3\x83\xa5", "1",
   "\xe3\x81\xab\xe3\x82\x83", "\xe3\x83\x8b\xe3\x83\xa3", "1",
   "\xe3\x81\xab\xe3\x81\x87", "\xe3\x83\x8b\xe3\x82\xa7", "1",
   "\xe3\x81\xab", "\xe3\x83\x8b", "1",
   "\xe3\x81\xaa", "\xe3\x83\x8a", "1",
   "\xe3\x81\xa9\xe3\x81\x85", "\xe3\x83\x89\xe3\x82\xa5", "1",
   "\xe3\x81\xa9", "\xe3\x83\x89", "1",
   "\xe3\x81\xa8\xe3\x81\x85", "\xe3\x83\x88\xe3\x82\xa5", "1",
   "\xe3\x81\xa8", "\xe3\x83\x88", "1",
   "\xe3\x81\xa7\xe3\x82\x87", "\xe3\x83\x87\xe3\x83\xa7", "1",
   "\xe3\x81\xa7\xe3\x82\x85", "\xe3\x83\x87\xe3\x83\xa5", "1",
   "\xe3\x81\xa7\xe3\x82\x83", "\xe3\x83\x87\xe3\x83\xa3", "1",
   "\xe3\x81\xa7\xe3\x81\x83", "\xe3\x83\x87\xe3\x82\xa3", "1",
   "\xe3\x81\xa7", "\xe3\x83\x87", "1",
   "\xe3\x81\xa6\xe3\x82\x87", "\xe3\x83\x86\xe3\x83\xa7", "1",
   "\xe3\x81\xa6\xe3\x82\x85", "\xe3\x83\x86\xe3\x83\xa5", "1",
   "\xe3\x81\xa6\xe3\x82\x83", "\xe3\x83\x86\xe3\x83\xa3", "1",
   "\xe3\x81\xa6\xe3\x81\x83", "\xe3\x83\x86\xe3\x82\xa3", "1",
   "\xe3\x81\xa6", "\xe3\x83\x86", "1",
   "\xe3\x81\xa5", "\xe3\x83\x85", "1",
   "\xe3\x81\xa4\xe3\x81\x89", "\xe3\x83\x84\xe3\x82\xa9", "1",
   "\xe3\x81\xa4\xe3\x81\x87", "\xe3\x83\x84\xe3\x82\xa7", "1",
   "\xe3\x81\xa4\xe3\x81\x83", "\xe3\x83\x84\xe3\x82\xa3", "1",
   "\xe3\x81\xa4\xe3\x81\x81", "\xe3\x83\x84\xe3\x82\xa1", "1",
   "\xe3\x81\xa4", "\xe3\x83\x84", "1",
   "\xe3\x81\xa3", "\xe3\x83\x83", "1",
   "\xe3\x81\xa2", "\xe3\x83\x82", "1",
   "\xe3\x81\xa1\xe3\x82\x87", "\xe3\x83\x81\xe3\x83\xa7", "1",
   "\xe3\x81\xa1\xe3\x82\x85", "\xe3\x83\x81\xe3\x83\xa5", "1",
   "\xe3\x81\xa1\xe3\x82\x83", "\xe3\x83\x81\xe3\x83\xa3", "1",
   "\xe3\x81\xa1\xe3\x81\x87", "\xe3\x83\x81\xe3\x82\xa7", "1",
   "\xe3\x81\xa1", "\xe3\x83\x81", "1",
   "\xe3\x81\xa0", "\xe3\x83\x80", "1",
   "\xe3\x81\x9f", "\xe3\x82\xbf", "1",
   "\xe3\x81\x9e", "\xe3\x82\xbe", "1",
   "\xe3\x81\x9d", "\xe3\x82\xbd", "1",
   "\xe3\x81\x9c", "\xe3\x82\xbc", "1",
   "\xe3\x81\x9b", "\xe3\x82\xbb", "1",
   "\xe3\x81\x9a\xe3\x81\x83", "\xe3\x82\xba\xe3\x82\xa3", "1",
   "\xe3\x81\x9a", "\xe3\x82\xba", "1",
   "\xe3\x81\x99\xe3\x81\x83", "\xe3\x82\xb9\xe3\x82\xa3", "1",
   "\xe3\x81\x99", "\xe3\x82\xb9", "1",
   "\xe3\x81\x98\xe3\x82\x87", "\xe3\x82\xb8\xe3\x83\xa7", "1",
   "\xe3\x81\x98\xe3\x82\x85", "\xe3\x82\xb8\xe3\x83\xa5", "1",
   "\xe3\x81\x98\xe3\x82\x83", "\xe3\x82\xb8\xe3\x83\xa3", "1",
   "\xe3\x81\x98\xe3\x81\x87", "\xe3\x82\xb8\xe3\x82\xa7", "1",
   "\xe3\x81\x98", "\xe3\x82\xb8", "1",
   "\xe3\x81\x97\xe3\x82\x87", "\xe3\x82\xb7\xe3\x83\xa7", "1",
   "\xe3\x81\x97\xe3\x82\x85", "\xe3\x82\xb7\xe3\x83\xa5", "1",
   "\xe3\x81\x97\xe3\x82\x83", "\xe3\x82\xb7\xe3\x83\xa3", "1",
   "\xe3\x81\x97\xe3\x81\x87", "\xe3\x82\xb7\xe3\x82\xa7", "1",
   "\xe3\x81\x97", "\xe3\x82\xb7", "1",
   "\xe3\x81\x96", "\xe3\x82\xb6", "1",
   "\xe3\x81\x95", "\xe3\x82\xb5", "1",
   "\xe3\x81\x94", "\xe3\x82\xb4", "1",
   "\xe3\x81\x93", "\xe3\x82\xb3", "1",
   "\xe3\x81\x92", "\xe3\x82\xb2", "1",
   "\xe3\x81\x91", "\xe3\x82\xb1", "1",
   "\xe3\x81\x90", "\xe3\x82\xb0", "1",
   "\xe3\x81\x8f", "\xe3\x82\xaf", "1",
   "\xe3\x81\x8e\xe3\x82\x87", "\xe3\x82\xae\xe3\x83\xa7", "1",
   "\xe3\x81\x8e\xe3\x82\x85", "\xe3\x82\xae\xe3\x83\xa5", "1",
   "\xe3\x81\x8e\xe3\x82\x83", "\xe3\x82\xae\xe3\x83\xa3", "1",
   "\xe3\x81\x8e\xe3\x81\x87", "\xe3\x82\xae\xe3\x82\xa7", "1",
   "\xe3\x81\x8e", "\xe3\x82\xae", "1",
   "\xe3\x81\x8d\xe3\x82\x87", "\xe3\x82\xad\xe3\x83\xa7", "1",
   "\xe3\x81\x8d\xe3\x82\x85", "\xe3\x82\xad\xe3\x83\xa5", "1",
   "\xe3\x81\x8d\xe3\x82\x83", "\xe3\x82\xad\xe3\x83\xa3", "1",
   "\xe3\x81\x8d\xe3\x81\x87", "\xe3\x82\xad\xe3\x82\xa7", "1",
   "\xe3\x81\x8d", "\xe3\x82\xad", "1",
   "\xe3\x81\x8c", "\xe3\x82\xac", "1",
   "\xe3\x81\x8b", "\xe3\x82\xab", "1",
   "\xe3\x81\x8a", "\xe3\x82\xaa", "1",
   "\xe3\x81\x89", "\xe3\x82\xa9", "1",
   "\xe3\x81\x88", "\xe3\x82\xa8", "1",
   "\xe3\x81\x87", "\xe3\x82\xa7", "1",
   "\xe3\x81\x86\xe3\x81\x89", "\xe3\x82\xa6\xe3\x82\xa9", "1",
   "\xe3\x81\x86\xe3\x81\x87", "\xe3\x82\xa6\xe3\x82\xa7", "1",
   "\xe3\x81\x86\xe3\x81\x83", "\xe3\x82\xa6\xe3\x82\xa3", "1",
   "\xe3\x81\x86", "\xe3\x82\xa6", "1",
   "\xe3\x81\x85", "\xe3\x82\xa5", "1",
   "\xe3\x81\x84\xe3\x81\x87", "\xe3\x82\xa4\xe3\x82\xa7", "1",
   "\xe3\x81\x84", "\xe3\x82\xa4", "1",
   "\xe3\x81\x83", "\xe3\x82\xa3", "1",
   "\xe3\x81\x82", "\xe3\x82\xa2", "1",
   "\xe3\x81\x81", "\xe3\x82\xa1", "1",
   "\xef\xbd\x9a", "\xe3\x82\xba\xe3\x82\xa3\xe3\x83\xbc", "2",
   "\xef\xbd\x99", "\xe3\x83\xaf\xe3\x82\xa4", "2",
   "\xef\xbd\x98", "\xe3\x82\xa8\xe3\x83\x83\xe3\x82\xaf\xe3\x82\xb9", "4",
   "\xef\xbd\x97", "\xe3\x83\x80\xe3\x83\x96\xe3\x83\xaa\xe3\x83\xa5\xe3\x83\xbc", "4",
   "\xef\xbd\x96", "\xe3\x83\x96\xe3\x82\xa4", "2",
   "\xef\xbd\x95", "\xe3\x83\xa6\xe3\x83\xbc", "2",
   "\xef\xbd\x94", "\xe3\x83\x86\xe3\x82\xa3\xe3\x83\xbc", "2",
   "\xef\xbd\x93", "\xe3\x82\xa8\xe3\x82\xb9", "2",
   "\xef\xbd\x92", "\xe3\x82\xa2\xe3\x83\xbc\xe3\x83\xab", "3",
   "\xef\xbd\x91", "\xe3\x82\xad\xe3\x83\xa5\xe3\x83\xbc", "2",
   "\xef\xbd\x90", "\xe3\x83\x94\xe3\x83\xbc", "2",
   "\xef\xbd\x8f", "\xe3\x82\xaa\xe3\x83\xbc", "2",
   "\xef\xbd\x8e", "\xe3\x82\xa8\xe3\x83\x8c", "2",
   "\xef\xbd\x8d", "\xe3\x82\xa8\xe3\x83\xa0", "2",
   "\xef\xbd\x8c", "\xe3\x82\xa8\xe3\x83\xab", "2",
   "\xef\xbd\x8b", "\xe3\x82\xb1\xe3\x83\xbc", "2",
   "\xef\xbd\x8a", "\xe3\x82\xb8\xe3\x82\xa7\xe3\x83\xbc", "2",
   "\xef\xbd\x89", "\xe3\x82\xa2\xe3\x82\xa4", "2",
   "\xef\xbd\x88", "\xe3\x82\xa8\xe3\x82\xa4\xe3\x83\x81", "3",
   "\xef\xbd\x87", "\xe3\x82\xb8\xe3\x83\xbc", "2",
   "\xef\xbd\x86", "\xe3\x82\xa8\xe3\x83\x95", "2",
   "\xef\xbd\x85", "\xe3\x82\xa4\xe3\x83\xbc", "2",
   "\xef\xbd\x84", "\xe3\x83\x87\xe3\x82\xa3\xe3\x83\xbc", "2",
   "\xef\xbd\x83", "\xe3\x82\xb7\xe3\x83\xbc", "2",
   "\xef\xbd\x82", "\xe3\x83\x93\xe3\x83\xbc", "2",
   "\xef\xbd\x81", "\xe3\x82\xa8\xe3\x83\xbc", "2",
   "\xef\xbc\xba", "\xe3\x82\xba\xe3\x82\xa3\xe3\x83\xbc", "2",
   "\xef\xbc\xb9", "\xe3\x83\xaf\xe3\x82\xa4", "2",
   "\xef\xbc\xb8", "\xe3\x82\xa8\xe3\x83\x83\xe3\x82\xaf\xe3\x82\xb9", "4",
   "\xef\xbc\xb7", "\xe3\x83\x80\xe3\x83\x96\xe3\x83\xaa\xe3\x83\xa5\xe3\x83\xbc", "4",
   "\xef\xbc\xb6", "\xe3\x83\x96\xe3\x82\xa4", "2",
   "\xef\xbc\xb5", "\xe3\x83\xa6\xe3\x83\xbc", "2",
   "\xef\xbc\xb4", "\xe3\x83\x86\xe3\x82\xa3\xe3\x83\xbc", "2",
   "\xef\xbc\xb3", "\xe3\x82\xa8\xe3\x82\xb9", "2",
   "\xef\xbc\xb2", "\xe3\x82\xa2\xe3\x83\xbc\xe3\x83\xab", "3",
   "\xef\xbc\xb1", "\xe3\x82\xad\xe3\x83\xa5\xe3\x83\xbc", "2",
   "\xef\xbc\xb0", "\xe3\x83\x94\xe3\x83\xbc", "2",
   "\xef\xbc\xaf", "\xe3\x82\xaa\xe3\x83\xbc", "2",
   "\xef\xbc\xae", "\xe3\x82\xa8\xe3\x83\x8c", "2",
   "\xef\xbc\xad", "\xe3\x82\xa8\xe3\x83\xa0", "2",
   "\xef\xbc\xac", "\xe3\x82\xa8\xe3\x83\xab", "2",
   "\xef\xbc\xab", "\xe3\x82\xb1\xe3\x83\xbc", "2",
   "\xef\xbc\xaa", "\xe3\x82\xb8\xe3\x82\xa7\xe3\x83\xbc", "2",
   "\xef\xbc\xa9", "\xe3\x82\xa2\xe3\x82\xa4", "2",
   "\xef\xbc\xa8", "\xe3\x82\xa8\xe3\x82\xa4\xe3\x83\x81", "3",
   "\xef\xbc\xa7", "\xe3\x82\xb8\xe3\x83\xbc", "2",
   "\xef\xbc\xa6", "\xe3\x82\xa8\xe3\x83\x95", "2",
   "\xef\xbc\xa5", "\xe3\x82\xa4\xe3\x83\xbc", "2",
   "\xef\xbc\xa4", "\xe3\x83\x87\xe3\x82\xa3\xe3\x83\xbc", "2",
   "\xef\xbc\xa3", "\xe3\x82\xb7\xe3\x83\xbc", "2",
   "\xef\xbc\xa2", "\xe3\x83\x93\xe3\x83\xbc", "2",
   "\xef\xbc\xa1", "\xe3\x82\xa8\xe3\x83\xbc", "2",
   "\xe3\x83\xbc", "\xe3\x83\xbc", "1",
   NULL, NULL, NULL
};

#define NJD_SET_PRONUNCIATION_KIGOU "\xe8\xa8\x98\xe5\x8f\xb7"
#define NJD_SET_PRONUNCIATION_KAZU "\xe6\x95\xb0"

static const char *njd_set_pronunciation_symbol_list[] = {
   "\xef\xbc\x9f", "\xef\xbc\x9f",
   NULL, NULL
};

#define NJD_SET_PRONUNCIATION_FILLER "\xe3\x83\x95\xe3\x82\xa3\xe3\x83\xa9\xe3\x83\xbc"

#define NJD_SET_PRONUNCIATION_U        "\xe3\x82\xa6"
#define NJD_SET_PRONUNCIATION_DOUSHI   "\xe5\x8b\x95\xe8\xa9\x9e"
#define NJD_SET_PRONUNCIATION_JODOUSHI "\xe5\x8a\xa9\xe5\x8b\x95\xe8\xa9\x9e"
#define NJD_SET_PRONUNCIATION_CHOUON   "\xe3\x83\xbc"

#define NJD_SET_PRONUNCIATION_QUESTION  "\xef\xbc\x9f"
#define NJD_SET_PRONUNCIATION_DESU_STR  "\xe3\x81\xa7\xe3\x81\x99"
#define NJD_SET_PRONUNCIATION_MASU_STR  "\xe3\x81\xbe\xe3\x81\x99"
#define NJD_SET_PRONUNCIATION_DESU_PRON "\xe3\x83\x87\xe3\x82\xb9"
#define NJD_SET_PRONUNCIATION_MASU_PRON "\xe3\x83\x9e\xe3\x82\xb9"

#define NJD_SET_PRONUNCIATION_TOUTEN "\xe3\x80\x81"

NJD_SET_PRONUNCIATION_RULE_H_END;

#endif                          /* !NJD_SET_PRONUNCIATION_RULE_H */
