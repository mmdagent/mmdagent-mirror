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

/* headers */
#include <ctype.h>

#include "MMDAgent.h"
#include "VIManager.h"

/* findAsciiString: find ascii string */
static const char* findAsciiString(const char *str, const char *c)
{
   if (MMDAgent_getcharsize(c) != 1)
      return NULL;
   return strchr(str, (int) * c);
}

/* getTokenFromStringWithQuoters: a general string separator with quote handling, quote will be erased */
static int getTokenFromStringWithQuoters(const char *str, int *index, char *buff, const char *separators, const char *quoters)
{
   int i, j = 0, len;
   const char *c;
   unsigned char size;
   bool in_quote = false;
   char current_quote;

   if (str == NULL) {
      buff[0] = '\0';
      return 0;
   }

   len = MMDAgent_strlen(str);
   if (len <= 0) {
      buff[0] = '\0';
      return 0;
   }

   /* skip the first separator sequence to move forward to the head of the next token */
   c = str + (*index);
   for (i = 0; i < len && (*c == '\0' || findAsciiString(separators, c) != NULL); i += size) {
      if (*c == '\0') {
         buff[0] = '\0';
         return 0;
      }
      size = MMDAgent_getcharsize(c);
      (*index) += size;
      c += size;
   }

   /* copy the token to buff till the end of the token */
   for (i = 0; i < len && *c != '\0' && (in_quote == true || findAsciiString(separators, c) == NULL); i += size) {
      if (quoters != NULL && findAsciiString(quoters, c) != NULL && ! (in_quote == true && *c != current_quote)) {
         /* toggle quote mode */
         if (in_quote)
            in_quote = false;
         else {
            in_quote = true;
            current_quote = *c;
         }
         /* skip quote */
         size = MMDAgent_getcharsize(c);
         (*index) += size;
         c += size;
         continue;
      }
      size = MMDAgent_getcharsize(c);
      memcpy(&buff[j], c, sizeof(char) * size);
      j += size;
      (*index) += size;
      c += size;
   }

   buff[j] = '\0';

   /* move index forward to skip the last separator sequence */
   for (; i < len && *c != '\0' && findAsciiString(separators, c) != NULL; i += size) {
      size = MMDAgent_getcharsize(c);
      (*index) += size;
      c += size;
   }

   return j;
}

/* getTokenFromString: get token from string for reading FST definition fields */
static int getTokenFromString(const char *str, int *index, char *buff)
{
   return getTokenFromStringWithQuoters(str, index, buff, " \t\n\r", "\"'");
}

/* getArgFromString: get argument from string for parsing argment string */
static int getArgFromString(const char *str, int *index, char *buff, char separator)
{
   char separatorList[2];
   separatorList[0] = separator;
   separatorList[1] = '\0';
   return getTokenFromStringWithQuoters(str, index, buff, separatorList, NULL);
}

/* countArgs: count arguments */
static int countArgs(const char *str, char separator)
{
   int i, len, num;
   const char *c;
   unsigned char size;

   len = MMDAgent_strlen(str);
   if(len <= 0)
      return 0;

   num = 1;
   c = str;
   for(i = 0; i < len; i += size) {
      size = MMDAgent_getcharsize(c);
      if (size == 1 && *c == separator)
         num++;
      c += size;
   }

   return num;
}

/* InputArguments_initialize: initialize input arguments */
void InputArguments_initialize(InputArguments *ia, const char *str)
{
   int i, j, idx1, idx2;
   char buff1[MMDAGENT_MAXBUFLEN];
   char buff2[MMDAGENT_MAXBUFLEN];

   /* get number of arguments */
   ia->size = countArgs(str, VIMANAGER_SEPARATOR1);
   if(ia->size <= 0) {
      ia->size = 0;
      ia->args = NULL;
      ia->argc = NULL;
      return;
   }

   ia->argc = (int *) malloc(ia->size * sizeof(int));
   ia->args = (char ***) malloc(ia->size * sizeof(char **));

   /* get event arguments */
   idx1 = 0;
   for(i = 0; i < ia->size; i++) {
      getArgFromString(str, &idx1, buff1, VIMANAGER_SEPARATOR1);

      ia->argc[i] = countArgs(buff1, VIMANAGER_SEPARATOR2);
      if(ia->argc[i] <= 0) {
         ia->args[i] = NULL;
      } else {
         ia->args[i] = (char **) malloc(ia->argc[i] * sizeof(char *));
         idx2 = 0;
         for(j = 0; j < ia->argc[i]; j++) {
            getArgFromString(buff1, &idx2, buff2, VIMANAGER_SEPARATOR2);
            ia->args[i][j] = MMDAgent_strdup(buff2);
         }
      }
   }
}

/* InputArguments_clear: free input arguments */
void InputArguments_clear(InputArguments *ia)
{
   int i, j;

   if(ia->args != NULL) {
      for(i = 0; i < ia->size; i++) {
         for(j = 0; j < ia->argc[i]; j++)
            free(ia->args[i][j]);
         if(ia->args[i] != NULL)
            free(ia->args[i]);
      }
      free(ia->args);
      free(ia->argc);
      ia->size = 0;
      ia->args = NULL;
      ia->argc = NULL;
   }
}

/* VIManager_Arc_initialize: initialize arc */
static void VIManager_Arc_initialize(VIManager_Arc *a, char *input_event_type, char *input_event_args, char *output_command_type, char *output_command_args, char *variable_action, VIManager_State *next_state)
{
   a->input_event_type = MMDAgent_strdup(input_event_type);
   InputArguments_initialize(&a->input_event_args, input_event_args);
   a->output_command_type = MMDAgent_strdup(output_command_type);
   a->output_command_args = MMDAgent_strdup(output_command_args);
   a->variable_action = MMDAgent_strdup(variable_action);
   a->next_state = next_state;
   a->next = NULL;
}

/* VIManager_Arc_clear: free arc */
static void VIManager_Arc_clear(VIManager_Arc * a)
{
   if (a->input_event_type != NULL)
      free(a->input_event_type);
   InputArguments_clear(&a->input_event_args);
   if (a->output_command_type != NULL)
      free(a->output_command_type);
   if (a->output_command_args != NULL)
      free(a->output_command_args);
   if (a->variable_action != NULL) {
      free(a->variable_action);
   }
   VIManager_Arc_initialize(a, NULL, NULL, NULL, NULL, NULL, NULL);
}

/* VIManager_AList_initialize: initialize arc list */
static void VIManager_AList_initialize(VIManager_AList *l)
{
   l->head = NULL;
}

/* VIManager_AList_clear: free arc list */
static void VIManager_AList_clear(VIManager_AList *l)
{
   VIManager_Arc *tmp1, *tmp2;

   for (tmp1 = l->head; tmp1 != NULL; tmp1 = tmp2) {
      tmp2 = tmp1->next;
      VIManager_Arc_clear(tmp1);
      free(tmp1);
   }
   l->head = NULL;
}

/* VIManager_State_initialize: initialize state */
static void VIManager_State_initialize(VIManager_State *s, unsigned int number, VIManager_State * next)
{
   s->number = number;
   VIManager_AList_initialize(&s->arc_list);
   s->next = next;
}

/* VIManager_State_clear: free state */
static void VIManager_State_clear(VIManager_State *s)
{
   VIManager_AList_clear(&s->arc_list);
   VIManager_State_initialize(s, 0, NULL);
}

/* VIManager_SList_initialize: initialize state list */
static void VIManager_SList_initialize(VIManager_SList *l)
{
   l->head = NULL;
}

/* VIManager_SList_clear: free state list */
static void VIManager_SList_clear(VIManager_SList *l)
{
   VIManager_State *tmp1, *tmp2;

   for (tmp1 = l->head; tmp1 != NULL; tmp1 = tmp2) {
      tmp2 = tmp1->next;
      VIManager_State_clear(tmp1);
      free(tmp1);
   }
   l->head = NULL;
}

/* VIManager_SList_searchState: search state pointer */
static VIManager_State *VIManager_SList_searchState(VIManager_SList *l, unsigned int n)
{
   VIManager_State *tmp1, *tmp2, *result = NULL;

   if (l->head == NULL) {
      l->head = (VIManager_State *) malloc(sizeof(VIManager_State));
      VIManager_State_initialize(l->head, n, NULL);
      result = l->head;
   } else if (l->head->number == n) {
      result = l->head;
   } else if (l->head->number > n) {
      tmp1 = l->head;
      l->head = (VIManager_State *) malloc(sizeof(VIManager_State));
      VIManager_State_initialize(l->head, n, tmp1);
      result = l->head;
   } else {
      for (tmp1 = l->head; tmp1 != NULL; tmp1 = tmp1->next) {
         if (tmp1->next == NULL) {
            tmp1->next = (VIManager_State *) malloc(sizeof(VIManager_State));
            VIManager_State_initialize(tmp1->next, n, NULL);
            result = tmp1->next;
            break;
         } else if (tmp1->next->number == n) {
            result = tmp1->next;
            break;
         } else if (n < tmp1->next->number) {
            tmp2 = tmp1->next;
            tmp1->next = (VIManager_State *) malloc(sizeof(VIManager_State));
            VIManager_State_initialize(tmp1->next, n, tmp2);
            result = tmp1->next;
            break;
         }
      }
   }
   return result;
}

/* VIManager_SList_addArc: add arc */
static void VIManager_SList_addArc(VIManager_SList *l, int index_s1, int index_s2, char *isymbol, char *osymbol, char *vsymbol)
{
   int i, idx;
   VIManager_State *s1, *s2;
   VIManager_Arc *a1, *a2;
   VIManager_AList *arc_list;

   char itype[MMDAGENT_MAXBUFLEN];
   char iargs[MMDAGENT_MAXBUFLEN];
   char otype[MMDAGENT_MAXBUFLEN];
   char oargs[MMDAGENT_MAXBUFLEN];

   s1 = VIManager_SList_searchState(l, index_s1);
   s2 = VIManager_SList_searchState(l, index_s2);
   arc_list = &s1->arc_list;

   /* analyze input symbol */
   idx = 0;
   i = getArgFromString(isymbol, &idx, itype, VIMANAGER_SEPARATOR1);
   if (i <= 0)
      return;
   getArgFromString(isymbol, &idx, iargs, '\0');

   /* analyze output symbol */
   idx = 0;
   i = getArgFromString(osymbol, &idx, otype, VIMANAGER_SEPARATOR1);
   if (i <= 0)
      return;
   getArgFromString(osymbol, &idx, oargs, '\0');

   /* create */
   a1 = (VIManager_Arc *) malloc(sizeof(VIManager_Arc));
   VIManager_Arc_initialize(a1, itype, iargs, otype, oargs, vsymbol, s2);

   /* set */
   if (arc_list->head == NULL) {
      arc_list->head = a1;
   } else {
      for (a2 = arc_list->head; a2->next != NULL; a2 = a2->next);
      a2->next = a1;
   }
}

/* VIManager_Variable_initialize: initialize variable */
static void VIManager_Variable_initialize(VIManager_Variable *v, const char *name, const char *value)
{
   v->name = MMDAgent_strdup(name);
   v->value = MMDAgent_strdup(value);
   v->next = NULL;
}

/* VIManager_Variable_clear: free variable */
static void VIManager_Variable_clear(VIManager_Variable * v)
{
   if (v->name != NULL)
      free(v->name);
   if (v->value != NULL)
      free(v->value);
}

/* VIManager_VList_initialize: initialize variable list */
static void VIManager_VList_initialize(VIManager_VList *l)
{
   l->head = NULL;
}

/* VIManager_VList_clear: free variable list */
static void VIManager_VList_clear(VIManager_VList *l)
{
   VIManager_Variable *tmp1, *tmp2;

   for (tmp1 = l->head; tmp1 != NULL; tmp1 = tmp2) {
      tmp2 = tmp1->next;
      VIManager_Variable_clear(tmp1);
      free(tmp1);
   }
   l->head = NULL;
}

/* VIManager_VList_search: search for a variable */
static VIManager_Variable *VIManager_VList_search(VIManager_VList *vlist, const char *name)
{
   VIManager_Variable *v;

   if (MMDAgent_strlen(name) <= 0)
      return NULL;
   for (v = vlist->head; v != NULL; v = v->next) {
      if (MMDAgent_strequal(v->name, name)) {
         return v;
      }
   }
   return NULL;
}

/* VIManager_VList_set: set variable to list */
static void VIManager_VList_set(VIManager_VList *vlist, const char *name, const char *value)
{
   VIManager_Variable *v;

   /* search */
   v = VIManager_VList_search(vlist, name);
   if (v != NULL) {
      /* replace */
      if (v->value != NULL)
         free(v->value);
      v->value = MMDAgent_strdup(value);
   } else {
      /* create */
      v = (VIManager_Variable *)malloc(sizeof(VIManager_Variable));
      VIManager_Variable_initialize(v, name, value);
      /* set */
      v->next = vlist->head;
      vlist->head = v;
   }
}

/* VIManager::initialize: initialize VIManager */
void VIManager::initialize()
{
   VIManager_SList_initialize(&m_stateList);
   VIManager_VList_initialize(&m_variableList);
   m_currentState = NULL;
}

/* VIManager:clear: free VIManager */
void VIManager::clear()
{
   VIManager_SList_clear(&m_stateList);
   VIManager_VList_clear(&m_variableList);
   initialize();
}

/* VIManager::substituteVariableAndCopy: substitute variables with their values in a string */
void VIManager::substituteVariableAndCopy(const char *input, char *output)
{
   int i, len, vlen;
   const char *c;
   char *out, *cv;
   unsigned char size;
   bool braced;
   VIManager_Variable *v;

   len = MMDAgent_strlen(input);
   if (len <= 0) {
      *output = '\0';
      return;
   }

   c = input;
   out = output;
   for (i = 0; i < len; i += size) {
      size = MMDAgent_getcharsize(c);
      memcpy(out, c, size);
      if (size == 1 && *c == '$') {
         /* variable start, hold this place to cv and read variable name */
         cv = out;
         c += size;
         out += size;
         i += size;
         if (MMDAgent_getcharsize(c) == 1 && *c == '$') {
            /* "$$" -> "$" */
            c += 1;
            size = 1;
            continue;
         }
         braced = false;
         if (MMDAgent_getcharsize(c) == 1 && *c == '{') {
            braced = true;
            c += 1;
            i += 1;
         }
         for (; i < len; i += size) {
            size = MMDAgent_getcharsize(c);
            if (braced == true) {
               if (MMDAgent_getcharsize(c) == 1 && *c == '}') {
                  c += 1;
                  i += 1;
                  break;
               }
            } else {
               if (size != 1 || ! (isalnum(*c) || *c == '_')) break;
            }
            memcpy(out, c, size);
            c += size;
            out += size;
         }
         *out = '\0';
         /* search for variable */
         v = VIManager_VList_search(&m_variableList, cv + 1);
         /* rewind out buf to the start of variable name */
         out = cv;
         /* overwrite with its value */
         if (v != NULL) {
            vlen = MMDAgent_strlen(v->value);
            memcpy(out, v->value, vlen);
            out += vlen;
         }
         size = 0;
      } else {
         c += size;
         out += size;
      }
   }
   *out = '\0';
}

/* VIManager::checkStringMatch: check if vstr with variables matches the string */
bool VIManager::checkStringMatch(const char *vstr, const char *str)
{
   char buf[MMDAGENT_MAXBUFLEN];

   if (vstr == NULL || str == NULL)
      return false;

   substituteVariableAndCopy(vstr, buf);

   return MMDAgent_strequal(str, buf);
}

/* VIManager::checkArcMatch: check if an arc matches an input */
bool VIManager::checkArcMatch(const char *arc_type, const char *input_type, const InputArguments *arc_arg, const InputArguments *input_arg)
{
   int i, j, k;
   bool match;

   if (checkStringMatch(arc_type, input_type) == false)
      return false;

   if (arc_arg == NULL)
      return false;
   if (input_arg == NULL) {
      if (arc_arg->size == 0)
         return true;
      else
         return false;
   }
   if (arc_arg->size != input_arg->size)
      return false;

   for (i = 0; i < arc_arg->size; i++) {
      for (j = 0; j < arc_arg->argc[i]; j++) {
         match = false;
         for (k = 0; k < input_arg->argc[i]; k++) {
            if (checkStringMatch(arc_arg->args[i][j], input_arg->args[i][k]) == true) {
               match = true;
               break;
            }
         }
         if (match == false)
            return false;
      }
   }
   return true;
}

/* VIManager::assignVariableByEquation: assign variable by equation */
bool VIManager::assignVariableByEquation(const char *va)
{
   int idx1, idx2;
   int s, len;
   char tok[2];
   char buff[MMDAGENT_MAXBUFLEN];
   char buffn[MMDAGENT_MAXBUFLEN];
   char buffv[MMDAGENT_MAXBUFLEN];
   char buffvr[MMDAGENT_MAXBUFLEN];

   if (MMDAgent_strlen(va) == 0)
      return true;

   tok[0] = VIMANAGER_SEPARATOR2;
   tok[1] = '\0';

   idx1 = 0;
   while (getTokenFromStringWithQuoters(va, &idx1, buff, tok, "\"'") != 0) {
      /* separate by equal */
      if (countArgs(buff, '=') != 2)
         return false;
      idx2 = 0;
      getArgFromString(buff, &idx2, buffn, '=');
      if (buffn[0] != '$')
         return false;
      s = 1;
      if (buffn[1] == '{') {
         len = MMDAgent_strlen(buffn);
         if (buffn[len - 1] != '}')
            return false;
         buffn[len - 1] = '\0';
         s = 2;
      }
      getArgFromString(buff, &idx2, buffv, '=');
      substituteVariableAndCopy(buffv, buffvr);
      VIManager_VList_set(&m_variableList, &(buffn[s]), buffvr);
   }
   return true;
}

/* VIManager::VIManager: constructor */
VIManager::VIManager()
{
   initialize();
}

/* VIManager::~VIManager: destructor */
VIManager::~VIManager()
{
   clear();
}

/* VIManager::load: load FST */
bool VIManager::load(const char *file)
{
   FILE *fp;
   char buff[MMDAGENT_MAXBUFLEN];
   int len;
   int idx;

   char buff_s1[MMDAGENT_MAXBUFLEN];
   char buff_s2[MMDAGENT_MAXBUFLEN];
   char buff_is[MMDAGENT_MAXBUFLEN];
   char buff_os[MMDAGENT_MAXBUFLEN];
   char buff_vs[MMDAGENT_MAXBUFLEN];
   char buff_er[MMDAGENT_MAXBUFLEN];
   int size_s1;
   int size_s2;
   int size_is;
   int size_os;
   int size_vs;
   int size_er;
   char *err_s1;
   char *err_s2;
   unsigned int index_s1;
   unsigned int index_s2;

   /* open */
   fp = MMDAgent_fopen(file, "r");
   if (fp == NULL)
      return false;

   /* unload */
   VIManager_SList_clear(&m_stateList);
   VIManager_SList_initialize(&m_stateList);

   while (fgets(buff, MMDAGENT_MAXBUFLEN - 3, fp) != NULL) { /* string + \r + \n + \0 */
      /* remove final \n and \r */
      len = MMDAgent_strlen(buff);
      while (len > 0 && (buff[len - 1] == '\n' || buff[len - 1] == '\r'))
         buff[--len] = '\0';

      /* check and load arc */
      if (len > 0) {
         idx = 0;
         size_s1 = getTokenFromString(buff, &idx, buff_s1);
         size_s2 = getTokenFromString(buff, &idx, buff_s2);
         size_is = getTokenFromString(buff, &idx, buff_is);
         size_os = getTokenFromString(buff, &idx, buff_os);
         size_vs = getTokenFromString(buff, &idx, buff_vs);
         size_er = getTokenFromString(buff, &idx, buff_er);
         if (size_s1 > 0 && size_s2 > 0 && size_is > 0 && size_os > 0 && size_er == 0 && buff_s1[0] != VIMANAGER_COMMENT) {
            index_s1 = (unsigned int) strtoul(buff_s1, &err_s1, 10);
            index_s2 = (unsigned int) strtoul(buff_s2, &err_s2, 10);
            if (buff_s1 + size_s1 == err_s1 && buff_s2 + size_s2 == err_s2)
               VIManager_SList_addArc(&m_stateList, index_s1, index_s2, buff_is, buff_os, buff_vs);
         }
      }
      if (feof(fp) || ferror(fp))
         break;
   }

   fclose(fp);

   /* set current state to zero */
   m_currentState = VIManager_SList_searchState(&m_stateList, VIMANAGER_STARTSTATE);

   return true;
}

/* VIManager::transition: state transition (if jumped, return arc) */
VIManager_Arc *VIManager::transition(const char *itype, const InputArguments *iargs, char *otype, char *oargs)
{
   VIManager_Arc *arc;
   VIManager_AList *arc_list;

   strcpy(otype, VIMANAGER_EPSILON);
   strcpy(oargs, "");

   /* FST isn't loaded yet */
   if (m_currentState == NULL)
      return NULL;

   /* state don't have arc list */
   arc_list = &m_currentState->arc_list;
   if (arc_list->head == NULL)
      return NULL;

   /* match */
   for (arc = arc_list->head; arc != NULL; arc = arc->next) {
      /* check if input matches this arc, consulting variables and wildcards */
      if (checkArcMatch(arc->input_event_type, itype, &arc->input_event_args, iargs)) {
         /* set output string, consulting variables if any */
         substituteVariableAndCopy(arc->output_command_type, otype);
         substituteVariableAndCopy(arc->output_command_args, oargs);
         /* if this arc has variable action, execute it */
         assignVariableByEquation(arc->variable_action);
         /* move to next state */
         m_currentState = arc->next_state;
         return arc;
      }
   }

   return NULL;
}

/* VIManager::getCurrentState: get current state */
VIManager_State *VIManager::getCurrentState()
{
   return m_currentState;
}

/* VIManager::getCurrentVariableList: get current variable list */
VIManager_VList *VIManager::getCurrentVariableList()
{
   return &m_variableList;
}
