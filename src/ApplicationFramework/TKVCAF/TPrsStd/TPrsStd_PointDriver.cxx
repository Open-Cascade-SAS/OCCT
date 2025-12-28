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

#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_Point.hxx>
#include <Geom_CartesianPoint.hxx>
#include <gp_Pnt.hxx>
#include <Standard_Type.hxx>
#include <TDataXtd_Geometry.hxx>
#include <TDataXtd_Point.hxx>
#include <TDF_Label.hxx>
#include <TPrsStd_PointDriver.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TPrsStd_PointDriver, TPrsStd_Driver)

// #include <TDataStd_Datum.hxx>
//=================================================================================================

TPrsStd_PointDriver::TPrsStd_PointDriver() {}

//=================================================================================================

bool TPrsStd_PointDriver::Update(const TDF_Label&               aLabel,
                                             occ::handle<AIS_InteractiveObject>& anAISObject)
{
  occ::handle<TDataXtd_Point> appoint;

  if (!aLabel.FindAttribute(TDataXtd_Point::GetID(), appoint))
  {
    return false;
  }

  gp_Pnt pnt;
  if (!TDataXtd_Geometry::Point(aLabel, pnt))
  {
    return false;
  }
  occ::handle<Geom_CartesianPoint> apt = new Geom_CartesianPoint(pnt);

  //  Update de l'AIS
  occ::handle<AIS_Point> aistrihed;
  if (anAISObject.IsNull())
    aistrihed = new AIS_Point(apt);
  else
  {
    aistrihed = occ::down_cast<AIS_Point>(anAISObject);
    if (aistrihed.IsNull())
      aistrihed = new AIS_Point(apt);
    else
    {
      aistrihed->SetComponent(apt);
      aistrihed->ResetTransformation();
      aistrihed->SetToUpdate();
      aistrihed->UpdateSelection();
    }
  }
  anAISObject = aistrihed;
  return true;
}
