// File:	Message.cxx
// Created:	Tue Nov 23 17:45:42 1999
// Author:	data exchange team
//		<det@kinox>


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
    sprintf (t, "%02dh:%02dm:%.2fs", hour, minute, second);
  else if (minute > 0)
    sprintf (t, "%02dm:%.2fs", minute, second);
  else
    sprintf (t, "%.2fs", second);
  return TCollection_AsciiString (t);
}
