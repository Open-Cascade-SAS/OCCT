// File:      Message_Printer.cxx
// Created:   Sat Jan  6 19:26:55 2001
// Author:    OCC Team
// Copyright: Open CASCADE S.A. 2005

#include <Message_Printer.ixx>

#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

//=======================================================================
//function : Send
//purpose  : 
//=======================================================================

void Message_Printer::Send (const Standard_CString theString,
			    const Message_Gravity theGravity,
			    const Standard_Boolean putEndl) const
{
  Send ( TCollection_ExtendedString(theString), theGravity, putEndl );
}

//=======================================================================
//function : Send
//purpose  : 
//=======================================================================

void Message_Printer::Send (const TCollection_AsciiString& theString,
			    const Message_Gravity theGravity,
			    const Standard_Boolean putEndl) const
{
  Send ( TCollection_ExtendedString(theString), theGravity, putEndl );
}
