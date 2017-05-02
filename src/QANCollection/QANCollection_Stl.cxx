// Created on: 2014-04-16
// Created by: Denis BOGOLEPOV
// Copyright (c) 2014 OPEN CASCADE SAS
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

#if defined(_MSC_VER) && ! defined(_SCL_SECURE_NO_WARNINGS)
  // suppress "std::Equal1" warning suggesting using msvc "Checked Iterators"
  #define _SCL_SECURE_NO_WARNINGS
#endif

#include <QANCollection.hxx>
#include <Draw_Interpretor.hxx>

#include <NCollection_Array1.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_Vector.hxx>
#include <NCollection_Map.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <Standard_Assert.hxx>
#include <OSD_Timer.hxx>
#include <OSD_Parallel.hxx>

#include <algorithm>
#include <list>
#include <set>
#include <typeinfo>
#include <vector>

//! Size of test data sets.
const int THE_TEST_SIZE = 5000;

namespace {
  // Auxiliary class to use in std::random_shuffle()
  struct RandomGenerator {
    RandomGenerator () { srand(1); }
    ptrdiff_t operator () (ptrdiff_t upper) const { return rand() % upper; }
  };
}

template<class CollectionType, class StlType>
struct CollectionFiller
{
  static void Perform (CollectionType** theCollec, Standard_Integer theSize = THE_TEST_SIZE)
  {
    *theCollec = new CollectionType();
    srand(1);
    for (Standard_Integer anIdx = 0; anIdx < theSize; ++anIdx)
    {
      (*theCollec)->Append (rand());
    }
  }

  static void Perform (StlType** theVector,
    CollectionType** theCollec, Standard_Integer theSize = THE_TEST_SIZE)
  {
    CollectionFiller::Perform (theCollec, theSize);

    *theVector = new StlType ((*theCollec)->begin(), (*theCollec)->end());
  }
};

template<class T, typename StlType>
struct CollectionFiller<NCollection_Array1<T>, StlType>
{
  static void Perform (NCollection_Array1<T>** theCollec,
    Standard_Integer theSize = THE_TEST_SIZE)
  {
    *theCollec = new NCollection_Array1<T> (0, theSize - 1);
    srand (1);
    for (Standard_Integer anIdx = 0; anIdx < theSize; ++anIdx)
    {
      (*theCollec)->ChangeValue (anIdx) = rand();
    }
  }

  static void Perform (StlType** theVector,
    NCollection_Array1<T>** theCollec, Standard_Integer theSize = THE_TEST_SIZE)
  {
    CollectionFiller<NCollection_Array1<T>, StlType >::Perform (theCollec, theSize);

    *theVector = new StlType ((*theCollec)->begin(), (*theCollec)->end());
  }
};

template<class CollectionType, class T>
struct MapFiller
{
  static void Perform (CollectionType** theCollec, Standard_Integer theSize = THE_TEST_SIZE)
  {
    *theCollec = new CollectionType();
    srand(1);
    for (Standard_Integer anIdx = 0; anIdx < theSize; ++anIdx)
    {
      (*theCollec)->Add (rand());
    }
  }
};

template<class T>
struct MapFiller<NCollection_DataMap<T, T>, T>
{
  static void Perform (NCollection_DataMap<T, T>** theCollec1,
    NCollection_DataMap<T, T>** theCollec2 = NULL, Standard_Integer theSize = THE_TEST_SIZE)
  {
    *theCollec1 = new NCollection_DataMap<T, T>();

    if (theCollec2 != NULL)
      *theCollec2 = new NCollection_DataMap<T, T>();
    srand(1);
    for (Standard_Integer anIdx = 0; anIdx < theSize; ++anIdx)
    {
      const T aVal1 = rand();
      const T aVal2 = rand();

      (*theCollec1)->Bind (aVal1, aVal2);

      if (theCollec2 != NULL)
        (*theCollec2)->Bind (aVal1, aVal2);
    }
  }
};

template<class T>
struct MapFiller<NCollection_IndexedDataMap<T, T>, T>
{
  static void Perform (NCollection_IndexedDataMap<T, T>** theCollec1,
    NCollection_IndexedDataMap<T, T>** theCollec2 = NULL, Standard_Integer theSize = THE_TEST_SIZE)
  {
    *theCollec1 = new NCollection_IndexedDataMap<T, T>();

    if (theCollec2 != NULL)
      *theCollec2 = new NCollection_IndexedDataMap<T, T>();
    srand(1);
    for (Standard_Integer anIdx = 0; anIdx < theSize; ++anIdx)
    {
      const T aVal1 = rand();
      const T aVal2 = rand();

      (*theCollec1)->Add (aVal1, aVal2);

      if (theCollec2 != NULL)
        (*theCollec2)->Add (aVal1, aVal2);
    }
  }
};

//=======================================================================
//function : TestIteration
//purpose  :
//=======================================================================
template<class CollectionType, class StlType>
Standard_Boolean TestIteration()
{
  StlType* aVector (NULL);
  CollectionType* aCollec (NULL);

  CollectionFiller<CollectionType, StlType>::Perform (&aVector, &aCollec);

  typename StlType::iterator aVecIter = aVector->begin();
  typename CollectionType::iterator aColIter = aCollec->begin();

  Standard_Boolean aResult (Standard_True);

  for (; aVecIter != aVector->end(); ++aVecIter, ++aColIter)
  {
    if (*aVecIter != *aColIter)
      aResult = Standard_False;
  }

  if (aColIter != aCollec->end())
  {
    aResult = Standard_False;
  }

  delete aVector;
  delete aCollec;

  return aResult;
}

//=======================================================================
//function : TestMinMax
//purpose  :
//=======================================================================
template<class CollectionType, class StlType>
Standard_Boolean TestMinMax()
{
  StlType* aVector (NULL);
  CollectionType* aCollec (NULL);

  CollectionFiller<CollectionType, StlType>::Perform (&aVector, &aCollec);

  typename StlType::value_type aValue1 = *std::min_element (aVector->begin(), aVector->end());
  typename CollectionType::value_type aValue2 = *std::min_element (aCollec->begin(), aCollec->end());

  Standard_Boolean aResult (Standard_True);

  if (aValue1 != aValue2)
    aResult = Standard_False;

  aValue1 = *std::max_element (aVector->begin(), aVector->end());
  aValue2 = *std::max_element (aCollec->begin(), aCollec->end());

  if (aValue1 != aValue2)
    aResult = Standard_False;

  delete aVector;
  delete aCollec;

  return aResult;
}

//=======================================================================
//function : TestReplace
//purpose  :
//=======================================================================
template<class CollectionType, class StlType>
Standard_Boolean TestReplace()
{
  StlType* aVector (NULL);
  CollectionType* aCollec (NULL);

  CollectionFiller<CollectionType, StlType>::Perform (&aVector, &aCollec);

  const typename StlType::value_type aValue = aVector->back();

  std::replace (aVector->begin(), aVector->end(), aValue, static_cast<typename StlType::value_type> (-1));
  std::replace (aCollec->begin(), aCollec->end(), aValue, static_cast<typename CollectionType::value_type> (-1));

  typename StlType::iterator aVecIter = aVector->begin();
  typename CollectionType::iterator aColIter = aCollec->begin();

  Standard_Boolean aResult (Standard_True);

  for (; aVecIter != aVector->end(); ++aVecIter, ++aColIter)
  {
    if (*aVecIter != *aColIter)
      aResult = Standard_False;
  }

  if (aColIter != aCollec->end())
  {
    aResult = Standard_False;
  }

  delete aVector;
  delete aCollec;

  return aResult;
}

//=======================================================================
//function : TestReverse
//purpose  :
//=======================================================================
template<class CollectionType, class StlType>
Standard_Boolean TestReverse()
{
  StlType* aVector (NULL);
  CollectionType* aCollec (NULL);

  CollectionFiller<CollectionType, StlType>::Perform (&aVector, &aCollec);

  std::reverse (aVector->begin(), aVector->end());
  std::reverse (aCollec->begin(), aCollec->end());

  typename StlType::iterator aVecIter = aVector->begin();
  typename CollectionType::iterator aColIter = aCollec->begin();

  Standard_Boolean aResult (Standard_True);

  for (; aVecIter != aVector->end(); ++aVecIter, ++aColIter)
  {
    if (*aVecIter != *aColIter)
      aResult = Standard_False;
  }

  if (aColIter != aCollec->end())
  {
    aResult = Standard_False;
  }

  delete aVector;
  delete aCollec;

  return aResult;
}

//=======================================================================
//function : TestSort
//purpose  :
//=======================================================================
template<class CollectionType, class StlType>
Standard_Boolean TestSort()
{
  StlType* aVector (NULL);
  CollectionType* aCollec (NULL);

  CollectionFiller<CollectionType, StlType>::Perform (&aVector, &aCollec);

  std::sort (aVector->begin(), aVector->end());
  std::sort (aCollec->begin(), aCollec->end());

  typename StlType::iterator aVecIter = aVector->begin();
  typename CollectionType::iterator aColIter = aCollec->begin();

  Standard_Boolean aResult (Standard_True);

  for (; aVecIter != aVector->end(); ++aVecIter, ++aColIter)
  {
    if (*aVecIter != *aColIter)
      aResult = Standard_False;
  }

  if (aColIter != aCollec->end())
  {
    aResult = Standard_False;
  }

  delete aVector;
  delete aCollec;

  return aResult;
}

template <typename T>
struct Invoker
{
  void operator()(T& theValue) const
  {
    theValue *= 2;
  }
};

//=======================================================================
//function : TestParallel
//purpose  :
//=======================================================================
template<class CollectionType, class StlType>
Standard_Boolean TestParallel()
{
  StlType* aVector (NULL);
  CollectionType* aCollec (NULL);

  CollectionFiller<CollectionType, StlType>::Perform (&aVector, &aCollec);

  OSD_Parallel::ForEach(aVector->begin(), aVector->end(), Invoker<typename StlType::value_type>());
  OSD_Parallel::ForEach(aCollec->begin(), aCollec->end(), Invoker<typename CollectionType::value_type>());

  typename StlType::iterator aVecIter = aVector->begin();
  typename CollectionType::iterator aColIter = aCollec->begin();

  Standard_Boolean aResult (Standard_True);

  for (; aVecIter != aVector->end(); ++aVecIter, ++aColIter)
  {
    if (*aVecIter != *aColIter)
      aResult = Standard_False;
  }

  if (aColIter != aCollec->end())
  {
    aResult = Standard_False;
  }

  delete aVector;
  delete aCollec;

  return aResult;
}

//=======================================================================
//function : TestDataMapParallel
//purpose  :
//=======================================================================
template<class CollectionType, class T>
Standard_Boolean TestDataMapParallel()
{
  CollectionType* aCollec1 (NULL);
  CollectionType* aCollec2 (NULL);

  MapFiller<CollectionType, T>::Perform (&aCollec1, &aCollec2);

  OSD_Parallel::ForEach(aCollec1->begin(), aCollec1->end(), Invoker<T>());

  // create OCCT-style iterator
  typename CollectionType::Iterator aOccIter (*aCollec2);

  // create STL-compatible iterator
  typename CollectionType::const_iterator aStlIter = aCollec1->cbegin();

  Standard_Boolean aResult (Standard_True);

  for (; aStlIter != aCollec1->cend(); ++aStlIter, aOccIter.Next())
  {
    if (static_cast<T> (2) * aOccIter.Value() != *aStlIter)
      aResult = Standard_False;
  }

  if (aOccIter.More())
  {
    aResult = Standard_False;
  }

  delete aCollec1;
  delete aCollec2;

  return aResult;
}

//=======================================================================
//function : TestMapIteration
//purpose  :
//=======================================================================
template<class CollectionType, class T>
Standard_Boolean TestMapIteration()
{
  CollectionType* aCollec (NULL);

  MapFiller<CollectionType, T>::Perform (&aCollec);

  // create OCCT-style iterator
  typename CollectionType::Iterator aOccIter (*aCollec);

  // create STL-compatible iterator
  typename CollectionType::const_iterator aStlIter = aCollec->cbegin();

  Standard_Boolean aResult (Standard_True);

  for (; aStlIter != aCollec->cend(); ++aStlIter, aOccIter.Next())
  {
    if (aOccIter.Value() != *aStlIter)
      aResult = Standard_False;
  }

  if (aOccIter.More())
  {
    aResult = Standard_False;
  }

  delete aCollec;

  return aResult;
}

//=======================================================================
//function : TestForwardIterator
//purpose  : test basic features of iterator (forward)
//=======================================================================
template <class CollectionType>
void TestForwardIterator ()
{
  CollectionType* aCollec (NULL);

  CollectionFiller<CollectionType, void>::Perform (&aCollec);
  
  // test non-const iteration
  typename CollectionType::iterator it = aCollec->begin(); // copy construction
  typename CollectionType::iterator it2; // default constructor
  it2 = it; // assignment
  it2 = it++; // postfix increment
  if (it2 == it || ! (it2 != it))
    std::cout << "Failed " << typeid(it).name() << " equality check" << std::endl;
  it2 = ++it; // prefix increment
  if (it2 != it || ! (it2 == it))
    std::cout << "Failed " << typeid(it).name() << " equality check" << std::endl;

  typename CollectionType::iterator::value_type t = *it;
  *it2 = t;
  *(it2.operator-> ()) = t;

  // test const iteration
  typename CollectionType::const_iterator cit = aCollec->cbegin(); // copy construction
  typename CollectionType::const_iterator cit2; // default constructor
  cit2 = cit; // assignment
  cit2 = cit++; // postfix increment
  if (cit2 == cit || ! (cit2 != cit))
    std::cout << "Failed " << typeid(cit).name() << " equality check" << std::endl;
  cit2 = ++cit; // prefix increment
  if (cit2 != it || ! (cit2 == cit))
    std::cout << "Failed " << typeid(cit).name() << " equality check" << std::endl;

  typename CollectionType::const_iterator::value_type ct = *cit;
  ct = *cit;
  (void)ct;
//  *cit2 = ct;
//  *(cit2.operator-> ()) = t;

  delete aCollec;
}

//=======================================================================
//function : TestBidirIterator
//purpose  : test features of bidirectional iterator
//=======================================================================
template <class CollectionType>
void TestBidirIterator ()
{
  CollectionType* aCollec (NULL);

  CollectionFiller<CollectionType, void>::Perform (&aCollec);
  
  // test non-const iteration
  typename CollectionType::iterator it = aCollec->end(); // copy construction
  typename CollectionType::iterator it2 = it--; // postfix decrement
  if (it2 == it || ! (it2 != it))
    std::cout << "Failed " << typeid(it).name() << " equality check" << std::endl;
  it2 = --it; // prefix decrement
  if (it2 != it || ! (it2 == it))
    std::cout << "Failed " << typeid(it).name() << " equality check" << std::endl;

  delete aCollec;
}

//=======================================================================
//function : TestRandomIterator
//purpose  : test features of random iterator
//=======================================================================
template <class CollectionType>
void TestRandomIterator ()
{
  CollectionType* aCollec (NULL);

  CollectionFiller<CollectionType, void>::Perform (&aCollec);
  
  // test non-const iteration
  typename CollectionType::iterator it = aCollec->begin(); // copy construction
  typename CollectionType::iterator it2 = it + 5;
  if ((it2 - it) != 5)
    std::cout << "Failed " << typeid(it).name() << " arithmetics" << std::endl;
  if (it2 < it || it2 <= it || ! (it2 > it) || ! (it2 >= it))
    std::cout << "Failed " << typeid(it).name() << " comparison" << std::endl;
  it += 5;
  if (it2 != it)
    std::cout << "Failed " << typeid(it).name() << " arithmetics" << std::endl;
  it2 = it - 5;
  if ((it2 - it) != -5)
    std::cout << "Failed " << typeid(it).name() << " arithmetics" << std::endl;
  if (it2 > it || it2 >= it || ! (it2 < it) || ! (it2 <= it))
    std::cout << "Failed " << typeid(it).name() << " comparison" << std::endl;
  it -= 5;
  if (it2 != it)
    std::cout << "Failed " << typeid(it).name() << " arithmetics" << std::endl;

  typename CollectionType::value_type t = it[5]; // offset dereference
  *it = t;

  delete aCollec;
}

//=======================================================================
//function : QANListStlIterator
//purpose  :
//=======================================================================
static Standard_Integer QANListStlIterator (Draw_Interpretor&, Standard_Integer, const char**)
{
  // compile-time tests
  TestForwardIterator <NCollection_List<Standard_Integer> >();

  // run-time tests
  Standard_Boolean aResult = TestIteration<NCollection_List<int>, std::list<int> >();
  std::cout << "NCollection_List<int> Iteration:                " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestIteration<NCollection_List<double>, std::list<double> >();
  std::cout << "NCollection_List<double> Iteration:             " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestMinMax<NCollection_List<int>, std::list<int> >();
  std::cout << "NCollection_List<int> Min-Max:                  " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestMinMax<NCollection_List<double>, std::list<double> >();
  std::cout << "NCollection_List<double> Min-Max:               " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestReplace<NCollection_List<int>, std::list<int> >();
  std::cout << "NCollection_List<int> Replace:                  " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestReplace<NCollection_List<double>, std::list<double> >();
  std::cout << "NCollection_List<double> Replace:               " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestParallel< NCollection_List<int>, std::list<int> >();
  std::cout << "NCollection_List<int> Parallel:                 " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestParallel<NCollection_List<double>, std::list<double> >();
  std::cout << "NCollection_List<double> Parallel:              " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  return 0;
}

//=======================================================================
//function : QANMapStlIterator
//purpose  :
//=======================================================================
static Standard_Integer QANMapStlIterator (Draw_Interpretor&, Standard_Integer, const char**)
{
  // compile-time tests
//  TestForwardIterator <NCollection_Map<Standard_Integer> >();

  // run-time tests
  Standard_Boolean aResult = TestMapIteration<NCollection_Map<Standard_Integer>, Standard_Integer>();
  std::cout << "NCollection_Map<int> Iteration:                 " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestMapIteration<NCollection_Map<Standard_Real>, Standard_Real>();
  std::cout << "NCollection_Map<double> Iteration:              " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  return 0;
}

//=======================================================================
//function : QANIndexedMapStlIterator
//purpose  :
//=======================================================================
static Standard_Integer QANIndexedMapStlIterator (Draw_Interpretor&, Standard_Integer, const char**)
{
  // compile-time tests
//  TestForwardIterator <NCollection_IndexedMap<Standard_Integer> >();
//  TestBidirIterator <NCollection_IndexedMap<Standard_Integer> >();

  // run-time tests
  Standard_Boolean aResult = TestMapIteration<NCollection_IndexedMap<Standard_Integer>, Standard_Integer>();
  std::cout << "NCollection_IndexedMap<int> Iteration:          " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestMapIteration<NCollection_IndexedMap<Standard_Real>, Standard_Real>();
  std::cout << "NCollection_IndexedMap<double> Iteration:       " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  return 0;
}

//=======================================================================
//function : QANDataMapStlIterator
//purpose  :
//=======================================================================
static Standard_Integer QANDataMapStlIterator (Draw_Interpretor&, Standard_Integer, const char**)
{
  // compile-time tests
//  TestForwardIterator <NCollection_DataMap<int, int> >();
//  TestBidirIterator <NCollection_DataMap<int, int> >();

  // run-time tests
  Standard_Boolean aResult = TestMapIteration<NCollection_DataMap<Standard_Integer, Standard_Integer>, Standard_Integer>();
  std::cout << "NCollection_DataMap<int> Iteration:             " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestMapIteration<NCollection_DataMap<Standard_Real, Standard_Real>, Standard_Real>();
  std::cout << "NCollection_DataMap<double> Iteration:          " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestDataMapParallel<NCollection_DataMap<Standard_Integer, Standard_Integer>, Standard_Integer>();
  std::cout << "NCollection_DataMap<int> Parallel:              " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestDataMapParallel<NCollection_DataMap<Standard_Real, Standard_Real>, Standard_Real>();
  std::cout << "NCollection_DataMap<double> Parallel:           " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  return 0;
}

//=======================================================================
//function : QANIndexedDataMapStlIterator
//purpose  :
//=======================================================================
static Standard_Integer QANIndexedDataMapStlIterator (Draw_Interpretor&, Standard_Integer, const char**)
{
  // compile-time tests
//  TestForwardIterator <NCollection_IndexedDataMap<int, int> >();
//  TestBidirIterator <NCollection_IndexedDataMap<int, int> >();

  // run-time tests
  Standard_Boolean aResult = TestMapIteration<NCollection_IndexedDataMap<Standard_Integer, Standard_Integer>, Standard_Integer>();
  std::cout << "NCollection_IndexedDataMap<int> Iteration:      " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestMapIteration<NCollection_IndexedDataMap<Standard_Real, Standard_Real>, Standard_Real>();
  std::cout << "NCollection_IndexedDataMap<double> Iteration:   " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestDataMapParallel<NCollection_IndexedDataMap<Standard_Integer, Standard_Integer>, Standard_Integer>();
  std::cout << "NCollection_IndexedDataMap<int> Parallel:       " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestDataMapParallel<NCollection_IndexedDataMap<Standard_Real, Standard_Real>, Standard_Real>();
  std::cout << "NCollection_IndexedDataMap<double> Parallel:    " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  return 0;
}

//=======================================================================
//function : QANSequenceStlIterator
//purpose  :
//=======================================================================
static Standard_Integer QANSequenceStlIterator (Draw_Interpretor&, Standard_Integer, const char**)
{
  // compile-time tests
  TestForwardIterator <NCollection_Sequence<int> >();
  TestBidirIterator <NCollection_Sequence<int> >();

  // run-time tests
  Standard_Boolean aResult = TestIteration<NCollection_Sequence<int>, std::list<int> >();
  std::cout << "NCollection_Sequence<int> Iteration:            " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestIteration<NCollection_Sequence<double>, std::list<double> >();
  std::cout << "NCollection_Sequence<double> Iteration:         " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestMinMax<NCollection_Sequence<int>, std::list<int> >();
  std::cout << "NCollection_Sequence<int> Min-Max:              " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestMinMax<NCollection_Sequence<double>, std::list<double> >();
  std::cout << "NCollection_Sequence<double> Min-Max:           " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestReplace<NCollection_Sequence<int>, std::list<int> >();
  std::cout << "NCollection_Sequence<int> Replace:              " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestReplace<NCollection_Sequence<double>, std::list<double> >();
  std::cout << "NCollection_Sequence<double> Replace:           " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestReverse<NCollection_Sequence<int>, std::list<int> >();
  std::cout << "NCollection_Sequence<int> Reverse:              " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestReverse<NCollection_Sequence<double>, std::list<double> >();
  std::cout << "NCollection_Sequence<double> Reverse:           " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestParallel<NCollection_Sequence<int>, std::list<int> >();
  std::cout << "NCollection_Sequence<int> Parallel:             " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestParallel<NCollection_Sequence<double>, std::list<double> >();
  std::cout << "NCollection_Sequence<double> Parallel:          " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  return 0;
}

//=======================================================================
//function : QANVectorStlIterator
//purpose  :
//=======================================================================
static Standard_Integer QANVectorStlIterator (Draw_Interpretor&, Standard_Integer, const char**)
{
  // compile-time tests
  TestForwardIterator <NCollection_Vector<int> >();
  TestBidirIterator <NCollection_Vector<int> >();
  TestRandomIterator <NCollection_Vector<int> >();

  // run-time tests
  Standard_Boolean aResult = TestIteration<NCollection_Vector<int>, std::vector<int> >();
  std::cout << "NCollection_Vector<int> Iteration:              " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestIteration<NCollection_Vector<double>, std::vector<double> >();
  std::cout << "NCollection_Vector<double> Iteration:           " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestMinMax<NCollection_Vector<int>, std::vector<int> >();
  std::cout << "NCollection_Vector<int> Min-Max:                " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestMinMax<NCollection_Vector<double>, std::vector<double> >();
  std::cout << "NCollection_Vector<double> Min-Max:             " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestReplace<NCollection_Vector<int>, std::vector<int> >();
  std::cout << "NCollection_Vector<int> Replace:                " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestReplace<NCollection_Vector<double>, std::vector<double> >();
  std::cout << "NCollection_Vector<double> Replace:             " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestReverse<NCollection_Vector<int>, std::vector<int> >();
  std::cout << "NCollection_Vector<int> Reverse:                " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestReverse<NCollection_Vector<double>, std::vector<double> >();
  std::cout << "NCollection_Vector<double> Reverse:             " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestSort<NCollection_Vector<int>, std::vector<int> >();
  std::cout << "NCollection_Vector<int> Sort:                   " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestSort<NCollection_Vector<double>, std::vector<double> >();
  std::cout << "NCollection_Vector<double> Sort:                " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestParallel<NCollection_Vector<int>, std::vector<int> >();
  std::cout << "NCollection_Vector<int> Parallel:               " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestParallel<NCollection_Vector<double>, std::vector<double> >();
  std::cout << "NCollection_Vector<double> Parallel:            " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  {
    // Test case for a corner case described in a bug #0027941
    // when vector length matches the increment.
    // In this case NCollection_Vector::Iterator::Offset() produced mathematically equal
    // but not the same iterator as returned by NCollection_Vector::end()
    // so that their comparison was not equal.
    // As result, std::stable_sort() crashed due to out-of-range access.
    const int THE_INCREMENT = 256;
    NCollection_Vector<int> aVector (THE_INCREMENT);
    for (int anIter = 0; anIter < THE_INCREMENT; ++anIter)
    {
      aVector.Append (THE_INCREMENT - anIter);
    }

    NCollection_Vector<int>::iterator aBegin = aVector.begin();
    NCollection_Vector<int>::iterator anEnd  = aVector.end();
    NCollection_Vector<int>::iterator aShift = aBegin + THE_INCREMENT;
    aResult = (aShift == anEnd);
    std::cout << "NCollection_Vector<int> Offset:                 " <<
      (aResult ? "SUCCESS" : "FAIL") << std::endl;

    std::stable_sort (aVector.begin(), aVector.end());
  }

  return 0;
}

//=======================================================================
//function : QANArray1StlIterator
//purpose  :
//=======================================================================
static Standard_Integer QANArray1StlIterator (Draw_Interpretor&, Standard_Integer, const char**)
{
  // compile-time tests
  TestForwardIterator <NCollection_Vector<int> >();
  TestBidirIterator <NCollection_Vector<int> >();
  TestRandomIterator <NCollection_Vector<int> >();

  // run-time tests
  Standard_Boolean aResult = TestIteration<NCollection_Array1<int>, std::vector<int> >();
  std::cout << "NCollection_Array1<int> Iteration:              " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestIteration<NCollection_Array1<double>, std::vector<double> >();
  std::cout << "NCollection_Array1<double> Iteration:           " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestMinMax<NCollection_Array1<int>, std::vector<int> >();
  std::cout << "NCollection_Array1<int> Min-Max:                " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestMinMax<NCollection_Array1<double>, std::vector<double> >();
  std::cout << "NCollection_Array1<double> Min-Max:             " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestReplace<NCollection_Array1<int>, std::vector<int> >();
  std::cout << "NCollection_Array1<int> Replace:                " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestReplace<NCollection_Array1<double>, std::vector<double> >();
  std::cout << "NCollection_Array1<double> Replace:             " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestReverse<NCollection_Array1<int>, std::vector<int> >();
  std::cout << "NCollection_Array1<int> Reverse:                " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestReverse<NCollection_Array1<double>, std::vector<double> >();
  std::cout << "NCollection_Array1<double> Reverse:             " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestSort<NCollection_Array1<int>, std::vector<int> >();
  std::cout << "NCollection_Array1<int> Sort:                   " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestSort<NCollection_Array1<double>, std::vector<double> >();
  std::cout << "NCollection_Array1<double> Sort:                " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestParallel<NCollection_Array1<int>, std::vector<int> >();
  std::cout << "NCollection_Array1<int> Parallel:               " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  aResult = TestParallel<NCollection_Array1<double>, std::vector<double> >();
  std::cout << "NCollection_Array1<double> Parallel:            " <<
    (aResult ? "SUCCESS" : "FAIL") << std::endl;

  return 0;
}

//=======================================================================
//function : QANTestStlIterators
//purpose  :
//=======================================================================
static Standard_Integer QANTestStlIterators (
  Draw_Interpretor& theInterpretor, Standard_Integer, const char**)
{
  QANListStlIterator           (theInterpretor, 0, NULL);
  QANArray1StlIterator         (theInterpretor, 0, NULL);
  QANVectorStlIterator         (theInterpretor, 0, NULL);
  QANSequenceStlIterator       (theInterpretor, 0, NULL);
  QANMapStlIterator            (theInterpretor, 0, NULL);
  QANDataMapStlIterator        (theInterpretor, 0, NULL);
  QANIndexedMapStlIterator     (theInterpretor, 0, NULL);
  QANIndexedDataMapStlIterator (theInterpretor, 0, NULL);

  return 0;
}

//=======================================================================
//function : TestPerformanceRandomIterator
//purpose  :
//=======================================================================
template<class CollectionType, class StlType>
void TestPerformanceRandomIterator(Draw_Interpretor& di)
{
  OSD_Timer aTimer;

  StlType* aVector (NULL);
  CollectionType* aCollec (NULL);

  for (Standard_Integer aSize = 10000; aSize <= 1280000; aSize *= 2)
  {
    CollectionFiller<CollectionType, StlType>::Perform (&aVector, &aCollec, aSize);

    aTimer.Reset();
    aTimer.Start();
    {
      RandomGenerator aRandomGen;
      for (Standard_Integer anIdx = 0; anIdx < 10; ++anIdx)
      {
        std::sort           (aVector->begin(), aVector->end());
        std::random_shuffle (aVector->begin(), aVector->end(), aRandomGen);
      }
    }
    aTimer.Stop();

    Standard_Real aStlTime = aTimer.ElapsedTime();

    aTimer.Reset();
    aTimer.Start();
    {
      RandomGenerator aRandomGen;
      for (Standard_Integer anIdx = 0; anIdx < 10; ++anIdx)
      {
        std::sort           (aCollec->begin(), aCollec->end());
        std::random_shuffle (aCollec->begin(), aCollec->end(), aRandomGen);
      }
    }
    aTimer.Stop();

    Standard_Real aOccTime = aTimer.ElapsedTime();

    di << aSize << "\t" << aStlTime << "\t" <<
      aOccTime << "\t" << aOccTime / aStlTime << "\n";

    // check that result is the same
    if ( ! std::equal (aVector->begin(), aVector->end(), aCollec->begin()) )
      di << "Error: sequences are not the same at the end!\n";

    delete aVector;
    delete aCollec;
  }
}

//=======================================================================
//function : TestPerformanceForwardIterator
//purpose  :
//=======================================================================
template<class CollectionType, class StlType>
void TestPerformanceForwardIterator(Draw_Interpretor& di)
{
  OSD_Timer aTimer;

  StlType* aVector = 0;
  CollectionType* aCollec = 0;

  for (Standard_Integer aSize = 10000; aSize <= 1280000; aSize *= 2)
  {
    CollectionFiller<CollectionType, StlType>::Perform (&aVector, &aCollec, aSize);

    aTimer.Reset();
    aTimer.Start();
    {
      for (Standard_Integer anIdx = 0; anIdx < 1000; ++anIdx)
      {
        std::replace (aVector->begin(), aVector->end(), *aVector->begin(), static_cast<typename StlType::value_type> (anIdx));
      }
    }
    aTimer.Stop();

    Standard_Real aStlTime = aTimer.ElapsedTime();

    aTimer.Reset();
    aTimer.Start();
    {
      for (Standard_Integer anIdx = 0; anIdx < 1000; ++anIdx)
      {
        std::replace (aCollec->begin(), aCollec->end(), *aCollec->begin(), static_cast<typename CollectionType::value_type> (anIdx));
      }
    }
    aTimer.Stop();

    Standard_Real aOccTime = aTimer.ElapsedTime();

    di << aSize << "\t" << aStlTime << "\t" <<
      aOccTime << "\t" << aOccTime / aStlTime << "\n";

    // check that result is the same
    if ( ! std::equal (aVector->begin(), aVector->end(), aCollec->begin()) )
      di << "Error: sequences are not the same at the end!\n";

    delete aVector;
    delete aCollec;
  }
}

//=======================================================================
//function : TestPerformanceBidirIterator
//purpose  :
//=======================================================================
template<class CollectionType, class StlType>
void TestPerformanceBidirIterator(Draw_Interpretor& di)
{
  OSD_Timer aTimer;

  StlType* aVector = 0;
  CollectionType* aCollec = 0;

  for (Standard_Integer aSize = 10000; aSize <= 1280000; aSize *= 2)
  {
    CollectionFiller<CollectionType, StlType>::Perform (&aVector, &aCollec, aSize);

    aTimer.Reset();
    aTimer.Start();
    {
      for (Standard_Integer anIdx = 0; anIdx < 1000; ++anIdx)
      {
        std::reverse (aVector->begin(), aVector->end());
      }
    }
    aTimer.Stop();

    Standard_Real aStlTime = aTimer.ElapsedTime();

    aTimer.Reset();
    aTimer.Start();
    {
      for (Standard_Integer anIdx = 0; anIdx < 1000; ++anIdx)
      {
        std::reverse (aCollec->begin(), aCollec->end());
      }
    }
    aTimer.Stop();

    Standard_Real aOccTime = aTimer.ElapsedTime();

    di << aSize << "\t" << aStlTime << "\t" <<
      aOccTime << "\t" << aOccTime / aStlTime << "\n";

    // check that result is the same
    if ( ! std::equal (aVector->begin(), aVector->end(), aCollec->begin()) )
      di << "Error: sequences are not the same at the end!\n";

    delete aVector;
    delete aCollec;
  }
}

//=======================================================================
//function : TestPerformanceMapAccess
//purpose  :
//=======================================================================
template<class CollectionType, class T>
void TestPerformanceMapAccess(Draw_Interpretor& di)
{
  OSD_Timer aTimer;

  CollectionType* aCollec (NULL);

  for (Standard_Integer aSize = 100000; aSize <= 3200000; aSize *= 2)
  {
    MapFiller<CollectionType, T>::Perform (&aCollec, aSize);

    std::set<T>    aSet (aCollec->cbegin(), aCollec->cend());
    std::vector<T> aVec (aCollec->cbegin(), aCollec->cend());

    Standard_Boolean aResult = Standard_True;

    aTimer.Reset();
    aTimer.Start();
    {
      for (Standard_Integer anIdx = 0; anIdx < 10000; ++anIdx)
      {
        if (aSet.find (aVec[anIdx + 1000]) == aSet.end())
          aResult = Standard_False;
        if (aSet.find (aVec[anIdx + 2000]) == aSet.end())
          aResult = Standard_False;
        if (aSet.find (aVec[anIdx + 3000]) == aSet.end())
          aResult = Standard_False;
        if (aSet.find (aVec[anIdx + 4000]) == aSet.end())
          aResult = Standard_False;
        if (aSet.find (aVec[anIdx + 5000]) == aSet.end())
          aResult = Standard_False;
      }
    }
    aTimer.Stop();

    Standard_Real aStlTime = aTimer.ElapsedTime();

    aTimer.Reset();
    aTimer.Start();
    {
      for (Standard_Integer anIdx = 0; anIdx < 10000; ++anIdx)
      {
        if (!aCollec->Contains (aVec[anIdx + 1000]))
          aResult = Standard_False;
        if (!aCollec->Contains (aVec[anIdx + 2000]))
          aResult = Standard_False;
        if (!aCollec->Contains (aVec[anIdx + 3000]))
          aResult = Standard_False;
        if (!aCollec->Contains (aVec[anIdx + 4000]))
          aResult = Standard_False;
        if (!aCollec->Contains (aVec[anIdx + 5000]))
          aResult = Standard_False;
      }
    }
    aTimer.Stop();

    Standard_Real aOccTime = aTimer.ElapsedTime();

    if (aResult)
    {
      di << aSize << "\t" << aStlTime << "\t" <<
        aOccTime << "\t" << (aStlTime > 1e-16 ? aOccTime / aStlTime : -1) << "\n";
    }

    delete aCollec;
  }
}

//=======================================================================
//function : QANTestNCollectionPerformance
//purpose  :
//=======================================================================
static Standard_Integer QANTestNCollectionPerformance (Draw_Interpretor& di, Standard_Integer, const char**)
{
  di << "Testing performance (Size | STL time | OCCT time | STL/OCCT boost)\n";

  di << "\nstd::vector vs NCollection_Array1 (sort):\n\n";
  TestPerformanceRandomIterator<NCollection_Array1<double>, std::vector<double> >(di);

  di << "\nstd::vector vs NCollection_Vector (sort):\n\n";
  TestPerformanceRandomIterator<NCollection_Vector<double>, std::vector<double> >(di);

  di << "\nstd::vector vs NCollection_Array1 (replace):\n\n";
  TestPerformanceForwardIterator<NCollection_Array1<double>, std::vector<double> >(di);

  di << "\nstd::vector vs NCollection_Vector (replace):\n\n";
  TestPerformanceForwardIterator<NCollection_Vector<double>, std::vector<double> >(di);

  di << "\nstd::list vs NCollection_List (replace):\n\n";
  TestPerformanceForwardIterator<NCollection_List<double>, std::list<double> >(di);

  di << "\nstd::list vs NCollection_Sequence (replace):\n\n";
  TestPerformanceForwardIterator<NCollection_Sequence<double>, std::list<double> >(di);

  di << "\nstd::list vs NCollection_Sequence (reverse):\n\n";
  TestPerformanceBidirIterator<NCollection_Sequence<double>, std::list<double> >(di);

  di << "\nstd::set vs NCollection_Map (search):\n\n";
  TestPerformanceMapAccess<NCollection_Map<int>, int>(di);

  di << "\nstd::set vs NCollection_IndexedMap (search):\n\n";
  TestPerformanceMapAccess<NCollection_IndexedMap<int>, int>(di);

  return 0;
}

//=======================================================================
//function : QANTestNCollectionIndexedMap
//purpose  :
//=======================================================================
static Standard_Integer QANTestNCollectionIndexedMap (Draw_Interpretor& di, Standard_Integer, const char**)
{
  OSD_Timer aTimer;

  std::vector<Standard_Integer>            aIndxs;
  std::vector<Standard_Integer>            aItems;
  NCollection_IndexedMap<Standard_Integer> aIndxMap;

  const Standard_Integer aNbItems = 1000000;

  srand (1);
  for (Standard_Integer anId = 1; anId <= aNbItems; ++anId)
  {
    const Standard_Integer aVal = anId * 2;

    aIndxs.push_back (anId);
    aItems.push_back (aVal);

    aIndxMap.Add  (aVal);
  }

  aTimer.Start();
  for (Standard_Integer anId = 0; anId < aNbItems; ++anId)
  {
    if (aIndxMap.FindIndex (aItems[anId]) != aIndxs[anId])
    {
      std::cout << "failed FindIndex\n";
    }

    if (aIndxMap.FindKey (aIndxs[anId]) != aItems[anId])
    {
      std::cout << "failed FindKey\n";
    }
  }
  aTimer.Stop();

  const Standard_Real aTime1 = aTimer.ElapsedTime();

  aTimer.Reset();
  aTimer.Start();
  for (Standard_Integer anId = 0; anId < aNbItems / 30; ++anId)
  {
    const Standard_Integer anId2 = Min (aNbItems - 1,
      static_cast<Standard_Integer> (rand() / float (RAND_MAX) * aNbItems));

    aIndxMap.Swap (aIndxs[anId], aIndxs[anId2]);

    std::swap (aIndxs[anId], aIndxs[anId2]);
  }
  aTimer.Stop();

  const Standard_Real aTime2 = aTimer.ElapsedTime();

  aTimer.Reset();
  aTimer.Start();
  for (Standard_Integer anId = 0; anId < aNbItems; ++anId)
  {
    if (aIndxMap.FindIndex (aItems[anId]) != aIndxs[anId])
    {
      std::cout << "failed FindIndex\n";
    }

    if (aIndxMap.FindKey (aIndxs[anId]) != aItems[anId])
    {
      std::cout << "failed FindKey\n";
    }
  }
  aTimer.Stop();

  const Standard_Real aTime3 = aTimer.ElapsedTime();

  aTimer.Reset();
  aTimer.Start();
  for (Standard_Integer anId = 0; anId < aNbItems; ++anId)
  {
    aIndxMap.RemoveLast();
  }
  aTimer.Stop();

  const Standard_Real aTime4 = aTimer.ElapsedTime();

  di << "Search time 1: " << aTime1 << "\n"
     << "Swapping time: " << aTime2 << "\n"
     << "Search time 2: " << aTime3 << "\n"
     << "Remove   time: " << aTime4 << "\n";

  return 0;
}

//=======================================================================
//function : QANTestNCollectionIndexedDataMap
//purpose  :
//=======================================================================
static Standard_Integer QANTestNCollectionIndexedDataMap (Draw_Interpretor& di, Standard_Integer, const char**)
{
  OSD_Timer aTimer;

  std::vector<Standard_Integer>                                  aIndxs;
  std::vector<Standard_Integer>                                  aItems;
  NCollection_IndexedDataMap<Standard_Integer, Standard_Integer> aIndxMap;

  const Standard_Integer aNbItems = 1000000;

  srand (1);
  for (Standard_Integer anId = 1; anId <= aNbItems; ++anId)
  {
    const Standard_Integer aVal = anId * 2;

    aIndxs.push_back (anId);
    aItems.push_back (aVal);

    aIndxMap.Add  (aVal, aVal * 2);
  }

  aTimer.Start();
  for (Standard_Integer anId = 0; anId < aNbItems; ++anId)
  {
    if (aIndxMap.FindIndex (aItems[anId]) != aIndxs[anId])
    {
      std::cout << "failed FindIndex\n";
    }

    if (aIndxMap.FindKey (aIndxs[anId]) != aItems[anId])
    {
      std::cout << "failed FindKey\n";
    }
  }
  aTimer.Stop();

  const Standard_Real aTime1 = aTimer.ElapsedTime();

  aTimer.Reset();
  aTimer.Start();
  for (Standard_Integer anId = 0; anId < aNbItems / 30; ++anId)
  {
    const Standard_Integer anId2 = Min (aNbItems - 1,
      static_cast<Standard_Integer> (rand() / float (RAND_MAX) * aNbItems));

    aIndxMap.Swap (aIndxs[anId], aIndxs[anId2]);

    std::swap (aIndxs[anId], aIndxs[anId2]);
  }
  aTimer.Stop();

  const Standard_Real aTime2 = aTimer.ElapsedTime();

  aTimer.Reset();
  aTimer.Start();
  for (Standard_Integer anId = 0; anId < aNbItems; ++anId)
  {
    if (aIndxMap.FindIndex (aItems[anId]) != aIndxs[anId])
    {
      std::cout << "failed FindIndex\n";
    }

    if (aIndxMap.FindKey (aIndxs[anId]) != aItems[anId])
    {
      std::cout << "failed FindKey\n";
    }
  }
  aTimer.Stop();

  const Standard_Real aTime3 = aTimer.ElapsedTime();

  aTimer.Reset();
  aTimer.Start();
  for (Standard_Integer anId = 0; anId < aNbItems; ++anId)
  {
    aIndxMap.RemoveLast();
  }
  aTimer.Stop();

  const Standard_Real aTime4 = aTimer.ElapsedTime();

  di << "Search time 1: " << aTime1 << "\n"
     << "Swapping time: " << aTime2 << "\n"
     << "Search time 2: " << aTime3 << "\n"
     << "Remove   time: " << aTime4 << "\n";

  return 0;
}

//=======================================================================
//function : CommandsStl
//purpose  :
//=======================================================================
void QANCollection::CommandsStl (Draw_Interpretor& theCommands)
{
  const char* aGroup = "QANCollection";

  theCommands.Add ("QANArray1StlIterator",
                   "QANArray1StlIterator",
                   __FILE__,
                   QANArray1StlIterator,
                   aGroup);

  theCommands.Add ("QANListStlIterator",
                   "QANListStlIterator",
                   __FILE__,
                   QANListStlIterator,
                   aGroup);

  theCommands.Add ("QANSequenceStlIterator",
                   "QANSequenceStlIterator",
                   __FILE__,
                   QANSequenceStlIterator,
                   aGroup);

  theCommands.Add ("QANVectorStlIterator",
                   "QANVectorStlIterator",
                   __FILE__,
                   QANVectorStlIterator,
                   aGroup);

  theCommands.Add ("QANMapStlIterator",
                   "QANMapStlIterator",
                   __FILE__,
                   QANMapStlIterator,
                   aGroup);

  theCommands.Add ("QANDataMapStlIterator",
                   "QANDataMapStlIterator",
                   __FILE__,
                   QANDataMapStlIterator,
                   aGroup);

  theCommands.Add ("QANIndexedMapStlIterator",
                   "QANIndexedMapStlIterator",
                   __FILE__,
                   QANIndexedMapStlIterator,
                   aGroup);

  theCommands.Add ("QANIndexedDataMapStlIterator",
                   "QANIndexedDataMapStlIterator",
                   __FILE__,
                   QANIndexedDataMapStlIterator,
                   aGroup);

  theCommands.Add ("QANTestStlIterators",
                   "QANTestStlIterators",
                   __FILE__,
                   QANTestStlIterators,
                   aGroup);

  theCommands.Add ("QANTestNCollectionPerformance",
                   "QANTestNCollectionPerformance",
                   __FILE__,
                   QANTestNCollectionPerformance,
                   aGroup);

  theCommands.Add ("QANTestNCollectionIndexedMap",
                   "QANTestNCollectionIndexedMap",
                   __FILE__,
                   QANTestNCollectionIndexedMap,
                   aGroup);

  theCommands.Add ("QANTestNCollectionIndexedDataMap",
                   "QANTestNCollectionIndexedDataMap",
                   __FILE__,
                   QANTestNCollectionIndexedDataMap,
                   aGroup);

  return;
}
