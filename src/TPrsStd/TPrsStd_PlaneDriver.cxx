// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <TPrsStd_PlaneDriver.ixx>

#include <TDF_Label.hxx>
#include <TDataXtd_Plane.hxx>
//#include <TDataStd_Datum.hxx>
#include <TDataXtd_Geometry.hxx>
#include <AIS_Plane.hxx>
#include <AIS_Drawer.hxx>
#include <AIS_InteractiveContext.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Tool.hxx>
#include <Geom_Plane.hxx>
#include <gp_Pln.hxx>

//=======================================================================
//function :
//purpose  : 
//=======================================================================
TPrsStd_PlaneDriver::TPrsStd_PlaneDriver()
{
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================
Standard_Boolean TPrsStd_PlaneDriver::Update (const TDF_Label& aLabel,
					     Handle(AIS_InteractiveObject)& anAISObject) 
{
  Handle(TDataXtd_Plane) apPlane;

  if ( !aLabel.FindAttribute(TDataXtd_Plane::GetID(), apPlane) ) {
    return Standard_False;
  }  

  gp_Pln pln;
  if (!TDataXtd_Geometry::Plane(aLabel,pln)) {
    return Standard_False;
  }
  Handle(Geom_Plane) apt = new Geom_Plane(pln);

  //  Update AIS
  Handle(AIS_Plane) aisplane;
  if (anAISObject.IsNull())
    aisplane = new AIS_Plane(apt,pln.Location());
  else {
    aisplane = Handle(AIS_Plane)::DownCast(anAISObject);
    if (aisplane.IsNull()) 
      aisplane = new AIS_Plane(apt,pln.Location());
    else {
      aisplane->SetComponent(apt); 
      aisplane->SetCenter(pln.Location());
      aisplane->ResetLocation();
      aisplane->SetToUpdate();
      aisplane->UpdateSelection();
    }
  }
  anAISObject = aisplane;
  return Standard_True;
}

