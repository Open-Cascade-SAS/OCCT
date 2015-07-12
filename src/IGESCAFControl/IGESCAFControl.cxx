// Created on: 2000-08-16
// Created by: Andrey BETENEV
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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


#include <IGESCAFControl.hxx>
#include <Quantity_Color.hxx>

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
