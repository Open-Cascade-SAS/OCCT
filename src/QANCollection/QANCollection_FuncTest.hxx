// File:        QANCollection_FuncTest.hxx
// Created:     Tue Apr 30 09:44:26 2002
// Author:      Alexander KARTOMIN (akm)
//              <a-kartomin@opencascade.com>
//
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
