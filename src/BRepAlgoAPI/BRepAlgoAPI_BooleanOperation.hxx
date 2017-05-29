// Created on: 1993-10-14
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _BRepAlgoAPI_BooleanOperation_HeaderFile
#define _BRepAlgoAPI_BooleanOperation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopTools_ListOfShape.hxx>
#include <BOPAlgo_Operation.hxx>
#include <Standard_Boolean.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <BRepAlgoAPI_BuilderAlgo.hxx>
class BOPAlgo_PaveFiller;
class TopoDS_Shape;



//! The abstract class BooleanOperation is the root
//! class of Boolean Operations (see Overview).
//! Boolean Operations algorithm is divided onto two parts:<br>
//! - The first one is computing interference between arguments;<br>
//! - The second one is building the result of operation;<br>
//! The class BooleanOperation provides API level of both parts.<br>
//!
//! Additionally to the errors of the parent class the algorithm
//! returns the following Error statuses:<br>
//! - 0 - in case of success;<br>
//! - *BOPAlgo_AlertBOPNotSet* - in case the type of Boolean Operation is not set.<br>
//!
class BRepAlgoAPI_BooleanOperation  : public BRepAlgoAPI_BuilderAlgo
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns the first argument involved in this Boolean operation.
  //! Obsolete
  Standard_EXPORT const TopoDS_Shape& Shape1() const;
  
  //! Returns the second argument involved in this Boolean operation.
  //! Obsolete
  Standard_EXPORT const TopoDS_Shape& Shape2() const;
  
  //! Sets the tools
  Standard_EXPORT void SetTools (const TopTools_ListOfShape& theLS);
  
  //! Gets the tools
  Standard_EXPORT const TopTools_ListOfShape& Tools() const;
  
  //! Sets the type of Boolean operation
  Standard_EXPORT void SetOperation (const BOPAlgo_Operation anOp);
  
  //! Returns the type of Boolean Operation
  Standard_EXPORT BOPAlgo_Operation Operation() const;
Standard_EXPORT virtual ~BRepAlgoAPI_BooleanOperation();
  
  //! Performs the algorithm
  //! Filling interference Data Structure (if it is necessary)
  //! Building the result of the operation.
  Standard_EXPORT virtual void Build() Standard_OVERRIDE;
  
  //! Returns True if there was no errors occured
  //! obsolete
  Standard_EXPORT Standard_Boolean BuilderCanWork() const;
  
  //! Returns the flag of edge refining
  Standard_EXPORT Standard_Boolean FuseEdges() const;
  
  //! Fuse C1 edges
  Standard_EXPORT void RefineEdges();
  
  //! Returns a list of section edges.
  //! The edges represent the result of intersection between arguments of
  //! Boolean Operation. They are computed during operation execution.
  Standard_EXPORT const TopTools_ListOfShape& SectionEdges();
  
  //! Returns the list  of shapes modified from the shape <S>.
  Standard_EXPORT virtual const TopTools_ListOfShape& Modified (const TopoDS_Shape& aS) Standard_OVERRIDE;
  
  //! Returns true if the shape S has been deleted. The
  //! result shape of the operation does not contain the shape S.
  Standard_EXPORT virtual Standard_Boolean IsDeleted (const TopoDS_Shape& aS) Standard_OVERRIDE;
  
  //! Returns the list  of shapes generated from the shape <S>.
  //! For use in BRepNaming.
  Standard_EXPORT virtual const TopTools_ListOfShape& Generated (const TopoDS_Shape& S) Standard_OVERRIDE;
  
  //! Returns true if there is at least one modified shape.
  //! For use in BRepNaming.
  Standard_EXPORT virtual Standard_Boolean HasModified() const Standard_OVERRIDE;
  
  //! Returns true if there is at least one generated shape.
  //! For use in BRepNaming.
  Standard_EXPORT virtual Standard_Boolean HasGenerated() const Standard_OVERRIDE;
  
  //! Returns true if there is at least one deleted shape.
  //! For use in BRepNaming.
  Standard_EXPORT virtual Standard_Boolean HasDeleted() const Standard_OVERRIDE;

protected:
  
  //! Empty constructor
  Standard_EXPORT BRepAlgoAPI_BooleanOperation();
  
  //! Empty constructor
  //! <PF> - PaveFiller object that is carried out
  Standard_EXPORT BRepAlgoAPI_BooleanOperation(const BOPAlgo_PaveFiller& PF);
  
  //! Constructor with two arguments
  //! <S1>, <S2>  -arguments
  //! <anOperation> - the type of the operation
  //! Obsolete
  Standard_EXPORT BRepAlgoAPI_BooleanOperation(const TopoDS_Shape& S1, const TopoDS_Shape& S2, const BOPAlgo_Operation anOperation);
  
  //! Constructor with two arguments
  //! <S1>, <S2>  -arguments
  //! <anOperation> - the type of the operation
  //! <PF> - PaveFiller object that is carried out
  //! Obsolete
  Standard_EXPORT BRepAlgoAPI_BooleanOperation(const TopoDS_Shape& S1, const TopoDS_Shape& S2, const BOPAlgo_PaveFiller& PF, const BOPAlgo_Operation anOperation);
  
  Standard_EXPORT virtual void Clear() Standard_OVERRIDE;
  
  Standard_EXPORT virtual void SetAttributes();
  
  //! Returns the list  of shapes generated from the shape <S>.
  //! For use in BRepNaming.
  Standard_EXPORT const TopTools_ListOfShape& RefinedList (const TopTools_ListOfShape& theL);


  TopTools_ListOfShape myTools;
  BOPAlgo_Operation myOperation;
  Standard_Boolean myBuilderCanWork;


private:



  Standard_Boolean myFuseEdges;
  TopTools_DataMapOfShapeShape myModifFaces;
  TopTools_DataMapOfShapeShape myEdgeMap;


};







#endif // _BRepAlgoAPI_BooleanOperation_HeaderFile
