// Created by: Peter KURNEV
// Copyright (c) 1999-2013 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _BOPDS_Col_HeaderFile
#define _BOPDS_Col_HeaderFile

#include <Standard_Macro.hxx>
#include <Standard_NotImplemented.hxx>

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
//
// 2. Implementation of Functors/Starters
//
// 2.1. Pure version
//
//=======================================================================
//class    : BOPCol_TBBFunctor
//purpose  : 
//=======================================================================
template <class TypeSolver, 
          class TypeSolverVector> class BOPCol_TBBFunctor {
 
 public:
  BOPCol_TBBFunctor(TypeSolverVector& aV) 
    : myPV(&aV) {
  }
  //
  ~BOPCol_TBBFunctor() {
  }
  //
  void operator()( const flexible_range<Standard_Integer>& aBR ) const{
    Standard_Integer i, iBeg, iEnd;
    //
    TypeSolverVector& aV=*myPV;
    //
    iBeg=aBR.begin();
    iEnd=aBR.end();
    for(i=iBeg; i!=iEnd; ++i) {
      TypeSolver& aSolver=aV(i);
      //
      aSolver.Perform();
    }
  }
  //
 protected:
  TypeSolverVector* myPV;
};
//=======================================================================
//class    : BOPCol_TBBCnt
//purpose  : 
//=======================================================================
template <class TypeFunctor, 
          class TypeSolverVector> class BOPCol_TBBCnt {
 public:
  //-------------------------------
  // Perform
  Standard_EXPORT 
    static void Perform(const Standard_Boolean bRunParallel,
                        TypeSolverVector& aV) {
    //
    TypeFunctor aFunctor(aV);
    Standard_Integer aNb=aV.Extent();
    //
    if (bRunParallel) {
#ifdef HAVE_TBB
      try {
        flexible_for(flexible_range<Standard_Integer>(0,aNb), aFunctor);
      }
      //
      catch( captured_exception&  ) {
        Standard_NotImplemented::Raise("");
      } 
      catch( ... ) {
        Standard_NotImplemented::Raise("");
      }
#else // not HAVE_TBB
      flexible_for(flexible_range<Standard_Integer>(0,aNb), aFunctor);
#endif      
    }
    else {
      aFunctor.operator()(flexible_range<Standard_Integer>(0,aNb));
    }
  }
};
//
// 2.2. Context dependent version
//

//=======================================================================
//class    : BOPCol_TBBContextFunctor
//purpose  : 
//=======================================================================
template <class TypeSolver, 
          class TypeSolverVector,
          class TypeContext, 
          typename TN> class BOPCol_TBBContextFunctor  {
 
 public:
  BOPCol_TBBContextFunctor(TypeSolverVector& aV) 
    : myPV(&aV) {
  }
  //
  ~BOPCol_TBBContextFunctor() {
  }
  //
  void SetContext(TypeContext& aCtx) {
    myContext=aCtx;
  }
  //
  void operator()( const flexible_range<Standard_Integer>& aBR ) const{
    Standard_Integer i, iBeg, iEnd;
    TypeContext aCtx;
    //
    if (myContext.IsNull()) {
      aCtx=new TN
        (NCollection_BaseAllocator::CommonBaseAllocator());
    }
    else {
      aCtx=myContext;
    }
    //
    TypeSolverVector& aV=*myPV;
    //
    iBeg=aBR.begin();
    iEnd=aBR.end();
    for(i=iBeg; i!=iEnd; ++i) {
      TypeSolver& aSolver=aV(i);
      //
      aSolver.SetContext(aCtx);
      aSolver.Perform();
    }
  }
  //
 protected:
  TypeSolverVector* myPV;
  TypeContext myContext;
  //
};

//=======================================================================
//class    : BOPCol_TBBContextCnt
//purpose  : 
//=======================================================================
template <class TypeFunctor, 
          class TypeSolverVector,
          class TypeContext> class BOPCol_TBBContextCnt {
 public:
  //-------------------------------
  // Perform
  Standard_EXPORT 
    static void Perform(const Standard_Boolean bRunParallel,
                        TypeSolverVector& aV,
                        TypeContext& aCtx) {
    //
    TypeFunctor aFunctor(aV);
    Standard_Integer aNb=aV.Extent();
    //
    if (bRunParallel) {
#ifdef HAVE_TBB
      try {
        flexible_for(flexible_range<Standard_Integer>(0,aNb), aFunctor);
      }
      //
      catch(captured_exception& ) {
        //cout<<" captured_exception: " << ex.what() << endl;
        Standard_NotImplemented::Raise("");
      } 
      catch( ... ) {
        Standard_NotImplemented::Raise("");
      }
#else // not HAVE_TBB
      flexible_for(flexible_range<Standard_Integer>(0,aNb), aFunctor);
#endif      
    }
    else {
      aFunctor.SetContext(aCtx);
      aFunctor.operator()(flexible_range<Standard_Integer>(0,aNb));
    }
  }
};

#endif
