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

#ifndef _Select3D_Pnt2d_HeaderFile
#define _Select3D_Pnt2d_HeaderFile

#include<gp_Pnt2d.hxx>
#include<Standard_ShortReal.hxx>
#include<Select3D_Macro.hxx>

struct Select3D_Pnt2d{

 Standard_ShortReal x, y;

 inline operator gp_Pnt2d() const
 {
   return gp_Pnt2d(x, y);
 }

 inline operator gp_XY() const
 {
  return gp_XY(x, y); 
 }

 inline gp_Pnt2d operator = (const gp_Pnt2d& thePnt)
 {
   x = DToF(thePnt.X());
   y = DToF(thePnt.Y());
   return *this;
 }


};

#endif

