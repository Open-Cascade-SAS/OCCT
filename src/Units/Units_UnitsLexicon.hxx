// Created on: 1993-10-08
// Created by: Gilles DEBARBOUILLE
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

#ifndef _Units_UnitsLexicon_HeaderFile
#define _Units_UnitsLexicon_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Time.hxx>
#include <Units_Lexicon.hxx>
#include <Standard_CString.hxx>
#include <Standard_Boolean.hxx>
class TCollection_HAsciiString;
class TCollection_AsciiString;


class Units_UnitsLexicon;
DEFINE_STANDARD_HANDLE(Units_UnitsLexicon, Units_Lexicon)

//! This class defines a lexicon useful to analyse and
//! recognize the different key  words  included  in a
//! sentence. The  lexicon is stored  in a sequence of
//! tokens.
class Units_UnitsLexicon : public Units_Lexicon
{

public:

  //! Returns an empty instance of UnitsLexicon
  Standard_EXPORT Units_UnitsLexicon();
  
  //! Reads  the files  <afilename1>  and  <afilename2>   to
  //! create     a   sequence     of    tokens   stored   in
  //! <thesequenceoftokens>.
  Standard_EXPORT void Creates (const Standard_Boolean amode = Standard_True);

  //! Useful for debugging.
    virtual void Dump() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(Units_UnitsLexicon,Units_Lexicon)

protected:



private:


};


#include <Units_UnitsLexicon.lxx>





#endif // _Units_UnitsLexicon_HeaderFile
