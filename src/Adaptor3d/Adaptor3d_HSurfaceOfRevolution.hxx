// Created on: 1992-10-08
// Created by: Isabelle GRIGNON
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

#ifndef _Adaptor3d_HSurfaceOfRevolution_HeaderFile
#define _Adaptor3d_HSurfaceOfRevolution_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Adaptor3d_SurfaceOfRevolution.hxx>
#include <Adaptor3d_HSurface.hxx>
class Standard_OutOfRange;
class Standard_NoSuchObject;
class Standard_DomainError;
class Adaptor3d_SurfaceOfRevolution;
class Adaptor3d_Surface;


class Adaptor3d_HSurfaceOfRevolution;
DEFINE_STANDARD_HANDLE(Adaptor3d_HSurfaceOfRevolution, Adaptor3d_HSurface)


class Adaptor3d_HSurfaceOfRevolution : public Adaptor3d_HSurface
{

public:

  
  //! Creates an empty GenHSurface.
  Standard_EXPORT Adaptor3d_HSurfaceOfRevolution();
  
  //! Creates a GenHSurface from a Surface.
  Standard_EXPORT Adaptor3d_HSurfaceOfRevolution(const Adaptor3d_SurfaceOfRevolution& S);
  
  //! Sets the field of the GenHSurface.
  Standard_EXPORT void Set (const Adaptor3d_SurfaceOfRevolution& S);
  
  //! Returns a reference to the Surface inside the HSurface.
  //! This is redefined from HSurface, cannot be inline.
  Standard_EXPORT const Adaptor3d_Surface& Surface() const;
  
  //! Returns the surface used to create the GenHSurface.
    Adaptor3d_SurfaceOfRevolution& ChangeSurface();




  DEFINE_STANDARD_RTTI(Adaptor3d_HSurfaceOfRevolution,Adaptor3d_HSurface)

protected:


  Adaptor3d_SurfaceOfRevolution mySurf;


private:




};

#define TheSurface Adaptor3d_SurfaceOfRevolution
#define TheSurface_hxx <Adaptor3d_SurfaceOfRevolution.hxx>
#define Adaptor3d_GenHSurface Adaptor3d_HSurfaceOfRevolution
#define Adaptor3d_GenHSurface_hxx <Adaptor3d_HSurfaceOfRevolution.hxx>
#define Handle_Adaptor3d_GenHSurface Handle(Adaptor3d_HSurfaceOfRevolution)

#include <Adaptor3d_GenHSurface.lxx>

#undef TheSurface
#undef TheSurface_hxx
#undef Adaptor3d_GenHSurface
#undef Adaptor3d_GenHSurface_hxx
#undef Handle_Adaptor3d_GenHSurface




#endif // _Adaptor3d_HSurfaceOfRevolution_HeaderFile
