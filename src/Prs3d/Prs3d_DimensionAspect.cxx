// Copyright (c) 1998-1999 Matra Datavision
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

#include <Prs3d_DimensionAspect.hxx>

#include <Aspect_TypeOfLine.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <Standard_Dump.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Prs3d_DimensionAspect, Prs3d_BasicAspect)

//=======================================================================
//function : Constructor
//purpose  :
//=======================================================================
Prs3d_DimensionAspect::Prs3d_DimensionAspect()
: myLineAspect        (new Prs3d_LineAspect (Quantity_NOC_LAWNGREEN, Aspect_TOL_SOLID, 1.0)),
  myTextAspect        (new Prs3d_TextAspect()),
  myArrowAspect       (new Prs3d_ArrowAspect()),
  myValueStringFormat ("%g"),
  myExtensionSize     (6.0),
  myArrowTailSize     (6.0),
  myArrowOrientation  (Prs3d_DAO_Fit),
  myTextHPosition     (Prs3d_DTHP_Fit),
  myTextVPosition     (Prs3d_DTVP_Center),
  myToDisplayUnits    (Standard_False),
  myIsText3d          (Standard_False),
  myIsTextShaded      (Standard_False),
  myIsArrows3d        (Standard_False)
{
  myTextAspect->Aspect()->SetTextZoomable (Standard_False);
  myTextAspect->SetColor (Quantity_NOC_LAWNGREEN);
  myTextAspect->SetHorizontalJustification (Graphic3d_HTA_CENTER);
  myTextAspect->SetVerticalJustification   (Graphic3d_VTA_CENTER);
  
  myArrowAspect->SetColor (Quantity_NOC_LAWNGREEN);
  myArrowAspect->SetAngle (M_PI * 12.0 / 180.0);
  myArrowAspect->SetLength (6.0);
}

//=======================================================================
//function : SetCommonColor
//purpose  : Sets the same color for all parts of dimension:
//           lines, arrows and text.
//=======================================================================
void Prs3d_DimensionAspect::SetCommonColor (const Quantity_Color& theColor)
{
  myLineAspect->SetColor (theColor);
  myTextAspect->SetColor (theColor);
  myArrowAspect->SetColor (theColor);
}

// =======================================================================
// function : DumpJson
// purpose  :
// =======================================================================
void Prs3d_DimensionAspect::DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN (theOStream)

  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myLineAspect.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myTextAspect.get())
  OCCT_DUMP_FIELD_VALUES_DUMPED (theOStream, theDepth, myArrowAspect.get())

  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myValueStringFormat)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myExtensionSize)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myArrowTailSize)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myArrowOrientation)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myTextHPosition)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myTextVPosition)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myToDisplayUnits)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myIsText3d)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myIsTextShaded)
  OCCT_DUMP_FIELD_VALUE_NUMERICAL (theOStream, myIsArrows3d)
}
