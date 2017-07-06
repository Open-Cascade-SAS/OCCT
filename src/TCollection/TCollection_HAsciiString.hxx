// Created on: 1992-12-15
// Created by: Mireille MERCIEN
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

#ifndef _TCollection_HAsciiString_HeaderFile
#define _TCollection_HAsciiString_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TCollection_AsciiString.hxx>
#include <Standard_Transient.hxx>
#include <Standard_CString.hxx>
#include <Standard_Character.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_OStream.hxx>
class Standard_NullObject;
class Standard_OutOfRange;
class Standard_NumericError;
class Standard_NegativeValue;
class TCollection_AsciiString;
class TCollection_HExtendedString;


class TCollection_HAsciiString;
DEFINE_STANDARD_HANDLE(TCollection_HAsciiString, Standard_Transient)

//! A variable-length sequence of ASCII characters
//! (normal 8-bit character type). It provides editing
//! operations with built-in memory management to
//! make HAsciiString objects easier to use than ordinary character arrays.
//! HAsciiString objects are handles to strings.
//! -   HAsciiString strings may be shared by several objects.
//! -   You may use an AsciiString object to get the actual string.
//! Note: HAsciiString objects use an AsciiString string as a field.
class TCollection_HAsciiString : public Standard_Transient
{

public:

  
  //! Initializes a HAsciiString to an empty AsciiString.
  Standard_EXPORT TCollection_HAsciiString();
  
  //! Initializes a HAsciiString with a CString.
  Standard_EXPORT TCollection_HAsciiString(const Standard_CString message);
  
  //! Initializes a HAsciiString with a single character.
  Standard_EXPORT TCollection_HAsciiString(const Standard_Character aChar);
  
  //! Initializes a HAsciiString with <length> space allocated.
  //! and filled with <filler>.This is useful for buffers.
  Standard_EXPORT TCollection_HAsciiString(const Standard_Integer length, const Standard_Character filler);
  
  //! Initializes a HAsciiString with an integer value
  Standard_EXPORT TCollection_HAsciiString(const Standard_Integer value);
  
  //! Initializes a HAsciiString with a real value
  Standard_EXPORT TCollection_HAsciiString(const Standard_Real value);
  
  //! Initializes a HAsciiString with a HAsciiString.
  Standard_EXPORT TCollection_HAsciiString(const TCollection_AsciiString& aString);
  
  //! Initializes a HAsciiString with a HAsciiString.
  Standard_EXPORT TCollection_HAsciiString(const Handle(TCollection_HAsciiString)& aString);
  
  //! Initializes a HAsciiString with a HAsciiString.
  //! If replaceNonAscii is non-null charecter, it will be used
  //! in place of any non-ascii character found in the source string.
  //! Otherwise, raises OutOfRange exception if at least one character
  //! in the source string is not in the "Ascii range".
  Standard_EXPORT TCollection_HAsciiString(const Handle(TCollection_HExtendedString)& aString, const Standard_Character replaceNonAscii);
  
  //! Appends <other>  to me.
    void AssignCat (const Standard_CString other);
  
  //! Appends <other>  to me.
  //! Example:  aString = aString + anotherString
    void AssignCat (const Handle(TCollection_HAsciiString)& other);
  
  //! Converts the first character into its corresponding
  //! upper-case character and the other characters into lowercase.
  //! Example:
  //! before
  //! me = "hellO "
  //! after
  //! me = "Hello "
  Standard_EXPORT void Capitalize();
  
  //! Creates a new string by concatenation of this
  //! ASCII string and the other ASCII string.
  //! Example:
  //! aString = aString + anotherString
  //! aString = aString + "Dummy"
  //! aString contains "I say "
  //! aString = aString + "Hello " + "Dolly"
  //! gives "I say Hello Dolly"
  //! Warning: To catenate more than one CString, you must put a String before.
  //! So the following example is WRONG !
  //! aString = "Hello " + "Dolly"  THIS IS NOT ALLOWED
  //! This rule is applicable to AssignCat (operator +=) too.
  Standard_EXPORT Handle(TCollection_HAsciiString) Cat (const Standard_CString other) const;
  
  //! Creates a new string by concatenation of this
  //! ASCII string and the other ASCII string.
  //! Example:  aString = aString + anotherString
  Standard_EXPORT Handle(TCollection_HAsciiString) Cat (const Handle(TCollection_HAsciiString)& other) const;
  
  //! Modifies this ASCII string so that its length
  //! becomes equal to Width and the new characters
  //! are equal to Filler. New characters are added
  //! both at the beginning and at the end of this string.
  //! If Width is less than the length of this ASCII string, nothing happens.
  //! Example
  //! Handle(TCollection_HAsciiString)
  //! myAlphabet
  //! = new
  //! TCollection_HAsciiString
  //! ("abcdef");
  //! myAlphabet->Center(9,' ');
  //! assert ( !strcmp(
  //! myAlphabet->ToCString(),
  //! " abcdef ") );
  Standard_EXPORT void Center (const Standard_Integer Width, const Standard_Character Filler);
  
  //! Replaces all characters equal to aChar by
  //! NewChar in this ASCII string. The substitution is
  //! case sensitive if CaseSensitive is true (default value).
  //! If you do not use the default case sensitive
  //! option, it does not matter whether aChar is upper-case or not.
  //! Example
  //! Handle(TCollection_HAsciiString)
  //! myMistake = new
  //! TCollection_HAsciiString
  //! ("Hather");
  //! myMistake->ChangeAll('H','F');
  //! assert ( !strcmp(
  //! myMistake->ToCString(),
  //! "Father") );
  Standard_EXPORT void ChangeAll (const Standard_Character aChar, const Standard_Character NewChar, const Standard_Boolean CaseSensitive = Standard_True);
  
  //! Removes all characters contained in <me>.
  //! This produces an empty HAsciiString.
  Standard_EXPORT void Clear();
  
  //! Returns the index of the first character of <me> that is
  //! present in <Set>.
  //! The search begins to the index FromIndex and ends to the
  //! the index ToIndex.
  //! Returns zero if failure.
  //! Raises an exception if FromIndex or ToIndex is out of range
  //! Example:
  //! before
  //! me = "aabAcAa", S = "Aa", FromIndex = 1, Toindex = 7
  //! after
  //! me = "aabAcAa"
  //! returns
  //! 1
  Standard_EXPORT Standard_Integer FirstLocationInSet (const Handle(TCollection_HAsciiString)& Set, const Standard_Integer FromIndex, const Standard_Integer ToIndex) const;
  
  //! Returns the index of the first character of <me>
  //! that is not present in the set <Set>.
  //! The search begins to the index FromIndex and ends to the
  //! the index ToIndex in <me>.
  //! Returns zero if failure.
  //! Raises an exception if FromIndex or ToIndex is out of range.
  //! Example:
  //! before
  //! me = "aabAcAa", S = "Aa", FromIndex = 1, Toindex = 7
  //! after
  //! me = "aabAcAa"
  //! returns
  //! 3
  Standard_EXPORT Standard_Integer FirstLocationNotInSet (const Handle(TCollection_HAsciiString)& Set, const Standard_Integer FromIndex, const Standard_Integer ToIndex) const;
  
  //! Insert a Character at position <where>.
  //! Example:
  //! aString contains "hy not ?"
  //! aString.Insert(1,'W'); gives "Why not ?"
  //! aString contains "Wh"
  //! aString.Insert(3,'y'); gives "Why"
  //! aString contains "Way"
  //! aString.Insert(2,'h'); gives "Why"
  Standard_EXPORT void Insert (const Standard_Integer where, const Standard_Character what);
  
  //! Insert a HAsciiString at position <where>.
  Standard_EXPORT void Insert (const Standard_Integer where, const Standard_CString what);
  
  //! Insert a HAsciiString at position <where>.
  Standard_EXPORT void Insert (const Standard_Integer where, const Handle(TCollection_HAsciiString)& what);
  
  //! Inserts the other ASCII string a after a specific index in the string <me>
  //! Example:
  //! before
  //! me = "cde" , Index = 0 , other = "ab"
  //! after
  //! me = "abcde" , other = "ab"
  Standard_EXPORT void InsertAfter (const Standard_Integer Index, const Handle(TCollection_HAsciiString)& other);
  
  //! Inserts the other ASCII string a before a specific index in the string <me>
  //! Raises an exception if Index is out of bounds
  //! Example:
  //! before
  //! me = "cde" , Index = 1 , other = "ab"
  //! after
  //! me = "abcde" , other = "ab"
  Standard_EXPORT void InsertBefore (const Standard_Integer Index, const Handle(TCollection_HAsciiString)& other);
  
  //! Returns True if the string <me> contains zero character
  Standard_EXPORT Standard_Boolean IsEmpty() const;
  
  //! Returns TRUE if <me> is 'ASCII' less than <other>.
  Standard_EXPORT Standard_Boolean IsLess (const Handle(TCollection_HAsciiString)& other) const;
  
  //! Returns TRUE if <me> is 'ASCII' greater than <other>.
  Standard_EXPORT Standard_Boolean IsGreater (const Handle(TCollection_HAsciiString)& other) const;
  
  //! Converts a HAsciiString containing a numeric expression to
  //! an Integer.
  //! Example: "215" returns 215.
  Standard_EXPORT Standard_Integer IntegerValue() const;
  
  //! Returns True if the string contains an integer value.
  Standard_EXPORT Standard_Boolean IsIntegerValue() const;
  
  //! Returns True if the string contains a real value.
  Standard_EXPORT Standard_Boolean IsRealValue() const;
  
  //! Returns True if the string contains only ASCII characters
  //! between ' ' and '~'.
  //! This means no control character and no extended ASCII code.
  Standard_EXPORT Standard_Boolean IsAscii() const;
  
  //! Returns True if the string S not contains same characters than
  //! the string <me>.
  Standard_EXPORT Standard_Boolean IsDifferent (const Handle(TCollection_HAsciiString)& S) const;
  
  //! Returns True if the string S contains same characters than the
  //! string <me>.
  Standard_EXPORT Standard_Boolean IsSameString (const Handle(TCollection_HAsciiString)& S) const;
  
  //! Returns True if the string S contains same characters than the
  //! string <me>.
  Standard_EXPORT Standard_Boolean IsSameString (const Handle(TCollection_HAsciiString)& S, const Standard_Boolean CaseSensitive) const;
  
  //! Removes all space characters in the begining of the string
  Standard_EXPORT void LeftAdjust();
  
  //! Left justify.
  //! Length becomes equal to Width and the new characters are
  //! equal to Filler
  //! if Width < Length nothing happens
  //! Raises an exception if Width is less than zero
  //! Example:
  //! before
  //! me = "abcdef" , Width = 9 , Filler = ' '
  //! after
  //! me = "abcdef   "
  Standard_EXPORT void LeftJustify (const Standard_Integer Width, const Standard_Character Filler);
  
  //! Returns number of characters in <me>.
  //! This is the same functionality as 'strlen' in C.
    Standard_Integer Length() const;
  
  //! returns an index in the string <me> of the first occurence
  //! of the string S in the string <me> from the starting index
  //! FromIndex to the ending index ToIndex
  //! returns zero if failure
  //! Raises an exception if FromIndex or ToIndex is out of range.
  //! Example:
  //! before
  //! me = "aabAaAa", S = "Aa", FromIndex = 1, ToIndex = 7
  //! after
  //! me = "aabAaAa"
  //! returns
  //! 4
  Standard_EXPORT Standard_Integer Location (const Handle(TCollection_HAsciiString)& other, const Standard_Integer FromIndex, const Standard_Integer ToIndex) const;
  
  //! Returns the index of the nth occurence of the character C
  //! in the string <me> from the starting index FromIndex to the
  //! ending index ToIndex.
  //! Returns zero if failure.
  //! Raises an exception if FromIndex or ToIndex is out of range
  //! Example:
  //! before
  //! me = "aabAa", N = 3, C = 'a', FromIndex = 1, ToIndex = 5
  //! after
  //! me = "aabAa"
  //! returns 5
  Standard_EXPORT Standard_Integer Location (const Standard_Integer N, const Standard_Character C, const Standard_Integer FromIndex, const Standard_Integer ToIndex) const;
  
  //! Converts <me> to its lower-case equivalent.
  Standard_EXPORT void LowerCase();
  
  //! Inserts the other string at the begining of the string <me>
  //! Example:
  //! before
  //! me = "cde" , S = "ab"
  //! after
  //! me = "abcde" , S = "ab"
  Standard_EXPORT void Prepend (const Handle(TCollection_HAsciiString)& other);
  
  //! Prints this string on the stream <astream>.
  Standard_EXPORT void Print (Standard_OStream& astream) const;
  
  //! Converts a string containing a numeric expression to a Real.
  //! Example:
  //! "215" returns 215.0.
  //! "3.14159267" returns 3.14159267.
  Standard_EXPORT Standard_Real RealValue() const;
  
  //! Remove all the occurences of the character C in the string
  //! Example:
  //! before
  //! me = "HellLLo", C = 'L' , CaseSensitive = True
  //! after
  //! me = "Hello"
  Standard_EXPORT void RemoveAll (const Standard_Character C, const Standard_Boolean CaseSensitive);
  
  //! Removes every <what> characters from <me>
  Standard_EXPORT void RemoveAll (const Standard_Character what);
  
  //! Erases <ahowmany> characters from position <where>,
  //! <where> included.
  //! Example:
  //! aString contains "Hello"
  //! aString.Erase(2,2) erases 2 characters from position 1
  //! This gives "Hlo".
  Standard_EXPORT void Remove (const Standard_Integer where, const Standard_Integer ahowmany = 1);
  
  //! Removes all space characters at the end of the string.
  Standard_EXPORT void RightAdjust();
  
  //! Right justify.
  //! Length becomes equal to Width and the new characters are
  //! equal to Filler
  //! if Width < Length nothing happens
  //! Raises an exception if Width is less than zero
  //! Example:
  //! before
  //! me = "abcdef" , Width = 9 , Filler = ' '
  //! after
  //! me = "   abcdef"
  Standard_EXPORT void RightJustify (const Standard_Integer Width, const Standard_Character Filler);
  
  //! Searches a CString in <me> from the beginning
  //! and returns position of first item <what> matching.
  //! It returns -1 if not found.
  //! Example:
  //! aString contains "Sample single test"
  //! aString.Search("le") returns 5
  Standard_EXPORT Standard_Integer Search (const Standard_CString what) const;
  
  //! Searches a String in <me> from the beginning
  //! and returns position of first item <what> matching.
  //! it returns -1 if not found.
  Standard_EXPORT Standard_Integer Search (const Handle(TCollection_HAsciiString)& what) const;
  
  //! Searches a CString in a String from the end
  //! and returns position of first item <what> matching.
  //! It returns -1 if not found.
  //! Example:
  //! aString contains "Sample single test"
  //! aString.SearchFromEnd("le") returns 12
  Standard_EXPORT Standard_Integer SearchFromEnd (const Standard_CString what) const;
  
  //! Searches a HAsciiString in another HAsciiString from the end
  //! and returns position of first item <what> matching.
  //! It returns -1 if not found.
  Standard_EXPORT Standard_Integer SearchFromEnd (const Handle(TCollection_HAsciiString)& what) const;
  
  //! Replaces one character in the string at position <where>.
  //! If <where> is less than zero or greater than the length of <me>
  //! an exception is raised.
  //! Example:
  //! aString contains "Garbake"
  //! astring.Replace(6,'g')  gives <me> = "Garbage"
  Standard_EXPORT void SetValue (const Standard_Integer where, const Standard_Character what);
  
  //! Replaces a part of <me> in the string at position <where>.
  //! If <where> is less than zero or greater than the length of <me>
  //! an exception is raised.
  //! Example:
  //! aString contains "Garbake"
  //! astring.Replace(6,'g')  gives <me> = "Garbage"
  Standard_EXPORT void SetValue (const Standard_Integer where, const Standard_CString what);
  
  //! Replaces a part of <me> by another string.
  Standard_EXPORT void SetValue (const Standard_Integer where, const Handle(TCollection_HAsciiString)& what);
  
  //! Splits a HAsciiString into two sub-strings.
  //! Example:
  //! aString contains "abcdefg"
  //! aString.Split(3) gives <me> = "abc" and returns "defg"
  Standard_EXPORT Handle(TCollection_HAsciiString) Split (const Standard_Integer where);
  
  //! Creation of a sub-string of the string <me>.
  //! The sub-string starts to the index Fromindex and ends
  //! to the index ToIndex.
  //! Raises an exception if ToIndex or FromIndex is out of
  //! bounds
  //! Example:
  //! before
  //! me = "abcdefg", ToIndex=3, FromIndex=6
  //! after
  //! me = "abcdefg"
  //! returns
  //! "cdef"
  Standard_EXPORT Handle(TCollection_HAsciiString) SubString (const Standard_Integer FromIndex, const Standard_Integer ToIndex) const;
  
  //! Returns pointer to string (char *)
  //! This is useful for some casual manipulations
  //! Because this "char *" is 'const', you can't modify its contents.
    Standard_CString ToCString() const;
  
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
  Standard_EXPORT Handle(TCollection_HAsciiString) Token (const Standard_CString separators = " \t", const Standard_Integer whichone = 1) const;
  
  //! Truncates <me> to <ahowmany> characters.
  //! Example:  me = "Hello Dolly" -> Trunc(3) -> me = "Hel"
  Standard_EXPORT void Trunc (const Standard_Integer ahowmany);
  
  //! Converts <me> to its upper-case equivalent.
  Standard_EXPORT void UpperCase();
  
  //! Length of the string ignoring all spaces (' ') and the
  //! control character at the end.
  Standard_EXPORT Standard_Integer UsefullLength() const;
  
  //! Returns character at position <where> in <me>.
  //! If <where> is less than zero or greater than the lenght of
  //! <me>, an exception is raised.
  //! Example:
  //! aString contains "Hello"
  //! aString.Value(2) returns 'e'
  Standard_EXPORT Standard_Character Value (const Standard_Integer where) const;
  
  //! Returns the field myString.
    const TCollection_AsciiString& String() const;
  
  Standard_EXPORT Standard_Boolean IsSameState (const Handle(TCollection_HAsciiString)& other) const;




  DEFINE_STANDARD_RTTIEXT(TCollection_HAsciiString,Standard_Transient)

protected:




private:


  TCollection_AsciiString myString;


};


#include <TCollection_HAsciiString.lxx>





#endif // _TCollection_HAsciiString_HeaderFile
