// Created on: 1995-01-25
// Created by: Mister rmi
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _Select3D_SensitiveSegment_HeaderFile
#define _Select3D_SensitiveSegment_HeaderFile

#include <Select3D_SensitiveEntity.hxx>
#include <SelectMgr_SelectingVolumeManager.hxx>

//! A framework to define sensitive zones along a segment
//!          One gives the 3D start and end point
class Select3D_SensitiveSegment : public Select3D_SensitiveEntity
{
  DEFINE_STANDARD_RTTIEXT(Select3D_SensitiveSegment, Select3D_SensitiveEntity)
public:
  //! Constructs the sensitive segment object defined by
  //! the owner theOwnerId, the points theFirstPnt, theLastPnt
  Standard_EXPORT Select3D_SensitiveSegment(const occ::handle<SelectMgr_EntityOwner>& theOwnerId,
                                            const gp_Pnt&                             theFirstPnt,
                                            const gp_Pnt&                             theLastPnt);

  //! changes the start Point of the Segment;
  void SetStartPoint(const gp_Pnt& thePnt) { myStart = thePnt; }

  //! changes the end point of the segment
  void SetEndPoint(const gp_Pnt& thePnt) { myEnd = thePnt; }

  //! gives the 3D start Point of the Segment
  const gp_Pnt& StartPoint() const { return myStart; }

  //! gives the 3D End Point of the Segment
  const gp_Pnt& EndPoint() const { return myEnd; }

  //! Returns the amount of points
  Standard_EXPORT int NbSubElements() const override;

  Standard_EXPORT occ::handle<Select3D_SensitiveEntity> GetConnected() override;

  //! Checks whether the segment overlaps current selecting volume
  Standard_EXPORT bool Matches(SelectBasics_SelectingVolumeManager& theMgr,
                               SelectBasics_PickResult&             thePickResult) override;

  //! Returns center of the segment. If location transformation
  //! is set, it will be applied
  Standard_EXPORT gp_Pnt CenterOfGeometry() const override;

  //! Returns bounding box of the segment. If location
  //! transformation is set, it will be applied
  Standard_EXPORT Select3D_BndBox3d BoundingBox() override;

  //! Returns TRUE if BVH tree is in invalidated state
  bool ToBuildBVH() const override { return false; }

public:
  //! changes the start Point of the Segment;
  void StartPoint(const gp_Pnt& thePnt) { myStart = thePnt; }

  //! changes the end point of the segment
  void EndPoint(const gp_Pnt& thePnt) { myEnd = thePnt; }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const override;

private:
  gp_Pnt myStart; //!< Start point
  gp_Pnt myEnd;   //!< End point
};

#endif // _Select3D_SensitiveSegment_HeaderFile
