// Created on: 2016-07-07
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
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

#ifndef _IMeshTools_MeshAlgo_HeaderFile
#define _IMeshTools_MeshAlgo_HeaderFile

#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <IMeshData_Types.hxx>

struct IMeshTools_Parameters;

//! Interface class providing API for algorithms intended to create mesh for discrete face.
class IMeshTools_MeshAlgo : public Standard_Transient
{
public:

  //! Destructor.
  Standard_EXPORT virtual ~IMeshTools_MeshAlgo()
  {
  }

  //! Performs processing of the given face.
  Standard_EXPORT virtual void Perform(
    const IMeshData::IFaceHandle& theDFace,
    const IMeshTools_Parameters&  theParameters) = 0;

  DEFINE_STANDARD_RTTI_INLINE(IMeshTools_MeshAlgo, Standard_Transient)

protected:

  //! Constructor.
  Standard_EXPORT IMeshTools_MeshAlgo()
  {
  }
};

#endif