// Created on: 2001-01-06
// Created by: OCC Team
// Copyright (c) 2001-2012 OPEN CASCADE SAS
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


#include <Message_Messenger.ixx>
#include <Message_PrinterOStream.hxx>

//=======================================================================
//function : Message_Messenger
//purpose  : 
//=======================================================================

Message_Messenger::Message_Messenger ()
{
  AddPrinter ( new Message_PrinterOStream );
}

//=======================================================================
//function : Message_Messenger
//purpose  : 
//=======================================================================

Message_Messenger::Message_Messenger (const Handle(Message_Printer)& thePrinter)
{
  AddPrinter (thePrinter);
}

//=======================================================================
//function : AddPrinter
//purpose  : 
//=======================================================================

Standard_Boolean Message_Messenger::AddPrinter (const Handle(Message_Printer)& thePrinter)
{
  // check whether printer is already in the list
  for (Standard_Integer i=1; i <= myPrinters.Length(); i++) 
    if ( myPrinters(i) == thePrinter ) 
      return Standard_False;
  // add to the list
  myPrinters.Append (thePrinter);
  return Standard_True;
}

//=======================================================================
//function : RemovePrinter
//purpose  : 
//=======================================================================

Standard_Boolean Message_Messenger::RemovePrinter (const Handle(Message_Printer)& thePrinter)
{
  // find printer in the list
  for (Standard_Integer i=1; i <= myPrinters.Length(); i++) 
    if ( myPrinters(i) == thePrinter ) 
    {
      myPrinters.Remove (i);
      return Standard_True;
    }
  return Standard_False;
}

//=======================================================================
//function : RemovePrinters
//purpose  : 
//=======================================================================

Standard_Integer Message_Messenger::RemovePrinters (const Handle(Standard_Type)& theType)
{
  // remove printers from the list
  Standard_Integer nb = 0;
  for (Standard_Integer i=1; i <= myPrinters.Length(); i++) 
    if ( myPrinters(i)->IsKind(theType) ) 
    {
      myPrinters.Remove (i--);
      nb++;
    }
  return nb;
}

//=======================================================================
//function : Send
//purpose  : 
//=======================================================================

void Message_Messenger::Send (const Standard_CString theString,
			      const Message_Gravity theGravity,
			      const Standard_Boolean putEndl) const
{
  Standard_Integer nb = myPrinters.Length();
  for (Standard_Integer i = 1; i <= nb; i++)
  {
    Handle(Message_Printer) aPrinter = Handle(Message_Printer)::DownCast ( myPrinters(i) );
    if ( ! aPrinter.IsNull() ) 
      aPrinter->Send ( theString, theGravity, putEndl );
  }
}

//=======================================================================
//function : Send
//purpose  : 
//=======================================================================

void Message_Messenger::Send (const TCollection_AsciiString& theString,
				     const Message_Gravity theGravity,
				     const Standard_Boolean putEndl) const
{
  Standard_Integer nb = myPrinters.Length();
  for (Standard_Integer i = 1; i <= nb; i++)
  {
    Handle(Message_Printer) aPrinter = Handle(Message_Printer)::DownCast ( myPrinters(i) );
    if ( ! aPrinter.IsNull() ) 
      aPrinter->Send ( theString, theGravity, putEndl );
  }
}

//=======================================================================
//function : Send
//purpose  : 
//=======================================================================

void Message_Messenger::Send (const TCollection_ExtendedString& theString,
				     const Message_Gravity theGravity,
				     const Standard_Boolean putEndl) const
{
  Standard_Integer nb = myPrinters.Length();
  for (Standard_Integer i = 1; i <= nb; i++)
  {
    Handle(Message_Printer) aPrinter = Handle(Message_Printer)::DownCast ( myPrinters(i) );
    if ( ! aPrinter.IsNull() ) 
      aPrinter->Send ( theString, theGravity, putEndl );
  }
}
