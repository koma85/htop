/*
htop - ColumnsPanel.c
(C) 2004-2011 Hisham H. Muhammad
Released under the GNU GPL, see the COPYING file
in the source distribution for its full text.
*/

#include "ColumnsPanel.h"

#include "String.h"

#include <assert.h>
#include <stdlib.h>
#include <ctype.h>

/*{
#include "Panel.h"
#include "Settings.h"
#include "ScreenManager.h"

typedef struct ColumnsPanel_ {
   Panel super;

   Settings* settings;
   ScreenManager* scr;
} ColumnsPanel;

}*/

static void ColumnsPanel_delete(Object* object) {
   Panel* super = (Panel*) object;
   ColumnsPanel* this = (ColumnsPanel*) object;
   Panel_done(super);
   free(this);
}

static HandlerResult ColumnsPanel_eventHandler(Panel* super, int ch) {
   
   int selected = Panel_getSelectedIndex(super);
   HandlerResult result = IGNORED;
   int size = Panel_size(super);

   switch(ch) {
      case KEY_F(7):
      case '[':
      case '-':
      {
         if (selected < size - 1)
            Panel_moveSelectedUp(super);
         result = HANDLED;
         break;
      }
      case KEY_F(8):
      case ']':
      case '+':
      {
         if (selected < size - 2) 
            Panel_moveSelectedDown(super);
         result = HANDLED;
         break;
      }
      case KEY_F(9):
      case KEY_DC:
      {
         if (selected < size - 1) {
            Panel_remove(super, selected);
         }
         result = HANDLED;
         break;
      }
      default:
      {
         if (ch < 255 && isalpha(ch))
            result = Panel_selectByTyping(super, ch);
         if (result == BREAK_LOOP)
            result = IGNORED;
         break;
      }
   }
   if (result == HANDLED)
      ColumnsPanel_update(super);
   return result;
}

PanelClass ColumnsPanel_class = {
   .super = {
      .extends = Class(Panel),
      .delete = ColumnsPanel_delete
   },
   .eventHandler = ColumnsPanel_eventHandler
};

ColumnsPanel* ColumnsPanel_new(Settings* settings, ScreenManager* scr) {
   ColumnsPanel* this = AllocThis(ColumnsPanel);
   Panel* super = (Panel*) this;
   Panel_init(super, 1, 1, 1, 1, Class(ListItem), true);

   this->settings = settings;
   this->scr = scr;
   Panel_setHeader(super, "Active Columns");

   ProcessField* fields = this->settings->pl->fields;
   for (; *fields; fields++) {
      Panel_add(super, (Object*) ListItem_new(Process_fieldNames[*fields], 0));
   }
   return this;
}

int ColumnsPanel_fieldNameToIndex(const char* name) {
   for (int j = 1; j <= LAST_PROCESSFIELD; j++) {
      if (String_eq(name, Process_fieldNames[j])) {
         return j;
      }
   }
   return 0;
}

void ColumnsPanel_update(Panel* super) {
   ColumnsPanel* this = (ColumnsPanel*) super;
   int size = Panel_size(super);
   this->settings->changed = true;
   // FIXME: this is crappily inefficient
   free(this->settings->pl->fields);
   this->settings->pl->fields = (ProcessField*) malloc(sizeof(ProcessField) * (size+1));
   this->settings->pl->flags = 0;
   for (int i = 0; i < size; i++) {
      char* text = ((ListItem*) Panel_get(super, i))->value;
          int j = ColumnsPanel_fieldNameToIndex(text);
          if (j > 0) {
             this->settings->pl->fields[i] = j;
             this->settings->pl->flags |= Process_fieldFlags[j];
          }
   }
   this->settings->pl->fields[size] = 0;
}

