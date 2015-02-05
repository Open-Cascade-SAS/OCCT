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
#include <OSD_Parallel.hxx>
#include <NCollection_DataMap.hxx>

//
// 1. Implementation of Functors/Starters
//
// 1.1. Pure version
//

//=======================================================================
//class    : BOPCol_Functor
//purpose  : 
//=======================================================================
template <class TypeSolver, class TypeSolverVector>
class BOPCol_Functor
{
public:
  //! Constructor.
  explicit BOPCol_Functor(TypeSolverVector& theSolverVec) 
  : mySolvers(theSolverVec) {}

  //! Defines functor interface.
  void operator() (const Standard_Integer theIndex) const
  {
    TypeSolver& aSolver = mySolvers(theIndex);
    aSolver.Perform();
  }

private:
  BOPCol_Functor(const BOPCol_Functor&);
  BOPCol_Functor& operator= (const BOPCol_Functor&);

private:
  TypeSolverVector& mySolvers;
};

//=======================================================================
//class    : BOPCol_Cnt
//purpose  : 
//=======================================================================
template <class TypeFunctor, class TypeSolverVector>
class BOPCol_Cnt
{
public:
  static void Perform( const Standard_Boolean isRunParallel,
                       TypeSolverVector&      theSolverVector )
  {
    TypeFunctor aFunctor(theSolverVector);
    OSD_Parallel::For(0, theSolverVector.Extent(), aFunctor, !isRunParallel);
  }
};

//
// 1.2. Context dependent version
//

//=======================================================================
//class    : BOPCol_ContextFunctor
//purpose  : 
//=======================================================================
template <class TypeSolver,  class TypeSolverVector,
          class TypeContext, typename TN>
class BOPCol_ContextFunctor
{
  //! Auxiliary thread ID  hasher.
  struct Hasher
  {
    static Standard_Integer HashCode(const Standard_ThreadId theKey,
                                     const Standard_Integer  Upper)
    {
      return ::HashCode(reinterpret_cast<Standard_Address>(theKey), Upper);
    }

    static Standard_Boolean IsEqual(const Standard_ThreadId theKey1,
                                    const Standard_ThreadId theKey2)
    {
      return theKey1 == theKey2;
    }
  };

  typedef NCollection_DataMap<Standard_ThreadId, TypeContext, Hasher> ContextMap;

public:

  //! Constructor
  explicit BOPCol_ContextFunctor( TypeSolverVector& theVector )
  : mySolverVector(theVector) {}

  //! Binds main thread context
  void SetContext( TypeContext& theContext )
  {
    myContexts.Bind(OSD_Thread::Current(), theContext);
  }

  //! Returns current thread context
  TypeContext& GetThreadContext() const
  {
    const Standard_ThreadId aThreadID = OSD_Thread::Current();
    if ( myContexts.IsBound(aThreadID) )
    {
      TypeContext& aContext = myContexts(aThreadID);
      if ( aContext.IsNull() == Standard_False )
        return aContext;
    }

    // Create new context
    TypeContext aContext = new TN
      ( NCollection_BaseAllocator::CommonBaseAllocator() );

    Standard_Mutex::Sentry aLocker(myMutex);
    myContexts.Bind(aThreadID, aContext);

    return myContexts(aThreadID);
  }

  //! Defines functor interface
  void operator()( const Standard_Integer theIndex ) const
  {
    TypeContext& aContext = GetThreadContext();
    TypeSolver&  aSolver  = mySolverVector(theIndex);

    aSolver.SetContext(aContext);
    aSolver.Perform();
  }

private:
  BOPCol_ContextFunctor(const BOPCol_ContextFunctor&);
  BOPCol_ContextFunctor& operator= (const BOPCol_ContextFunctor&);

private:
  TypeSolverVector&      mySolverVector;
  mutable ContextMap     myContexts;
  mutable Standard_Mutex myMutex;
};

//=======================================================================
//class    : BOPCol_ContextCnt
//purpose  : 
//=======================================================================
template <class TypeFunctor, class TypeSolverVector, class TypeContext>
class BOPCol_ContextCnt
{
public:
  static void Perform( const Standard_Boolean isRunParallel,
                       TypeSolverVector&      theSolverVector,
                       TypeContext&           theContext )
  {
    TypeFunctor aFunctor(theSolverVector);
    aFunctor.SetContext(theContext);

    OSD_Parallel::For(0, theSolverVector.Extent(), aFunctor, !isRunParallel);
  }
};

#endif
