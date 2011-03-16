// File:	ShapeExtend_MsgRegistrator.cxx
// Created:	Mon Jan 31 13:20:23 2000
// Author:	data exchange team
//		<det@kinox>


#include <ShapeExtend_MsgRegistrator.ixx>
#include <Message_ListOfMsg.hxx>

//=======================================================================
//function : ShapeExtend_MsgRegistrator
//purpose  : 
//=======================================================================

ShapeExtend_MsgRegistrator::ShapeExtend_MsgRegistrator() : ShapeExtend_BasicMsgRegistrator()
{
}

//=======================================================================
//function : Send
//purpose  : 
//=======================================================================

void ShapeExtend_MsgRegistrator::Send(const Handle(Standard_Transient)& object,
				      const Message_Msg& message,
				      const Message_Gravity) 
{
  if (object.IsNull()) {
#ifdef DEB
    cout << "Warning: ShapeExtend_MsgRegistrator::Send: null object" << endl;
#endif
    return;
  }
  if (myMapTransient.IsBound (object)) {
    Message_ListOfMsg& list = myMapTransient.ChangeFind (object);
    list.Append (message);
  }
  else {
    Message_ListOfMsg list;
    list.Append (message);
    myMapTransient.Bind (object, list);
  }
}

//=======================================================================
//function : Send
//purpose  : 
//=======================================================================

 void ShapeExtend_MsgRegistrator::Send(const TopoDS_Shape& shape,
				       const Message_Msg& message,
				       const Message_Gravity) 
{
  if (shape.IsNull()) {
#ifdef DEB
    cout << "Warning: ShapeExtend_MsgRegistrator::Send: null shape" << endl;
#endif
    return;
  }
  if (myMapShape.IsBound (shape)) {
    Message_ListOfMsg& list = myMapShape.ChangeFind (shape);
    list.Append (message);
  }
  else {
    Message_ListOfMsg list;
    list.Append (message);
    myMapShape.Bind (shape, list);
  }
}

