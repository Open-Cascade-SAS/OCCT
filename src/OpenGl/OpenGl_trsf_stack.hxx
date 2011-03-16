#ifndef  OPENGL_TRSF_STACK_H
#define  OPENGL_TRSF_STACK_H

typedef struct TSM_TRSF_STACK_STRUCT {
  struct TSM_TRSF_STACK_STRUCT *next;
  struct TSM_TRSF_STACK_STRUCT *prev;
  Tmatrix3 LocalTran3;
} TSM_TRSF_STACK, *tsm_trsf_stack;

#endif
