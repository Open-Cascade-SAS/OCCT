// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <StdPrs_BRepFontCache.hxx>

#include <StdPrs_BRepFont.hxx>
#include <Standard_HashUtils.hxx>

#include <array>
#include <cmath>
#include <mutex>

IMPLEMENT_STANDARD_RTTIEXT(StdPrs_BRepFontCache, Standard_Transient)

//=================================================================================================

size_t StdPrs_BRepFontCache::RequestHasher::operator()(const Request& theRequest) const noexcept
{
  const std::array<size_t, 4> aHashes = {theRequest.FontName.HashCode(),
                                         opencascade::hash(theRequest.Size),
                                         opencascade::hash(theRequest.FontAspect),
                                         opencascade::hash(theRequest.StrictLevel)};
  return opencascade::hashBytes(aHashes.data(), sizeof(aHashes));
}

//=================================================================================================

bool StdPrs_BRepFontCache::RequestHasher::operator()(const Request& theLeft,
                                                     const Request& theRight) const noexcept
{
  return theLeft.FontName == theRight.FontName && theLeft.Size == theRight.Size
         && theLeft.FontAspect == theRight.FontAspect
         && theLeft.StrictLevel == theRight.StrictLevel;
}

//=================================================================================================

StdPrs_BRepFontCache::StdPrs_BRepFontCache(const size_t theCapacity)
    : myCapacity(theCapacity)
{
  if (myCapacity > 0)
  {
    myFonts.ReSize(myCapacity);
  }
}

//=================================================================================================

occ::handle<StdPrs_BRepFont> StdPrs_BRepFontCache::FindFont(
  const TCollection_AsciiString& theFontName,
  const Font_FontAspect          theFontAspect,
  const double                   theSize,
  const Font_StrictLevel         theStrictLevel)
{
  if (myCapacity == 0 || !std::isfinite(theSize) || !(theSize > 0.0))
  {
    return {};
  }
  const Request aRequest(theFontName, theFontAspect, theSize, theStrictLevel);
  for (;;)
  {
    size_t aRevision = 0;
    {
      std::shared_lock<std::shared_mutex> aLock(myMutex);
      if (const Entry* anEntry = myFonts.Seek(aRequest))
      {
        if (!anEntry->Font.IsNull()
            && anEntry->Font->hasConfigurationRevision(anEntry->FontRevision))
        {
          return anEntry->Font;
        }
      }
      aRevision = myRevision;
    }

    occ::handle<StdPrs_BRepFont> aCreated =
      StdPrs_BRepFont::FindAndCreate(theFontName, theFontAspect, theSize, theStrictLevel);
    if (aCreated.IsNull())
    {
      return {};
    }

    std::unique_lock<std::shared_mutex> aLock(myMutex);
    if (aRevision != myRevision)
    {
      continue;
    }
    const Entry aCreatedEntry{aCreated, aCreated->configurationRevision()};
    if (Entry* aPublished = myFonts.ChangeSeek(aRequest))
    {
      if (!aPublished->Font.IsNull()
          && aPublished->Font->hasConfigurationRevision(aPublished->FontRevision))
      {
        return aPublished->Font;
      }
      *aPublished = aCreatedEntry;
      return aCreated;
    }
    if (myFonts.Size() < myCapacity)
    {
      myFonts.Add(aRequest, aCreatedEntry);
    }
    else
    {
      myFonts.Substitute(myNextIndex, aRequest, aCreatedEntry);
      myNextIndex = myNextIndex < myCapacity ? myNextIndex + 1 : 1;
    }
    return aCreated;
  }
}

//=================================================================================================

void StdPrs_BRepFontCache::Clear()
{
  std::unique_lock<std::shared_mutex> aLock(myMutex);
  myFonts.Clear();
  myNextIndex = 1;
  ++myRevision;
}

//=================================================================================================

size_t StdPrs_BRepFontCache::Size() const
{
  std::shared_lock<std::shared_mutex> aLock(myMutex);
  return myFonts.Size();
}
