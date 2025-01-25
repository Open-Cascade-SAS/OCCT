// Created on: 1993-04-15
// Created by: Christian CAILLET
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _Interface_LineBuffer_HeaderFile
#define _Interface_LineBuffer_HeaderFile

#include <NCollection_Array1.hxx>
#include <TCollection_HAsciiString.hxx>

//! Simple Management of a Line Buffer, to be used by Interface
//! File Writers.
//! While a String is suitable to do that, this class ensures an
//! optimised Memory Management, because this is a hard point of
//! File Writing.
class Interface_LineBuffer
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a LineBuffer with an absolute maximum size
  //! (Default value is only to satisfy compiler requirement)
  Standard_EXPORT Interface_LineBuffer(const Standard_Integer size = 10);

  //! Changes Maximum allowed size of Buffer.
  //! If <max> is Zero, Maximum size is set to the initial size.
  Standard_EXPORT void SetMax(const Standard_Integer max);

  //! Sets an Initial reservation for Blank characters
  //! (this reservation is counted in the size of the current Line)
  Standard_EXPORT void SetInitial(const Standard_Integer initial);

  //! Sets a Keep Status at current Length. It means that at next
  //! Move, the new line will begin by characters between Keep + 1
  //! and current Length
  Standard_EXPORT void SetKeep();

  //! Returns True if there is room enough to add <more> characters
  //! Else, it is required to Dump the Buffer before refilling it
  //! <more> is recorded to manage SetKeep status
  Standard_EXPORT Standard_Boolean CanGet(const Standard_Integer more);

  //! Returns the Content of the LineBuffer
  Standard_CString Content() const { return &myLine.First(); }

  //! Returns the Length of the LineBuffer
  Standard_Integer Length() const { return myLen + myInit; }

  //! Clears completely the LineBuffer
  Standard_EXPORT void Clear();

  //! Inhibits effect of SetInitial until the next Move (i.e. Keep)
  //! Then Prepare will not insert initial blanks, but further ones
  //! will. This allows to cancel initial blanks on an internal Split
  //! A call to SetInitial has no effect on this until Move
  Standard_EXPORT void FreezeInitial();

  //! Fills a AsciiString <str> with the Content of the Line Buffer,
  //! then Clears the LineBuffer
  Standard_EXPORT void Move(TCollection_AsciiString& str);

  //! Same as above, but <str> is known through a Handle
  Standard_EXPORT void Move(const Handle(TCollection_HAsciiString)& str);

  //! Same as above, but generates the HAsciiString
  Standard_EXPORT Handle(TCollection_HAsciiString) Moved();

  //! Adds a text as a CString. Its Length is evaluated from the
  //! text (by C function strlen)
  Standard_EXPORT void Add(const Standard_CString text);

  //! Adds a text as a CString. Its length is given as <lntext>
  Standard_EXPORT void Add(const Standard_CString text, const Standard_Integer lntext);

  //! Adds a text as a AsciiString from TCollection
  Standard_EXPORT void Add(const TCollection_AsciiString& text);

  //! Adds a text made of only ONE Character
  Standard_EXPORT void Add(const Standard_Character text);

private:
  //! Prepares Move : Inserts Initial Blanks if required, and
  //! determines if SetKeep can be supported (it cannot be if Length
  //! + Next String to get (see CanGet) overpass Max Size)
  Standard_EXPORT void Prepare();

  //! Keeps characters from SetKeep. If SetKeep is Zero, equivalent
  //! to Clear
  Standard_EXPORT void Keep();

private:
  NCollection_Array1<Standard_Character> myLine;
  Standard_Integer                       myMax;
  Standard_Integer                       myInit;
  Standard_Integer                       myKeep;
  Standard_Integer                       myGet;
  Standard_Integer                       myLen;
  Standard_Integer                       myFriz;
  Standard_Character                     myKept;
};

#endif // _Interface_LineBuffer_HeaderFile
