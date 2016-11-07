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

#ifndef _Standard_Real_HeaderFile
#define _Standard_Real_HeaderFile

#include <cmath>
#include <float.h>
#include <Standard_values.h>
#include <Standard_math.hxx>
#include <Standard_TypeDef.hxx>

// ===============================================
// Methods from Standard_Entity class which are redefined:  
//    - Hascode
//    - IsEqual
// ===============================================

// ==================================
// Methods implemeted in Standard_Real.cxx
// ==================================
__Standard_API Standard_Integer HashCode    (const Standard_Real, const Standard_Integer);  

__Standard_API Standard_Real    ACos        (const Standard_Real );
__Standard_API Standard_Real    ACosApprox  (const Standard_Real );
__Standard_API Standard_Real    ASin        (const Standard_Real );
__Standard_API Standard_Real    ATan2       (const Standard_Real , const Standard_Real );
__Standard_API Standard_Real    NextAfter   (const Standard_Real , const Standard_Real );

//! Returns |a| if b >= 0; -|a| if b < 0.
__Standard_API Standard_Real    Sign(const Standard_Real a, const Standard_Real b);

__Standard_API Standard_Real    ATanh       (const Standard_Real );
__Standard_API Standard_Real    ACosh       (const Standard_Real );
__Standard_API Standard_Real    Sinh       (const Standard_Real );
__Standard_API Standard_Real    Cosh       (const Standard_Real );
__Standard_API Standard_Real    Log         (const Standard_Real );
__Standard_API Standard_Real    Sqrt        (const Standard_Real );

//-------------------------------------------------------------------
// RealSmall : Returns the smallest positive real
//-------------------------------------------------------------------
inline Standard_Real     RealSmall() 
{ return DBL_MIN; }

//-------------------------------------------------------------------
// Abs : Returns the absolute value of a real
//-------------------------------------------------------------------
inline Standard_Real     Abs(const Standard_Real Value) 
{ return fabs(Value); }


//-------------------------------------------------------------------
// IsEqual : Returns Standard_True if two reals are equal
//-------------------------------------------------------------------
inline Standard_Boolean  IsEqual (const Standard_Real Value1, 
				  const Standard_Real Value2) 
{ return Abs((Value1 - Value2)) < RealSmall(); }

         //  *********************************** //
         //       Class methods                  //
         //                                      //
         //  Machine-dependant values            //
         //  Should be taken from include file   //
         //  *********************************** //


//-------------------------------------------------------------------
// RealDigit : Returns the number of digits of precision in a real
//-------------------------------------------------------------------
inline Standard_Integer  RealDigits() 
{ return DBL_DIG; }

//-------------------------------------------------------------------
// RealEpsilon : Returns the minimum positive real such that 
//               1.0 + x is not equal to 1.0
//-------------------------------------------------------------------
inline Standard_Real     RealEpsilon() 
{ return DBL_EPSILON; }

//-------------------------------------------------------------------
// RealFirst : Returns the minimum negative value of a real
//-------------------------------------------------------------------
inline Standard_Real     RealFirst() 
{ return -DBL_MAX; }
  
//-------------------------------------------------------------------
// RealFirst10Exp : Returns the minimum value of exponent(base 10) of
//                  a real.
//-------------------------------------------------------------------
inline Standard_Integer  RealFirst10Exp() 
{ return DBL_MIN_10_EXP; }

//-------------------------------------------------------------------
// RealLast : Returns the maximum value of a real
//-------------------------------------------------------------------
inline Standard_Real     RealLast() 
{ return  DBL_MAX; }

//-------------------------------------------------------------------
// RealLast10Exp : Returns the maximum value of exponent(base 10) of
//                 a real.
//-------------------------------------------------------------------
inline Standard_Integer  RealLast10Exp() 
{ return  DBL_MAX_10_EXP; }

//-------------------------------------------------------------------
// RealMantissa : Returns the size in bits of the matissa part of a 
//                real.
//-------------------------------------------------------------------
inline Standard_Integer  RealMantissa() 
{ return  DBL_MANT_DIG; }

//-------------------------------------------------------------------
// RealRadix : Returns the radix of exponent representation
//-------------------------------------------------------------------
inline Standard_Integer  RealRadix() 
{ return  FLT_RADIX; }

//-------------------------------------------------------------------
// RealSize : Returns the size in bits of an integer
//-------------------------------------------------------------------
inline Standard_Integer  RealSize() 
{ return BITS(Standard_Real); }



         //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//
         //   End of machine-dependant values   //
         //=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=//


//-------------------------------------------------------------------
// IntToReal : Converts an integer in a real
//-------------------------------------------------------------------
inline Standard_Real     IntToReal(const Standard_Integer Value) 
{ return Value; }

//-------------------------------------------------------------------
// ATan : Returns the value of the arc tangent of a real
//-------------------------------------------------------------------
inline Standard_Real     ATan(const Standard_Real Value) 
{ return atan(Value); }


//-------------------------------------------------------------------
// Ceiling : Returns the smallest integer not less than a real
//-------------------------------------------------------------------
inline Standard_Real     Ceiling (const Standard_Real Value) 
{ return ceil(Value); }

//-------------------------------------------------------------------
// Cos : Returns the cosine of a real
//-------------------------------------------------------------------
inline Standard_Real     Cos (const Standard_Real Value) 
{ return cos(Value); }


//-------------------------------------------------------------------
// Epsilon : The function returns absolute value of difference
//           between 'Value' and other nearest value of
//           Standard_Real type.
//           Nearest value is choseen in direction of infinity
//           the same sign as 'Value'.
//           If 'Value' is 0 then returns minimal positive value
//           of Standard_Real type.
//-------------------------------------------------------------------
inline Standard_Real     Epsilon (const Standard_Real Value) 
{
  Standard_Real aEpsilon;

  if (Value>=0.0){
    aEpsilon = NextAfter(Value, RealLast()) - Value;
  } else {
    aEpsilon = Value - NextAfter(Value, RealFirst());
  }
  return aEpsilon;
}

//-------------------------------------------------------------------
// Exp : Returns the exponential function of a real
//-------------------------------------------------------------------
inline Standard_Real     Exp (const Standard_Real Value) 
{ return exp(Value); }

//-------------------------------------------------------------------
// Floor : Return the largest integer not greater than a real
//-------------------------------------------------------------------
inline Standard_Real     Floor (const Standard_Real Value) 
{ return floor(Value); }

//-------------------------------------------------------------------
// IntegerPart : Returns the integer part of a real
//-------------------------------------------------------------------
inline Standard_Real     IntegerPart (const Standard_Real Value) 
{ return ( (Value>0) ? floor(Value) : ceil(Value) ); }


//-------------------------------------------------------------------
// Log10 : Returns the base-10 logarithm of a real 
//-------------------------------------------------------------------
inline Standard_Real     Log10 (const Standard_Real Value) 
{ return log10(Value); }

//-------------------------------------------------------------------
// Max : Returns the maximum value of two reals
//-------------------------------------------------------------------
inline Standard_Real     Max (const Standard_Real Val1, 
                              const Standard_Real Val2) 
{
  return Val1 >= Val2 ? Val1 : Val2;
}

//-------------------------------------------------------------------
// Min : Returns the minimum value of two reals
//-------------------------------------------------------------------
inline Standard_Real     Min (const Standard_Real Val1, 
                              const Standard_Real Val2)
{
  return Val1 <= Val2 ? Val1 : Val2;
}

//-------------------------------------------------------------------
// Pow : Returns a real to a given power
//-------------------------------------------------------------------
inline Standard_Real     Pow (const Standard_Real Value, const Standard_Real P)
{ return pow(Value,P); }

//-------------------------------------------------------------------
// RealPart : Returns the fractional part of a real.
//-------------------------------------------------------------------
inline  Standard_Real    RealPart (const Standard_Real Value) 
{ return fabs(IntegerPart(Value) - Value); }

//-------------------------------------------------------------------
// RealToInt : Returns the real converted to nearest valid integer.
//             If input value is out of valid range for integers,
//             minimal or maximal possible integer is returned.
//-------------------------------------------------------------------
inline  Standard_Integer RealToInt (const Standard_Real Value) 
{ 
  // Note that on WNT under MS VC++ 8.0 conversion of double value less 
  // than INT_MIN or greater than INT_MAX to integer will cause signal 
  // "Floating point multiple trap" (OCC17861)
  return Value < INT_MIN ? INT_MIN
    : Value > INT_MAX ? INT_MAX
    : (Standard_Integer)Value;
}

// =======================================================================
// function : RealToShortReal
// purpose  : Converts Standard_Real value to the nearest valid
//            Standard_ShortReal. If input value is out of valid range
//            for Standard_ShortReal, minimal or maximal
//            Standard_ShortReal is returned.
// =======================================================================
inline Standard_ShortReal RealToShortReal (const Standard_Real theVal)
{
  return theVal < -FLT_MAX ? -FLT_MAX
    : theVal > FLT_MAX ? FLT_MAX
    : (Standard_ShortReal)theVal;
}

//-------------------------------------------------------------------
// Round : Returns the nearest integer of a real
//-------------------------------------------------------------------
inline Standard_Real     Round (const Standard_Real Value) 
{ return IntegerPart(Value + (Value > 0 ? 0.5 : -0.5)); }

//-------------------------------------------------------------------
// Sin : Returns the sine of a real
//-------------------------------------------------------------------
inline Standard_Real     Sin (const Standard_Real Value) 
{ return sin(Value); }


//-------------------------------------------------------------------
// ASinh : Returns the hyperbolic arc sine of a real
//-------------------------------------------------------------------
inline Standard_Real     ASinh(const Standard_Real Value)
#if __QNX__
{ return std::asinh(Value); }
#else
{ return asinh(Value); }
#endif

//-------------------------------------------------------------------
// Square : Returns a real to the power 2
//-------------------------------------------------------------------
inline Standard_Real     Square(const Standard_Real Value) 
{ return Value * Value; }

//-------------------------------------------------------------------
// Tan : Returns the tangent of a real
//-------------------------------------------------------------------
inline Standard_Real     Tan (const Standard_Real Value) 
{ return tan(Value); }

//-------------------------------------------------------------------
// Tanh : Returns the hyperbolic tangent of a real
//-------------------------------------------------------------------
inline Standard_Real     Tanh (const Standard_Real Value) 
{ return tanh(Value); }

#endif
