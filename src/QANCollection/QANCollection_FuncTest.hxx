// Created on: 2002-04-30
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
