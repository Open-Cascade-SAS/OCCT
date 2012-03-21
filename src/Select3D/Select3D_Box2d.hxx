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

#ifndef _Select3D_Box2d_HeaderFile
#define _Select3D_Box2d_HeaderFile

#include<Bnd_Box2d.hxx>
#include<Standard_ShortReal.hxx>
#include<Select3D_Macro.hxx>

struct Select3D_Box2d
{
 Standard_ShortReal xmin, ymin, xmax, ymax;

 Select3D_Box2d()
 { 
   SetVoid();
 }

 Select3D_Box2d(const Bnd_Box2d& theBox)
 { 
   SetField(theBox);
 }

 inline operator Bnd_Box2d() const
 {
   Bnd_Box2d aBox;
   aBox.SetVoid();
   if( !IsVoid() )
     aBox.Update(xmin, ymin, xmax, ymax);
   return aBox;
 }

 inline Select3D_Box2d operator = ( const Bnd_Box2d& theBox)
 { 
   SetField(theBox); 
   return *this;
 }

 inline void Update(const gp_Pnt2d& thePnt)
 {
  Bnd_Box2d aBox;
  aBox.Set(thePnt);
  if( !IsVoid() )
    aBox.Update(xmin, ymin, xmax, ymax);
  SetField(aBox);
 }

 inline void SetVoid()
 {
   xmin = ymin = ShortRealLast();
   xmax = ymax = ShortRealFirst();
 }

 inline Standard_Boolean IsVoid() const
 {
   return ( xmin == ShortRealLast() && ymin == ShortRealLast() && xmax == ShortRealFirst() && ymax == ShortRealFirst() );
 }

private: 
 inline void SetField(const Bnd_Box2d& theBox)
 {
  if( theBox.IsVoid() )
    SetVoid();
  else {
    Standard_Real x, y, x1, y1;
    theBox.Get(x, y, x1, y1);   

    xmin = DToF(x);
    ymin = DToF(y);
    xmax = DToF(x1);
    ymax = DToF(y1);
  }
 }

};

#endif




