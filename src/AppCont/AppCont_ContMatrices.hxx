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

#ifndef math_ContMatrices_HeaderFile
#define math_ContMatrices_HeaderFile
#include <math_Vector.hxx>
#include <Standard_Real.hxx>

//#ifndef __AppCont_API
//#if defined(_WIN32) && !defined(HAVE_NO_DLL)
//#  ifdef __AppCont_DLL
//#   define __AppCont_API __declspec( dllexport )
//#  else
//#   define __AppCont_API __declspec( dllimport )
//#  endif  /* __AppCont_DLL */
//# else
//#  define __AppCont_API
//# endif  /* _WIN32 */
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
