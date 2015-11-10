// Created on: 1993-10-14
// Created by: Prestataire Mary FABIEN
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

#ifndef _BRepAlgo_BooleanOperations_HeaderFile
#define _BRepAlgo_BooleanOperations_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepAlgo_DSAccess.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <TopAbs_State.hxx>
#include <TopTools_ListOfShape.hxx>
class TopoDS_Shape;
class TopOpeBRepDS_HDataStructure;
class TopOpeBRepBuild_HBuilder;
class BRepAlgo_DSAccess;



class BRepAlgo_BooleanOperations 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepAlgo_BooleanOperations();
  
  //! S1 is a Shell with ALL faces supported by the SAME S2 is
  //! an Edge INCLUDED in that surface with pcurve.
  //! this avoids a time-consuming 3D operation, compared to Shapes.
  Standard_EXPORT void Shapes2d (const TopoDS_Shape& S1, const TopoDS_Shape& S2);
  
  //! Defines the arguments.
  Standard_EXPORT void Shapes (const TopoDS_Shape& S1, const TopoDS_Shape& S2);
  
  //! Sets different parameters for the curve approximations :
  //! NbPntMax : Maximum number of points to be approximated at
  //! the same time in one curve.
  //! Tol3D, Tol2D : Tolerances to be reached by the approximation.
  //! RelativeTol : The given tolerances are relative.
  Standard_EXPORT void SetApproxParameters (const Standard_Integer NbPntMax, const Standard_Real Tol3D, const Standard_Real Tol2D);
  
  Standard_EXPORT void Define (const TopoDS_Shape& S1, const TopoDS_Shape& S2, Handle(TopOpeBRepDS_HDataStructure)& HDS);
  
  //! returns the common part of the shapes.
  Standard_EXPORT const TopoDS_Shape& Common();
  
  //! returns the fuse part of the shapes.
  Standard_EXPORT const TopoDS_Shape& Fus();
  
  //! returns the cut part of the shapes.
  Standard_EXPORT const TopoDS_Shape& Cut();
  
  //! returns the intersection of the shapes.
  Standard_EXPORT const TopoDS_Shape& Section();
  
  //! returns the result of the boolean operation.
  Standard_EXPORT const TopoDS_Shape& Shape();
  
  //! Returns the shape(s) resulting of the boolean operation
  //! issued from the shape <S>.
  Standard_EXPORT const TopoDS_Shape& ShapeFrom (const TopoDS_Shape& S);
  
  //! Returns the list of the descendant shapes of the shape <S>.
  Standard_EXPORT const TopTools_ListOfShape& Modified (const TopoDS_Shape& S);
  
  //! Returns the fact that the shape <S> has been deleted or not
  //! by the boolean operation.
  Standard_EXPORT Standard_Boolean IsDeleted (const TopoDS_Shape& S);
  
  Standard_EXPORT const Handle(TopOpeBRepDS_HDataStructure)& DataStructure() const;
  
  Standard_EXPORT Handle(TopOpeBRepDS_HDataStructure)& ChangeDataStructure();
  
  Standard_EXPORT const Handle(TopOpeBRepBuild_HBuilder)& Builder() const;
  
  Standard_EXPORT Handle(TopOpeBRepBuild_HBuilder)& ChangeBuilder();
  
  //! returns the member myDSA. It is useful to then access
  //! the method GetSectionEdgeSet (wich is a member of DSAccess)
  Standard_EXPORT BRepAlgo_DSAccess& DataStructureAccess();




protected:





private:

  
  //! Performs the global boolean operation.
  Standard_EXPORT void Perform();
  
  //! Performs the global boolean operation and build the
  //! result in regards of the given states.
  Standard_EXPORT void Perform (const TopAbs_State State1, const TopAbs_State State2);


  BRepAlgo_DSAccess myDSA;
  TopoDS_Shape myS1;
  TopoDS_Shape myS2;
  TopoDS_Shape myShape;
  TopTools_MapOfShape myMapShape;
  TopoDS_Shape myResult;
  Standard_Integer myApproxNbPntMax;
  Standard_Real myApproxTol3D;
  Standard_Real myApproxTol2D;
};







#endif // _BRepAlgo_BooleanOperations_HeaderFile
