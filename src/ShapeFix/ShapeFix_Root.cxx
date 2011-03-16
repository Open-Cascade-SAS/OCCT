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
