// Created on: 2008-01-03
// Created by: Alexander GRIGORIEV
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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

#include <Poly_CoherentLink.hxx>
#include <Poly_CoherentTriangle.hxx>
#include <Standard_ProgramError.hxx>

//=======================================================================
//function : Poly_CoherentLink()
//purpose  : Empty Constructor
//=======================================================================

Poly_CoherentLink::Poly_CoherentLink ()
  : myAttribute (0L)
{
  myNode[0] = -1;
  myNode[1] = -1;
  myOppositeNode[0] = -1;
  myOppositeNode[1] = -1;
}

//=======================================================================
//function : Poly_CoherentLink()
//purpose  : Constructor
//=======================================================================

Poly_CoherentLink::Poly_CoherentLink (const Poly_CoherentTriangle& theTri,
                                      Standard_Integer             iSide)
  : myAttribute (0L)
{
  static const Standard_Integer ind[] = { 1, 2, 0, 1 };
  Standard_ProgramError_Raise_if(iSide < 0 || iSide > 2,
                                 "Poly_CoherentLink::Poly_CoherentLink: "
                                 "Wrong iSide parameter");
  const Standard_Integer aNodeInd[2] = {
    theTri.Node(ind[iSide+0]),
    theTri.Node(ind[iSide+1])
  };
  if (aNodeInd[0] < aNodeInd[1]) {
    myNode[0] = aNodeInd[0];
    myNode[1] = aNodeInd[1];
    myOppositeNode[0] = theTri.Node(iSide);
    myOppositeNode[1] = theTri.GetConnectedNode(iSide);
  } else {
    myNode[0] = aNodeInd[1];
    myNode[1] = aNodeInd[0];
    myOppositeNode[0] = theTri.GetConnectedNode(iSide);
    myOppositeNode[1] = theTri.Node(iSide);
  }
}
