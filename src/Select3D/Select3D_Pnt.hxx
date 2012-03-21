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

#ifndef _Select3D_Pnt_HeaderFile
#define _Select3D_Pnt_HeaderFile

#include<gp_Pnt.hxx>
#include<Standard_ShortReal.hxx>
#include<Select3D_Macro.hxx>

struct Select3D_Pnt{  

 Standard_ShortReal x, y, z;
 
 inline operator gp_Pnt() const
 {
   return gp_Pnt(x, y, z);
 }

 inline operator gp_XYZ() const
 {
  return gp_XYZ(x, y, z);
 }

 inline gp_Pnt operator = (const gp_Pnt& thePnt)
 {
   x = DToF(thePnt.X());
   y = DToF(thePnt.Y());
   z = DToF(thePnt.Z());
   return *this;
 }

};

#endif

