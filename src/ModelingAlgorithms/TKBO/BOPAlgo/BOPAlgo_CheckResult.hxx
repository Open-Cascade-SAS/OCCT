// Created on: 2004-09-03
// Created by: Oleg FEDYAEV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

#ifndef _BOPAlgo_CheckResult_HeaderFile
#define _BOPAlgo_CheckResult_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>
#include <BOPAlgo_CheckStatus.hxx>
#include <TopTools_ListOfShape.hxx>

//! contains information about faulty shapes and faulty types
//! can't be processed by Boolean Operations
class BOPAlgo_CheckResult
{
public:
  DEFINE_STANDARD_ALLOC

  //! empty constructor
  Standard_EXPORT BOPAlgo_CheckResult();

  //! sets ancestor shape (object) for faulty sub-shapes
  Standard_EXPORT void SetShape1(const TopoDS_Shape& TheShape);

  //! adds faulty sub-shapes from object to a list
  Standard_EXPORT void AddFaultyShape1(const TopoDS_Shape& TheShape);

  //! sets ancestor shape (tool) for faulty sub-shapes
  Standard_EXPORT void SetShape2(const TopoDS_Shape& TheShape);

  //! adds faulty sub-shapes from tool to a list
  Standard_EXPORT void AddFaultyShape2(const TopoDS_Shape& TheShape);

  //! returns ancestor shape (object) for faulties
  Standard_EXPORT const TopoDS_Shape& GetShape1() const;

  //! returns ancestor shape (tool) for faulties
  Standard_EXPORT const TopoDS_Shape& GetShape2() const;

  //! returns list of faulty shapes for object
  Standard_EXPORT const TopTools_ListOfShape& GetFaultyShapes1() const;

  //! returns list of faulty shapes for tool
  Standard_EXPORT const TopTools_ListOfShape& GetFaultyShapes2() const;

  //! set status of faulty
  Standard_EXPORT void SetCheckStatus(const BOPAlgo_CheckStatus TheStatus);

  //! gets status of faulty
  Standard_EXPORT BOPAlgo_CheckStatus GetCheckStatus() const;

  //! Sets max distance for the first shape
  Standard_EXPORT void SetMaxDistance1(const Standard_Real theDist);

  //! Sets max distance for the second shape
  Standard_EXPORT void SetMaxDistance2(const Standard_Real theDist);

  //! Sets the parameter for the first shape
  Standard_EXPORT void SetMaxParameter1(const Standard_Real thePar);

  //! Sets the parameter for the second shape
  Standard_EXPORT void SetMaxParameter2(const Standard_Real thePar);

  //! Returns the distance for the first shape
  Standard_EXPORT Standard_Real GetMaxDistance1() const;

  //! Returns the distance for the second shape
  Standard_EXPORT Standard_Real GetMaxDistance2() const;

  //! Returns the parameter for the fircst shape
  Standard_EXPORT Standard_Real GetMaxParameter1() const;

  //! Returns the parameter for the second shape
  Standard_EXPORT Standard_Real GetMaxParameter2() const;

protected:
private:
  TopoDS_Shape         myShape1;
  TopoDS_Shape         myShape2;
  BOPAlgo_CheckStatus  myStatus;
  TopTools_ListOfShape myFaulty1;
  TopTools_ListOfShape myFaulty2;
  Standard_Real        myMaxDist1;
  Standard_Real        myMaxDist2;
  Standard_Real        myMaxPar1;
  Standard_Real        myMaxPar2;
};

#endif // _BOPAlgo_CheckResult_HeaderFile
