// Created on: 1998-06-03
// Created by: data exchange team
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

#ifndef _ShapeFix_EdgeProjAux_HeaderFile
#define _ShapeFix_EdgeProjAux_HeaderFile

#include <Standard.hxx>

#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <Standard_Real.hxx>
#include <Standard_Transient.hxx>
class Geom2d_Curve;

class ShapeFix_EdgeProjAux;
DEFINE_STANDARD_HANDLE(ShapeFix_EdgeProjAux, Standard_Transient)

//! Project 3D point (vertex) on pcurves to find Vertex Parameter
//! on parametric representation of an edge
class ShapeFix_EdgeProjAux : public Standard_Transient
{

public:
  Standard_EXPORT ShapeFix_EdgeProjAux();

  Standard_EXPORT ShapeFix_EdgeProjAux(const TopoDS_Face& F, const TopoDS_Edge& E);

  Standard_EXPORT void Init(const TopoDS_Face& F, const TopoDS_Edge& E);

  Standard_EXPORT void Compute(const Standard_Real preci);

  Standard_EXPORT Standard_Boolean IsFirstDone() const;

  Standard_EXPORT Standard_Boolean IsLastDone() const;

  Standard_EXPORT Standard_Real FirstParam() const;

  Standard_EXPORT Standard_Real LastParam() const;

  Standard_EXPORT Standard_Boolean IsIso(const Handle(Geom2d_Curve)& C);

  DEFINE_STANDARD_RTTIEXT(ShapeFix_EdgeProjAux, Standard_Transient)

protected:
  Standard_EXPORT void Init2d(const Standard_Real preci);

  Standard_EXPORT void Init3d(const Standard_Real preci);

  Standard_EXPORT void UpdateParam2d(const Handle(Geom2d_Curve)& C);

  TopoDS_Face      myFace;
  TopoDS_Edge      myEdge;
  Standard_Real    myFirstParam;
  Standard_Real    myLastParam;
  Standard_Boolean myFirstDone;
  Standard_Boolean myLastDone;

private:
};

#endif // _ShapeFix_EdgeProjAux_HeaderFile
