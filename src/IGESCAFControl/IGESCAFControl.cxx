// Created on: 2000-08-16
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


#include <IGESCAFControl.ixx>

//=======================================================================
//function : DecodeColor
//purpose  : 
//=======================================================================

Quantity_Color IGESCAFControl::DecodeColor (const Standard_Integer color)
{
  switch ( color ) {
  case 1: return Quantity_Color ( Quantity_NOC_BLACK );
  case 2: return Quantity_Color ( Quantity_NOC_RED );
  case 3: return Quantity_Color ( Quantity_NOC_GREEN );
  case 4: return Quantity_Color ( Quantity_NOC_BLUE1 );
  case 5: return Quantity_Color ( Quantity_NOC_YELLOW );
  case 6: return Quantity_Color ( Quantity_NOC_MAGENTA1 );
  case 7: return Quantity_Color ( Quantity_NOC_CYAN1 );
  case 8: 
  default:return Quantity_Color ( Quantity_NOC_WHITE );
  }
}

//=======================================================================
//function : DecodeColor
//purpose  : 
//=======================================================================

Standard_Integer IGESCAFControl::EncodeColor (const Quantity_Color &col)
{
  Standard_Integer code = 0;
  if ( Abs ( col.Red() - 1. ) <= col.Epsilon() ) code |= 0x001;
  else if ( Abs ( col.Red() ) > col.Epsilon() ) return 0;
  if ( Abs ( col.Green() - 1. ) <= col.Epsilon() ) code |= 0x010;
  else if ( Abs ( col.Green() ) > col.Epsilon() ) return 0;
  if ( Abs ( col.Blue() - 1. ) <= col.Epsilon() ) code |= 0x100;
  else if ( Abs ( col.Blue() ) > col.Epsilon() ) return 0;

  switch ( code ) {
  case 0x000: return 1;
  case 0x001: return 2;
  case 0x010: return 3;
  case 0x100: return 4;
  case 0x011: return 5;
  case 0x101: return 6;
  case 0x110: return 7;
  case 0x111:  
  default   : return 8;
  }
}
