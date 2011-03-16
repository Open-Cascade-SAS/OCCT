// File:	XCAFPrs_Style.cxx
// Created:	Fri Aug 11 19:36:48 2000
// Author:	Andrey BETENEV
//		<abv@doomox.nnov.matra-dtv.fr>

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
