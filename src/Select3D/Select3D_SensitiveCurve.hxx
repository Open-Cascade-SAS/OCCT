// Created on: 1995-03-13
// Created by: Robert COUBLANC
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

#ifndef _Select3D_SensitiveCurve_HeaderFile
#define _Select3D_SensitiveCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <Standard_Type.hxx>

#include <Geom_Curve.hxx>
#include <Handle_Geom_Curve.hxx>
#include <Select3D_SensitivePoly.hxx>
#include <Handle_SelectBasics_EntityOwner.hxx>
#include <Handle_TColgp_HArray1OfPnt.hxx>
#include <Standard_Boolean.hxx>
#include <SelectMgr_SelectingVolumeManager.hxx>
#include <Standard_Real.hxx>
#include <Standard_OStream.hxx>

class Geom_Curve;
class Standard_ConstructionError;
class Standard_OutOfRange;
class SelectBasics_EntityOwner;
class TColgp_HArray1OfPnt;
class TColgp_Array1OfPnt;
class Select3D_SensitiveEntity;
class TopLoc_Location;


//! A framework to define a sensitive 3D curve.
//! In some cases this class can raise Standard_ConstructionError and
//! Standard_OutOfRange exceptions. For more details see Select3D_SensitivePoly.
class Select3D_SensitiveCurve : public Select3D_SensitivePoly
{
public:

  //! Constructs a sensitive curve object defined by the
  //! owner theOwnerId, the curve theCurve, and the
  //! maximum number of points on the curve: theNbPnts.
  Standard_EXPORT Select3D_SensitiveCurve (const Handle(SelectBasics_EntityOwner)& theOwnerId,
                                           const Handle(Geom_Curve)& theCurve,
                                           const Standard_Integer theNbPnts = 17);

  //! Constructs a sensitive curve object defined by the
  //! owner theOwnerId and the set of points ThePoints.
  Standard_EXPORT Select3D_SensitiveCurve (const Handle(SelectBasics_EntityOwner)& theOwnerId,
                                           const Handle(TColgp_HArray1OfPnt)& thePoints);

  //! Creation of Sensitive Curve from Points.
  //!          Warning : This Method should disappear in the next version...
  Standard_EXPORT Select3D_SensitiveCurve (const Handle(SelectBasics_EntityOwner)& theOwnerId,
                                           const TColgp_Array1OfPnt& thePoints);

  //! Returns the copy of this
  Standard_EXPORT virtual Handle(Select3D_SensitiveEntity) GetConnected() Standard_OVERRIDE;

public:

  DEFINE_STANDARD_RTTI(Select3D_SensitiveCurve)

private:

  void loadPoints (const Handle(Geom_Curve)& aCurve,
                   const Standard_Integer NbPoints);

private:

  Handle_Geom_Curve myCurve;     //!< Curve points
};

DEFINE_STANDARD_HANDLE(Select3D_SensitiveCurve, Select3D_SensitivePoly)

#endif // _Select3D_SensitiveCurve_HeaderFile
