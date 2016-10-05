// Created on: 1997-04-17
// Created by: Christophe MARION
// Copyright (c) 1997-1999 Matra Datavision
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


#include <BRep_Tool.hxx>
#include <HLRAlgo_EdgeStatus.hxx>
#include <HLRBRep_Curve.hxx>
#include <HLRBRep_EdgeData.hxx>
#include <TopoDS_Edge.hxx>

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void HLRBRep_EdgeData::Set (const Standard_Boolean Rg1L,
			    const Standard_Boolean RgNL,
			    const TopoDS_Edge& EG,
			    const Standard_Integer V1,
			    const Standard_Integer V2,
			    const Standard_Boolean Out1,
			    const Standard_Boolean Out2,
			    const Standard_Boolean Cut1,
			    const Standard_Boolean Cut2,
			    const Standard_Real Start,
			    const Standard_ShortReal TolStart,
			    const Standard_Real End,
			    const Standard_ShortReal TolEnd)
{ 
  Rg1Line(Rg1L);
  RgNLine(RgNL);
  Used(Standard_False);
  ChangeGeometry().Curve(EG);
  myTolerance = (Standard_ShortReal)(BRep_Tool::Tolerance(EG));
  VSta(V1);
  VEnd(V2);
  OutLVSta(Out1);
  OutLVEnd(Out2);
  CutAtSta(Cut1);
  CutAtEnd(Cut2);
  Status().Initialize
    (Start,(Standard_ShortReal)(ChangeGeometry().Curve().Resolution
				((Standard_Real)TolStart)),
     End  ,(Standard_ShortReal)(ChangeGeometry().Curve().Resolution
				((Standard_Real)TolEnd  )));
}
