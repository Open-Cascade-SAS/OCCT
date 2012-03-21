// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef math_ContMatrices_HeaderFile
#define math_ContMatrices_HeaderFile
#include <math_Vector.hxx>
#include <Standard_Real.hxx>

//#ifndef __AppCont_API
//#if defined(WNT) && !defined(HAVE_NO_DLL)
//#  ifdef __AppCont_DLL
//#   define __AppCont_API __declspec( dllexport )
//#  else
//#   define __AppCont_API __declspec( dllimport )
//#  endif  /* __AppCont_DLL */
//# else
//#  define __AppCont_API
//# endif  /* WNT */
//#endif  /* __AppCont_API */


// Stockage des matrices de resolution d approxiamtion continue.
// La classe est limitee a 26 pour MMatrix et InvMMatrix, (pas de contraintes)
// pour IBPMatrix (contraintes de passage aux extremites) 
// et IBTMatrix (contraintes de tangence aux extremites).


//__AppCont_API void InvMMatrix(const Standard_Integer classe, math_Matrix& M);
Standard_EXPORT void InvMMatrix(const Standard_Integer classe, math_Matrix& M);

//__AppCont_API void MMatrix(const Standard_Integer classe, math_Matrix& M);
Standard_EXPORT void MMatrix(const Standard_Integer classe, math_Matrix& M);

//__AppCont_API void IBPMatrix(const Standard_Integer classe, math_Matrix& M);
Standard_EXPORT void IBPMatrix(const Standard_Integer classe, math_Matrix& M);

//__AppCont_API void IBTMatrix(const Standard_Integer classe, math_Matrix& M);
Standard_EXPORT void IBTMatrix(const Standard_Integer classe, math_Matrix& M);

//__AppCont_API void VBernstein(const Standard_Integer classe, 
Standard_EXPORT void VBernstein(const Standard_Integer classe, 
		const Standard_Integer nbpoints,
		math_Matrix&           M);



#endif
