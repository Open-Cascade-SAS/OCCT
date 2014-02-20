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

void Compute(const Standard_Real P1,const Standard_Real P2,
const Standard_Real Ra,const Standard_Real Rb,const gp_XY& Xd,const gp_XY& Yd,
const gp_XY& O,Bnd_Box2d& B) ;

void Compute(const Standard_Real P1,const Standard_Real P2,
const Standard_Real Ra,const Standard_Real Rb,const gp_XYZ& Xd,const gp_XYZ& Yd,
const gp_XYZ& O,Bnd_Box& B) ;
