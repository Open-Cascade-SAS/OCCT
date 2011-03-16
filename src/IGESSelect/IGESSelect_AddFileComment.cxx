// Copyright: 	Matra-Datavision 1994
// File:	IGESSelect_AddFileComment.cxx
// Created:	Fri Aug 26 14:10:58 1994
// Author:	Christian CAILLET
//		<cky>

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
