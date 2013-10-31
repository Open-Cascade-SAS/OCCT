// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2013 OPEN CASCADE SAS
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

#include <Prs3d_DimensionAspect.hxx>

#include <Aspect_TypeOfLine.hxx>
#include <Graphic3d_AspectText3d.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_TextAspect.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Quantity_Color.hxx>

IMPLEMENT_STANDARD_HANDLE (Prs3d_DimensionAspect, Prs3d_BasicAspect)
IMPLEMENT_STANDARD_RTTIEXT (Prs3d_DimensionAspect, Prs3d_BasicAspect)

//=======================================================================
//function : Constructor
//purpose  : 
//=======================================================================

Prs3d_DimensionAspect::Prs3d_DimensionAspect ()
{
  // Text alignment
  myHorTextAlignment = Prs3d_HTA_Center;
  myVerTextAlignment = Prs3d_VTA_Center;
  // Arrow orientation, will be computed on further steps, by default it is internal.
  myArrowOrientation = Prs3d_DAO_Internal;
  myLineAspect = new Prs3d_LineAspect (Quantity_NOC_LAWNGREEN,Aspect_TOL_SOLID,1.);
  myTextAspect = new Prs3d_TextAspect;
  myTextAspect->Aspect()->SetTextZoomable (Standard_False);
  myTextAspect->SetColor (Quantity_NOC_LAWNGREEN);
  myTextAspect->SetHorizontalJustification (Graphic3d_HTA_CENTER);
  myTextAspect->SetVerticalJustification (Graphic3d_VTA_CENTER);
  myArrowAspect = new Prs3d_ArrowAspect;
  myArrowAspect->SetColor (Quantity_NOC_LAWNGREEN);
  myArrowAspect->SetLength (6.);
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

//=======================================================================
//function : LineAspect
//purpose  : 
//=======================================================================

Handle(Prs3d_LineAspect) Prs3d_DimensionAspect::LineAspect () const
{
  return myLineAspect;
}

//=======================================================================
//function : SetLineAspect
//purpose  : 
//=======================================================================

void Prs3d_DimensionAspect::SetLineAspect(const Handle(Prs3d_LineAspect)& theAspect)
{
  myLineAspect = theAspect;
}

//=======================================================================
//function : TextAspect
//purpose  : 
//=======================================================================

Handle(Prs3d_TextAspect) Prs3d_DimensionAspect::TextAspect () const
{
  return myTextAspect;
}

//=======================================================================
//function : SetTextAspect
//purpose  : 
//=======================================================================

void Prs3d_DimensionAspect::SetTextAspect (const Handle(Prs3d_TextAspect)& theAspect)
{
  myTextAspect = theAspect;
}

//=======================================================================
//function : MakeArrows3D
//purpose  : 
//=======================================================================

void Prs3d_DimensionAspect::MakeArrows3d (const Standard_Boolean isArrows3d)
{
  myIsArrows3d = isArrows3d;
}
 
//=======================================================================
//function : IsArrows3D
//purpose  : 
//======================================================================= 
Standard_Boolean Prs3d_DimensionAspect::IsArrows3d () const
{
  return myIsArrows3d;
}

//=======================================================================
//function : MakeText3D
//purpose  : 
//=======================================================================

void Prs3d_DimensionAspect::MakeText3d (const Standard_Boolean isText3d)
{
  myIsText3d = isText3d;
}
 
//=======================================================================
//function : IsText3D
//purpose  : 
//======================================================================= 
Standard_Boolean Prs3d_DimensionAspect::IsText3d () const
{
  return myIsText3d;
}

//=======================================================================
//function : IsTextShaded
//purpose  : 
//======================================================================= 
Standard_Boolean Prs3d_DimensionAspect::IsTextShaded () const
{
  return myIsTextShaded;
}

//=======================================================================
//function : MakeTextShaded
//purpose  : 
//=======================================================================

void Prs3d_DimensionAspect::MakeTextShaded (const Standard_Boolean isTextShaded)
{
  myIsTextShaded = isTextShaded;
}

//=======================================================================
//function : SetArrowOrientation
//purpose  : 
//======================================================================= 

void Prs3d_DimensionAspect::SetArrowOrientation (const Prs3d_DimensionArrowOrientation theArrowOrient)
{
  myArrowOrientation = theArrowOrient;
}

//=======================================================================
//function : GetArrowOrientation
//purpose  : 
//======================================================================= 

Prs3d_DimensionArrowOrientation Prs3d_DimensionAspect::GetArrowOrientation () const
{
  return myArrowOrientation;
}

//=======================================================================
//function : VerticalTextAlignment
//purpose  : 
//======================================================================= 

Prs3d_VerticalTextAlignment Prs3d_DimensionAspect::VerticalTextAlignment () const
{
  return myVerTextAlignment;
}
   
//=======================================================================
//function : SetVerticalTextAlignment
//purpose  : 
//=======================================================================

void Prs3d_DimensionAspect::SetVerticalTextAlignment (const Prs3d_VerticalTextAlignment theVertTextAlignment)
{
  myVerTextAlignment = theVertTextAlignment;
}

//=======================================================================
//function : HorizontalTextAlignment
//purpose  : 
//======================================================================= 

Prs3d_HorizontalTextAlignment Prs3d_DimensionAspect::HorizontalTextAlignment () const
{
  return myHorTextAlignment;
}
   
//=======================================================================
//function : SetHorizontalTextAlignment
//purpose  : 
//=======================================================================

void Prs3d_DimensionAspect::SetHorizontalTextAlignment (const Prs3d_HorizontalTextAlignment theHorTextAlignment)
{
  myHorTextAlignment = theHorTextAlignment;
}

//=======================================================================
//function : ArrowAspect
//purpose  : 
//=======================================================================

Handle(Prs3d_ArrowAspect) Prs3d_DimensionAspect::ArrowAspect () const
{
  return myArrowAspect;
}

//=======================================================================
//function : SetArrowAspect
//purpose  : 
//=======================================================================

void Prs3d_DimensionAspect::SetArrowAspect (const Handle(Prs3d_ArrowAspect)& theAspect)
{
  myArrowAspect = theAspect;
}
