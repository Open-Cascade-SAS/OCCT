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
//==== Titre: AlienImage_X11XWDFileHeader.hxx
//==== Role : The header file of primitve type "XWDFileHeader" from package //====           "X11"
//====
//==== Implementation:  This is a primitive type implemented with typedef
//====        typedef XColor AlienImage_X11XWDFileHeader;
//============================================================================

#ifndef _AlienImage_X11XWDFileHeader_HeaderFile
#define _AlienImage_X11XWDFileHeader_HeaderFile

/*
 * Data structure used by color operations from <X11/XWDFile.h>
 */
//==== Definition de Type ====================================================

#include <Standard_Type.hxx>
const Handle(Standard_Type)& STANDARD_TYPE(AlienImage_X11XWDFileHeader);
//============================================================================

#include <Aspect_XWD.hxx>

typedef XWDFileHeader AlienImage_X11XWDFileHeader ;

ostream& operator << ( ostream& s,  const AlienImage_X11XWDFileHeader& h );

Standard_Boolean   operator==(const AlienImage_X11XWDFileHeader& AnObject,
                              const AlienImage_X11XWDFileHeader& AnotherObject);
void ShallowDump (const AlienImage_X11XWDFileHeader& AnObject,Standard_OStream& S)  ;
#endif
 
