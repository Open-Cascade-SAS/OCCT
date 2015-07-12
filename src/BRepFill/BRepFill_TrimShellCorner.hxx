// Created on: 2003-10-21
// Created by: Mikhail KLOKOV
// Copyright (c) 2003-2014 OPEN CASCADE SAS
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

#ifndef _BRepFill_TrimShellCorner_HeaderFile
#define _BRepFill_TrimShellCorner_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Ax2.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_HArray2OfShape.hxx>
#include <Standard_Boolean.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
class gp_Ax2;
class TopoDS_Face;
class TopoDS_Wire;
class TopoDS_Shape;



class BRepFill_TrimShellCorner 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT BRepFill_TrimShellCorner(const Handle(TopTools_HArray2OfShape)& theFaces, const gp_Ax2& theAxeOfBisPlane, const TopoDS_Face& theSecPlane);
  
  Standard_EXPORT BRepFill_TrimShellCorner(const Handle(TopTools_HArray2OfShape)& theFaces, const gp_Ax2& theAxeOfBisPlane, const TopoDS_Wire& theSpine, const TopoDS_Face& theSecPlane);
  
  Standard_EXPORT void SetSpine (const TopoDS_Wire& theSpine);
  
  Standard_EXPORT void AddBounds (const Handle(TopTools_HArray2OfShape)& Bounds);
  
  Standard_EXPORT void AddUEdges (const Handle(TopTools_HArray2OfShape)& theUEdges);
  
  Standard_EXPORT void Perform();
  
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  Standard_EXPORT Standard_Boolean HasSection() const;
  
  Standard_EXPORT void Modified (const TopoDS_Shape& S, TopTools_ListOfShape& theModified);




protected:





private:



  gp_Ax2 myAxeOfBisPlane;
  TopoDS_Shape myShape1;
  TopoDS_Shape myShape2;
  TopoDS_Wire mySpine;
  TopoDS_Face mySecPln;
  Handle(TopTools_HArray2OfShape) myBounds;
  Handle(TopTools_HArray2OfShape) myUEdges;
  Handle(TopTools_HArray2OfShape) myFaces;
  Standard_Boolean myDone;
  Standard_Boolean myHasSection;
  TopTools_DataMapOfShapeListOfShape myHistMap;


};







#endif // _BRepFill_TrimShellCorner_HeaderFile
