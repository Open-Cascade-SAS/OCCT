// Created on: 2007-07-31
// Created by: OCC Team
// Copyright (c) 2007-2012 OPEN CASCADE SAS
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
  if ( putEndl ){
    (*(Draw_Interpretor*)myTcl) << "\n";
  }
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
  if ( putEndl ){
    (*(Draw_Interpretor*)myTcl) << "\n";
  }
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
  if ( putEndl ){
    (*(Draw_Interpretor*)myTcl) << "\n";
  }
}
