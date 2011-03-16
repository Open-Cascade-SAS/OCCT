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
