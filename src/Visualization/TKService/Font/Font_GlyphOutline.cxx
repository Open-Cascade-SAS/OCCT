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

#include <Font_GlyphOutline.hxx>

#include <utility>

Font_GlyphOutline Font_GlyphOutline::Create(const double   theUnitsPerEm,
                                            const FillRule theFillRule,
                                            const bool     theHasPossibleOverlaps,
                                            const bool     theIsSingleStroke,
                                            NCollection_LinearVector<Segment>&& theSegments,
                                            NCollection_LinearVector<Contour>&& theContours)
{
  Font_GlyphOutline anOutline;
  anOutline.myUnitsPerEm          = theUnitsPerEm;
  anOutline.myFillRule            = theFillRule;
  anOutline.myHasPossibleOverlaps = theHasPossibleOverlaps;
  anOutline.myIsSingleStroke      = theIsSingleStroke;
  anOutline.mySegments            = std::move(theSegments);
  anOutline.myContours            = std::move(theContours);
  return anOutline;
}
