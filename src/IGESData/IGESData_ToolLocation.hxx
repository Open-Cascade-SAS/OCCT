// Created on: 1993-09-21
// Created by: Christian CAILLET
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _IGESData_ToolLocation_HeaderFile
#define _IGESData_ToolLocation_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <Interface_GeneralLib.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Boolean.hxx>
class IGESData_IGESModel;
class Standard_DomainError;
class IGESData_Protocol;
class IGESData_IGESEntity;
class gp_GTrsf;
class gp_Trsf;


class IGESData_ToolLocation;
DEFINE_STANDARD_HANDLE(IGESData_ToolLocation, Standard_Transient)

//! This Tool determines and gives access to effective Locations
//! of IGES Entities as defined by the IGES Norm. These Locations
//! can be for each Entity :
//! - on one part, explicitly defined by a Transf in Directory
//! Part (this Transf can be itself compound); if not defined,
//! no proper Transformation is defined
//! - on the other part, implicitly defined by a reference from
//! another Entity : its Parent
//! Both implicit and explicit locations are combinable.
//!
//! Implicit definition can be itself defined, either through the
//! definition of an Entity (i.e. a Composite Curve references
//! a list of Curves), or by a specific Associativity, of type
//! SingleParentEntity, by which the Location of the Parent is
//! applied to the Childs defined by this Associativity.
//! Remark that a Transf itself has no Location, but it can be
//! compound
//!
//! This is a TShared object, then it is easier to use in an
//! interactive session
class IGESData_ToolLocation : public Standard_Transient
{

public:

  
  //! Creates a ToolLocation on a given Model, filled with the help
  //! of a Protocol (which allows to known Entities referenced by
  //! other ones)
  Standard_EXPORT IGESData_ToolLocation(const Handle(IGESData_IGESModel)& amodel, const Handle(IGESData_Protocol)& protocol);
  
  //! Does the effective work of determining Locations of Entities
  Standard_EXPORT void Load();
  
  //! Sets a precision for the Analysis of Locations
  //! (default by constructor is 1.E-05)
  Standard_EXPORT void SetPrecision (const Standard_Real prec);
  
  //! Sets the "Reference" information for <child> as being <parent>
  //! Sets an Error Status if already set (see method IsAmbiguous)
  Standard_EXPORT void SetReference (const Handle(IGESData_IGESEntity)& parent, const Handle(IGESData_IGESEntity)& child);
  
  //! Sets the "Associativity" information for <child> as being
  //! <parent> (it must be the Parent itself, not the Associativity)
  Standard_EXPORT void SetParentAssoc (const Handle(IGESData_IGESEntity)& parent, const Handle(IGESData_IGESEntity)& child);
  
  //! Resets all informations about dependences for <child>
  Standard_EXPORT void ResetDependences (const Handle(IGESData_IGESEntity)& child);
  
  //! Unitary action which defines Entities referenced by <ent>
  //! (except those in Directory Part and Associativities List)
  //! as Dependent (their Locations are related to that of <ent>)
  Standard_EXPORT void SetOwnAsDependent (const Handle(IGESData_IGESEntity)& ent);
  
  //! Returns True if <ent> is kind of TransfEntity. Then, it has
  //! no location, while it can be used to define a Location)
  Standard_EXPORT Standard_Boolean IsTransf (const Handle(IGESData_IGESEntity)& ent) const;
  
  //! Returns True if <ent> is an Associativity (IGES Type 402).
  //! Then, Location does not apply.
  Standard_EXPORT Standard_Boolean IsAssociativity (const Handle(IGESData_IGESEntity)& ent) const;
  
  //! Returns True if <ent> has a Transformation Matrix in proper
  //! (referenced from its Directory Part)
  Standard_EXPORT Standard_Boolean HasTransf (const Handle(IGESData_IGESEntity)& ent) const;
  
  //! Returns the Explicit Location defined by the Transformation
  //! Matrix of <ent>. Identity if there is none
  Standard_EXPORT gp_GTrsf ExplicitLocation (const Handle(IGESData_IGESEntity)& ent) const;
  
  //! Returns True if more than one Parent has been determined for
  //! <ent>, by adding direct References and Associativities
  Standard_EXPORT Standard_Boolean IsAmbiguous (const Handle(IGESData_IGESEntity)& ent) const;
  
  //! Returns True if <ent> is dependent from one and only one other
  //! Entity, either by Reference or by Associativity
  Standard_EXPORT Standard_Boolean HasParent (const Handle(IGESData_IGESEntity)& ent) const;
  
  //! Returns the unique Parent recorded for <ent>.
  //! Returns a Null Handle if there is none
  Standard_EXPORT Handle(IGESData_IGESEntity) Parent (const Handle(IGESData_IGESEntity)& ent) const;
  
  //! Returns True if the Parent, if there is one, is defined by
  //! a SingleParentEntity Associativity
  //! Else, if HasParent is True, it is by Reference
  Standard_EXPORT Standard_Boolean HasParentByAssociativity (const Handle(IGESData_IGESEntity)& ent) const;
  
  //! Returns the effective Location of the Parent of <ent>, if
  //! there is one : this Location is itself given as compound
  //! according dependences on the Parent, if there are some.
  //! Returns an Identity Transformation if no Parent is recorded.
  Standard_EXPORT gp_GTrsf ParentLocation (const Handle(IGESData_IGESEntity)& ent) const;
  
  //! Returns the effective Location of an Entity, i.e. the
  //! composition of its proper Transformation Matrix (returned by
  //! Transf) and its Parent's Location (returned by ParentLocation)
  Standard_EXPORT gp_GTrsf EffectiveLocation (const Handle(IGESData_IGESEntity)& ent) const;
  
  //! Analysis a Location given as a GTrsf, by trying to convert it
  //! to a Trsf (i.e. to a True Location of which effect is
  //! described by an Isometry or a Similarity)
  //! Works with the Precision given by default or by SetPrecision
  //! Calls ConvertLocation (see below)
  Standard_EXPORT Standard_Boolean AnalyseLocation (const gp_GTrsf& loc, gp_Trsf& result) const;
  
  //! Convertion of a Location, from GTrsf form to Trsf form
  //! Works with a precision given as argument.
  //! Returns True if the Conversion is possible, (hence, <result>
  //! contains the converted location), False else
  //! <unit>, if given, indicates the unit in which <loc> is defined
  //! in meters. It concerns the translation part (to be converted.
  //!
  //! As a class method, it can be called separately
  Standard_EXPORT static Standard_Boolean ConvertLocation (const Standard_Real prec, const gp_GTrsf& loc, gp_Trsf& result, const Standard_Real uni = 1);




  DEFINE_STANDARD_RTTIEXT(IGESData_ToolLocation,Standard_Transient)

protected:




private:


  Standard_Real theprec;
  Handle(IGESData_IGESModel) themodel;
  Interface_GeneralLib thelib;
  TColStd_Array1OfInteger therefs;
  TColStd_Array1OfInteger theassocs;


};







#endif // _IGESData_ToolLocation_HeaderFile
