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

//============================================================================
//==== Titre: ImageUtility_X11GC.hxx
//==== Role : The header file of primitve type "X11GC" from package
//====  "ImageUtility"
//====
//==== Implementation:  This is a primitive type implemented with typedef
//====        typedef GC ImageUtility_X11GC;
//============================================================================

#ifndef _ImageUtility_X11GC_HeaderFile
#define _ImageUtility_X11GC_HeaderFile

/*
 * Data structure used by color operations from <X11/Xlib.h>
 */
//==== Definition de Type ====================================================
#include <Standard_Type.hxx>
const Handle(Standard_Type)& STANDARD_TYPE(ImageUtility_X11GC);
//============================================================================

#include <X11/Xlib.h>

typedef GC ImageUtility_X11GC ;

#endif
