// Created on: 1992-05-06
// Created by: Jacques GOUSSARD
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

#ifndef _IntSurf_PntOn2S_HeaderFile
#define _IntSurf_PntOn2S_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Pnt.hxx>
#include <Standard_Boolean.hxx>
class gp_Pnt2d;


//! This class defines the geometric information
//! for an intersection point between 2 surfaces :
//! The coordinates ( Pnt from gp ), and two
//! parametric coordinates.
class IntSurf_PntOn2S 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor.
  Standard_EXPORT IntSurf_PntOn2S();
  
  //! Sets the value of the point in 3d space.
    void SetValue (const gp_Pnt& Pt);
  
  //! Sets the values of the point in 3d space, and
  //! in the parametric space of one of the surface.
  Standard_EXPORT void SetValue (const gp_Pnt& Pt, const Standard_Boolean OnFirst, const Standard_Real U, const Standard_Real V);
  
  //! Sets the values of the point in 3d space, and
  //! in the parametric space of each surface.
    void SetValue (const gp_Pnt& Pt, const Standard_Real U1, const Standard_Real V1, const Standard_Real U2, const Standard_Real V2);
  
  //! Set the values of the point in the parametric
  //! space of one of the surface.
  Standard_EXPORT void SetValue (const Standard_Boolean OnFirst, const Standard_Real U, const Standard_Real V);
  
  //! Set the values of the point in the parametric
  //! space of one of the surface.
      void SetValue (const Standard_Real U1, const Standard_Real V1, const Standard_Real U2, const Standard_Real V2) ;
  
  //! Returns the point in 3d space.
     const  gp_Pnt& Value()  const;
  
  //! Returns the point in 2d space of one of the surfaces.
  Standard_EXPORT   gp_Pnt2d ValueOnSurface (const Standard_Boolean OnFirst)  const;
  
  //! Returns the parameters of the point on the first surface.
    void ParametersOnS1 (Standard_Real& U1, Standard_Real& V1) const;
  
  //! Returns the parameters of the point on the second surface.
      void ParametersOnS2 (Standard_Real& U2, Standard_Real& V2)  const;
  
  //! Returns the parameters of the point in the
  //! parametric space of one of the surface.
  Standard_EXPORT   void ParametersOnSurface (const Standard_Boolean OnFirst, Standard_Real& U, Standard_Real& V)  const;
  
  //! Returns the parameters of the point on both surfaces.
      void Parameters (Standard_Real& U1, Standard_Real& V1, Standard_Real& U2, Standard_Real& V2)  const;
  
  //! Returns TRUE if 2D- and 3D-coordinates of theOterPoint are equal to
  //! corresponding coordinates of me (with given tolerance).
  //! If theTol2D < 0.0 we will compare 3D-points only.
  Standard_EXPORT Standard_Boolean IsSame (const IntSurf_PntOn2S& theOtherPoint, const Standard_Real theTol3D = 0.0, const Standard_Real theTol2D = -1.0) const;




protected:





private:



  gp_Pnt pt;
  Standard_Real u1;
  Standard_Real v1;
  Standard_Real u2;
  Standard_Real v2;


};


#include <IntSurf_PntOn2S.lxx>





#endif // _IntSurf_PntOn2S_HeaderFile
