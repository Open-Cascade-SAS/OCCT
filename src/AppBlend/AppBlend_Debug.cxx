// File:	AppBlend_Debug.cxx
// Created:	Tue Oct 18 16:11:21 1994
// Author:	Laurent BOURESCHE
//		<lbo@phylox>


#include <Standard_Boolean.hxx>

#ifndef __AppBlend_API
# if defined(WNT) && !defined(HAVE_NO_DLL)
#  ifdef __AppBlend_DLL
#   define __AppBlend_API __declspec( dllexport )
#  else
#   define __AppBlend_API __declspec( dllimport )
#  endif /*__AppBlend_DLL*/
# else
#  define __AppBlend_API
# endif /*WNT*/
#endif /*__AppBlend_API*/

//****************************************************
// Fonctions permettant de changer le type des approx.
//****************************************************

static Standard_Boolean AppBlend_ContextSplineApprox = Standard_False;
__AppBlend_API extern void AppBlend_SetContextSplineApprox(const Standard_Boolean b) 
{ AppBlend_ContextSplineApprox = b; }
__AppBlend_API extern Standard_Boolean AppBlend_GetContextSplineApprox() 
{ return AppBlend_ContextSplineApprox; }

static Standard_Boolean AppBlend_ContextApproxWithNoTgt = Standard_False;
__AppBlend_API extern void AppBlend_SetContextApproxWithNoTgt(const Standard_Boolean b) 
{ AppBlend_ContextApproxWithNoTgt = b; }
__AppBlend_API extern Standard_Boolean AppBlend_GetContextApproxWithNoTgt() 
{ return AppBlend_ContextApproxWithNoTgt; }
