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

// -------------------------------------------------------------------------
// Definition des fonctions mathematiques hyperboliques: acosh, asinh, atanh
// pour Windows/NT uniquement.
// -------------------------------------------------------------------------
#ifdef WNT
#include <math.h>
#include <Standard_math.hxx>

#ifndef _Standard_MacroHeaderFile
# include <Standard_Macro.hxx>
#endif  // _Standard_MacroHeaderFile

Standard_EXPORT double  __cdecl acosh( double X)
{
	double  res;
	res = log(X + sqrt(X * X - 1));
	return res;
};
Standard_EXPORT double  __cdecl  asinh( double X)
{
	double  res;
//  Modified by Sergey KHROMOV - Mon Nov 11 16:27:11 2002 Begin
//  Correction of the formula to avoid numerical problems.
//	res = log(X + sqrt(X * X + 1));
	if (X > 0.)
	res = log(X + sqrt(X * X + 1));
	else
	  res = -log(sqrt(X * X + 1) - X);
//  Modified by Sergey KHROMOV - Mon Nov 11 16:27:13 2002 End
	return res;
};
Standard_EXPORT double __cdecl  atanh( double X)
{
	double res;
	res = log((1 + X) / (1 - X)) / 2;
	return res;
};
#endif
