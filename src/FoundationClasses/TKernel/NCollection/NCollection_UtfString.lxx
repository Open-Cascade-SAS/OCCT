// Created on: 2013-01-28
// Created by: Kirill GAVRILOV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

//=================================================================================================

template <typename Type>
char32_t NCollection_UtfString<Type>::GetChar(const int theCharIndex) const
{
  // Standard_ASSERT_SKIP (theCharIndex < myLength, "Out of range");
  NCollection_UtfIterator<Type> anIter(myString);
  for (; *anIter != 0; ++anIter)
  {
    if (anIter.Index() == theCharIndex)
    {
      return *anIter;
    }
  }
  return 0;
}

//=================================================================================================

template <typename Type>
const Type* NCollection_UtfString<Type>::GetCharBuffer(const int theCharIndex) const
{
  // Standard_ASSERT_SKIP(theCharIndex < myLength);
  NCollection_UtfIterator<Type> anIter(myString);
  for (; *anIter != 0; ++anIter)
  {
    if (anIter.Index() == theCharIndex)
    {
      return anIter.BufferHere();
    }
  }
  return NULL;
}

//=================================================================================================

template <typename Type>
inline void NCollection_UtfString<Type>::Clear()
{
  strFree(myString);
  mySize   = 0;
  myLength = 0;
  myString = strAlloc(mySize);
}

//=================================================================================================

template <typename Type>
inline NCollection_UtfString<Type>::NCollection_UtfString()
    : myString(strAlloc(0)),
      mySize(0),
      myLength(0)
{
}

//=================================================================================================

template <typename Type>
inline NCollection_UtfString<Type>::NCollection_UtfString(const NCollection_UtfString& theCopy)
    : myString(strAlloc(theCopy.mySize)),
      mySize(theCopy.mySize),
      myLength(theCopy.myLength)
{
  strCopy((uint8_t*)myString, (const uint8_t*)theCopy.myString, mySize);
}

//=================================================================================================

template <typename Type>
inline NCollection_UtfString<Type>::NCollection_UtfString(NCollection_UtfString&& theOther) noexcept
    : myString(theOther.myString),
      mySize(theOther.mySize),
      myLength(theOther.myLength)
{
  theOther.myString = nullptr;
  theOther.mySize   = 0;
  theOther.myLength = 0;
}

//=================================================================================================

template <typename Type>
inline NCollection_UtfString<Type>::NCollection_UtfString(const char* theCopyUtf8,
                                                          const int   theLength)
    : myString(nullptr),
      mySize(0),
      myLength(0)
{
  FromUnicode(theCopyUtf8, theLength);
}

//=================================================================================================

template <typename Type>
inline NCollection_UtfString<Type>::NCollection_UtfString(const char16_t* theCopyUtf16,
                                                          const int       theLength)
    : myString(nullptr),
      mySize(0),
      myLength(0)
{
  FromUnicode(theCopyUtf16, theLength);
}

//=================================================================================================

template <typename Type>
inline NCollection_UtfString<Type>::NCollection_UtfString(const char32_t* theCopyUtf32,
                                                          const int       theLength)
    : myString(nullptr),
      mySize(0),
      myLength(0)
{
  FromUnicode(theCopyUtf32, theLength);
}

#if !defined(_MSC_VER) || defined(_NATIVE_WCHAR_T_DEFINED)                                         \
  || (defined(_MSC_VER) && _MSC_VER >= 1900)
//=================================================================================================

template <typename Type>
inline NCollection_UtfString<Type>::NCollection_UtfString(const wchar_t* theCopyUtfWide,
                                                          const int      theLength)
    : myString(NULL),
      mySize(0),
      myLength(0)
{
  FromUnicode(theCopyUtfWide, theLength);
}
#endif

// =======================================================================
// function : ~NCollection_UtfString
// purpose  :
// =======================================================================
template <typename Type>
inline NCollection_UtfString<Type>::~NCollection_UtfString()
{
  strFree(myString);
}

//=================================================================================================

template <typename Type>
inline const NCollection_UtfString<Type>& NCollection_UtfString<Type>::Assign(
  const NCollection_UtfString<Type>& theOther)
{
  if (this == &theOther)
  {
    return (*this);
  }

  strFree(myString);
  mySize   = theOther.mySize;
  myLength = theOther.myLength;
  myString = strAlloc(mySize);
  strCopy((uint8_t*)myString, (const uint8_t*)theOther.myString, mySize);
  return (*this);
}

//=================================================================================================

template <typename Type>
inline void NCollection_UtfString<Type>::Swap(NCollection_UtfString<Type>& theOther) noexcept
{
  // Note: we could use std::swap() here, but prefer to not
  // have dependency on <algorithm> header at that level
  Type*     aString = myString;
  const int aSize   = mySize;
  const int aLength = myLength;
  myString          = theOther.myString;
  mySize            = theOther.mySize;
  myLength          = theOther.myLength;
  theOther.myString = aString;
  theOther.mySize   = aSize;
  theOther.myLength = aLength;
}

#if !defined(__ANDROID__)
//! Auxiliary conversion tool.
class NCollection_UtfStringTool
{
public:
  //! Empty constructor.
  NCollection_UtfStringTool()
      : myWideBuffer(nullptr)
  {
  }

  //! Destructor for temporary resources.
  Standard_EXPORT ~NCollection_UtfStringTool();

  //! Convert the string from current locale into UNICODE (wide characters) using system APIs.
  //! Returned pointer will be released by this tool.
  Standard_EXPORT wchar_t* FromLocale(const char* theString);

  //! Convert the UNICODE (wide characters) string into locale using system APIs.
  Standard_EXPORT static bool ToLocale(const wchar_t* theWideString,
                                       char*          theBuffer,
                                       const int      theSizeBytes);

private:
  wchar_t* myWideBuffer; //!< temporary variable
};
#endif

//=================================================================================================

template <typename Type>
inline void NCollection_UtfString<Type>::FromLocale(const char* theString, const int theLength)
{
#if defined(__ANDROID__)
  // no locales on Android
  FromUnicode(theString, theLength);
#else
  NCollection_UtfStringTool aConvertor;
  wchar_t*                  aWideBuffer = aConvertor.FromLocale(theString);
  if (aWideBuffer == nullptr)
  {
    Clear();
    return;
  }
  FromUnicode(aWideBuffer, theLength);
#endif
}

//=================================================================================================

template <typename Type>
inline bool NCollection_UtfString<Type>::ToLocale(char* theBuffer, const int theSizeBytes) const
{
#if defined(__ANDROID__)
  // no locales on Android
  NCollection_UtfString<char> anUtf8Copy(myString, myLength);
  const int                   aSize = anUtf8Copy.Size() + 1;
  if (theSizeBytes < aSize)
  {
    return false;
  }
  std::memcpy(theBuffer, anUtf8Copy.ToCString(), (size_t)aSize);
  return true;
#else
  NCollection_UtfString<wchar_t> aWideCopy(myString, myLength);
  return NCollection_UtfStringTool::ToLocale(aWideCopy.ToCString(), theBuffer, theSizeBytes);
#endif
}

// =======================================================================
// function : operator=
// purpose  :
// =======================================================================
template <typename Type>
inline const NCollection_UtfString<Type>& NCollection_UtfString<Type>::operator=(
  const char* theStringUtf8)
{
  FromUnicode(theStringUtf8);
  return (*this);
}

// =======================================================================
// function : operator=
// purpose  :
// =======================================================================
template <typename Type>
inline const NCollection_UtfString<Type>& NCollection_UtfString<Type>::operator=(
  const wchar_t* theStringUtfWide)
{
  FromUnicode(theStringUtfWide);
  return (*this);
}

//=================================================================================================

template <typename Type>
inline bool NCollection_UtfString<Type>::IsEqual(
  const NCollection_UtfString& theCompare) const noexcept
{
  return this == &theCompare
         || strAreEqual(myString, mySize, theCompare.myString, theCompare.mySize);
}

// =======================================================================
// function : operator!=
// purpose  :
// =======================================================================
template <typename Type>
inline bool NCollection_UtfString<Type>::operator!=(
  const NCollection_UtfString& theCompare) const noexcept
{
  return (!NCollection_UtfString::operator==(theCompare));
}

// =======================================================================
// function : operator+=
// purpose  :
// =======================================================================
template <typename Type>
inline NCollection_UtfString<Type>& NCollection_UtfString<Type>::operator+=(
  const NCollection_UtfString<Type>& theAppend)
{
  if (theAppend.IsEmpty())
  {
    return (*this);
  }

  // create new string
  int   aSize   = mySize + theAppend.mySize;
  Type* aString = strAlloc(aSize);
  strCopy((uint8_t*)aString, (const uint8_t*)myString, mySize);
  strCopy((uint8_t*)aString + mySize, (const uint8_t*)theAppend.myString, theAppend.mySize);

  strFree(myString);
  mySize   = aSize;
  myString = aString;
  myLength += theAppend.myLength;
  return (*this);
}

//=================================================================================================

template <typename Type>
inline NCollection_UtfString<Type> NCollection_UtfString<Type>::SubString(const int theStart,
                                                                          const int theEnd) const
{
  if (theStart >= theEnd)
  {
    return NCollection_UtfString<Type>();
  }
  for (NCollection_UtfIterator<Type> anIter(myString); *anIter != 0; ++anIter)
  {
    if (anIter.Index() >= theStart)
    {
      return NCollection_UtfString<Type>(anIter.BufferHere(), theEnd - theStart);
    }
  }
  return NCollection_UtfString<Type>();
}

//=================================================================================================

template <typename Type>
inline const NCollection_UtfString<char> NCollection_UtfString<Type>::ToUtf8() const
{
  NCollection_UtfString<char> aCopy;
  aCopy.FromUnicode(myString);
  return aCopy;
}

//=================================================================================================

template <typename Type>
inline const NCollection_UtfString<char16_t> NCollection_UtfString<Type>::ToUtf16() const
{
  NCollection_UtfString<char16_t> aCopy;
  aCopy.FromUnicode(myString);
  return aCopy;
}

//=================================================================================================

template <typename Type>
inline const NCollection_UtfString<char32_t> NCollection_UtfString<Type>::ToUtf32() const
{
  NCollection_UtfString<char32_t> aCopy;
  aCopy.FromUnicode(myString);
  return aCopy;
}

//=================================================================================================

template <typename Type>
inline const NCollection_UtfString<wchar_t> NCollection_UtfString<Type>::ToUtfWide() const
{
  NCollection_UtfString<wchar_t> aCopy;
  aCopy.FromUnicode(myString);
  return aCopy;
}
