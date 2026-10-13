// Copyright (c) 2013-2026 OPEN CASCADE SAS
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

#ifndef StdPrs_BRepFont_HeaderFile
#define StdPrs_BRepFont_HeaderFile

#include <BRepFont_Builder.hxx>
#include <Font_FontAspect.hxx>
#include <Font_FTFont.hxx>
#include <Font_StrictLevel.hxx>
#include <Graphic3d_HorizontalTextAlignment.hxx>
#include <Graphic3d_VerticalTextAlignment.hxx>
#include <NCollection_String.hxx>
#include <Standard_Transient.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Ax3.hxx>

#include <cstddef>
#include <optional>
#include <mutex>

class StdPrs_BRepFontCache;
class Font_TextFormatter;

//! Creates BRep glyph and text shapes from one initialized font.
//! Rendering operations synchronize access to shared font and glyph-cache state.
//! Configuration changes and direct backend access require caller synchronization.
class StdPrs_BRepFont : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(StdPrs_BRepFont, Standard_Transient)
public:
  //! Options for rendering one glyph.
  struct GlyphOptions
  {
    //! Concatenate each contour into one C0 B-spline edge.
    bool ToConcatenateContours = false;
  };

  //! Options for rendering text.
  struct TextOptions
  {
    //! Position and orientation of the text baseline.
    gp_Ax3 Pen;
    //! Horizontal text alignment.
    Graphic3d_HorizontalTextAlignment HorizontalAlignment = Graphic3d_HTA_LEFT;
    //! Vertical text alignment.
    Graphic3d_VerticalTextAlignment VerticalAlignment = Graphic3d_VTA_BOTTOM;
    //! Concatenate each contour into one C0 B-spline edge.
    bool ToConcatenateContours = false;
  };

  //! Find and initialize a font.
  //! @param[in] theFontName requested font family
  //! @param[in] theFontAspect requested font style
  //! @param[in] theSize glyph size in model units
  //! @param[in] theStrictLevel font matching strictness
  //! @return initialized font, or a null handle when no font is found
  Standard_EXPORT static occ::handle<StdPrs_BRepFont> FindAndCreate(
    const TCollection_AsciiString& theFontName,
    const Font_FontAspect          theFontAspect,
    const double                   theSize,
    const Font_StrictLevel         theStrictLevel = Font_StrictLevel_Any);

  //! Create an uninitialized font.
  Standard_EXPORT StdPrs_BRepFont();

  //! Initialize a font from a file.
  //! @param[in] theFontPath font file path
  //! @param[in] theSize glyph size in model units
  //! @param[in] theFaceId face index within the file
  Standard_EXPORT StdPrs_BRepFont(const NCollection_String& theFontPath,
                                  const double              theSize,
                                  const int                 theFaceId = 0);

  //! Find and initialize a font by family name.
  //! @param[in] theFontName requested font family
  //! @param[in] theFontAspect requested font style
  //! @param[in] theSize glyph size in model units
  //! @param[in] theStrictLevel font matching strictness
  Standard_EXPORT StdPrs_BRepFont(const NCollection_String& theFontName,
                                  const Font_FontAspect     theFontAspect,
                                  const double              theSize,
                                  const Font_StrictLevel    theStrictLevel = Font_StrictLevel_Any);

  Standard_EXPORT ~StdPrs_BRepFont() override;

  //! Release the current font and cached glyph geometry.
  Standard_EXPORT virtual void Release();

  //! Return whether a font with a positive model-space size is initialized.
  [[nodiscard]] Standard_EXPORT bool IsValid() const;

  //! Initialize a font from a file.
  //! @param[in] theFontPath font file path
  //! @param[in] theSize glyph size in model units
  //! @param[in] theFaceId face index within the file
  //! @return true on success; the current font is preserved on failure
  Standard_EXPORT bool Init(const NCollection_String& theFontPath,
                            const double              theSize,
                            const int                 theFaceId = 0);

  //! Find and initialize a font by family name.
  //! @param[in] theFontName requested font family
  //! @param[in] theFontAspect requested font style
  //! @param[in] theSize glyph size in model units
  //! @param[in] theStrictLevel font matching strictness
  //! @return true on success; the current font is preserved on failure
  Standard_EXPORT bool FindAndInit(const TCollection_AsciiString& theFontName,
                                   const Font_FontAspect          theFontAspect,
                                   const double                   theSize,
                                   const Font_StrictLevel theStrictLevel = Font_StrictLevel_Any);

  //! Render one glyph.
  //! @param[in] theChar Unicode character
  //! @return generated shape, or a null shape when the glyph is unavailable
  Standard_EXPORT TopoDS_Shape RenderGlyph(const char32_t theChar);

  //! Render one glyph with explicit contour options.
  //! @param[in] theChar Unicode character
  //! @param[in] theOptions rendering options
  //! @return generated shape, or a null shape when the glyph is unavailable
  Standard_EXPORT TopoDS_Shape RenderGlyph(const char32_t theChar, const GlyphOptions& theOptions);

  //! Render text in the default XOY plane with left-bottom alignment.
  //! Missing characters use the primary font's .notdef glyph when no fallback glyph is available.
  //! @param[in] theText UTF text
  //! @return generated compound, or a null shape when no text can be rendered
  Standard_EXPORT TopoDS_Shape RenderText(const NCollection_String& theText);

  //! Render text with explicit placement and contour options.
  //! Missing characters use the primary font's .notdef glyph when no fallback glyph is available.
  //! @param[in] theText UTF text
  //! @param[in] theOptions rendering options
  //! @return generated compound, or a null shape when no text can be rendered
  Standard_EXPORT TopoDS_Shape RenderText(const NCollection_String& theText,
                                          const TextOptions&        theOptions);

  //! Render an existing layout in this font's pixel metric scale.
  //! @param[in] theFormatter formatter with completed layout using this font
  //! @param[in] thePenLoc text baseline placement
  //! @return text compound, or a null shape on invalid input or construction failure
  Standard_EXPORT TopoDS_Shape RenderText(const Font_TextFormatter& theFormatter,
                                          const gp_Ax3&             thePenLoc = gp_Ax3());

  //! Format text and load each unique planar glyph region.
  //! Missing characters use the primary font's .notdef glyph when no fallback glyph is available.
  //! @param[in] theText UTF text
  //! @param[in] theHorizontalAlignment horizontal layout alignment
  //! @param[in] theVerticalAlignment vertical layout alignment
  //! @return text plan including model-space bounds, or empty on failure or empty text
  [[nodiscard]] Standard_EXPORT std::optional<BRepFont_Builder::TextPlan> PlanText(
    const NCollection_String&               theText,
    const Graphic3d_HorizontalTextAlignment theHorizontalAlignment = Graphic3d_HTA_LEFT,
    const Graphic3d_VerticalTextAlignment   theVerticalAlignment   = Graphic3d_VTA_BOTTOM);

  //! Render a text plan, reusing cached glyph regions when available.
  //! The plan stores its glyph regions and remains valid after changing this font.
  //! @param[in] thePlan text plan
  //! @return generated compound, or a null shape when the plan is invalid
  [[nodiscard]] Standard_EXPORT TopoDS_Shape RenderText(const BRepFont_Builder::TextPlan& thePlan);

  //! Render a text plan, reusing cached glyph regions when available.
  //! The plan stores its glyph regions and remains valid after changing this font.
  //! @param[in] thePlan text plan
  //! @param[in] theOptions placement and contour options
  //! @return generated compound, or a null shape when the plan is invalid
  [[nodiscard]] Standard_EXPORT TopoDS_Shape RenderText(const BRepFont_Builder::TextPlan& thePlan,
                                                        const TextOptions& theOptions);

  //! Return the font backend after detaching it from shared font caches.
  //! The handle reference is borrowed until Init(), FindAndInit(), or Release().
  //! Backend access requires external synchronization with all operations on this font.
  //! Backend initialization and configuration changes update wrapper metrics on its next operation.
  //! Completed text plans retain their original geometry and layout.
  //! @return backend handle; throws Standard_ConstructionError if detaching fails
  Standard_DEPRECATED("Use font setters or Font_FTFont directly")
  Standard_EXPORT const occ::handle<Font_FTFont>& FTFont() const;

  //! Return an optional caller-owned lock for compatibility.
  //! Font operations do not acquire this mutex. Obtain it before sharing the instance,
  //! then use it to guard all mutable calls from different workers. It does not guard
  //! other wrappers returned by a shared cache; FTFont() detaches their shared backend.
  //! @return mutex owned by this font
  Standard_DEPRECATED("Use caller-owned synchronization for multi-call operations")
  Standard_EXPORT std::mutex& Mutex();

  //! Set contour concatenation for rendering calls without explicit options.
  //! Explicit GlyphOptions and TextOptions override this setting.
  //! @param[in] theToConcatenate true to build one C0 B-spline edge per contour
  Standard_DEPRECATED("Use ToConcatenateContours in GlyphOptions or TextOptions")
  void SetCompositeCurveMode(bool theToConcatenate) { myToConcatenateContours = theToConcatenate; }

  //! Change horizontal outline scaling and clear cached geometry.
  //! @param[in] theScaleFactor positive finite X scale
  //! @return true when the positive finite scale is accepted
  Standard_EXPORT bool SetWidthScaling(const float theScaleFactor);

  //! Set whether outlines should be interpreted as single-stroke geometry.
  //! Changing this mode clears cached glyph geometry.
  //! @param[in] theIsSingleStroke true to build open stroke wires instead of filled contours
  Standard_EXPORT void SetSingleStrokeFont(const bool theIsSingleStroke);

  //! Return the ascender in model units.
  Standard_EXPORT double Ascender() const;

  //! Return the descender in model units.
  Standard_EXPORT double Descender() const;

  //! Return the baseline spacing in model units.
  Standard_EXPORT double LineSpacing() const;

  //! Return the FreeType point size converted to model units.
  Standard_EXPORT double PointSize() const;

  //! Return horizontal advance from the last character rendered or selected by this wrapper.
  //! @param[in] theUCharNext next Unicode character
  Standard_EXPORT double AdvanceX(const char32_t theUCharNext);

  //! Return horizontal advance between two characters, including kerning.
  //! @param[in] theUChar current Unicode character
  //! @param[in] theUCharNext next Unicode character
  Standard_EXPORT double AdvanceX(const char32_t theUChar, const char32_t theUCharNext);

  //! Return vertical advance from the last character rendered or selected by this wrapper.
  //! @param[in] theUCharNext next Unicode character
  Standard_EXPORT double AdvanceY(const char32_t theUCharNext);

  //! Return vertical advance between two characters, including kerning.
  //! @param[in] theUChar current Unicode character
  //! @param[in] theUCharNext next Unicode character
  Standard_EXPORT double AdvanceY(const char32_t theUChar, const char32_t theUCharNext);

  //! Return the model-space scale applied to font metrics.
  [[nodiscard]] Standard_EXPORT double Scale() const;

  //! Return the configured glyph size in model units.
  [[nodiscard]] Standard_EXPORT double Size() const;

  //! Find and initialize a font by family name.
  //! Font aliases and the global fallback font may be used when the family is unavailable.
  //! @param[in] theFontName requested font family
  //! @param[in] theFontAspect requested font style
  //! @param[in] theSize glyph size in model units
  //! @return true if a suitable font was initialized; false leaves the current font unchanged
  bool Init(const NCollection_String& theFontName,
            const Font_FontAspect     theFontAspect,
            const double              theSize)
  {
    return FindAndInit(theFontName.ToCString(), theFontAspect, theSize, Font_StrictLevel_Any);
  }

private:
  friend class StdPrs_BRepFontCache;
  friend class StdPrs_BRepTextBuilder;

  class Impl;
  explicit StdPrs_BRepFont(const occ::handle<Impl>& theImpl);

  mutable occ::handle<Impl> myImpl;     //!< Font state, detached before exposing the backend.
  std::mutex                myMutex;    //!< Caller lock for operations using the exposed backend.
  char32_t myCurrentCharacter      = 0; //!< Per-wrapper character used by one-argument advances.
  bool     myToConcatenateContours = false; //!< Default contour mode for compatibility calls.
};

#endif // StdPrs_BRepFont_HeaderFile
