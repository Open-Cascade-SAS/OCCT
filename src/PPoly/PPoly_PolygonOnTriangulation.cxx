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



#include <PPoly_PolygonOnTriangulation.ixx>

//=======================================================================
//function : PPoly_PolygonOnTriangulation
//purpose  : 
//=======================================================================

PPoly_PolygonOnTriangulation::PPoly_PolygonOnTriangulation()
{
}

//=======================================================================
//function : PPoly_PolygonOnTriangulation
//purpose  : 
//=======================================================================

PPoly_PolygonOnTriangulation::PPoly_PolygonOnTriangulation
(const Handle(PColStd_HArray1OfInteger)& Nodes,
 const Standard_Real                     Defl) :
    myDeflection(Defl),
    myNodes(Nodes)
{
}

//=======================================================================
//function : PPoly_PolygonOnTriangulation
//purpose  : 
//=======================================================================

PPoly_PolygonOnTriangulation::PPoly_PolygonOnTriangulation
(const Handle(PColStd_HArray1OfInteger)& Nodes,
 const Standard_Real                     Defl,
 const Handle(PColStd_HArray1OfReal)&    Param) :
    myDeflection(Defl),
    myNodes(Nodes),
    myParameters(Param)
{
}

//=======================================================================
//function : Deflection
//purpose  : 
//=======================================================================

Standard_Real PPoly_PolygonOnTriangulation::Deflection() const 
{
  return myDeflection;
}

//=======================================================================
//function : Deflection
//purpose  : 
//=======================================================================

void PPoly_PolygonOnTriangulation::Deflection(const Standard_Real D)
{
  myDeflection  = D;
}

//=======================================================================
//function : NbNodes
//purpose  : 
//=======================================================================

Standard_Integer PPoly_PolygonOnTriangulation::NbNodes() const 
{
  return myNodes->Length();
}

//=======================================================================
//function : Nodes
//purpose  : 
//=======================================================================

Handle(PColStd_HArray1OfInteger) PPoly_PolygonOnTriangulation::Nodes() const 
{
  return myNodes;
}

//=======================================================================
//function : Nodes
//purpose  : 
//=======================================================================

void PPoly_PolygonOnTriangulation::Nodes(const Handle(PColStd_HArray1OfInteger)& Nodes)
{
  myNodes = Nodes;
}


//=======================================================================
//function : HasParameters
//purpose  : 
//=======================================================================

Standard_Boolean PPoly_PolygonOnTriangulation::HasParameters() const 
{
  return (!myParameters.IsNull());
}

//=======================================================================
//function : Parameters
//purpose  : 
//=======================================================================

Handle(PColStd_HArray1OfReal) PPoly_PolygonOnTriangulation::Parameters() const 
{
  return myParameters;
}

//=======================================================================
//function : Parameters
//purpose  : 
//=======================================================================

void PPoly_PolygonOnTriangulation::Parameters(const Handle(PColStd_HArray1OfReal)& Param) 
{
  myParameters = Param;
}

