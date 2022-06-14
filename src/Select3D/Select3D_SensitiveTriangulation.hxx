// Created on: 1997-05-15
// Created by: Robert COUBLANC
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

#ifndef _Select3D_SensitiveTriangulation_Header
#define _Select3D_SensitiveTriangulation_Header

#include <TColStd_HArray1OfInteger.hxx>
#include <Select3D_SensitiveSet.hxx>

class Poly_Triangle;
class Poly_Triangulation;

//! A framework to define selection of a sensitive entity made of a set of triangles.
class Select3D_SensitiveTriangulation : public Select3D_SensitiveSet
{
  DEFINE_STANDARD_RTTIEXT(Select3D_SensitiveTriangulation, Select3D_SensitiveSet)
public:

  //! Constructs a sensitive triangulation object defined by
  //! the owner theOwnerId, the triangulation theTrg,
  //! the location theInitLoc, and the flag theIsInterior.
  Standard_EXPORT Select3D_SensitiveTriangulation (const Handle(SelectMgr_EntityOwner)& theOwnerId,
                                                   const Handle(Poly_Triangulation)& theTrg,
                                                   const TopLoc_Location& theInitLoc,
                                                   const Standard_Boolean theIsInterior = Standard_True);

  //! Constructs a sensitive triangulation object defined by
  //! the owner theOwnerId, the triangulation theTrg,
  //! the location theInitLoc, the array of free edges
  //! theFreeEdges, the center of gravity theCOG, and the flag theIsInterior.
  //! As free edges and the center of gravity do not have
  //! to be computed later, this syntax reduces computation time.
  Standard_EXPORT Select3D_SensitiveTriangulation (const Handle(SelectMgr_EntityOwner)& theOwnerId,
                                                   const Handle(Poly_Triangulation)& theTrg,
                                                   const TopLoc_Location& theInitLoc,
                                                   const Handle(TColStd_HArray1OfInteger)& theFreeEdges,
                                                   const gp_Pnt& theCOG,
                                                   const Standard_Boolean theIsInterior);
public:

  //! Get last detected triangle.
  //! @param theTriangle [out] triangle node indexes
  //! @return TRUE if defined
  Standard_EXPORT bool LastDetectedTriangle (Poly_Triangle& theTriangle) const;

  //! Get last detected triangle.
  //! @param theTriangle [out] triangle node indexes
  //! @param theTriNodes [out] triangle nodes (with pre-applied transformation)
  //! @return TRUE if defined
  Standard_EXPORT bool LastDetectedTriangle (Poly_Triangle& theTriangle,
                                             gp_Pnt theTriNodes[3]) const;

  //! Return index of last detected triangle within [1..NbTris] range, or -1 if undefined.
  Standard_Integer LastDetectedTriangleIndex() const
  {
    return (myDetectedIdx != -1 && mySensType == Select3D_TOS_INTERIOR && !myBVHPrimIndexes.IsNull())
          ? myBVHPrimIndexes->Value (myDetectedIdx) + 1
          : -1;
  }

public:

  //! Returns the amount of nodes in triangulation
  Standard_EXPORT virtual Standard_Integer NbSubElements() const Standard_OVERRIDE;

  Standard_EXPORT Handle(Select3D_SensitiveEntity) GetConnected() Standard_OVERRIDE;

  const Handle(Poly_Triangulation)& Triangulation() const { return myTriangul; }

  //! Returns the length of array of triangles or edges
  Standard_EXPORT virtual Standard_Integer Size() const Standard_OVERRIDE;

  //! Returns bounding box of triangle/edge with index theIdx
  Standard_EXPORT virtual Select3D_BndBox3d Box (const Standard_Integer theIdx) const Standard_OVERRIDE;

  //! Returns geometry center of triangle/edge with index theIdx
  //! in array along the given axis theAxis
  Standard_EXPORT virtual Standard_Real Center (const Standard_Integer theIdx,
                                                const Standard_Integer theAxis) const Standard_OVERRIDE;

  //! Swaps items with indexes theIdx1 and theIdx2 in array
  Standard_EXPORT virtual void Swap (const Standard_Integer theIdx1,
                                     const Standard_Integer theIdx2) Standard_OVERRIDE;

  //! Returns bounding box of the triangulation. If location
  //! transformation is set, it will be applied
  Standard_EXPORT virtual Select3D_BndBox3d BoundingBox() Standard_OVERRIDE;

  //! Returns center of triangulation. If location transformation
  //! is set, it will be applied
  Standard_EXPORT virtual gp_Pnt CenterOfGeometry() const Standard_OVERRIDE;

  //! Returns true if the shape corresponding to the entity has init location
  Standard_EXPORT virtual Standard_Boolean HasInitLocation() const Standard_OVERRIDE;

  //! Returns inversed location transformation matrix if the shape corresponding
  //! to this entity has init location set. Otherwise, returns identity matrix.
  Standard_EXPORT virtual gp_GTrsf InvInitLocation() const Standard_OVERRIDE;

  const TopLoc_Location& GetInitLocation() const { return myInitLocation; }

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;

  //! Checks whether one or more entities of the set overlap current selecting volume.
  Standard_EXPORT virtual Standard_Boolean Matches (SelectBasics_SelectingVolumeManager& theMgr,
                                                    SelectBasics_PickResult& thePickResult) Standard_OVERRIDE;

protected:

  //! Compute bounding box.
  void computeBoundingBox();

  //! Inner function for transformation application to bounding
  //! box of the triangulation
  Select3D_BndBox3d applyTransformation();

private:

  //! Checks whether the element with index theIdx overlaps the current selecting volume
  Standard_EXPORT virtual Standard_Boolean overlapsElement (SelectBasics_PickResult& thePickResult,
                                                            SelectBasics_SelectingVolumeManager& theMgr,
                                                            Standard_Integer theElemIdx,
                                                            Standard_Boolean theIsFullInside) Standard_OVERRIDE;

  //! Calculates distance from the 3d projection of used-picked screen point to center of the geometry
  Standard_EXPORT virtual Standard_Real distanceToCOG (SelectBasics_SelectingVolumeManager& theMgr) Standard_OVERRIDE;

  //! Checks whether the entity with index theIdx is inside the current selecting volume
  Standard_EXPORT virtual Standard_Boolean elementIsInside (SelectBasics_SelectingVolumeManager& theMgr,
                                                            Standard_Integer theElemIdx,
                                                            Standard_Boolean theIsFullInside) Standard_OVERRIDE;

protected:

  Handle(Poly_Triangulation)       myTriangul;
  TopLoc_Location                  myInitLocation;
  gp_Pnt                           myCDG3D;              //!< Center of the whole triangulation
  Handle(TColStd_HArray1OfInteger) myFreeEdges;
  Standard_Boolean                 mySensType;            //!< Type of sensitivity: boundary or interior
  Standard_Integer                 myPrimitivesNb;       //!< Amount of free edges or triangles depending on sensitivity type
  Handle(TColStd_HArray1OfInteger) myBVHPrimIndexes;     //!< Indexes of edges or triangles for BVH build
  mutable Select3D_BndBox3d        myBndBox;             //!< Bounding box of the whole triangulation
  gp_GTrsf                         myInvInitLocation;
};

DEFINE_STANDARD_HANDLE(Select3D_SensitiveTriangulation, Select3D_SensitiveSet)

#endif // _Select3D_SensitiveTriangulation_Header
