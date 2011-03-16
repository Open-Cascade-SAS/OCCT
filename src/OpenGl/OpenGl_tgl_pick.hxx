#ifndef  OPENGL_TGL_PICK_H
#define  OPENGL_TGL_PICK_H

typedef  struct
{
  Tint  number;
  Tint *integers;
} Tintlst;

/* Primitive pick and name set ids. */ /* defined in tgl/util */
extern  Tintlst  inc_pick_filter;
extern  Tintlst  exc_pick_filter;

/* Primitive highlight and name set ids. */ /* defined in tgl/util */
extern  Tintlst  inc_highl_filter;
extern  Tintlst  exc_highl_filter;

#define CALL_MAX_DEPTH_LENGTH 128

#endif
