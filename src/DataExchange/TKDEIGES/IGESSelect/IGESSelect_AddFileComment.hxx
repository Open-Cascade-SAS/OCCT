// Created on: 1994-08-26
// Created by: Christian CAILLET
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef _IGESSelect_AddFileComment_HeaderFile
#define _IGESSelect_AddFileComment_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TCollection_HAsciiString.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <IGESSelect_FileModifier.hxx>
#include <Standard_Integer.hxx>
class IFSelect_ContextWrite;
class IGESData_IGESWriter;
class TCollection_AsciiString;

//! This class allows to add comment lines on writing an IGES File
//! These lines are added to Start Section, instead of the only
//! one blank line written by default.
class IGESSelect_AddFileComment : public IGESSelect_FileModifier
{

public:
  //! Creates a new empty AddFileComment. Use AddLine to complete it
  Standard_EXPORT IGESSelect_AddFileComment();

  //! Clears the list of file comment lines already stored
  Standard_EXPORT void Clear();

  //! Adds a line for file comment
  //! Remark: Lines are limited to 72 useful chars. A line of more than
  //! 72 chars will be split into several ones of 72 max each.
  Standard_EXPORT void AddLine(const char* const line);

  //! Adds a list of lines for file comment
  //! Each of them must comply with demand of AddLine
  Standard_EXPORT void AddLines(const occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>>& lines);

  //! Returns the count of stored lines
  Standard_EXPORT int NbLines() const;

  //! Returns a stored line given its rank
  Standard_EXPORT const char* Line(const int num) const;

  //! Returns the complete list of lines in once
  Standard_EXPORT occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> Lines() const;

  //! Sends the comment lines to the file (Start Section)
  Standard_EXPORT void Perform(IFSelect_ContextWrite& ctx,
                               IGESData_IGESWriter&   writer) const override;

  //! Returns specific Label, which is
  //! "Add <nn> Comment Lines (Start Section)"
  Standard_EXPORT TCollection_AsciiString Label() const override;

  DEFINE_STANDARD_RTTIEXT(IGESSelect_AddFileComment, IGESSelect_FileModifier)

private:
  occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> thelist;
};

#endif // _IGESSelect_AddFileComment_HeaderFile
