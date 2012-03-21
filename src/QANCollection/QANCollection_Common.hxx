// Created on: 2002-05-15
// Created by: Alexander KARTOMIN (akm)
// Copyright (c) 2002-2012 OPEN CASCADE SAS
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



#ifndef QANCollection_Common_HeaderFile
#define QANCollection_Common_HeaderFile

#include <gp_Pnt.hxx>

// ===================== Methods for accessing items/keys =====================

// To print other type of items define PrintItem for it

Standard_EXPORT void PrintItem(const gp_Pnt&       thePnt);
Standard_EXPORT void PrintItem(const Standard_Real theDbl);

// So do for the pseudo-random generation

Standard_EXPORT void Random (Standard_Real& theValue);
Standard_EXPORT void Random (Standard_Integer& theValue,
                             const Standard_Integer theMax=RAND_MAX);
Standard_EXPORT void Random (gp_Pnt& thePnt);

#endif
