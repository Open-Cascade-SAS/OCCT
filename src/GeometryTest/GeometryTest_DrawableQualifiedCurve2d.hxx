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

class GeometryTest_DrawableQualifiedCurve2d;
DEFINE_STANDARD_HANDLE(GeometryTest_DrawableQualifiedCurve2d, DrawTrSurf_Curve)

//! Create geom curve drawable presentation with the position of a solution of a construction algorithm.
class GeometryTest_DrawableQualifiedCurve2d : public DrawTrSurf_Curve2d
{

public:


  //! Creates a drawable curve from a curve of package Geom.
  Standard_EXPORT GeometryTest_DrawableQualifiedCurve2d (const Handle(Geom2d_Curve)& theCurve,
                                                         const GccEnt_Position thePosition,
                                                         const Standard_Boolean theDispOrigin = Standard_True);
  
  //! Creates a drawable curve from a curve of package Geom.
  Standard_EXPORT GeometryTest_DrawableQualifiedCurve2d (const Handle(Geom2d_Curve)& theCurve,
                                                         const Draw_Color& theColor,
                                                         const Standard_Integer theDiscret,
                                                         const GccEnt_Position thePosition,
                                                         const Standard_Boolean theDispOrigin = Standard_True,
                                                         const Standard_Boolean theDispCurvRadius = Standard_False,
                                                         const Standard_Real theRadiusMax = 1.0e3,
                                                         const Standard_Real theRatioOfRadius = 0.1);

  //! \returns position of a solution
  GccEnt_Position GetPosition() const { return myPosition; }
  
  //! Sets position of a solution
  //! \param thePosition the value
  void SetPosition (const GccEnt_Position thePosition) { myPosition = thePosition; }

  //! Paints the drawable presentation in given display
  //! \param theDisplay
  Standard_EXPORT void DrawOn (Draw_Display& theDisplay) const Standard_OVERRIDE;

  //! For variable dump.
  Standard_EXPORT virtual void Dump (Standard_OStream& S) const Standard_OVERRIDE;

  //! For variable whatis command. Set  as a result  the
  //! type of the variable.
  Standard_EXPORT virtual void Whatis (Draw_Interpretor& I) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(GeometryTest_DrawableQualifiedCurve2d, DrawTrSurf_Curve2d)

protected:




private:


  GccEnt_Position myPosition;

};







#endif // _GeometryTest_DrawableQualifiedCirc_HeaderFile
