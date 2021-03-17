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
  virtual ~SelectMgr_BaseFrustum() {}

  //! Nullifies the builder created in the constructor and copies the pointer given
  Standard_EXPORT void SetBuilder (const Handle(SelectMgr_FrustumBuilder)& theBuilder);

  //! Saves camera definition and passes it to builder
  Standard_EXPORT virtual void SetCamera (const Handle(Graphic3d_Camera)& theCamera) Standard_OVERRIDE;

  Standard_EXPORT virtual void SetPixelTolerance (const Standard_Integer theTol) Standard_OVERRIDE;

  Standard_EXPORT virtual void SetWindowSize (const Standard_Integer theWidth,
                                              const Standard_Integer theHeight) Standard_OVERRIDE;

  Standard_EXPORT virtual void WindowSize (Standard_Integer& theWidth,
                                           Standard_Integer& theHeight) const Standard_OVERRIDE;

  //! Passes viewport parameters to builder
  Standard_EXPORT virtual void SetViewport (const Standard_Real theX,
                                            const Standard_Real theY,
                                            const Standard_Real theWidth,
                                            const Standard_Real theHeight) Standard_OVERRIDE;

  //! Checks whether the boundary of the current volume selection intersects with a sphere or are there it's boundaries lying inside the sphere
  Standard_EXPORT Standard_Boolean IsBoundaryIntersectSphere (const gp_Pnt& theCenter,
                                                              const Standard_Real theRadius,
                                                              const gp_Dir& thePlaneNormal,
                                                              const TColgp_Array1OfPnt& theBoundaries,
                                                              Standard_Boolean& theBoundaryInside) const;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(SelectMgr_BaseFrustum, SelectMgr_BaseIntersector)

protected:
  Standard_Integer myPixelTolerance; //!< Pixel tolerance

  Handle(SelectMgr_FrustumBuilder) myBuilder; //!< A tool implementing methods for volume build
};

#endif // _SelectMgr_BaseFrustum_HeaderFile
