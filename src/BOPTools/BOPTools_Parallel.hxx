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

#ifndef _BOPTools_Parallel_HeaderFile
#define _BOPTools_Parallel_HeaderFile

#include <OSD_Parallel.hxx>
#include <OSD_ThreadPool.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_Mutex.hxx>
#include <OSD_Thread.hxx>

//! Implementation of Functors/Starters
class BOPTools_Parallel
{
  template<class TypeSolverVector>
  class Functor
  {
  public:
    //! Constructor.
    explicit Functor(TypeSolverVector& theSolverVec) : mySolvers (theSolverVec) {}

    //! Defines functor interface.
    void operator() (const Standard_Integer theIndex) const
    {
      typename TypeSolverVector::value_type& aSolver = mySolvers[theIndex];
      aSolver.Perform();
    }

  private:
    Functor(const Functor&);
    Functor& operator= (const Functor&);

  private:
    TypeSolverVector& mySolvers;
  };

  //! Functor storing map of thread id -> algorithm context
  template<class TypeSolverVector, class TypeContext>
  class ContextFunctor
  {
  public:

    //! Constructor
    explicit ContextFunctor (TypeSolverVector& theVector) : mySolverVector(theVector) {}

    //! Binds main thread context
    void SetContext (const opencascade::handle<TypeContext>& theContext)
    {
      myContextMap.Bind (OSD_Thread::Current(), theContext);
    }

    //! Returns current thread context
    const opencascade::handle<TypeContext>& GetThreadContext() const
    {
      const Standard_ThreadId aThreadID = OSD_Thread::Current();
      if (const opencascade::handle<TypeContext>* aContextPtr = myContextMap.Seek (aThreadID))
      {
        if (!aContextPtr->IsNull())
        {
          return *aContextPtr;
        }
      }

      // Create new context
      opencascade::handle<TypeContext> aContext = new TypeContext (NCollection_BaseAllocator::CommonBaseAllocator());

      Standard_Mutex::Sentry aLocker (myMutex);
      myContextMap.Bind (aThreadID, aContext);
      return myContextMap (aThreadID);
    }

    //! Defines functor interface
    void operator()( const Standard_Integer theIndex ) const
    {
      const opencascade::handle<TypeContext>& aContext = GetThreadContext();
      typename TypeSolverVector::value_type& aSolver = mySolverVector[theIndex];

      aSolver.SetContext(aContext);
      aSolver.Perform();
    }

  private:
    ContextFunctor(const ContextFunctor&);
    ContextFunctor& operator= (const ContextFunctor&);

  private:
    TypeSolverVector& mySolverVector;
    mutable NCollection_DataMap<Standard_ThreadId, opencascade::handle<TypeContext>> myContextMap;
    mutable Standard_Mutex myMutex;
  };

  //! Functor storing array of algorithm contexts per thread in pool
  template<class TypeSolverVector, class TypeContext>
  class ContextFunctor2
  {
  public:

    //! Constructor
    explicit ContextFunctor2 (TypeSolverVector& theVector, const OSD_ThreadPool::Launcher& thePoolLauncher)
    : mySolverVector(theVector),
      myContextArray (thePoolLauncher.LowerThreadIndex(), thePoolLauncher.UpperThreadIndex()) {}

    //! Binds main thread context
    void SetContext (const opencascade::handle<TypeContext>& theContext)
    {
      myContextArray.ChangeLast() = theContext; // OSD_ThreadPool::Launcher::UpperThreadIndex() is reserved for a main thread
    }

    //! Defines functor interface with serialized thread index.
    void operator() (int theThreadIndex,
                     int theIndex) const
    {
      opencascade::handle<TypeContext>& aContext = myContextArray.ChangeValue (theThreadIndex);
      if (aContext.IsNull())
      {
        aContext = new TypeContext (NCollection_BaseAllocator::CommonBaseAllocator());
      }
      typename TypeSolverVector::value_type& aSolver = mySolverVector[theIndex];
      aSolver.SetContext (aContext);
      aSolver.Perform();
    }

  private:
    ContextFunctor2(const ContextFunctor2&);
    ContextFunctor2& operator= (const ContextFunctor2&);

  private:
    TypeSolverVector& mySolverVector;
    mutable NCollection_Array1< opencascade::handle<TypeContext> > myContextArray;
  };

public:

  //! Pure version
  template<class TypeSolverVector>
  static void Perform (Standard_Boolean theIsRunParallel,
                       TypeSolverVector& theSolverVector)
  {
    Functor<TypeSolverVector> aFunctor (theSolverVector);
    OSD_Parallel::For (0, theSolverVector.Length(), aFunctor, !theIsRunParallel);
  }

  //! Context dependent version
  template<class TypeSolverVector, class TypeContext>
  static void Perform (Standard_Boolean  theIsRunParallel,
                       TypeSolverVector& theSolverVector,
                       opencascade::handle<TypeContext>& theContext)
  {
    if (OSD_Parallel::ToUseOcctThreads())
    {
      const Handle(OSD_ThreadPool)& aThreadPool = OSD_ThreadPool::DefaultPool();
      OSD_ThreadPool::Launcher aPoolLauncher (*aThreadPool, theIsRunParallel ? theSolverVector.Length() : 0);
      ContextFunctor2<TypeSolverVector, TypeContext> aFunctor (theSolverVector, aPoolLauncher);
      aFunctor.SetContext (theContext);
      aPoolLauncher.Perform (0, theSolverVector.Length(), aFunctor);
    }
    else
    {
      ContextFunctor<TypeSolverVector, TypeContext> aFunctor (theSolverVector);
      aFunctor.SetContext (theContext);
      OSD_Parallel::For (0, theSolverVector.Length(), aFunctor, !theIsRunParallel);
    }
  }
};

#endif
