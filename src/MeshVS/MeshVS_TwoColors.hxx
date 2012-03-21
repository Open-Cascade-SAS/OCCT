// Created on: 2003-10-08
// Created by: Alexander SOLOVYOV
// Copyright (c) 2003-2012 OPEN CASCADE SAS
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


#ifndef MeshVS_TWOCOLORSHXX
#define MeshVS_TWOCOLORSHXX

#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif
#ifndef _Standard_Macro_HeaderFile
#include <Standard_Macro.hxx>
#endif

#include <Quantity_Color.hxx>

Standard_EXPORT typedef struct {
  unsigned int r1 : 8;
  unsigned int g1 : 8;
  unsigned int b1 : 8;
  unsigned int r2 : 8;
  unsigned int g2 : 8;
  unsigned int b2 : 8;
}   MeshVS_TwoColors;


Standard_EXPORT Standard_Integer HashCode ( const MeshVS_TwoColors& theKey,
                                            const Standard_Integer  theUpper  );

Standard_EXPORT Standard_Boolean IsEqual (const MeshVS_TwoColors& K1,
                                          const MeshVS_TwoColors& K2  );

Standard_EXPORT Standard_Boolean operator== ( const MeshVS_TwoColors& K1,
                                              const MeshVS_TwoColors& K2  );

Standard_EXPORT MeshVS_TwoColors  BindTwoColors ( const Quantity_Color&, const Quantity_Color& );
Standard_EXPORT Quantity_Color    ExtractColor  ( MeshVS_TwoColors&, const Standard_Integer );
Standard_EXPORT void              ExtractColors ( MeshVS_TwoColors&, Quantity_Color&, Quantity_Color& );

#endif
