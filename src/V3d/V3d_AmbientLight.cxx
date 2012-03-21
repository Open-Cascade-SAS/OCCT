// Created by: GG
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

// Modified     30-03-98 : ZOV ; PRO6774 (reconstruction of the class hierarchy and suppressing useless methods)


#include <V3d.hxx>
#include <V3d_AmbientLight.ixx>
#include <Visual3d_Light.hxx>

V3d_AmbientLight::V3d_AmbientLight(const Handle(V3d_Viewer)& VM, const Quantity_NameOfColor Name):V3d_Light(VM) { 
  
  Quantity_Color C(Name) ;
  MyType = V3d_AMBIENT ;
  MyLight = new Visual3d_Light(C) ;
}

