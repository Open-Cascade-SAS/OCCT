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

  //! <Or> is the orientation of the face.  <Cl> is true
  //! if the face  belongs to a  closed  volume. <NW> is
  //! the number of wires ( or block  of  edges ) of the
  //! face.
  Standard_EXPORT void Set(const TopoDS_Face&       FG,
                           const TopAbs_Orientation Or,
                           const Standard_Boolean   Cl,
                           const Standard_Integer   NW);

  //! Set <NE> the number  of  edges of the wire  number
  //! <WI>.
  Standard_EXPORT void SetWire(const Standard_Integer WI, const Standard_Integer NE);

  //! Set the edge number <EWI> of the  wire <WI>.
  Standard_EXPORT void SetWEdge(const Standard_Integer   WI,
                                const Standard_Integer   EWI,
                                const Standard_Integer   EI,
                                const TopAbs_Orientation Or,
                                const Standard_Boolean   OutL,
                                const Standard_Boolean   Inte,
                                const Standard_Boolean   Dble,
                                const Standard_Boolean   IsoL);

  Standard_Boolean Selected() const;

  void Selected(const Standard_Boolean B);

  Standard_Boolean Back() const;

  void Back(const Standard_Boolean B);

  Standard_Boolean Side() const;

  void Side(const Standard_Boolean B);

  Standard_Boolean Closed() const;

  void Closed(const Standard_Boolean B);

  Standard_Boolean Hiding() const;

  void Hiding(const Standard_Boolean B);

  Standard_Boolean Simple() const;

  void Simple(const Standard_Boolean B);

  Standard_Boolean Cut() const;

  void Cut(const Standard_Boolean B);

  Standard_Boolean WithOutL() const;

  void WithOutL(const Standard_Boolean B);

  Standard_Boolean Plane() const;

  void Plane(const Standard_Boolean B);

  Standard_Boolean Cylinder() const;

  void Cylinder(const Standard_Boolean B);

  Standard_Boolean Cone() const;

  void Cone(const Standard_Boolean B);

  Standard_Boolean Sphere() const;

  void Sphere(const Standard_Boolean B);

  Standard_Boolean Torus() const;

  void Torus(const Standard_Boolean B);

  Standard_Real Size() const;

  void Size(const Standard_Real S);

  TopAbs_Orientation Orientation() const;

  void Orientation(const TopAbs_Orientation O);

  Handle(HLRAlgo_WiresBlock)& Wires();

  HLRBRep_Surface& Geometry();

  Standard_ShortReal Tolerance() const;

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
  Standard_Integer           myFlags;
  Handle(HLRAlgo_WiresBlock) myWires;
  HLRBRep_Surface            myGeometry;
  Standard_Real              mySize;
  Standard_ShortReal         myTolerance;
};

#include <HLRBRep_FaceData.lxx>

#endif // _HLRBRep_FaceData_HeaderFile
