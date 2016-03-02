// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <ShapePersistent_Geom.hxx>


//=======================================================================
//function : Read
//purpose  : Read persistent data from a file
//=======================================================================
void ShapePersistent_Geom::Geometry::Read (StdObjMgt_ReadData&) {}

//=======================================================================
//function : Read
//purpose  : Read persistent data from a file
//=======================================================================
template<>
void ShapePersistent_Geom::instance<ShapePersistent_Geom::AxisPlacement,
                                    Geom_Axis2Placement>
  ::Read (StdObjMgt_ReadData& theReadData)
{
  StdObject_gp::Object<gp_Ax1> anAxis;
  StdObject_gp::Object<gp_Dir> anXDirection;

  theReadData >> anAxis >> anXDirection;

  myTransient = new Geom_Axis2Placement (anAxis.Location(),
                                         anAxis.Direction(),
                                         anXDirection);
}
