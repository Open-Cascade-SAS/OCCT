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

#include <PPoly_Polygon3D.ixx>

//=======================================================================
//function :
//purpose  : 
//=======================================================================


PPoly_Polygon3D::PPoly_Polygon3D(const Handle(PColgp_HArray1OfPnt)& Nodes,
				 const Standard_Real Defl) :
				 myDeflection(Defl),
				 myNodes(Nodes)
{
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================


PPoly_Polygon3D::PPoly_Polygon3D(const Handle(PColgp_HArray1OfPnt)& Nodes,
				 const Handle(PColStd_HArray1OfReal)& Param,
				 const Standard_Real Defl) :
				 myDeflection(Defl),
				 myNodes(Nodes),
				 myParameters(Param)
{
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================


Standard_Real PPoly_Polygon3D::Deflection() const 
{
  return myDeflection;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================


void PPoly_Polygon3D::Deflection(const Standard_Real Defl)
{
  myDeflection = Defl;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================


Standard_Integer PPoly_Polygon3D::NbNodes() const 
{
  return myNodes->Length();
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================


Handle(PColgp_HArray1OfPnt) PPoly_Polygon3D::Nodes() const 
{
  return myNodes;
}


//=======================================================================
//function :
//purpose  : 
//=======================================================================

void PPoly_Polygon3D::Nodes(const Handle(PColgp_HArray1OfPnt)& Nodes)
{
  myNodes = Nodes;
}

//=======================================================================
//function :
//purpose  : 
//=======================================================================

Standard_Boolean PPoly_Polygon3D::HasParameters() const 
{
  return (!myParameters.IsNull());
}


//=======================================================================
//function :
//purpose  : 
//=======================================================================

void PPoly_Polygon3D::Parameters(const Handle(PColStd_HArray1OfReal)& Param)
{
  myParameters = Param;
}


//=======================================================================
//function :
//purpose  : 
//=======================================================================

Handle(PColStd_HArray1OfReal) PPoly_Polygon3D::Parameters() const 
{
  return myParameters;
}

