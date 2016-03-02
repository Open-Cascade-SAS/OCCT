// Copyright (c) 2015 OPEN CASCADE SAS
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


#ifndef _StdObject_gp_HeaderFile
#define _StdObject_gp_HeaderFile

#include <StdObjMgt_ContentTypes.hxx>

class StdObject_gp : private StdObjMgt_ContentTypes
{
  template <class Data>
  struct object : Data
    { Standard_EXPORT void Read (StdObjMgt_ReadData& theReadData); };

public:
  template <class Data>
  struct Object : StdObjMgt_ContentTypes::Object <object<Data> > {};

  template <class Data>
  static Object<Data>& Ref (Data& theData)
    { return static_cast<Object<Data>&> (theData); }
};

// read vectors

#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>

inline StdObjMgt_ReadData& operator >> (StdObjMgt_ReadData& theReadData, gp_XY& theXY)
{
  Standard_Real aX, aY;
  theReadData.ReadValue(aX);
  theReadData.ReadValue(aY);
  theXY.SetCoord(aX, aY);
  return theReadData;
}

inline StdObjMgt_ReadData& operator >> (StdObjMgt_ReadData& theReadData, gp_Pnt2d& thePnt)
{
  Standard_Real aX, aY;
  theReadData.ReadValue(aX);
  theReadData.ReadValue(aY);
  thePnt.SetCoord(aX, aY);
  return theReadData;
}

inline StdObjMgt_ReadData& operator >> (StdObjMgt_ReadData& theReadData, gp_Vec2d& theVec)
{
  Standard_Real aX, aY;
  theReadData.ReadValue(aX);
  theReadData.ReadValue(aY);
  theVec.SetCoord(aX, aY);
  return theReadData;
}

inline StdObjMgt_ReadData& operator >> (StdObjMgt_ReadData& theReadData, gp_Dir2d& theDir)
{
  Standard_Real aX, aY;
  theReadData.ReadValue(aX);
  theReadData.ReadValue(aY);
  theDir.SetCoord(aX, aY);
  return theReadData;
}

inline StdObjMgt_ReadData& operator >> (StdObjMgt_ReadData& theReadData, gp_XYZ& theXYZ)
{
  Standard_Real aX, aY, aZ;
  theReadData.ReadValue(aX);
  theReadData.ReadValue(aY);
  theReadData.ReadValue(aZ);
  theXYZ.SetCoord(aX, aY, aZ);
  return theReadData;
}

inline StdObjMgt_ReadData& operator >> (StdObjMgt_ReadData& theReadData, gp_Pnt& thePnt)
{
  Standard_Real aX, aY, aZ;
  theReadData.ReadValue(aX);
  theReadData.ReadValue(aY);
  theReadData.ReadValue(aZ);
  thePnt.SetCoord(aX, aY, aZ);
  return theReadData;
}

inline StdObjMgt_ReadData& operator >> (StdObjMgt_ReadData& theReadData, gp_Vec& theVec)
{
  Standard_Real aX, aY, aZ;
  theReadData.ReadValue(aX);
  theReadData.ReadValue(aY);
  theReadData.ReadValue(aZ);
  theVec.SetCoord(aX, aY, aZ);
  return theReadData;
}

inline StdObjMgt_ReadData& operator >> (StdObjMgt_ReadData& theReadData, gp_Dir& theDir)
{
  Standard_Real aX, aY, aZ;
  theReadData.ReadValue(aX);
  theReadData.ReadValue(aY);
  theReadData.ReadValue(aZ);
  theDir.SetCoord(aX, aY, aZ);
  return theReadData;
}

// read axis placements

#include <gp_Ax2d.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>

inline StdObjMgt_ReadData& operator >> (StdObjMgt_ReadData& theReadData, gp_Ax2d& theAx)
{
  gp_Pnt2d aLoc;
  gp_Dir2d aDir;
  theReadData >> aLoc >> aDir;
  theAx = gp_Ax2d (aLoc, aDir);
  return theReadData;
}

inline StdObjMgt_ReadData& operator >> (StdObjMgt_ReadData& theReadData, gp_Ax22d& theAx)
{
  gp_Pnt2d aLoc;
  gp_Dir2d aYDir, aXDir;
  theReadData >> aLoc >> aYDir >> aXDir;
  theAx = gp_Ax22d (aLoc, aXDir, aYDir);
  return theReadData;
}

inline StdObjMgt_ReadData& operator >> (StdObjMgt_ReadData& theReadData, gp_Ax1& theAx)
{
  gp_XYZ aLoc;
  gp_Dir aDir;
  theReadData >> aLoc >> aDir;
  theAx = gp_Ax1 (aLoc, aDir);
  return theReadData;
}

inline StdObjMgt_ReadData& operator >> (StdObjMgt_ReadData& theReadData, gp_Ax2& theAx)
{
  gp_Ax1 anAx;
  gp_Dir aYDir, aXDir;
  theReadData >> anAx >> aYDir >> aXDir;
  theAx = gp_Ax2 (anAx.Location(), anAx.Direction(), aXDir);
  return theReadData;
}

inline StdObjMgt_ReadData& operator >> (StdObjMgt_ReadData& theReadData, gp_Ax3& theAx)
{
  gp_Ax1 anAx;
  gp_Dir aYDir, aXDir;
  theReadData >> anAx >> aYDir >> aXDir;
  theAx = gp_Ax3 (anAx.Location(), anAx.Direction(), aXDir);
  if (aYDir * theAx.YDirection() < 0.)
    theAx.YReverse();
  return theReadData;
}

// read curves

#include <gp_Lin2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Lin.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Parab.hxx>

inline StdObjMgt_ReadData& operator >> (StdObjMgt_ReadData& theReadData, gp_Lin2d& theLin)
{
  gp_Ax2d anAx;
  theReadData >> anAx;
  theLin.SetPosition(anAx);
  return theReadData;
}

inline StdObjMgt_ReadData& operator >> (StdObjMgt_ReadData& theReadData, gp_Circ2d& theCirc)
{
  gp_Ax22d anAx;
  Standard_Real aRadius;
  theReadData >> anAx;
  theReadData.ReadValue(aRadius);
  theCirc.SetAxis(anAx);
  theCirc.SetRadius (aRadius);
  return theReadData;
}

inline StdObjMgt_ReadData& operator >> (StdObjMgt_ReadData& theReadData, gp_Elips2d& theElips)
{
  gp_Ax22d anAx;
  Standard_Real aMajorRadius, aMinorRadius;
  theReadData >> anAx;
  theReadData.ReadValue(aMajorRadius);
  theReadData.ReadValue(aMinorRadius);
  theElips.SetAxis(anAx);
  theElips.SetMajorRadius(aMajorRadius);
  theElips.SetMinorRadius(aMinorRadius);
  return theReadData;
}

inline StdObjMgt_ReadData& operator >> (StdObjMgt_ReadData& theReadData, gp_Hypr2d& theHypr)
{
  gp_Ax22d anAx;
  Standard_Real aMajorRadius, aMinorRadius;
  theReadData >> anAx;
  theReadData.ReadValue(aMajorRadius);
  theReadData.ReadValue(aMinorRadius);
  theHypr.SetAxis(anAx);
  theHypr.SetMajorRadius(aMajorRadius);
  theHypr.SetMinorRadius(aMinorRadius);
  return theReadData;
}

inline StdObjMgt_ReadData& operator >> (StdObjMgt_ReadData& theReadData, gp_Parab2d& theParab)
{
  gp_Ax22d anAx;
  Standard_Real aFocalLength;
  theReadData >> anAx;
  theReadData.ReadValue(aFocalLength);
  theParab.SetAxis(anAx);
  theParab.SetFocal(aFocalLength);
  return theReadData;
}

inline StdObjMgt_ReadData& operator >> (StdObjMgt_ReadData& theReadData, gp_Lin& theLin)
{
  gp_Ax1 anAx;
  theReadData >> anAx;
  theLin.SetPosition(anAx);
  return theReadData;
}

inline StdObjMgt_ReadData& operator >> (StdObjMgt_ReadData& theReadData, gp_Circ& theCirc)
{
  gp_Ax2 anAx;
  Standard_Real aRadius;
  theReadData >> anAx;
  theReadData.ReadValue(aRadius);
  theCirc.SetPosition(anAx);
  theCirc.SetRadius (aRadius);
  return theReadData;
}

inline StdObjMgt_ReadData& operator >> (StdObjMgt_ReadData& theReadData, gp_Elips& theElips)
{
  gp_Ax2 anAx;
  Standard_Real aMajorRadius, aMinorRadius;
  theReadData >> anAx;
  theReadData.ReadValue(aMajorRadius);
  theReadData.ReadValue(aMinorRadius);
  theElips.SetPosition(anAx);
  theElips.SetMajorRadius(aMajorRadius);
  theElips.SetMinorRadius(aMinorRadius);
  return theReadData;
}

inline StdObjMgt_ReadData& operator >> (StdObjMgt_ReadData& theReadData, gp_Hypr& theHypr)
{
  gp_Ax2 anAx;
  Standard_Real aMajorRadius, aMinorRadius;
  theReadData >> anAx;
  theReadData.ReadValue(aMajorRadius);
  theReadData.ReadValue(aMinorRadius);
  theHypr.SetPosition(anAx);
  theHypr.SetMajorRadius(aMajorRadius);
  theHypr.SetMinorRadius(aMinorRadius);
  return theReadData;
}

inline StdObjMgt_ReadData& operator >> (StdObjMgt_ReadData& theReadData, gp_Parab& theParab)
{
  gp_Ax2 anAx;
  Standard_Real aFocalLength;
  theReadData >> anAx;
  theReadData.ReadValue(aFocalLength);
  theParab.SetPosition(anAx);
  theParab.SetFocal(aFocalLength);
  return theReadData;
}

// read surfaces

#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>

inline StdObjMgt_ReadData& operator >> (StdObjMgt_ReadData& theReadData, gp_Cone& theCone)
{
  gp_Ax3 anAx;
  Standard_Real aRadius, aSemiAngle;
  theReadData >> anAx;
  theReadData.ReadValue(aRadius);
  theReadData.ReadValue(aSemiAngle);
  theCone.SetPosition(anAx);
  theCone.SetRadius(aRadius);
  theCone.SetSemiAngle(aSemiAngle);
  return theReadData;
}

inline StdObjMgt_ReadData& operator >> (StdObjMgt_ReadData& theReadData, gp_Cylinder& theCyl)
{
  gp_Ax3 anAx;
  Standard_Real aRadius;
  theReadData >> anAx;
  theReadData.ReadValue(aRadius);
  theCyl.SetPosition(anAx);
  theCyl.SetRadius(aRadius);
  return theReadData;
}

inline StdObjMgt_ReadData& operator >> (StdObjMgt_ReadData& theReadData, gp_Sphere& theSph)
{
  gp_Ax3 anAx;
  Standard_Real aRadius;
  theReadData >> anAx;
  theReadData.ReadValue(aRadius);
  theSph.SetPosition(anAx);
  theSph.SetRadius(aRadius);
  return theReadData;
}

inline StdObjMgt_ReadData& operator >> (StdObjMgt_ReadData& theReadData, gp_Torus& theTorus)
{
  gp_Ax3 anAx;
  Standard_Real aMajorRadius, aMinorRadius;
  theReadData >> anAx;
  theReadData.ReadValue(aMajorRadius);
  theReadData.ReadValue(aMinorRadius);
  theTorus.SetPosition(anAx);
  theTorus.SetMajorRadius(aMajorRadius);
  theTorus.SetMinorRadius(aMinorRadius);
  return theReadData;
}

// read transformations

#include <gp_Mat2d.hxx>
#include <gp_Mat.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Trsf.hxx>

inline StdObjMgt_ReadData& operator >> (StdObjMgt_ReadData& theReadData, gp_Mat2d& theMat)
{
  gp_XY aRow1, aRow2;
  theReadData >> aRow1 >> aRow2;
  theMat.SetRows(aRow1, aRow2);
  return theReadData;
}

inline StdObjMgt_ReadData& operator >> (StdObjMgt_ReadData& theReadData, gp_Mat& theMat)
{
  gp_XYZ aRow1, aRow2, aRow3;
  theReadData >> aRow1 >> aRow2 >> aRow3;
  theMat.SetRows(aRow1, aRow2, aRow3);
  return theReadData;
}

inline StdObjMgt_ReadData& operator >> (StdObjMgt_ReadData& theReadData, gp_Trsf2d& theTrsf)
{
  Standard_Real aScale;
  gp_TrsfForm aForm;
  gp_Mat2d aMat;
  gp_XY aLoc;
  theReadData.ReadValue(aScale);
  theReadData.ReadEnum(aForm);
  theReadData >> aMat >> aLoc;
  theTrsf.SetValues(aScale * aMat(1, 1), aScale * aMat(1, 2), aLoc.X(),
                    aScale * aMat(2, 1), aScale * aMat(2, 2), aLoc.Y());
  return theReadData;
}

inline StdObjMgt_ReadData& operator >> (StdObjMgt_ReadData& theReadData, gp_Trsf& theTrsf)
{
  Standard_Real aScale;
  gp_TrsfForm aForm;
  gp_Mat aMat;
  gp_XYZ aLoc;
  theReadData.ReadValue(aScale);
  theReadData.ReadEnum(aForm);
  theReadData >> aMat >> aLoc;
  theTrsf.SetValues(aScale * aMat(1, 1), aScale * aMat(1, 2), aScale * aMat(1, 3), aLoc.X(),
                    aScale * aMat(2, 1), aScale * aMat(2, 2), aScale * aMat(2, 3), aLoc.Y(),
                    aScale * aMat(3, 1), aScale * aMat(3, 2), aScale * aMat(3, 3), aLoc.Z());
  return theReadData;
}

template<class T>
inline void StdObject_gp::object<T>::Read (StdObjMgt_ReadData& theReadData)
{
  theReadData >> (*this);
}


#endif
