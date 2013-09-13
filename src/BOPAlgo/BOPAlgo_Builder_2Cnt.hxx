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

#ifndef BOPAlgo_Builder_2Cnt_HeaderFile
#define BOPAlgo_Builder_2Cnt_HeaderFile

/*
#ifdef HAVE_TBB
#undef HAVE_TBB
#endif
*/
#include <BOPCol_NCVector.hxx>
#include <BOPAlgo_BuilderFace.hxx>

#ifdef HAVE_TBB
#include <BOPCol_TBB.hxx>

#define flexible_range blocked_range
#define flexible_for   parallel_for
#else 
#define flexible_range serial_range
#define flexible_for   serial_for
#endif

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
//
//=======================================================================
//class    : BOPAlgo_VectorOfBuilderFace
//purpose  : 
//=======================================================================
typedef BOPCol_NCVector<BOPAlgo_BuilderFace> BOPAlgo_VectorOfBuilderFace;
//
//=======================================================================
//class    : BOPAlgo_BuilderFaceFunctor
//purpose  : 
//=======================================================================
class BOPAlgo_BuilderFaceFunctor {
 protected:
  BOPAlgo_VectorOfBuilderFace* myPVBF;
  //
 public:
  //
  BOPAlgo_BuilderFaceFunctor(BOPAlgo_VectorOfBuilderFace& aVBF) 
    : myPVBF(&aVBF) {
  }
  //
  void operator()( const flexible_range<Standard_Size>& aBR ) const{
    Standard_Size i, iBeg, iEnd;
    //
    BOPAlgo_VectorOfBuilderFace& aVBF=*myPVBF;
    //
    iBeg=aBR.begin();
    iEnd=aBR.end();
    for(i=iBeg; i!=iEnd; ++i) {
      BOPAlgo_BuilderFace& aBF=aVBF(i);
      //
      aBF.Perform();
    }
  }
};
//=======================================================================
//class    : BOPAlgo_BuilderFaceCnt
//purpose  : 
//=======================================================================
class BOPAlgo_BuilderFaceCnt {
 public:
  //-------------------------------
  // Perform
  Standard_EXPORT static void Perform(const Standard_Boolean bRunParallel,
				      BOPAlgo_VectorOfBuilderFace& aVBF) {
    //
    BOPAlgo_BuilderFaceFunctor aBFF(aVBF);
    Standard_Size aNbBF=aVBF.Extent();
    //
    if (bRunParallel) {
      flexible_for(flexible_range<Standard_Size>(0,aNbBF), aBFF);
    }
    else {
      aBFF.operator()(flexible_range<Standard_Size>(0,aNbBF));
    }
  }
  //
};

#endif
