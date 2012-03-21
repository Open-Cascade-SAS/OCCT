// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _Standard_ShortReal_HeaderFile
#define _Standard_ShortReal_HeaderFile

#include <float.h>
#include <math.h>
#ifndef _Standard_values_HeaderFile
# include <Standard_values.h>
#endif


#ifndef _Standard_TypeDef_HeaderFile
#include <Standard_TypeDef.hxx>
#endif

class Handle_Standard_Type;

__Standard_API const Handle_Standard_Type& Standard_ShortReal_Type_();
//class Standard_OStream;
//void ShallowDump (const Standard_ShortReal, Standard_OStream& );

         //  *********************************** //
         //       Class methods                  //
         //                                      //
         //  Machine-dependant values            //
         //  Should be taken from include file   //
         //  *********************************** //

//-------------------------------------------------------------------
// ShortRealSmall : Returns the smallest positive ShortReal
//-------------------------------------------------------------------
inline Standard_ShortReal     ShortRealSmall() 
{ return FLT_MIN; }

//-------------------------------------------------------------------
// Abs : Returns the absolute value of a ShortReal
//-------------------------------------------------------------------
inline Standard_ShortReal     Abs(const Standard_ShortReal Value) 
#if defined (__alpha) || defined(DECOSF1)
{ return fabsf(Value); }
#else
{ return float( fabs (Value) ) ; }
#endif

//-------------------------------------------------------------------
// ShortRealDigit : Returns the number of digits of precision in a ShortReal
//-------------------------------------------------------------------
inline Standard_Integer  ShortRealDigits() 
{ return FLT_DIG; }

//-------------------------------------------------------------------
// ShortRealEpsilon : Returns the minimum positive ShortReal such that 
//               1.0 + x is not equal to 1.0
//-------------------------------------------------------------------
inline Standard_ShortReal     ShortRealEpsilon() 
{ return FLT_EPSILON; }

//-------------------------------------------------------------------
// ShortRealFirst : Returns the minimum negative value of a ShortReal
//-------------------------------------------------------------------
inline Standard_ShortReal     ShortRealFirst() 
{ Standard_ShortReal MaxFloatTmp = -FLT_MAX;
  return MaxFloatTmp; }
  
//-------------------------------------------------------------------
// ShortRealFirst10Exp : Returns the minimum value of exponent(base 10) of
//                  a ShortReal.
//-------------------------------------------------------------------
inline Standard_Integer  ShortRealFirst10Exp() 
{ return FLT_MIN_10_EXP; }

//-------------------------------------------------------------------
// ShortRealLast : Returns the maximum value of a ShortReal
//-------------------------------------------------------------------
inline Standard_ShortReal     ShortRealLast() 
{ return  FLT_MAX; }

//-------------------------------------------------------------------
// ShortRealLast10Exp : Returns the maximum value of exponent(base 10) of
//                 a ShortReal.
//-------------------------------------------------------------------
inline Standard_Integer  ShortRealLast10Exp() 
{ return  FLT_MAX_10_EXP; }

//-------------------------------------------------------------------
// ShortRealMantissa : Returns the size in bits of the matissa part of a 
//                ShortReal.
//-------------------------------------------------------------------
inline Standard_Integer  ShortRealMantissa() 
{ return  FLT_MANT_DIG; }

//-------------------------------------------------------------------
// ShortRealRadix : Returns the radix of exponent representation
//-------------------------------------------------------------------
inline Standard_Integer  ShortRealRadix() 
{ return  FLT_RADIX; }

//-------------------------------------------------------------------
// ShortRealSize : Returns the size in bits of an integer
//-------------------------------------------------------------------
inline Standard_Integer  ShortRealSize() 
{ return BITS(Standard_ShortReal); }

//-------------------------------------------------------------------
// Max : Returns the maximum value of two ShortReals
//-------------------------------------------------------------------
inline Standard_ShortReal     Max (const Standard_ShortReal Val1, 
				   const Standard_ShortReal Val2) 
{
  if (Val1 >= Val2) {
    return Val1;
  } else {
    return Val2;
  }
}

//-------------------------------------------------------------------
// Min : Returns the minimum value of two ShortReals
//-------------------------------------------------------------------
inline Standard_ShortReal     Min (const Standard_ShortReal Val1, 
				   const Standard_ShortReal Val2)
{
  if (Val1 <= Val2) {
    return Val1;
  } else {
    return Val2;
  }
}

// ===============================================
// Methods from Standard_Entity class which are redefined:  
//    - Hascode
//    - IsEqual
//    - IsSimilar
//    - Shallowcopy
//    - ShallowDump
// ===============================================

// ==================================
// Methods implemeted in Standard_ShortReal.cxx
// ==================================
//....             Herited from Standard_Storable
__Standard_API Standard_Integer HashCode (const Standard_ShortReal, const Standard_Integer);  
__Standard_API Standard_ShortReal ShallowCopy (const Standard_ShortReal );  

//-------------------------------------------------------------------
// IsEqual : Returns Standard_True if two ShortReals are equal
//-------------------------------------------------------------------
inline Standard_Boolean  IsEqual (const Standard_ShortReal Value1, 
				  const Standard_ShortReal Value2) 
{ return Abs((Value1 - Value2)) < ShortRealSmall(); }

//-------------------------------------------------------------------
// IsSimilar : Returns Standard_True if two ShortReals are equal
//-------------------------------------------------------------------
inline Standard_Boolean  IsSimilar(const Standard_ShortReal One, 
				   const Standard_ShortReal Two)
{ return IsEqual (One,Two); }

#endif


