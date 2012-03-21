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

#include <GProp.ixx>

#include <Standard_DimensionError.hxx>

#include <gp.hxx>
#include <gp_XYZ.hxx>


void GProp::HOperator (

		       const gp_Pnt& G, 
		       const gp_Pnt& Q, 
		       const Standard_Real Mass,
		       gp_Mat&       Operator

) {

  gp_XYZ QG = G.XYZ() - Q.XYZ();
  Standard_Real Ixx = QG.Y() * QG.Y() + QG.Z() * QG.Z();
  Standard_Real Iyy = QG.X() * QG.X() + QG.Z() * QG.Z();
  Standard_Real Izz = QG.Y() * QG.Y() + QG.X() * QG.X();
  Standard_Real Ixy =  - QG.X() * QG.Y();
  Standard_Real Iyz =  - QG.Y() * QG.Z();
  Standard_Real Ixz =  - QG.X() * QG.Z();
  Operator.SetCols (gp_XYZ (Ixx, Ixy, Ixz), gp_XYZ (Ixy, Iyy, Iyz),
                    gp_XYZ (Ixz, Iyz, Izz));
  Operator.Multiply (Mass);
}
