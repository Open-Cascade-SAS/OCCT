// Created on: 2012-11-13
// Created by: Peter KURNEV
// Copyright (c) 2012 OPEN CASCADE SAS
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

#ifndef IntPolyh_Array_HeaderFile
#define IntPolyh_Array_HeaderFile

#include <NCollection_Vector.hxx>
#include <stdio.h>

/**
* Class IntPolyh_Array (dynamic array of objects)
*
*   1. The Array is dynamic array of objects.
*
*   2. The Array uses NCollection_Vector to store objects
*
*   3. The Array can be created:
*      3.1.  with initial length Nb=0. 
*            In this case Array should be initiated by invoke 
*            the method Init(Nb).
*      3.2.  with initial length Nb>0.
*            In this case Array is initiated automatically.
* 
*      The memory is allocated to store myNbAllocated oblects.
*
*   4. The number of items that are stored in the Array (myNbItems)
*      can be increased by calling the method:  IncrementNbItems().
*      The objects are stored in already allocated memory if it is 
*      possible.
*      Otherwise the new chunk of memory is allocated to store the 
*      objects.
*      The size of chunk <aIncrement> can be defined during the creation
*      of the Array.
*      
*   5. The start index of the Array is 0, The end index of the Array 
*      can be obtained by the method  NbItems();

*   6. The contents of the element with index "i" can be queried or 
*      modified by the methods:  Value(i), ChangeValue(i), operator[](i)
*/

//=======================================================================
// class : IntPolyh_Array
//
//=======================================================================
template <class Type> class IntPolyh_Array {
 public:
  typedef NCollection_Vector <Type> IntPolyh_VectorOfType;
  
  /**
   * Constructor.
   * @param aIncrement
   *   size of memory (in terms of Items) to expand the array
   */
  IntPolyh_Array(const Standard_Integer aIncrement=256) {
    myNbAllocated=0;
    myNbItems=0;
    myIncrement=aIncrement;
  }
  
  /**
   * Constructor.
   * @param aN
   *   size of memory (in terms of Items) to allocate
   * @param aIncrement
   *   size of memory (in terms of Items) to expand the array
   */
  IntPolyh_Array(const Standard_Integer aN,
		 const Standard_Integer aIncrement=256) {
    myNbItems=0;
    myIncrement=aIncrement;
    Init(aN);
  }
  
  /**
   * Assignment operator
   * @param
   *   aOther - the array to copy from 
   * @return
   *   the array
   */
  IntPolyh_Array& operator =(const IntPolyh_Array& aOther) {
    return Copy(aOther);
  }

  /**
   * Copy 
   * @param
   *   aOther - the array to copy from
   * @return
   *   the array 
   */
  IntPolyh_Array& Copy(const IntPolyh_Array& aOther) {
    myVectorOfType.Clear();
    Init(aOther.myNbAllocated);
    myVectorOfType=aOther.myVectorOfType;
    myNbItems=aOther.myNbItems; 
    //
    return *this;
  }

  /**
   * Init - allocate memory for <aN> items  
   * @param
   *   aN - the number of items to allocate the memory
   */
  void Init(const Standard_Integer aN) {
    Type aSL;
    //
    myVectorOfType.SetValue(aN, aSL);
    myNbAllocated=aN;
  }

  /**
   * IncrementNbItems - increment the number of stored items 
   */
  void IncrementNbItems() {
    myNbItems++; 
    if (myNbItems>=myNbAllocated) {
      Standard_Integer aN;
      //
      aN=myNbAllocated+myIncrement;
      Init(aN);
    }
  } 

  /**
   * GetN - returns the number of 'allocated' items  
   * @return
   *   the number of 'allocated' items 
   */
  Standard_Integer GetN() const { 
    return myNbAllocated; 
  }

  /**
   * NbItems - returns the number of stored items  
   * @return
   *   the number of stored items 
   */
  Standard_Integer NbItems() const { 
    return myNbItems; 
  }
  

  /**
   * set the number of stored items  
   * @param aNb
   *   the number of stored items 
   */
  void SetNbItems(const Standard_Integer aNb){ 
    myNbItems=aNb; 
  } 

  /**
   * query the const value
   * @param aIndex
   *   index 
   * @return
   *   the const item
   */
  const Type& Value(const Standard_Integer aIndex) const {
    return myVectorOfType.Value(aIndex);
  }

  /**
   * query the const value
   * @param aIndex
   *   index 
   * @return
   *   the const item
   */
  const Type& operator [](const Standard_Integer aIndex) const {
    return Value(aIndex);
  }

   /**
   * query the value
   * @param aIndex
   *   index 
   * @return
   *   the item
   */
  Type& ChangeValue(const Standard_Integer aIndex)  {
    return myVectorOfType.ChangeValue(aIndex);
  }
  
  /**
   * query the value
   * @param aIndex
   *   index 
   * @return
   *   the item
   */
  Type& operator [](const Standard_Integer aIndex)  {
    return ChangeValue(aIndex);
  }

  /**
   * dump the contents
   */
  void Dump() const   { 
    printf("\n ArrayOfSectionLines 0-> %d",myNbItems-1);
    for(Standard_Integer i=0;i<myNbItems;i++) { 
      (*this)[i].Dump();
    }
    printf("\n");
  }

 protected:
  Standard_Integer myNbAllocated;
  Standard_Integer myNbItems;
  Standard_Integer myIncrement;
  IntPolyh_VectorOfType myVectorOfType;
};

#endif
