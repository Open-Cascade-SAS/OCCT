// Created on: 2010-12-03
// Created by: Artem SHAL
// Copyright (c) 2010-2014 OPEN CASCADE SAS
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

#ifndef _Bnd_Sphere_HeaderFile
#define _Bnd_Sphere_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_XYZ.hxx>
#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>


//! This class represents a bounding sphere of a geometric entity
//! (triangle, segment of line or whatever else).
class Bnd_Sphere 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Empty constructor
  Standard_EXPORT Bnd_Sphere();
  
  //! Constructor of a definite sphere
  Standard_EXPORT Bnd_Sphere(const gp_XYZ& theCntr, const Standard_Real theRad, const Standard_Integer theU, const Standard_Integer theV);
  
  //! Returns the U parameter on shape
    Standard_Integer U() const;
  
  //! Returns the V parameter on shape
    Standard_Integer V() const;
  
  //! Returns validity status, indicating that this
  //! sphere corresponds to a real entity
    Standard_Boolean IsValid() const;
  
    void SetValid (const Standard_Boolean isValid);
  
  //! Returns center of sphere object
    const gp_XYZ& Center() const;
  
  //! Returns the radius value
    Standard_Real Radius() const;
  
  //! Calculate and return minimal and maximal distance to sphere.
  //! NOTE: This function is tightly optimized; any modifications
  //! may affect performance!
  Standard_EXPORT void Distances (const gp_XYZ& theXYZ, Standard_Real& theMin, Standard_Real& theMax) const;
  
  //! Calculate and return minimal and maximal distance to sphere.
  //! NOTE: This function is tightly optimized; any modifications
  //! may affect performance!
  Standard_EXPORT void SquareDistances (const gp_XYZ& theXYZ, Standard_Real& theMin, Standard_Real& theMax) const;
  
  //! Projects a point on entity.
  //! Returns true if success
  Standard_EXPORT Standard_Boolean Project (const gp_XYZ& theNode, gp_XYZ& theProjNode, Standard_Real& theDist, Standard_Boolean& theInside) const;
  
  Standard_EXPORT Standard_Real Distance (const gp_XYZ& theNode) const;
  
  Standard_EXPORT Standard_Real SquareDistance (const gp_XYZ& theNode) const;
  
  Standard_EXPORT void Add (const Bnd_Sphere& theOther);
  
  Standard_EXPORT Standard_Boolean IsOut (const Bnd_Sphere& theOther) const;
  
  Standard_EXPORT Standard_Boolean IsOut (const gp_XYZ& thePnt, Standard_Real& theMaxDist) const;
  
  Standard_EXPORT Standard_Real SquareExtent() const;




protected:





private:



  gp_XYZ myCenter;
  Standard_Real myRadius;
  Standard_Boolean myIsValid;
  Standard_Integer myU;
  Standard_Integer myV;


};


#include <Bnd_Sphere.lxx>





#endif // _Bnd_Sphere_HeaderFile
