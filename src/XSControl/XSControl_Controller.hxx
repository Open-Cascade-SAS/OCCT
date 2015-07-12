// Created on: 1995-03-13
// Created by: Christian CAILLET
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

#ifndef _XSControl_Controller_HeaderFile
#define _XSControl_Controller_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TCollection_AsciiString.hxx>
#include <TColStd_SequenceOfTransient.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <Interface_HArray1OfHAsciiString.hxx>
#include <MMgt_TShared.hxx>
#include <Standard_CString.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <IFSelect_ReturnStatus.hxx>
class MoniTool_Profile;
class IFSelect_WorkLibrary;
class Interface_Protocol;
class IFSelect_Signature;
class Transfer_ActorOfTransientProcess;
class Transfer_ActorOfFinderProcess;
class Dico_DictionaryOfTransient;
class Standard_DomainError;
class XSControl_WorkSession;
class Interface_InterfaceModel;
class Standard_Transient;
class Transfer_FinderProcess;
class TopoDS_Shape;
class Interface_CheckIterator;


class XSControl_Controller;
DEFINE_STANDARD_HANDLE(XSControl_Controller, MMgt_TShared)

//! This class allows a general X-STEP engine to run generic
//! functions on any interface norm, in the same way. It includes
//! the transfer operations. I.e. it gathers the already available
//! general modules, the engine has just to know it
//!
//! The important point is that a given X-STEP Controller is
//! attached to a given couple made of an Interface Norm (such as
//! IGES-5.1) and an application data model (CasCade Shapes for
//! instance).
//!
//! A Controller brings a Profile, this allows to have several
//! variants on the same basic definition, for instance keep the
//! norm definition but give several transfer actors, etc
//!
//! Finally, Controller can be gathered in a general dictionary then
//! retreived later by a general call (method Recorded)
//!
//! It does not manage the produced data, but the Actors make the
//! link between the norm and the application
class XSControl_Controller : public MMgt_TShared
{

public:

  
  //! Changes names
  //! if a name is empty, the formerly set one remains
  //! Remark : Does not call Record or AutoRecord
  Standard_EXPORT void SetNames (const Standard_CString longname, const Standard_CString shortname);
  
  //! Records <me> is a general dictionary under Short and Long
  //! Names (see method Name)
  Standard_EXPORT void AutoRecord() const;
  
  //! Records <me> in a general dictionary under a name
  //! Error if <name> already used for another one
  Standard_EXPORT void Record (const Standard_CString name) const;
  
  //! Returns the Controller attached to a given name
  //! Returns a Null Handle if <name> is unknown
  Standard_EXPORT static Handle(XSControl_Controller) Recorded (const Standard_CString name);
  
  //! Returns the list of names of recorded norms, according to mode
  //! = 0 (D) : all the recorded names
  //! < 0 : for each distinct norm, its resource (short) name
  //! > 0 : for each distinct norm, its complete (long)  name
  Standard_EXPORT static Handle(TColStd_HSequenceOfHAsciiString) ListRecorded (const Standard_Integer mode = 0);
  
  //! Returns a name, as given when initializing :
  //! rsc = False (D) : True Name attached to the Norm (long name)
  //! rsc = True : Name of the ressource set (i.e. short name)
  Standard_EXPORT Standard_CString Name (const Standard_Boolean rsc = Standard_False) const;
  
  //! Returns the Profile
  //! It starts with a first configuration Base (empty) and the
  //! following options :
  //! protocol    for the Protocol
  //! sign-type   for the SignType (Default Signature for Type)
  //! access      for the WorkLibrary
  //! tr-read  for ActorRead  (import processor)
  //! tr-write for ActorWrite (export processor)
  Standard_EXPORT Handle(MoniTool_Profile) Profile() const;
  
  //! Considers the current state of the Controller as defining a
  //! configuration, newly created or already existing
  Standard_EXPORT void DefineProfile (const Standard_CString confname);
  
  //! Sets the Controller in a given Configuration of its Profile
  //! Calls SettingProfile (which can be redefined)
  //!
  //! Returns True if done, False if <confname> unknown
  Standard_EXPORT Standard_Boolean SetProfile (const Standard_CString confname);
  
  //! This method is called by SetProfile, it can be redefined
  //! for specific sub-class of Controller
  //! The default does nothing
  Standard_EXPORT virtual Standard_Boolean SettingProfile (const Standard_CString confname);
  
  //! Applies a Configuration of the Profile to the WorkSession
  //! I.E. calls SetProfile then fills WorkSession with definitions
  Standard_EXPORT Standard_Boolean ApplyProfile (const Handle(XSControl_WorkSession)& WS, const Standard_CString confname);
  
  //! Called by ApplyProfile, can be redefined for specific
  //! sub-class of Controller
  //! The default does nothing
  Standard_EXPORT virtual Standard_Boolean ApplyingProfile (const Handle(XSControl_WorkSession)& WS, const Standard_CString confname);
  
  //! Returns the Protocol attached to the Norm (from field)
  Standard_EXPORT Handle(Interface_Protocol) Protocol() const;
  
  //! Returns the SignType attached to the norm (from field)
  Standard_EXPORT Handle(IFSelect_Signature) SignType() const;
  
  //! Returns the WorkLibrary attached to the Norm. Remark that it
  //! has to be in phase with the Protocol  (read from field)
  Standard_EXPORT Handle(IFSelect_WorkLibrary) WorkLibrary() const;
  
  //! Creates a new empty Model ready to receive data of the Norm
  //! Used to write data from Imagine to an interface file
  Standard_EXPORT virtual Handle(Interface_InterfaceModel) NewModel() const = 0;
  
  //! Returns the Actor for Read attached to the pair (norm,appli)
  //! It can be adapted for data of the input Model, as required
  //! Can be read from field then adapted with Model as required
  Standard_EXPORT virtual Handle(Transfer_ActorOfTransientProcess) ActorRead (const Handle(Interface_InterfaceModel)& model) const = 0;
  
  //! Returns the Actor for Write attached to the pair (norm,appli)
  //! Read from field. Can be redefined
  Standard_EXPORT virtual Handle(Transfer_ActorOfFinderProcess) ActorWrite() const;
  
  //! Updates static values
  //! <mode> precises the kind of updating : (see Items from Static)
  //! -1 : a precise static item : criter = its name
  //! 0  : all items of a family : criter = the family name
  //! 1  : all items which match regexp name : criter = regexp name
  //! By default (criter empty) should consider all relevant statics
  //! If <name> is defined, can consider only this static item
  //! The provided default method does nothing, to be redefined
  Standard_EXPORT virtual void UpdateStatics (const Standard_Integer mode, const Standard_CString criter = "") const;
  
  //! Sets mininum and maximum values for modetrans (write)
  //! Erases formerly recorded bounds and values
  //! Actually only for shape
  //! Then, for each value a little help can be attached
  Standard_EXPORT void SetModeWrite (const Standard_Integer modemin, const Standard_Integer modemax, const Standard_Boolean shape = Standard_True);
  
  //! Attaches a short line of help to a value of modetrans (write)
  Standard_EXPORT void SetModeWriteHelp (const Standard_Integer modetrans, const Standard_CString help, const Standard_Boolean shape = Standard_True);
  
  //! Returns recorded min and max values for modetrans (write)
  //! Actually only for shapes
  //! Returns True if bounds are set, False else (then, free value)
  Standard_EXPORT Standard_Boolean ModeWriteBounds (Standard_Integer& modemin, Standard_Integer& modemax, const Standard_Boolean shape = Standard_True) const;
  
  //! Tells if a value of <modetrans> is a good value(within bounds)
  //! Actually only for shapes
  Standard_EXPORT Standard_Boolean IsModeWrite (const Standard_Integer modetrans, const Standard_Boolean shape = Standard_True) const;
  
  //! Returns the help line recorded for a value of modetrans
  //! empty if help not defined or not within bounds or if values are free
  Standard_EXPORT Standard_CString ModeWriteHelp (const Standard_Integer modetrans, const Standard_Boolean shape = Standard_True) const;
  
  //! Tells if <obj> (an application object) is a valid candidate
  //! for a transfer to a Model.
  //! By default, asks the ActorWrite if known (through a
  //! TransientMapper). Can be redefined
  Standard_EXPORT virtual Standard_Boolean RecognizeWriteTransient (const Handle(Standard_Transient)& obj, const Standard_Integer modetrans = 0) const;
  
  //! Takes one Transient Object and transfers it to an
  //! InterfaceModel (already created, e.g. by NewModel)
  //! (result is recorded in the model by AddWithRefs)
  //! FP records produced results and checks
  //!
  //! Default uses ActorWrite; can be redefined as necessary
  //! Returned value is a status, as follows :
  //! 0  OK ,  1 No Result ,  2 Fail (e.g. exception raised)
  //! -1 bad conditions ,  -2 bad model or null model
  //! For type of object not recognized : should return 1
  Standard_EXPORT virtual IFSelect_ReturnStatus TransferWriteTransient (const Handle(Standard_Transient)& obj, const Handle(Transfer_FinderProcess)& FP, const Handle(Interface_InterfaceModel)& model, const Standard_Integer modetrans = 0) const;
  
  //! Tells if a shape is valid for a transfer to a model
  //! Asks the ActorWrite (through a ShapeMapper)
  Standard_EXPORT virtual Standard_Boolean RecognizeWriteShape (const TopoDS_Shape& shape, const Standard_Integer modetrans = 0) const;
  
  //! Takes one Shape and transfers it to an
  //! InterfaceModel (already created, e.g. by NewModel)
  //! Default uses ActorWrite; can be redefined as necessary
  //! Returned value is a status, as follows :
  //! Done  OK ,  Void : No Result ,  Fail : Fail (e.g. exception)
  //! Error : bad conditions , bad model or null model
  //! Resolution of file clusters
  //! According to each norm, there can (or not) be files of which
  //! definition is not complete but refers to other files : this defines
  //! a file cluster.
  //! It can then be resolved by two calls :
  //! - ClusterContext prepares the resolution, specific of each case
  //! - ResolveCluster performs the resolution, its result consists in
  //! having all data gathered in one final model
  Standard_EXPORT virtual IFSelect_ReturnStatus TransferWriteShape (const TopoDS_Shape& shape, const Handle(Transfer_FinderProcess)& FP, const Handle(Interface_InterfaceModel)& model, const Standard_Integer modetrans = 0) const;
  
  //! Prepares and returns a context to resolve a cluster
  //! All data to be used are detained by the WorkSession
  //! The definition of this context is free and proper to each case
  //! remark that it is aimed to be used in ResolveCluster
  //!
  //! The context must be prepared, but resolution must not have
  //! began
  //!
  //! If no cluster has to be resolved, should return a null handle
  //! This is the default case, which can be redefined
  Standard_EXPORT virtual Handle(Standard_Transient) ClusterContext (const Handle(XSControl_WorkSession)& WS) const;
  
  //! Performs the resolution itself, from the starting data and
  //! the cluster context
  //!
  //! Can fill a CheckList as necessary (especially when one or
  //! more references remain unresolved)
  //!
  //! Default does nothing and returns an empty CheckList
  Standard_EXPORT virtual Interface_CheckIterator ResolveCluster (const Handle(XSControl_WorkSession)& WS, const Handle(Standard_Transient)& context) const;
  
  //! Adds an item in the control list
  //! A control item of a controller is accessed by its name which
  //! is specific of a kind of item (i.e. a kind of functionnality)
  //! Adds or replaces if <name> is already recorded
  Standard_EXPORT void AddControlItem (const Handle(Standard_Transient)& item, const Standard_CString name);
  
  //! Returns a control item from its name, Null if <name> unknown
  //! To be used then, it just remains to be down-casted
  Standard_EXPORT Handle(Standard_Transient) ControlItem (const Standard_CString name) const;
  
  //! Records the name of a Static to be traced for a given use
  Standard_EXPORT void TraceStatic (const Standard_CString name, const Standard_Integer use);
  
  //! Records a Session Item, to be added for customisation of the
  //! Work Session. It must have a specific name.
  //! <setapplied> is used if <item> is a GeneralModifier, to decide
  //! to which hook list it will be applied, if not empty (else,
  //! not applied to any hook list)
  //! ACTUAL : only one hook list is managed : "send"
  //! Remark : this method is to be called at Create time, the
  //! recorded items will be used by Customise
  //! Warning : if <name> conflicts, the last recorded item is kept
  Standard_EXPORT void AddSessionItem (const Handle(Standard_Transient)& item, const Standard_CString name, const Standard_CString setapplied = "");
  
  //! Returns an item given its name to record in a Session
  //! If <name> is unknown, returns a Null Handle
  Standard_EXPORT Handle(Standard_Transient) SessionItem (const Standard_CString name) const;
  
  //! Returns True if <item> is recorded as <setapplied = True>
  Standard_EXPORT Standard_Boolean IsApplied (const Handle(Standard_Transient)& item) const;
  
  //! Customises a WorkSession, by adding to it the recorded items
  //! (by AddSessionItem), then by calling a specific method
  //! Customising, set by default to do nothing
  Standard_EXPORT virtual void Customise (Handle(XSControl_WorkSession)& WS);
  
  //! Specific customisation method, which can be redefined
  //! Default does nothing
  Standard_EXPORT void Customising (Handle(XSControl_WorkSession)& WS);
  
  Standard_EXPORT Handle(Dico_DictionaryOfTransient) AdaptorSession() const;




  DEFINE_STANDARD_RTTI(XSControl_Controller,MMgt_TShared)

protected:

  
  //! Initializing with names
  //! <longname>  is for the complete, official, long  name
  //! <shortname> is for the short name used for resources
  Standard_EXPORT XSControl_Controller(const Standard_CString longname, const Standard_CString shortname);

  TCollection_AsciiString theShortName;
  TCollection_AsciiString theLongName;
  Handle(IFSelect_WorkLibrary) theAdaptorLibrary;
  Handle(Interface_Protocol) theAdaptorProtocol;
  Handle(IFSelect_Signature) theSignType;
  Handle(Transfer_ActorOfTransientProcess) theAdaptorRead;
  Handle(Transfer_ActorOfFinderProcess) theAdaptorWrite;
  Handle(Dico_DictionaryOfTransient) theAdaptorSession;


private:


  Handle(MoniTool_Profile) theProfile;
  Handle(Dico_DictionaryOfTransient) theItems;
  TColStd_SequenceOfTransient theAdaptorApplied;
  Handle(TColStd_HSequenceOfHAsciiString) theAdaptorHooks;
  TColStd_SequenceOfTransient theParams;
  TColStd_SequenceOfInteger theParamUses;
  Handle(Interface_HArray1OfHAsciiString) theModeWriteShapeN;


};







#endif // _XSControl_Controller_HeaderFile
