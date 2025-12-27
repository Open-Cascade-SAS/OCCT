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

#include <Standard_Type.hxx>
#include <Vrml_Material.hxx>
#include <VrmlConverter_Drawer.hxx>
#include <VrmlConverter_IsoAspect.hxx>
#include <VrmlConverter_LineAspect.hxx>
#include <VrmlConverter_PointAspect.hxx>
#include <VrmlConverter_ShadingAspect.hxx>

IMPLEMENT_STANDARD_RTTIEXT(VrmlConverter_Drawer, Standard_Transient)

VrmlConverter_Drawer::VrmlConverter_Drawer()
    : myNbPoints(17),
      myIsoOnPlane(false),
      myFreeBoundaryDraw(true),
      myUnFreeBoundaryDraw(true),
      myWireDraw(true),
      myChordialDeviation(0.1),
      myTypeOfDeflection(Aspect_TOD_RELATIVE),
      myMaximalParameterValue(500.),
      myDeviationCoefficient(0.001),
      myDrawHiddenLine(false)
{
}

void VrmlConverter_Drawer::SetTypeOfDeflection(const Aspect_TypeOfDeflection aTypeOfDeflection)
{
  myTypeOfDeflection = aTypeOfDeflection;
}

Aspect_TypeOfDeflection VrmlConverter_Drawer::TypeOfDeflection() const
{
  return myTypeOfDeflection;
}

void VrmlConverter_Drawer::SetMaximalChordialDeviation(const double aChordialDeviation)
{
  myChordialDeviation = aChordialDeviation;
}

double VrmlConverter_Drawer::MaximalChordialDeviation() const
{
  return myChordialDeviation;
}

void VrmlConverter_Drawer::SetDeviationCoefficient(const double aCoefficient)
{
  myDeviationCoefficient = aCoefficient;
}

double VrmlConverter_Drawer::DeviationCoefficient() const
{
  return myDeviationCoefficient;
}

void VrmlConverter_Drawer::SetDiscretisation(const int d)
{
  myNbPoints = d;
}

int VrmlConverter_Drawer::Discretisation() const
{
  return myNbPoints;
}

void VrmlConverter_Drawer::SetMaximalParameterValue(const double Value)
{
  myMaximalParameterValue = Value;
}

double VrmlConverter_Drawer::MaximalParameterValue() const
{
  return myMaximalParameterValue;
}

void VrmlConverter_Drawer::SetIsoOnPlane(const bool OnOff)
{
  myIsoOnPlane = OnOff;
}

bool VrmlConverter_Drawer::IsoOnPlane() const
{
  return myIsoOnPlane;
}

occ::handle<VrmlConverter_IsoAspect> VrmlConverter_Drawer::UIsoAspect()
{
  if (myUIsoAspect.IsNull())
  {
    occ::handle<Vrml_Material> m = new Vrml_Material;
    myUIsoAspect            = new VrmlConverter_IsoAspect(m, false, 1);
  }
  return myUIsoAspect;
}

void VrmlConverter_Drawer::SetUIsoAspect(const occ::handle<VrmlConverter_IsoAspect>& anAspect)
{
  myUIsoAspect = anAspect;
}

occ::handle<VrmlConverter_IsoAspect> VrmlConverter_Drawer::VIsoAspect()
{
  if (myVIsoAspect.IsNull())
  {
    occ::handle<Vrml_Material> m = new Vrml_Material;
    myVIsoAspect            = new VrmlConverter_IsoAspect(m, false, 1);
  }
  return myVIsoAspect;
}

void VrmlConverter_Drawer::SetVIsoAspect(const occ::handle<VrmlConverter_IsoAspect>& anAspect)
{
  myVIsoAspect = anAspect;
}

occ::handle<VrmlConverter_LineAspect> VrmlConverter_Drawer::FreeBoundaryAspect()
{
  if (myFreeBoundaryAspect.IsNull())
  {
    occ::handle<Vrml_Material> m = new Vrml_Material;
    myFreeBoundaryAspect    = new VrmlConverter_LineAspect(m, false);
  }
  return myFreeBoundaryAspect;
}

void VrmlConverter_Drawer::SetFreeBoundaryAspect(const occ::handle<VrmlConverter_LineAspect>& anAspect)
{
  myFreeBoundaryAspect = anAspect;
}

void VrmlConverter_Drawer::SetFreeBoundaryDraw(const bool OnOff)
{
  myFreeBoundaryDraw = OnOff;
}

bool VrmlConverter_Drawer::FreeBoundaryDraw() const
{
  return myFreeBoundaryDraw;
}

occ::handle<VrmlConverter_LineAspect> VrmlConverter_Drawer::WireAspect()
{
  if (myWireAspect.IsNull())
  {
    occ::handle<Vrml_Material> m = new Vrml_Material;
    myWireAspect            = new VrmlConverter_LineAspect(m, false);
  }
  return myWireAspect;
}

void VrmlConverter_Drawer::SetWireAspect(const occ::handle<VrmlConverter_LineAspect>& anAspect)
{
  myWireAspect = anAspect;
}

void VrmlConverter_Drawer::SetWireDraw(const bool OnOff)
{
  myWireDraw = OnOff;
}

bool VrmlConverter_Drawer::WireDraw() const
{
  return myWireDraw;
}

occ::handle<VrmlConverter_LineAspect> VrmlConverter_Drawer::UnFreeBoundaryAspect()
{
  if (myUnFreeBoundaryAspect.IsNull())
  {
    occ::handle<Vrml_Material> m = new Vrml_Material;
    myUnFreeBoundaryAspect  = new VrmlConverter_LineAspect(m, false);
  }
  return myUnFreeBoundaryAspect;
}

void VrmlConverter_Drawer::SetUnFreeBoundaryAspect(const occ::handle<VrmlConverter_LineAspect>& anAspect)
{
  myUnFreeBoundaryAspect = anAspect;
}

void VrmlConverter_Drawer::SetUnFreeBoundaryDraw(const bool OnOff)
{
  myUnFreeBoundaryDraw = OnOff;
}

bool VrmlConverter_Drawer::UnFreeBoundaryDraw() const
{
  return myUnFreeBoundaryDraw;
}

occ::handle<VrmlConverter_LineAspect> VrmlConverter_Drawer::LineAspect()
{
  if (myLineAspect.IsNull())
  {
    occ::handle<Vrml_Material> m = new Vrml_Material;
    myLineAspect            = new VrmlConverter_LineAspect(m, false);
  }
  return myLineAspect;
}

void VrmlConverter_Drawer::SetLineAspect(const occ::handle<VrmlConverter_LineAspect>& anAspect)
{
  myLineAspect = anAspect;
}

occ::handle<VrmlConverter_PointAspect> VrmlConverter_Drawer::PointAspect()
{
  if (myPointAspect.IsNull())
  {
    occ::handle<Vrml_Material> m = new Vrml_Material;
    myPointAspect           = new VrmlConverter_PointAspect(m, false);
  }
  return myPointAspect;
}

void VrmlConverter_Drawer::SetPointAspect(const occ::handle<VrmlConverter_PointAspect>& anAspect)
{
  myPointAspect = anAspect;
}

occ::handle<VrmlConverter_ShadingAspect> VrmlConverter_Drawer::ShadingAspect()
{
  if (myShadingAspect.IsNull())
  {
    myShadingAspect = new VrmlConverter_ShadingAspect;
  }
  return myShadingAspect;
}

void VrmlConverter_Drawer::SetShadingAspect(const occ::handle<VrmlConverter_ShadingAspect>& anAspect)
{
  myShadingAspect = anAspect;
}

bool VrmlConverter_Drawer::DrawHiddenLine() const
{
  return myDrawHiddenLine;
}

void VrmlConverter_Drawer::EnableDrawHiddenLine()
{
  myDrawHiddenLine = true;
}

void VrmlConverter_Drawer::DisableDrawHiddenLine()
{
  myDrawHiddenLine = false;
}

occ::handle<VrmlConverter_LineAspect> VrmlConverter_Drawer::HiddenLineAspect()
{
  if (myHiddenLineAspect.IsNull())
  {
    occ::handle<Vrml_Material> m = new Vrml_Material;
    myHiddenLineAspect      = new VrmlConverter_LineAspect(m, false);
  }
  return myHiddenLineAspect;
}

void VrmlConverter_Drawer::SetHiddenLineAspect(const occ::handle<VrmlConverter_LineAspect>& anAspect)
{
  myHiddenLineAspect = anAspect;
}

occ::handle<VrmlConverter_LineAspect> VrmlConverter_Drawer::SeenLineAspect()
{
  if (mySeenLineAspect.IsNull())
  {
    occ::handle<Vrml_Material> m = new Vrml_Material;
    mySeenLineAspect        = new VrmlConverter_LineAspect(m, false);
  }
  return mySeenLineAspect;
}

void VrmlConverter_Drawer::SetSeenLineAspect(const occ::handle<VrmlConverter_LineAspect>& anAspect)
{
  mySeenLineAspect = anAspect;
}
