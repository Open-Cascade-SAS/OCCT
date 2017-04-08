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


#include <IGESDimen.hxx>
#include <IGESDimen_Protocol.hxx>
#include <IGESDraw_CircArraySubfigure.hxx>
#include <IGESDraw_ConnectPoint.hxx>
#include <IGESDraw_Drawing.hxx>
#include <IGESDraw_DrawingWithRotation.hxx>
#include <IGESDraw_LabelDisplay.hxx>
#include <IGESDraw_NetworkSubfigure.hxx>
#include <IGESDraw_NetworkSubfigureDef.hxx>
#include <IGESDraw_PerspectiveView.hxx>
#include <IGESDraw_Planar.hxx>
#include <IGESDraw_Protocol.hxx>
#include <IGESDraw_RectArraySubfigure.hxx>
#include <IGESDraw_SegmentedViewsVisible.hxx>
#include <IGESDraw_View.hxx>
#include <IGESDraw_ViewsVisible.hxx>
#include <IGESDraw_ViewsVisibleWithAttr.hxx>
#include <Interface_Protocol.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IGESDraw_Protocol,IGESData_Protocol)

static int THE_IGESDraw_Protocol_deja = 0;
static Handle(Standard_Type) atype01,atype02,atype03,atype04,atype05,atype06,
  atype07,atype08,atype09,atype10,atype11,atype12,atype13,atype14;

IGESDraw_Protocol::IGESDraw_Protocol()
{
  if (THE_IGESDraw_Protocol_deja)
  {
    return;
  }

  THE_IGESDraw_Protocol_deja = 1;
  atype01 = STANDARD_TYPE(IGESDraw_CircArraySubfigure);
  atype02 = STANDARD_TYPE(IGESDraw_ConnectPoint);
  atype03 = STANDARD_TYPE(IGESDraw_Drawing);
  atype04 = STANDARD_TYPE(IGESDraw_DrawingWithRotation);
  atype05 = STANDARD_TYPE(IGESDraw_LabelDisplay);
  atype06 = STANDARD_TYPE(IGESDraw_NetworkSubfigure);
  atype07 = STANDARD_TYPE(IGESDraw_NetworkSubfigureDef);
  atype08 = STANDARD_TYPE(IGESDraw_PerspectiveView);
  atype09 = STANDARD_TYPE(IGESDraw_Planar);
  atype10 = STANDARD_TYPE(IGESDraw_RectArraySubfigure);
  atype11 = STANDARD_TYPE(IGESDraw_SegmentedViewsVisible);
  atype12 = STANDARD_TYPE(IGESDraw_View);
  atype13 = STANDARD_TYPE(IGESDraw_ViewsVisible);
  atype14 = STANDARD_TYPE(IGESDraw_ViewsVisibleWithAttr);
}

    Standard_Integer IGESDraw_Protocol::NbResources () const
      {  return 1;  }

    Handle(Interface_Protocol) IGESDraw_Protocol::Resource
  (const Standard_Integer /*num*/) const
{
  Handle(Interface_Protocol) res = IGESDimen::Protocol();;
  return res;
}

    Standard_Integer IGESDraw_Protocol::TypeNumber
  (const Handle(Standard_Type)& atype) const
{
  if      (atype == atype01) return  1;
  else if (atype == atype02) return  2;
  else if (atype == atype03) return  3;
  else if (atype == atype04) return  4;
  else if (atype == atype05) return  5;
  else if (atype == atype06) return  6;
  else if (atype == atype07) return  7;
  else if (atype == atype08) return  8;
  else if (atype == atype09) return  9;
  else if (atype == atype10) return 10;
  else if (atype == atype11) return 11;
  else if (atype == atype12) return 12;
  else if (atype == atype13) return 13;
  else if (atype == atype14) return 14;
  return 0;
}
