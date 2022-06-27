// Created on: 1997-06-10
// Created by: Yves FRICAUD
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _TNaming_Localizer_HeaderFile
#define _TNaming_Localizer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TNaming_ListOfMapOfShape.hxx>
#include <TNaming_ListOfIndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TNaming_Evolution.hxx>
#include <TNaming_ListOfNamedShape.hxx>
#include <TNaming_MapOfNamedShape.hxx>
class TNaming_UsedShapes;
class TopoDS_Shape;
class TDF_Label;
class TNaming_NamedShape;



class TNaming_Localizer 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT TNaming_Localizer();
  
  Standard_EXPORT void Init (const Handle(TNaming_UsedShapes)& US, const Standard_Integer CurTrans);
  
  Standard_EXPORT const TopTools_MapOfShape& SubShapes (const TopoDS_Shape& S, const TopAbs_ShapeEnum Type);
  
  Standard_EXPORT const TopTools_IndexedDataMapOfShapeListOfShape& Ancestors (const TopoDS_Shape& S, const TopAbs_ShapeEnum Type);
  
  Standard_EXPORT void FindFeaturesInAncestors (const TopoDS_Shape& S, const TopoDS_Shape& In, TopTools_MapOfShape& AncInFeatures);
  
  Standard_EXPORT void GoBack (const TopoDS_Shape& S, const TDF_Label& Lab, const TNaming_Evolution Evol, TopTools_ListOfShape& OldS, TNaming_ListOfNamedShape& OldLab);
  
  Standard_EXPORT void Backward (const Handle(TNaming_NamedShape)& NS, const TopoDS_Shape& S, TNaming_MapOfNamedShape& Primitives, TopTools_MapOfShape& ValidShapes);
  
  Standard_EXPORT void FindNeighbourg (const TopoDS_Shape& Cont, const TopoDS_Shape& S, TopTools_MapOfShape& Neighbourg);
  
  Standard_EXPORT static Standard_Boolean IsNew (const TopoDS_Shape& S, const Handle(TNaming_NamedShape)& NS);
  
  Standard_EXPORT static void FindGenerator (const Handle(TNaming_NamedShape)& NS, const TopoDS_Shape& S, TopTools_ListOfShape& theListOfGenerators);
  
  //! Finds context of the shape <S>.
  Standard_EXPORT static void FindShapeContext (const Handle(TNaming_NamedShape)& NS, const TopoDS_Shape& theS, TopoDS_Shape& theSC);




protected:





private:



  Standard_Integer myCurTrans;
  Handle(TNaming_UsedShapes) myUS;
  TopTools_ListOfShape myShapeWithSubShapes;
  TNaming_ListOfMapOfShape mySubShapes;
  TopTools_ListOfShape myShapeWithAncestors;
  TNaming_ListOfIndexedDataMapOfShapeListOfShape myAncestors;


};







#endif // _TNaming_Localizer_HeaderFile
