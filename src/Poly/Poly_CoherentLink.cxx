// Created on: 2008-01-03
// Created by: Alexander GRIGORIEV
// Copyright (c) 2008-2012 OPEN CASCADE SAS
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
