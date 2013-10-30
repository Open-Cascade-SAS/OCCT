// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

#ifdef _MSC_VER

#define _USE_MATH_DEFINES
#include <math.h>

// MSVC versions prior to 12 did not provided acosh, asinh, atanh functions in standard library
#if _MSC_VER < 1800
__Standard_API double __cdecl acosh ( double );
__Standard_API double __cdecl asinh ( double );
__Standard_API double __cdecl atanh ( double );
#endif

#endif  /* _MSC_VER */

