// Created on: 2004-03-05
// Created by: Mikhail KUZMITCHEV
// Copyright (c) 2004-2012 OPEN CASCADE SAS
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



#include <QANCollection.hxx>
#include <Draw_Interpretor.hxx>

#include <gp_Pnt.hxx>
// HashCode and IsEquel must be defined for key types of maps

Standard_Integer HashCode(const gp_Pnt thePnt, 
                          int theUpper)
{
  return HashCode(thePnt.X(),theUpper);
}

Standard_Boolean IsEqual(const gp_Pnt& theP1, const gp_Pnt& theP2)
{
  return theP1.IsEqual(theP2,gp::Resolution());
}

#define ItemType gp_Pnt
#define Key1Type gp_Pnt
#define Key2Type Standard_Real

#include <QANCollection_FuncTest.hxx>

IMPLEMENT_HARRAY1(QANCollection_HArray1Func)
IMPLEMENT_HARRAY2(QANCollection_HArray2Func)
IMPLEMENT_HSET(QANCollection_HSetFunc)
IMPLEMENT_HSEQUENCE(QANCollection_HSequenceFunc)

//=======================================================================
//function : CheckArguments1
//purpose  : 
//=======================================================================
Standard_Integer CheckArguments1(Draw_Interpretor& di, Standard_Integer argc, const char ** argv, Standard_Integer& Lower, Standard_Integer& Upper)
{
  if ( argc != 3) {
    di << "Usage : " << argv[0] << " Lower Upper" << "\n";
    return 1;
  }
  Lower = atoi(argv[1]);
  Upper = atoi(argv[2]);
  if ( Lower > Upper ) {
    di << "Lower > Upper" << "\n";
    return 1;
  }
  return 0;
}

//=======================================================================
//function : CheckArguments2
//purpose  : 
//=======================================================================
Standard_Integer CheckArguments2(Draw_Interpretor& di, Standard_Integer argc, const char ** argv, Standard_Integer& LowerRow, Standard_Integer& UpperRow, Standard_Integer& LowerCol, Standard_Integer& UpperCol)
{
  if ( argc != 5) {
    di << "Usage : " << argv[0] << " LowerRow UpperRow LowerCol UpperCol" << "\n";
    return 1;
  }
  LowerRow = atoi(argv[1]);
  UpperRow = atoi(argv[2]);
  LowerCol = atoi(argv[3]);
  UpperCol = atoi(argv[4]);
  if ( LowerRow > UpperRow ) {
    di << "LowerRow > UpperRow" << "\n";
    return 1;
  }
  if ( LowerCol > UpperCol ) {
    di << "LowerCol UpperCol> " << "\n";
    return 1;
  }
  return 0;
}


//=======================================================================
//function : QANColTestArray1
//purpose  : 
//=======================================================================
static Standard_Integer QANColTestArray1(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  Standard_Integer Lower, Upper;
  if ( CheckArguments1(di, argc, argv, Lower, Upper) ) {
    return 1;
  }
  QANCollection_Array1Func anArr1(Lower, Upper);
  TestArray1(anArr1);
  return 0;
}

//=======================================================================
//function : QANColTestArray2
//purpose  : 
//=======================================================================
static Standard_Integer QANColTestArray2(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  Standard_Integer LowerRow, UpperRow, LowerCol, UpperCol;
  if ( CheckArguments2(di, argc, argv, LowerRow, UpperRow, LowerCol, UpperCol) ) {
    return 1;
  }
  QANCollection_Array2Func anArr2(LowerRow, UpperRow, LowerCol, UpperCol);
  TestArray2(anArr2);
  return 0;
}

//=======================================================================
//function : QANColTestMap
//purpose  : 
//=======================================================================
static Standard_Integer QANColTestMap(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 1) {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  QANCollection_MapFunc aMap;
  TestMap(aMap);
  return 0;
}

//=======================================================================
//function : QANColTestDataMap
//purpose  : 
//=======================================================================
static Standard_Integer QANColTestDataMap(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 1) {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  QANCollection_DataMapFunc aDataMap;
  TestDataMap(aDataMap);
  return 0;
}

//=======================================================================
//function : QANColTestDoubleMap
//purpose  : 
//=======================================================================
static Standard_Integer QANColTestDoubleMap(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 1) {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  QANCollection_DoubleMapFunc aDoubleMap;
  TestDoubleMap(aDoubleMap);
  return 0;
}

//=======================================================================
//function : QANColTestIndexedMap
//purpose  : 
//=======================================================================
static Standard_Integer QANColTestIndexedMap(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 1) {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  QANCollection_IndexedMapFunc aIndexedMap;
  TestIndexedMap(aIndexedMap);
  return 0;
}

//=======================================================================
//function : QANColTestIndexedDataMap
//purpose  : 
//=======================================================================
static Standard_Integer QANColTestIndexedDataMap(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 1) {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  QANCollection_IDMapFunc aIDMap;
  TestIndexedDataMap(aIDMap);
  return 0;
}

//=======================================================================
//function : QANColTestList
//purpose  : 
//=======================================================================
static Standard_Integer QANColTestList(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 1) {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  QANCollection_ListFunc aList;
  TestList(aList);
  return 0;
}

//=======================================================================
//function : QANColTestQueue
//purpose  : 
//=======================================================================
static Standard_Integer QANColTestQueue(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 1) {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  QANCollection_QueueFunc aQueue;
  TestQueue(aQueue);
  return 0;
}

//=======================================================================
//function : QANColTestStack
//purpose  : 
//=======================================================================
static Standard_Integer QANColTestStack(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 1) {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  QANCollection_StackFunc aStack;
  TestStack(aStack);
  return 0;
}

//=======================================================================
//function : QANColTestSet
//purpose  : 
//=======================================================================
static Standard_Integer QANColTestSet(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 1) {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  QANCollection_SetFunc aSet;
  TestSet(aSet);
  return 0;
}

//=======================================================================
//function : QANColTestSList
//purpose  : 
//=======================================================================
static Standard_Integer QANColTestSList(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 1) {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  QANCollection_SListFunc aSList;
  TestSList(aSList);
  return 0;
}

//=======================================================================
//function : QANColTestSequence
//purpose  : 
//=======================================================================
static Standard_Integer QANColTestSequence(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  if ( argc != 1) {
    di << "Usage : " << argv[0] << "\n";
    return 1;
  }
  QANCollection_SequenceFunc aSeq;
  TestSequence(aSeq);
  return 0;
}

void QANCollection::Commands2(Draw_Interpretor& theCommands) {
  const char *group = "QANCollection";

  // from agvCollTest/src/CollectionEXE/FuncTestEXE.cxx
  theCommands.Add("QANColTestArray1",         "QANColTestArray1",         __FILE__, QANColTestArray1,         group);  
  theCommands.Add("QANColTestArray2",         "QANColTestArray2",         __FILE__, QANColTestArray2,         group);  
  theCommands.Add("QANColTestMap",            "QANColTestMap",            __FILE__, QANColTestMap,            group);  
  theCommands.Add("QANColTestDataMap",        "QANColTestDataMap",        __FILE__, QANColTestDataMap,        group);  
  theCommands.Add("QANColTestDoubleMap",      "QANColTestDoubleMap",      __FILE__, QANColTestDoubleMap,      group);  
  theCommands.Add("QANColTestIndexedMap",     "QANColTestIndexedMap",     __FILE__, QANColTestIndexedMap,     group);  
  theCommands.Add("QANColTestIndexedDataMap", "QANColTestIndexedDataMap", __FILE__, QANColTestIndexedDataMap, group);  
  theCommands.Add("QANColTestList",           "QANColTestList",           __FILE__, QANColTestList,           group);  
  theCommands.Add("QANColTestQueue",          "QANColTestQueue",          __FILE__, QANColTestQueue,          group);  
  theCommands.Add("QANColTestStack",          "QANColTestStack",          __FILE__, QANColTestStack,          group);  
  theCommands.Add("QANColTestSet",            "QANColTestSet",            __FILE__, QANColTestSet,            group);  
  theCommands.Add("QANColTestSList",          "QANColTestSList",          __FILE__, QANColTestSList,          group);  
  theCommands.Add("QANColTestSequence",       "QANColTestSequence",       __FILE__, QANColTestSequence,       group);  

  return;
}
