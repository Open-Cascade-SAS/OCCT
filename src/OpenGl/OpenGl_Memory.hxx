#ifndef OPENGL_MEMORY_H
#define OPENGL_MEMORY_H

#include <OpenGl_tgl_all.hxx>
#include <Standard_TypeDef.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_Stack.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Vector.hxx>


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

class OpenGl_MemoryMgr {
private:
  OpenGl_MemoryMgr();
  OpenGl_MemoryMgr(const OpenGl_MemoryMgr&);
  ~OpenGl_MemoryMgr();
};

#endif //OPENGL_MEMORY_H
