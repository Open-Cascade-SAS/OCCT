// Created on: 1998-01-07
// Created by: Philippe MANGIN
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _BRepFill_SectionLaw_HeaderFile
#define _BRepFill_SectionLaw_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <GeomFill_HArray1OfSectionLaw.hxx>
#include <TopTools_DataMapOfShapeInteger.hxx>
#include <Standard_Boolean.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Real.hxx>
class GeomFill_SectionLaw;
class TopoDS_Vertex;
class TopoDS_Shape;
class TopoDS_Wire;
class TopoDS_Edge;

class BRepFill_SectionLaw;
DEFINE_STANDARD_HANDLE(BRepFill_SectionLaw, Standard_Transient)

//! Build Section Law, with an Vertex, or an Wire
class BRepFill_SectionLaw : public Standard_Transient
{

public:

  
  Standard_EXPORT Standard_Integer NbLaw() const;
  
  Standard_EXPORT const Handle(GeomFill_SectionLaw)& Law (const Standard_Integer Index) const;
  
  Standard_EXPORT Standard_Integer IndexOfEdge(const TopoDS_Shape& anEdge) const;
  
  Standard_EXPORT virtual Standard_Boolean IsConstant() const = 0;
  
  Standard_EXPORT Standard_Boolean IsUClosed() const;
  
  Standard_EXPORT Standard_Boolean IsVClosed() const;
  
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Say if the input shape is a  vertex.
  Standard_EXPORT virtual Standard_Boolean IsVertex() const = 0;
  
  Standard_EXPORT virtual Handle(GeomFill_SectionLaw) ConcatenedLaw() const = 0;
  
  Standard_EXPORT virtual GeomAbs_Shape Continuity (const Standard_Integer Index, const Standard_Real TolAngular) const = 0;
  
  Standard_EXPORT virtual Standard_Real VertexTol (const Standard_Integer Index, const Standard_Real Param) const = 0;
  
  Standard_EXPORT virtual TopoDS_Vertex Vertex (const Standard_Integer Index, const Standard_Real Param) const = 0;
  
  Standard_EXPORT virtual void D0 (const Standard_Real U, TopoDS_Shape& S) = 0;
  
  Standard_EXPORT void Init (const TopoDS_Wire& W);
  
  Standard_EXPORT TopoDS_Edge CurrentEdge();




  DEFINE_STANDARD_RTTIEXT(BRepFill_SectionLaw,Standard_Transient)

protected:


  Handle(GeomFill_HArray1OfSectionLaw) myLaws;
  Standard_Boolean uclosed;
  Standard_Boolean vclosed;
  Standard_Boolean myDone;
  TopTools_DataMapOfShapeInteger myIndices;


private:


  BRepTools_WireExplorer myIterator;


};







#endif // _BRepFill_SectionLaw_HeaderFile
