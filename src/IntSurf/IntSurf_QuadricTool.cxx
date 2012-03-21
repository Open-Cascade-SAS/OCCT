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

#include <IntSurf_QuadricTool.ixx>

#include <gp_Sphere.hxx>
#include <gp_Cylinder.hxx>

Standard_Real IntSurf_QuadricTool::Tolerance (const IntSurf_Quadric& Q) {
  switch (Q.TypeQuadric()) {
  case GeomAbs_Sphere:
    return 2.e-6*Q.Sphere().Radius();
  case GeomAbs_Cylinder:
    return 2.e-6*Q.Cylinder().Radius();
  default: 
    break;
  }
  return(1e-6);
}

