// Created on: 1996-01-11
// Created by: Jacques GOUSSARD
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

#ifndef _LocOpe_WiresOnShape_HeaderFile
#define _LocOpe_WiresOnShape_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Shape.hxx>
#include <TopTools_IndexedDataMapOfShapeShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <Standard_Boolean.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Real.hxx>
#include <TopTools_SequenceOfShape.hxx>

class TopoDS_Shape;
class TopoDS_Wire;
class TopoDS_Face;
class TopoDS_Compound;
class TopoDS_Edge;
class TopoDS_Vertex;


class LocOpe_WiresOnShape;
DEFINE_STANDARD_HANDLE(LocOpe_WiresOnShape, Standard_Transient)


class LocOpe_WiresOnShape : public Standard_Transient
{

public:

  
  Standard_EXPORT LocOpe_WiresOnShape(const TopoDS_Shape& S);
  
  Standard_EXPORT void Init (const TopoDS_Shape& S);

  //! Add splitting edges or wires for whole initial shape
  //! withot additional specification edge->face, edge->edge
  //! This method puts edge on the corresponding faces from initial shape
  Standard_EXPORT Standard_Boolean Add(const TopTools_SequenceOfShape& theEdges);
  
  //! Set the flag of check internal intersections
  //! default value is True (to check)
    void SetCheckInterior (const Standard_Boolean ToCheckInterior);
  
  Standard_EXPORT void Bind (const TopoDS_Wire& W, const TopoDS_Face& F);
  
  Standard_EXPORT void Bind (const TopoDS_Compound& Comp, const TopoDS_Face& F);
  
  Standard_EXPORT void Bind (const TopoDS_Edge& E, const TopoDS_Face& F);
  
  Standard_EXPORT void Bind (const TopoDS_Edge& EfromW, const TopoDS_Edge& EonFace);
  
  Standard_EXPORT void BindAll();
  
    Standard_Boolean IsDone() const;
  
  Standard_EXPORT void InitEdgeIterator();
  
  Standard_EXPORT Standard_Boolean MoreEdge();
  
  Standard_EXPORT TopoDS_Edge Edge();
  
  //! Returns the face of the shape on which the current
  //! edge is projected.
  Standard_EXPORT TopoDS_Face OnFace();
  
  //! If the   current  edge is  projected  on  an edge,
  //! returns <Standard_True> and sets the value of <E>.
  //! Otherwise, returns <Standard_False>.
  Standard_EXPORT Standard_Boolean OnEdge (TopoDS_Edge& E);
  
  Standard_EXPORT void NextEdge();
  
  Standard_EXPORT Standard_Boolean OnVertex (const TopoDS_Vertex& Vwire, TopoDS_Vertex& Vshape);
  
  //! If the vertex <V> lies on  an edge of the original
  //! shape,  returns     <Standard_True> and   sets the
  //! concerned edge in  <E>,  and the parameter on  the
  //! edge in <P>.
  //! Else returns <Standard_False>.
  Standard_EXPORT Standard_Boolean OnEdge (const TopoDS_Vertex& V, TopoDS_Edge& E, Standard_Real& P);
  
  //! If the vertex <V> lies on  an edge of the original
  //! shape,  returns     <Standard_True> and   sets the
  //! concerned edge in  <E>,  and the parameter on  the
  //! edge in <P>.
  //! Else returns <Standard_False>.
  Standard_EXPORT Standard_Boolean OnEdge (const TopoDS_Vertex& V, const TopoDS_Edge& EdgeFrom, TopoDS_Edge& E, Standard_Real& P);
  
  //! tells is the face to be split by section or not
    Standard_Boolean IsFaceWithSection (const TopoDS_Shape& aFace) const;




  DEFINE_STANDARD_RTTIEXT(LocOpe_WiresOnShape,Standard_Transient)

protected:




private:


  TopoDS_Shape myShape;
  TopTools_IndexedDataMapOfShapeShape myMapEF;
  TopTools_MapOfShape myFacesWithSection;
  Standard_Boolean myCheckInterior;
  TopTools_DataMapOfShapeShape myMap;
  Standard_Boolean myDone;
  Standard_Integer myIndex;


};


#include <LocOpe_WiresOnShape.lxx>





#endif // _LocOpe_WiresOnShape_HeaderFile
