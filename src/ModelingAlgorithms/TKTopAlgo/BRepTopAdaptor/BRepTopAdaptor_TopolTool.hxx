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

#ifndef _BRepTopAdaptor_TopolTool_HeaderFile
#define _BRepTopAdaptor_TopolTool_HeaderFile

#include <Adaptor2d_Curve2d.hxx>
#include <Adaptor3d_Surface.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Face.hxx>
#include <TColStd_ListIteratorOfListOfTransient.hxx>
#include <Adaptor3d_TopolTool.hxx>
#include <TopAbs_State.hxx>
#include <TopAbs_Orientation.hxx>

class Adaptor3d_HVertex;
class gp_Pnt2d;
class gp_Pnt;

class BRepTopAdaptor_TopolTool;
DEFINE_STANDARD_HANDLE(BRepTopAdaptor_TopolTool, Adaptor3d_TopolTool)

class BRepTopAdaptor_TopolTool : public Adaptor3d_TopolTool
{

public:
  Standard_EXPORT BRepTopAdaptor_TopolTool();

  Standard_EXPORT BRepTopAdaptor_TopolTool(const Handle(Adaptor3d_Surface)& Surface);

  Standard_EXPORT virtual void Initialize() Standard_OVERRIDE;

  Standard_EXPORT virtual void Initialize(const Handle(Adaptor3d_Surface)& S) Standard_OVERRIDE;

  Standard_EXPORT virtual void Initialize(const Handle(Adaptor2d_Curve2d)& Curve) Standard_OVERRIDE;

  Standard_EXPORT virtual void Init() Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_Boolean More() Standard_OVERRIDE;

  Standard_EXPORT virtual Handle(Adaptor2d_Curve2d) Value() Standard_OVERRIDE;

  Standard_EXPORT virtual void Next() Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_Address Edge() const Standard_OVERRIDE;

  Standard_EXPORT virtual void InitVertexIterator() Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_Boolean MoreVertex() Standard_OVERRIDE;

  Standard_EXPORT virtual Handle(Adaptor3d_HVertex) Vertex() Standard_OVERRIDE;

  Standard_EXPORT virtual void NextVertex() Standard_OVERRIDE;

  Standard_EXPORT virtual TopAbs_State Classify(
    const gp_Pnt2d&        P2d,
    const Standard_Real    Tol,
    const Standard_Boolean RecadreOnPeriodic = Standard_True) Standard_OVERRIDE;

  //! see the code for specifications)
  Standard_EXPORT virtual Standard_Boolean IsThePointOn(
    const gp_Pnt2d&        P2d,
    const Standard_Real    Tol,
    const Standard_Boolean RecadreOnPeriodic = Standard_True) Standard_OVERRIDE;

  //! If the function returns the orientation of the arc.
  //! If the orientation is FORWARD or REVERSED, the arc is
  //! a "real" limit of the surface.
  //! If the orientation is INTERNAL or EXTERNAL, the arc is
  //! considered as an arc on the surface.
  Standard_EXPORT virtual TopAbs_Orientation Orientation(const Handle(Adaptor2d_Curve2d)& C)
    Standard_OVERRIDE;

  //! If the function returns the orientation of the arc.
  //! If the orientation is FORWARD or REVERSED, the arc is
  //! a "real" limit of the surface.
  //! If the orientation is INTERNAL or EXTERNAL, the arc is
  //! considered as an arc on the surface.
  Standard_EXPORT virtual TopAbs_Orientation Orientation(const Handle(Adaptor3d_HVertex)& C)
    Standard_OVERRIDE;

  Standard_EXPORT void Destroy();

  ~BRepTopAdaptor_TopolTool() { Destroy(); }

  //! answers if arcs and vertices may have 3d representations,
  //! so that we could use Tol3d and Pnt methods.
  Standard_EXPORT virtual Standard_Boolean Has3d() const Standard_OVERRIDE;

  //! returns 3d tolerance of the arc C
  Standard_EXPORT virtual Standard_Real Tol3d(const Handle(Adaptor2d_Curve2d)& C) const
    Standard_OVERRIDE;

  //! returns 3d tolerance of the vertex V
  Standard_EXPORT virtual Standard_Real Tol3d(const Handle(Adaptor3d_HVertex)& V) const
    Standard_OVERRIDE;

  //! returns 3d point of the vertex V
  Standard_EXPORT virtual gp_Pnt Pnt(const Handle(Adaptor3d_HVertex)& V) const Standard_OVERRIDE;

  Standard_EXPORT virtual void ComputeSamplePoints() Standard_OVERRIDE;

  //! compute the sample-points for the intersections algorithms
  Standard_EXPORT virtual Standard_Integer NbSamplesU() Standard_OVERRIDE;

  //! compute the sample-points for the intersections algorithms
  Standard_EXPORT virtual Standard_Integer NbSamplesV() Standard_OVERRIDE;

  //! compute the sample-points for the intersections algorithms
  Standard_EXPORT virtual Standard_Integer NbSamples() Standard_OVERRIDE;

  Standard_EXPORT virtual void SamplePoint(const Standard_Integer Index,
                                           gp_Pnt2d&              P2d,
                                           gp_Pnt&                P3d) Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_Boolean DomainIsInfinite() Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(BRepTopAdaptor_TopolTool, Adaptor3d_TopolTool)

protected:
private:
  TopExp_Explorer                       myVIterator;
  TopoDS_Face                           myFace;
  Standard_Address                      myFClass2d;
  Handle(BRepAdaptor_Curve2d)           myCurve;
  TColStd_ListOfTransient               myCurves;
  TColStd_ListIteratorOfListOfTransient myCIterator;
  Standard_Real                         myU0;
  Standard_Real                         myV0;
  Standard_Real                         myDU;
  Standard_Real                         myDV;
};

#endif // _BRepTopAdaptor_TopolTool_HeaderFile
