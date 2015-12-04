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

#include <Select3D_SensitiveCurve.hxx>
#include <Precision.hxx>
#include <TColgp_Array1OfPnt.hxx>


IMPLEMENT_STANDARD_RTTIEXT(Select3D_SensitiveCurve,Select3D_SensitivePoly)

//==================================================
// Function: Creation
// Purpose :
//==================================================
Select3D_SensitiveCurve::Select3D_SensitiveCurve (const Handle(SelectBasics_EntityOwner)& theOwnerId,
                                                  const Handle(Geom_Curve)& theCurve,
                                                  const Standard_Integer theNbPnts)
: Select3D_SensitivePoly (theOwnerId, Standard_True, theNbPnts),
  myCurve (theCurve)
{
  loadPoints (theCurve, theNbPnts);
  SetSensitivityFactor (3);
}

//==================================================
// Function: Creation
// Purpose :
//==================================================
Select3D_SensitiveCurve::Select3D_SensitiveCurve (const Handle(SelectBasics_EntityOwner)& theOwnerId,
                                                  const Handle(TColgp_HArray1OfPnt)& thePoints)
: Select3D_SensitivePoly (theOwnerId, thePoints, Standard_True)

{
  SetSensitivityFactor (3);
}

//==================================================
// Function: Creation
// Purpose :
//==================================================
Select3D_SensitiveCurve::Select3D_SensitiveCurve (const Handle(SelectBasics_EntityOwner)& theOwnerId,
                                                  const TColgp_Array1OfPnt& thePoints)
: Select3D_SensitivePoly (theOwnerId, thePoints, Standard_True)
{
  SetSensitivityFactor (3);
}

//==================================================
// Function: loadPoints
// Purpose :
//==================================================
void Select3D_SensitiveCurve::loadPoints (const Handle(Geom_Curve)& theCurve, const Standard_Integer theNbPnts)
{
  Standard_Real aStep = (theCurve->LastParameter() - theCurve->FirstParameter()) / (theNbPnts - 1);
  Standard_Real aParam = theCurve->FirstParameter();
  for (Standard_Integer aPntIdx = 0; aPntIdx < myPolyg.Size(); ++aPntIdx)
  {
    myPolyg.SetPnt (aPntIdx, theCurve->Value (aParam));
    aParam += aStep;
  }
}

//=======================================================================
//function : GetConnected
//purpose  :
//=======================================================================
Handle(Select3D_SensitiveEntity) Select3D_SensitiveCurve::GetConnected()
{
  // Create a copy of this
  Handle(Select3D_SensitiveEntity) aNewEntity;
  // this was constructed using Handle(Geom_Curve)
  if (!myCurve.IsNull())
  {
    aNewEntity = new Select3D_SensitiveCurve (myOwnerId, myCurve);
  }
  // this was constructed using TColgp_HArray1OfPnt
  else
  {
    Standard_Integer aSize = myPolyg.Size();
    Handle(TColgp_HArray1OfPnt) aPoints = new TColgp_HArray1OfPnt (1, aSize);
    // Fill the array with points from mypolyg3d
    for (Standard_Integer anIndex = 1; anIndex <= aSize; ++anIndex)
    {
      aPoints->SetValue (anIndex, myPolyg.Pnt (anIndex-1));
    }
     aNewEntity = new Select3D_SensitiveCurve (myOwnerId, aPoints);
  }

  return aNewEntity;
}