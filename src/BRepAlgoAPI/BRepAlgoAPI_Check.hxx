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

#include <BOPAlgo_ListOfCheckResult.hxx>
#include <BOPAlgo_Operation.hxx>
#include <BOPAlgo_Options.hxx>
#include <TopoDS_Shape.hxx>
#include <Message_ProgressRange.hxx>


//! The class Check provides a diagnostic tool for checking the validity
//! of the single shape or couple of shapes.
//! The shapes are checked on:
//! - Topological validity;
//! - Small edges;
//! - Self-interference;
//! - Validity for Boolean operation of certain type (for couple of shapes only).
//!
//! The class provides two ways of checking shape(-s)
//! 1. Constructors
//! BRepAlgoAPI_Check aCh(theS);
//! Standard_Boolean isValid = aCh.IsValid();
//! 2. Methods SetData and Perform
//! BRepAlgoAPI_Check aCh;
//! aCh.SetData(theS1, theS2, BOPAlgo_FUSE, Standard_False);
//! aCh.Perform();
//! Standard_Boolean isValid = aCh.IsValid();
//!
class BRepAlgoAPI_Check : public BOPAlgo_Options
{
public:

  DEFINE_STANDARD_ALLOC


public: //! @name Constructors

  //! Empty constructor.
  Standard_EXPORT BRepAlgoAPI_Check();
  Standard_EXPORT virtual ~BRepAlgoAPI_Check();

  //! Constructor for checking single shape.
  //!
  //! @param theS [in] - the shape to check;
  //! @param bTestSE [in] - flag which specifies whether to check the shape
  //!                       on small edges or not; by default it is set to TRUE;
  //! @param bTestSI [in] - flag which specifies whether to check the shape
  //!                       on self-interference or not; by default it is set to TRUE;
  //! @param theRange [in] - parameter to use progress indicator
  Standard_EXPORT BRepAlgoAPI_Check(const TopoDS_Shape& theS,
                                    const Standard_Boolean bTestSE = Standard_True,
                                    const Standard_Boolean bTestSI = Standard_True,
                                    const Message_ProgressRange& theRange = Message_ProgressRange());

  //! Constructor for checking the couple of shapes.
  //! Additionally to the validity checks of each given shape,
  //! the types of the given shapes will be checked on validity
  //! for Boolean operation of given type.
  //!
  //! @param theS1 [in] - the first shape to check;
  //! @param theS2 [in] - the second shape to check;
  //! @param theOp [in] - the type of Boolean Operation for which the validity
  //!                     of given shapes should be checked.
  //! @param bTestSE [in] - flag which specifies whether to check the shape
  //!                       on small edges or not; by default it is set to TRUE;
  //! @param bTestSI [in] - flag which specifies whether to check the shape
  //!                       on self-interference or not; by default it is set to TRUE;
  //! @param theRange [in] - parameter to use progress indicator
  Standard_EXPORT BRepAlgoAPI_Check(const TopoDS_Shape& theS1,
                                    const TopoDS_Shape& theS2,
                                    const BOPAlgo_Operation theOp = BOPAlgo_UNKNOWN,
                                    const Standard_Boolean bTestSE = Standard_True,
                                    const Standard_Boolean bTestSI = Standard_True,
                                    const Message_ProgressRange& theRange = Message_ProgressRange());


public: //! @name Initializing the algorithm

  //! Initializes the algorithm with single shape.
  //!
  //! @param theS [in] - the shape to check;
  //! @param bTestSE [in] - flag which specifies whether to check the shape
  //!                       on small edges or not; by default it is set to TRUE;
  //! @param bTestSI [in] - flag which specifies whether to check the shape
  //!                       on self-interference or not; by default it is set to TRUE;
  void SetData(const TopoDS_Shape& theS,
               const Standard_Boolean bTestSE = Standard_True,
               const Standard_Boolean bTestSI = Standard_True)
  {
    myS1 = theS;
    myS2 = TopoDS_Shape();
    myTestSE = bTestSE;
    myTestSI = bTestSI;
    myFaultyShapes.Clear();
  }

  //! Initializes the algorithm with couple of shapes.
  //! Additionally to the validity checks of each given shape,
  //! the types of the given shapes will be checked on validity
  //! for Boolean operation of given type.
  //!
  //! @param theS1 [in] - the first shape to check;
  //! @param theS2 [in] - the second shape to check;
  //! @param theOp [in] - the type of Boolean Operation for which the validity
  //!                     of given shapes should be checked.
  //! @param bTestSE [in] - flag which specifies whether to check the shape
  //!                       on small edges or not; by default it is set to TRUE;
  //! @param bTestSI [in] - flag which specifies whether to check the shape
  //!                       on self-interference or not; by default it is set to TRUE;
  void SetData(const TopoDS_Shape& theS1,
               const TopoDS_Shape& theS2,
               const BOPAlgo_Operation theOp = BOPAlgo_UNKNOWN,
               const Standard_Boolean bTestSE = Standard_True,
               const Standard_Boolean bTestSI = Standard_True)
  {
    myS1 = theS1;
    myS2 = theS2;
    myOperation = theOp;
    myTestSE = bTestSE;
    myTestSI = bTestSI;
    myFaultyShapes.Clear();
  }


public: //! @name Performing the operation

  //! Performs the check.
  Standard_EXPORT void Perform(const Message_ProgressRange& theRange = Message_ProgressRange());


public: //! @name Getting the results.

  //! Shows whether shape(s) valid or not.
  Standard_Boolean IsValid()
  {
    return myFaultyShapes.IsEmpty();
  }

  //! Returns faulty shapes.
  const BOPAlgo_ListOfCheckResult& Result()
  {
    return myFaultyShapes;
  }


protected: //! @name Fields

  // Inputs
  TopoDS_Shape myS1;                        //!< The first shape
  TopoDS_Shape myS2;                        //!< The second shape
  Standard_Boolean myTestSE;                //!< Flag defining whether to look for small edges in the given shapes or not
  Standard_Boolean myTestSI;                //!< Flag defining whether to check the input edges on self-interference or not
  BOPAlgo_Operation myOperation;            //!< Type of Boolean operation for which the validity of input shapes should be checked

  // Results
  BOPAlgo_ListOfCheckResult myFaultyShapes; //!< Found faulty shapes

};

#endif // _BRepAlgoAPI_Check_HeaderFile
