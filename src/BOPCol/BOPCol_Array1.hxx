// Created by: Peter KURNEV
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef BOPCol_Array1_HeaderFile
#define BOPCol_Array1_HeaderFile

#include <NCollection_List.hxx>
#include <Standard.hxx>
#include <NCollection_BaseAllocator.hxx>

/**
* Class BOPCol_Array1 (dynamic array of objects)
*
*   1. The Array1 uses the allocator (in terms of NCollection_BaseAllocator)
*      to allocate the memory.
*   2. The memory is allocated as a list of memory blocks. The size of the first
*      block is myStartSize. The size of the other blocks is myIncrement  
*   3  The Array1 is  created with 0 length. 
*   4. The Array1 must be initiated by invoke the method Init(). 
*      Init() allocates the memory block for initial myStartSize elements. 
*   5. The elements can be added by the method Append(theElement). the method 
*      Append(theElement) arranges theElement in 
*       a) previously allocated memory block (if it is possible)     
*       b) new allocated memory block (otherwise). The size of new blocks to 
*          allocate can be set [once only] by the method SetIncrement(theSize). 
*   6. The contents of the element with index "i" can be queried or modified 
*      by the methods  
*      SetValue(i, theElement), Value(i), ChangeValue(i), operator()(i)
*/

//=======================================================================
//class : MemBlock
//
//=======================================================================
template <class Type> class BOPCol_MemBlock {
 
 public:
  // Ctor
  BOPCol_MemBlock(const Handle(NCollection_BaseAllocator)& theAllocator) {
    myI1=0;
    myI2=0;
    mySize=0;
    myData=NULL;
    myAllocator=theAllocator;
    myNext=NULL;
  }
  // ~
  ~BOPCol_MemBlock(){
    Clear();
  }
  // Clear
  void Clear() {
    if (myData) {
      //Standard_Integer i;
      Standard_Size i;
      //
      for (i=0; i<mySize; ++i) {
        myData[i].~Type();
      }
      myAllocator->Free((Standard_Address&)myData); 
      //
      myData=NULL;
      mySize=0;
      myNext=NULL;
    }
  }
  // Allocate
  void Allocate(const Standard_Size theSize) { 
    //Standard_Integer i;
    Standard_Size i;
    //
    Clear();
    mySize=theSize;
    myData=(Type*)myAllocator->Allocate(theSize*sizeof(Type));
    for (i=0; i<mySize; ++i) {
#ifdef BOPCol_Array1_Use_Allocator
      new (myData+i) Type (myAllocator);
#else
      new (myData+i) Type;
#endif
    }
  }
  // SetRange
  void SetRange(const Standard_Integer theI1,
                const Standard_Integer theI2) {
    myI1=theI1;
    myI2=theI2;
  }
  // Range
  void Range(Standard_Integer& theI1,
             Standard_Integer& theI2) const {
    theI1=myI1;
    theI2=myI2;
  }
  // Contains
  Standard_Boolean Contains(const Standard_Integer theIndex)const {
    return (theIndex>=myI1 && theIndex<=myI2);
  }
  // SetValue
  void SetValue(const Standard_Integer theIndex,
                const Type& theValue) {
    *(myData+theIndex-myI1)=theValue;
  }
  // Value
  const Type& Value(const Standard_Integer theIndex)const {
    return *(myData+theIndex-myI1);
  }
  // ChangeValue
  Type& ChangeValue(const Standard_Integer theIndex) {
    return *(myData+theIndex-myI1);
  }
  // SetNext
  void SetNext(BOPCol_MemBlock<Type>* pNext) {
    myNext=pNext;
  }
  // Next
  BOPCol_MemBlock<Type>* Next() {
    return myNext;
  }
  //
 public:
  BOPCol_MemBlock<Type>* myNext;
 protected:
  Standard_Integer myI1;
  Standard_Integer myI2;
  Standard_Size mySize;
  Type *myData;
  Handle(NCollection_BaseAllocator) myAllocator;
};

//=======================================================================
//class  : BOPCol_Array1
//
//=======================================================================
template <class Type> class BOPCol_Array1 {

 public:
  typedef BOPCol_MemBlock<Type>*  BOPCol_PMemBlock;
  typedef BOPCol_MemBlock<Type>   BOPCol_XMemBlock;
  //=========
  // Ctor
  BOPCol_Array1(const Handle(NCollection_BaseAllocator)& theAllocator=0L) {
    myStartSize=32;
    myIncrement=8;
    myExtent=0;
    myPBlock=NULL;
    myPBlock1=NULL;
    //
    if (theAllocator.IsNull()) {
      myAllocator = NCollection_BaseAllocator::CommonBaseAllocator();
    }
    else {
      myAllocator=theAllocator;
    }
  }
  // ~
  ~BOPCol_Array1() {
    Clear();
  }
  //===========
  // Clear
  void Clear() {
    BOPCol_PMemBlock pBlock, pNext;
    //
    pBlock=myPBlock1;
    while(pBlock) {
      pNext=pBlock->Next();
      //
      //pBlock->~BOPCol_MemBlock<Type> ();
      pBlock->~BOPCol_XMemBlock();
      //pBlock->Clear();
      myAllocator->Free((Standard_Address&)pBlock);
      //
      pBlock=pNext;
    }
    myPBlock1=NULL;
    myPBlock=NULL;
    myExtent=0;
  }
  // SetStartSize
  void SetStartSize(const Standard_Integer theSize) {
    if (theSize>0) {
      myStartSize=theSize;
    }
  }
  // SetIncrement
  void SetIncrement(const Standard_Integer theSize){
    if (theSize>0) {
      myIncrement=theSize;
    }
  }
  // Increment
  Standard_Integer Increment()const {
    return myIncrement;
  }
  // Extent
  Standard_Integer Extent()const {
    return myExtent;
  }
  // Size
  Standard_Integer Size()const {
    return myExtent;
  }
  // Size
  Standard_Integer Length()const {
    return myExtent;
  }
  // Init
  void Init();
  // Append(const Type& theValue)
  Standard_Integer Append(const Type& theValue);
  // Append()
  Standard_Integer Append();
  // SetValue
  void SetValue(const Standard_Integer theIndex,
                const Type& theValue);
  // Value
  const Type& operator()(const Standard_Integer theIndex)const;
  //
  const Type& Value(const Standard_Integer theIndex)const;
  // ChangeValue
  Type& operator()(const Standard_Integer theIndex);
  //
  Type& ChangeValue(const Standard_Integer theIndex);
 //
 // protected
 protected:
  //===========
  //FindBlock
  Standard_Boolean FindBlock(const Standard_Integer theIndex)const ;
  //Copy ctor
  BOPCol_Array1(const BOPCol_Array1&);
  //Assign operator
  BOPCol_Array1& operator =(const BOPCol_Array1& theOther);
  //==========
  // fields
 protected:
  Standard_Integer myStartSize;
  Standard_Integer myIncrement;
  Standard_Integer myExtent;
  Type myDfltItem;
  BOPCol_MemBlock<Type> * myPBlock; 
  BOPCol_MemBlock<Type> * myPBlock1; 
  Handle(NCollection_BaseAllocator) myAllocator;
};
//
//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
template <class Type> 
  void BOPCol_Array1<Type>::Init() {
    BOPCol_PMemBlock pBlock;
    //
    pBlock=(BOPCol_PMemBlock)myAllocator->Allocate(sizeof(BOPCol_MemBlock<Type>));
    new (pBlock) BOPCol_MemBlock<Type>(myAllocator);
    //
    pBlock->Allocate(myStartSize);
    pBlock->SetRange(0, myStartSize-1);
    myPBlock1=pBlock;
    myPBlock=pBlock;
  }
//=======================================================================
//function : Append
//purpose  : 
//=======================================================================
template <class Type> 
  Standard_Integer BOPCol_Array1<Type>::Append(const Type& theValue) {
    BOPCol_PMemBlock pBlock, pNext;
    //
    pBlock=myPBlock1;
    for(;;) {
      pNext=pBlock->myNext;
      if(!pNext) {
        myPBlock=pBlock;
        break;
      }
      pBlock=pNext;
    }
    //
    if (!myPBlock->Contains(myExtent)) {
      //
      pBlock=(BOPCol_PMemBlock)myAllocator->Allocate(sizeof(BOPCol_MemBlock<Type>));
      new (pBlock) BOPCol_MemBlock<Type>(myAllocator);
      //
      pBlock->Allocate(myIncrement);
      pBlock->SetRange(myExtent, myExtent+myIncrement-1);
      //
      myPBlock->myNext=pBlock;
      //
      myPBlock=pBlock;
    }
    //
    myPBlock->SetValue(myExtent, theValue);
    //
    ++myExtent;
    //
    return myExtent;
  }
//=======================================================================
//function : Append
//purpose  : 
//=======================================================================
template <class Type> 
  Standard_Integer BOPCol_Array1<Type>::Append() {
    BOPCol_PMemBlock pBlock, pNext;
    //
    pBlock=myPBlock1;
    for(;;) {
      pNext=pBlock->myNext;
      if(!pNext) {
        myPBlock=pBlock;
        break;
      }
      pBlock=pNext;
    }
    //
    if (!myPBlock->Contains(myExtent)) {
      //
      pBlock=(BOPCol_PMemBlock)myAllocator->Allocate(sizeof(BOPCol_MemBlock<Type>));
      new (pBlock) BOPCol_MemBlock<Type>(myAllocator);
      //
      pBlock->Allocate(myIncrement);
      pBlock->SetRange(myExtent, myExtent+myIncrement-1);
      //
      myPBlock->myNext=pBlock;
      //
      myPBlock=pBlock;
    }
    ++myExtent;
    //
    return myExtent;
  }
//=======================================================================
//function : SetValue
//purpose  : 
//=======================================================================
template <class Type> 
  void BOPCol_Array1<Type>::SetValue(const Standard_Integer theIndex,
                                     const Type& theValue) {
    if (FindBlock(theIndex)) {
      myPBlock->SetValue(theIndex, theValue);
    }
  }
//=======================================================================
//function : operator()
//purpose  : 
//=======================================================================
template <class Type> 
  const Type& BOPCol_Array1<Type>::operator()(const Standard_Integer theIndex)const {
    if (FindBlock(theIndex)) {
      return myPBlock->Value(theIndex);
    }
    return myDfltItem;
  }
//=======================================================================
//function : Value
//purpose  : 
//=======================================================================
template <class Type> 
  const Type& BOPCol_Array1<Type>::Value(const Standard_Integer theIndex)const {
    return operator()(theIndex);
  }
//=======================================================================
//function : operator
//purpose  : 
//=======================================================================
template <class Type> 
  Type& BOPCol_Array1<Type>::operator()(const Standard_Integer theIndex) {
    if (FindBlock(theIndex)) {
      return myPBlock->ChangeValue(theIndex);
    }
    return myDfltItem;
  }
//=======================================================================
//function : ChangeValue
//purpose  : 
//=======================================================================
template <class Type> 
  Type& BOPCol_Array1<Type>::ChangeValue(const Standard_Integer theIndex) {
    return operator()(theIndex);
  }
//=======================================================================
//function : FindBlock
//purpose  : 
//=======================================================================
template <class Type> 
  Standard_Boolean BOPCol_Array1<Type>::FindBlock(const Standard_Integer theIndex)const {
    Standard_Boolean bRet;
    //
    bRet=myPBlock->Contains(theIndex);
    if(bRet) {
      return bRet;
    }
    //
    Standard_Integer i1, i2;
    BOPCol_PMemBlock pBlock, *xPBlock;
    //
    xPBlock=(BOPCol_PMemBlock*)&myPBlock;
    pBlock=myPBlock1;
    //
    i1=0;
    i2=myStartSize-1;
    do {
      if (theIndex>=i1 && theIndex<=i2) {
        *xPBlock=pBlock;
        return !bRet;
      }
      //
      i1=i2+1;
      i2=i1+myIncrement-1;
      pBlock=pBlock->myNext;
    } while(pBlock);
    //
    return bRet;
  }

#endif
