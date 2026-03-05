// Copyright (c) 2025 OPEN CASCADE SAS
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

#ifndef BndLib_LegacyCheck_hxx
#define BndLib_LegacyCheck_hxx

//! Temporary debugging facility: compares old (pre-migration) BndLib bounding
//! box results against the new GeomBndLib-delegating results. Prints to
//! std::cerr when the boxes differ. Remove once all regressions are resolved.

#include <Bnd_Box.hxx>
#include <Bnd_Box2d.hxx>
#include <Geom2d_Curve.hxx>
#include <Standard_Handle.hxx>

class Adaptor3d_Curve;
class Adaptor3d_Surface;

namespace BndLib_LegacyCheck
{

//! Compare old Add3dCurve result with new; print to std::cerr if different.
void Compare3dCurveAdd(const Adaptor3d_Curve& theC,
                       double                 theU1,
                       double                 theU2,
                       double                 theTol,
                       const Bnd_Box&         theNewBox);

//! Compare old AddOptimal3dCurve result with new; print to std::cerr if different.
void Compare3dCurveAddOptimal(const Adaptor3d_Curve& theC,
                              double                 theU1,
                              double                 theU2,
                              double                 theTol,
                              const Bnd_Box&         theNewBox);

//! Compare old AddSurface result with new; print to std::cerr if different.
void CompareSurfaceAdd(const Adaptor3d_Surface& theS,
                       double                   theUMin,
                       double                   theUMax,
                       double                   theVMin,
                       double                   theVMax,
                       double                   theTol,
                       const Bnd_Box&           theNewBox);

//! Compare old AddOptimalSurface result with new; print to std::cerr if different.
void CompareSurfaceAddOptimal(const Adaptor3d_Surface& theS,
                              double                   theUMin,
                              double                   theUMax,
                              double                   theVMin,
                              double                   theVMax,
                              double                   theTol,
                              const Bnd_Box&           theNewBox);

//! Compare old Add2dCurve result with new; print to std::cerr if different.
void Compare2dCurveAdd(const occ::handle<Geom2d_Curve>& theC,
                       double                      theT1,
                       double                      theT2,
                       double                      theTol,
                       const Bnd_Box2d&            theNewBox);

//! Compare old AddOptimal2dCurve result with new; print to std::cerr if different.
void Compare2dCurveAddOptimal(const occ::handle<Geom2d_Curve>& theC,
                              double                      theT1,
                              double                      theT2,
                              double                      theTol,
                              const Bnd_Box2d&            theNewBox);

} // namespace BndLib_LegacyCheck

#endif // BndLib_LegacyCheck_hxx
