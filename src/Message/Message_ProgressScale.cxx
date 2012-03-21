// Copyright (c) 1999-2012 OPEN CASCADE SAS
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

#include <Message_ProgressScale.ixx>

static const Standard_Real ZERO = 1e-10;
static const Standard_Real INFINITE = 1e100;
  
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
  if ( myMax - myMin <= ZERO ) return myLast;
  
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
  if ( myLast - val <= ZERO ) 
    return myInfinite ? INFINITE : myMax;
  if ( ! myInfinite )
    return myMin + ( myMax - myMin ) * ( val - myFirst ) / ( myLast - myFirst );
//  Standard_Real x = log ( ( val - myFirst ) / ( myLast - val ) ); // exponent
  Standard_Real x = ( val - myFirst ) / ( myLast - val );         // hyperbola
  return myMin + x * ( myMax - myMin );
}
