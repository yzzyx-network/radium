// This file is automatically generated from gfx_op_queue.scm 

#ifdef OP_ENUMS 
ENUM_GFX_C2V_bitBlt, 
ENUM_GFX_C_DrawCursor, 
ENUM_GFX_P2V_bitBlt, 
ENUM_GFX_P_FilledBox, 
ENUM_GFX_P_Box, 
ENUM_GFX_P_SetClipRect, 
ENUM_GFX_P_CancelClipRect, 
ENUM_GFX_SetClipRect, 
ENUM_GFX_CancelClipRect, 
ENUM_GFX_P_Line, 
ENUM_GFX_P_Text, 
ENUM_GFX_Line, 
ENUM_GFX_Box, 
ENUM_GFX_FilledBox, 
ENUM_GFX_Text, 
ENUM_GFX_BitBlt, 
#endif 

#ifdef OP_CASES 
case ENUM_GFX_C2V_bitBlt: OS_GFX_C2V_bitBlt(window, el->i1, el->i2, el->i3); break; 
case ENUM_GFX_C_DrawCursor: OS_GFX_C_DrawCursor(window, el->i1, el->i2, el->i3, el->i4, el->i5, el->i6); break; 
case ENUM_GFX_P2V_bitBlt: OS_GFX_P2V_bitBlt(window, el->i1, el->i2, el->i3, el->i4, el->i5, el->i6); break; 
case ENUM_GFX_P_FilledBox: OS_GFX_P_FilledBox(window, el->i1, el->i2, el->i3, el->i4, el->i5); break; 
case ENUM_GFX_P_Box: OS_GFX_P_Box(window, el->i1, el->i2, el->i3, el->i4, el->i5); break; 
case ENUM_GFX_P_SetClipRect: OS_GFX_P_SetClipRect(window, el->i1, el->i2, el->i3, el->i4); break; 
case ENUM_GFX_P_CancelClipRect: OS_GFX_P_CancelClipRect(window); break; 
case ENUM_GFX_SetClipRect: OS_GFX_SetClipRect(window, el->i1, el->i2, el->i3, el->i4); break; 
case ENUM_GFX_CancelClipRect: OS_GFX_CancelClipRect(window); break; 
case ENUM_GFX_P_Line: PREOS_GFX_P_Line(window, el->i1, el->i2, el->i3, el->i4, el->i5); break; 
case ENUM_GFX_P_Text: PREOS_GFX_P_Text(window, el->i1, el->s2, el->i3, el->i4, el->i5, el->i6); break; 
case ENUM_GFX_Line: OS_GFX_Line(window, el->i1, el->i2, el->i3, el->i4, el->i5); break; 
case ENUM_GFX_Box: OS_GFX_Box(window, el->i1, el->i2, el->i3, el->i4, el->i5); break; 
case ENUM_GFX_FilledBox: OS_GFX_FilledBox(window, el->i1, el->i2, el->i3, el->i4, el->i5); break; 
case ENUM_GFX_Text: PREOS_GFX_Text(window, el->i1, el->s2, el->i3, el->i4, el->i5, el->i6); break; 
case ENUM_GFX_BitBlt: OS_GFX_BitBlt(window, el->i1, el->i2, el->i3, el->i4, el->i5, el->i6); break; 
#endif 

#ifdef OP_FUNCS 
void QUEUE_GFX_C2V_bitBlt(struct Tracker_Windows* window, int from_x1, int from_x2, int to_y){ 
  queue_element_t *el = get_next_element(window->op_queue); 
  el->type = ENUM_GFX_C2V_bitBlt ; 
  el->i1 = from_x1 ; 
  el->i2 = from_x2 ; 
  el->i3 = to_y ; 
} 

void QUEUE_GFX_C_DrawCursor(struct Tracker_Windows* window, int x1, int x2, int x3, int x4, int height, int y_pixmap){ 
  queue_element_t *el = get_next_element(window->op_queue); 
  el->type = ENUM_GFX_C_DrawCursor ; 
  el->i1 = x1 ; 
  el->i2 = x2 ; 
  el->i3 = x3 ; 
  el->i4 = x4 ; 
  el->i5 = height ; 
  el->i6 = y_pixmap ; 
} 

void QUEUE_GFX_P2V_bitBlt(struct Tracker_Windows* window, int from_x, int from_y, int to_x, int to_y, int width, int height){ 
  queue_element_t *el = get_next_element(window->op_queue); 
  el->type = ENUM_GFX_P2V_bitBlt ; 
  el->i1 = from_x ; 
  el->i2 = from_y ; 
  el->i3 = to_x ; 
  el->i4 = to_y ; 
  el->i5 = width ; 
  el->i6 = height ; 
} 

void QUEUE_GFX_P_FilledBox(struct Tracker_Windows* tvisual, int color, int x, int y, int x2, int y2){ 
  queue_element_t *el = get_next_element(tvisual->op_queue); 
  el->type = ENUM_GFX_P_FilledBox ; 
  el->i1 = color ; 
  el->i2 = x ; 
  el->i3 = y ; 
  el->i4 = x2 ; 
  el->i5 = y2 ; 
} 

void QUEUE_GFX_P_Box(struct Tracker_Windows* tvisual, int color, int x, int y, int x2, int y2){ 
  queue_element_t *el = get_next_element(tvisual->op_queue); 
  el->type = ENUM_GFX_P_Box ; 
  el->i1 = color ; 
  el->i2 = x ; 
  el->i3 = y ; 
  el->i4 = x2 ; 
  el->i5 = y2 ; 
} 

void QUEUE_GFX_P_SetClipRect(struct Tracker_Windows* tvisual, int x, int y, int x2, int y2){ 
  queue_element_t *el = get_next_element(tvisual->op_queue); 
  el->type = ENUM_GFX_P_SetClipRect ; 
  el->i1 = x ; 
  el->i2 = y ; 
  el->i3 = x2 ; 
  el->i4 = y2 ; 
} 

void QUEUE_GFX_P_CancelClipRect(struct Tracker_Windows* tvisual){ 
  queue_element_t *el = get_next_element(tvisual->op_queue); 
  el->type = ENUM_GFX_P_CancelClipRect ; 
} 

void QUEUE_GFX_SetClipRect(struct Tracker_Windows* tvisual, int x, int y, int x2, int y2){ 
  queue_element_t *el = get_next_element(tvisual->op_queue); 
  el->type = ENUM_GFX_SetClipRect ; 
  el->i1 = x ; 
  el->i2 = y ; 
  el->i3 = x2 ; 
  el->i4 = y2 ; 
} 

void QUEUE_GFX_CancelClipRect(struct Tracker_Windows* tvisual){ 
  queue_element_t *el = get_next_element(tvisual->op_queue); 
  el->type = ENUM_GFX_CancelClipRect ; 
} 

void QUEUE_GFX_P_Line(struct Tracker_Windows* tvisual, int color, int x, int y, int x2, int y2){ 
  queue_element_t *el = get_next_element(tvisual->op_queue); 
  el->type = ENUM_GFX_P_Line ; 
  el->i1 = color ; 
  el->i2 = x ; 
  el->i3 = y ; 
  el->i4 = x2 ; 
  el->i5 = y2 ; 
} 

void QUEUE_GFX_P_Text(struct Tracker_Windows* tvisual, int color, char* text, int x, int y, int width, int flags){ 
  queue_element_t *el = get_next_element(tvisual->op_queue); 
  el->type = ENUM_GFX_P_Text ; 
  el->i1 = color ; 
  memcpy(el->s2, text, R_MIN(strlen(text)+1,62)); 
  el->i3 = x ; 
  el->i4 = y ; 
  el->i5 = width ; 
  el->i6 = flags ; 
} 

void QUEUE_GFX_Line(struct Tracker_Windows* tvisual, int color, int x, int y, int x2, int y2){ 
  queue_element_t *el = get_next_element(tvisual->op_queue); 
  el->type = ENUM_GFX_Line ; 
  el->i1 = color ; 
  el->i2 = x ; 
  el->i3 = y ; 
  el->i4 = x2 ; 
  el->i5 = y2 ; 
} 

void QUEUE_GFX_Box(struct Tracker_Windows* tvisual, int color, int x, int y, int x2, int y2){ 
  queue_element_t *el = get_next_element(tvisual->op_queue); 
  el->type = ENUM_GFX_Box ; 
  el->i1 = color ; 
  el->i2 = x ; 
  el->i3 = y ; 
  el->i4 = x2 ; 
  el->i5 = y2 ; 
} 

void QUEUE_GFX_FilledBox(struct Tracker_Windows* tvisual, int color, int x, int y, int x2, int y2){ 
  queue_element_t *el = get_next_element(tvisual->op_queue); 
  el->type = ENUM_GFX_FilledBox ; 
  el->i1 = color ; 
  el->i2 = x ; 
  el->i3 = y ; 
  el->i4 = x2 ; 
  el->i5 = y2 ; 
} 

void QUEUE_GFX_Text(struct Tracker_Windows* tvisual, int color, char* text, int x, int y, int width, int flags){ 
  queue_element_t *el = get_next_element(tvisual->op_queue); 
  el->type = ENUM_GFX_Text ; 
  el->i1 = color ; 
  memcpy(el->s2, text, R_MIN(strlen(text)+1,62)); 
  el->i3 = x ; 
  el->i4 = y ; 
  el->i5 = width ; 
  el->i6 = flags ; 
} 

void QUEUE_GFX_BitBlt(struct Tracker_Windows* tvisual, int dx, int dy, int x, int y, int x2, int y2){ 
  queue_element_t *el = get_next_element(tvisual->op_queue); 
  el->type = ENUM_GFX_BitBlt ; 
  el->i1 = dx ; 
  el->i2 = dy ; 
  el->i3 = x ; 
  el->i4 = y ; 
  el->i5 = x2 ; 
  el->i6 = y2 ; 
} 

#endif 
