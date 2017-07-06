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


#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <HLRAlgo_Projector.hxx>
#include <Prs3d_Projector.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Prs3d_Projector,Standard_Transient)

Prs3d_Projector::Prs3d_Projector (const HLRAlgo_Projector& HLPr): MyProjector(HLPr)
{}



Prs3d_Projector::Prs3d_Projector (const Standard_Boolean Pers,
				const Standard_Real Focus,
			        const Standard_Real DX,
			        const Standard_Real DY,
			        const Standard_Real DZ,
			        const Standard_Real XAt,
			        const Standard_Real YAt,
			        const Standard_Real ZAt,
			        const Standard_Real XUp,
			        const Standard_Real YUp,
			        const Standard_Real ZUp) 
{
  gp_Pnt At (XAt,YAt,ZAt);
  gp_Dir Zpers (DX,DY,DZ);
  gp_Dir Ypers (XUp,YUp,ZUp);
  gp_Dir Xpers = Ypers.Crossed(Zpers);
  gp_Ax3 Axe (At, Zpers, Xpers);
  gp_Trsf T;
  T.SetTransformation(Axe);
  MyProjector = HLRAlgo_Projector(T,Pers,Focus);
}


HLRAlgo_Projector Prs3d_Projector::Projector () const 
{
  return MyProjector;
}
