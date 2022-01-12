// Created on: 1994-11-14
// Created by: Bruno DUMORTIER
// Copyright (c) 1994-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _BRepFill_MultiLine_HeaderFile
#define _BRepFill_MultiLine_HeaderFile

#include <AppCont_Function.hxx>

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>

#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <Standard_Integer.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Real.hxx>

class TopoDS_Edge;
class Geom2d_Curve;
class Geom_Curve;
class gp_Pnt;
class gp_Pnt2d;


//! Class used to compute the 3d curve and the
//! two 2d curves resulting from the intersection of a
//! surface of linear extrusion( Bissec, Dz) and the 2
//! faces.
//! This 3 curves will  have  the same parametrization
//! as the Bissectrice.
//! This  class  is  to  be  send  to an approximation
//! routine.
class BRepFill_MultiLine : public AppCont_Function
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepFill_MultiLine();

  Standard_EXPORT BRepFill_MultiLine(const TopoDS_Face& Face1, const TopoDS_Face& Face2, const TopoDS_Edge& Edge1, const TopoDS_Edge& Edge2, const Standard_Boolean Inv1, const Standard_Boolean Inv2, const Handle(Geom2d_Curve)& Bissec);

  //! Search if the Projection of the Bissectrice on the
  //! faces needs an approximation or not.
  //! Returns true if the approximation is not needed.
  Standard_EXPORT   Standard_Boolean IsParticularCase()  const;

  //! Returns   the continuity  betwwen  the two  faces
  //! seShape         from GeomAbsparated by myBis.
  Standard_EXPORT   GeomAbs_Shape Continuity()  const;

  //! raises if IsParticularCase is <False>.
  Standard_EXPORT   void Curves (Handle(Geom_Curve)& Curve, Handle(Geom2d_Curve)& PCurve1, Handle(Geom2d_Curve)& PCurve2)  const;

  //! returns the first parameter of the Bissectrice.
  Standard_EXPORT virtual Standard_Real FirstParameter()  const;

  //! returns the last parameter of the Bissectrice.
  Standard_EXPORT virtual Standard_Real LastParameter()  const;
  
  //! Returns the current point on the 3d curve
  Standard_EXPORT   gp_Pnt Value (const Standard_Real U)  const;

  //! returns the current point on the PCurve of the
  //! first face
  Standard_EXPORT   gp_Pnt2d ValueOnF1 (const Standard_Real U)  const;

  //! returns the current point on the PCurve of the
  //! first face
  Standard_EXPORT   gp_Pnt2d ValueOnF2 (const Standard_Real U)  const;

  Standard_EXPORT   void Value3dOnF1OnF2 (const Standard_Real U, gp_Pnt& P3d, gp_Pnt2d& PF1, gp_Pnt2d& PF2)  const;

  //! Returns the point at parameter <theU>.
  Standard_EXPORT virtual Standard_Boolean Value(const Standard_Real   theU,
                                                 NCollection_Array1<gp_Pnt2d>& thePnt2d,
                                                 NCollection_Array1<gp_Pnt>&   thePnt) const;

  //! Returns the derivative at parameter <theU>.
  Standard_EXPORT virtual Standard_Boolean D1(const Standard_Real   theU,
                                              NCollection_Array1<gp_Vec2d>& theVec2d,
                                              NCollection_Array1<gp_Vec>&   theVec) const;

private:
  TopoDS_Face myFace1;
  TopoDS_Face myFace2;
  Geom2dAdaptor_Curve myU1;
  Geom2dAdaptor_Curve myV1;
  Geom2dAdaptor_Curve myU2;
  Geom2dAdaptor_Curve myV2;
  Standard_Boolean myIsoU1;
  Standard_Boolean myIsoU2;
  Geom2dAdaptor_Curve myBis;
  Standard_Integer myKPart;
  GeomAbs_Shape myCont;
};

#endif // _BRepFill_MultiLine_HeaderFile
