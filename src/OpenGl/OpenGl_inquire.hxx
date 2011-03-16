#ifndef  OPENGL_INQUIRE_H
#define  OPENGL_INQUIRE_H

typedef struct {
  int AntiAliasing;
  int DepthCueing;
  int DoubleBuffer;
  int ZBuffer;
  int MaxLights;
  int MaxPlanes;
  int MaxViews;
} CALL_DEF_INQUIRE;

extern  CALL_DEF_INQUIRE  call_facilities_list; /* defined in tgl/tgl1 */

#endif
