// Created on: 1992-09-29
// Created by: Remi GILET
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef _GCE2d_Root_HeaderFile
#define _GCE2d_Root_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gce_ErrorType.hxx>
#include <Standard_Boolean.hxx>

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

//! This class implements the common services for
//! all classes of gce which report error.
class GCE2d_Root 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Returns true if the construction is successful.
    Standard_Boolean IsDone() const;
  
  //! Returns the status of the construction
  //! -   gce_Done, if the construction is successful, or
  //! -   another value of the gce_ErrorType enumeration
  //! indicating why the construction failed.
    gce_ErrorType Status() const;




protected:



  gce_ErrorType TheError;


private:





};


#include <GCE2d_Root.lxx>





#endif // _GCE2d_Root_HeaderFile
