// Created on: 1994-10-18
// Created by: Laurent BOURESCHE
// Copyright (c) 1994-1999 Matra Datavision
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
// Functions to change the type of approx.
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
