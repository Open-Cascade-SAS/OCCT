// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

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

