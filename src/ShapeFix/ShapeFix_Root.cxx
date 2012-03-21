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

#include <ShapeFix_Root.ixx>

//=======================================================================
//function : ShapeFix_Root
//purpose  : 
//=======================================================================

ShapeFix_Root::ShapeFix_Root()
{
  myPrecision = myMinTol = myMaxTol = Precision::Confusion();
  myMsgReg = new ShapeExtend_BasicMsgRegistrator;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void ShapeFix_Root::Set (const Handle(ShapeFix_Root)& Root) 
{
  myContext   = Root->myContext;
  myMsgReg    = Root->myMsgReg;
  myPrecision = Root->myPrecision;
  myMinTol    = Root->myMinTol;
  myMaxTol    = Root->myMaxTol;
  myShape     = Root->myShape;
}

//=======================================================================
//function : SetContext
//purpose  : 
//=======================================================================

void ShapeFix_Root::SetContext (const Handle(ShapeBuild_ReShape)& context) 
{
  myContext = context;
}

//=======================================================================
//function : SetMsgRegistrator
//purpose  : 
//=======================================================================

 void ShapeFix_Root::SetMsgRegistrator(const Handle(ShapeExtend_BasicMsgRegistrator)& msgreg) 
{
  myMsgReg = msgreg;
}

//=======================================================================
//function : SetPrecision
//purpose  : 
//=======================================================================

void ShapeFix_Root::SetPrecision (const Standard_Real preci) 
{
  myPrecision = preci;
  if(myMaxTol < myPrecision) myMaxTol = myPrecision;
  if(myMinTol > myPrecision) myMinTol = myPrecision;
}

//=======================================================================
//function : SetMinTolerance
//purpose  : 
//=======================================================================

void ShapeFix_Root::SetMinTolerance (const Standard_Real mintol) 
{
  myMinTol = mintol;
}

//=======================================================================
//function : SetMaxTolerance
//purpose  : 
//=======================================================================

void ShapeFix_Root::SetMaxTolerance (const Standard_Real maxtol) 
{
  myMaxTol = maxtol;
}

//=======================================================================
//function : SendMsg
//purpose  : 
//=======================================================================

 void ShapeFix_Root::SendMsg(const TopoDS_Shape& shape,
			     const Message_Msg& message,
			     const Message_Gravity gravity) const
{
  myMsgReg->Send (shape, message, gravity);
}
