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
