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


#include <Message_ProgressScale.hxx>
#include <TCollection_HAsciiString.hxx>

static const Standard_Real Message_ProgressScale_ZERO = 1e-10;
static const Standard_Real Message_ProgressScale_INFINITE = 1e100;
  
//=======================================================================
//function : Message_ProgressScale
//purpose  : 
//=======================================================================

Message_ProgressScale::Message_ProgressScale () :
     myMin(0.), myMax(100.), myStep(1.), myInfinite(Standard_False),
     myFirst(0.), myLast(1.)
{
}

//=======================================================================
//function : LocalToBase
//purpose  : 
//=======================================================================

Standard_Real Message_ProgressScale::LocalToBase (const Standard_Real val) const
{
  if ( val <= myMin ) return myFirst;
  if ( myMax - myMin <= Message_ProgressScale_ZERO ) return myLast;
  
  if ( ! myInfinite ) {
    if ( val >= myMax ) return myLast;
    return myFirst + ( myLast - myFirst ) * ( val - myMin ) / ( myMax - myMin );
  }
  Standard_Real x = ( val - myMin ) / ( myMax - myMin );
//  return myFirst + ( myLast - myFirst ) * ( 1. - exp ( -x ) ); // exponent
  return myFirst + ( myLast - myFirst ) * x / ( 1. + x );     // hyperbola
}

//=======================================================================
//function : BaseToLocal
//purpose  : 
//=======================================================================

Standard_Real Message_ProgressScale::BaseToLocal (const Standard_Real val) const
{
  if ( myLast - val <= Message_ProgressScale_ZERO ) 
    return myInfinite ? Message_ProgressScale_INFINITE : myMax;
  if ( ! myInfinite )
    return myMin + ( myMax - myMin ) * ( val - myFirst ) / ( myLast - myFirst );
//  Standard_Real x = log ( ( val - myFirst ) / ( myLast - val ) ); // exponent
  Standard_Real x = ( val - myFirst ) / ( myLast - val );         // hyperbola
  return myMin + x * ( myMax - myMin );
}
