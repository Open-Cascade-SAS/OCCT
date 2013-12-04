// Created by: Peter KURNEV
// Copyright (c) 1999-2013 OPEN CASCADE SAS
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

#ifndef _BOPDS_Col_HeaderFile
#define _BOPDS_Col_HeaderFile

#ifdef HAVE_TBB

// On Windows, function TryEnterCriticalSection has appeared in Windows NT
// and is surrounded by #ifdef in MS VC++ 7.1 headers.
// Thus to use it we need to define appropriate macro saying that we wil
// run on Windows NT 4.0 at least
#if ((defined(_WIN32) || defined(__WIN32__)) && !defined(_WIN32_WINNT))
  #define _WIN32_WINNT 0x0501
#endif

#include <tbb/tbb.h>
using namespace tbb;

#define flexible_range blocked_range
#define flexible_for   parallel_for

#else // not HAVE_TBB

#define flexible_range serial_range
#define flexible_for   serial_for

//=======================================================================
//class : serial_range
//purpose  : 
//=======================================================================
template <class Type> class serial_range {
 public:
  serial_range(const Type& aBegin,
	       const Type& aEnd)
    : myBegin(aBegin), myEnd(aEnd) {
  }
  //
  ~serial_range() {
  }
  //
  const Type& begin() const{
    return myBegin;
  }
  //
  const Type& end() const{
    return myEnd;
  };
  //
 protected:
  Type myBegin;
  Type myEnd;
};

//=======================================================================
//function : serial_for
//purpose  : 
//=======================================================================
template<typename Range, typename Body>
static void serial_for( const Range& range, const Body& body ) {
  body.operator()(range);
};
#endif // not HAVE_TBB

#endif
