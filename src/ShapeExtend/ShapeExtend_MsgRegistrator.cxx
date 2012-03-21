// Created on: 2000-01-31
// Created by: data exchange team
// Copyright (c) 2000-2012 OPEN CASCADE SAS
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

