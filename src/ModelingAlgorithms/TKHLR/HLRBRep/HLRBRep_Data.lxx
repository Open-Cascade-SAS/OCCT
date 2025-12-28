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

inline NCollection_Array1<HLRBRep_EdgeData>& HLRBRep_Data::EDataArray()
{
  return myEData;
}

//=================================================================================================

inline NCollection_Array1<HLRBRep_FaceData>& HLRBRep_Data::FDataArray()
{
  return myFData;
}

//=================================================================================================

inline void HLRBRep_Data::Tolerance(const float tol)
{
  myToler = tol;
}

//=================================================================================================

inline float HLRBRep_Data::Tolerance() const
{
  return myToler;
}

//=================================================================================================

inline HLRAlgo_Projector& HLRBRep_Data::Projector()
{
  return myProj;
}

//=================================================================================================

inline int HLRBRep_Data::NbVertices() const
{
  return myNbVertices;
}

//=================================================================================================

inline int HLRBRep_Data::NbEdges() const
{
  return myNbEdges;
}

//=================================================================================================

inline int HLRBRep_Data::NbFaces() const
{
  return myNbFaces;
}

//=================================================================================================

inline NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& HLRBRep_Data::EdgeMap()
{
  return myEMap;
}

//=================================================================================================

inline NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>& HLRBRep_Data::FaceMap()
{
  return myFMap;
}

//=================================================================================================

inline bool HLRBRep_Data::SimpleHidingFace() const
{
  return iFaceSimp;
}

//=================================================================================================

inline bool HLRBRep_Data::HidingTheFace() const
{
  return iFaceTest;
}

//=================================================================================================

inline bool HLRBRep_Data::MoreInterference() const
{
  return (iInterf <= myNbPoints + 2 * myNbSegments);
}

//=================================================================================================

inline HLRAlgo_Interference& HLRBRep_Data::Interference()
{
  return myIntf;
}

//=================================================================================================

inline bool HLRBRep_Data::EdgeOfTheHidingFace(const int, const HLRBRep_EdgeData& ED) const
{
  return ED.HideCount() == myHideCount - 1;
}
