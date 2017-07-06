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


#include <Message_Msg.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeExtend_BasicMsgRegistrator.hxx>
#include <ShapeFix_Root.hxx>
#include <Standard_Type.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeFix_Root,Standard_Transient)

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
  if ( !myMsgReg.IsNull() )
    myMsgReg->Send (shape, message, gravity);
}
