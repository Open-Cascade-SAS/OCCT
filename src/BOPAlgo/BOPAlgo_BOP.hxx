// Created by: Peter KURNEV
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _BOPAlgo_BOP_HeaderFile
#define _BOPAlgo_BOP_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BOPAlgo_Operation.hxx>
#include <Standard_Integer.hxx>
#include <TopoDS_Shape.hxx>
#include <BOPAlgo_ToolsProvider.hxx>
#include <NCollection_BaseAllocator.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <Standard_Boolean.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
class BOPAlgo_PaveFiller;

//!
//! The class represents the Building part of the Boolean Operations
//! algorithm.<br>
//! The arguments of the algorithms are divided in two groups - *Objects*
//! and *Tools*.<br>
//! The algorithm builds the splits of the given arguments using the intersection
//! results and combines the result of Boolean Operation of given type:<br>
//! - *FUSE* - union of two groups of objects;<br>
//! - *COMMON* - intersection of two groups of objects;<br>
//! - *CUT* - subtraction of one group from the other.<br>
//!
//! The rules for the arguments and type of the operation are the following:<br>
//! - For Boolean operation *FUSE* all arguments should have equal dimensions;<br>
//! - For Boolean operation *CUT* the minimal dimension of *Tools* should not be
//!   less than the maximal dimension of *Objects*;<br>
//! - For Boolean operation *COMMON* the arguments can have any dimension.<br>
//!
//! The class is a General Fuse based algorithm. Thus, all options
//! of the General Fuse algorithm such as Fuzzy mode, safe processing mode,
//! parallel processing mode, gluing mode and history support are also
//! available in this algorithm.<br>
//!
//! Additionally to the Warnings of the parent class the algorithm returns
//! the following warnings:
//! - *BOPAlgo_AlertEmptyShape* - in case some of the input shapes are empty shapes.
//!
//! Additionally to Errors of the parent class the algorithm returns
//! the following Error statuses:
//! - *BOPAlgo_AlertBOPIsNotSet* - in case the type of Boolean operation is not set;
//! - *BOPAlgo_AlertBOPNotAllowed* - in case the operation of given type is not allowed on
//!                     given inputs;
//! - *BOPAlgo_AlertSolidBuilderFailed* - in case the BuilderSolid algorithm failed to
//!                          produce the Fused solid.
//!
class BOPAlgo_BOP  : public BOPAlgo_ToolsProvider
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor
  Standard_EXPORT BOPAlgo_BOP();
  Standard_EXPORT virtual ~BOPAlgo_BOP();
  
  Standard_EXPORT BOPAlgo_BOP(const Handle(NCollection_BaseAllocator)& theAllocator);
  
  //! Clears internal fields and arguments
  Standard_EXPORT virtual void Clear() Standard_OVERRIDE;
  
  Standard_EXPORT void SetOperation (const BOPAlgo_Operation theOperation);
  
  Standard_EXPORT BOPAlgo_Operation Operation() const;
  
  Standard_EXPORT virtual void Perform(const Message_ProgressRange& theRange = Message_ProgressRange()) Standard_OVERRIDE;

protected:
  
  Standard_EXPORT virtual void CheckData() Standard_OVERRIDE;
  
  //! Performs calculations using prepared Filler
  //! object <thePF>
  Standard_EXPORT virtual void PerformInternal1 (const BOPAlgo_PaveFiller& thePF,
                                                 const Message_ProgressRange& theRange) Standard_OVERRIDE;

  Standard_EXPORT virtual void BuildResult (const TopAbs_ShapeEnum theType) Standard_OVERRIDE;
  
  Standard_EXPORT void BuildShape(const Message_ProgressRange& theRange);
  
  Standard_EXPORT void BuildRC(const Message_ProgressRange& theRange);
  
  Standard_EXPORT void BuildSolid(const Message_ProgressRange& theRange);
  
  //! Treatment of the cases with empty shapes.<br>
  //! It returns TRUE if there is nothing to do, i.e.
  //! all shapes in one of the groups are empty shapes.
  Standard_EXPORT Standard_Boolean TreatEmptyShape();

  //! Checks if the arguments of Boolean Operation on solids
  //! contain any open solids, for which the building of the splits
  //! has failed. In case of positive check, run different procedure
  //! for building the result shape.
  Standard_EXPORT virtual Standard_Boolean CheckArgsForOpenSolid();

protected:

  //! Extend list of operations to be supported by the Progress Indicator
  enum BOPAlgo_PIOperation
  {
    PIOperation_BuildShape = BOPAlgo_ToolsProvider::PIOperation_Last,
    PIOperation_Last
  };

  //! Fill PI steps
  Standard_EXPORT virtual void fillPIConstants(const Standard_Real theWhole, BOPAlgo_PISteps& theSteps) const Standard_OVERRIDE;

protected:

  BOPAlgo_Operation myOperation;
  Standard_Integer  myDims[2];
  TopoDS_Shape      myRC;
};

#endif // _BOPAlgo_BOP_HeaderFile
