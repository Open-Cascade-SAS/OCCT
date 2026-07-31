// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef _GeometryTest_DrawableQualifiedCirc_HeaderFile
#define _GeometryTest_DrawableQualifiedCirc_HeaderFile

#include <Standard.hxx>
#include <DrawTrSurf_Curve2d.hxx>
#include <GccEnt_Position.hxx>

class Geom2d_Curve;

//! Create geom curve drawable presentation with the position of a solution of a construction
//! algorithm.
class GeometryTest_DrawableQualifiedCurve2d : public DrawTrSurf_Curve2d
{

public:
  //! Creates a drawable curve from a curve of package Geom.
  Standard_EXPORT GeometryTest_DrawableQualifiedCurve2d(const occ::handle<Geom2d_Curve>& theCurve,
                                                        const GccEnt_Position thePosition,
                                                        const bool            theDispOrigin = true);

  //! Creates a drawable curve from a curve of package Geom.
  Standard_EXPORT GeometryTest_DrawableQualifiedCurve2d(const occ::handle<Geom2d_Curve>& theCurve,
                                                        const Draw_Color&                theColor,
                                                        const int                        theDiscret,
                                                        const GccEnt_Position thePosition,
                                                        const bool            theDispOrigin = true,
                                                        const bool   theDispCurvRadius      = false,
                                                        const double theRadiusMax           = 1.0e3,
                                                        const double theRatioOfRadius       = 0.1);

  //! \returns position of a solution
  GccEnt_Position GetPosition() const { return myPosition; }

  //! Sets position of a solution
  //! \param thePosition the value
  void SetPosition(const GccEnt_Position thePosition) { myPosition = thePosition; }

  //! Paints the drawable presentation in given display
  //! \param theDisplay
  Standard_EXPORT void DrawOn(Draw_Display& theDisplay) const override;

  //! For variable dump.
  Standard_EXPORT void Dump(Standard_OStream& S) const override;

  //! For variable whatis command. Set as a result the
  //! type of the variable.
  Standard_EXPORT void Whatis(Draw_Interpretor& I) const override;

  DEFINE_STANDARD_RTTIEXT(GeometryTest_DrawableQualifiedCurve2d, DrawTrSurf_Curve2d)

private:
  GccEnt_Position myPosition;
};

#endif // _GeometryTest_DrawableQualifiedCirc_HeaderFile
