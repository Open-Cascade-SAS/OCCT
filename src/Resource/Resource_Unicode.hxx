// Created on: 1996-09-26
// Created by: Arnaud BOUZY
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _Resource_Unicode_HeaderFile
#define _Resource_Unicode_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_CString.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_PCharacter.hxx>
#include <Standard_Integer.hxx>
#include <Resource_FormatType.hxx>
class TCollection_ExtendedString;


//! This class provides functions used to convert a non-ASCII C string
//! given in ANSI, EUC, GB or SJIS format, to a
//! Unicode string of extended characters, and vice versa.
class Resource_Unicode 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Converts non-ASCII CString <fromstr> in SJIS format
  //! to Unicode ExtendedString <tostr>.
  Standard_EXPORT static void ConvertSJISToUnicode (const Standard_CString fromstr, TCollection_ExtendedString& tostr);
  
  //! Converts non-ASCII CString <fromstr> in EUC format
  //! to Unicode ExtendedString <tostr>.
  Standard_EXPORT static void ConvertEUCToUnicode (const Standard_CString fromstr, TCollection_ExtendedString& tostr);
  
  //! Converts non-ASCII CString <fromstr> in GB format
  //! to Unicode ExtendedString <tostr>.
  Standard_EXPORT static void ConvertGBToUnicode (const Standard_CString fromstr, TCollection_ExtendedString& tostr);
  
  //! Converts non-ASCII CString <fromstr> in GBK format
  //! to Unicode ExtendedString <tostr>.
  Standard_EXPORT static Standard_Boolean ConvertGBKToUnicode (const Standard_CString fromstr, TCollection_ExtendedString& tostr);
  
  //! Converts non-ASCII CString <fromstr> in Big5 format
  //! to Unicode ExtendedString <tostr>.
  Standard_EXPORT static Standard_Boolean ConvertBig5ToUnicode (const Standard_CString fromstr, TCollection_ExtendedString& tostr);
  
  //! Converts Unicode ExtendedString <fromstr> to non-ASCII
  //! CString <tostr> in SJIS format, limited to <maxsize>
  //! characters. To translate the whole <fromstr>, use more
  //! than twice the length of <fromstr>. Returns true if
  //! <maxsize> has not been reached before end of conversion.
  Standard_EXPORT static Standard_Boolean ConvertUnicodeToSJIS (const TCollection_ExtendedString& fromstr, Standard_PCharacter& tostr, const Standard_Integer maxsize);
  
  //! Converts Unicode ExtendedString <fromstr> to non-ASCII
  //! CString <tostr> in EUC format, limited to <maxsize>
  //! characters. To translate the whole <fromstr>, use more
  //! than twice the length of <fromstr>. Returns true if
  //! <maxsize> has not been reached before end of conversion.
  Standard_EXPORT static Standard_Boolean ConvertUnicodeToEUC (const TCollection_ExtendedString& fromstr, Standard_PCharacter& tostr, const Standard_Integer maxsize);
  
  //! Converts Unicode ExtendedString <fromstr> to non-ASCII
  //! CString <tostr> in GB format, limited to <maxsize>
  //! characters. To translate the whole <fromstr>, use more
  //! than twice the length of <fromstr>. Returns true if
  //! <maxsize> has not been reached before end of conversion.
  Standard_EXPORT static Standard_Boolean ConvertUnicodeToGB (const TCollection_ExtendedString& fromstr, Standard_PCharacter& tostr, const Standard_Integer maxsize);
  
  //! Converts Unicode ExtendedString <fromstr> to non-ASCII
  //! CString <tostr> in ANSI format, limited to <maxsize>
  //! characters. To translate the whole <fromstr>, use more
  //! than twice the length of <fromstr>.  Returns true if
  //! <maxsize> has not been reached before end of conversion.
  Standard_EXPORT static Standard_Boolean ConvertUnicodeToANSI (const TCollection_ExtendedString& fromstr, Standard_PCharacter& tostr, const Standard_Integer maxsize);
  
  //! Defines the current conversion format as typecode.
  //! This conversion format will then be used by the
  //! functions ConvertFormatToUnicode and
  //! ConvertUnicodeToFormat to convert the strings.
  Standard_EXPORT static void SetFormat (const Resource_FormatType typecode);
  
  //! Returns the current conversion format (either
  //! ANSI, EUC, GB or SJIS).
  //! The current converting format must be defined in
  //! advance with the SetFormat function.
  Standard_EXPORT static Resource_FormatType GetFormat();
  
  //! Reads converting format from resource "FormatType"
  //! in Resource Manager "CharSet"
  Standard_EXPORT static void ReadFormat();
  
  //! Converts the non-ASCII C string (as specified by GetFormat()) to the Unicode string of extended characters.
  static void ConvertFormatToUnicode (const Standard_CString theFromStr,
                                      TCollection_ExtendedString& theToStr)
  {
    return ConvertFormatToUnicode (Resource_Unicode::GetFormat(), theFromStr, theToStr);
  }

  //! Converts the non-ASCII C string in specified format to the Unicode string of extended characters.
  //! @param theFormat  [in] source encoding
  //! @param theFromStr [in] text to convert
  //! @param theToStr  [out] destination string
  Standard_EXPORT static void ConvertFormatToUnicode (const Resource_FormatType theFormat,
                                                      const Standard_CString theFromStr,
                                                      TCollection_ExtendedString& theToStr);

  //! Converts the Unicode string of extended characters to the non-ASCII string according to specified format.
  //! You need more than twice the length of the source string to complete the conversion.
  //! The function returns true if conversion is complete, i.e. the maximum number of characters is not reached before the end of conversion.
  //! @param theFormat  [in] destination encoding
  //! @param theFromStr [in] text to convert
  //! @param theToStr  [out] destination buffer
  //! @param theMaxSize [in] destination buffer length
  Standard_EXPORT static Standard_Boolean ConvertUnicodeToFormat (const Resource_FormatType theFormat,
                                                                  const TCollection_ExtendedString& theFromStr,
                                                                  Standard_PCharacter& theToStr,
                                                                  const Standard_Integer theMaxSize);

  //! Converts the Unicode string of extended characters to the non-ASCII string according to the format returned by the function GetFormat.
  //! @param theFromStr [in] text to convert
  //! @param theToStr  [out] destination buffer
  //! @param theMaxSize [in] destination buffer length
  static Standard_Boolean ConvertUnicodeToFormat (const TCollection_ExtendedString& theFromStr,
                                                  Standard_PCharacter& theToStr,
                                                  const Standard_Integer theMaxSize)
  {
    return ConvertUnicodeToFormat (Resource_Unicode::GetFormat(), theFromStr, theToStr, theMaxSize);
  }
  
};

#endif // _Resource_Unicode_HeaderFile
