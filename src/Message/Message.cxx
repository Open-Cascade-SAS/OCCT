// Created on: 1999-11-23
// Created by: data exchange team
// Copyright (c) 1999-1999 Matra Datavision
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



#include <Message.ixx>
#include <Message_Messenger.hxx>

#include <stdio.h>
#include <string.h>

//=======================================================================
//function : DefaultMessenger
//purpose  : 
//=======================================================================

const Handle(Message_Messenger)& Message::DefaultMessenger ()
{
  static Handle(Message_Messenger) aMessenger = new Message_Messenger;
  return aMessenger;
}

//=======================================================================
//function : FillTime
//purpose  : 
//=======================================================================

TCollection_AsciiString Message::FillTime (const Standard_Integer hour,
					       const Standard_Integer minute,
					       const Standard_Real second)
{
  char t [30];
  if (hour > 0)
    Sprintf (t, "%02dh:%02dm:%.2fs", hour, minute, second);
  else if (minute > 0)
    Sprintf (t, "%02dm:%.2fs", minute, second);
  else
    Sprintf (t, "%.2fs", second);
  return TCollection_AsciiString (t);
}
