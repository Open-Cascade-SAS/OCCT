// Created on: 1994-11-16
// Created by: Christian CAILLET
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _IGESConvGeom_GeomBuilder_HeaderFile
#define _IGESConvGeom_GeomBuilder_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColgp_HSequenceOfXYZ.hxx>
#include <gp_Trsf.hxx>
#include <Standard_Integer.hxx>
class gp_XY;
class gp_XYZ;
class IGESGeom_CopiousData;
class gp_Ax3;
class gp_Ax2;
class gp_Ax1;
class IGESGeom_TransformationMatrix;

//! This class provides some useful basic tools to build IGESGeom
//! curves, especially :
//! define a curve in a plane in 3D space (ex. Circular or Conic
//! arc, or Copious Data defined in 2D)
//! make a CopiousData from a list of points/vectors
class IGESConvGeom_GeomBuilder
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a GeomBuilder at initial state.
  Standard_EXPORT IGESConvGeom_GeomBuilder();

  //! Clears list of Points/Vectors and data about Transformation
  Standard_EXPORT void Clear();

  //! Adds a XY (Z=0) to the list of points
  Standard_EXPORT void AddXY(const gp_XY& val);

  //! Adds a XYZ to the list of points
  Standard_EXPORT void AddXYZ(const gp_XYZ& val);

  //! Adds a Vector part to the list of points. It will be used
  //! for CopiousData, datatype=3, only.
  //! AddXY and AddXYZ consider a null vector part (0,0,0)
  //! AddVec adds to the last added XY or XYZ
  Standard_EXPORT void AddVec(const gp_XYZ& val);

  //! Returns the count of already recorded points
  Standard_EXPORT Standard_Integer NbPoints() const;

  //! Returns a point given its rank (if added as XY, Z will be 0)
  Standard_EXPORT gp_XYZ Point(const Standard_Integer num) const;

  //! Makes a CopiousData with the list of recorded Points/Vectors
  //! according to <datatype>, which must be 1,2 or 3
  //! If <polyline> is given True, the CopiousData is coded as a
  //! Polyline, but <datatype> must not be 3
  //! <datatype> = 1 : Common Z is computed as average of all Z
  //! <datatype> = 1 or 2 : Vectors are ignored
  Standard_EXPORT Handle(IGESGeom_CopiousData) MakeCopiousData(
    const Standard_Integer datatype,
    const Standard_Boolean polyline = Standard_False) const;

  //! Returns the Position in which the method EvalXYZ will
  //! evaluate a XYZ. It can be regarded as defining a local system.
  //! It is initially set to Identity
  Standard_EXPORT gp_Trsf Position() const;

  //! Sets final position from an already defined Trsf
  Standard_EXPORT void SetPosition(const gp_Trsf& pos);

  //! Sets final position from an Ax3
  Standard_EXPORT void SetPosition(const gp_Ax3& pos);

  //! Sets final position from an Ax2
  Standard_EXPORT void SetPosition(const gp_Ax2& pos);

  //! Sets final position from an Ax1
  //! (this means that origin point and Z-axis are defined, the
  //! other axes are defined arbitrarily)
  Standard_EXPORT void SetPosition(const gp_Ax1& pos);

  //! Returns True if the Position is Identity
  Standard_EXPORT Standard_Boolean IsIdentity() const;

  //! Returns True if the Position is a Translation only
  //! Remark : Identity and ZOnly will answer True
  Standard_EXPORT Standard_Boolean IsTranslation() const;

  //! Returns True if the Position corresponds to a Z-Displacement,
  //! i.e. is a Translation only, and only on Z
  //! Remark : Identity will answer True
  Standard_EXPORT Standard_Boolean IsZOnly() const;

  //! Evaluates a XYZ value in the Position already defined.
  //! Returns the transformed coordinates.
  //! For a 2D definition, X,Y will then be used to define a XY and
  //! Z will be regarded as a Z Displacement (can be ignored)
  Standard_EXPORT void EvalXYZ(const gp_XYZ&  val,
                               Standard_Real& X,
                               Standard_Real& Y,
                               Standard_Real& Z) const;

  //! Returns the IGES Transformation which corresponds to the
  //! Position. Even if it is an Identity : IsIdentity should be
  //! tested first.
  //! <unit> is the unit value in which the model is created :
  //! it is used to convert translation part
  Standard_EXPORT Handle(IGESGeom_TransformationMatrix) MakeTransformation(
    const Standard_Real unit = 1) const;

protected:
private:
  Handle(TColgp_HSequenceOfXYZ) theXYZ;
  Handle(TColgp_HSequenceOfXYZ) theVec;
  gp_Trsf                       thepos;
};

#endif // _IGESConvGeom_GeomBuilder_HeaderFile
