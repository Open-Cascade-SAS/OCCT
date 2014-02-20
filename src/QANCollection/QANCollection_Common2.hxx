// Created on: 2002-05-15
// Created by: Alexander KARTOMIN (akm)
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef QANCollection_Common2_HeaderFile
#define QANCollection_Common2_HeaderFile

#include <stdio.h>
#include <gp_Pnt.hxx>

#include <QANCollection_Common.hxx>

// =====================               INSTANTIATIONS               ===========
// ===================== The Types must be defined before this line ===========
// These are: TheItemType, TheKey1Type, TheKey2Type
// So must be defined ::HashCode and ::IsEqual too

#if defined(ItemType) && defined(Key1Type) && defined(Key2Type)

#include <NCollection_DefineBaseCollection.hxx>
////////////////////////////////DEFINE_BASECOLLECTION(QANCollection_BaseCol,ItemType)
////////////////////////////////DEFINE_BASECOLLECTION(QANCollection_Key1BaseCol,Key1Type)
////////////////////////////////DEFINE_BASECOLLECTION(QANCollection_Key2BaseCol,Key2Type)
DEFINE_BASECOLLECTION(QANCollection_BaseColFunc,ItemType)
DEFINE_BASECOLLECTION(QANCollection_Key1BaseColFunc,Key1Type)
DEFINE_BASECOLLECTION(QANCollection_Key2BaseColFunc,Key2Type)

#include <NCollection_DefineArray1.hxx>
#include <NCollection_DefineHArray1.hxx>
////////////////////////////////DEFINE_ARRAY1(QANCollection_Array1,QANCollection_BaseCol,ItemType)
////////////////////////////////DEFINE_HARRAY1(QANCollection_HArray1,QANCollection_Array1)
DEFINE_ARRAY1(QANCollection_Array1Func,QANCollection_BaseColFunc,ItemType)
DEFINE_HARRAY1(QANCollection_HArray1Func,QANCollection_Array1Func)

#include <NCollection_DefineArray2.hxx>
#include <NCollection_DefineHArray2.hxx>
////////////////////////////////DEFINE_ARRAY2(QANCollection_Array2,QANCollection_BaseCol,ItemType)
////////////////////////////////DEFINE_HARRAY2(QANCollection_HArray2,QANCollection_Array2)
DEFINE_ARRAY2(QANCollection_Array2Func,QANCollection_BaseColFunc,ItemType)
DEFINE_HARRAY2(QANCollection_HArray2Func,QANCollection_Array2Func)

#include <NCollection_DefineMap.hxx>
#include <NCollection_DefineDataMap.hxx>
#include <NCollection_DefineDoubleMap.hxx>
#include <NCollection_DefineIndexedMap.hxx>
#include <NCollection_DefineIndexedDataMap.hxx>
////////////////////////////////DEFINE_MAP(QANCollection_Map,QANCollection_Key1BaseCol,Key1Type)
////////////////////////////////DEFINE_DATAMAP(QANCollection_DataMap,QANCollection_BaseCol,Key1Type,ItemType)
////////////////////////////////DEFINE_DOUBLEMAP(QANCollection_DoubleMap,QANCollection_Key2BaseCol,Key1Type,Key2Type)
////////////////////////////////DEFINE_INDEXEDMAP(QANCollection_IndexedMap,QANCollection_Key1BaseCol,Key1Type)
////////////////////////////////DEFINE_INDEXEDDATAMAP(QANCollection_IDMap,QANCollection_BaseCol,Key1Type,ItemType)
DEFINE_MAP(QANCollection_MapFunc,QANCollection_Key1BaseColFunc,Key1Type)
DEFINE_DATAMAP(QANCollection_DataMapFunc,QANCollection_BaseColFunc,Key1Type,ItemType)
DEFINE_DOUBLEMAP(QANCollection_DoubleMapFunc,QANCollection_Key2BaseColFunc,Key1Type,Key2Type)
DEFINE_INDEXEDMAP(QANCollection_IndexedMapFunc,QANCollection_Key1BaseColFunc,Key1Type)
DEFINE_INDEXEDDATAMAP(QANCollection_IDMapFunc,QANCollection_BaseColFunc,Key1Type,ItemType)

#include <NCollection_DefineList.hxx>
#include <NCollection_DefineQueue.hxx>
#include <NCollection_DefineStack.hxx>
#include <NCollection_DefineSet.hxx>
#include <NCollection_DefineHSet.hxx>
////////////////////////////////DEFINE_LIST(QANCollection_List,QANCollection_BaseCol,ItemType)
////////////////////////////////DEFINE_QUEUE(QANCollection_Queue,QANCollection_BaseCol,ItemType)
////////////////////////////////DEFINE_STACK(QANCollection_Stack,QANCollection_BaseCol,ItemType)
////////////////////////////////DEFINE_SET(QANCollection_Set,QANCollection_Key2BaseCol,Key2Type)
////////////////////////////////DEFINE_HSET(QANCollection_HSet,QANCollection_Set)
DEFINE_LIST(QANCollection_ListFunc,QANCollection_BaseColFunc,ItemType)
DEFINE_QUEUE(QANCollection_QueueFunc,QANCollection_BaseColFunc,ItemType)
DEFINE_STACK(QANCollection_StackFunc,QANCollection_BaseColFunc,ItemType)
DEFINE_SET(QANCollection_SetFunc,QANCollection_Key2BaseColFunc,Key2Type)
DEFINE_HSET(QANCollection_HSetFunc,QANCollection_SetFunc)

#include <NCollection_DefineSList.hxx>
////////////////////////////////DEFINE_SLIST(QANCollection_SList,QANCollection_BaseCol,ItemType)
DEFINE_SLIST(QANCollection_SListFunc,QANCollection_BaseColFunc,ItemType)

#include <NCollection_DefineSequence.hxx>
#include <NCollection_DefineHSequence.hxx>
////////////////////////////////DEFINE_SEQUENCE(QANCollection_Sequence,QANCollection_BaseCol,ItemType)
////////////////////////////////DEFINE_HSEQUENCE(QANCollection_HSequence,QANCollection_Sequence)
DEFINE_SEQUENCE(QANCollection_SequenceFunc,QANCollection_BaseColFunc,ItemType)
DEFINE_HSEQUENCE(QANCollection_HSequenceFunc,QANCollection_SequenceFunc)


// ===================== Type independent methods =============================
////////////////////////////////void printCollection (QANCollection_BaseCol& aColl, 
void printCollection (QANCollection_BaseColFunc& aColl, 
                      const char * str)
{
  printf ("%s:\n",str);
  Standard_Integer iSize = aColl.Size();
  ////////////////////////////////QANCollection_BaseCol::Iterator& anIter = aColl.CreateIterator();
  QANCollection_BaseColFunc::Iterator& anIter = aColl.CreateIterator();
  if (!anIter.More())
  {
    if (iSize==0)
      printf ("   <Empty collection>\n");
    else
      printf ("Error   : empty collection has size==%d",iSize);
  }
  else
  {
    printf ("   Size==%d\n",iSize);
    for (; anIter.More(); anIter.Next())
      PrintItem(anIter.Value());
  }
}

////////////////////////////////void printCollection (QANCollection_Key1BaseCol& aColl, 
#if 0
void printCollection (QANCollection_Key1BaseColFunc& aColl, 
                      const char * str)
{
  printf ("%s:\n",str);
  Standard_Integer iSize = aColl.Size();
  ////////////////////////////////QANCollection_Key1BaseCol::Iterator& anIter = aColl.CreateIterator();
  QANCollection_Key1BaseColFunc::Iterator& anIter = aColl.CreateIterator();
  if (!anIter.More())
  {
    if (iSize==0)
      printf ("   <Empty collection>\n");
    else
      printf ("Error   : empty collection has size==%d",iSize);
  }
  else
  {
    printf ("   Size==%d\n",iSize);
    for (; anIter.More(); anIter.Next())
      PrintItem(anIter.Value());
  }
}
#endif

////////////////////////////////void printCollection (QANCollection_Key2BaseCol& aColl, 
void printCollection (QANCollection_Key2BaseColFunc& aColl, 
                      const char * str)
{
  printf ("%s:\n",str);
  Standard_Integer iSize = aColl.Size();
  ////////////////////////////////QANCollection_Key2BaseCol::Iterator& anIter = aColl.CreateIterator();
  QANCollection_Key2BaseColFunc::Iterator& anIter = aColl.CreateIterator();
  if (!anIter.More())
  {
    if (iSize==0)
      printf ("   <Empty collection>\n");
    else
      printf ("Error   : empty collection has size==%d",iSize);
  }
  else
  {
    printf ("   Size==%d\n",iSize);
    for (; anIter.More(); anIter.Next())
      PrintItem(anIter.Value());
  }
}

////////////////////////////////void AssignCollection (QANCollection_BaseCol& aCollSrc,
////////////////////////////////                       QANCollection_BaseCol& aCollDst)
void AssignCollection (QANCollection_BaseColFunc& aCollSrc,
                       QANCollection_BaseColFunc& aCollDst)
{
  printCollection (aCollSrc,"Source collection");
  aCollDst.Assign(aCollSrc);
  printCollection (aCollDst,"Target collection");
}

////////////////////////////////void AssignCollection (QANCollection_Key1BaseCol& aCollSrc,
////////////////////////////////                       QANCollection_Key1BaseCol& aCollDst)
#if 0
void AssignCollection (QANCollection_Key1BaseColFunc& aCollSrc,
                       QANCollection_Key1BaseColFunc& aCollDst)
{
  printCollection (aCollSrc,"Source collection");
  aCollDst.Assign(aCollSrc);
  printCollection (aCollDst,"Target collection");
}
#endif

////////////////////////////////void AssignCollection (QANCollection_Key2BaseCol& aCollSrc,
////////////////////////////////                       QANCollection_Key2BaseCol& aCollDst)
void AssignCollection (QANCollection_Key2BaseColFunc& aCollSrc,
                       QANCollection_Key2BaseColFunc& aCollDst)
{
  printCollection (aCollSrc,"Source collection");
  aCollDst.Assign(aCollSrc);
  printCollection (aCollDst,"Target collection");
}

#endif

#endif
