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

#ifndef _Adaptor3d_FaceDomain_HeaderFile
#define _Adaptor3d_FaceDomain_HeaderFile

#include <Standard_Transient.hxx>
#include <TopAbs_State.hxx>

class gp_Pnt2d;

//! Minimal face-domain contract required by surface-domain intersection algorithms.
class Adaptor3d_FaceDomain : public Standard_Transient
{
public:
  //! Number of U samples for non-analytical surface polyhedron construction.
  Standard_EXPORT virtual int NbSamplesU() = 0;

  //! Number of V samples for non-analytical surface polyhedron construction.
  Standard_EXPORT virtual int NbSamplesV() = 0;

  //! Classify UV point against the face domain.
  Standard_EXPORT virtual TopAbs_State Classify(const gp_Pnt2d& thePnt,
                                                const double    theTol,
                                                const bool      theRecadreOnPeriodic = true) = 0;

  DEFINE_STANDARD_RTTIEXT(Adaptor3d_FaceDomain, Standard_Transient)
};

#endif // _Adaptor3d_FaceDomain_HeaderFile
