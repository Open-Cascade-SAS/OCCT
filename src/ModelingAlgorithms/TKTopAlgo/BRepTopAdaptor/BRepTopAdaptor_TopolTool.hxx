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
#include <Standard_Transient.hxx>
#include <NCollection_List.hxx>
#include <Adaptor3d_TopolTool.hxx>
#include <TopAbs_State.hxx>
#include <TopAbs_Orientation.hxx>

class Adaptor3d_HVertex;
class gp_Pnt2d;
class gp_Pnt;

class BRepTopAdaptor_TopolTool : public Adaptor3d_TopolTool
{

public:
  Standard_EXPORT BRepTopAdaptor_TopolTool();

  Standard_EXPORT BRepTopAdaptor_TopolTool(const occ::handle<Adaptor3d_Surface>& Surface);

  Standard_EXPORT void Initialize() override;

  Standard_EXPORT void Initialize(const occ::handle<Adaptor3d_Surface>& S) override;

  Standard_EXPORT void Initialize(const occ::handle<Adaptor2d_Curve2d>& Curve) override;

  Standard_EXPORT void Init() override;

  Standard_EXPORT bool More() override;

  Standard_EXPORT occ::handle<Adaptor2d_Curve2d> Value() override;

  Standard_EXPORT void Next() override;

  Standard_EXPORT void* Edge() const override;

  Standard_EXPORT void InitVertexIterator() override;

  Standard_EXPORT bool MoreVertex() override;

  Standard_EXPORT occ::handle<Adaptor3d_HVertex> Vertex() override;

  Standard_EXPORT void NextVertex() override;

  Standard_EXPORT TopAbs_State Classify(const gp_Pnt2d& P2d,
                                        const double    Tol,
                                        const bool      RecadreOnPeriodic = true) override;

  //! see the code for specifications)
  Standard_EXPORT bool IsThePointOn(const gp_Pnt2d& P2d,
                                    const double    Tol,
                                    const bool      RecadreOnPeriodic = true) override;

  //! If the function returns the orientation of the arc.
  //! If the orientation is FORWARD or REVERSED, the arc is
  //! a "real" limit of the surface.
  //! If the orientation is INTERNAL or EXTERNAL, the arc is
  //! considered as an arc on the surface.
  Standard_EXPORT TopAbs_Orientation Orientation(const occ::handle<Adaptor2d_Curve2d>& C) override;

  //! If the function returns the orientation of the arc.
  //! If the orientation is FORWARD or REVERSED, the arc is
  //! a "real" limit of the surface.
  //! If the orientation is INTERNAL or EXTERNAL, the arc is
  //! considered as an arc on the surface.
  Standard_EXPORT TopAbs_Orientation Orientation(const occ::handle<Adaptor3d_HVertex>& C) override;

  Standard_EXPORT void Destroy();

  ~BRepTopAdaptor_TopolTool() override { Destroy(); }

  //! answers if arcs and vertices may have 3d representations,
  //! so that we could use Tol3d and Pnt methods.
  Standard_EXPORT bool Has3d() const override;

  //! returns 3d tolerance of the arc C
  Standard_EXPORT double Tol3d(const occ::handle<Adaptor2d_Curve2d>& C) const override;

  //! returns 3d tolerance of the vertex V
  Standard_EXPORT double Tol3d(const occ::handle<Adaptor3d_HVertex>& V) const override;

  //! returns 3d point of the vertex V
  Standard_EXPORT gp_Pnt Pnt(const occ::handle<Adaptor3d_HVertex>& V) const override;

  Standard_EXPORT void ComputeSamplePoints() override;

  //! compute the sample-points for the intersections algorithms
  Standard_EXPORT int NbSamplesU() override;

  //! compute the sample-points for the intersections algorithms
  Standard_EXPORT int NbSamplesV() override;

  //! compute the sample-points for the intersections algorithms
  Standard_EXPORT int NbSamples() override;

  Standard_EXPORT void SamplePoint(const int Index, gp_Pnt2d& P2d, gp_Pnt& P3d) override;

  Standard_EXPORT bool DomainIsInfinite() override;

  DEFINE_STANDARD_RTTIEXT(BRepTopAdaptor_TopolTool, Adaptor3d_TopolTool)

private:
  TopExp_Explorer                                             myVIterator;
  TopoDS_Face                                                 myFace;
  void*                                                       myFClass2d;
  occ::handle<BRepAdaptor_Curve2d>                            myCurve;
  NCollection_List<occ::handle<Standard_Transient>>           myCurves;
  NCollection_List<occ::handle<Standard_Transient>>::Iterator myCIterator;
  double                                                      myU0;
  double                                                      myV0;
  double                                                      myDU;
  double                                                      myDV;
};

#endif // _BRepTopAdaptor_TopolTool_HeaderFile
