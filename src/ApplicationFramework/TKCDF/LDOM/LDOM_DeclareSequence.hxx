// Created on: 2001-01-29
// Created by: Alexander GRIGORIEV
// Copyright (c) 2001-2014 OPEN CASCADE SAS
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

#ifndef LDOM_DeclareSequence_HeaderFile
#define LDOM_DeclareSequence_HeaderFile

#ifndef _Standard_Macro_HeaderFile
  #include <Standard_Macro.hxx>
#endif

//      Declaration of Sequence (numbered list) class.
//  Remarks on the current implementation:
//
// 1. Methods First() and Last() added
// 2. The method InsertAt(anIndex) replaces InsertBefore and InsertAfter.
//    This method never throws exception "OutOfRange". Its behaviour:
//              anIndex <= 1            => equivalent to Prepend()
//              anIndex >  Length()     => equivalent to Append()
//              else                    => equivalent to InsertBefore.

// *******************************************************************
// use the following somewhere in a header file;
//    ClassName - name of the list class to create
//    Type      - type of members of the list
// *******************************************************************

#define DECLARE_SEQUENCE(ClassName, Type)                                                          \
                                                                                                   \
  class ClassName                                                                                  \
  {                                                                                                \
  public:                                                                                          \
    inline ClassName();                                                                            \
    inline ClassName(const ClassName& anOther);                                                    \
    inline ClassName&       operator=(const ClassName& anOther);                                   \
    inline int Length() const;                                                        \
    inline const Type&      First() const;                                                         \
    inline const Type&      Last() const;                                                          \
    inline const Type&      Value(const int) const;                                   \
    inline Type&            ChangeValue(const int);                                   \
    inline const Type&      operator()(const int) const;                              \
    inline Type&            operator()(const int);                                    \
                                                                                                   \
    Standard_EXPORT virtual ~ClassName();                                                          \
    Standard_EXPORT void Append(const Type& aVal);                                                 \
    Standard_EXPORT void Prepend(const Type& aVal);                                                \
    Standard_EXPORT void InsertAt(const int, const Type& aVal);                       \
    Standard_EXPORT void Clear();                                                                  \
    Standard_EXPORT void Remove(const int);                                           \
                                                                                                   \
  private:                                                                                         \
    class Node                                                                                     \
    {                                                                                              \
    private:                                                                                       \
      Type  myValue;                                                                               \
      Node* myPrev;                                                                                \
      Node* myNext;                                                                                \
                                                                                                   \
    public:                                                                                        \
      Node(const Type& aValue, Node* aPrv, Node* aNxt)                                             \
          : myValue(aValue),                                                                       \
            myPrev(aPrv),                                                                          \
            myNext(aNxt)                                                                           \
      {                                                                                            \
      }                                                                                            \
      const Type& Value() const                                                                    \
      {                                                                                            \
        return myValue;                                                                            \
      }                                                                                            \
      Type& ChangeValue()                                                                          \
      {                                                                                            \
        return myValue;                                                                            \
      }                                                                                            \
      friend class ClassName;                                                                      \
    };                                                                                             \
                                                                                                   \
    Standard_EXPORT const void* FindItem(const int) const;                            \
    Standard_EXPORT void        Assign(const ClassName& anOther);                                  \
                                                                                                   \
    Node*            myFirst;                                                                      \
    Node*            myLast;                                                                       \
    Node*            myCurrent;                                                                    \
    int myICur;                                                                       \
    int myLength;                                                                     \
  };                                                                                               \
                                                                                                   \
  inline ClassName::ClassName()                                                                    \
      : myFirst(NULL),                                                                             \
        myLast(NULL),                                                                              \
        myCurrent(NULL),                                                                           \
        myICur(0),                                                                                 \
        myLength(0)                                                                                \
  {                                                                                                \
  }                                                                                                \
                                                                                                   \
  inline ClassName::ClassName(const ClassName& anOther)                                            \
      : myFirst(NULL)                                                                              \
  {                                                                                                \
    Assign(anOther);                                                                               \
  }                                                                                                \
                                                                                                   \
  inline ClassName& ClassName::operator=(const ClassName& anOther)                                 \
  {                                                                                                \
    Assign(anOther);                                                                               \
    return *this;                                                                                  \
  }                                                                                                \
                                                                                                   \
  inline int ClassName::Length() const                                                \
  {                                                                                                \
    return myLength;                                                                               \
  }                                                                                                \
                                                                                                   \
  inline const Type& ClassName::First() const                                                      \
  {                                                                                                \
    return myFirst->Value(); /* exception if out of range */                                       \
  }                                                                                                \
                                                                                                   \
  inline const Type& ClassName::Last() const                                                       \
  {                                                                                                \
    return myLast->Value(); /* exception if out of range */                                        \
  }                                                                                                \
                                                                                                   \
  inline const Type& ClassName::Value(const int anI) const                            \
  {                                                                                                \
    const Node* anItem = (const Node*)FindItem(anI);                                               \
    return anItem->Value(); /* exception if out of range */                                        \
  }                                                                                                \
                                                                                                   \
  inline Type& ClassName::ChangeValue(const int anI)                                  \
  {                                                                                                \
    Node* anItem = (Node*)FindItem(anI);                                                           \
    return anItem->ChangeValue(); /* exception if out of range */                                  \
  }                                                                                                \
                                                                                                   \
  inline const Type& ClassName::operator()(const int anI) const                       \
  {                                                                                                \
    return Value(anI);                                                                             \
  }                                                                                                \
                                                                                                   \
  inline Type& ClassName::operator()(const int anI)                                   \
  {                                                                                                \
    return ChangeValue(anI);                                                                       \
  }

// *******************************************************************
// use the following in a translation unit (*.cxx);
//
// *******************************************************************
#define IMPLEMENT_SEQUENCE(ClassName, Type)                                                        \
  const void* ClassName::FindItem(const int anI) const                                \
  {                                                                                                \
    if (anI < 1 || anI > myLength)                                                                 \
      return NULL;                                                                                 \
    int aCounter;                                                                     \
    Node*            aCurrent = (Node*)myCurrent;                                                  \
    bool aDir(false);                                                         \
    if (aCurrent == NULL)                                                                          \
    {                                                                                              \
      aCurrent = myFirst;                                                                          \
      aCounter = anI - 1;                                                                          \
      aDir     = true;                                                                    \
    }                                                                                              \
    else                                                                                           \
    {                                                                                              \
      aCounter = std::abs(anI - myICur);                                                           \
      if (anI <= aCounter)                                                                         \
      {                                                                                            \
        aCurrent = myFirst;                                                                        \
        aCounter = anI - 1;                                                                        \
        aDir     = true;                                                                  \
      }                                                                                            \
      else if (myLength - anI < aCounter)                                                          \
      {                                                                                            \
        aCurrent = myLast;                                                                         \
        aCounter = myLength - anI;                                                                 \
      }                                                                                            \
      else if (anI > myICur)                                                                       \
        aDir = true;                                                                      \
    }                                                                                              \
    if (aDir)                                                                                      \
      while (aCounter--)                                                                           \
        aCurrent = aCurrent->myNext;                                                               \
    else                                                                                           \
      while (aCounter--)                                                                           \
        aCurrent = aCurrent->myPrev;                                                               \
    (int&)myICur = anI;                                                               \
    (Node*&)myCurrent         = aCurrent;                                                          \
    return aCurrent;                                                                               \
  }                                                                                                \
                                                                                                   \
  ClassName::~ClassName()                                                                          \
  {                                                                                                \
    Clear();                                                                                       \
  }                                                                                                \
                                                                                                   \
  void ClassName::Append(const Type& aVal)                                                         \
  {                                                                                                \
    Node* anItem = new Node(aVal, myLast, NULL);                                                   \
    if (myLength == 0)                                                                             \
      myFirst = anItem;                                                                            \
    else                                                                                           \
      myLast->myNext = anItem;                                                                     \
    myLast = anItem;                                                                               \
    myLength++;                                                                                    \
  }                                                                                                \
                                                                                                   \
  void ClassName::Prepend(const Type& aVal)                                                        \
  {                                                                                                \
    Node* anItem = new Node(aVal, NULL, myFirst);                                                  \
    if (myLength == 0)                                                                             \
      myLast = anItem;                                                                             \
    else                                                                                           \
      myFirst->myPrev = anItem;                                                                    \
    myFirst = anItem;                                                                              \
    myLength++;                                                                                    \
    if (myICur > 0)                                                                                \
      myICur++;                                                                                    \
  }                                                                                                \
                                                                                                   \
  void ClassName::InsertAt(const int anI, const Type& aVal)                           \
  {                                                                                                \
    if (anI <= 1)                                                                                  \
      Prepend(aVal);                                                                               \
    else if (anI > myLength)                                                                       \
      Append(aVal);                                                                                \
    else if (FindItem(anI))                                                                        \
    {                                                                                              \
      Node* anItem      = new Node(aVal, myCurrent->myPrev, myCurrent);                            \
      myCurrent->myPrev = anItem;                                                                  \
      if (anItem->myPrev)                                                                          \
        anItem->myPrev->myNext = anItem;                                                           \
      myLength++;                                                                                  \
      myICur++;                                                                                    \
    }                                                                                              \
  }                                                                                                \
                                                                                                   \
  void ClassName::Clear()                                                                          \
  {                                                                                                \
    while (myFirst)                                                                                \
    {                                                                                              \
      Node* aCurr = myFirst->myNext;                                                               \
      delete myFirst;                                                                              \
      myFirst = aCurr;                                                                             \
    }                                                                                              \
    myFirst = myLast = myCurrent = NULL;                                                           \
    myLength                     = 0;                                                              \
    myICur                       = 0;                                                              \
  }                                                                                                \
                                                                                                   \
  void ClassName::Remove(const int anI)                                               \
  {                                                                                                \
    Node* anItem = (Node*)FindItem(anI);                                                           \
    if (anItem)                                                                                    \
    {                                                                                              \
      if (myCurrent->myPrev)                                                                       \
      {                                                                                            \
        myCurrent->myPrev->myNext = myCurrent->myNext;                                             \
      }                                                                                            \
      if (myCurrent->myNext)                                                                       \
      {                                                                                            \
        myCurrent->myNext->myPrev = myCurrent->myPrev;                                             \
        myCurrent                 = myCurrent->myNext;                                             \
      }                                                                                            \
      else                                                                                         \
      {                                                                                            \
        myCurrent = myCurrent->myPrev;                                                             \
        myICur--;                                                                                  \
      }                                                                                            \
      if (myFirst == anItem)                                                                       \
        myFirst = myFirst->myNext;                                                                 \
      if (myLast == anItem)                                                                        \
        myLast = myLast->myPrev;                                                                   \
      delete anItem;                                                                               \
      myLength--;                                                                                  \
    }                                                                                              \
  }                                                                                                \
                                                                                                   \
  void ClassName::Assign(const ClassName& anOther)                                                 \
  {                                                                                                \
    Clear();                                                                                       \
    if (anOther.Length() == 0)                                                                     \
      return;                                                                                      \
    myFirst         = new Node(anOther.First(), NULL, NULL);                                       \
    Node* aPrevious = myFirst;                                                                     \
    myLength        = 1;                                                                           \
    while (myLength < anOther.Length())                                                            \
    {                                                                                              \
      myLength++;                                                                                  \
      Node* aCurrent = new Node(anOther.Value(myLength), aPrevious, NULL);                         \
      aPrevious = aPrevious->myNext = aCurrent;                                                    \
    }                                                                                              \
    myLast = aPrevious;                                                                            \
  }

#endif
