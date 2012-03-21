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

#include <PPoly_Triangulation.ixx>

//=======================================================================
//function :
//purpose  : 
//=======================================================================


PPoly_Triangulation::PPoly_Triangulation
(const Standard_Real Defl,
 const Handle(PColgp_HArray1OfPnt)& Nodes,
 const Handle(PPoly_HArray1OfTriangle)& Triangles) :
 myDeflection(Defl),
 myNodes(Nodes),
 myTriangles(Triangles)
{
}


//=======================================================================
//function :
//purpose  : 
//=======================================================================

PPoly_Triangulation::PPoly_Triangulation
(const Standard_Real Defl,
 const Handle(PColgp_HArray1OfPnt)& Nodes, 
 const Handle(PColgp_HArray1OfPnt2d)& UVNodes, 
 const Handle(PPoly_HArray1OfTriangle)& Triangles) :
 myDeflection(Defl),
 myNodes(Nodes),
 myUVNodes(UVNodes),
 myTriangles(Triangles)
{
}


//=======================================================================
//function :
//purpose  : 
//=======================================================================

Standard_Real PPoly_Triangulation::Deflection() const 
{
  return myDeflection;
}


//=======================================================================
//function :
//purpose  : 
//=======================================================================

void PPoly_Triangulation::Deflection(const Standard_Real Defl)
{
  myDeflection = Defl;
}


//=======================================================================
//function :
//purpose  : 
//=======================================================================

Standard_Integer PPoly_Triangulation::NbNodes() const 
{
  return myNodes->Length();
}


//=======================================================================
//function :
//purpose  : 
//=======================================================================

Standard_Integer PPoly_Triangulation::NbTriangles() const 
{
  return myTriangles->Length();
}


//=======================================================================
//function :
//purpose  : 
//=======================================================================

Standard_Boolean PPoly_Triangulation::HasUVNodes() const 
{
  return !myUVNodes.IsNull();
}


//=======================================================================
//function :
//purpose  : 
//=======================================================================

Handle(PColgp_HArray1OfPnt) PPoly_Triangulation::Nodes() const 
{
  return myNodes;
}


//=======================================================================
//function :
//purpose  : 
//=======================================================================

Handle(PColgp_HArray1OfPnt2d) PPoly_Triangulation::UVNodes() const 
{
  return myUVNodes;
}


//=======================================================================
//function :
//purpose  : 
//=======================================================================

Handle(PPoly_HArray1OfTriangle) PPoly_Triangulation::Triangles() const 
{
  return myTriangles;
}
