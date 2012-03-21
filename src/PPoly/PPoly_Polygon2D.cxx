// Copyright (c) 1998-1999 Matra Datavision
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


#include <PPoly_Polygon2D.ixx>

PPoly_Polygon2D::PPoly_Polygon2D(const Handle(PColgp_HArray1OfPnt2d)& Nodes,
				 const Standard_Real Defl) :
				 myDeflection(Defl),
				 myNodes(Nodes)
{
}

Standard_Real PPoly_Polygon2D::Deflection() const 
{
  return myDeflection;
}

void PPoly_Polygon2D::Deflection(const Standard_Real Defl)
{
  myDeflection = Defl;
}

Standard_Integer PPoly_Polygon2D::NbNodes() const 
{
  return myNodes->Length();

}

Handle(PColgp_HArray1OfPnt2d) PPoly_Polygon2D::Nodes() const 
{
  return myNodes;
}

void PPoly_Polygon2D::Nodes(const Handle(PColgp_HArray1OfPnt2d)& Nodes)
{
  myNodes = Nodes;
}
