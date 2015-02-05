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

#ifndef OSD_Parallel_HeaderFile
#define OSD_Parallel_HeaderFile

#include <OSD_Thread.hxx>
#include <Standard_Mutex.hxx>
#include <Standard_NotImplemented.hxx>
#include <Standard_Atomic.hxx>
#include <NCollection_Array1.hxx>

#ifdef HAVE_TBB
#include <tbb/parallel_for.h>
#include <tbb/parallel_for_each.h>
#include <tbb/blocked_range.h>
#endif

//! @class OSD_Parallel
//! @brief Simplifies code parallelization.
//!
//! The Class provides an interface of parallel processing "for" and "foreach" loops.
//! These primitives encapsulates complete logic for creating and managing parallel context of loops.
//! Moreover the primitives may be a wrapper for some primitives from 3rd-party library - TBB.
//! To use it is necessary to implement TBB like interface which is based on functors.
//!
//! @code
//! class Functor
//! {
//! public:
//!   void operator() ([proccesing instance]) const
//!   {
//!     //...
//!   }
//! };
//! @endcode
//!
//! In the body of the operator () should be implemented thread-safe logic of computations that can be performed in parallel context.
//! If parallelized loop iterates on the collections with direct access by index (such as Vector, Array),
//! it is more efficient to use the primitive ParallelFor (because it has no critical section).
class OSD_Parallel
{
  //! Auxiliary class which ensures exclusive
  //! access to iterators of processed data pool.
  template <typename Value>
  class Range
  {
  public: //! @name public methods

    typedef Value Iterator;

    //! Constructor
    Range(const Value& theBegin, const Value& theEnd)
    : myBegin(theBegin),
      myEnd  (theEnd),
      myIt   (theBegin)
    {
    }

    //! Returns const link on the first element.
    inline const Value& Begin() const
    {
      return myBegin;
    }

    //! Returns const link on the last element.
    inline const Value& End() const
    {
      return myEnd;
    }

    //! Returns first non processed element or end.
    //! Thread-safe method.
    inline Iterator It() const
    {
      Standard_Mutex::Sentry aMutex( myMutex );
      return ( myIt != myEnd ) ? myIt++ : myEnd;
    }

  private: //! @name private methods

    //! Empty copy constructor
    Range(const Range& theCopy);

    //! Empty copy operator.
    Range& operator=(const Range& theCopy);

  private: //! @name private fields

    const Value&           myBegin; //!< Fisrt element of range.
    const Value&           myEnd;   //!< Last element of range.
    mutable Value          myIt;    //!< First non processed element of range.
    mutable Standard_Mutex myMutex; //!< Access controller for the first non processed element.
  };

  //! Auxiliary wrapper class for thread function.
  template <typename Functor, typename InputIterator>
  class Task
  {
  public: //! @name public methods

    //! Constructor.
    Task(const Functor& thePerformer, Range<InputIterator>& theRange)
    : myPerformer(thePerformer),
      myRange    (theRange)
    {
    }

    //! Method is executed in the context of thread,
    //! so this method defines the main calculations.
    static Standard_Address RunWithIterator(Standard_Address theTask)
    {
      Task<Functor, InputIterator>& aTask =
        *( static_cast< Task<Functor, InputIterator>* >(theTask) );

      const Range<InputIterator>& aData( aTask.myRange );
      typename Range<InputIterator>::Iterator i = aData.It();

      for ( ; i != aData.End(); i = aData.It() )
      {
        aTask.myPerformer(*i);
      }

      return NULL;
    }

    //! Method is executed in the context of thread,
    //! so this method defines the main calculations.
    static Standard_Address RunWithIndex(Standard_Address theTask)
    {
      Task<Functor, InputIterator>& aTask =
        *( static_cast< Task<Functor, Standard_Integer>* >(theTask) );

      const Range<Standard_Integer>& aData( aTask.myRange );
      Standard_Integer i = aData.It();

      for ( ; i < aData.End(); i = aData.It())
      {
        aTask.myPerformer(i);
      }

      return NULL;
    }

  private: //! @name private methods

    //! Empty copy constructor.
    Task(const Task& theCopy);

    //! Empty copy operator.
    Task& operator=(const Task& theCopy);

  private: //! @name private fields

    const Functor&              myPerformer; //!< Link on functor.
    const Range<InputIterator>& myRange;     //!< Link on processed data block.
  };

public: //! @name public methods

  //! Returns number of logical proccesrs.
  Standard_EXPORT static Standard_Integer NbLogicalProcessors();

  //! Simple primitive for parallelization of "foreach" loops.
  template <typename InputIterator, typename Functor>
  static void ForEach( InputIterator  theBegin,
                       InputIterator  theEnd,
                       const Functor& theFunctor,
                       const Standard_Boolean isForceSingleThreadExecution
                         = Standard_False );

  //! Simple primitive for parallelization of "for" loops.
  template <typename Functor>
  static void For( const Standard_Integer theBegin,
                   const Standard_Integer theEnd,
                   const Functor&         theFunctor,
                   const Standard_Boolean isForceSingleThreadExecution
                     = Standard_False );
};

//=======================================================================
//function : OSD_Parallel::Range::It
//purpose  : Template concretization.
//=======================================================================
template<> inline Standard_Integer OSD_Parallel::Range<Standard_Integer>::It() const
{
  return Standard_Atomic_Increment( reinterpret_cast<volatile int*>(&myIt) ) - 1;
}

//=======================================================================
//function : ParallelForEach
//purpose  : 
//=======================================================================
template <typename InputIterator, typename Functor>
void OSD_Parallel::ForEach( InputIterator          theBegin,
                            InputIterator          theEnd,
                            const Functor&         theFunctor,
                            const Standard_Boolean isForceSingleThreadExecution )
{
  if ( isForceSingleThreadExecution )
  {
    for ( InputIterator it(theBegin); it != theEnd; it++ )
      theFunctor(*it);

    return;
  }
  #ifdef HAVE_TBB
  {
    try
    {
      tbb::parallel_for_each(theBegin, theEnd, theFunctor);
    }
    catch ( tbb::captured_exception& anException )
    {
      Standard_NotImplemented::Raise(anException.what());
    }
  }
  #else
  {
    Range<InputIterator> aData(theBegin, theEnd);
    Task<Functor, InputIterator> aTask(theFunctor, aData);

    const Standard_Integer aNbThreads = OSD_Parallel::NbLogicalProcessors();
    NCollection_Array1<OSD_Thread> aThreads(0, aNbThreads - 1);

    for ( Standard_Integer i = 0; i < aNbThreads; ++i )
    {
      OSD_Thread& aThread = aThreads(i);
      aThread.SetFunction(&Task<Functor, InputIterator>::RunWithIterator);
      aThread.Run(&aTask);
    }

    for ( Standard_Integer i = 0; i < aNbThreads; ++i )
      aThreads(i).Wait();
  }
  #endif
}

//=======================================================================
//function : ParallelFor
//purpose  : 
//=======================================================================
template <typename Functor>
void OSD_Parallel::For( const Standard_Integer theBegin,
                        const Standard_Integer theEnd,
                        const Functor&         theFunctor,
                        const Standard_Boolean isForceSingleThreadExecution )
{
  if ( isForceSingleThreadExecution )
  {
    for ( Standard_Integer i = theBegin; i < theEnd; ++i )
      theFunctor(i);

    return;
  }
  #ifdef HAVE_TBB
  {
    try
    {
      tbb::parallel_for( theBegin, theEnd, theFunctor );
    }
    catch ( tbb::captured_exception& anException )
    {
      Standard_NotImplemented::Raise(anException.what());
    }
  }
  #else
  {
    Range<Standard_Integer> aData(theBegin, theEnd);
    Task<Functor, Standard_Integer> aTask(theFunctor, aData);

    const Standard_Integer aNbThreads = OSD_Parallel::NbLogicalProcessors();
    NCollection_Array1<OSD_Thread> aThreads(0, aNbThreads - 1);

    for ( Standard_Integer i = 0; i < aNbThreads; ++i )
    {
      OSD_Thread& aThread = aThreads(i);
      aThread.SetFunction(&Task<Functor, Standard_Integer>::RunWithIndex);
      aThread.Run(&aTask);
    }

    for ( Standard_Integer i = 0; i < aNbThreads; ++i )
      aThreads(i).Wait();
  }
  #endif
}

#endif
