// Created by: Peter KURNEV
// Copyright (c) 1999-2013 OPEN CASCADE SAS
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
