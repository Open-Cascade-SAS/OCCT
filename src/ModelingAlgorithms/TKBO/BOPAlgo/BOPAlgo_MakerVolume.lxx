// Created by: Eugeny MALTCHIKOV
// Copyright (c) 2014 OPEN CASCADE SAS
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

inline BOPAlgo_MakerVolume::BOPAlgo_MakerVolume()
    : myIntersect(true),
      myAvoidInternalShapes(false)
{
}

//=================================================================================================

inline BOPAlgo_MakerVolume::BOPAlgo_MakerVolume(
  const occ::handle<NCollection_BaseAllocator>& theAllocator)
    : BOPAlgo_Builder(theAllocator),
      myIntersect(true),
      myAvoidInternalShapes(false)
{
}

//=======================================================================
// function : ~BOPAlgo_MakerVolume
// purpose  :
//=======================================================================
inline BOPAlgo_MakerVolume::~BOPAlgo_MakerVolume()
{
  Clear();
}

//=================================================================================================

inline void BOPAlgo_MakerVolume::Clear()
{
  BOPAlgo_Builder::Clear();
  myIntersect = true;
  myBBox      = Bnd_Box();
  mySBox.Nullify();
  myFaces.Clear();
  myAvoidInternalShapes = false;
}

//=================================================================================================

inline void BOPAlgo_MakerVolume::SetIntersect(const bool bIntersect)
{
  myIntersect = bIntersect;
}

//=================================================================================================

inline bool BOPAlgo_MakerVolume::IsIntersect() const
{
  return myIntersect;
}

//=================================================================================================

inline const TopoDS_Solid& BOPAlgo_MakerVolume::Box() const
{
  return mySBox;
}

//=================================================================================================

inline const NCollection_List<TopoDS_Shape>& BOPAlgo_MakerVolume::Faces() const
{
  return myFaces;
}
