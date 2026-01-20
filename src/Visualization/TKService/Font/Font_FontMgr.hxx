// Created on: 2008-01-20
// Created by: Alexander A. BORODIN
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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

#ifndef _Font_FontMgr_HeaderFile
#define _Font_FontMgr_HeaderFile

#include <Standard.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_List.hxx>
#include <Font_SystemFont.hxx>
#include <Font_StrictLevel.hxx>
#include <Font_UnicodeSubset.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_Shared.hxx>
#include <TCollection_HAsciiString.hxx>
#include <NCollection_Sequence.hxx>

class TCollection_HAsciiString;
class NCollection_Buffer;

//! Collects and provides information about available fonts in system.
class Font_FontMgr : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Font_FontMgr, Standard_Transient)
public:
  //! Return global instance of font manager.
  Standard_EXPORT static occ::handle<Font_FontMgr> GetInstance();

  //! Return font aspect as string.
  static const char* FontAspectToString(Font_FontAspect theAspect)
  {
    switch (theAspect)
    {
      case Font_FontAspect_UNDEFINED:
        return "undefined";
      case Font_FontAspect_Regular:
        return "regular";
      case Font_FontAspect_Bold:
        return "bold";
      case Font_FontAspect_Italic:
        return "italic";
      case Font_FontAspect_BoldItalic:
        return "bold-italic";
    }
    return "invalid";
  }

  //! Return flag to use fallback fonts in case if used font does not include symbols from specific
  //! Unicode subset; TRUE by default.
  Standard_EXPORT static bool& ToUseUnicodeSubsetFallback();

public:
  //! Return the list of available fonts.
  void AvailableFonts(NCollection_List<occ::handle<Font_SystemFont>>& theList) const
  {
    for (Font_FontMap::Iterator aFontIter(myFontMap); aFontIter.More(); aFontIter.Next())
    {
      theList.Append(aFontIter.Value());
    }
  }

  //! Return the list of available fonts.
  NCollection_List<occ::handle<Font_SystemFont>> GetAvailableFonts() const
  {
    NCollection_List<occ::handle<Font_SystemFont>> aList;
    AvailableFonts(aList);
    return aList;
  }

  //! Returns sequence of available fonts names
  Standard_EXPORT void GetAvailableFontsNames(
    NCollection_Sequence<occ::handle<TCollection_HAsciiString>>& theFontsNames) const;

  //! Returns font that match given parameters.
  //! If theFontName is empty string returned font can have any FontName.
  //! If theFontAspect is Font_FA_Undefined returned font can have any FontAspect.
  //! If theFontSize is "-1" returned font can have any FontSize.
  Standard_EXPORT occ::handle<Font_SystemFont> GetFont(
    const occ::handle<TCollection_HAsciiString>& theFontName,
    const Font_FontAspect                        theFontAspect,
    const int                                    theFontSize) const;

  //! Returns font that match given name or NULL if such font family is NOT registered.
  //! Note that unlike FindFont(), this method ignores font aliases and does not look for fall-back.
  Standard_EXPORT occ::handle<Font_SystemFont> GetFont(
    const TCollection_AsciiString& theFontName) const;

  //! Tries to find font by given parameters.
  //! If the specified font is not found tries to use font names mapping.
  //! If the requested family name not found -> search for any font family with given aspect and
  //! height. If the font is still not found, returns any font available in the system. Returns NULL
  //! in case when the fonts are not found in the system.
  //! @param[in] theFontName           font family to find or alias name
  //! @param[in] theStrictLevel        search strict level for using aliases and fallback
  //! @param[in][out] theFontAspect    font aspect to find (considered only if family name is not
  //! found);
  //!                                  can be modified if specified font alias refers to another
  //!                                  style (compatibility with obsolete aliases)
  //! @param[in] theDoFailMsg          put error message on failure into default messenger
  Standard_EXPORT occ::handle<Font_SystemFont> FindFont(const TCollection_AsciiString& theFontName,
                                                        Font_StrictLevel theStrictLevel,
                                                        Font_FontAspect& theFontAspect,
                                                        bool             theDoFailMsg = true) const;

  //! Tries to find font by given parameters.
  occ::handle<Font_SystemFont> FindFont(const TCollection_AsciiString& theFontName,
                                        Font_FontAspect&               theFontAspect) const
  {
    return FindFont(theFontName, Font_StrictLevel_Any, theFontAspect);
  }

  //! Tries to find fallback font for specified Unicode subset.
  //! Returns NULL in case when fallback font is not found in the system.
  //! @param[in] theSubset      Unicode subset
  //! @param[in] theFontAspect  font aspect to find
  Standard_EXPORT occ::handle<Font_SystemFont> FindFallbackFont(
    Font_UnicodeSubset theSubset,
    Font_FontAspect    theFontAspect) const;

  //! Read font file and retrieve information from it (the list of font faces).
  Standard_EXPORT bool CheckFont(NCollection_Sequence<occ::handle<Font_SystemFont>>& theFonts,
                                 const TCollection_AsciiString& theFontPath) const;

  //! Read font file and retrieve information from it.
  Standard_EXPORT occ::handle<Font_SystemFont> CheckFont(const char* const theFontPath) const;

  //! Register new font.
  //! If there is existing entity with the same name and properties but different path
  //! then font will be overridden or ignored depending on theToOverride flag.
  Standard_EXPORT bool RegisterFont(const occ::handle<Font_SystemFont>& theFont,
                                    const bool                          theToOverride);

  //! Register new fonts.
  bool RegisterFonts(const NCollection_Sequence<occ::handle<Font_SystemFont>>& theFonts,
                     const bool                                                theToOverride)
  {
    bool isRegistered = false;
    for (NCollection_Sequence<occ::handle<Font_SystemFont>>::Iterator aFontIter(theFonts);
         aFontIter.More();
         aFontIter.Next())
    {
      isRegistered = RegisterFont(aFontIter.Value(), theToOverride) || isRegistered;
    }
    return isRegistered;
  }

public:
  //! Return flag for tracing font aliases usage via Message_Trace messages; TRUE by default.
  bool ToTraceAliases() const { return myToTraceAliases; }

  //! Set flag for tracing font alias usage; useful to trace which fonts are actually used.
  //! Can be disabled to avoid redundant messages with Message_Trace level.
  void SetTraceAliases(bool theToTrace) { myToTraceAliases = theToTrace; }

  //! Return flag for printing error messages via Message_Fail messages; TRUE by default.
  bool ToPrintErrors() const { return myToPrintErrors; }

  //! Set flag for printing error messages.
  //! Can be disabled to avoid error messages with Message_Fail level.
  void SetPrintErrors(bool theToPrintErrors) { myToPrintErrors = theToPrintErrors; }

  //! Return font names with defined aliases.
  //! @param[out] theAliases  alias names
  Standard_EXPORT void GetAllAliases(
    NCollection_Sequence<occ::handle<TCollection_HAsciiString>>& theAliases) const;

  //! Return aliases to specified font name.
  //! @param[out] theFontNames  font names associated with alias name
  //! @param[in] theAliasName   alias name
  Standard_EXPORT void GetFontAliases(
    NCollection_Sequence<occ::handle<TCollection_HAsciiString>>& theFontNames,
    const TCollection_AsciiString&                               theAliasName) const;

  //! Register font alias.
  //!
  //! Font alias allows using predefined short-cuts like Font_NOF_MONOSPACE or Font_NOF_SANS_SERIF,
  //! and defining several fallback fonts like Font_NOF_CJK ("cjk") or "courier" for fonts,
  //! which availability depends on system.
  //!
  //! By default, Font_FontMgr registers standard aliases, which could be extended or replaced by
  //! application basing on better knowledge of the system or basing on additional fonts packaged
  //! with application itself. Aliases are defined "in advance", so that they could point to
  //! non-existing fonts, and they are resolved dynamically on request - first existing font is
  //! returned in case of multiple aliases to the same name.
  //!
  //! @param[in] theAliasName  alias name or name of another font to be used as alias
  //! @param[in] theFontName   font to be used as substitution for alias
  //! @return FALSE if alias has been already registered
  Standard_EXPORT bool AddFontAlias(const TCollection_AsciiString& theAliasName,
                                    const TCollection_AsciiString& theFontName);

  //! Unregister font alias.
  //! @param[in] theAliasName  alias name or name of another font to be used as alias;
  //!                          all aliases will be removed in case of empty name
  //! @param[in] theFontName   font to be used as substitution for alias;
  //!                          all fonts will be removed in case of empty name
  //! @return TRUE if alias has been removed
  Standard_EXPORT bool RemoveFontAlias(const TCollection_AsciiString& theAliasName,
                                       const TCollection_AsciiString& theFontName);

public:
  //! Collects available fonts paths.
  Standard_EXPORT void InitFontDataBase();

  //! Clear registry. Can be used for testing purposes.
  Standard_EXPORT void ClearFontDataBase();

  //! Return DejaVu font as embed a single fallback font.
  //! It can be used in cases when there is no own font file.
  //! Note: result buffer is readonly and should not be changed,
  //!       any data modification can lead to unpredictable consequences.
  Standard_EXPORT static occ::handle<NCollection_Buffer> EmbedFallbackFont();

private:
  //! Creates empty font manager object
  Standard_EXPORT Font_FontMgr();

private:
  struct FontHasher
  {
    size_t operator()(const occ::handle<Font_SystemFont>& theFont) const noexcept
    {
      return std::hash<TCollection_AsciiString>{}(theFont->FontKey());
    }

    bool operator()(const occ::handle<Font_SystemFont>& theFont1,
                    const occ::handle<Font_SystemFont>& theFont2) const
    {
      return theFont1->IsEqual(theFont2);
    }
  };

  //! Map storing registered fonts.
  class Font_FontMap : public NCollection_IndexedMap<occ::handle<Font_SystemFont>, FontHasher>
  {
  public:
    //! Empty constructor.
    Font_FontMap() = default;

    //! Try finding font with specified parameters or the closest one.
    //! @param[in] theFontName  font family to find (or empty string if family name can be ignored)
    //! @return best match font or NULL if not found
    occ::handle<Font_SystemFont> Find(const TCollection_AsciiString& theFontName) const;
  };

  //! Structure defining font alias.
  struct Font_FontAlias
  {
    TCollection_AsciiString FontName;
    Font_FontAspect         FontAspect;

    Font_FontAlias(const TCollection_AsciiString& theFontName,
                   Font_FontAspect                theFontAspect = Font_FontAspect_UNDEFINED)
        : FontName(theFontName),
          FontAspect(theFontAspect)
    {
    }

    Font_FontAlias()
        : FontAspect(Font_FontAspect_UNDEFINED)
    {
    }
  };

  //! Sequence of font aliases.
  typedef NCollection_Shared<NCollection_Sequence<Font_FontAlias>> Font_FontAliasSequence;

  //! Register font alias.
  void addFontAlias(const TCollection_AsciiString&             theAliasName,
                    const occ::handle<Font_FontAliasSequence>& theAliases,
                    Font_FontAspect theAspect = Font_FontAspect_UNDEFINED);

private:
  Font_FontMap                                                                      myFontMap;
  NCollection_DataMap<TCollection_AsciiString, occ::handle<Font_FontAliasSequence>> myFontAliases;
  occ::handle<Font_FontAliasSequence>                                               myFallbackAlias;
  bool myToTraceAliases;
  bool myToPrintErrors;
};

#endif // _Font_FontMgr_HeaderFile
