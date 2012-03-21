// Created on: 1994-08-26
// Created by: Christian CAILLET
// Copyright (c) 1994-1999 Matra Datavision
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


#include <IGESSelect_AddFileComment.ixx>
#include <TCollection_HAsciiString.hxx>
#include <stdio.h>


IGESSelect_AddFileComment::IGESSelect_AddFileComment  ()
      {  thelist = new TColStd_HSequenceOfHAsciiString();  }

    void  IGESSelect_AddFileComment::Clear ()
      {  thelist->Clear();  }


    void  IGESSelect_AddFileComment::AddLine (const Standard_CString line)
      {  thelist->Append (new TCollection_HAsciiString(line)); }

    void  IGESSelect_AddFileComment::AddLines
  (const Handle(TColStd_HSequenceOfHAsciiString)& lines)
      {  thelist->Append (lines);  }

    Standard_Integer  IGESSelect_AddFileComment::NbLines () const
      {  return thelist->Length();  }

    Standard_CString  IGESSelect_AddFileComment::Line
  (const Standard_Integer num) const
      {  return thelist->Value(num)->ToCString();  }

    Handle(TColStd_HSequenceOfHAsciiString)  IGESSelect_AddFileComment::Lines
  () const
      {  return thelist;  }

    void  IGESSelect_AddFileComment::Perform
  (IFSelect_ContextWrite& ,
   IGESData_IGESWriter& writer) const
{
  Standard_Integer i, nb = NbLines();
  for (i = 1; i <= nb; i ++) {
    writer.SendStartLine (Line(i));
  }
}

    TCollection_AsciiString  IGESSelect_AddFileComment::Label () const
{
  Standard_Integer nb = NbLines();
  char labl[80];
  sprintf (labl, "Add %d Comment Lines (Start Section)",nb);
  return TCollection_AsciiString (labl);
}
