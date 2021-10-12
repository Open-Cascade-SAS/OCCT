// Created on: 1996-04-09
// Created by: Yves FRICAUD
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _BRepBuilderAPI_Collect_HeaderFile
#define _BRepBuilderAPI_Collect_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopoDS_Shape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
class BRepBuilderAPI_MakeShape;



class BRepBuilderAPI_Collect 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepBuilderAPI_Collect();
  
  Standard_EXPORT void Add (const TopoDS_Shape& SI, BRepBuilderAPI_MakeShape& MKS);
  
  Standard_EXPORT void AddGenerated (const TopoDS_Shape& S, const TopoDS_Shape& Gen);
  
  Standard_EXPORT void AddModif (const TopoDS_Shape& S, const TopoDS_Shape& Mod);
  
  Standard_EXPORT void Filter (const TopoDS_Shape& SF);
  
  Standard_EXPORT const TopTools_DataMapOfShapeListOfShape& Modification() const;
  
  Standard_EXPORT const TopTools_DataMapOfShapeListOfShape& Generated() const;




protected:





private:



  TopoDS_Shape myInitialShape;
  TopTools_MapOfShape myDeleted;
  TopTools_DataMapOfShapeListOfShape myMod;
  TopTools_DataMapOfShapeListOfShape myGen;


};







#endif // _BRepBuilderAPI_Collect_HeaderFile
