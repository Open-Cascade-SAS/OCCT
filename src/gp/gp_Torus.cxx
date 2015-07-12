// Copyright (c) 1995-1999 Matra Datavision
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


#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Pnt.hxx>
#include <gp_Torus.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DimensionError.hxx>

void gp_Torus::Coefficients (TColStd_Array1OfReal& Coef) const
{
  // Dans le repere local du tore :
  // X*X + Y*Y + Z*Z - 2.0 * majorRadius * sqrt (X*X + Y*Y)
  // - minorRadius * minorRadius + majorRadius * majorRadius = 0.0
  // X**4 + Y **4 + 2.0 * (X*Y)**2 + 2.0 * (X*Z)**2 + 2.0 * (Y*Z)**2 -
  // 2.0 * (majorRadius + minorRadius) * (X**2 + Y**2) +
  // 2.0 * (majorRadius - minorRadius) * Z**2 - 2.0 *
  // majorRadius * minorRadius = 0.0
  Standard_Integer Low = Coef.Lower();
  Standard_DimensionError_Raise_if (Coef.Length() < 31, " ");
  gp_Trsf T;
  Standard_Real SumRadius = (majorRadius * majorRadius +
			     minorRadius * minorRadius);
  Standard_Real SubRadius = (majorRadius * majorRadius -
			     minorRadius * minorRadius);
  T.SetTransformation (pos);
  Standard_Real T11 = T.Value (1, 1);
  Standard_Real T12 = T.Value (1, 2);
  Standard_Real T13 = T.Value (1, 3);
  Standard_Real T14 = T.Value (1, 4);
  Standard_Real T21 = T.Value (2, 1);
  Standard_Real T22 = T.Value (2, 2);
  Standard_Real T23 = T.Value (2, 3);
  Standard_Real T24 = T.Value (2, 4);
  Standard_Real T31 = T.Value (3, 1);
  Standard_Real T32 = T.Value (3, 2);
  Standard_Real T33 = T.Value (3, 3);
  Standard_Real T34 = T.Value (3, 4);
  Coef(Low) = Pow (T11, 4) + Pow (T21, 4) + Pow(T31, 4) +
    2.0 * (T11 * T11 * T21 * T21 + T11 * T11 * T31 * T31 +
	   T21 * T21 * T31 * T31);
  Coef(Low+1) = Pow (T12, 4) + Pow (T22, 4) + Pow(T32, 4) +
    2.0 * (T12 * T12 * T22 * T22 + T12 * T12 * T32 * T32 +
	   T22 * T22 * T32 * T32);
  Coef(Low+2) = Pow (T13, 4) + Pow (T23, 4) + Pow(T33, 4) +
    2.0 * (T13 * T13 * T23 * T23 + T13 * T13 * T33 * T33 +
	   T23 * T23 * T33 * T33);
  Coef(Low+3) = 4.0 * (Pow (T11, 3) * T12 + Pow (T21, 3) * T22 +
		       Pow (T31, 3) * T32 + T11 * T11 * T21 * T22 +  T21 * T21 * T11 * T12 +
		       T11 * T11 * T31 * T32 + T31 * T31 * T11 * T12 + T21 * T21 * T31 * T32
		       + T31 * T31 * T21 * T22);
  Coef(Low+4) = 4.0 * (Pow (T11, 3) * T13 + Pow (T21, 3) * T23 +
		       Pow (T31, 3) * T33 + T11 * T11 * T21 * T23 + T21 * T21 * T11 * T13 +
		       T11 * T11 * T31 * T33 + T31 * T31 * T11 * T13 + T21 * T21 * T31 * T33
		       + T31 * T31 * T21 * T23);
  Coef(Low+5) = 4.0 * (Pow (T12, 3) * T11 + Pow (T22, 3) * T21 +
		       Pow (T32, 3) * T31  + T12 * T12 * T21 * T22 + T22 * T22 * T11 * T12 +
		       T12 * T12 * T31 * T32 + T32 * T32 * T11 * T12 + T22 * T22 * T31 * T32
		       + T32 * T32 * T21 * T22);
  Coef(Low+6) = 4.0 * (Pow (T12, 3) * T13 + Pow (T22, 3) * T23 +
		       Pow (T32, 3) * T33 + T12 * T12 * T22 * T23 + T22 * T22 * T12 * T13 +
		       T12 * T12 * T32 * T33 + T32 * T32 * T12 * T13 + T22 * T22 * T32 * T33
		       + T32 * T32 * T22 * T23);
  Coef(Low+7) = 4.0 * (Pow (T13, 3) * T11 + Pow (T23, 3) * T21 +
		       Pow (T33, 3) * T31 + T13 * T13 * T21 * T23 + T23 * T23 * T11 * T13 +
		       T13 * T13 * T31 * T33 + T33 * T33 * T11 * T13 + T23 * T23 * T31 * T33
		       + T33 * T33 * T21 * T23);
  Coef(Low+8) = 4.0 * (Pow (T13, 3) * T12 + Pow (T23, 3) * T22 +
		       Pow (T33, 3) * T32 + T13 * T13 * T22 * T23 + T23 * T23 * T12 * T13 +
		       T13 * T13 * T32 * T33 + T33 * T33 * T12 * T13 + T23 * T23 * T32 * T33
		       + T33 * T33 * T22 * T23);
  Coef(Low+9) = 6.0 * (T11 * T11 * T12 * T12 + T21 * T21 * T22 * T22 +
		       T31 * T31 * T32 * T32) + 8.0 * ( T11 * T12 * T21 * T22 +
						       T11 * T12 * T31 * T32 + T21 * T22 * T31 * T32) +
							 2.0 * (T11 * T11 * T22 * T22 + T11 * T11 * T32 * T32 +
								T21 * T21 * T32 * T32 + T12 * T12 * T21 * T21 +
								T12 * T12 * T31 * T31 + T22 * T22 * T31 * T31 );
  Coef(Low+10) = 6.0 * (T11 * T11 * T13 * T13 + T21 * T21 * T23 * T23 +
			T31 * T31 * T33 * T33) + 8.0 * ( T11 * T13 * T21 * T23 +
							T11 * T13 * T31 * T33 + T21 * T23 * T31 * T33) +
							  2.0 * (T11 * T11 * T23 * T23 + T11 * T11 * T33 * T33 +
								 T21 * T21 * T33 * T33 + T13 * T13 * T21 * T21 +
								 T13 * T13 * T31 * T31 + T23 * T23 * T31 * T31);
  Coef(Low+11) = 6.0 * (T12 * T12 * T13 * T13 + T22 * T22 * T23 * T23 +
			T32 * T32 * T33 * T33) + 8.0 * ( T12 * T13 * T22 * T23 +
							T12 * T23 * T32 * T33 + T22 * T23 * T32 * T33) + 
							  2.0 * (T12 * T12 * T23 * T23 + T12 * T12 * T33 * T33 +
								 T22 * T22 * T33 * T33 +  T13 * T13 * T22 * T22 +
								 T13 * T13 * T32 * T32 + T23 * T23 * T32 * T32);
  Coef(Low+12) = 4.0 * (Pow (T11, 3) * T14 + Pow (T21, 3) * T24 +
			Pow (T31, 3) * T34 + T11 * T11 * T21 * T24 + T11 * T11 * T31 * T34 +
			T21 * T21 * T31 * T34 + T21 * T21 * T11 * T14 +
			T31 * T31 * T11 * T34 + T31 * T31 * T21 * T24);
  Coef(Low+13) = 4.0 * (Pow (T12, 3) * T14 + Pow (T22, 3) * T24 +
			Pow (T32, 3) * T34 + T12 * T12 * T22 * T24 + T12 * T12 * T32 * T34 +
			T22 * T22 * T32 * T34 + T22 * T22 * T12 * T14 +
			T32 * T32 * T12 * T34 + T32 * T32 * T22 * T24);
  Coef(Low+14) = 4.0 * (Pow (T13, 3) * T14 + Pow (T23, 3) * T24 +
			Pow (T33, 3) * T34 + T13 * T13 * T23 * T24 + T13 * T13 * T33 * T34 +
			T23 * T23 * T33 * T34 + T23 * T23 * T13 * T14 +
			T33 * T33 * T13 * T34 + T33 * T33 * T23 * T24);
  Coef(Low+15) = 4.0 * (T11 * T11 * T22 * T24 + T11 * T11 * T32 * T34 +
			T21 * T21 * T32 * T34 + T21 * T21 * T12 * T14 + T31 * T31 * T12 * T14
			+ T31 * T31 * T22 * T24);
  Coef(Low+16) = 4.0 * (T11 * T11 * T23 * T24 + T11 * T11 * T33 * T34 +
			T21 * T21 * T33 * T34 + T21 * T21 * T13 * T14 + T31 * T31 * T13 * T14
			+ T31 * T31 * T23 * T24);
  Coef(Low+17) = 4.0 * (T12 * T12 * T21 * T24 + T12 * T12 * T31 * T34 +
			T22 * T22 * T31 * T34 + T22 * T22 * T11 * T14 + T32 * T32 * T11 * T14
			+ T32 * T32 * T21 * T24);
  Coef(Low+18) = 4.0 * (T12 * T12 * T23 * T24 + T12 * T12 * T33 * T34 +
			T22 * T22 * T33 * T34 + T22 * T22 * T13 * T14 + T32 * T32 * T13 * T14
			+ T32 * T32 * T23 * T24);
  Coef(Low+19) = 4.0 * (T13 * T13 * T21 * T24 + T13 * T13 * T31 * T34 +
			T23 * T23 * T31 * T34 + T23 * T23 * T11 * T14 + T33 * T33 * T11 * T14
			+ T33 * T33 * T21 * T24);
  Coef(Low+20) = 4.0 * (T13 * T13 * T22 * T24 + T13 * T13 * T32 * T34 +
			T23 * T23 * T32 * T34 + T23 * T23 * T12 * T14 + T33 * T33 * T12 * T14
			+ T33 * T33 * T22 * T24);
  Coef(Low+21) = 6.0 * (T11 * T11 * T14 * T14 + T21 * T21 * T24 * T24 +
			T31 * T31 * T34 * T34) + 2.0 * (T11 * T11 * T24 * T24 +
							T11 * T11 * T34 * T34 + T21 * T21 * T34 * T34 + T21 * T21 * T14 * T14
							+ T31 * T31 * T14 * T14 + T31 * T31 * T24 * T24 - 
							SumRadius * (T11 * T11 + T21 * T21) + SubRadius * T31 * T31);
  Coef(Low+22) = 6.0 * (T12 * T12 * T14 * T14 + T22 * T22 * T24 * T24 +
			T32 * T32 * T34 * T34) + 2.0 * (T12 * T12 * T24 * T24 +
							T12 * T12 * T34 * T34 + T22 * T22 * T34 * T34 + T22 * T22 * T14 * T14
							+ T32 * T32 * T14 * T14 + T32 * T32 * T24 * T24 - 
							SumRadius * (T12 * T12 + T22 * T22) + SubRadius * T32 * T32);
  Coef(Low+23) = 6.0 * (T13 * T13 * T14 * T14 + T23 * T23 * T24 * T24 +
			T33 * T33 * T34 * T34) + 2.0 * (T13 * T13 * T24 * T24 +
							T13 * T13 * T34 * T34 + T23 * T23 * T34 * T34 + T23 * T23 * T14 * T14
							+ T33 * T33 * T14 * T14 + T33 * T33 * T24 * T24 - 
							SumRadius * (T13 * T13 + T23 * T23) + SubRadius * T33 * T33);
  Coef(Low+24) = 8.0 * (T11 * T14 * T22 * T24 + T11 * T14 * T32 * T34 +
			T21 * T24 * T32 * T34 + T21 * T24 * T12 * T14 + T31 * T34 * T12 * T14
			+ T31 * T34 *T22 * T24) + 4.0 * (T11 * T12 * T24 * T24  +
							 T11 * T12 * T34 * T34 + T21 * T22 * T34 * T34 + T21 * T22 * T14 * T14
							 + T31 * T32 * T14 * T14 + T31 * T32 * T24 * T24 - SumRadius * (
															T11 * T12 + T21 * T22) + SubRadius * T31 * T32);
  Coef(Low+25) = 8.0 * (T11 * T14 * T23 * T24 + T11 * T14 * T33 * T34 +
			T21 * T24 * T33 * T34 + T21 * T24 * T13 * T14 + T31 * T34 * T13 * T14
			+ T31 * T34 *T23 * T24) + 4.0 * (T11 * T13 * T24 * T24  +
							 T11 * T13 * T34 * T34 + T21 * T23 * T34 * T34 + T21 * T23 * T14 * T14
							 + T31 * T33 * T14 * T14 + T31 * T33 * T24 * T24 - SumRadius * (
															T11 * T13 + T21 * T22) + SubRadius * T31 * T33);
  Coef(Low+26) = 8.0 * (T12 * T14 * T23 * T24 + T12 * T14 * T33 * T34 +
			T22 * T24 * T33 * T34 + T22 * T24 * T13 * T14 + T32 * T34 * T13 * T14
			+ T32 * T34 *T23 * T24) + 4.0 * (T12 * T13 * T24 * T24  +
							 T12 * T13 * T34 * T34 + T22 * T23 * T34 * T34 + T22 * T23 * T14 * T14
							 + T32 * T33 * T14 * T14 + T32 * T33 * T24 * T24 - SumRadius * (
															T12 * T13 + T21 * T22) + SubRadius * T32 * T33);


  Coef(Low+27) = 4.0 * (Pow (T14, 3) * T11 + Pow (T24, 3) * T21 +
			Pow (T34, 3) * T31  +  T11 * T14 * T24 * T24 + T11 * T14 * T34 * T34
			+ T21 * T24 * T34 * T34 + T21 * T24 * T14 * T14 +
			T31 * T34 * T14 * T14 + T31 * T34 * T24 * T24 + SubRadius * T31 * T34
			- SumRadius * (T11 * T14 + T21 * T24));
  Coef(Low+28) = 4.0 * (Pow (T14, 3) * T12 + Pow (T24, 3) * T22 +
			Pow (T34, 3) * T32  +  T12 * T14 * T24 * T24 + T12 * T14 * T34 * T34
			+ T22 * T24 * T34 * T34 + T22 * T24 * T14 * T14 +
			T32 * T34 * T14 * T14 + T32 * T34 * T24 * T24 + SubRadius * T32 * T34
			- SumRadius * (T12 * T14 + T22 * T24));
  Coef(Low+29) = 4.0 * (Pow (T14, 3) * T13 + Pow (T24, 3) * T23 +
			Pow (T34, 3) * T33  +  T13 * T14 * T24 * T24 + T13 * T14 * T34 * T34
			+ T23 * T24 * T34 * T34 + T23 * T24 * T14 * T14 +
			T33 * T34 * T14 * T14 + T33 * T34 * T24 * T24 + SubRadius * T33 * T34
			- SumRadius * (T13 * T14 + T21 * T24));
  Coef(Low+30) = Pow (T14, 4) + Pow (T24, 4) + Pow (T34, 4) + 2.0 * (
								     T14 * T14 * T24 * T24 + T14 * T14 * T34 * T34 + T24 * T24 * T34 * T34
								     - SumRadius * (T14 * T14 + T24 * T24) + SubRadius * T34 * T34 -
								     majorRadius * majorRadius * minorRadius * minorRadius);
}

void gp_Torus::Mirror (const gp_Pnt& P)
{ pos.Mirror (P); }

gp_Torus gp_Torus::Mirrored (const gp_Pnt& P) const
{
  gp_Torus C = *this;
  C.pos.Mirror (P);
  return C;
}

void gp_Torus::Mirror (const gp_Ax1& A1)
{ pos.Mirror (A1); }

gp_Torus gp_Torus::Mirrored (const gp_Ax1& A1) const
{
  gp_Torus C = *this;
  C.pos.Mirror (A1);
  return C;
}

void gp_Torus::Mirror (const gp_Ax2& A2)
{ pos.Mirror (A2); }

gp_Torus gp_Torus::Mirrored (const gp_Ax2& A2) const
{
  gp_Torus C = *this;
  C.pos.Mirror (A2);
  return C;
}

