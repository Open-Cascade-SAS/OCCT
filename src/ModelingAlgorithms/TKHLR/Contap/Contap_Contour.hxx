// Created on: 1993-02-05
// Created by: Jacques GOUSSARD
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _Contap_Contour_HeaderFile
#define _Contap_Contour_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <Contap_Line.hxx>
#include <NCollection_Sequence.hxx>
#include <Contap_TheSearch.hxx>
#include <Contap_TheSearchInside.hxx>
#include <Contap_SurfFunction.hxx>
#include <Contap_ArcFunction.hxx>

class gp_Vec;
class gp_Pnt;
class Adaptor3d_TopolTool;

class Contap_Contour
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT Contap_Contour();

  Standard_EXPORT Contap_Contour(const gp_Vec& Direction);

  Standard_EXPORT Contap_Contour(const gp_Vec& Direction, const double Angle);

  Standard_EXPORT Contap_Contour(const gp_Pnt& Eye);

  //! Creates the contour in a given direction.
  Standard_EXPORT Contap_Contour(const occ::handle<Adaptor3d_Surface>&   Surf,
                                 const occ::handle<Adaptor3d_TopolTool>& Domain,
                                 const gp_Vec&                           Direction);

  //! Creates the contour in a given direction.
  Standard_EXPORT Contap_Contour(const occ::handle<Adaptor3d_Surface>&   Surf,
                                 const occ::handle<Adaptor3d_TopolTool>& Domain,
                                 const gp_Vec&                           Direction,
                                 const double                            Angle);

  //! Creates the contour for a perspective view.
  Standard_EXPORT Contap_Contour(const occ::handle<Adaptor3d_Surface>&   Surf,
                                 const occ::handle<Adaptor3d_TopolTool>& Domain,
                                 const gp_Pnt&                           Eye);

  //! Creates the contour in a given direction.
  Standard_EXPORT void Perform(const occ::handle<Adaptor3d_Surface>&   Surf,
                               const occ::handle<Adaptor3d_TopolTool>& Domain);

  //! Creates the contour in a given direction.
  Standard_EXPORT void Perform(const occ::handle<Adaptor3d_Surface>&   Surf,
                               const occ::handle<Adaptor3d_TopolTool>& Domain,
                               const gp_Vec&                           Direction);

  //! Creates the contour in a given direction.
  Standard_EXPORT void Perform(const occ::handle<Adaptor3d_Surface>&   Surf,
                               const occ::handle<Adaptor3d_TopolTool>& Domain,
                               const gp_Vec&                           Direction,
                               const double                            Angle);

  //! Creates the contour for a perspective view.
  Standard_EXPORT void Perform(const occ::handle<Adaptor3d_Surface>&   Surf,
                               const occ::handle<Adaptor3d_TopolTool>& Domain,
                               const gp_Pnt&                           Eye);

  Standard_EXPORT void Init(const gp_Vec& Direction);

  Standard_EXPORT void Init(const gp_Vec& Direction, const double Angle);

  Standard_EXPORT void Init(const gp_Pnt& Eye);

  bool IsDone() const;

  //! Returns true if the is no line.
  bool IsEmpty() const;

  int NbLines() const;

  const Contap_Line& Line(const int Index) const;

  //! Returns a reference on the internal SurfaceFunction.
  //! This is used to compute tangents on the lines.
  Contap_SurfFunction& SurfaceFunction();

private:
  Standard_EXPORT void Perform(const occ::handle<Adaptor3d_TopolTool>& Domain);

  Standard_EXPORT void PerformAna(const occ::handle<Adaptor3d_TopolTool>& Domain);

  bool                              done;
  NCollection_Sequence<Contap_Line> slin;
  Contap_TheSearch                  solrst;
  Contap_TheSearchInside            solins;
  Contap_SurfFunction               mySFunc;
  Contap_ArcFunction                myAFunc;
  bool                              modeset;
};

#include <Contap_Contour.lxx>

#endif // _Contap_Contour_HeaderFile
