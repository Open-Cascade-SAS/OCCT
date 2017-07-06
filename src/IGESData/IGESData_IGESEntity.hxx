// Created on: 1992-04-07
// Created by: Christian CAILLET
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

#ifndef _IGESData_IGESEntity_HeaderFile
#define _IGESData_IGESEntity_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Integer.hxx>
#include <IGESData_DefSwitch.hxx>
#include <Standard_Real.hxx>
#include <Standard_Character.hxx>
#include <Interface_EntityList.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Boolean.hxx>
#include <IGESData_DefType.hxx>
#include <IGESData_DefList.hxx>
#include <Standard_CString.hxx>
#include <Standard_Type.hxx>
class TCollection_HAsciiString;
class Interface_InterfaceError;
class IGESData_ReadWriteModule;
class IGESData_GeneralModule;
class IGESData_IGESReaderTool;
class IGESData_DirChecker;
class IGESData_IGESType;
class IGESData_LineFontEntity;
class IGESData_LevelListEntity;
class IGESData_ViewKindEntity;
class IGESData_TransfEntity;
class IGESData_LabelDisplayEntity;
class IGESData_ColorEntity;
class gp_GTrsf;
class Interface_EntityIterator;
class Interface_EntityList;


class IGESData_IGESEntity;
DEFINE_STANDARD_HANDLE(IGESData_IGESEntity, Standard_Transient)

//! defines root of IGES Entity definition, including Directory
//! Part, lists of (optionnal) Properties and Associativities
class IGESData_IGESEntity : public Standard_Transient
{

public:

  
  //! gives IGES typing info (includes "Type" and "Form" data)
  Standard_EXPORT IGESData_IGESType IGESType() const;
  
  //! gives IGES Type Number (often coupled with Form Number)
  Standard_EXPORT Standard_Integer TypeNumber() const;
  
  //! Returns the form number for that
  //! type of an IGES entity. The default form number is 0.
  Standard_EXPORT Standard_Integer FormNumber() const;
  
  //! Returns the Entity which has been recorded for a given
  //! Field Number, i.e. without any cast. Maps with :
  //! 3 : Structure   4 : LineFont     5 : LevelList     6 : View
  //! 7 : Transf(ormation Matrix)      8 : LabelDisplay
  //! 13 : Color.  Other values give a null handle
  //! It can then be of any kind, while specific items have a Type
  Standard_EXPORT Handle(IGESData_IGESEntity) DirFieldEntity (const Standard_Integer fieldnum) const;
  
  //! returns True if an IGESEntity is defined with a Structure
  //! (it is normally reserved for certain classes, such as Macros)
  Standard_EXPORT Standard_Boolean HasStructure() const;
  
  //! Returns Structure (used by some types of IGES Entities only)
  //! Returns a Null Handle if Structure is not defined
  Standard_EXPORT Handle(IGESData_IGESEntity) Structure() const;
  
  //! Returns the definition status of LineFont
  Standard_EXPORT virtual IGESData_DefType DefLineFont() const;
  
  //! Returns LineFont definition as an Integer (if defined as Rank)
  //! If LineFont is defined as an Entity, returns a negative value
  Standard_EXPORT Standard_Integer RankLineFont() const;
  
  //! Returns LineFont as an Entity (if defined as Reference)
  //! Returns a Null Handle if DefLineFont is not "DefReference"
  Standard_EXPORT Handle(IGESData_LineFontEntity) LineFont() const;
  
  //! Returns the definition status of Level
  Standard_EXPORT virtual IGESData_DefList DefLevel() const;
  
  //! Returns the level the entity
  //! belongs to. Returns -1 if the entity belongs to more than one  level.
  Standard_EXPORT Standard_Integer Level() const;
  
  //! Returns LevelList if Level is
  //! defined as a list. Returns a null handle if DefLevel is not DefSeveral.
  Standard_EXPORT Handle(IGESData_LevelListEntity) LevelList() const;
  
  //! Returns the definition status of
  //! the view. This can be: none, one or several.
  Standard_EXPORT virtual IGESData_DefList DefView() const;
  
  //! Returns the view of this IGES entity.
  //! This view can be a single view or a list of views.
  //! Warning A null handle is returned if the view is not defined.
  Standard_EXPORT Handle(IGESData_ViewKindEntity) View() const;
  
  //! Returns the view as a single view
  //! if it was defined as such and not as a list of views.
  //! Warning A null handle is returned if DefView does not have the value DefOne.
  Standard_EXPORT Handle(IGESData_ViewKindEntity) SingleView() const;
  
  //! Returns the view of this IGES entity as a list.
  //! Warning A null handle is returned if the
  //! definition status does not have the value DefSeveral.
  Standard_EXPORT Handle(IGESData_ViewKindEntity) ViewList() const;
  
  //! Returns True if a Transformation Matrix is defined
  Standard_EXPORT Standard_Boolean HasTransf() const;
  
  //! Returns the Transformation Matrix (under IGES definition)
  //! Returns a Null Handle if there is none
  //! for a more complete use, see Location & CompoundLocation
  Standard_EXPORT Handle(IGESData_TransfEntity) Transf() const;
  
  //! Returns True if a LabelDisplay mode is defined for this entity
  Standard_EXPORT Standard_Boolean HasLabelDisplay() const;
  
  //! Returns the Label Display
  //! Associativity Entity if there is one. Returns a null handle if there is none.
  Standard_EXPORT Handle(IGESData_LabelDisplayEntity) LabelDisplay() const;
  
  //! gives Blank Status (0 visible, 1 blanked)
  Standard_EXPORT Standard_Integer BlankStatus() const;
  
  //! gives Subordinate Switch (0-1-2-3)
  Standard_EXPORT Standard_Integer SubordinateStatus() const;
  
  //! gives Entity's Use Flag (0 to 5)
  Standard_EXPORT Standard_Integer UseFlag() const;
  
  //! gives Hierarchy status (0-1-2)
  Standard_EXPORT Standard_Integer HierarchyStatus() const;
  
  //! Returns the LineWeight Number (0  not defined), see also LineWeight
  Standard_EXPORT Standard_Integer LineWeightNumber() const;
  
  //! Returns the true Line Weight, computed from LineWeightNumber and
  //! Global Parameter in the Model by call to SetLineWeight
  Standard_EXPORT Standard_Real LineWeight() const;
  
  //! Returns the definition status of Color.
  Standard_EXPORT virtual IGESData_DefType DefColor() const;
  
  //! Returns the color definition as
  //! an integer value if the color was defined as a rank.
  //! Warning A negative value is returned if the color was defined as an entity.
  Standard_EXPORT Standard_Integer RankColor() const;
  
  //! Returns the IGES entity which
  //! describes the color of the entity.
  //! Returns a null handle if this entity was defined as an integer.
  Standard_EXPORT Handle(IGESData_ColorEntity) Color() const;
  
  //! returns "reserved" alphanumeric values res1 and res2
  //! res1 and res2 have to be reserved as Character[9 at least]
  //! (remark : their content is changed)
  //! returned values are ended by null character in 9th
  //! returned Boolean is False if res1 and res2 are blank, true else
  Standard_EXPORT Standard_Boolean CResValues (const Standard_CString res1, const Standard_CString res2) const;
  
  //! Returns true if a short label is defined.
  //! A short label is a non-blank 8-character string.
  Standard_EXPORT Standard_Boolean HasShortLabel() const;
  
  //! Returns the label value for this IGES entity as a string.
  //! Warning If the label is blank, this string is null.
  Standard_EXPORT Handle(TCollection_HAsciiString) ShortLabel() const;
  
  //! Returns true if a subscript number is defined.
  //! A subscript number is an integer used to identify a label.
  Standard_EXPORT virtual Standard_Boolean HasSubScriptNumber() const;
  
  //! Returns the integer subscript number used to identify this IGES entity.
  //! Warning 0 is returned if no subscript number is defined for this IGES entity.
  Standard_EXPORT Standard_Integer SubScriptNumber() const;
  
  //! Initializes a directory field as an Entiy of any kind
  //! See DirFieldEntity for more details
  Standard_EXPORT void InitDirFieldEntity (const Standard_Integer fieldnum, const Handle(IGESData_IGESEntity)& ent);
  
  //! Initializes Transf, or erases it if <ent> is given Null
  Standard_EXPORT void InitTransf (const Handle(IGESData_TransfEntity)& ent);
  
  //! Initializes View, or erases it if <ent> is given Null
  Standard_EXPORT void InitView (const Handle(IGESData_ViewKindEntity)& ent);
  
  //! Initializes LineFont : if <ent> is not Null, it gives LineFont,
  //! else <rank> gives or erases (if zero) RankLineFont
  Standard_EXPORT void InitLineFont (const Handle(IGESData_LineFontEntity)& ent, const Standard_Integer rank = 0);
  
  //! Initializes Level : if <ent> is not Null, it gives LevelList,
  //! else <val> gives or erases (if zero) unique Level
  Standard_EXPORT void InitLevel (const Handle(IGESData_LevelListEntity)& ent, const Standard_Integer val = 0);
  
  //! Initializes Color data : if <ent> is not Null, it gives Color,
  //! else <rank> gives or erases (if zero) RankColor
  Standard_EXPORT void InitColor (const Handle(IGESData_ColorEntity)& ent, const Standard_Integer rank = 0);
  
  //! Initializes the Status of Directory Part
  Standard_EXPORT void InitStatus (const Standard_Integer blank, const Standard_Integer subordinate, const Standard_Integer useflag, const Standard_Integer hierarchy);
  
  //! Sets a new Label to an IGES Entity
  //! If <sub> is given, it sets value of SubScriptNumber
  //! else, SubScriptNumber is erased
  Standard_EXPORT void SetLabel (const Handle(TCollection_HAsciiString)& label, const Standard_Integer sub = -1);
  
  //! Initializes various data (those not yet seen above), or erases
  //! them if they are given as Null (Zero for <weightnum>) :
  //! <str> for Structure, <lab> for LabelDisplay, and
  //! <weightnum> for WeightNumber
  Standard_EXPORT void InitMisc (const Handle(IGESData_IGESEntity)& str, const Handle(IGESData_LabelDisplayEntity)& lab, const Standard_Integer weightnum);
  
  //! Returns True if an entity has one and only one parent, defined
  //! by a SingleParentEntity Type Associativity (explicit sharing).
  //! Thus, implicit sharing remains defined at model level
  //! (see class ToolLocation)
  Standard_EXPORT Standard_Boolean HasOneParent() const;
  
  //! Returns the Unique Parent (in the sense given by HasOneParent)
  //! Error if there is none or several
  Standard_EXPORT Handle(IGESData_IGESEntity) UniqueParent() const;
  
  //! Returns Location given by Transf in Directory Part (see above)
  //! It must be considered for local definition : if the Entity is
  //! set in a "Parent", that one can add its one Location, but this
  //! is not taken in account here : see CompoundLocation for that.
  //! If no Transf is defined, returns Identity
  //! If Transf is itself compound, gives the final result
  Standard_EXPORT gp_GTrsf Location() const;
  
  //! Returns Location considered for Vectors, i.e. without its
  //! Translation Part. As Location, it gives local definition.
  Standard_EXPORT gp_GTrsf VectorLocation() const;
  
  //! Returns Location by taking in account a Parent which has its
  //! own Location : that one will be combined to that of <me>
  //! The Parent is considered only if HasOneParent is True,
  //! else it is ignored and CompoundLocation = Location
  Standard_EXPORT gp_GTrsf CompoundLocation() const;
  
  //! says if a Name is defined, as Short Label or as Name Property
  //! (Property is looked first, else ShortLabel is considered)
  Standard_EXPORT Standard_Boolean HasName() const;
  
  //! returns Name value as a String (Property Name or ShortLabel)
  //! if SubNumber is defined, it is concatenated after ShortLabel
  //! as follows label(number). Ignored with a Property Name
  Standard_EXPORT Handle(TCollection_HAsciiString) NameValue() const;
  
  //! Returns True if the Entity is defined with an Associativity
  //! list, even empty (that is, file contains its length 0)
  //! Else, the file contained NO idencation at all about this list.
  Standard_EXPORT Standard_Boolean ArePresentAssociativities() const;
  
  //! gives number of recorded associativities (0  no list defined)
  Standard_EXPORT Standard_Integer NbAssociativities() const;
  
  //! Returns the Associativity List under the form of an EntityIterator.
  Standard_EXPORT Interface_EntityIterator Associativities() const;
  
  //! gives how many Associativities have a given type
  Standard_EXPORT Standard_Integer NbTypedAssociativities (const Handle(Standard_Type)& atype) const;
  
  //! returns the Associativity of a given Type (if only one exists)
  //! Error if none or more than one
  Standard_EXPORT Handle(IGESData_IGESEntity) TypedAssociativity (const Handle(Standard_Type)& atype) const;
  
  //! Sets "me" in the Associativity list of another Entity
  Standard_EXPORT void Associate (const Handle(IGESData_IGESEntity)& ent) const;
  
  //! Resets "me" from the Associativity list of another Entity
  Standard_EXPORT void Dissociate (const Handle(IGESData_IGESEntity)& ent) const;
  
  //! Returns True if the Entity is defined with a Property list,
  //! even empty (that is, file contains its length 0)
  //! Else, the file contained NO idencation at all about this list
  Standard_EXPORT Standard_Boolean ArePresentProperties() const;
  
  //! Gives number of recorded properties (0  no list defined)
  Standard_EXPORT Standard_Integer NbProperties() const;
  
  //! Returns Property List under the form of an EntityIterator
  Standard_EXPORT Interface_EntityIterator Properties() const;
  
  //! gives how many Properties have a given type
  Standard_EXPORT Standard_Integer NbTypedProperties (const Handle(Standard_Type)& atype) const;
  
  //! returns the Property of a given Type
  //! Error if none or more than one
  Standard_EXPORT Handle(IGESData_IGESEntity) TypedProperty (const Handle(Standard_Type)& atype, const Standard_Integer anum = 0) const;
  
  //! Adds a Property in the list
  Standard_EXPORT void AddProperty (const Handle(IGESData_IGESEntity)& ent);
  
  //! Removes a Property from the list
  Standard_EXPORT void RemoveProperty (const Handle(IGESData_IGESEntity)& ent);
  
  //! computes and sets "true" line weight according IGES rules from
  //! global data MaxLineWeight (maxv) and LineWeightGrad (gradw),
  //! or sets it to defw (Default) if LineWeightNumber is null
  Standard_EXPORT void SetLineWeight (const Standard_Real defw, const Standard_Real maxw, const Standard_Integer gradw);


friend class IGESData_ReadWriteModule;
friend class IGESData_GeneralModule;
friend class IGESData_IGESReaderTool;
friend class IGESData_DirChecker;


  DEFINE_STANDARD_RTTIEXT(IGESData_IGESEntity,Standard_Transient)

protected:

  
  //! prepares lists of optionnal data, set values to defaults
  Standard_EXPORT IGESData_IGESEntity();
  
  //! Initializes Type and Form Numbers to new values. Reserved for
  //! special uses
  Standard_EXPORT void InitTypeAndForm (const Standard_Integer typenum, const Standard_Integer formnum);
  
  //! Loads a complete, already loaded, List of Asociativities
  //! (used during Read or Copy Operations)
  Standard_EXPORT void LoadAssociativities (const Interface_EntityList& list);
  
  //! Loads a complete, already loaded, List of Properties
  //! (used during Read or Copy Operations)
  Standard_EXPORT void LoadProperties (const Interface_EntityList& list);
  
  //! Removes all properties in once
  Standard_EXPORT void ClearProperties();



private:

  
  //! Clears specific IGES data
  Standard_EXPORT void Clear();
  
  //! Adds an Associativity in the list (called by Associate only)
  Standard_EXPORT void AddAssociativity (const Handle(IGESData_IGESEntity)& ent);
  
  //! Removes an Associativity from the list (called by Dissociate)
  Standard_EXPORT void RemoveAssociativity (const Handle(IGESData_IGESEntity)& ent);
  
  //! Removes all associativities in once
  Standard_EXPORT void ClearAssociativities();

  Standard_Integer theType;
  Standard_Integer theForm;
  Handle(IGESData_IGESEntity) theStructure;
  IGESData_DefSwitch theDefLineFont;
  Handle(IGESData_IGESEntity) theLineFont;
  Standard_Integer theDefLevel;
  Handle(IGESData_IGESEntity) theLevelList;
  Handle(IGESData_IGESEntity) theView;
  Handle(IGESData_IGESEntity) theTransf;
  Handle(IGESData_IGESEntity) theLabDisplay;
  Standard_Integer theStatusNum;
  Standard_Integer theLWeightNum;
  Standard_Real theLWeightVal;
  IGESData_DefSwitch theDefColor;
  Handle(IGESData_IGESEntity) theColor;
  Standard_Character theRes1[9];
  Standard_Character theRes2[9];
  Handle(TCollection_HAsciiString) theShortLabel;
  Standard_Integer theSubScriptN;
  Interface_EntityList theAssocs;
  Interface_EntityList theProps;


};







#endif // _IGESData_IGESEntity_HeaderFile
