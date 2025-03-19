// Created on: 2009-04-06
// Created by: Sergey ZARITCHNY
// Copyright (c) 2009-2014 OPEN CASCADE SAS
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

#ifndef _TDataXtd_Constraint_HeaderFile
#define _TDataXtd_Constraint_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TDataXtd_ConstraintEnum.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_Integer.hxx>
#include <TDF_LabelList.hxx>
#include <Standard_OStream.hxx>

class TDataStd_Real;
class Standard_GUID;
class TDF_Label;
class TNaming_NamedShape;
class TDF_RelocationTable;
class TDF_DataSet;

class TDataXtd_Constraint;
DEFINE_STANDARD_HANDLE(TDataXtd_Constraint, TDF_Attribute)

//! The groundwork to define constraint attributes.
//! The constraint attribute contains the following sorts of data:
//! -   Type whether the constraint attribute is a
//! geometric constraint or a dimension
//! -   Value the real number value of a numeric
//! constraint such as an angle or a radius
//! -   Geometries to identify the geometries
//! underlying the topological attributes which
//! define the constraint (up to 4)
//! -   Plane for 2D constraints.
class TDataXtd_Constraint : public TDF_Attribute
{

public:
  //! Returns the GUID for constraints.
  Standard_EXPORT static const Standard_GUID& GetID();

  //! Finds or creates the 2D constraint attribute
  //! defined by the planar topological attribute plane
  //! and the label label.
  //! Constraint methods
  //! ==================
  Standard_EXPORT static Handle(TDataXtd_Constraint) Set(const TDF_Label& label);

  Standard_EXPORT TDataXtd_Constraint();

  //! Finds or creates the constraint attribute defined
  //! by the topological attribute G1 and the constraint type type.
  Standard_EXPORT void Set(const TDataXtd_ConstraintEnum     type,
                           const Handle(TNaming_NamedShape)& G1);

  //! Finds or creates the constraint attribute defined
  //! by the topological attributes G1 and G2, and by
  //! the constraint type type.
  Standard_EXPORT void Set(const TDataXtd_ConstraintEnum     type,
                           const Handle(TNaming_NamedShape)& G1,
                           const Handle(TNaming_NamedShape)& G2);

  //! Finds or creates the constraint attribute defined
  //! by the topological attributes G1, G2 and G3, and
  //! by the constraint type type.
  Standard_EXPORT void Set(const TDataXtd_ConstraintEnum     type,
                           const Handle(TNaming_NamedShape)& G1,
                           const Handle(TNaming_NamedShape)& G2,
                           const Handle(TNaming_NamedShape)& G3);

  //! Finds or creates the constraint attribute defined
  //! by the topological attributes G1, G2, G3 and G4,
  //! and by the constraint type type.
  //! methods to read constraint fields
  //! =================================
  Standard_EXPORT void Set(const TDataXtd_ConstraintEnum     type,
                           const Handle(TNaming_NamedShape)& G1,
                           const Handle(TNaming_NamedShape)& G2,
                           const Handle(TNaming_NamedShape)& G3,
                           const Handle(TNaming_NamedShape)& G4);

  //! Returns true if this constraint attribute is valid.
  //! By default, true is returned.
  //! When the value of a dimension is changed or
  //! when a geometry is moved, false is returned
  //! until the solver sets it back to true.
  Standard_EXPORT Standard_Boolean Verified() const;

  //! Returns the type of constraint.
  //! This will be an element of the
  //! TDataXtd_ConstraintEnum enumeration.
  Standard_EXPORT TDataXtd_ConstraintEnum GetType() const;

  //! Returns true if this constraint attribute is
  //! two-dimensional.
  Standard_EXPORT Standard_Boolean IsPlanar() const;

  //! Returns the topological attribute of the plane
  //! used for planar - i.e., 2D - constraints.
  //! This plane is attached to another label.
  //! If the constraint is not planar, in other words, 3D,
  //! this function will return a null handle.
  Standard_EXPORT const Handle(TNaming_NamedShape)& GetPlane() const;

  //! Returns true if this constraint attribute is a
  //! dimension, and therefore has a value.
  Standard_EXPORT Standard_Boolean IsDimension() const;

  //! Returns the value of a dimension.
  //! This value is a reference to a TDataStd_Real attribute.
  //! If the attribute is not a dimension, this value will
  //! be 0. Use IsDimension to test this condition.
  Standard_EXPORT const Handle(TDataStd_Real)& GetValue() const;

  //! Returns the number of geometry attributes in this constraint attribute.
  //! This number will be between 1 and 4.
  Standard_EXPORT Standard_Integer NbGeometries() const;

  //! Returns the integer index Index used to access
  //! the array of the constraint or stored geometries of a dimension
  //! Index has a value between 1 and 4.
  //! methods to write constraint fields (use builder)
  //! ==================================
  Standard_EXPORT Handle(TNaming_NamedShape) GetGeometry(const Standard_Integer Index) const;

  //! Removes the geometries involved in the
  //! constraint or dimension from the array of
  //! topological attributes where they are stored.
  Standard_EXPORT void ClearGeometries();

  //! Finds or creates the type of constraint CTR.
  Standard_EXPORT void SetType(const TDataXtd_ConstraintEnum CTR);

  //! Finds or creates the plane of the 2D constraint
  //! attribute, defined by the planar topological attribute plane.
  Standard_EXPORT void SetPlane(const Handle(TNaming_NamedShape)& plane);

  //! Finds or creates the real number value V of the dimension constraint attribute.
  Standard_EXPORT void SetValue(const Handle(TDataStd_Real)& V);

  //! Finds or creates the underlying geometry of the
  //! constraint defined by the topological attribute G
  //! and the integer index Index.
  Standard_EXPORT void SetGeometry(const Standard_Integer            Index,
                                   const Handle(TNaming_NamedShape)& G);

  //! Returns true if this constraint attribute defined by status is valid.
  //! By default, true is returned.
  //! When the value of a dimension is changed or
  //! when a geometry is moved, false is returned until
  //! the solver sets it back to true.
  //! If status is false, Verified is set to false.
  Standard_EXPORT void Verified(const Standard_Boolean status);

  Standard_EXPORT void Inverted(const Standard_Boolean status);

  Standard_EXPORT Standard_Boolean Inverted() const;

  Standard_EXPORT void Reversed(const Standard_Boolean status);

  Standard_EXPORT Standard_Boolean Reversed() const;

  //! collects constraints on Childs for label <aLabel>
  Standard_EXPORT static void CollectChildConstraints(const TDF_Label& aLabel,
                                                      TDF_LabelList&   TheList);

  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;

  Standard_EXPORT void Restore(const Handle(TDF_Attribute)& With) Standard_OVERRIDE;

  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;

  Standard_EXPORT void Paste(const Handle(TDF_Attribute)&       Into,
                             const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;

  Standard_EXPORT virtual Standard_OStream& Dump(Standard_OStream& anOS) const Standard_OVERRIDE;

  Standard_EXPORT virtual void References(const Handle(TDF_DataSet)& DS) const Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(TDataXtd_Constraint, TDF_Attribute)

protected:
private:
  TDataXtd_ConstraintEnum    myType;
  Handle(TDataStd_Real)      myValue;
  Handle(TDF_Attribute)      myGeometries[4];
  Handle(TNaming_NamedShape) myPlane;
  Standard_Boolean           myIsReversed;
  Standard_Boolean           myIsInverted;
  Standard_Boolean           myIsVerified;
};

#endif // _TDataXtd_Constraint_HeaderFile
