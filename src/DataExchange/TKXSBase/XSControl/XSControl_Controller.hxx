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
#include <TColStd_SequenceOfInteger.hxx>
#include <Interface_HArray1OfHAsciiString.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Vector.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <NCollection_DataMap.hxx>
#include <Message_ProgressRange.hxx>

class IFSelect_WorkLibrary;
class Interface_Protocol;
class Transfer_ActorOfTransientProcess;
class Transfer_ActorOfFinderProcess;
class XSControl_WorkSession;
class Interface_InterfaceModel;
class Transfer_FinderProcess;
class TopoDS_Shape;

class XSControl_Controller;
DEFINE_STANDARD_HANDLE(XSControl_Controller, Standard_Transient)

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
//! Finally, Controller can be gathered in a general dictionary then
//! retrieved later by a general call (method Recorded)
//!
//! It does not manage the produced data, but the Actors make the
//! link between the norm and the application
class XSControl_Controller : public Standard_Transient
{
public:
  //! Changes names
  //! if a name is empty, the formerly set one remains
  //! Remark : Does not call Record or AutoRecord
  Standard_EXPORT void SetNames(const Standard_CString theLongName,
                                const Standard_CString theShortName);

  //! Records <me> is a general dictionary under Short and Long
  //! Names (see method Name)
  void AutoRecord() const
  {
    Record(Name(Standard_True));
    Record(Name(Standard_False));
  }

  //! Records <me> in a general dictionary under a name
  //! Error if <name> already used for another one
  Standard_EXPORT void Record(const Standard_CString name) const;

  //! Returns the Controller attached to a given name
  //! Returns a Null Handle if <name> is unknown
  Standard_EXPORT static Handle(XSControl_Controller) Recorded(const Standard_CString name);

  //! Returns a name, as given when initializing :
  //! rsc = False (D) : True Name attached to the Norm (long name)
  //! rsc = True : Name of the resource set (i.e. short name)
  Standard_CString Name(const Standard_Boolean rsc = Standard_False) const
  {
    return (rsc ? myShortName.ToCString() : myLongName.ToCString());
  }

  //! Returns the Protocol attached to the Norm (from field)
  const Handle(Interface_Protocol)& Protocol() const { return myAdaptorProtocol; }

  //! Returns the SignType attached to the norm (from field)
  // szv:const Handle(IFSelect_Signature) & SignType1() const
  // szv:{ return mySignType; }

  //! Returns the WorkLibrary attached to the Norm. Remark that it
  //! has to be in phase with the Protocol  (read from field)
  const Handle(IFSelect_WorkLibrary)& WorkLibrary() const { return myAdaptorLibrary; }

  //! Creates a new empty Model ready to receive data of the Norm
  //! Used to write data from Imagine to an interface file
  Standard_EXPORT virtual Handle(Interface_InterfaceModel) NewModel() const = 0;

  //! Returns the Actor for Read attached to the pair (norm,appli)
  //! It can be adapted for data of the input Model, as required
  //! Can be read from field then adapted with Model as required
  Standard_EXPORT virtual Handle(Transfer_ActorOfTransientProcess) ActorRead(
    const Handle(Interface_InterfaceModel)& model) const;

  //! Returns the Actor for Write attached to the pair (norm,appli)
  //! Read from field. Can be redefined
  Standard_EXPORT virtual Handle(Transfer_ActorOfFinderProcess) ActorWrite() const;

  //! Sets minimum and maximum values for modetrans (write)
  //! Erases formerly recorded bounds and values
  //! Actually only for shape
  //! Then, for each value a little help can be attached
  Standard_EXPORT void SetModeWrite(const Standard_Integer modemin,
                                    const Standard_Integer modemax,
                                    const Standard_Boolean shape = Standard_True);

  //! Attaches a short line of help to a value of modetrans (write)
  Standard_EXPORT void SetModeWriteHelp(const Standard_Integer modetrans,
                                        const Standard_CString help,
                                        const Standard_Boolean shape = Standard_True);

  //! Returns recorded min and max values for modetrans (write)
  //! Actually only for shapes
  //! Returns True if bounds are set, False else (then, free value)
  Standard_EXPORT Standard_Boolean
    ModeWriteBounds(Standard_Integer&      modemin,
                    Standard_Integer&      modemax,
                    const Standard_Boolean shape = Standard_True) const;

  //! Tells if a value of <modetrans> is a good value(within bounds)
  //! Actually only for shapes
  Standard_EXPORT Standard_Boolean IsModeWrite(const Standard_Integer modetrans,
                                               const Standard_Boolean shape = Standard_True) const;

  //! Returns the help line recorded for a value of modetrans
  //! empty if help not defined or not within bounds or if values are free
  Standard_EXPORT Standard_CString
    ModeWriteHelp(const Standard_Integer modetrans,
                  const Standard_Boolean shape = Standard_True) const;

  //! Tells if <obj> (an application object) is a valid candidate
  //! for a transfer to a Model.
  //! By default, asks the ActorWrite if known (through a
  //! TransientMapper). Can be redefined
  Standard_EXPORT virtual Standard_Boolean RecognizeWriteTransient(
    const Handle(Standard_Transient)& obj,
    const Standard_Integer            modetrans = 0) const;

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
  Standard_EXPORT virtual IFSelect_ReturnStatus TransferWriteTransient(
    const Handle(Standard_Transient)&       obj,
    const Handle(Transfer_FinderProcess)&   FP,
    const Handle(Interface_InterfaceModel)& model,
    const Standard_Integer                  modetrans   = 0,
    const Message_ProgressRange&            theProgress = Message_ProgressRange()) const;

  //! Tells if a shape is valid for a transfer to a model
  //! Asks the ActorWrite (through a ShapeMapper)
  Standard_EXPORT virtual Standard_Boolean RecognizeWriteShape(
    const TopoDS_Shape&    shape,
    const Standard_Integer modetrans = 0) const;

  //! Takes one Shape and transfers it to an
  //! InterfaceModel (already created, e.g. by NewModel)
  //! Default uses ActorWrite; can be redefined as necessary
  //! Returned value is a status, as follows :
  //! Done  OK ,  Void : No Result ,  Fail : Fail (e.g. exception)
  //! Error : bad conditions , bad model or null model
  Standard_EXPORT virtual IFSelect_ReturnStatus TransferWriteShape(
    const TopoDS_Shape&                     shape,
    const Handle(Transfer_FinderProcess)&   FP,
    const Handle(Interface_InterfaceModel)& model,
    const Standard_Integer                  modetrans   = 0,
    const Message_ProgressRange&            theProgress = Message_ProgressRange()) const;

  //! Records a Session Item, to be added for customisation of the Work Session.
  //! It must have a specific name.
  //! <setapplied> is used if <item> is a GeneralModifier, to decide
  //! If set to true, <item> will be applied to the hook list "send".
  //! Else, it is not applied to any hook list.
  //! Remark : this method is to be called at Create time,
  //! the recorded items will be used by Customise
  //! Warning : if <name> conflicts, the last recorded item is kept
  Standard_EXPORT void AddSessionItem(const Handle(Standard_Transient)& theItem,
                                      const Standard_CString            theName,
                                      const Standard_Boolean            toApply = Standard_False);

  //! Returns an item given its name to record in a Session
  //! If <name> is unknown, returns a Null Handle
  Standard_EXPORT Handle(Standard_Transient) SessionItem(const Standard_CString theName) const;

  //! Customises a WorkSession, by adding to it the recorded items (by AddSessionItem)
  Standard_EXPORT virtual void Customise(Handle(XSControl_WorkSession)& WS);

  const NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)>& AdaptorSession()
    const
  {
    return myAdaptorSession;
  }

  DEFINE_STANDARD_RTTIEXT(XSControl_Controller, Standard_Transient)

protected:
  //! Initializing with names
  //! <theLongName>  is for the complete, official, long  name
  //! <theShortName> is for the short name used for resources
  Standard_EXPORT XSControl_Controller(const Standard_CString theLongName,
                                       const Standard_CString theShortName);

  //! Records the name of a Static to be traced for a given use
  Standard_EXPORT void TraceStatic(const Standard_CString theName, const Standard_Integer theUse);

  TCollection_AsciiString      myShortName;
  TCollection_AsciiString      myLongName;
  Handle(IFSelect_WorkLibrary) myAdaptorLibrary;
  Handle(Interface_Protocol)   myAdaptorProtocol;
  // szv:Handle(IFSelect_Signature) mySignType;
  Handle(Transfer_ActorOfTransientProcess)                                 myAdaptorRead;
  Handle(Transfer_ActorOfFinderProcess)                                    myAdaptorWrite;
  NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)> myAdaptorSession;

private:
  TColStd_SequenceOfTransient                    myAdaptorApplied;
  NCollection_Vector<Handle(Standard_Transient)> myParams;
  NCollection_Vector<Standard_Integer>           myParamUses;
  Handle(Interface_HArray1OfHAsciiString)        myModeWriteShapeN;
};

#endif // _XSControl_Controller_HeaderFile
