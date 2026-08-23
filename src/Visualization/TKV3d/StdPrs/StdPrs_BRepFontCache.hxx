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

#ifndef StdPrs_BRepFontCache_HeaderFile
#define StdPrs_BRepFontCache_HeaderFile

#include <Font_FontAspect.hxx>
#include <Font_StrictLevel.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <Standard_Transient.hxx>
#include <TCollection_AsciiString.hxx>

#include <cstddef>
#include <shared_mutex>

class StdPrs_BRepFont;

//! Bounded shareable cache of initialized BRep fonts.
//!
//! Each retained StdPrs_BRepFont owns its resolved glyph regions and output artifacts. This class
//! owns only the multi-font configuration map and can be shared by presentation algorithms that
//! need the same font resources without coupling them to a presentation manager or global state.
class StdPrs_BRepFontCache : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(StdPrs_BRepFontCache, Standard_Transient)
public:
  //! Construct an empty cache.
  //! @param[in] theCapacity maximum number of retained font configurations
  Standard_EXPORT explicit StdPrs_BRepFontCache(const size_t theCapacity = 16);

  //! Find or initialize a font for an exact configuration.
  //! @param[in] theFontName requested font family
  //! @param[in] theFontAspect requested font style
  //! @param[in] theSize glyph size in model units
  //! @param[in] theStrictLevel font matching strictness
  //! @return initialized cached font, or a null handle when the request cannot be satisfied
  [[nodiscard]] Standard_EXPORT occ::handle<StdPrs_BRepFont> FindFont(
    const TCollection_AsciiString& theFontName,
    const Font_FontAspect          theFontAspect,
    const double                   theSize,
    const Font_StrictLevel         theStrictLevel = Font_StrictLevel_Any);

  //! Release all retained font resources.
  Standard_EXPORT void Clear();

  //! Return the maximum number of retained font configurations.
  [[nodiscard]] size_t Capacity() const noexcept { return myCapacity; }

  //! Return the current number of retained font configurations.
  [[nodiscard]] Standard_EXPORT size_t Size() const;

private:
  struct Request
  {
    Request(const TCollection_AsciiString& theFontName,
            const Font_FontAspect          theFontAspect,
            const double                   theSize,
            const Font_StrictLevel         theStrictLevel)
        : FontName(theFontName),
          Size(theSize),
          FontAspect(theFontAspect),
          StrictLevel(theStrictLevel)
    {
    }

    TCollection_AsciiString FontName;
    double                  Size;
    Font_FontAspect         FontAspect;
    Font_StrictLevel        StrictLevel;
  };

  struct RequestHasher
  {
    [[nodiscard]] size_t operator()(const Request& theRequest) const noexcept;
    [[nodiscard]] bool   operator()(const Request& theLeft, const Request& theRight) const noexcept;
  };

  struct Entry
  {
    occ::handle<StdPrs_BRepFont> Font;
    size_t                       FontRevision;
  };

  size_t                                                    myCapacity;
  NCollection_IndexedDataMap<Request, Entry, RequestHasher> myFonts;
  size_t                                                    myNextIndex = 1;
  size_t                                                    myRevision  = 0;
  mutable std::shared_mutex                                 myMutex;
};

#endif // StdPrs_BRepFontCache_HeaderFile
