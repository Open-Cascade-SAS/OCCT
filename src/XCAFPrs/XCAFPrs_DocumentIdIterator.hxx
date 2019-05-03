// Author: Kirill Gavrilov
// Copyright (c) 2017-2019 OPEN CASCADE SAS
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

#ifndef _XCAFPrs_DocumentIdIterator_HeaderFile
#define _XCAFPrs_DocumentIdIterator_HeaderFile

#include <XCAFPrs_Style.hxx>

#include <TDF_ChildIterator.hxx>
#include <TDF_Label.hxx>
#include <TopLoc_Location.hxx>

//! Auxiliary tool for iterating through Path identification string.
class XCAFPrs_DocumentIdIterator
{
public:
  //! Main constructor.
  XCAFPrs_DocumentIdIterator (const TCollection_AsciiString& thePath)
  : myPath (thePath), myPosition (0)
  {
    Next();
  }

  //! Return TRUE if iterator points to a value.
  bool More() const { return !mySubId.IsEmpty(); }

  //! Return current value.
  const TCollection_AsciiString& Value() const { return mySubId; }

  //! Find the next value.
  void Next();

private:

  // Disable assignment operator.
  XCAFPrs_DocumentIdIterator& operator= (const XCAFPrs_DocumentIdIterator& );

private:
  const TCollection_AsciiString& myPath;     //!< full path
  TCollection_AsciiString        mySubId;    //!< current value
  Standard_Integer               myPosition; //!< last processed new-line symbol
};

// =======================================================================
// function : Next
// purpose  :
// =======================================================================
inline void XCAFPrs_DocumentIdIterator::Next()
{
  for (Standard_Integer aCharIndex = myPosition + 1; aCharIndex <= myPath.Length(); ++aCharIndex)
  {
    if (myPath.Value (aCharIndex) == '/')
    {
      // intermediate items have trailing dot and separator before the next item
      const Standard_Integer aLen = aCharIndex - myPosition - 2;
      if (aLen < 1)
      {
        return; // assert - should never happen for valid IDs!
      }

      mySubId = myPath.SubString (myPosition + 1, aCharIndex - 2);
      myPosition = aCharIndex;
      return;
    }
  }
  if (myPosition < myPath.Length())
  {
    // last item has only trailing dot
    mySubId = myPath.SubString (myPosition + 1, myPath.Length() - 1);
    myPosition = myPath.Length();
  }
  else
  {
    mySubId.Clear();
    myPosition = myPath.Length();
  }
}

#endif // _XCAFPrs_DocumentIdIterator_HeaderFile
