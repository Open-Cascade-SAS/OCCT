// Created on: 1994-04-21
// Created by: s:	Christophe GUYOT & Frederic UNTEREINER
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

#ifndef _IGESToBRep_TopoCurve_HeaderFile
#define _IGESToBRep_TopoCurve_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColGeom_SequenceOfCurve.hxx>
#include <TColGeom2d_SequenceOfCurve.hxx>
#include <Standard_Boolean.hxx>
#include <IGESToBRep_CurveAndSurface.hxx>
#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
class TopoDS_Shape;
class IGESData_IGESEntity;
class TopoDS_Face;
class gp_Trsf2d;
class TopoDS_Vertex;
class IGESGeom_Point;
class IGESGeom_CompositeCurve;
class IGESGeom_OffsetCurve;
class IGESGeom_CurveOnSurface;
class IGESGeom_Boundary;
class Geom_BSplineCurve;
class Geom_Curve;
class Geom2d_BSplineCurve;
class Geom2d_Curve;


//! Provides methods to transfer topologic curves entities
//! from IGES to CASCADE.
class IGESToBRep_TopoCurve  : public IGESToBRep_CurveAndSurface
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates  a tool TopoCurve  ready  to  run, with
  //! epsilons  set  to  1.E-04,  TheModeTopo  to  True,  the
  //! optimization of  the continuity to False.
  Standard_EXPORT IGESToBRep_TopoCurve();
  
  //! Creates a tool TopoCurve ready to run and sets its
  //! fields as CS's.
  Standard_EXPORT IGESToBRep_TopoCurve(const IGESToBRep_CurveAndSurface& CS);
  
  //! Creates a tool TopoCurve ready to run and sets its
  //! fields as CS's.
  Standard_EXPORT IGESToBRep_TopoCurve(const IGESToBRep_TopoCurve& CS);
  
  //! Creates a tool TopoCurve ready to run.
  Standard_EXPORT IGESToBRep_TopoCurve(const Standard_Real eps, const Standard_Real epsGeom, const Standard_Real epsCoeff, const Standard_Boolean mode, const Standard_Boolean modeapprox, const Standard_Boolean optimized);
  
  Standard_EXPORT TopoDS_Shape TransferTopoCurve (const Handle(IGESData_IGESEntity)& start);
  
  Standard_EXPORT TopoDS_Shape Transfer2dTopoCurve (const Handle(IGESData_IGESEntity)& start, const TopoDS_Face& face, const gp_Trsf2d& trans, const Standard_Real uFact);
  
  Standard_EXPORT TopoDS_Shape TransferTopoBasicCurve (const Handle(IGESData_IGESEntity)& start);
  
  Standard_EXPORT TopoDS_Shape Transfer2dTopoBasicCurve (const Handle(IGESData_IGESEntity)& start, const TopoDS_Face& face, const gp_Trsf2d& trans, const Standard_Real uFact);
  
  Standard_EXPORT TopoDS_Vertex TransferPoint (const Handle(IGESGeom_Point)& start);
  
  Standard_EXPORT TopoDS_Vertex Transfer2dPoint (const Handle(IGESGeom_Point)& start);
  
  Standard_EXPORT TopoDS_Shape TransferCompositeCurve (const Handle(IGESGeom_CompositeCurve)& start);
  
  Standard_EXPORT TopoDS_Shape Transfer2dCompositeCurve (const Handle(IGESGeom_CompositeCurve)& start, const TopoDS_Face& face, const gp_Trsf2d& trans, const Standard_Real uFact);
  
  Standard_EXPORT TopoDS_Shape TransferOffsetCurve (const Handle(IGESGeom_OffsetCurve)& start);
  
  Standard_EXPORT TopoDS_Shape Transfer2dOffsetCurve (const Handle(IGESGeom_OffsetCurve)& start, const TopoDS_Face& face, const gp_Trsf2d& trans, const Standard_Real uFact);
  
  Standard_EXPORT TopoDS_Shape TransferCurveOnSurface (const Handle(IGESGeom_CurveOnSurface)& start);
  
  //! Transfers a CurveOnSurface directly on a face to trim it.
  //! The CurveOnSurface have to be defined Outer or Inner.
  Standard_EXPORT TopoDS_Shape TransferCurveOnFace (TopoDS_Face& face, const Handle(IGESGeom_CurveOnSurface)& start, const gp_Trsf2d& trans, const Standard_Real uFact, const Standard_Boolean IsCurv);
  
  Standard_EXPORT TopoDS_Shape TransferBoundary (const Handle(IGESGeom_Boundary)& start);
  
  //! Transfers a Boundary directly on a face to trim it.
  Standard_EXPORT TopoDS_Shape TransferBoundaryOnFace (TopoDS_Face& face, const Handle(IGESGeom_Boundary)& start, const gp_Trsf2d& trans, const Standard_Real uFact);
  
  Standard_EXPORT void ApproxBSplineCurve (const Handle(Geom_BSplineCurve)& start);
  
  //! Returns the count of Curves in "TheCurves"
  Standard_EXPORT Standard_Integer NbCurves() const;
  
  //! Returns a Curve given its rank, by default the first one
  //! (null Curvee if out of range) in "TheCurves"
  Standard_EXPORT Handle(Geom_Curve) Curve (const Standard_Integer num = 1) const;
  
  Standard_EXPORT void Approx2dBSplineCurve (const Handle(Geom2d_BSplineCurve)& start);
  
  //! Returns the count of Curves in "TheCurves2d"
  Standard_EXPORT Standard_Integer NbCurves2d() const;
  
  //! Returns a Curve given its rank, by default the first one
  //! (null Curvee if out of range) in "TheCurves2d"
  Standard_EXPORT Handle(Geom2d_Curve) Curve2d (const Standard_Integer num = 1) const;
  
  //! Sets TheBadCase flag
  Standard_EXPORT void SetBadCase (const Standard_Boolean value);
  
  //! Returns TheBadCase flag
  Standard_EXPORT Standard_Boolean BadCase() const;




protected:





private:

  
  Standard_EXPORT TopoDS_Shape TransferCompositeCurveGeneral (const Handle(IGESGeom_CompositeCurve)& start, const Standard_Boolean is2d, const TopoDS_Face& face, const gp_Trsf2d& trans, const Standard_Real uFact);


  TColGeom_SequenceOfCurve TheCurves;
  TColGeom2d_SequenceOfCurve TheCurves2d;
  Standard_Boolean TheBadCase;


};







#endif // _IGESToBRep_TopoCurve_HeaderFile
