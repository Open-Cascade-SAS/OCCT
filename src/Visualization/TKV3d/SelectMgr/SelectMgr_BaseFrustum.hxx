// Created on: 2014-05-22
// Created by: Varvara POSKONINA
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#ifndef _SelectMgr_BaseFrustum_HeaderFile
#define _SelectMgr_BaseFrustum_HeaderFile

#include <SelectMgr_BaseIntersector.hxx>

//! This class is an interface for different types of selecting frustums,
//! defining different selection types, like point, box or polyline
//! selection. It contains signatures of functions for detection of
//! overlap by sensitive entity and initializes some data for building
//! the selecting frustum
class SelectMgr_BaseFrustum : public SelectMgr_BaseIntersector
{
public:
  //! Creates new selecting volume with pixel tolerance set to 2,
  //! orthographic camera and empty frustum builder
  Standard_EXPORT SelectMgr_BaseFrustum();

  //! Destructor
  ~SelectMgr_BaseFrustum() override = default;

  //! Nullifies the builder created in the constructor and copies the pointer given
  Standard_EXPORT void SetBuilder(const occ::handle<SelectMgr_FrustumBuilder>& theBuilder);

  //! Saves camera definition and passes it to builder
  Standard_EXPORT void SetCamera(const occ::handle<Graphic3d_Camera>& theCamera) override;

  Standard_EXPORT void SetPixelTolerance(const int theTol) override;

  Standard_EXPORT void SetWindowSize(const int theWidth, const int theHeight) override;

  Standard_EXPORT void WindowSize(int& theWidth, int& theHeight) const override;

  //! Passes viewport parameters to builder
  Standard_EXPORT void SetViewport(const double theX,
                                   const double theY,
                                   const double theWidth,
                                   const double theHeight) override;

  //! Checks whether the boundary of the current volume selection intersects with a sphere or are
  //! there it's boundaries lying inside the sphere
  Standard_EXPORT bool IsBoundaryIntersectSphere(const gp_Pnt&                     theCenter,
                                                 const double                      theRadius,
                                                 const gp_Dir&                     thePlaneNormal,
                                                 const NCollection_Array1<gp_Pnt>& theBoundaries,
                                                 bool& theBoundaryInside) const;

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const override;

  DEFINE_STANDARD_RTTIEXT(SelectMgr_BaseFrustum, SelectMgr_BaseIntersector)

protected:
  int myPixelTolerance; //!< Pixel tolerance

  occ::handle<SelectMgr_FrustumBuilder> myBuilder; //!< A tool implementing methods for volume build
};

#endif // _SelectMgr_BaseFrustum_HeaderFile
