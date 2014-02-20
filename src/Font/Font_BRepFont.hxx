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

#ifndef _Font_BRepFont_H__
#define _Font_BRepFont_H__

#include <Adaptor3d_CurveOnSurface.hxx>
#include <BRep_Builder.hxx>
#include <Font_FTFont.hxx>
#include <Geom2dAdaptor_HCurve.hxx>
#include <Geom2dConvert_CompCurveToBSplineCurve.hxx>
#include <gp_Ax3.hxx>
#include <gp_XY.hxx>
#include <gp_XYZ.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_String.hxx>
#include <Standard_Mutex.hxx>
#include <ShapeFix_Face.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>

//! This tool provides basic services for rendering of vectorized text glyphs as BRep shapes.
//! Single instance initialize single font for sequential glyphs rendering with implicit caching of already rendered glyphs.
//! Thus position of each glyph in the text is specified by shape location.
//!
//! Please notice that this implementation uses mutex for thread-safety access,
//! thus may lead to performance penalties in case of concurrent access.
//! Although caching should eliminate this issue after rendering of sufficient number of glyphs.
class Font_BRepFont : protected Font_FTFont
{
public:

  //! Empty constructor
  Standard_EXPORT Font_BRepFont();

  //! Constructor with initialization.
  //! @param theFontPath FULL path to the font
  //! @param theSize     the face size in model units
  Standard_EXPORT Font_BRepFont (const NCollection_String& theFontPath,
                                 const Standard_Real       theSize);

  //! Constructor with initialization.
  //! @param theFontName   the font name
  //! @param theFontAspect the font style
  //! @param theSize       the face size in model units
  Standard_EXPORT Font_BRepFont (const NCollection_String& theFontName,
                                 const Font_FontAspect     theFontAspect,
                                 const Standard_Real       theSize);

  //! Release currently loaded font.
  Standard_EXPORT virtual void Release();

  //! Initialize the font.
  //! @param theFontPath FULL path to the font
  //! @param theSize     the face size in model units
  //! @return true on success
  Standard_EXPORT bool Init (const NCollection_String& theFontPath,
                             const Standard_Real       theSize);

  //! Initialize the font.
  //! Please take into account that size is specified NOT in typography points (pt.).
  //! If you need to specify size in points, value should be converted.
  //! Formula for pt. -> m conversion:
  //!   aSizeMeters = 0.0254 * theSizePt / 72.0
  //! @param theFontName   the font name
  //! @param theFontAspect the font style
  //! @param theSize       the face size in model units
  //! @return true on success
  Standard_EXPORT bool Init (const NCollection_String& theFontName,
                             const Font_FontAspect     theFontAspect,
                             const Standard_Real       theSize);

  //! Render single glyph as TopoDS_Shape.
  //! @param theChar glyph identifier
  //! @return rendered glyph within cache, might be NULL shape
  Standard_EXPORT TopoDS_Shape RenderGlyph (const Standard_Utf32Char& theChar);

  //! Setup glyph geometry construction mode.
  //! By default algorithm creates independent TopoDS_Edge
  //! for each original curve in the glyph (line segment or Bezie curve).
  //! Algorithm might optionally create composite BSpline curve for each contour
  //! which reduces memory footprint but limits curve class to C0.
  //! Notice that altering this flag clears currently accumulated cache!
  Standard_EXPORT void SetCompositeCurveMode (const Standard_Boolean theToConcatenate);

public:

  //! Render text as BRep shape.
  //! @param theString text in UTF-8 encoding
  //! @return result shape within XOY plane and start position (0,0,0) on the baseline
  Standard_EXPORT TopoDS_Shape RenderText (const NCollection_String& theString);

  //! Render text as BRep shape.
  //! @param theString text in UTF-8 encoding
  //! @param thePenLoc start position and orientation on the baseline
  //! @return result shape with pen transformation applied as shape location
  TopoDS_Shape RenderText (const NCollection_String& theString,
                           const gp_Ax3&             thePenLoc)
  {
    TopoDS_Shape aResult = RenderText (theString);
    gp_Trsf aTrsf;
    aTrsf.SetTransformation (thePenLoc, gp_Ax3 (gp::XOY()));
    aResult.Move (aTrsf);
    return aResult;
  }

public:

  //! @return vertical distance from the horizontal baseline to the highest character coordinate.
  Standard_Real Ascender() const
  {
    return myScaleUnits * Standard_Real(Font_FTFont::Ascender());
  }

  //! @return vertical distance from the horizontal baseline to the lowest character coordinate.
  Standard_Real Descender() const
  {
    return myScaleUnits * Standard_Real(Font_FTFont::Descender());
  }

  //! @return default line spacing (the baseline-to-baseline distance).
  Standard_Real LineSpacing() const
  {
    return myScaleUnits * Standard_Real(Font_FTFont::LineSpacing());
  }

  //! Configured point size
  Standard_Real PointSize() const
  {
    return myScaleUnits * Standard_Real(Font_FTFont::PointSize());
  }

  //! Compute advance to the next character with kerning applied when applicable.
  //! Assuming text rendered horizontally.
  Standard_Real AdvanceX (const Standard_Utf32Char theUCharNext)
  {
    return myScaleUnits * Standard_Real(Font_FTFont::AdvanceX (theUCharNext));
  }

  //! Compute advance to the next character with kerning applied when applicable.
  //! Assuming text rendered horizontally.
  Standard_Real AdvanceX (const Standard_Utf32Char theUChar,
                          const Standard_Utf32Char theUCharNext)
  {
    return myScaleUnits * Standard_Real(Font_FTFont::AdvanceX (theUChar, theUCharNext));
  }

  //! Compute advance to the next character with kerning applied when applicable.
  //! Assuming text rendered vertically.
  Standard_Real AdvanceY (const Standard_Utf32Char theUCharNext)
  {
    return myScaleUnits * Standard_Real(Font_FTFont::AdvanceY (theUCharNext));
  }

  //! Compute advance to the next character with kerning applied when applicable.
  //! Assuming text rendered vertically.
  Standard_Real AdvanceY (const Standard_Utf32Char theUChar,
                          const Standard_Utf32Char theUCharNext)
  {
    return myScaleUnits * Standard_Real(Font_FTFont::AdvanceY (theUChar, theUCharNext));
  }

protected:

  //! Render single glyph as TopoDS_Shape. This method does not lock the mutex.
  //! @param theChar  glyph identifier
  //! @param theShape rendered glyph within cache, might be NULL shape
  //! @return true if glyph's geometry is available
  Standard_EXPORT Standard_Boolean renderGlyph (const Standard_Utf32Char theChar,
                                                TopoDS_Shape&            theShape);

private:

  //! Initialize class fields
  void init();

  //! Auxiliary method to create 3D curve
  bool to3d (const Handle(Geom2d_Curve) theCurve2d,
             const GeomAbs_Shape        theContinuity,
             Handle(Geom_Curve)&        theCurve3d);

  //! Auxiliary method to convert FT_Vector to gp_XY
  gp_XY readFTVec (const FT_Vector& theVec) const
  {
    return gp_XY (myScaleUnits * Standard_Real(theVec.x) / 64.0, myScaleUnits * Standard_Real(theVec.y) / 64.0);
  }

protected: //! @name Protected fields

  NCollection_DataMap<Standard_Utf32Char, TopoDS_Shape>
                       myCache;            //!< glyphs cache
  Standard_Mutex       myMutex;            //!< lock for thread-safety
  Handle(Geom_Surface) mySurface;          //!< surface to place glyphs on to
  Standard_Real        myPrecision;        //!< algorithm precision
  Standard_Real        myScaleUnits;       //!< scale font rendering units into model units
  Standard_Boolean     myIsCompositeCurve; //!< flag to merge C1 curves of each contour into single C0 curve, ON by default

protected: //! @name Shared temporary variables for glyph construction

  Adaptor3d_CurveOnSurface myCurvOnSurf;
  Handle(Geom2dAdaptor_HCurve) myCurve2dAdaptor;
  Geom2dConvert_CompCurveToBSplineCurve myConcatMaker;
  TColgp_Array1OfPnt2d     my3Poles;
  TColgp_Array1OfPnt2d     my4Poles;
  BRep_Builder             myBuilder;
  ShapeFix_Face            myFixer;

public:

  DEFINE_STANDARD_RTTI(Font_BRepFont)

};

// Definition of HANDLE object using Standard_DefineHandle.hxx
DEFINE_STANDARD_HANDLE(Font_BRepFont, Font_FTFont)

#endif // _Font_BRepFont_H__
