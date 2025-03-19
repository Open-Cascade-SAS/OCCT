// Created on: 2014-08-19
// Created by: Alexander Zaikin
// Copyright (c) 1996-1999 Matra Datavision
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#include <OSD_Parallel.hxx>

#include <OSD_ThreadPool.hxx>

#include <NCollection_Array1.hxx>
#include <Standard_Mutex.hxx>
#include <OSD_Thread.hxx>

namespace
{
//! Class implementing tools for parallel processing
//! using threads (when TBB is not available);
//! it is derived from OSD_Parallel to get access to
//! Iterator and FunctorInterface nested types.
class OSD_Parallel_Threads : public OSD_ThreadPool, public OSD_Parallel
{
public:
  //! Auxiliary class which ensures exclusive
  //! access to iterators of processed data pool.
  class Range
  {
  public: //! @name public methods
    //! Constructor
    Range(const OSD_Parallel::UniversalIterator& theBegin,
          const OSD_Parallel::UniversalIterator& theEnd)
        : myBegin(theBegin),
          myEnd(theEnd),
          myIt(theBegin)
    {
    }

    //! Returns const link on the first element.
    inline const OSD_Parallel::UniversalIterator& Begin() const { return myBegin; }

    //! Returns const link on the last element.
    inline const OSD_Parallel::UniversalIterator& End() const { return myEnd; }

    //! Returns first non processed element or end.
    //! Thread-safe method.
    inline OSD_Parallel::UniversalIterator It() const
    {
      Standard_Mutex::Sentry aMutex(myMutex);
      return (myIt != myEnd) ? myIt++ : myEnd;
    }

  private: //! @name private methods
    //! Empty copy constructor
    Range(const Range& theCopy);

    //! Empty copy operator.
    Range& operator=(const Range& theCopy);

  private:                                          //! @name private fields
    const OSD_Parallel::UniversalIterator& myBegin; //!< First element of range.
    const OSD_Parallel::UniversalIterator& myEnd;   //!< Last element of range.
                                                    // clang-format off
      mutable OSD_Parallel::UniversalIterator   myIt;    //!< First non processed element of range.
      mutable Standard_Mutex                 myMutex; //!< Access controller for the first non processed element.
                                                    // clang-format on
  };

  //! Auxiliary wrapper class for thread function.
  class Task : public JobInterface
  {
  public: //! @name public methods
    //! Constructor.
    Task(const OSD_Parallel::FunctorInterface& thePerformer, Range& theRange)
        : myPerformer(thePerformer),
          myRange(theRange)
    {
    }

    //! Method is executed in the context of thread,
    //! so this method defines the main calculations.
    virtual void Perform(int) Standard_OVERRIDE
    {
      for (OSD_Parallel::UniversalIterator anIter = myRange.It(); anIter != myRange.End();
           anIter                                 = myRange.It())
      {
        myPerformer(*anIter);
      }
    }

  private: //! @name private methods
    //! Empty copy constructor.
    Task(const Task& theCopy);

    //! Empty copy operator.
    Task& operator=(const Task& theCopy);

  private:                               //! @name private fields
    const FunctorInterface& myPerformer; //!< Link on functor
    const Range&            myRange;     //!< Link on processed data block
  };

  //! Launcher specialization.
  class UniversalLauncher : public Launcher
  {
  public:
    //! Constructor.
    UniversalLauncher(OSD_ThreadPool& thePool, int theMaxThreads = -1)
        : Launcher(thePool, theMaxThreads)
    {
    }

    //! Primitive for parallelization of "for" loops.
    void Perform(OSD_Parallel::UniversalIterator&      theBegin,
                 OSD_Parallel::UniversalIterator&      theEnd,
                 const OSD_Parallel::FunctorInterface& theFunctor)
    {
      Range aData(theBegin, theEnd);
      Task  aJob(theFunctor, aData);
      perform(aJob);
    }
  };
};
} // namespace

//=================================================================================================

void OSD_Parallel::forEachOcct(UniversalIterator&      theBegin,
                               UniversalIterator&      theEnd,
                               const FunctorInterface& theFunctor,
                               Standard_Integer        theNbItems)
{
  const Handle(OSD_ThreadPool)& aThreadPool = OSD_ThreadPool::DefaultPool();
  const Standard_Integer        aNbThreads =
    theNbItems != -1 ? Min(theNbItems, aThreadPool->NbDefaultThreadsToLaunch()) : -1;
  OSD_Parallel_Threads::UniversalLauncher aLauncher(*aThreadPool, aNbThreads);
  aLauncher.Perform(theBegin, theEnd, theFunctor);
}

// Version of parallel executor used when TBB is not available
#ifndef HAVE_TBB
//=================================================================================================

void OSD_Parallel::forEachExternal(UniversalIterator&      theBegin,
                                   UniversalIterator&      theEnd,
                                   const FunctorInterface& theFunctor,
                                   Standard_Integer        theNbItems)
{
  forEachOcct(theBegin, theEnd, theFunctor, theNbItems);
}

#endif /* ! HAVE_TBB */
