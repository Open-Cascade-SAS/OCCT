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

#include <OSD_ThreadPool.hxx>
#include <Standard_Type.hxx>
#include <memory>

//! @brief Simple tool for code parallelization.
//!
//! OSD_Parallel class provides simple interface for parallel processing of
//! tasks that can be formulated in terms of "for" or "foreach" loops.
//!
//! To use this tool it is necessary to:
//! - organize the data to be processed in a collection accessible by
//!   iteration (usually array or vector);
//! - implement a functor class providing operator () accepting iterator
//!   (or index in array) that does the job;
//! - call either For() or ForEach() providing begin and end iterators and
//!   a functor object.
//!
//! Iterators should satisfy requirements of STL forward iterator.
//! Functor
//!
//! @code
//! class Functor
//! {
//! public:
//!   void operator() ([processing instance]) const
//!   {
//!     //...
//!   }
//! };
//! @endcode
//!
//! The operator () should be implemented in a thread-safe way so that
//! the same functor object can process different data items in parallel threads.
//!
//! Iteration by index (For) is expected to be more efficient than using iterators
//! (ForEach).
//!
//! Implementation uses TBB if OCCT is built with support of TBB; otherwise it
//! uses ad-hoc parallelization tool. In general, if TBB is available, it is
//! more efficient to use it directly instead of using OSD_Parallel.

class OSD_Parallel
{
private:
  //! Interface class defining API for polymorphic wrappers over iterators.
  //! Intended to add polymorphic behaviour to For and ForEach functionality
  //! for arbitrary objects and eliminate dependency on template parameters.
  class IteratorInterface
  {
  public:
    virtual ~IteratorInterface() {}

    //! Returns true if iterators wrapped by this and theOther are equal
    virtual bool IsEqual(const IteratorInterface& theOther) const = 0;

    //! Increments wrapped iterator
    virtual void Increment() = 0;

    //! Returns new instance of the wrapper containing copy
    //! of the wrapped iterator.
    virtual IteratorInterface* Clone() const = 0;
  };

  //! Implementation of polymorphic iterator wrapper suitable for basic
  //! types as well as for std iterators.
  //! Wraps instance of actual iterator type Type.
  template <class Type>
  class IteratorWrapper : public IteratorInterface
  {
  public:
    IteratorWrapper() {}

    IteratorWrapper(const Type& theValue)
        : myValue(theValue)
    {
    }

    virtual bool IsEqual(const IteratorInterface& theOther) const Standard_OVERRIDE
    {
      return myValue == dynamic_cast<const IteratorWrapper<Type>&>(theOther).myValue;
    }

    virtual void Increment() Standard_OVERRIDE { ++myValue; }

    virtual IteratorInterface* Clone() const Standard_OVERRIDE
    {
      return new IteratorWrapper<Type>(myValue);
    }

    const Type& Value() const { return myValue; }

  private:
    Type myValue;
  };

protected:
  // Note: UniversalIterator and FunctorInterface are made protected to be
  // accessible from specialization using threads (non-TBB).

  //! Fixed-type iterator, implementing STL forward iterator interface, used for
  //! iteration over objects subject to parallel processing.
  //! It stores pointer to instance of polymorphic iterator inheriting from
  //! IteratorInterface, which contains actual type-specific iterator.
  class UniversalIterator
  // Note that TBB requires that value_type of iterator be copyable,
  // thus we use its own type for that
  {
  public:
    // Since C++20 inheritance from std::iterator is deprecated, so define predefined types
    // manually:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = IteratorInterface*;
    using difference_type   = ptrdiff_t;
    using pointer           = value_type;
    using reference         = value_type;

    UniversalIterator() {}

    UniversalIterator(IteratorInterface* theOther)
        : myPtr(theOther)
    {
    }

    UniversalIterator(const UniversalIterator& theOther)
        : myPtr(theOther.myPtr->Clone())
    {
    }

    UniversalIterator& operator=(const UniversalIterator& theOther)
    {
      myPtr.reset(theOther.myPtr->Clone());
      return *this;
    }

    bool operator!=(const UniversalIterator& theOther) const
    {
      return !myPtr->IsEqual(*theOther.myPtr);
    }

    bool operator==(const UniversalIterator& theOther) const
    {
      return myPtr->IsEqual(*theOther.myPtr);
    }

    UniversalIterator& operator++()
    {
      myPtr->Increment();
      return *this;
    }

    UniversalIterator operator++(int)
    {
      UniversalIterator aValue(*this);
      myPtr->Increment();
      return aValue;
    }

    reference operator*() const { return myPtr.get(); }

    reference operator*() { return myPtr.get(); }

  private:
    std::unique_ptr<IteratorInterface> myPtr;
  };

  //! Interface class representing functor object.
  //! Intended to add polymorphic behaviour to For and ForEach functionality
  //! enabling execution of arbitrary function in parallel mode.
  class FunctorInterface
  {
  public:
    virtual ~FunctorInterface() {}

    virtual void operator()(IteratorInterface* theIterator) const = 0;

    // type cast to actual iterator
    template <typename Iterator>
    static const Iterator& DownCast(IteratorInterface* theIterator)
    {
      return dynamic_cast<OSD_Parallel::IteratorWrapper<Iterator>*>(theIterator)->Value();
    }
  };

private:
  //! Wrapper for functors manipulating on std iterators.
  template <class Iterator, class Functor>
  class FunctorWrapperIter : public FunctorInterface
  {
  public:
    FunctorWrapperIter(const Functor& theFunctor)
        : myFunctor(theFunctor)
    {
    }

    virtual void operator()(IteratorInterface* theIterator) const Standard_OVERRIDE
    {
      const Iterator& anIt = DownCast<Iterator>(theIterator);
      myFunctor(*anIt);
    }

  private:
    FunctorWrapperIter(const FunctorWrapperIter&);
    void           operator=(const FunctorWrapperIter&);
    const Functor& myFunctor;
  };

  //! Wrapper for functors manipulating on integer index.
  template <class Functor>
  class FunctorWrapperInt : public FunctorInterface
  {
  public:
    FunctorWrapperInt(const Functor& theFunctor)
        : myFunctor(theFunctor)
    {
    }

    virtual void operator()(IteratorInterface* theIterator) const Standard_OVERRIDE
    {
      Standard_Integer anIndex = DownCast<Standard_Integer>(theIterator);
      myFunctor(anIndex);
    }

  private:
    FunctorWrapperInt(const FunctorWrapperInt&);
    void           operator=(const FunctorWrapperInt&);
    const Functor& myFunctor;
  };

  //! Wrapper redirecting functor taking element index to functor taking also thread index.
  template <class Functor>
  class FunctorWrapperForThreadPool
  {
  public:
    FunctorWrapperForThreadPool(const Functor& theFunctor)
        : myFunctor(theFunctor)
    {
    }

    void operator()(int theThreadIndex, int theElemIndex) const
    {
      (void)theThreadIndex;
      myFunctor(theElemIndex);
    }

  private:
    FunctorWrapperForThreadPool(const FunctorWrapperForThreadPool&);
    void           operator=(const FunctorWrapperForThreadPool&);
    const Functor& myFunctor;
  };

private:
  //! Simple primitive for parallelization of "foreach" loops, e.g.:
  //! @code
  //!   for (std::iterator anIter = theBegin; anIter != theEnd; ++anIter) {}
  //! @endcode
  //! Implementation of framework-dependent functionality should be provided by
  //! forEach_impl function defined in opencascade::parallel namespace.
  //! @param theBegin   the first index (inclusive)
  //! @param theEnd     the last  index (exclusive)
  //! @param theFunctor functor providing an interface "void operator(InputIterator theIter){}"
  //!                   performing task for the specified iterator position
  //! @param theNbItems number of items passed by iterator, -1 if unknown
  Standard_EXPORT static void forEachOcct(UniversalIterator&      theBegin,
                                          UniversalIterator&      theEnd,
                                          const FunctorInterface& theFunctor,
                                          Standard_Integer        theNbItems);

  //! Same as forEachOcct() but can be implemented using external threads library.
  Standard_EXPORT static void forEachExternal(UniversalIterator&      theBegin,
                                              UniversalIterator&      theEnd,
                                              const FunctorInterface& theFunctor,
                                              Standard_Integer        theNbItems);

public: //! @name public methods
  //! Returns TRUE if OCCT threads should be used instead of auxiliary threads library;
  //! default value is FALSE if alternative library has been enabled while OCCT building and TRUE
  //! otherwise.
  Standard_EXPORT static Standard_Boolean ToUseOcctThreads();

  //! Sets if OCCT threads should be used instead of auxiliary threads library.
  //! Has no effect if OCCT has been built with no auxiliary threads library.
  Standard_EXPORT static void SetUseOcctThreads(Standard_Boolean theToUseOcct);

  //! Returns number of logical processors.
  Standard_EXPORT static Standard_Integer NbLogicalProcessors();

  //! Simple primitive for parallelization of "foreach" loops, equivalent to:
  //! @code
  //!   for (auto anIter = theBegin; anIter != theEnd; ++anIter) {
  //!     theFunctor(*anIter);
  //!   }
  //! @endcode
  //! @param theBegin   the first index (inclusive)
  //! @param theEnd     the last  index (exclusive)
  //! @param theFunctor functor providing an interface "void operator(InputIterator theIter){}"
  //!                   performing task for specified iterator position
  //! @param isForceSingleThreadExecution if true, then no threads will be created
  //! @param theNbItems number of items passed by iterator, -1 if unknown
  template <typename InputIterator, typename Functor>
  static void ForEach(InputIterator          theBegin,
                      InputIterator          theEnd,
                      const Functor&         theFunctor,
                      const Standard_Boolean isForceSingleThreadExecution = Standard_False,
                      Standard_Integer       theNbItems                   = -1)
  {
    if (isForceSingleThreadExecution || theNbItems == 1)
    {
      for (InputIterator it(theBegin); it != theEnd; ++it)
        theFunctor(*it);
    }
    else
    {
      UniversalIterator aBegin(new IteratorWrapper<InputIterator>(theBegin));
      UniversalIterator aEnd(new IteratorWrapper<InputIterator>(theEnd));
      FunctorWrapperIter<InputIterator, Functor> aFunctor(theFunctor);
      if (ToUseOcctThreads())
      {
        forEachOcct(aBegin, aEnd, aFunctor, theNbItems);
      }
      else
      {
        forEachExternal(aBegin, aEnd, aFunctor, theNbItems);
      }
    }
  }

  //! Simple primitive for parallelization of "for" loops, equivalent to:
  //! @code
  //!   for (int anIter = theBegin; anIter != theEnd; ++anIter) {
  //!     theFunctor(anIter);
  //!   }
  //! @endcode
  //! @param theBegin   the first index (inclusive)
  //! @param theEnd     the last  index (exclusive)
  //! @param theFunctor functor providing an interface "void operator(int theIndex){}"
  //!                   performing task for specified index
  //! @param isForceSingleThreadExecution if true, then no threads will be created
  template <typename Functor>
  static void For(const Standard_Integer theBegin,
                  const Standard_Integer theEnd,
                  const Functor&         theFunctor,
                  const Standard_Boolean isForceSingleThreadExecution = Standard_False)
  {
    const Standard_Integer aRange = theEnd - theBegin;
    if (isForceSingleThreadExecution || aRange == 1)
    {
      for (Standard_Integer it(theBegin); it != theEnd; ++it)
        theFunctor(it);
    }
    else if (ToUseOcctThreads())
    {
      const Handle(OSD_ThreadPool)&        aThreadPool = OSD_ThreadPool::DefaultPool();
      OSD_ThreadPool::Launcher             aPoolLauncher(*aThreadPool, aRange);
      FunctorWrapperForThreadPool<Functor> aFunctor(theFunctor);
      aPoolLauncher.Perform(theBegin, theEnd, aFunctor);
    }
    else
    {
      UniversalIterator          aBegin(new IteratorWrapper<Standard_Integer>(theBegin));
      UniversalIterator          aEnd(new IteratorWrapper<Standard_Integer>(theEnd));
      FunctorWrapperInt<Functor> aFunctor(theFunctor);
      forEachExternal(aBegin, aEnd, aFunctor, aRange);
    }
  }
};

#endif
