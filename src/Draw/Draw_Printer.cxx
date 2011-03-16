// File:      Draw_Printer.cxx
// Created:   Tue Jul 31 19:26:55 2007
// Author:    OCC Team
// Copyright: Open CASCADE S.A. 2007

#include <Draw_Printer.ixx>

#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

//=======================================================================
//function : Draw_Printer
//purpose  : 
//=======================================================================

Draw_Printer::Draw_Printer (const Draw_Interpretor& theTcl)
: myTcl((Standard_Address)&theTcl)
{
}

//=======================================================================
//function : Send
//purpose  : 
//=======================================================================

void Draw_Printer::Send (const TCollection_ExtendedString& theString,
			 const Message_Gravity /*theGravity*/,
			 const Standard_Boolean putEndl) const
{
  if ( ! myTcl )
    return;
  (*(Draw_Interpretor*)myTcl) << theString;
  if ( putEndl )
    (*(Draw_Interpretor*)myTcl) << '\n';
}

//=======================================================================
//function : Send
//purpose  : 
//=======================================================================

void Draw_Printer::Send (const Standard_CString theString,
			 const Message_Gravity /*theGravity*/,
			 const Standard_Boolean putEndl) const
{
  if ( ! myTcl )
    return;
  (*(Draw_Interpretor*)myTcl) << theString;
  if ( putEndl )
    (*(Draw_Interpretor*)myTcl) << '\n';
}

//=======================================================================
//function : Send
//purpose  : 
//=======================================================================

void Draw_Printer::Send (const TCollection_AsciiString& theString,
			 const Message_Gravity /*theGravity*/,
			 const Standard_Boolean putEndl) const
{
  if ( ! myTcl )
    return;
  (*(Draw_Interpretor*)myTcl) << theString;
  if ( putEndl )
    (*(Draw_Interpretor*)myTcl) << '\n';
}
