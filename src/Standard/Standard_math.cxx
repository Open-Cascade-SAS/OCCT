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

#include <Standard_math.hxx>

// MSVC versions prior to 12 did not provided acosh, asinh, atanh functions in standard library
#if defined(_MSC_VER) && (_MSC_VER < 1800)

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
