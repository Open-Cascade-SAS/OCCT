// Created on: 1993-03-17
// Created by: Mireille MERCIEN
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

#ifndef _TCollection_HExtendedString_HeaderFile
#define _TCollection_HExtendedString_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TCollection_ExtendedString.hxx>
#include <Standard_Transient.hxx>
#include <Standard_CString.hxx>
#include <Standard_Integer.hxx>
#include <Standard_OStream.hxx>
class TCollection_HAsciiString;


class TCollection_HExtendedString;
DEFINE_STANDARD_HANDLE(TCollection_HExtendedString, Standard_Transient)

//! A variable-length sequence of "extended"
//! (UNICODE) characters (16-bit character
//! type). It provides editing operations with
//! built-in memory management to make
//! ExtendedString objects easier to use than
//! ordinary extended character arrays.
//! HExtendedString objects are handles to strings.
//! - HExtendedString strings may be shared by several objects.
//! - You may use an ExtendedString object to get the actual string.
//! Note: HExtendedString objects use an
//! ExtendedString string as a field.
class TCollection_HExtendedString : public Standard_Transient
{

public:

  
  //! Initializes a HExtendedString to an empty ExtendedString.
  Standard_EXPORT TCollection_HExtendedString();
  
  //! Initializes a HExtendedString with a CString.
  Standard_EXPORT TCollection_HExtendedString(const Standard_CString message);
  
  //! Initializes a HExtendedString with an ExtString.
  Standard_EXPORT TCollection_HExtendedString(const Standard_ExtString message);
  
  //! Initializes a HExtendedString with a single character.
  Standard_EXPORT TCollection_HExtendedString(const Standard_ExtCharacter aChar);
  
  //! Initializes a HExtendedString with <length> space allocated.
  //! and filled with <filler>. This is useful for buffers.
  Standard_EXPORT TCollection_HExtendedString(const Standard_Integer length, const Standard_ExtCharacter filler);
  
  //! Initializes a HExtendedString with a ExtendedString.
  Standard_EXPORT TCollection_HExtendedString(const TCollection_ExtendedString& aString);

  //! Initializes a HExtendedString with a ExtendedString.
  TCollection_HExtendedString(TCollection_ExtendedString&& theString) noexcept :
    myString(std::move(theString))
  {}
  
  //! Initializes a HExtendedString with an HAsciiString.
  Standard_EXPORT TCollection_HExtendedString(const Handle(TCollection_HAsciiString)& aString);
  
  //! Initializes a HExtendedString with a HExtendedString.
  Standard_EXPORT TCollection_HExtendedString(const Handle(TCollection_HExtendedString)& aString);
  
  //! Appends <other>  to me.
  Standard_EXPORT void AssignCat (const Handle(TCollection_HExtendedString)& other);
  
  //! Returns a string appending <other>  to me.
  Standard_EXPORT Handle(TCollection_HExtendedString) Cat (const Handle(TCollection_HExtendedString)& other) const;
  
  //! Substitutes all the characters equal to aChar by NewChar
  //! in the string <me>.
  Standard_EXPORT void ChangeAll (const Standard_ExtCharacter aChar, const Standard_ExtCharacter NewChar);
  
  //! Removes all characters contained in <me>.
  //! This produces an empty ExtendedString.
  Standard_EXPORT void Clear();
  
  //! Returns True if the string <me> contains zero character
  Standard_EXPORT Standard_Boolean IsEmpty() const;
  
  //! Insert a ExtCharacter at position <where>.
  //! Example:
  //! aString contains "hy not ?"
  //! aString.Insert(1,'W'); gives "Why not ?"
  //! aString contains "Wh"
  //! aString.Insert(3,'y'); gives "Why"
  //! aString contains "Way"
  //! aString.Insert(2,'h'); gives "Why"
  Standard_EXPORT void Insert (const Standard_Integer where, const Standard_ExtCharacter what);
  
  //! Insert a HExtendedString at position <where>.
  Standard_EXPORT void Insert (const Standard_Integer where, const Handle(TCollection_HExtendedString)& what);
  
  //! Returns TRUE if <me> is less than <other>.
  Standard_EXPORT Standard_Boolean IsLess (const Handle(TCollection_HExtendedString)& other) const;
  
  //! Returns TRUE if <me> is greater than <other>.
  Standard_EXPORT Standard_Boolean IsGreater (const Handle(TCollection_HExtendedString)& other) const;
  
  //! Returns True if the string contains only "Ascii Range"  characters
  Standard_EXPORT Standard_Boolean IsAscii() const;
  
  //! Returns number of characters in <me>.
  //! This is the same functionality as 'strlen' in C.
  Standard_EXPORT Standard_Integer Length() const;
  
  //! Erases <ahowmany> characters from position <where>,
  //! <where> included.
  //! Example:
  //! aString contains "Hello"
  //! aString.Erase(2,2) erases 2 characters from position 1
  //! This gives "Hlo".
  Standard_EXPORT void Remove (const Standard_Integer where, const Standard_Integer ahowmany = 1);
  
  //! Removes every <what> characters from <me>.
  Standard_EXPORT void RemoveAll (const Standard_ExtCharacter what);
  
  //! Replaces one character in the string at position <where>.
  //! If <where> is less than zero or greater than the length of <me>
  //! an exception is raised.
  //! Example:
  //! aString contains "Garbake"
  //! astring.Replace(6,'g')  gives <me> = "Garbage"
  Standard_EXPORT void SetValue (const Standard_Integer where, const Standard_ExtCharacter what);
  
  //! Replaces a part of <me> by another string.
  Standard_EXPORT void SetValue (const Standard_Integer where, const Handle(TCollection_HExtendedString)& what);
  
  //! Splits a ExtendedString into two sub-strings.
  //! Example:
  //! aString contains "abcdefg"
  //! aString.Split(3) gives <me> = "abc" and returns "defg"
  Standard_EXPORT Handle(TCollection_HExtendedString) Split (const Standard_Integer where);
  
  //! Searches a String in <me> from the beginning
  //! and returns position of first item <what> matching.
  //! It returns -1 if not found.
  Standard_EXPORT Standard_Integer Search (const Handle(TCollection_HExtendedString)& what) const;
  
  //! Searches a ExtendedString in another ExtendedString from the end
  //! and returns position of first item <what> matching.
  //! It returns -1 if not found.
  Standard_EXPORT Standard_Integer SearchFromEnd (const Handle(TCollection_HExtendedString)& what) const;
  
  //! Returns pointer to ExtString
  Standard_ExtString ToExtString() const
  { 
    return myString.ToExtString();
  }
  
  //! Extracts <whichone> token from <me>.
  //! By default, the <separators> is set to space and tabulation.
  //! By default, the token extracted is the first one (whichone = 1).
  //! <separators> contains all separators you need.
  //! If no token indexed by <whichone> is found, it returns an empty String.
  //! Example:
  //! aString contains "This is a     message"
  //! aString.Token()  returns "This"
  //! aString.Token(" ",4) returns "message"
  //! aString.Token(" ",2) returns "is"
  //! aString.Token(" ",9) returns ""
  //! Other separators than space character and tabulation are allowed
  //! aString contains "1234; test:message   , value"
  //! aString.Token("; :,",4) returns "value"
  //! aString.Token("; :,",2) returns "test"
  Standard_EXPORT Handle(TCollection_HExtendedString) Token (const Standard_ExtString separators, const Standard_Integer whichone = 1) const;
  
  //! Truncates <me> to <ahowmany> characters.
  //! Example:  me = "Hello Dolly" -> Trunc(3) -> me = "Hel"
  Standard_EXPORT void Trunc (const Standard_Integer ahowmany);
  
  //! Returns ExtCharacter at position <where> in <me>.
  //! If <where> is less than zero or greater than the length of
  //! <me>, an exception is raised.
  //! Example:
  //! aString contains "Hello"
  //! aString.Value(2) returns 'e'
  Standard_EXPORT Standard_ExtCharacter Value (const Standard_Integer where) const;
  
  //! Returns the field myString
  Standard_EXPORT const TCollection_ExtendedString& String() const;
  
  //! Displays <me> .
  Standard_EXPORT void Print (Standard_OStream& astream) const;
  
  Standard_EXPORT Standard_Boolean IsSameState (const Handle(TCollection_HExtendedString)& other) const;




  DEFINE_STANDARD_RTTIEXT(TCollection_HExtendedString,Standard_Transient)

protected:




private:

  
  //! Returns the field myString
  Standard_EXPORT TCollection_ExtendedString& ChangeString() const;

  TCollection_ExtendedString myString;


};

namespace std
{
  template <>
  struct hash<Handle(TCollection_HExtendedString)>
  {
    size_t operator()(const Handle(TCollection_HExtendedString)& theString) const
    {
      if (theString.IsNull()) return 0;
      return std::hash<TCollection_ExtendedString>{}(theString->String());
    }
  };

  template<>
  struct equal_to<Handle(TCollection_HExtendedString)>
  {
    bool operator()(const Handle(TCollection_HExtendedString)& theString1,
                    const Handle(TCollection_HExtendedString)& theString2) const
    {
      return theString1 == theString2 ||
        (!theString1.IsNull() && !theString2.IsNull() && theString1->String() == theString2->String());

    }
  };
}

#endif // _TCollection_HExtendedString_HeaderFile
