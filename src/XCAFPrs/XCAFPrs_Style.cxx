// Created on: 2000-08-11
// Created by: Andrey BETENEV
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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


#include <XCAFPrs_Style.ixx>

//=======================================================================
//function : XCAFPrs_Style
//purpose  : 
//=======================================================================

XCAFPrs_Style::XCAFPrs_Style () : 
       defColorSurf(Standard_False),
       defColorCurv(Standard_False),
       myVisibility(Standard_True)
{
}

//=======================================================================
//function : IsSetColorSurf
//purpose  : 
//=======================================================================

Standard_Boolean XCAFPrs_Style::IsSetColorSurf () const
{
  return defColorSurf;
}

//=======================================================================
//function : GetColorSurf
//purpose  : 
//=======================================================================

Quantity_Color XCAFPrs_Style::GetColorSurf () const
{
  return myColorSurf;
}

//=======================================================================
//function : SetColorSurf
//purpose  : 
//=======================================================================

void XCAFPrs_Style::SetColorSurf (const Quantity_Color &col) 
{
  myColorSurf = col;
  defColorSurf = Standard_True;
}

//=======================================================================
//function : UnSetColorSurf
//purpose  : 
//=======================================================================

void XCAFPrs_Style::UnSetColorSurf () 
{
  defColorSurf = Standard_False;
  myColorSurf.SetValues ( Quantity_NOC_YELLOW );
}

//=======================================================================
//function : IsSetColorCurv
//purpose  : 
//=======================================================================

Standard_Boolean XCAFPrs_Style::IsSetColorCurv () const
{
  return defColorCurv;
}

//=======================================================================
//function : GetColorCurv
//purpose  : 
//=======================================================================

Quantity_Color XCAFPrs_Style::GetColorCurv () const
{
  return myColorCurv;
}

//=======================================================================
//function : SetColorCurv
//purpose  : 
//=======================================================================

void XCAFPrs_Style::SetColorCurv (const Quantity_Color &col) 
{
  myColorCurv = col;
  defColorCurv = Standard_True;
}

//=======================================================================
//function : UnSetColorCurv
//purpose  : 
//=======================================================================

void XCAFPrs_Style::UnSetColorCurv () 
{
  defColorCurv = Standard_False;
  myColorCurv.SetValues ( Quantity_NOC_YELLOW );
}

//=======================================================================
//function : SetVisibility
//purpose  : 
//=======================================================================

void XCAFPrs_Style::SetVisibility (const Standard_Boolean visibility)
{
  myVisibility = visibility;
  if ( ! visibility ) { UnSetColorSurf(); UnSetColorCurv(); } // for hash codes
}

//=======================================================================
//function : IsVisible
//purpose  : 
//=======================================================================

Standard_Boolean XCAFPrs_Style::IsVisible () const
{
  return myVisibility;
}

//=======================================================================
//function : IsEqual
//purpose  : 
//=======================================================================

Standard_Boolean XCAFPrs_Style::IsEqual (const XCAFPrs_Style &other) const
{
  return myVisibility == other.myVisibility &&
         ( ! myVisibility || 
	   ( defColorSurf == other.defColorSurf &&
	     defColorCurv == other.defColorCurv &&
             ( ! defColorSurf || myColorSurf == other.myColorSurf ) &&
             ( ! defColorCurv || myColorCurv == other.myColorCurv ) ) );
}

//=======================================================================
//function : HashCode
//purpose  : 
//=======================================================================

Standard_Integer XCAFPrs_Style::HashCode (const XCAFPrs_Style& S, const Standard_Integer Upper)
{
  int  *meintPtr  = (int*)&S;
  Standard_Integer aHashCode=0, i, aSize = sizeof(S) / sizeof(int);
  
  for (i = 0;  i < aSize; i++, meintPtr++) {
    aHashCode = aHashCode ^ *meintPtr;
  }
  return ::HashCode( aHashCode, Upper);
}

//=======================================================================
//function : IsEqual
//purpose  : 
//=======================================================================

Standard_Boolean XCAFPrs_Style::IsEqual (const XCAFPrs_Style& S1, const XCAFPrs_Style& S2)
{
  return S1.IsEqual(S2);
}
