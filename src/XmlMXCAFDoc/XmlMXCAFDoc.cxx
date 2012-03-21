// Created on: 2001-09-11
// Created by: Julia DOROVSKIKH
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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


#include <XmlMXCAFDoc.ixx>

#include <XmlMXCAFDoc_AreaDriver.hxx>
#include <XmlMXCAFDoc_CentroidDriver.hxx>
#include <XmlMXCAFDoc_ColorDriver.hxx>
#include <XmlMXCAFDoc_GraphNodeDriver.hxx>
#include <XmlMXCAFDoc_LocationDriver.hxx>
#include <XmlMXCAFDoc_VolumeDriver.hxx>
#include <XmlMXCAFDoc_DatumDriver.hxx>
#include <XmlMXCAFDoc_DimTolDriver.hxx>
#include <XmlMXCAFDoc_MaterialDriver.hxx>

#include <XmlMXCAFDoc_ColorToolDriver.hxx>
#include <XmlMXCAFDoc_DocumentToolDriver.hxx>
#include <XmlMXCAFDoc_LayerToolDriver.hxx>
#include <XmlMXCAFDoc_ShapeToolDriver.hxx>
#include <XmlMXCAFDoc_DimTolToolDriver.hxx>
#include <XmlMXCAFDoc_MaterialToolDriver.hxx>

#include <XmlMNaming_NamedShapeDriver.hxx>
#include <TNaming_NamedShape.hxx>

//=======================================================================
//function : AddDrivers
//purpose  : 
//=======================================================================
void XmlMXCAFDoc::AddDrivers (const Handle(XmlMDF_ADriverTable)& aDriverTable,
                              const Handle(CDM_MessageDriver)&   anMsgDrv)
{
  aDriverTable -> AddDriver (new XmlMXCAFDoc_AreaDriver      (anMsgDrv));
  aDriverTable -> AddDriver (new XmlMXCAFDoc_CentroidDriver  (anMsgDrv));
  aDriverTable -> AddDriver (new XmlMXCAFDoc_ColorDriver     (anMsgDrv));
  aDriverTable -> AddDriver (new XmlMXCAFDoc_GraphNodeDriver (anMsgDrv));
  
  //oan: changes for sharing locations map
  Handle(XmlMNaming_NamedShapeDriver) aNamedShapeDriver;
  aDriverTable->GetDriver(STANDARD_TYPE(TNaming_NamedShape), aNamedShapeDriver);
  
  Handle(XmlMXCAFDoc_LocationDriver) aLocationDriver = new XmlMXCAFDoc_LocationDriver (anMsgDrv);
  if( !aNamedShapeDriver.IsNull() )
  {
    aLocationDriver->SetSharedLocations( &(aNamedShapeDriver->GetShapesLocations()) );
  }
  
  aDriverTable -> AddDriver( aLocationDriver);
  aDriverTable -> AddDriver (new XmlMXCAFDoc_VolumeDriver    (anMsgDrv));
  aDriverTable -> AddDriver (new XmlMXCAFDoc_DatumDriver     (anMsgDrv));
  aDriverTable -> AddDriver (new XmlMXCAFDoc_DimTolDriver    (anMsgDrv));
  aDriverTable -> AddDriver (new XmlMXCAFDoc_MaterialDriver  (anMsgDrv));

  aDriverTable -> AddDriver (new XmlMXCAFDoc_ColorToolDriver    (anMsgDrv));
  aDriverTable -> AddDriver (new XmlMXCAFDoc_DocumentToolDriver (anMsgDrv));
  aDriverTable -> AddDriver (new XmlMXCAFDoc_LayerToolDriver    (anMsgDrv));
  aDriverTable -> AddDriver (new XmlMXCAFDoc_ShapeToolDriver    (anMsgDrv));
  aDriverTable -> AddDriver (new XmlMXCAFDoc_DimTolToolDriver   (anMsgDrv));
  aDriverTable -> AddDriver (new XmlMXCAFDoc_MaterialToolDriver (anMsgDrv));
}
