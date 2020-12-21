// Created on: 1998-12-29
// Created by: Joelle CHAUVET
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

#ifndef _BRepFill_NSections_HeaderFile
#define _BRepFill_NSections_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <GeomFill_SequenceOfTrsf.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <TopTools_HArray2OfShape.hxx>
#include <BRepFill_SectionLaw.hxx>
#include <Standard_Boolean.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
class Geom_BSplineSurface;
class GeomFill_SectionLaw;
class TopoDS_Vertex;
class TopoDS_Shape;


class BRepFill_NSections;
DEFINE_STANDARD_HANDLE(BRepFill_NSections, BRepFill_SectionLaw)

//! Build Section Law, with N Sections
class BRepFill_NSections : public BRepFill_SectionLaw
{

public:

  
  //! Construct
  Standard_EXPORT BRepFill_NSections(const TopTools_SequenceOfShape& S, const Standard_Boolean Build = Standard_True);
  
  //! Construct
  Standard_EXPORT BRepFill_NSections(const TopTools_SequenceOfShape& S, const GeomFill_SequenceOfTrsf& Trsfs, const TColStd_SequenceOfReal& P, const Standard_Real VF, const Standard_Real VL, const Standard_Boolean Build = Standard_True);
  
  //! Say if the input shape is a  vertex.
  Standard_EXPORT virtual Standard_Boolean IsVertex() const Standard_OVERRIDE;
  
  //! Say if the Law is  Constant.
  Standard_EXPORT virtual Standard_Boolean IsConstant() const Standard_OVERRIDE;
  
  //! Give the law build on a concatenated section
  Standard_EXPORT virtual Handle(GeomFill_SectionLaw) ConcatenedLaw() const Standard_OVERRIDE;
  
  Standard_EXPORT virtual GeomAbs_Shape Continuity (const Standard_Integer Index, const Standard_Real TolAngular) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_Real VertexTol (const Standard_Integer Index, const Standard_Real Param) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual TopoDS_Vertex Vertex (const Standard_Integer Index, const Standard_Real Param) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual void D0 (const Standard_Real Param, TopoDS_Shape& S) Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(BRepFill_NSections,BRepFill_SectionLaw)

protected:




private:

  
  Standard_EXPORT void Init (const TColStd_SequenceOfReal& P, const Standard_Boolean B);

  Standard_Real VFirst;
  Standard_Real VLast;
  TopTools_SequenceOfShape myShapes;
  GeomFill_SequenceOfTrsf myTrsfs;
  TColStd_SequenceOfReal myParams;
  Handle(TopTools_HArray2OfShape) myEdges;
  Handle(Geom_BSplineSurface) mySurface;


};







#endif // _BRepFill_NSections_HeaderFile
