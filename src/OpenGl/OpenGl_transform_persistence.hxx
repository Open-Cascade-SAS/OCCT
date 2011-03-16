#ifndef OPENGL_TRANSFORM_PERSISTENCE_H
#define OPENGL_TRANSFORM_PERSISTENCE_H

enum TP_FLAGS
{
  TPF_PAN             = 0x001,
  TPF_ZOOM            = 0x002,
  TPF_RELATIVE_ZOOM   = 0x004,
  TPF_ROTATE          = 0x008,
  TPF_RELATIVE_ROTATE = 0x010,
  TPF_TRIEDRON        = 0x020   
};


extern void  transform_persistence_begin( int, float, float, float );

extern void  transform_persistence_end(void);


#endif /*OPENGL_TRANSFORM_PERSISTENCE_H*/
