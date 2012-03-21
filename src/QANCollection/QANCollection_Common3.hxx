// Created on: 2002-05-15
// Created by: Alexander KARTOMIN (akm)
// Copyright (c) 2002-2012 OPEN CASCADE SAS
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



#ifndef QANCollection_Common3_HeaderFile
#define QANCollection_Common3_HeaderFile

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
DEFINE_BASECOLLECTION(QANCollection_BaseColPerf,ItemType)
DEFINE_BASECOLLECTION(QANCollection_Key1BaseColPerf,Key1Type)
DEFINE_BASECOLLECTION(QANCollection_Key2BaseColPerf,Key2Type)

#include <NCollection_DefineArray1.hxx>
#include <NCollection_DefineHArray1.hxx>
////////////////////////////////DEFINE_ARRAY1(QANCollection_Array1,QANCollection_BaseCol,ItemType)
////////////////////////////////DEFINE_HARRAY1(QANCollection_HArray1,QANCollection_Array1)
DEFINE_ARRAY1(QANCollection_Array1Perf,QANCollection_BaseColPerf,ItemType)
DEFINE_HARRAY1(QANCollection_HArray1Perf,QANCollection_Array1Perf)

#include <NCollection_DefineArray2.hxx>
#include <NCollection_DefineHArray2.hxx>
////////////////////////////////DEFINE_ARRAY2(QANCollection_Array2,QANCollection_BaseCol,ItemType)
////////////////////////////////DEFINE_HARRAY2(QANCollection_HArray2,QANCollection_Array2)
DEFINE_ARRAY2(QANCollection_Array2Perf,QANCollection_BaseColPerf,ItemType)
DEFINE_HARRAY2(QANCollection_HArray2Perf,QANCollection_Array2Perf)

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
DEFINE_MAP(QANCollection_MapPerf,QANCollection_Key1BaseColPerf,Key1Type)
DEFINE_DATAMAP(QANCollection_DataMapPerf,QANCollection_BaseColPerf,Key1Type,ItemType)
DEFINE_DOUBLEMAP(QANCollection_DoubleMapPerf,QANCollection_Key2BaseColPerf,Key1Type,Key2Type)
DEFINE_INDEXEDMAP(QANCollection_IndexedMapPerf,QANCollection_Key1BaseColPerf,Key1Type)
DEFINE_INDEXEDDATAMAP(QANCollection_IDMapPerf,QANCollection_BaseColPerf,Key1Type,ItemType)

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
DEFINE_LIST(QANCollection_ListPerf,QANCollection_BaseColPerf,ItemType)
DEFINE_QUEUE(QANCollection_QueuePerf,QANCollection_BaseColPerf,ItemType)
DEFINE_STACK(QANCollection_StackPerf,QANCollection_BaseColPerf,ItemType)
DEFINE_SET(QANCollection_SetPerf,QANCollection_Key2BaseColPerf,Key2Type)
DEFINE_HSET(QANCollection_HSetPerf,QANCollection_SetPerf)

#include <NCollection_DefineSList.hxx>
////////////////////////////////DEFINE_SLIST(QANCollection_SList,QANCollection_BaseCol,ItemType)
DEFINE_SLIST(QANCollection_SListPerf,QANCollection_BaseColPerf,ItemType)

#include <NCollection_DefineSequence.hxx>
#include <NCollection_DefineHSequence.hxx>
////////////////////////////////DEFINE_SEQUENCE(QANCollection_Sequence,QANCollection_BaseCol,ItemType)
////////////////////////////////DEFINE_HSEQUENCE(QANCollection_HSequence,QANCollection_Sequence)
DEFINE_SEQUENCE(QANCollection_SequencePerf,QANCollection_BaseColPerf,ItemType)
DEFINE_HSEQUENCE(QANCollection_HSequencePerf,QANCollection_SequencePerf)


// ===================== Type independent methods =============================
////////////////////////////////void printCollection (QANCollection_BaseCol& aColl, 
#if 0
void printCollection (QANCollection_BaseColPerf& aColl, 
                      const char * str)
{
  printf ("%s:\n",str);
  Standard_Integer iSize = aColl.Size();
  ////////////////////////////////QANCollection_BaseCol::Iterator& anIter = aColl.CreateIterator();
  QANCollection_BaseColPerf::Iterator& anIter = aColl.CreateIterator();
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
void printCollection (QANCollection_Key1BaseColPerf& aColl, 
                      const char * str)
{
  printf ("%s:\n",str);
  Standard_Integer iSize = aColl.Size();
  ////////////////////////////////QANCollection_Key1BaseCol::Iterator& anIter = aColl.CreateIterator();
  QANCollection_Key1BaseColPerf::Iterator& anIter = aColl.CreateIterator();
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

////////////////////////////////void printCollection (QANCollection_Key2BaseCol& aColl, 
void printCollection (QANCollection_Key2BaseColPerf& aColl, 
                      const char * str)
{
  printf ("%s:\n",str);
  Standard_Integer iSize = aColl.Size();
  ////////////////////////////////QANCollection_Key2BaseCol::Iterator& anIter = aColl.CreateIterator();
  QANCollection_Key2BaseColPerf::Iterator& anIter = aColl.CreateIterator();
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
void AssignCollection (QANCollection_BaseColPerf& aCollSrc,
                       QANCollection_BaseColPerf& aCollDst)
{
  printCollection (aCollSrc,"Source collection");
  aCollDst.Assign(aCollSrc);
  printCollection (aCollDst,"Target collection");
}

////////////////////////////////void AssignCollection (QANCollection_Key1BaseCol& aCollSrc,
////////////////////////////////                       QANCollection_Key1BaseCol& aCollDst)
void AssignCollection (QANCollection_Key1BaseColPerf& aCollSrc,
                       QANCollection_Key1BaseColPerf& aCollDst)
{
  printCollection (aCollSrc,"Source collection");
  aCollDst.Assign(aCollSrc);
  printCollection (aCollDst,"Target collection");
}

////////////////////////////////void AssignCollection (QANCollection_Key2BaseCol& aCollSrc,
////////////////////////////////                       QANCollection_Key2BaseCol& aCollDst)
void AssignCollection (QANCollection_Key2BaseColPerf& aCollSrc,
                       QANCollection_Key2BaseColPerf& aCollDst)
{
  printCollection (aCollSrc,"Source collection");
  aCollDst.Assign(aCollSrc);
  printCollection (aCollDst,"Target collection");
}
#endif

#endif

#endif
