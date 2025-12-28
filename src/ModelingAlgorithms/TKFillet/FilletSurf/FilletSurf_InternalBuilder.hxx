// Created on: 1996-07-26
// Created by: s:	Maria PUMBORIOS
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _FilletSurf_InternalBuilder_HeaderFile
#define _FilletSurf_InternalBuilder_HeaderFile

#include <BRepAdaptor_Curve2d.hxx>
#include <ChFi3d_FilBuilder.hxx>
#include <ChFi3d_FilletShape.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <ChFiDS_SurfData.hxx>
#include <NCollection_Sequence.hxx>
#include <math_Vector.hxx>
#include <TopAbs_Orientation.hxx>
#include <FilletSurf_StatusType.hxx>

class Adaptor3d_TopolTool;
class ChFiDS_Spine;
class Geom_Surface;
class Geom_Curve;
class Geom2d_Curve;
class Geom_TrimmedCurve;
class TopoDS_Shape;
class TopoDS_Face;

//! This class is private. It is used by the class Builder
//! from FilletSurf. It computes geometric information about fillets.
class FilletSurf_InternalBuilder : public ChFi3d_FilBuilder
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT FilletSurf_InternalBuilder(const TopoDS_Shape&      S,
                                             const ChFi3d_FilletShape FShape = ChFi3d_Polynomial,
                                             const double             Ta     = 1.0e-2,
                                             const double             Tapp3d = 1.0e-4,
                                             const double             Tapp2d = 1.0e-5);

  //! Initializes the contour with a list of Edges
  //! 0 : no problem
  //! 1 : empty list
  //! 2 : the edges are not G1
  //! 3 : two connected faces on a same support are not G1
  //! 4 : the edge is not on shape
  //! 5 : NotSharpEdge: the edge is not sharp
  Standard_EXPORT int Add(const NCollection_List<TopoDS_Shape>& E, const double R);

  Standard_EXPORT void Perform();

  Standard_EXPORT bool Done() const;

  //! gives the number of NUBS surfaces of the Fillet.
  Standard_EXPORT int NbSurface() const;

  //! gives the NUBS surface of index Index.
  Standard_EXPORT const occ::handle<Geom_Surface>& SurfaceFillet(const int Index) const;

  //! gives the 3d tolerance reached during approximation
  //! of the surface of index Index
  Standard_EXPORT double TolApp3d(const int Index) const;

  //! gives the first support face relative to SurfaceFillet(Index);
  Standard_EXPORT const TopoDS_Face& SupportFace1(const int Index) const;

  //! gives the second support face relative to SurfaceFillet(Index);
  Standard_EXPORT const TopoDS_Face& SupportFace2(const int Index) const;

  //! gives the 3d curve of SurfaceFillet(Index) on SupportFace1(Index)
  Standard_EXPORT const occ::handle<Geom_Curve>& CurveOnFace1(const int Index) const;

  //! gives the 3d curve of SurfaceFillet(Index) on SupportFace2(Index)
  Standard_EXPORT const occ::handle<Geom_Curve>& CurveOnFace2(const int Index) const;

  //! gives the PCurve associated to CurvOnSup1(Index) on the support face
  Standard_EXPORT const occ::handle<Geom2d_Curve>& PCurveOnFace1(const int Index) const;

  //! gives the PCurve associated to CurveOnFace1(Index) on the Fillet
  Standard_EXPORT const occ::handle<Geom2d_Curve>& PCurve1OnFillet(const int Index) const;

  //! gives the PCurve associated to CurveOnSup2(Index) on the support face
  Standard_EXPORT const occ::handle<Geom2d_Curve>& PCurveOnFace2(const int Index) const;

  //! gives the PCurve associated to CurveOnSup2(Index) on the fillet
  Standard_EXPORT const occ::handle<Geom2d_Curve>& PCurve2OnFillet(const int Index) const;

  //! gives the parameter of the fillet on the first edge.
  Standard_EXPORT double FirstParameter() const;

  //! gives the parameter of the fillet on the last edge
  Standard_EXPORT double LastParameter() const;

  Standard_EXPORT FilletSurf_StatusType StartSectionStatus() const;

  Standard_EXPORT FilletSurf_StatusType EndSectionStatus() const;

  Standard_EXPORT void Simulate();

  Standard_EXPORT int NbSection(const int IndexSurf) const;

  Standard_EXPORT void Section(const int                       IndexSurf,
                               const int                       IndexSec,
                               occ::handle<Geom_TrimmedCurve>& Circ) const;

protected:
  //! This method calculates the elements of construction of the
  //! fillet (constant or evolutive).
  Standard_EXPORT bool PerformSurf(NCollection_Sequence<occ::handle<ChFiDS_SurfData>>& SeqData,
                                   const occ::handle<ChFiDS_ElSpine>&                  Guide,
                                   const occ::handle<ChFiDS_Spine>&                    Spine,
                                   const int                                           Choix,
                                   const occ::handle<BRepAdaptor_Surface>&             S1,
                                   const occ::handle<Adaptor3d_TopolTool>&             I1,
                                   const occ::handle<BRepAdaptor_Surface>&             S2,
                                   const occ::handle<Adaptor3d_TopolTool>&             I2,
                                   const double                                        MaxStep,
                                   const double                                        Fleche,
                                   const double                                        TolGuide,
                                   double&                                             First,
                                   double&                                             Last,
                                   const bool                                          Inside,
                                   const bool                                          Appro,
                                   const bool                                          Forward,
                                   const bool                                          RecOnS1,
                                   const bool                                          RecOnS2,
                                   const math_Vector&                                  Soldep,
                                   int&                                                Intf,
                                   int& Intl) override;

  Standard_EXPORT void PerformSurf(NCollection_Sequence<occ::handle<ChFiDS_SurfData>>& SeqData,
                                   const occ::handle<ChFiDS_ElSpine>&                  Guide,
                                   const occ::handle<ChFiDS_Spine>&                    Spine,
                                   const int                                           Choix,
                                   const occ::handle<BRepAdaptor_Surface>&             S1,
                                   const occ::handle<Adaptor3d_TopolTool>&             I1,
                                   const occ::handle<BRepAdaptor_Curve2d>&             PC1,
                                   const occ::handle<BRepAdaptor_Surface>&             Sref1,
                                   const occ::handle<BRepAdaptor_Curve2d>&             PCref1,
                                   bool&                                               Decroch1,
                                   const occ::handle<BRepAdaptor_Surface>&             S2,
                                   const occ::handle<Adaptor3d_TopolTool>&             I2,
                                   const TopAbs_Orientation                            Or2,
                                   const double                                        MaxStep,
                                   const double                                        Fleche,
                                   const double                                        TolGuide,
                                   double&                                             First,
                                   double&                                             Last,
                                   const bool                                          Inside,
                                   const bool                                          Appro,
                                   const bool                                          Forward,
                                   const bool                                          RecP,
                                   const bool                                          RecS,
                                   const bool                                          RecRst,
                                   const math_Vector& Soldep) override;

  Standard_EXPORT void PerformSurf(NCollection_Sequence<occ::handle<ChFiDS_SurfData>>& SeqData,
                                   const occ::handle<ChFiDS_ElSpine>&                  Guide,
                                   const occ::handle<ChFiDS_Spine>&                    Spine,
                                   const int                                           Choix,
                                   const occ::handle<BRepAdaptor_Surface>&             S1,
                                   const occ::handle<Adaptor3d_TopolTool>&             I1,
                                   const TopAbs_Orientation                            Or1,
                                   const occ::handle<BRepAdaptor_Surface>&             S2,
                                   const occ::handle<Adaptor3d_TopolTool>&             I2,
                                   const occ::handle<BRepAdaptor_Curve2d>&             PC2,
                                   const occ::handle<BRepAdaptor_Surface>&             Sref2,
                                   const occ::handle<BRepAdaptor_Curve2d>&             PCref2,
                                   bool&                                               Decroch2,
                                   const double                                        MaxStep,
                                   const double                                        Fleche,
                                   const double                                        TolGuide,
                                   double&                                             First,
                                   double&                                             Last,
                                   const bool                                          Inside,
                                   const bool                                          Appro,
                                   const bool                                          Forward,
                                   const bool                                          RecP,
                                   const bool                                          RecS,
                                   const bool                                          RecRst,
                                   const math_Vector& Soldep) override;

  Standard_EXPORT void PerformSurf(NCollection_Sequence<occ::handle<ChFiDS_SurfData>>& Data,
                                   const occ::handle<ChFiDS_ElSpine>&                  Guide,
                                   const occ::handle<ChFiDS_Spine>&                    Spine,
                                   const int                                           Choix,
                                   const occ::handle<BRepAdaptor_Surface>&             S1,
                                   const occ::handle<Adaptor3d_TopolTool>&             I1,
                                   const occ::handle<BRepAdaptor_Curve2d>&             PC1,
                                   const occ::handle<BRepAdaptor_Surface>&             Sref1,
                                   const occ::handle<BRepAdaptor_Curve2d>&             PCref1,
                                   bool&                                               Decroch1,
                                   const TopAbs_Orientation                            Or1,
                                   const occ::handle<BRepAdaptor_Surface>&             S2,
                                   const occ::handle<Adaptor3d_TopolTool>&             I2,
                                   const occ::handle<BRepAdaptor_Curve2d>&             PC2,
                                   const occ::handle<BRepAdaptor_Surface>&             Sref2,
                                   const occ::handle<BRepAdaptor_Curve2d>&             PCref2,
                                   bool&                                               Decroch2,
                                   const TopAbs_Orientation                            Or2,
                                   const double                                        MaxStep,
                                   const double                                        Fleche,
                                   const double                                        TolGuide,
                                   double&                                             First,
                                   double&                                             Last,
                                   const bool                                          Inside,
                                   const bool                                          Appro,
                                   const bool                                          Forward,
                                   const bool                                          RecP1,
                                   const bool                                          RecRst1,
                                   const bool                                          RecP2,
                                   const bool                                          RecRst2,
                                   const math_Vector& Soldep) override;
};

#endif // _FilletSurf_InternalBuilder_HeaderFile
