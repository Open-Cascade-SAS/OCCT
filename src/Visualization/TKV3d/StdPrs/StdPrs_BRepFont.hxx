// Created on: 2013-09-16
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

#ifndef _StdPrs_BRepFont_H__
#define _StdPrs_BRepFont_H__

#include <Adaptor3d_CurveOnSurface.hxx>
#include <BRep_Builder.hxx>
#include <Font_FTFont.hxx>
#include <Font_TextFormatter.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Geom2dConvert_CompCurveToBSplineCurve.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_String.hxx>
#include <gp_Pnt2d.hxx>
#include <NCollection_Array1.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_Sequence.hxx>

#include <mutex>

//! This tool provides basic services for rendering of vectorized text glyphs as BRep shapes.
//! Single instance initialize single font for sequential glyphs rendering with implicit caching of
//! already rendered glyphs. Thus position of each glyph in the text is specified by shape location.
//!
//! Please notice that this implementation uses mutex for thread-safety access,
//! thus may lead to performance penalties in case of concurrent access.
//! Although caching should eliminate this issue after rendering of sufficient number of glyphs.
class StdPrs_BRepFont : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(StdPrs_BRepFont, Standard_Transient)
public:
  //! Find the font Initialize the font.
  //! @param theFontName    the font name
  //! @param theFontAspect  the font style
  //! @param theSize        the face size in model units
  //! @param theStrictLevel search strict level for using aliases and fallback
  //! @return true on success
  Standard_EXPORT static occ::handle<StdPrs_BRepFont> FindAndCreate(
    const TCollection_AsciiString& theFontName,
    const Font_FontAspect          theFontAspect,
    const double                   theSize,
    const Font_StrictLevel         theStrictLevel = Font_StrictLevel_Any);

  //! Empty constructor
  Standard_EXPORT StdPrs_BRepFont();

  //! Constructor with initialization.
  //! @param theFontPath FULL path to the font
  //! @param theSize     the face size in model units
  //! @param theFaceId   face id within the file (0 by default)
  Standard_EXPORT StdPrs_BRepFont(const NCollection_String& theFontPath,
                                  const double              theSize,
                                  const int                 theFaceId = 0);

  //! Constructor with initialization.
  //! @param theFontName    the font name
  //! @param theFontAspect  the font style
  //! @param theSize        the face size in model units
  //! @param theStrictLevel search strict level for using aliases and fallback
  Standard_EXPORT StdPrs_BRepFont(const NCollection_String& theFontName,
                                  const Font_FontAspect     theFontAspect,
                                  const double              theSize,
                                  const Font_StrictLevel    theStrictLevel = Font_StrictLevel_Any);

  //! Release currently loaded font.
  Standard_EXPORT virtual void Release();

  //! Initialize the font.
  //! @param theFontPath FULL path to the font
  //! @param theSize     the face size in model units
  //! @param theFaceId   face id within the file (0 by default)
  //! @return true on success
  Standard_EXPORT bool Init(const NCollection_String& theFontPath,
                            const double              theSize,
                            const int                 theFaceId);

  //! Find (using Font_FontMgr) and initialize the font from the given name.
  //! Please take into account that size is specified NOT in typography points (pt.).
  //! If you need to specify size in points, value should be converted.
  //! Formula for pt. -> m conversion:
  //!   aSizeMeters = 0.0254 * theSizePt / 72.0
  //! @param theFontName   the font name
  //! @param theFontAspect the font style
  //! @param theSize       the face size in model units
  //! @param theStrictLevel search strict level for using aliases and fallback
  //! @return true on success
  Standard_EXPORT bool FindAndInit(const TCollection_AsciiString& theFontName,
                                   const Font_FontAspect          theFontAspect,
                                   const double                   theSize,
                                   const Font_StrictLevel theStrictLevel = Font_StrictLevel_Any);

  //! Return wrapper over FreeType font.
  const occ::handle<Font_FTFont>& FTFont() const { return myFTFont; }

  //! Render single glyph as TopoDS_Shape.
  //! @param theChar glyph identifier
  //! @return rendered glyph within cache, might be NULL shape
  Standard_EXPORT TopoDS_Shape RenderGlyph(const char32_t& theChar);

  //! Setup glyph geometry construction mode.
  //! By default algorithm creates independent TopoDS_Edge
  //! for each original curve in the glyph (line segment or Bezie curve).
  //! Algorithm might optionally create composite BSpline curve for each contour
  //! which reduces memory footprint but limits curve class to C0.
  //! Notice that altering this flag clears currently accumulated cache!
  Standard_EXPORT void SetCompositeCurveMode(const bool theToConcatenate);

  //! Setup glyph scaling along X-axis.
  //! By default glyphs are not scaled (scaling factor = 1.0)
  void SetWidthScaling(const float theScaleFactor) { myFTFont->SetWidthScaling(theScaleFactor); }

public:
  //! @return vertical distance from the horizontal baseline to the highest character coordinate.
  double Ascender() const { return myScaleUnits * double(myFTFont->Ascender()); }

  //! @return vertical distance from the horizontal baseline to the lowest character coordinate.
  double Descender() const { return myScaleUnits * double(myFTFont->Descender()); }

  //! @return default line spacing (the baseline-to-baseline distance).
  double LineSpacing() const { return myScaleUnits * double(myFTFont->LineSpacing()); }

  //! Configured point size
  double PointSize() const { return myScaleUnits * double(myFTFont->PointSize()); }

  //! Compute advance to the next character with kerning applied when applicable.
  //! Assuming text rendered horizontally.
  double AdvanceX(const char32_t theUCharNext)
  {
    return myScaleUnits * double(myFTFont->AdvanceX(theUCharNext));
  }

  //! Compute advance to the next character with kerning applied when applicable.
  //! Assuming text rendered horizontally.
  double AdvanceX(const char32_t theUChar, const char32_t theUCharNext)
  {
    return myScaleUnits * double(myFTFont->AdvanceX(theUChar, theUCharNext));
  }

  //! Compute advance to the next character with kerning applied when applicable.
  //! Assuming text rendered vertically.
  double AdvanceY(const char32_t theUCharNext)
  {
    return myScaleUnits * double(myFTFont->AdvanceY(theUCharNext));
  }

  //! Compute advance to the next character with kerning applied when applicable.
  //! Assuming text rendered vertically.
  double AdvanceY(const char32_t theUChar, const char32_t theUCharNext)
  {
    return myScaleUnits * double(myFTFont->AdvanceY(theUChar, theUCharNext));
  }

  //! Returns scaling factor for current font size.
  double Scale() const { return myScaleUnits; }

  //! Returns mutex.
  std::mutex& Mutex() { return myMutex; }

public:
  //! Find (using Font_FontMgr) and initialize the font from the given name.
  //! Alias for FindAndInit() for backward compatibility.
  bool Init(const NCollection_String& theFontName,
            const Font_FontAspect     theFontAspect,
            const double              theSize)
  {
    return FindAndInit(theFontName.ToCString(), theFontAspect, theSize, Font_StrictLevel_Any);
  }

protected:
  //! Render single glyph as TopoDS_Shape. This method does not lock the mutex.
  //! @param theChar  glyph identifier
  //! @param theShape rendered glyph within cache, might be NULL shape
  //! @return true if glyph's geometry is available
  Standard_EXPORT bool renderGlyph(const char32_t theChar, TopoDS_Shape& theShape);

private:
  //! Initialize class fields
  void init();

  //! Auxiliary method to create 3D curve
  bool to3d(const occ::handle<Geom2d_Curve>& theCurve2d,
            const GeomAbs_Shape              theContinuity,
            occ::handle<Geom_Curve>&         theCurve3d);

  //! Auxiliary method for creation faces from sequence of wires.
  //! Splits to few faces (if it is needed) and updates orientation of wires.
  bool buildFaces(const NCollection_Sequence<TopoDS_Wire>& theWires, TopoDS_Shape& theRes);

protected:                                                 //! @name Protected fields
  occ::handle<Font_FTFont>                    myFTFont;    //!< wrapper over FreeType font
  NCollection_DataMap<char32_t, TopoDS_Shape> myCache;     //!< glyphs cache
  std::mutex                                  myMutex;     //!< lock for thread-safety
  occ::handle<Geom_Surface>                   mySurface;   //!< surface to place glyphs on to
  double                                      myPrecision; //!< algorithm precision
  double myScaleUnits; //!< scale font rendering units into model units
  // clang-format off
  bool     myIsCompositeCurve; //!< flag to merge C1 curves of each contour into single C0 curve, OFF by default
  // clang-format on

protected: //! @name Shared temporary variables for glyph construction
  Adaptor3d_CurveOnSurface              myCurvOnSurf;
  occ::handle<Geom2dAdaptor_Curve>      myCurve2dAdaptor;
  Geom2dConvert_CompCurveToBSplineCurve myConcatMaker;
  NCollection_Array1<gp_Pnt2d>          my3Poles;
  NCollection_Array1<gp_Pnt2d>          my4Poles;
  BRep_Builder                          myBuilder;
};

#endif // _StdPrs_BRepFont_H__
