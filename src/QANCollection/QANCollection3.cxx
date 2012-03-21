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

#define ItemType gp_Pnt
#define Key1Type Standard_Real
#define Key2Type Standard_Integer

#include <QANCollection_PerfTest.hxx>

#include <QANCollection_PerfSparseArray.hxx>

IMPLEMENT_HARRAY1(QANCollection_HArray1Perf)
IMPLEMENT_HARRAY2(QANCollection_HArray2Perf)
IMPLEMENT_HSET(QANCollection_HSetPerf)
IMPLEMENT_HSEQUENCE(QANCollection_HSequencePerf)

//=======================================================================
//function : CheckArguments
//purpose  : 
//=======================================================================
Standard_Integer CheckArguments(Draw_Interpretor& di, Standard_Integer argc, const char ** argv, Standard_Integer& Repeat, Standard_Integer& Size)
{
  if ( argc != 3) {
    di << "Usage : " << argv[0] << " Repeat Size" << "\n";
    return 1;
  }
  Repeat = atoi(argv[1]);
  Size   = atoi(argv[2]);
  if ( Repeat < 1 ) {
    di << "Repeat > 0" << "\n";
    return 1;
  }
  if ( Size < 1 ) {
    di << "Size > 0" << "\n";
    return 1;
  }
  return 0;
}


//=======================================================================
//function : QANColPerfArray1
//purpose  : 
//=======================================================================
static Standard_Integer QANColPerfArray1(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  Standard_Integer Repeat, Size;
  if ( CheckArguments(di, argc, argv, Repeat, Size) ) {
    return 1;
  }
  CompArray1(Repeat,Size);
  return 0;
}

//=======================================================================
//function : QANColPerfArray2
//purpose  : 
//=======================================================================
static Standard_Integer QANColPerfArray2(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  Standard_Integer Repeat, Size;
  if ( CheckArguments(di, argc, argv, Repeat, Size) ) {
    return 1;
  }
  CompArray2(Repeat,Size);
  return 0;
}

//=======================================================================
//function : QANColPerfList
//purpose  : 
//=======================================================================
static Standard_Integer QANColPerfList(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  Standard_Integer Repeat, Size;
  if ( CheckArguments(di, argc, argv, Repeat, Size) ) {
    return 1;
  }
  CompList(Repeat,Size);
  return 0;
}

//=======================================================================
//function : QANColPerfQueue
//purpose  : 
//=======================================================================
static Standard_Integer QANColPerfQueue(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  Standard_Integer Repeat, Size;
  if ( CheckArguments(di, argc, argv, Repeat, Size) ) {
    return 1;
  }
  CompQueue(Repeat,Size);
  return 0;
}

//=======================================================================
//function : QANColPerfStack
//purpose  : 
//=======================================================================
static Standard_Integer QANColPerfStack(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  Standard_Integer Repeat, Size;
  if ( CheckArguments(di, argc, argv, Repeat, Size) ) {
    return 1;
  }
  CompStack(Repeat,Size);
  return 0;
}

//=======================================================================
//function : QANColPerfSet
//purpose  : 
//=======================================================================
static Standard_Integer QANColPerfSet(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  Standard_Integer Repeat, Size;
  if ( CheckArguments(di, argc, argv, Repeat, Size) ) {
    return 1;
  }
  CompSet(Repeat,Size);
  return 0;
}

//=======================================================================
//function : QANColPerfSList
//purpose  : 
//=======================================================================
static Standard_Integer QANColPerfSList(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  Standard_Integer Repeat, Size;
  if ( CheckArguments(di, argc, argv, Repeat, Size) ) {
    return 1;
  }
  CompSList(Repeat,Size);
  return 0;
}

//=======================================================================
//function : QANColPerfSequence
//purpose  : 
//=======================================================================
static Standard_Integer QANColPerfSequence(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  Standard_Integer Repeat, Size;
  if ( CheckArguments(di, argc, argv, Repeat, Size) ) {
    return 1;
  }
  CompSequence(Repeat,Size);
  return 0;
}

//=======================================================================
//function : QANColPerfMap
//purpose  : 
//=======================================================================
static Standard_Integer QANColPerfMap(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  Standard_Integer Repeat, Size;
  if ( CheckArguments(di, argc, argv, Repeat, Size) ) {
    return 1;
  }
  CompMap(Repeat,Size);
  return 0;
}

//=======================================================================
//function : QANColPerfDataMap
//purpose  : 
//=======================================================================
static Standard_Integer QANColPerfDataMap(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  Standard_Integer Repeat, Size;
  if ( CheckArguments(di, argc, argv, Repeat, Size) ) {
    return 1;
  }
  CompDataMap(Repeat,Size);
  return 0;
}

//=======================================================================
//function : QANColPerfDoubleMap
//purpose  : 
//=======================================================================
static Standard_Integer QANColPerfDoubleMap(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  Standard_Integer Repeat, Size;
  if ( CheckArguments(di, argc, argv, Repeat, Size) ) {
    return 1;
  }
  CompDoubleMap(Repeat,Size);
  return 0;
}

//=======================================================================
//function : QANColPerfIndexedMap
//purpose  : 
//=======================================================================
static Standard_Integer QANColPerfIndexedMap(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  Standard_Integer Repeat, Size;
  if ( CheckArguments(di, argc, argv, Repeat, Size) ) {
    return 1;
  }
  CompIndexedMap(Repeat,Size);
  return 0;
}

//=======================================================================
//function : QANColPerfIndexedDataMap
//purpose  : 
//=======================================================================
static Standard_Integer QANColPerfIndexedDataMap(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  Standard_Integer Repeat, Size;
  if ( CheckArguments(di, argc, argv, Repeat, Size) ) {
    return 1;
  }
  CompIndexedDataMap(Repeat,Size);
  return 0;
}

//=======================================================================
//function : QANColCheckSparseArray
//purpose  : 
//=======================================================================
static Standard_Integer QANColCheckSparseArray(Draw_Interpretor& di, Standard_Integer argc, const char ** argv)
{
  Standard_Integer Repeat, Size;
  if ( CheckArguments(di, argc, argv, Repeat, Size) ) {
    return 1;
  }
  CompSparseArray(Repeat,Size);
  return 0;
}

void QANCollection::Commands3(Draw_Interpretor& theCommands) {
  char *group = "QANCollection";

  // from agvCollTest/src/CollectionEXE/PerfTestEXE.cxx
  theCommands.Add("QANColPerfArray1",         "QANColPerfArray1 Repeat Size",         __FILE__, QANColPerfArray1,         group);  
  theCommands.Add("QANColPerfArray2",         "QANColPerfArray2 Repeat Size",         __FILE__, QANColPerfArray2,         group);  
  theCommands.Add("QANColPerfList",           "QANColPerfList Repeat Size",           __FILE__, QANColPerfList,           group);  
  theCommands.Add("QANColPerfQueue",          "QANColPerfQueue Repeat Size",          __FILE__, QANColPerfQueue,          group);  
  theCommands.Add("QANColPerfStack",          "QANColPerfStack Repeat Size",          __FILE__, QANColPerfStack,          group);  
  theCommands.Add("QANColPerfSet",            "QANColPerfSet Repeat Size",            __FILE__, QANColPerfSet,            group);  
  theCommands.Add("QANColPerfSList",          "QANColPerfSList Repeat Size",          __FILE__, QANColPerfSList,          group);  
  theCommands.Add("QANColPerfSequence",       "QANColPerfSequence Repeat Size",       __FILE__, QANColPerfSequence,       group);  
  theCommands.Add("QANColPerfMap",            "QANColPerfMap Repeat Size",            __FILE__, QANColPerfMap,            group);  
  theCommands.Add("QANColPerfDataMap",        "QANColPerfDataMap Repeat Size",        __FILE__, QANColPerfDataMap,        group);  
  theCommands.Add("QANColPerfDoubleMap",      "QANColPerfDoubleMap Repeat Size",      __FILE__, QANColPerfDoubleMap,      group);  
  theCommands.Add("QANColPerfIndexedMap",     "QANColPerfIndexedMap Repeat Size",     __FILE__, QANColPerfIndexedMap,     group);  
  theCommands.Add("QANColPerfIndexedDataMap", "QANColPerfIndexedDataMap Repeat Size", __FILE__, QANColPerfIndexedDataMap, group);  
  
  theCommands.Add("QANColCheckSparseArray",   "QANColCheckSparseArray Repeat Size",   __FILE__, QANColCheckSparseArray,   group);
  
  return;
}

