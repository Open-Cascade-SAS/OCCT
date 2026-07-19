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

//=================================================================================================

inline bool HLRBRep_FaceData::Selected() const
{
  return (myFlags & (int)FMaskSelected) != 0;
}

//=================================================================================================

inline void HLRBRep_FaceData::Selected(const bool B)
{
  if (B)
    myFlags |= (int)FMaskSelected;
  else
    myFlags &= ~((int)FMaskSelected);
}

//=================================================================================================

inline bool HLRBRep_FaceData::Back() const
{
  return (myFlags & (int)FMaskBack) != 0;
}

//=================================================================================================

inline void HLRBRep_FaceData::Back(const bool B)
{
  if (B)
    myFlags |= (int)FMaskBack;
  else
    myFlags &= ~((int)FMaskBack);
}

//=================================================================================================

inline bool HLRBRep_FaceData::Side() const
{
  return (myFlags & (int)FMaskSide) != 0;
}

//=================================================================================================

inline void HLRBRep_FaceData::Side(const bool B)
{
  if (B)
    myFlags |= (int)FMaskSide;
  else
    myFlags &= ~((int)FMaskSide);
}

//=================================================================================================

inline bool HLRBRep_FaceData::Closed() const
{
  return (myFlags & (int)FMaskClosed) != 0;
}

//=================================================================================================

inline void HLRBRep_FaceData::Closed(const bool B)
{
  if (B)
    myFlags |= (int)FMaskClosed;
  else
    myFlags &= ~((int)FMaskClosed);
}

//=================================================================================================

inline bool HLRBRep_FaceData::Hiding() const
{
  return (myFlags & (int)FMaskHiding) != 0;
}

//=================================================================================================

inline void HLRBRep_FaceData::Hiding(const bool B)
{
  if (B)
    myFlags |= (int)FMaskHiding;
  else
    myFlags &= ~((int)FMaskHiding);
}

//=================================================================================================

inline bool HLRBRep_FaceData::Simple() const
{
  return (myFlags & (int)FMaskSimple) != 0;
}

//=================================================================================================

inline void HLRBRep_FaceData::Simple(const bool B)
{
  if (B)
    myFlags |= (int)FMaskSimple;
  else
    myFlags &= ~((int)FMaskSimple);
}

//=================================================================================================

inline bool HLRBRep_FaceData::Cut() const
{
  return (myFlags & (int)FMaskCut) != 0;
}

//=================================================================================================

inline void HLRBRep_FaceData::Cut(const bool B)
{
  if (B)
    myFlags |= (int)FMaskCut;
  else
    myFlags &= ~((int)FMaskCut);
}

//=================================================================================================

inline bool HLRBRep_FaceData::WithOutL() const
{
  return (myFlags & (int)FMaskWithOutL) != 0;
}

//=================================================================================================

inline void HLRBRep_FaceData::WithOutL(const bool B)
{
  if (B)
    myFlags |= (int)FMaskWithOutL;
  else
    myFlags &= ~((int)FMaskWithOutL);
}

//=================================================================================================

inline bool HLRBRep_FaceData::Plane() const
{
  return (myFlags & (int)FMaskPlane) != 0;
}

//=================================================================================================

inline void HLRBRep_FaceData::Plane(const bool B)
{
  if (B)
    myFlags |= (int)FMaskPlane;
  else
    myFlags &= ~((int)FMaskPlane);
}

//=================================================================================================

inline bool HLRBRep_FaceData::Cylinder() const
{
  return (myFlags & (int)FMaskCylinder) != 0;
}

//=================================================================================================

inline void HLRBRep_FaceData::Cylinder(const bool B)
{
  if (B)
    myFlags |= (int)FMaskCylinder;
  else
    myFlags &= ~((int)FMaskCylinder);
}

//=================================================================================================

inline bool HLRBRep_FaceData::Cone() const
{
  return (myFlags & (int)FMaskCone) != 0;
}

//=================================================================================================

inline void HLRBRep_FaceData::Cone(const bool B)
{
  if (B)
    myFlags |= (int)FMaskCone;
  else
    myFlags &= ~((int)FMaskCone);
}

//=================================================================================================

inline bool HLRBRep_FaceData::Sphere() const
{
  return (myFlags & (int)FMaskSphere) != 0;
}

//=================================================================================================

inline void HLRBRep_FaceData::Sphere(const bool B)
{
  if (B)
    myFlags |= (int)FMaskSphere;
  else
    myFlags &= ~((int)FMaskSphere);
}

//=================================================================================================

inline bool HLRBRep_FaceData::Torus() const
{
  return (myFlags & (int)FMaskTorus) != 0;
}

//=================================================================================================

inline void HLRBRep_FaceData::Torus(const bool B)
{
  if (B)
    myFlags |= (int)FMaskTorus;
  else
    myFlags &= ~((int)FMaskTorus);
}

//=================================================================================================

inline double HLRBRep_FaceData::Size() const
{
  return mySize;
}

//=================================================================================================

inline void HLRBRep_FaceData::Size(const double S)
{
  mySize = S;
}

//=================================================================================================

inline TopAbs_Orientation HLRBRep_FaceData::Orientation() const
{
  return ((TopAbs_Orientation)(myFlags & (int)EMaskOrient));
}

//=================================================================================================

inline void HLRBRep_FaceData::Orientation(const TopAbs_Orientation O)
{
  myFlags &= ~(int)EMaskOrient;
  myFlags |= ((int)O & (int)EMaskOrient);
}

//=================================================================================================

inline occ::handle<HLRAlgo_WiresBlock>& HLRBRep_FaceData::Wires()
{
  return myWires;
}

//=================================================================================================

inline HLRBRep_Surface& HLRBRep_FaceData::Geometry()
{
  return myGeometry;
}

//=================================================================================================

inline float HLRBRep_FaceData::Tolerance() const
{
  return myTolerance;
}
