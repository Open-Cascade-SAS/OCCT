#ifndef OPENGL_MEMORY_H
#define OPENGL_MEMORY_H

#include <OpenGl_tgl_all.hxx>

template <class XType> XType *cmn_resizemem( XType *ptr, Tint size )
{
  size *= sizeof(XType);

  ptr = (XType*)realloc( ptr, size );

  if ( !ptr ) {
    fprintf(stderr, "Could not reallocate '%d'\
                    bytes of memory.\n", size);
  }

  return ptr;
}

#endif //OPENGL_MEMORY_H
