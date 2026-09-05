/*
 Copyright (c) 1999-2014 OPEN CASCADE SAS

 This file is part of Open CASCADE Technology software library.

 This library is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License version 2.1 as published
 by the Free Software Foundation, with special exception defined in the file
 OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
 distribution for complete text of the license and disclaimer of any warranty.

 Alternatively, this file may be used under the terms of Open CASCADE
 commercial license or contractual agreement.
*/

#include "igesread.pxx"

#include <Standard.hxx>

#include <cstring>

struct oneparam
{
  oneparam* Next;
  int       Type;
  char*     Value;
};

namespace
{
constexpr size_t THE_DIRECTORY_PAGE_CAPACITY = 1000;
constexpr size_t THE_PARAMETER_PAGE_CAPACITY = 20000;
constexpr size_t THE_CHARACTER_PAGE_CAPACITY = 10000;

struct DirectoryPage
{
  int            Used;
  DirectoryPage* Next;
  dirpart        Parts[THE_DIRECTORY_PAGE_CAPACITY];
};

struct ParameterPage
{
  ParameterPage* Next;
  int            Used;
  oneparam       Parameters[THE_PARAMETER_PAGE_CAPACITY + 1];
};

struct CharacterPage
{
  CharacterPage* Next;
  int            Used;
  char           Characters[THE_CHARACTER_PAGE_CAPACITY + 1];
};

thread_local int THE_NumberOfParts      = 0;
thread_local int THE_NumberOfParameters = 0;

thread_local parlist* THE_CurrentList = nullptr;
thread_local parlist* THE_StartList   = nullptr;
thread_local parlist* THE_HeaderList  = nullptr;

thread_local dirpart*  THE_CurrentPart      = nullptr;
thread_local oneparam* THE_CurrentParameter = nullptr;

thread_local DirectoryPage* THE_FirstDirectoryPage   = nullptr;
thread_local DirectoryPage* THE_CurrentDirectoryPage = nullptr;
thread_local int            THE_CurrentPartIndex     = 0;

thread_local ParameterPage* THE_ParameterPage = nullptr;
thread_local CharacterPage* THE_CharacterPage = nullptr;
thread_local char*          THE_CurrentText   = nullptr;

//! Reserves contiguous character storage from the parser's page allocator.
//! @param[in] theLength number of characters to reserve, excluding the terminator
//! @return pointer to zero-terminated reserved storage
static char* iges_newchar(const int theLength)
{
  int aPageOffset = THE_CharacterPage->Used;
  if (aPageOffset > static_cast<int>(THE_CHARACTER_PAGE_CAPACITY) - theLength - 1)
  {
    size_t aPageSize = sizeof(CharacterPage);
    if (theLength >= static_cast<int>(THE_CHARACTER_PAGE_CAPACITY))
    {
      aPageSize += static_cast<size_t>(theLength + 1) - THE_CHARACTER_PAGE_CAPACITY;
    }

    CharacterPage* const aNewPage =
      static_cast<CharacterPage*>(Standard::AllocateOptimal(aPageSize));
    aNewPage->Next          = THE_CharacterPage;
    THE_CharacterPage       = aNewPage;
    aPageOffset             = 0;
    THE_CharacterPage->Used = 0;
  }

  THE_CurrentText            = THE_CharacterPage->Characters + aPageOffset;
  THE_CharacterPage->Used    = aPageOffset + theLength + 1;
  THE_CurrentText[theLength] = '\0';
  return THE_CurrentText;
}
} // namespace

//=================================================================================================

dirpart* iges_get_curp(void)
{
  return THE_CurrentPart;
}

//=================================================================================================

void iges_initfile()
{
  THE_CharacterPage = static_cast<CharacterPage*>(Standard::AllocateOptimal(sizeof(CharacterPage)));
  THE_CharacterPage->Used = 0;
  THE_CharacterPage->Next = nullptr;
  THE_CurrentText         = nullptr;

  THE_ParameterPage = static_cast<ParameterPage*>(Standard::AllocateOptimal(sizeof(ParameterPage)));
  THE_ParameterPage->Used = 0;
  THE_ParameterPage->Next = nullptr;

  THE_StartList          = static_cast<parlist*>(Standard::AllocateOptimal(sizeof(parlist)));
  THE_StartList->first   = nullptr;
  THE_StartList->last    = nullptr;
  THE_StartList->nbparam = 0;

  THE_HeaderList          = static_cast<parlist*>(Standard::AllocateOptimal(sizeof(parlist)));
  THE_HeaderList->first   = nullptr;
  THE_HeaderList->last    = nullptr;
  THE_HeaderList->nbparam = 0;

  THE_CurrentList        = THE_StartList;
  THE_CurrentParameter   = nullptr;
  THE_CurrentPart        = nullptr;
  THE_NumberOfParts      = 0;
  THE_NumberOfParameters = 0;

  THE_FirstDirectoryPage =
    static_cast<DirectoryPage*>(Standard::AllocateOptimal(sizeof(DirectoryPage)));
  THE_FirstDirectoryPage->Next = nullptr;
  THE_FirstDirectoryPage->Used = 0;
  THE_CurrentDirectoryPage     = THE_FirstDirectoryPage;
  THE_CurrentPartIndex         = 0;
}

//=================================================================================================

void iges_setglobal()
{
  if (THE_CurrentList == THE_HeaderList)
  {
    return;
  }
  THE_CurrentList      = THE_HeaderList;
  THE_CurrentParameter = THE_CurrentList->first;
}

//=================================================================================================

void iges_newpart(const int theSectionNumber)
{
  if (THE_CurrentDirectoryPage->Used >= static_cast<int>(THE_DIRECTORY_PAGE_CAPACITY))
  {
    DirectoryPage* const aNewPage =
      static_cast<DirectoryPage*>(Standard::AllocateOptimal(sizeof(DirectoryPage)));
    aNewPage->Next                 = nullptr;
    aNewPage->Used                 = 0;
    THE_CurrentDirectoryPage->Next = aNewPage;
    THE_CurrentDirectoryPage       = aNewPage;
  }

  THE_CurrentPartIndex = THE_CurrentDirectoryPage->Used;
  THE_CurrentPart      = &THE_CurrentDirectoryPage->Parts[THE_CurrentPartIndex];
  THE_CurrentList      = &THE_CurrentPart->list;

  THE_CurrentPart->numpart = theSectionNumber;
  THE_CurrentList->nbparam = 0;
  THE_CurrentList->first   = nullptr;
  THE_CurrentList->last    = nullptr;

  ++THE_CurrentDirectoryPage->Used;
  ++THE_NumberOfParts;
}

//=================================================================================================

void iges_curpart(const int theDirectoryNumber)
{
  if (THE_CurrentPart == nullptr)
  {
    return;
  }
  if (theDirectoryNumber == THE_CurrentPart->numpart)
  {
    return;
  }

  if (THE_CurrentPartIndex < THE_CurrentDirectoryPage->Used - 1)
  {
    ++THE_CurrentPartIndex;
  }
  else
  {
    if (THE_CurrentDirectoryPage->Next == nullptr)
    {
      THE_CurrentDirectoryPage = THE_FirstDirectoryPage;
    }
    else
    {
      THE_CurrentDirectoryPage = THE_CurrentDirectoryPage->Next;
    }
    THE_CurrentPartIndex = 0;
  }

  THE_CurrentPart = &THE_CurrentDirectoryPage->Parts[THE_CurrentPartIndex];
  THE_CurrentList = &THE_CurrentPart->list;
  if (theDirectoryNumber == THE_CurrentPart->numpart)
  {
    return;
  }

  THE_CurrentDirectoryPage = THE_FirstDirectoryPage;
  while (THE_CurrentDirectoryPage != nullptr)
  {
    const int aNumberOfParts = THE_CurrentDirectoryPage->Used;
    for (int aPartIndex = 0; aPartIndex < aNumberOfParts; ++aPartIndex)
    {
      if (THE_CurrentDirectoryPage->Parts[aPartIndex].numpart == theDirectoryNumber)
      {
        THE_CurrentPartIndex = aPartIndex;
        THE_CurrentPart      = &THE_CurrentDirectoryPage->Parts[aPartIndex];
        THE_CurrentList      = &THE_CurrentPart->list;
        return;
      }
    }
    THE_CurrentDirectoryPage = THE_CurrentDirectoryPage->Next;
  }
  THE_CurrentPart = nullptr;
}

//=================================================================================================

void iges_newparam(const int theType, const int theLength, char* const theValue)
{
  if (THE_CurrentList == nullptr)
  {
    return;
  }

  char* const aNewValue = iges_newchar(theLength);
  for (int aCharacterIndex = 0; aCharacterIndex < theLength; ++aCharacterIndex)
  {
    aNewValue[aCharacterIndex] = theValue[aCharacterIndex];
  }

  if (THE_ParameterPage->Used > static_cast<int>(THE_PARAMETER_PAGE_CAPACITY))
  {
    ParameterPage* const aNewPage =
      static_cast<ParameterPage*>(Standard::AllocateOptimal(sizeof(ParameterPage)));
    aNewPage->Next    = THE_ParameterPage;
    aNewPage->Used    = 0;
    THE_ParameterPage = aNewPage;
  }

  THE_CurrentParameter = &THE_ParameterPage->Parameters[THE_ParameterPage->Used];
  ++THE_ParameterPage->Used;
  THE_CurrentParameter->Type  = theType;
  THE_CurrentParameter->Value = aNewValue;
  THE_CurrentParameter->Next  = nullptr;

  if (THE_CurrentList->first == nullptr)
  {
    THE_CurrentList->first = THE_CurrentParameter;
  }
  else
  {
    THE_CurrentList->last->Next = THE_CurrentParameter;
  }
  THE_CurrentList->last = THE_CurrentParameter;
  ++THE_CurrentList->nbparam;
  ++THE_NumberOfParameters;
}

//=================================================================================================

void iges_addparam(const int theLength, char* const theValue)
{
  if (theLength <= 0)
  {
    return;
  }

  const char* const anOldValue  = THE_CurrentParameter->Value;
  const size_t      anOldLength = std::strlen(anOldValue);
  char* const       aNewValue   = iges_newchar(static_cast<int>(anOldLength) + theLength + 1);

  for (size_t aCharacterIndex = 0; aCharacterIndex < anOldLength; ++aCharacterIndex)
  {
    aNewValue[aCharacterIndex] = anOldValue[aCharacterIndex];
  }
  for (int aCharacterIndex = 0; aCharacterIndex < theLength; ++aCharacterIndex)
  {
    aNewValue[anOldLength + static_cast<size_t>(aCharacterIndex)] = theValue[aCharacterIndex];
  }
  aNewValue[anOldLength + static_cast<size_t>(theLength)] = '\0';
  THE_CurrentParameter->Value                             = aNewValue;
}

//=================================================================================================

void iges_stats(int* theNumberOfParts, int* theNumberOfParameters)
{
  THE_CurrentDirectoryPage = THE_FirstDirectoryPage;
  THE_CurrentPartIndex     = 0;
  THE_CurrentList          = THE_StartList;
  THE_CurrentParameter     = THE_CurrentList->first;
  *theNumberOfParts        = THE_NumberOfParts;
  *theNumberOfParameters   = THE_NumberOfParameters;
}

//=================================================================================================

int iges_lirpart(int**  theValues,
                 char** theReservedValue1,
                 char** theReservedValue2,
                 char** theName,
                 char** theNumber,
                 int*   theNumberOfParameters)
{
  if (THE_CurrentDirectoryPage == nullptr)
  {
    return 0;
  }

  THE_CurrentPart        = &THE_CurrentDirectoryPage->Parts[THE_CurrentPartIndex];
  THE_CurrentList        = &THE_CurrentPart->list;
  *theNumberOfParameters = THE_CurrentList->nbparam;
  THE_CurrentParameter   = THE_CurrentList->first;
  *theValues             = &THE_CurrentPart->typ;
  *theReservedValue1     = THE_CurrentPart->res1;
  *theReservedValue2     = THE_CurrentPart->res2;
  *theName               = THE_CurrentPart->nom;
  *theNumber             = THE_CurrentPart->num;
  return THE_CurrentPart->numpart;
}

//=================================================================================================

void iges_nextpart()
{
  ++THE_CurrentPartIndex;
  if (THE_CurrentPartIndex >= THE_CurrentDirectoryPage->Used)
  {
    THE_CurrentDirectoryPage = THE_CurrentDirectoryPage->Next;
    THE_CurrentPartIndex     = 0;
  }
}

//=================================================================================================

int iges_lirparam(int* theType, char** theValue)
{
  if (THE_CurrentParameter == nullptr)
  {
    return 0;
  }

  *theType             = THE_CurrentParameter->Type;
  *theValue            = THE_CurrentParameter->Value;
  THE_CurrentParameter = THE_CurrentParameter->Next;
  return 1;
}

//=================================================================================================

void iges_finfile(const int theMode)
{
  if (theMode == 0 || theMode == 1)
  {
    THE_CurrentDirectoryPage = THE_FirstDirectoryPage;
    while (THE_CurrentDirectoryPage != nullptr)
    {
      DirectoryPage* const aNextPage = THE_CurrentDirectoryPage->Next;
      Standard::Free(THE_CurrentDirectoryPage);
      THE_CurrentDirectoryPage = aNextPage;
    }
    THE_FirstDirectoryPage = nullptr;
    THE_CurrentPart        = nullptr;
    THE_CurrentList        = nullptr;

    while (THE_ParameterPage != nullptr)
    {
      ParameterPage* const aNextPage = THE_ParameterPage->Next;
      Standard::Free(THE_ParameterPage);
      THE_ParameterPage = aNextPage;
    }
    THE_CurrentParameter = nullptr;
  }

  if (theMode == 0 || theMode == 2)
  {
    Standard::Free(THE_StartList);
    Standard::Free(THE_HeaderList);
    THE_StartList   = nullptr;
    THE_HeaderList  = nullptr;
    THE_CurrentList = nullptr;

    while (THE_CharacterPage != nullptr)
    {
      CharacterPage* const aNextPage = THE_CharacterPage->Next;
      Standard::Free(THE_CharacterPage);
      THE_CharacterPage = aNextPage;
    }
    THE_CurrentText = nullptr;
  }
}
