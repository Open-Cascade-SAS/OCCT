// Created on: 2002-04-30
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

// Purpose:     To test all methods of all NCollection classes

#ifndef QANCollection_FuncTest_HeaderFile
#define QANCollection_FuncTest_HeaderFile

#include <QANCollection_Common2.hxx>

// ===================== Test methods for each type ===========================
// Parameters of funcs below must be empty collections

// Standard_EXPORT void TestArray1  (QANCollection_Array1&     theA1);
// Standard_EXPORT void TestArray2  (QANCollection_Array2&     theA2);
#include <QANCollection_FuncArrays.hxx>
// Standard_EXPORT void TestMap     (QANCollection_Map&        theM );
// Standard_EXPORT void TestDataMap (QANCollection_DataMap&    theDM);
// Standard_EXPORT void TestDblMap  (QANCollection_DoubleMap&  theBM);
// Standard_EXPORT void TestIndyMap (QANCollection_IndexedMap& theIM);
// Standard_EXPORT void TestInDaMap (QANCollection_IDMap&      theNM);
// Standard_EXPORT void TestInDaMap (QANCollection_IDMap&      theNM);
#include <QANCollection_FuncMaps.hxx>
// Standard_EXPORT void TestList    (QANCollection_List&       theLi);
// Standard_EXPORT void TestQueue   (QANCollection_Queue&      theQ);
// Standard_EXPORT void TestStack   (QANCollection_Stack&      theSt);
// Standard_EXPORT void TestSet     (QANCollection_Set&        theSe);
// Standard_EXPORT void TestSList   (QANCollection_SList&      theSL);
// Standard_EXPORT void TestSequence(QANCollection_Sequence&   theSq);
#include <QANCollection_FuncLists.hxx>

#endif
