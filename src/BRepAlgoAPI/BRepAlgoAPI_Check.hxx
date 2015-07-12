// Created on: 2012-12-17
// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2012-2014 OPEN CASCADE SAS
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

#ifndef _BRepAlgoAPI_Check_HeaderFile
#define _BRepAlgoAPI_Check_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>
#include <BOPAlgo_PArgumentAnalyzer.hxx>
#include <BOPAlgo_ListOfCheckResult.hxx>
#include <Standard_Real.hxx>
#include <BRepAlgoAPI_Algo.hxx>
#include <Standard_Boolean.hxx>
#include <BOPAlgo_Operation.hxx>
class TopoDS_Shape;



//! The class Check provides a diagnostic tool for checking
//! single shape or couple of shapes.
//! Single shape is checking on topological validity, small edges
//! and self-interference. For couple of shapes added check
//! on validity for boolean operation of given type.
//!
//! The class provides two ways of checking shape(-s)
//! 1. Constructors
//! BRepAlgoAPI_Check aCh(theS);
//! Standard_Boolean bV=aCh.IsValid();
//! 2. Methods SetData and Perform
//! BRepAlgoAPI_Check aCh;
//! aCh.SetData(theS1, theS2, BOPAlgo_FUSE, Standard_False);
//! aCh.Perform();
//! Standard_Boolean bV=aCh.IsValid();
class BRepAlgoAPI_Check  : public BRepAlgoAPI_Algo
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor.
  Standard_EXPORT BRepAlgoAPI_Check();
Standard_EXPORT virtual ~BRepAlgoAPI_Check();
  
  //! Constructor for checking single shape.
  //! It calls methods
  //! Init(theS, TopoDS_Shape(), BOPAlgo_UNKNOWN, bTestSE, bTestSI)
  //! and Perform().
  //! Params:
  //! theS   - the shape that should be checked;
  //! bTestSE - flag that specifies whether check on small edges
  //! should be performed; by default it is set to TRUE;
  //! bTestSI - flag that specifies whether check on self-interference
  //! should be performed; by default it is set to TRUE;
  Standard_EXPORT BRepAlgoAPI_Check(const TopoDS_Shape& theS, const Standard_Boolean bTestSE = Standard_True, const Standard_Boolean bTestSI = Standard_True);
  
  //! Constructor for couple of shapes.
  //! It calls methods
  //! Init(theS1, theS2, theOp, bTestSE, bTestSI) and Perform().
  //! Params:
  //! theS1, theS2 - the initial shapes.
  //! theOp - the type of Boolean Operation;
  //! if it is not defined (set to UNKNOWN) for each shape
  //! performed check as for single shape.
  //! bTestSE - flag that specifies whether check on small edges
  //! should be performed; by default it is set to TRUE;
  //! bTestSI - flag that specifies whether check on self-interference
  //! should be performed; by default it is set to TRUE;
  Standard_EXPORT BRepAlgoAPI_Check(const TopoDS_Shape& theS1, const TopoDS_Shape& theS2, const BOPAlgo_Operation theOp = BOPAlgo_UNKNOWN, const Standard_Boolean bTestSE = Standard_True, const Standard_Boolean bTestSI = Standard_True);
  
  //! Sets data for check by Init method.
  //! The method provides alternative way for checking single shape.
  Standard_EXPORT void SetData (const TopoDS_Shape& theS, const Standard_Boolean bTestSE = Standard_True, const Standard_Boolean bTestSI = Standard_True);
  
  //! Sets data for check by Init method.
  //! The method provides alternative way for checking couple of shapes.
  Standard_EXPORT void SetData (const TopoDS_Shape& theS1, const TopoDS_Shape& theS2, const BOPAlgo_Operation theOp = BOPAlgo_UNKNOWN, const Standard_Boolean bTestSE = Standard_True, const Standard_Boolean bTestSI = Standard_True);
  
  //! Performs the check.
  Standard_EXPORT void Perform();
  
  //! Shows whether shape(s) valid or not.
  Standard_EXPORT Standard_Boolean IsValid();
  
  //! Returns faulty shapes.
  Standard_EXPORT const BOPAlgo_ListOfCheckResult& Result();
  
  //! Sets the additional tolerance
  Standard_EXPORT void SetFuzzyValue (const Standard_Real theFuzz);
  
  //! Returns the additional tolerance
  Standard_EXPORT Standard_Real FuzzyValue() const;




protected:

  
  //! Initialyzes data.
  Standard_EXPORT void Init (const TopoDS_Shape& theS1, const TopoDS_Shape& theS2, const BOPAlgo_Operation theOp, const Standard_Boolean bTestSE, const Standard_Boolean bTestSI);


  TopoDS_Shape myS1;
  TopoDS_Shape myS2;
  BOPAlgo_PArgumentAnalyzer myAnalyzer;
  BOPAlgo_ListOfCheckResult myResult;
  Standard_Real myFuzzyValue;


private:





};







#endif // _BRepAlgoAPI_Check_HeaderFile
