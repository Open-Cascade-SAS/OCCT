// Created on: 1994-10-03
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

#ifndef __Blend_API
# if defined(WNT) && !defined(HAVE_NO_DLL)
#  ifdef __Blend_DLL
#   define __Blend_API __declspec( dllexport )
#  else
#   define __Blend_API __declspec( dllimport )
#  endif /*__Blend_DLL*/
# else
#  define __Blend_API
# endif /*WNT*/
#endif /*__Blend_API*/

//*************************************************
// recuperation des surfaces des conges approximes.
//*************************************************

static Standard_Boolean Blend_traceDRAWSECT = Standard_False;
__Blend_API extern void Blend_SettraceDRAWSECT(const Standard_Boolean b) 
{ Blend_traceDRAWSECT = b; }
__Blend_API extern Standard_Boolean Blend_GettraceDRAWSECT() 
{ return Blend_traceDRAWSECT; }

//*************************************************
// Contexte sans test de deflexion
//*************************************************

static Standard_Boolean Blend_contextNOTESTDEFL = Standard_False;
__Blend_API extern void Blend_SetcontextNOTESTDEFL(const Standard_Boolean b) 
{ Blend_contextNOTESTDEFL = b; }
__Blend_API extern Standard_Boolean Blend_GetcontextNOTESTDEFL() 
{ return Blend_contextNOTESTDEFL; }
