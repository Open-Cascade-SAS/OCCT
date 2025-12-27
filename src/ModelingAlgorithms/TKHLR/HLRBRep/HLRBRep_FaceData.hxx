// Created on: 1997-04-17
// Created by: Christophe MARION
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

#ifndef _HLRBRep_FaceData_HeaderFile
#define _HLRBRep_FaceData_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <HLRBRep_Surface.hxx>
#include <TopAbs_Orientation.hxx>
#include <Standard_Integer.hxx>
class HLRAlgo_WiresBlock;
class TopoDS_Face;

class HLRBRep_FaceData
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT HLRBRep_FaceData();

  //! <Or> is the orientation of the face. <Cl> is true
  //! if the face belongs to a closed volume. <NW> is
  //! the number of wires (or block of edges) of the
  //! face.
  Standard_EXPORT void Set(const TopoDS_Face&       FG,
                           const TopAbs_Orientation Or,
                           const bool   Cl,
                           const int   NW);

  //! Set <NE> the number of edges of the wire number
  //! <WI>.
  Standard_EXPORT void SetWire(const int WI, const int NE);

  //! Set the edge number <EWI> of the wire <WI>.
  Standard_EXPORT void SetWEdge(const int   WI,
                                const int   EWI,
                                const int   EI,
                                const TopAbs_Orientation Or,
                                const bool   OutL,
                                const bool   Inte,
                                const bool   Dble,
                                const bool   IsoL);

  bool Selected() const;

  void Selected(const bool B);

  bool Back() const;

  void Back(const bool B);

  bool Side() const;

  void Side(const bool B);

  bool Closed() const;

  void Closed(const bool B);

  bool Hiding() const;

  void Hiding(const bool B);

  bool Simple() const;

  void Simple(const bool B);

  bool Cut() const;

  void Cut(const bool B);

  bool WithOutL() const;

  void WithOutL(const bool B);

  bool Plane() const;

  void Plane(const bool B);

  bool Cylinder() const;

  void Cylinder(const bool B);

  bool Cone() const;

  void Cone(const bool B);

  bool Sphere() const;

  void Sphere(const bool B);

  bool Torus() const;

  void Torus(const bool B);

  double Size() const;

  void Size(const double S);

  TopAbs_Orientation Orientation() const;

  void Orientation(const TopAbs_Orientation O);

  occ::handle<HLRAlgo_WiresBlock>& Wires();

  HLRBRep_Surface& Geometry();

  float Tolerance() const;

protected:
  enum EMaskFlags
  {
    EMaskOrient   = 15,
    FMaskSelected = 16,
    FMaskBack     = 32,
    FMaskSide     = 64,
    FMaskClosed   = 128,
    FMaskHiding   = 256,
    FMaskSimple   = 512,
    FMaskCut      = 1024,
    FMaskWithOutL = 2048,
    FMaskPlane    = 4096,
    FMaskCylinder = 8192,
    FMaskCone     = 16384,
    FMaskSphere   = 32768,
    FMaskTorus    = 65536
  };

private:
  int           myFlags;
  occ::handle<HLRAlgo_WiresBlock> myWires;
  HLRBRep_Surface            myGeometry;
  double              mySize;
  float         myTolerance;
};

#include <HLRBRep_FaceData.lxx>

#endif // _HLRBRep_FaceData_HeaderFile
