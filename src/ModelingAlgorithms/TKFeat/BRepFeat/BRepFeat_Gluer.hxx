// Created on: 1996-03-08
// Created by: Jacques GOUSSARD
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _BRepFeat_Gluer_HeaderFile
#define _BRepFeat_Gluer_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <LocOpe_Gluer.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>
#include <LocOpe_Operation.hxx>
#include <TopTools_ListOfShape.hxx>
class TopoDS_Shape;
class TopoDS_Face;
class TopoDS_Edge;

//! One of the most significant aspects
//! of BRepFeat functionality is the use of local operations as opposed
//! to global ones. In a global operation, you would first
//! construct a form of the type you wanted in your final feature, and
//! then remove matter so that it could fit into your initial basis object.
//! In a local operation, however, you specify the domain of the feature
//! construction with aspects of the shape on which the feature is being
//! created. These semantics are expressed in terms of a member
//! shape of the basis shape from which - or up to which - matter will be
//! added or removed. As a result, local operations make calculations
//! simpler and faster than global operations.
//! Glueing uses wires or edges of a face in the basis shape. These are
//! to become a part of the feature. They are first cut out and then
//! projected to a plane outside or inside the basis shape. By
//! rebuilding the initial shape incorporating the edges and the
//! faces of the tool, protrusion features can be constructed.
class BRepFeat_Gluer : public BRepBuilderAPI_MakeShape
{
public:
  DEFINE_STANDARD_ALLOC

  //! Initializes an empty constructor
  BRepFeat_Gluer();

  //! Initializes the shapes to be glued, the new shape
  //! Snew and the basis shape Sbase.
  BRepFeat_Gluer(const TopoDS_Shape& Snew, const TopoDS_Shape& Sbase);

  //! Initializes the new shape Snew and the basis shape
  //! Sbase for the local glueing operation.
  void Init(const TopoDS_Shape& Snew, const TopoDS_Shape& Sbase);

  //! Defines a contact between Fnew on the new shape
  //! Snew and Fbase on the basis shape Sbase. Informs
  //! other methods that Fnew in the new shape Snew is
  //! connected to the face Fbase in the basis shape Sbase.
  //! The contact faces of the glued shape must not have
  //! parts outside the contact faces of the basis shape.
  //! This indicates that glueing is possible.
  void Bind(const TopoDS_Face& Fnew, const TopoDS_Face& Fbase);

  //! nforms other methods that the edge Enew in the new
  //! shape is the same as the edge Ebase in the basis
  //! shape and is therefore attached to the basis shape. This
  //! indicates that glueing is possible.
  void Bind(const TopoDS_Edge& Enew, const TopoDS_Edge& Ebase);

  //! Determine which operation type to use glueing or sliding.
  LocOpe_Operation OpeType() const;

  //! Returns the basis shape of the compound shape.
  const TopoDS_Shape& BasisShape() const;

  //! Returns the resulting compound shape.
  const TopoDS_Shape& GluedShape() const;

  //! This is  called by  Shape().  It does  nothing but
  //! may be redefined.
  Standard_EXPORT virtual void Build(
    const Message_ProgressRange& theRange = Message_ProgressRange()) Standard_OVERRIDE;

  //! returns the status of the Face after
  //! the shape creation.
  Standard_EXPORT virtual Standard_Boolean IsDeleted(const TopoDS_Shape& F) Standard_OVERRIDE;

  //! returns the list of generated Faces.
  Standard_EXPORT virtual const TopTools_ListOfShape& Modified(const TopoDS_Shape& F)
    Standard_OVERRIDE;

protected:
private:
  LocOpe_Gluer myGluer;
};

#include <BRepFeat_Gluer.lxx>

#endif // _BRepFeat_Gluer_HeaderFile
