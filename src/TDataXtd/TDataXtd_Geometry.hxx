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

#ifndef _TDataXtd_Geometry_HeaderFile
#define _TDataXtd_Geometry_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TDataXtd_GeometryEnum.hxx>
#include <TDF_Attribute.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_OStream.hxx>
class TDF_Label;
class TNaming_NamedShape;
class gp_Pnt;
class gp_Ax1;
class gp_Lin;
class gp_Circ;
class gp_Elips;
class gp_Pln;
class gp_Cylinder;
class Standard_GUID;
class TDF_Attribute;
class TDF_RelocationTable;


class TDataXtd_Geometry;
DEFINE_STANDARD_HANDLE(TDataXtd_Geometry, TDF_Attribute)

//! This class is used to model construction geometry.
//! The specific geometric construction of the
//! attribute is defined by an element of the
//! enumeration TDataXtd_GeometryEnum.
//! This attribute may  also be used to qualify  underlying
//! geometry  of   the  associated NamedShape.     for
//! Constructuion element by example.
class TDataXtd_Geometry : public TDF_Attribute
{

public:

  
  //! API class methods
  //! =================
  //! Finds, or  creates, a Geometry attribute  defined by the label label.
  //! The default type of geometry is the value
  //! ANY_GEOM of the enumeration TDataXtd_GeometryEnum.
  //! To specify another value of this enumeration, use
  //! the function SetType.
  Standard_EXPORT static Handle(TDataXtd_Geometry) Set (const TDF_Label& label);
  

  //! Returns the label L used to define the type of
  //! geometric construction for the geometry attribute.
  Standard_EXPORT static TDataXtd_GeometryEnum Type (const TDF_Label& L);
  
  //! Returns the topological attribute S used to define
  //! the type of geometric construction for the geometry attribute.
  Standard_EXPORT static TDataXtd_GeometryEnum Type (const Handle(TNaming_NamedShape)& S);
  

  //! Returns the point attribute defined by the label L and the point G.
  Standard_EXPORT static Standard_Boolean Point (const TDF_Label& L, gp_Pnt& G);
  

  //! Returns the point attribute defined by the topological attribute S and the point G.
  Standard_EXPORT static Standard_Boolean Point (const Handle(TNaming_NamedShape)& S, gp_Pnt& G);
  

  //! Returns the axis attribute defined by the label L and the axis G.
  Standard_EXPORT static Standard_Boolean Axis (const TDF_Label& L, gp_Ax1& G);
  

  //! Returns the axis attribute defined by the topological attribute S and the axis G.
  Standard_EXPORT static Standard_Boolean Axis (const Handle(TNaming_NamedShape)& S, gp_Ax1& G);
  

  //! Returns the line attribute defined by the label L and the line G.
  Standard_EXPORT static Standard_Boolean Line (const TDF_Label& L, gp_Lin& G);
  

  //! Returns the line attribute defined by the topological attribute S and the line G.
  Standard_EXPORT static Standard_Boolean Line (const Handle(TNaming_NamedShape)& S, gp_Lin& G);
  

  //! Returns the circle attribute defined by the label L and the circle G.
  Standard_EXPORT static Standard_Boolean Circle (const TDF_Label& L, gp_Circ& G);
  

  //! Returns the circle attribute defined by the topological attribute S and the circle G.
  Standard_EXPORT static Standard_Boolean Circle (const Handle(TNaming_NamedShape)& S, gp_Circ& G);
  

  //! Returns the ellipse attribute defined by the label L and the ellipse G.
  Standard_EXPORT static Standard_Boolean Ellipse (const TDF_Label& L, gp_Elips& G);
  

  //! Returns the ellipse attribute defined by the
  //! topological attribute S and the ellipse G.
  Standard_EXPORT static Standard_Boolean Ellipse (const Handle(TNaming_NamedShape)& S, gp_Elips& G);
  

  //! Returns the plane attribute defined by the label L and the plane G.
  Standard_EXPORT static Standard_Boolean Plane (const TDF_Label& L, gp_Pln& G);
  

  //! Returns the plane attribute defined by the
  //! topological attribute S and the plane G.
  Standard_EXPORT static Standard_Boolean Plane (const Handle(TNaming_NamedShape)& S, gp_Pln& G);
  

  //! Returns the cylinder attribute defined by the label L and the cylinder G.
  Standard_EXPORT static Standard_Boolean Cylinder (const TDF_Label& L, gp_Cylinder& G);
  

  //! Returns the cylinder attribute defined by the
  //! topological attribute S and the cylinder G.
  Standard_EXPORT static Standard_Boolean Cylinder (const Handle(TNaming_NamedShape)& S, gp_Cylinder& G);
  
  //! Returns the GUID for geometry attributes.
  Standard_EXPORT static const Standard_GUID& GetID();
  
  //! This and the next methods are used to  retrieve  underlying geometry of  the
  //! NamedShape, even   if  noone Geometry   Attribute  is
  //! associated  .  if not  found or not compliant geometry return False.
  Standard_EXPORT TDataXtd_Geometry();
  
  //! Returns the type of geometric construction T of this attribute.
  //! T will be a value of the enumeration TDataXtd_GeometryEnum.
  Standard_EXPORT void SetType (const TDataXtd_GeometryEnum T);
  
  //! Returns the type of geometric construction.
  Standard_EXPORT TDataXtd_GeometryEnum GetType() const;
  
  Standard_EXPORT const Standard_GUID& ID() const Standard_OVERRIDE;
  
  Standard_EXPORT void Restore (const Handle(TDF_Attribute)& with) Standard_OVERRIDE;
  
  Standard_EXPORT Handle(TDF_Attribute) NewEmpty() const Standard_OVERRIDE;
  
  Standard_EXPORT void Paste (const Handle(TDF_Attribute)& into, const Handle(TDF_RelocationTable)& RT) const Standard_OVERRIDE;
  
  Standard_EXPORT virtual Standard_OStream& Dump (Standard_OStream& anOS) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(TDataXtd_Geometry,TDF_Attribute)

protected:




private:


  TDataXtd_GeometryEnum myType;


};







#endif // _TDataXtd_Geometry_HeaderFile
