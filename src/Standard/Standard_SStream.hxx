// File:      Standard_SStream.hxx
// Copyright: Open Cascade 2010
// Purpose:   Defines Standard_SStream as typedef to C++ string stream.

#ifndef _Standard_SStream_HeaderFile
#define _Standard_SStream_HeaderFile

#ifndef _Standard_Stream_HeaderFile
#include <Standard_Stream.hxx>
#endif

#ifdef USE_STL_STREAM

  #include <sstream>

  typedef std::stringstream Standard_SStream;

#else /* USE_STL_STREAM */

  #ifdef WNT
    #include <strstrea.h>
  #else
    #include <strstream.h>
  #endif

  typedef strstream Standard_SStream;

#endif /* USE_STL_STREAM */

#endif
