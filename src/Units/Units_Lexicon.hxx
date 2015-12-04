// Created on: 1992-06-22
// Created by: Gilles DEBARBOUILLE
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _Units_Lexicon_HeaderFile
#define _Units_Lexicon_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Time.hxx>
#include <Units_TokensSequence.hxx>
#include <MMgt_TShared.hxx>
#include <Standard_CString.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
class TCollection_HAsciiString;
class TCollection_AsciiString;


class Units_Lexicon;
DEFINE_STANDARD_HANDLE(Units_Lexicon, MMgt_TShared)

//! This class defines a lexicon useful to analyse and
//! recognize the  different key words  included  in a
//! sentence.  The lexicon is stored  in a sequence of
//! tokens.
class Units_Lexicon : public MMgt_TShared
{

public:

  
  //! Creates an empty instance of Lexicon.
  Standard_EXPORT Units_Lexicon();
  
  //! Reads the file <afilename> to create a sequence  of tokens
  //! stored in <thesequenceoftokens>.
  Standard_EXPORT void Creates (const Standard_CString afilename);
  
  //! Returns the first item of the sequence of tokens.
    Handle(Units_TokensSequence) Sequence() const;
  
  //! Returns in a AsciiString from TCollection the name of the file.
  Standard_EXPORT TCollection_AsciiString FileName() const;
  
  //! Returns true if  the  file has not  changed  since the
  //! creation   of   the  Lexicon   object.   Returns false
  //! otherwise.
  Standard_EXPORT virtual Standard_Boolean UpToDate() const;
  
  //! Adds to the lexicon a new token with <aword>, <amean>,
  //! <avalue>  as  arguments.  If there is  already a token
  //! with   the  field  <theword>  equal    to <aword>, the
  //! existing token is updated.
  Standard_EXPORT void AddToken (const Standard_CString aword, const Standard_CString amean, const Standard_Real avalue);
  
  //! Useful for debugging.
    virtual void Dump() const;




  DEFINE_STANDARD_RTTIEXT(Units_Lexicon,MMgt_TShared)

protected:




private:


  Handle(TCollection_HAsciiString) thefilename;
  Standard_Time thetime;
  Handle(Units_TokensSequence) thesequenceoftokens;


};


#include <Units_Lexicon.lxx>





#endif // _Units_Lexicon_HeaderFile
