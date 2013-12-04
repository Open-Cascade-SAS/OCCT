// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

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
