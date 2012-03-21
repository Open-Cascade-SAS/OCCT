// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

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
