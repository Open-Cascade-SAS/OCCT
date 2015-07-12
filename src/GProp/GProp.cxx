// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <gp.hxx>
#include <gp_Mat.hxx>
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
#include <GProp.hxx>
#include <Standard_DimensionError.hxx>

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
