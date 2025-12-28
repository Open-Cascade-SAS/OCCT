// Created on: 1994-04-01
// Created by: Modelistation
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _BRepTopAdaptor_HVertex_HeaderFile
#define _BRepTopAdaptor_HVertex_HeaderFile

#include <Adaptor3d_HVertex.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <TopAbs_Orientation.hxx>
#include <TopoDS_Vertex.hxx>

class gp_Pnt2d;

class BRepTopAdaptor_HVertex : public Adaptor3d_HVertex
{

public:
  Standard_EXPORT BRepTopAdaptor_HVertex(const TopoDS_Vertex&                    Vtx,
                                         const occ::handle<BRepAdaptor_Curve2d>& Curve);

  const TopoDS_Vertex& Vertex() const;

  TopoDS_Vertex& ChangeVertex();

  Standard_EXPORT virtual gp_Pnt2d Value() override;

  Standard_EXPORT virtual double Parameter(const occ::handle<Adaptor2d_Curve2d>& C) override;

  //! Parametric resolution (2d).
  Standard_EXPORT virtual double Resolution(const occ::handle<Adaptor2d_Curve2d>& C) override;

  Standard_EXPORT virtual TopAbs_Orientation Orientation() override;

  Standard_EXPORT virtual bool IsSame(const occ::handle<Adaptor3d_HVertex>& Other) override;

  DEFINE_STANDARD_RTTIEXT(BRepTopAdaptor_HVertex, Adaptor3d_HVertex)

private:
  TopoDS_Vertex                    myVtx;
  occ::handle<BRepAdaptor_Curve2d> myCurve;
};

#include <BRepTopAdaptor_HVertex.lxx>

#endif // _BRepTopAdaptor_HVertex_HeaderFile
