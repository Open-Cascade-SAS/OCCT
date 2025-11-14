// Copyright (c) 1995-1999 Matra Datavision
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

// JCV 30/08/90 Modif passage version C++ 2.0 sur Sun
// JCV 1/10/90 Changement de nom du package vgeom -> gp
// JCV 07/12/90 Modifs suite a l'introduction des classes XYZ et Mat dans gp

#define No_Standard_OutOfRange

#include <gp_Vec.hxx>

#include <gp.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Trsf.hxx>
#include <gp_XYZ.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Dump.hxx>
#include <Standard_OutOfRange.hxx>

Standard_Boolean gp_Vec::IsEqual(const gp_Vec&       theOther,
                                 const Standard_Real theLinearTolerance,
                                 const Standard_Real theAngularTolerance) const
{
  const Standard_Real aMagnitude       = Magnitude();
  const Standard_Real anOtherMagnitude = theOther.Magnitude();

  if (aMagnitude <= theLinearTolerance || anOtherMagnitude <= theLinearTolerance)
  {
    const Standard_Real aVal = std::abs(aMagnitude - anOtherMagnitude);
    return aVal <= theLinearTolerance;
  }
  else
  {
    const Standard_Real aVal = std::abs(aMagnitude - anOtherMagnitude);
    return aVal <= theLinearTolerance && Angle(theOther) <= theAngularTolerance;
  }
}

void gp_Vec::Mirror(const gp_Vec& theVec) noexcept
{
  const Standard_Real aMagnitude = theVec.coord.Modulus();
  if (aMagnitude > gp::Resolution())
  {
    const gp_XYZ&       aMirrorVecXYZ = theVec.coord;
    const Standard_Real aOrigX        = coord.X();
    const Standard_Real aOrigY        = coord.Y();
    const Standard_Real aOrigZ        = coord.Z();

    // Normalize the mirror vector components
    const Standard_Real aNormDirX = aMirrorVecXYZ.X() / aMagnitude;
    const Standard_Real aNormDirY = aMirrorVecXYZ.Y() / aMagnitude;
    const Standard_Real aNormDirZ = aMirrorVecXYZ.Z() / aMagnitude;

    // Precompute common terms for 3D reflection matrix
    const Standard_Real aCrossTermXY = 2.0 * aNormDirX * aNormDirY;
    const Standard_Real aCrossTermXZ = 2.0 * aNormDirX * aNormDirZ;
    const Standard_Real aCrossTermYZ = 2.0 * aNormDirY * aNormDirZ;
    const Standard_Real aXXTerm      = 2.0 * aNormDirX * aNormDirX - 1.0;
    const Standard_Real aYYTerm      = 2.0 * aNormDirY * aNormDirY - 1.0;
    const Standard_Real aZZTerm      = 2.0 * aNormDirZ * aNormDirZ - 1.0;

    coord.SetX(aXXTerm * aOrigX + aCrossTermXY * aOrigY + aCrossTermXZ * aOrigZ);
    coord.SetY(aCrossTermXY * aOrigX + aYYTerm * aOrigY + aCrossTermYZ * aOrigZ);
    coord.SetZ(aCrossTermXZ * aOrigX + aCrossTermYZ * aOrigY + aZZTerm * aOrigZ);
  }
}

void gp_Vec::Mirror(const gp_Ax1& theAxis) noexcept
{
  const gp_XYZ&       aDirectionXYZ = theAxis.Direction().XYZ();
  const Standard_Real aOrigX        = coord.X();
  const Standard_Real aOrigY        = coord.Y();
  const Standard_Real aOrigZ        = coord.Z();
  const Standard_Real aDirX         = aDirectionXYZ.X();
  const Standard_Real aDirY         = aDirectionXYZ.Y();
  const Standard_Real aDirZ         = aDirectionXYZ.Z();

  // Precompute common terms for 3D reflection matrix
  const Standard_Real aCrossTermXY = 2.0 * aDirX * aDirY;
  const Standard_Real aCrossTermXZ = 2.0 * aDirX * aDirZ;
  const Standard_Real aCrossTermYZ = 2.0 * aDirY * aDirZ;
  const Standard_Real aXXTerm      = 2.0 * aDirX * aDirX - 1.0;
  const Standard_Real aYYTerm      = 2.0 * aDirY * aDirY - 1.0;
  const Standard_Real aZZTerm      = 2.0 * aDirZ * aDirZ - 1.0;

  coord.SetX(aXXTerm * aOrigX + aCrossTermXY * aOrigY + aCrossTermXZ * aOrigZ);
  coord.SetY(aCrossTermXY * aOrigX + aYYTerm * aOrigY + aCrossTermYZ * aOrigZ);
  coord.SetZ(aCrossTermXZ * aOrigX + aCrossTermYZ * aOrigY + aZZTerm * aOrigZ);
}

void gp_Vec::Mirror(const gp_Ax2& theAxis) noexcept
{
  const gp_XYZ& aZDir   = theAxis.Direction().XYZ();
  const gp_XYZ  aMirXYZ = aZDir.Crossed(coord);

  if (aMirXYZ.Modulus() <= gp::Resolution())
  {
    coord.Reverse();
  }
  else
  {
    gp_XYZ aNewZ = aZDir;
    aNewZ.Cross(aMirXYZ);
    Mirror(gp_Vec(aNewZ));
  }
}

void gp_Vec::Transform(const gp_Trsf& theTransformation)
{
  if (theTransformation.Form() == gp_Identity || theTransformation.Form() == gp_Translation)
  {
  }
  else if (theTransformation.Form() == gp_PntMirror)
  {
    coord.Reverse();
  }
  else if (theTransformation.Form() == gp_Scale)
  {
    coord.Multiply(theTransformation.ScaleFactor());
  }
  else
  {
    coord.Multiply(theTransformation.VectorialPart());
  }
}

gp_Vec gp_Vec::Mirrored(const gp_Vec& theVec) const noexcept
{
  gp_Vec aResult = *this;
  aResult.Mirror(theVec);
  return aResult;
}

gp_Vec gp_Vec::Mirrored(const gp_Ax1& theAxis) const noexcept
{
  gp_Vec aResult = *this;
  aResult.Mirror(theAxis);
  return aResult;
}

gp_Vec gp_Vec::Mirrored(const gp_Ax2& theAxis) const noexcept
{
  gp_Vec aResult = *this;
  aResult.Mirror(theAxis);
  return aResult;
}

//=================================================================================================

void gp_Vec::DumpJson(Standard_OStream& theOStream, Standard_Integer) const
{
  OCCT_DUMP_VECTOR_CLASS(theOStream, "gp_Vec", 3, coord.X(), coord.Y(), coord.Z())
}
