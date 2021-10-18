// Created on: 1992-12-15
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

#ifndef _IFSelect_WorkSession_HeaderFile
#define _IFSelect_WorkSession_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Boolean.hxx>
#include <TCollection_AsciiString.hxx>
#include <Interface_CheckIterator.hxx>
#include <TColStd_IndexedDataMapOfTransientTransient.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Vector.hxx>
#include <NCollection_DataMap.hxx>
#include <Standard_CString.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <TColStd_HSequenceOfInteger.hxx>
#include <Standard_Type.hxx>
#include <Standard_Transient.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>
#include <IFSelect_RemainMode.hxx>
#include <TColStd_SequenceOfTransient.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <IFSelect_PrintCount.hxx>
#include <TCollection_AsciiString.hxx>
class IFSelect_ShareOut;
class IFSelect_WorkLibrary;
class Interface_Protocol;
class Interface_InterfaceModel;
class Interface_HGraph;
class Interface_GTool;

class IFSelect_ModelCopier;
class IFSelect_Signature;
class TCollection_HAsciiString;
class Interface_Graph;
class IFSelect_IntParam;
class IFSelect_Selection;
class Interface_EntityIterator;
class IFSelect_SelectionIterator;
class IFSelect_SignCounter;
class IFSelect_Dispatch;
class IFSelect_GeneralModifier;
class IFSelect_Modifier;
class IFSelect_Transformer;
class IFSelect_PacketList;
class IFSelect_SignatureList;

class IFSelect_WorkSession;
DEFINE_STANDARD_HANDLE(IFSelect_WorkSession, Standard_Transient)

//! This class can be used to simply manage a process such as
//! splitting a file, extracting a set of Entities ...
//! It allows to manage different types of Variables : Integer or
//! Text Parameters, Selections, Dispatches, in addition to a
//! ShareOut. To each of these variables, a unique Integer
//! Identifier is attached. A Name can be attached too as desired.
class IFSelect_WorkSession : public Standard_Transient
{

public:

  //! Creates a Work Session
  //! It provides default, empty ShareOut and ModelCopier, which can
  //! be replaced (if required, should be done just after creation).
  Standard_EXPORT IFSelect_WorkSession();
  
  //! Changes the Error Handler status (by default, it is not set)
  Standard_EXPORT void SetErrorHandle (const Standard_Boolean toHandle);
  
  //! Returns the Error Handler status
  Standard_Boolean ErrorHandle() const
  { return theerrhand; }
  
  //! Returns the ShareOut defined at creation time
  const Handle(IFSelect_ShareOut) & ShareOut() const
  { return theshareout; }
  
  //! Sets a new ShareOut. Fills Items which its content
  //! Warning : data from the former ShareOut are lost
  Standard_EXPORT void SetShareOut (const Handle(IFSelect_ShareOut)& shareout);

  //! Set value of mode responsible for presence of selections after loading
  //! If mode set to true that different selections will be accessible after loading
  //! else selections will be not accessible after loading( for economy memory in applications)
  void SetModeStat (const Standard_Boolean theMode)
  { themodelstat = theMode; }

  //! Return value of mode defining of filling selection during loading
  Standard_Boolean GetModeStat() const
  { return themodelstat; }
  
  //! Sets a WorkLibrary, which will be used to Read and Write Files
  void SetLibrary (const Handle(IFSelect_WorkLibrary) &theLib)
  { thelibrary = theLib; }
  
  //! Returns the WorkLibrary. Null Handle if not yet set
  //! should be C++ : return const &
  const Handle(IFSelect_WorkLibrary) & WorkLibrary() const
  { return thelibrary; }
  
  //! Sets a Protocol, which will be used to determine Graphs, to
  //! Read and to Write Files
  Standard_EXPORT void SetProtocol (const Handle(Interface_Protocol)& protocol);
  
  //! Returns the Protocol. Null Handle if not yet set
  //! should be C++ : return const &
  const Handle(Interface_Protocol) & Protocol() const
  { return theprotocol; }
  
  //! Sets a specific Signature to be the SignType, i.e. the
  //! Signature which will determine TypeName from the Model
  //! (basic function). It is recorded in the GTool
  //! This Signature is also set as "xst-sign-type" (reserved name)
  Standard_EXPORT void SetSignType (const Handle(IFSelect_Signature)& signtype);
  
  //! Returns the current SignType
  Standard_EXPORT Handle(IFSelect_Signature) SignType() const;
  
  //! Returns True is a Model has been set
  Standard_Boolean HasModel() const
  { return (!myModel.IsNull()); }
  
  //! Sets a Model as input : this will be the Model from which the
  //! ShareOut will work
  //! if <clearpointed> is True (default) all SelectPointed items
  //! are cleared, else they must be managed by the caller
  //! Remark : SetModel clears the Graph, recomputes it if a
  //! Protocol is set and if the Model is not empty, of course
  Standard_EXPORT void SetModel (const Handle(Interface_InterfaceModel)& model, const Standard_Boolean clearpointed = Standard_True);
  
  //! Returns the Model of the Work Session (Null Handle if none)
  //! should be C++ : return const &
  const Handle(Interface_InterfaceModel) & Model () const
  { return myModel; }
  
  //! Stores the filename used for read for setting the model
  //! It is cleared by SetModel and ClearData(1)
  void SetLoadedFile (const Standard_CString theFileName)
  { theloaded = theFileName; }
  
  //! Returns the filename used to load current model
  //! empty if unknown
  Standard_CString LoadedFile() const
  { return theloaded.ToCString(); }
  
  //! Reads a file with the WorkLibrary (sets Model and LoadedFile)
  //! Returns a integer status which can be :
  //! RetDone if OK,  RetVoid if no Protocol not defined,
  //! RetError for file not found, RetFail if fail during read
  Standard_EXPORT IFSelect_ReturnStatus ReadFile (const Standard_CString filename);

  //! Reads a file from stream with the WorkLibrary (sets Model and LoadedFile)
  //! Returns a integer status which can be :
  //! RetDone if OK,  RetVoid if no Protocol not defined,
  //! RetError for file not found, RetFail if fail during read
  Standard_EXPORT IFSelect_ReturnStatus ReadStream (const Standard_CString theName, std::istream& theIStream);
  
  //! Returns the count of Entities stored in the Model, or 0
  Standard_EXPORT Standard_Integer NbStartingEntities() const;
  
  //! Returns an  Entity stored in the Model of the WorkSession
  //! (Null Handle is no Model or num out of range)
  Standard_EXPORT Handle(Standard_Transient) StartingEntity (const Standard_Integer num) const;
  
  //! Returns the Number of an Entity in the Model
  //! (0 if no Model set or <ent> not in the Model)
  Standard_EXPORT Standard_Integer StartingNumber (const Handle(Standard_Transient)& ent) const;
  
  //! From a given label in Model, returns the corresponding number
  //! Starts from first entity by Default, may start after a given
  //! number : this number may be given negative, its absolute value
  //! is then considered. Hence a loop on NumberFromLabel may be
  //! programmed (stop test is : returned value positive or null)
  //!
  //! Returns 0 if not found, < 0 if more than one found (first
  //! found in negative).
  //! If <val> just gives an integer value, returns it
  Standard_EXPORT Standard_Integer NumberFromLabel (const Standard_CString val, const Standard_Integer afternum = 0) const;
  
  //! Returns the label for <ent>, as the Model does
  //! If <ent> is not in the Model or if no Model is loaded, a Null
  //! Handle is returned
  Standard_EXPORT Handle(TCollection_HAsciiString) EntityLabel (const Handle(Standard_Transient)& ent) const;
  
  //! Returns the Name of an Entity
  //! This Name is computed by the general service Name
  //! Returns a Null Handle if fails
  Standard_EXPORT Handle(TCollection_HAsciiString) EntityName (const Handle(Standard_Transient)& ent) const;
  
  //! Returns the Category Number determined for an entity
  //! it is computed by the class Category
  //! An unknown entity (number 0) gives a value -1
  Standard_EXPORT Standard_Integer CategoryNumber (const Handle(Standard_Transient)& ent) const;
  
  //! Returns the Category Name determined for an entity
  //! it is computed by the class Category
  //! Remark : an unknown entity gives an empty string
  Standard_EXPORT Standard_CString CategoryName (const Handle(Standard_Transient)& ent) const;
  
  //! Returns the Validity Name determined for an entity
  //! it is computed by the class SignValidity
  //! Remark : an unknown entity gives an empty string
  Standard_EXPORT Standard_CString ValidityName (const Handle(Standard_Transient)& ent) const;
  
  //! Clears recorded data (not the items) according mode :
  //! 1 : all Data : Model, Graph, CheckList, + ClearData 4
  //! 2 : Graph and CheckList (they will then be recomputed later)
  //! 3 : CheckList (it will be recomputed by ComputeCheck)
  //! 4 : just content of SelectPointed and Counters
  //! Plus 0 : does nothing but called by SetModel
  //! ClearData is virtual, hence it can be redefined to clear
  //! other data of a specialised Work Session
  Standard_EXPORT virtual void ClearData (const Standard_Integer mode);
  
  //! Computes the Graph used for Selections, Displays ...
  //! If a HGraph is already set, with same model as given by method
  //! Model, does nothing. Else, computes a new Graph.
  //! If <enforce> is given True, computes a new Graph anyway.
  //! Remark that a call to ClearGraph will cause ComputeGraph to
  //! really compute a new Graph
  //! Returns True if Graph is OK, False else (i.e. if no Protocol
  //! is set, or if Model is absent or empty).
  Standard_EXPORT Standard_Boolean ComputeGraph (const Standard_Boolean enforce = Standard_False);
  
  //! Returns the Computed Graph as HGraph (Null Handle if not set)
  Standard_EXPORT Handle(Interface_HGraph) HGraph();
  
  //! Returns the Computed Graph, for Read only
  Standard_EXPORT const Interface_Graph& Graph();
  
  //! Returns the list of entities shared by <ent> (can be empty)
  //! Returns a null Handle if <ent> is unknown
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) Shareds (const Handle(Standard_Transient)& ent);
  
  //! Returns the list of entities sharing <ent> (can be empty)
  //! Returns a null Handle if <ent> is unknown
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) Sharings (const Handle(Standard_Transient)& ent);
  
  //! Returns True if a Model is defined and really loaded (not
  //! empty), a Protocol is set and a Graph has been computed.
  //! In this case, the WorkSession can start to work
  Standard_EXPORT Standard_Boolean IsLoaded() const;
  
  //! Computes the CheckList for the Model currently loaded
  //! It can then be used for displays, queries ...
  //! Returns True if OK, False else (i.e. no Protocol set, or Model
  //! absent). If <enforce> is False, works only if not already done
  //! or if a new Model has been loaded from last call.
  //! Remark : computation is enforced by every call to
  //! SetModel or RunTransformer
  Standard_EXPORT Standard_Boolean ComputeCheck (const Standard_Boolean enforce = Standard_False);
  
  //! Returns the Check List for the Model currently loaded :
  //! <complete> = True  : complete (syntactic & semantic messages),
  //! computed if not yet done
  //! <complete> = False : only syntactic (check file form)
  Standard_EXPORT Interface_CheckIterator ModelCheckList (const Standard_Boolean complete = Standard_True);
  
  //! Returns a Check for a single entity, under the form of a
  //! CheckIterator (this gives only one form for the user)
  //! if <ent> is Null or equates the current Model, it gives the
  //! Global Check, else the Check for the given entity
  //! <complete> as for ModelCheckList
  Standard_EXPORT Interface_CheckIterator CheckOne (const Handle(Standard_Transient)& ent, const Standard_Boolean complete = Standard_True);
  
  //! Returns the Check List produced by the last execution of
  //! either : EvaluateFile(for Split), SendSplit, SendAll,
  //! SendSelected, RunTransformer-RunModifier
  //! Cleared by SetModel or ClearData(1)
  //! The field is protected, hence a specialized WorkSession may
  //! fill it
  Interface_CheckIterator LastRunCheckList() const
  { return thecheckrun; }
  
  //! Returns the Maximum Value for an Item Identifier. It can be
  //! greater to the count of known Items, because some can have
  //! been removed
  Standard_EXPORT Standard_Integer MaxIdent() const;
  
  //! Returns an Item, given its Ident. Returns a Null Handle if
  //! no Item corresponds to this Ident.
  Standard_EXPORT Handle(Standard_Transient) Item (const Standard_Integer id) const;
  
  //! Returns the Ident attached to an Item in the WorkSession, or
  //! Zero if it is unknown
  Standard_EXPORT Standard_Integer ItemIdent (const Handle(Standard_Transient)& item) const;
  
  //! Returns the Item which corresponds to a Variable, given its
  //! Name (whatever the type of this Item).
  //! Returns a Null Handle if this Name is not recorded
  Standard_EXPORT Handle(Standard_Transient) NamedItem (const Standard_CString name) const;
  
  //! Same as above, but <name> is given through a Handle
  //! Especially useful with methods SelectionNames, etc...
  Standard_EXPORT Handle(Standard_Transient) NamedItem (const Handle(TCollection_HAsciiString)& name) const;
  
  //! Returns the Ident attached to a Name, 0 if name not recorded
  Standard_EXPORT Standard_Integer NameIdent (const Standard_CString name) const;
  
  //! Returns True if an Item of the WorkSession has an attached Name
  Standard_EXPORT Standard_Boolean HasName (const Handle(Standard_Transient)& item) const;
  
  //! Returns the Name attached to an Item as a Variable of this
  //! WorkSession. If <item> is Null or not recorded, returns an
  //! empty string.
  Standard_EXPORT Handle(TCollection_HAsciiString) Name (const Handle(Standard_Transient)& item) const;
  
  //! Adds an Item and returns its attached Ident. Does nothing
  //! if <item> is already recorded (and returns its attached Ident)
  //! <active> if True commands call to SetActive (see below)
  //! Remark : the determined Ident is used if <item> is a Dispatch,
  //! to fill the ShareOut
  Standard_EXPORT Standard_Integer AddItem (const Handle(Standard_Transient)& item, const Standard_Boolean active = Standard_True);
  
  //! Adds an Item with an attached Name. If the Name is already
  //! known in the WorkSession, the older item losts it
  //! Returns Ident if Done, 0 else, i.e. if <item> is null
  //! If <name> is empty, works as AddItem (i.e. with no name)
  //! If <item> is already known but with no attached Name, this
  //! method tries to attached a Name to it
  //! <active> if True commands call to SetActive (see below)
  Standard_EXPORT Standard_Integer AddNamedItem (const Standard_CString name, const Handle(Standard_Transient)& item, const Standard_Boolean active = Standard_True);
  
  //! Following the type of <item> :
  //! - Dispatch : Adds or Removes it in the ShareOut & FileNaming
  //! - GeneralModifier : Adds or Removes it for final sending
  //! (i.e. in the ModelCopier)
  //! Returns True if it did something, False else (state unchanged)
  Standard_EXPORT Standard_Boolean SetActive (const Handle(Standard_Transient)& item, const Standard_Boolean mode);
  
  //! Removes an Item from the Session, given its Name
  //! Returns True if Done, False else (Name not recorded)
  //! (Applies only on Item which are Named)
  Standard_EXPORT Standard_Boolean RemoveNamedItem (const Standard_CString name);
  
  //! Removes a Name without removing the Item
  //! Returns True if Done, False else (Name not recorded)
  Standard_EXPORT Standard_Boolean RemoveName (const Standard_CString name);
  
  //! Removes an Item given its Ident. Returns False if <id> is
  //! attached to no Item in the WorkSession. For a Named Item,
  //! also removes its Name.
  Standard_EXPORT Standard_Boolean RemoveItem (const Handle(Standard_Transient)& item);
  
  //! Clears all the recorded Items : Selections, Dispatches,
  //! Modifiers, and Strings & IntParams, with their Idents & Names.
  //! Remark that if a Model has been loaded, it is not cleared.
  Standard_EXPORT void ClearItems();
  
  //! Returns a Label which illustrates the content of an Item,
  //! given its Ident. This Label is :
  //! - for a Text Parameter, "Text:<text value>"
  //! - for an Integer Parameter, "Integer:<integer value>"
  //! - for a Selection, a Dispatch or a Modifier, its Label
  //! (see these classes)
  //! - for any other kind of Variable, its cdl type
  Standard_EXPORT Handle(TCollection_HAsciiString) ItemLabel (const Standard_Integer id) const;
  
  //! Fills a Sequence with the List of Idents attached to the Items
  //! of which Type complies with (IsKind) <type> (alphabetic order)
  //! Remark : <type> = TYPE(Standard_Transient) gives all the
  //! Idents which are suitable in the WorkSession
  Standard_EXPORT Handle(TColStd_HSequenceOfInteger) ItemIdents (const Handle(Standard_Type)& type) const;
  
  //! Fills a Sequence with the list of the Names attached to Items
  //! of which Type complies with (IsKind) <type> (alphabetic order)
  //! Remark : <type> = TYPE(Standard_Transient) gives all the Names
  Standard_EXPORT Handle(TColStd_HSequenceOfHAsciiString) ItemNames (const Handle(Standard_Type)& type) const;
  
  //! Fills a Sequence with the NAMES of the control items, of which
  //! the label matches <label> (contain it) : see NextIdentForLabel
  //! Search mode is fixed to "contained"
  //! If <label> is empty, returns all Names
  Standard_EXPORT Handle(TColStd_HSequenceOfHAsciiString) ItemNamesForLabel (const Standard_CString label) const;
  
  //! For query by Label with possible iterations
  //! Searches the Ident of which Item has a Label which matches a
  //! given one, the search starts from an initial Ident.
  //! Returns the first found Ident which follows <id>, or ZERO
  //!
  //! The search must start with <id> = 0, it returns the next Ident
  //! which matches. To iterate, call again this method which this
  //! returned value as <id>. Once an Ident has been returned, the
  //! Item can be obtained by the method Item
  //!
  //! <mode> precises the required matching mode :
  //! - 0 (Default) : <label> must match exactly with the Item Label
  //! - 1 : <label> must match the exact beginning (the end is free)
  //! - 2 : <label> must be at least once wherever in the Item Label
  //! - other values are ignored
  Standard_EXPORT Standard_Integer NextIdentForLabel (const Standard_CString label, const Standard_Integer id, const Standard_Integer mode = 0) const;
  
  //! Creates a parameter as being bound to a Static
  //! If the Static is Integer, this creates an IntParam bound to
  //! it by its name. Else this creates a String which is the value
  //! of the Static.
  //! Returns a null handle if <statname> is unknown as a Static
  Standard_EXPORT Handle(Standard_Transient) NewParamFromStatic (const Standard_CString statname, const Standard_CString name = "");
  
  //! Returns an IntParam, given its Ident in the Session
  //! Null result if <id> is not suitable for an IntParam
  //! (undefined, or defined for another kind of variable)
  Standard_EXPORT Handle(IFSelect_IntParam) IntParam (const Standard_Integer id) const;
  
  //! Returns Integer Value of an IntParam
  Standard_EXPORT Standard_Integer IntValue (const Handle(IFSelect_IntParam)& it) const;
  
  //! Creates a new IntParam. A Name can be set (Optional)
  //! Returns the created IntParam, or a Null Handle in case of
  //! Failure (see AddItem/AddNamedItem)
  Standard_EXPORT Handle(IFSelect_IntParam) NewIntParam (const Standard_CString name = "");
  
  //! Changes the Integer Value of an IntParam
  //! Returns True if Done, False if <it> is not in the WorkSession
  Standard_EXPORT Standard_Boolean SetIntValue (const Handle(IFSelect_IntParam)& it, const Standard_Integer val);
  
  //! Returns a TextParam, given its Ident in the Session
  //! Null result if <id> is not suitable for a TextParam
  //! (undefined, or defined for another kind of variable)
  Standard_EXPORT Handle(TCollection_HAsciiString) TextParam (const Standard_Integer id) const;
  
  //! Returns Text Value of a TextParam (a String)
  //! or an empty string if <it> is not in the WorkSession
  Standard_EXPORT TCollection_AsciiString TextValue (const Handle(TCollection_HAsciiString)& par) const;
  
  //! Creates a new (empty) TextParam. A Name can be set (Optional)
  //! Returns the created TextParam (as an HAsciiString), or a Null
  //! Handle in case of Failure (see AddItem/AddNamedItem)
  Standard_EXPORT Handle(TCollection_HAsciiString) NewTextParam (const Standard_CString name = "");
  
  //! Changes the Text Value of a TextParam (an HAsciiString)
  //! Returns True if Done, False if <it> is not in the WorkSession
  Standard_EXPORT Standard_Boolean SetTextValue (const Handle(TCollection_HAsciiString)& par, const Standard_CString val);
  
  //! Returns a Signature, given its Ident in the Session
  //! Null result if <id> is not suitable for a Signature
  //! (undefined, or defined for another kind of variable)
  Standard_EXPORT Handle(IFSelect_Signature) Signature (const Standard_Integer id) const;
  
  //! Returns the Value computed by a Signature for an Entity
  //! Returns an empty string if the entity does not belong to the
  //! loaded model
  Standard_EXPORT Standard_CString SignValue (const Handle(IFSelect_Signature)& sign, const Handle(Standard_Transient)& ent) const;
  
  //! Returns a Selection, given its Ident in the Session
  //! Null result if <id> is not suitable for a Selection
  //! (undefined, or defined for another kind of variable)
  Standard_EXPORT Handle(IFSelect_Selection) Selection (const Standard_Integer id) const;
  
  //! Evaluates the effect of a Selection applied on the input Model
  //! Returned Result remains empty if no input Model has been set
  Standard_EXPORT Interface_EntityIterator EvalSelection (const Handle(IFSelect_Selection)& sel) const;
  
  //! Returns the Selections which are source of Selection, given
  //! its rank in the List of Selections (see SelectionIterator)
  //! Returned value is empty if <num> is out of range or if
  //! <sel> is not in the WorkSession
  Standard_EXPORT IFSelect_SelectionIterator Sources (const Handle(IFSelect_Selection)& sel) const;
  
  //! Returns the result of a Selection, computed by EvalSelection
  //! (see above) under the form of a HSequence (hence, it can be
  //! used by a frontal-engine logic). It can be empty
  //! Returns a Null Handle if <sel> is not in the WorkSession
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) SelectionResult (const Handle(IFSelect_Selection)& sel) const;
  
  //! Returns the result of a Selection, by forcing its input with
  //! a given list <list> (unless <list> is Null).
  //! RULES :
  //! <list> applies only for a SelectDeduct kind Selection :
  //! its Input is considered : if it is a SelectDeduct kind
  //! Selection, its Input is considered, etc... until an Input
  //! is not a Deduct/Extract : its result is replaced by <list>
  //! and all the chain of deductions is applied
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) SelectionResultFromList (const Handle(IFSelect_Selection)& sel, const Handle(TColStd_HSequenceOfTransient)& list) const;
  
  //! Sets a Selection as input for an item, according its type :
  //! if <item> is a Dispatch : as Final Selection
  //! if <item> is a GeneralModifier (i.e. any kind of Modifier) :
  //! as Selection used to filter entities to modify
  //! <sel>  Null  causes this Selection to be nullified
  //! Returns False if <item> is not of a suitable type, or
  //! <item> or <sel> is not in the WorkSession
  Standard_EXPORT Standard_Boolean SetItemSelection (const Handle(Standard_Transient)& item, const Handle(IFSelect_Selection)& sel);
  
  //! Resets input Selection which was set by SetItemSelection
  //! Same conditions as for SetItemSelection
  //! Returns True if done, False if <item> is not in the WorkSession
  Standard_EXPORT Standard_Boolean ResetItemSelection (const Handle(Standard_Transient)& item);
  
  //! Returns the Selection of a Dispatch or a GeneralModifier.
  //! Returns a Null Handle if none is defined or <item> not good type
  Standard_EXPORT Handle(IFSelect_Selection) ItemSelection (const Handle(Standard_Transient)& item) const;
  
  //! Returns a SignCounter from its ident in the Session
  //! Null result if <id> is not suitable for a SignCounter
  //! (undefined, or defined for another kind of variable)
  Standard_EXPORT Handle(IFSelect_SignCounter) SignCounter (const Standard_Integer id) const;
  
  //! Computes the content of a SignCounter when it is defined with
  //! a Selection, then returns True
  //! Returns False if the SignCounter is not defined with a
  //! Selection, or if its Selection Mode is inhibited
  //! <forced> to work around optimisations
  Standard_EXPORT Standard_Boolean ComputeCounter (const Handle(IFSelect_SignCounter)& counter, const Standard_Boolean forced = Standard_False);
  
  //! Computes the content of a SignCounter from an input list
  //! If <list> is Null, uses internal definition of the Counter :
  //! a Selection, else the whole Model (recomputation forced)
  //! If <clear> is True (D), starts from scratch
  //! Else, cumulates computations
  Standard_EXPORT Standard_Boolean ComputeCounterFromList (const Handle(IFSelect_SignCounter)& counter, const Handle(TColStd_HSequenceOfTransient)& list, const Standard_Boolean clear = Standard_True);
  
  //! Returns the ordered list of dispatches stored by the ShareOut
  Standard_EXPORT Handle(TColStd_HSequenceOfInteger) AppliedDispatches() const;
  
  //! Clears the list of Dispatches recorded by the ShareOut
  //! if <only> disp is True, tha's all. Else, clears also the lists
  //! of Modifiers recorded by the ShareOut
  Standard_EXPORT void ClearShareOut (const Standard_Boolean onlydisp);
  
  //! Returns a Dispatch, given its Ident in the Session
  //! Null result if <id> is not suitable for a Dispatch
  //! (undefined, or defined for another kind of variable)
  Standard_EXPORT Handle(IFSelect_Dispatch) Dispatch (const Standard_Integer id) const;
  
  //! Returns the rank of a Dispatch in the ShareOut, or 0 if <disp>
  //! is not in the ShareOut or not in the WorkSession
  Standard_EXPORT Standard_Integer DispatchRank (const Handle(IFSelect_Dispatch)& disp) const;
  
  //! Gives access to the complete ModelCopier
  const Handle(IFSelect_ModelCopier) & ModelCopier() const
  { return thecopier; }
  
  //! Sets a new ModelCopier. Fills Items which its content
  Standard_EXPORT void SetModelCopier (const Handle(IFSelect_ModelCopier)& copier);
  
  //! Returns the count of Modifiers applied to final sending
  //! Model Modifiers if <formodel> is True, File Modifiers else
  //! (i.e. Modifiers which apply once the Models have been filled)
  Standard_EXPORT Standard_Integer NbFinalModifiers (const Standard_Boolean formodel) const;
  
  //! Fills a Sequence with a list of Idents, those attached to
  //! the Modifiers applied to final sending.
  //! Model Modifiers if <formodel> is True, File Modifiers else
  //! This list is given in the order in which they will be applied
  //! (which takes into account the Changes to Modifier Ranks)
  Standard_EXPORT Handle(TColStd_HSequenceOfInteger) FinalModifierIdents (const Standard_Boolean formodel) const;
  
  //! Returns a Modifier, given its Ident in the Session
  //! Null result if <id> is not suitable for a Modifier
  //! (undefined, or defined for another kind of variable)
  Standard_EXPORT Handle(IFSelect_GeneralModifier) GeneralModifier (const Standard_Integer id) const;
  
  //! Returns a Model Modifier, given its Ident in the Session,
  //! i.e. typed as a Modifier (not simply a GeneralModifier)
  //! Null result if <id> is not suitable for a Modifier
  //! (undefined, or defined for another kind of variable)
  Standard_EXPORT Handle(IFSelect_Modifier) ModelModifier (const Standard_Integer id) const;
  
  //! Returns the Rank of a Modifier given its Ident. Model or File
  //! Modifier according its type (ModelModifier or not)
  //! Remember that Modifiers are applied sequentially following
  //! their Rank : first Model Modifiers then File Modifiers
  //! Rank is given by rank of call to AddItem and can be
  //! changed by ChangeModifierRank
  Standard_EXPORT Standard_Integer ModifierRank (const Handle(IFSelect_GeneralModifier)& item) const;
  
  //! Changes the Rank of a Modifier in the Session :
  //! Model Modifiers if <formodel> is True, File Modifiers else
  //! the Modifier n0 <before> is put to n0 <after>
  //! Return True if Done, False if <before> or <after> out of range
  Standard_EXPORT Standard_Boolean ChangeModifierRank (const Standard_Boolean formodel, const Standard_Integer before, const Standard_Integer after);
  
  //! Removes all the Modifiers active in the ModelCopier : they
  //! become inactive and they are removed from the Session
  Standard_EXPORT void ClearFinalModifiers();
  
  //! Sets a GeneralModifier to be applied to an item :
  //! - item = ShareOut : applies for final sending (all dispatches)
  //! - item is a Dispatch : applies for this dispatch only
  //! Returns True if done, False if <modif> or <item> not in <me>
  Standard_EXPORT Standard_Boolean SetAppliedModifier (const Handle(IFSelect_GeneralModifier)& modif, const Handle(Standard_Transient)& item);
  
  //! Resets a GeneralModifier to be applied
  //! Returns True if done, False if <modif> was not applied
  Standard_EXPORT Standard_Boolean ResetAppliedModifier (const Handle(IFSelect_GeneralModifier)& modif);
  
  //! Returns the item on which a GeneralModifier is applied :
  //! the ShareOut, or a given Dispatch
  //! Returns a Null Handle if <modif> is not applied
  Standard_EXPORT Handle(Standard_Transient) UsesAppliedModifier (const Handle(IFSelect_GeneralModifier)& modif) const;
  
  //! Returns a Transformer, given its Ident in the Session
  //! Null result if <id> is not suitable for a Transformer
  //! (undefined, or defined for another kind of variable)
  Standard_EXPORT Handle(IFSelect_Transformer) Transformer (const Standard_Integer id) const;
  
  //! Runs a Transformer on starting Model, which can then be edited
  //! or replaced by a new one. The Protocol can also be changed.
  //! Fills LastRunCheckList
  //!
  //! Returned status is 0 if nothing done (<transf> or model
  //! undefined), positive if OK, negative else :
  //! 0  : Nothing done
  //! 1  : OK, edition on the spot with no change to the graph
  //! of dependances (purely local)
  //! 2  : OK, model edited on the spot (graph recomputed, may
  //! have changed), protocol unchanged
  //! 3  : OK, new model produced, same protocol
  //! 4  : OK, model edited on the spot (graph recomputed),
  //! but protocol has changed
  //! 5  : OK, new model produced, protocol has changed
  //! -1 : Error on the spot (slight changes), data may be corrupted
  //! (remark : corruption should not be profound)
  //! -2 : Error on edition the spot, data may be corrupted
  //! (checking them is recommended)
  //! -3 : Error with a new data set, transformation ignored
  //! -4 : OK as 4, but graph of dependances count not be recomputed
  //! (the former one is kept) : check the protocol
  Standard_EXPORT Standard_Integer RunTransformer (const Handle(IFSelect_Transformer)& transf);
  
  //! Runs a Modifier on Starting Model. It can modify entities, or
  //! add new ones. But the Model or the Protocol is unchanged.
  //! The Modifier is applied on each entity of the Model. See also
  //! RunModifierSelected
  //! Fills LastRunCheckList
  //!
  //! <copy> : if True, a new data set is produced which brings
  //! the modifications (Model + its Entities)
  //! if False, data are modified on the spot
  //!
  //! It works through a TransformStandard defined with <modif>
  //! Returned status as RunTransformer : 0 nothing done, >0 OK,
  //! <0 problem, but only between -3 and 3 (protocol unchanged)
  //! Remark : <copy> True will give <effect> = 3 or -3
  Standard_EXPORT Standard_Integer RunModifier (const Handle(IFSelect_Modifier)& modif, const Standard_Boolean copy);
  
  //! Acts as RunModifier, but the Modifier is applied on the list
  //! determined by a Selection, rather than on the whole Model
  //! If the selection is a null handle, the whole model is taken
  Standard_EXPORT Standard_Integer RunModifierSelected (const Handle(IFSelect_Modifier)& modif, const Handle(IFSelect_Selection)& sel, const Standard_Boolean copy);
  
  //! Creates and returns a TransformStandard, empty, with its
  //! Copy Option (True = Copy, False = On the Spot) and an
  //! optional name.
  //! To a TransformStandard, the method SetAppliedModifier applies
  Standard_EXPORT Handle(IFSelect_Transformer) NewTransformStandard (const Standard_Boolean copy, const Standard_CString name = "");
  
  //! Defines a new content from the former one
  //! If <keep> is True, it is given by entities selected by
  //! Selection <sel>  (and all shared entities)
  //! Else, it is given by all the former content but entities
  //! selected by the Selection <sel> (and properly shared ones)
  //! Returns True if done. Returns False if the selected list
  //! (from <sel>) is empty, hence nothing is done
  Standard_EXPORT Standard_Boolean SetModelContent (const Handle(IFSelect_Selection)& sel, const Standard_Boolean keep);
  
  //! Returns the defined File Prefix. Null Handle if not defined
  Standard_EXPORT Handle(TCollection_HAsciiString) FilePrefix() const;
  
  //! Returns the defined Default File Root. It is used for
  //! Dispatches which have no specific root attached.
  //! Null Handle if not defined
  Standard_EXPORT Handle(TCollection_HAsciiString) DefaultFileRoot() const;
  
  //! Returns the defined File Extension. Null Handle if not defined
  Standard_EXPORT Handle(TCollection_HAsciiString) FileExtension() const;
  
  //! Returns the File Root defined for a Dispatch. Null if no
  //! Root Name is defined for it (hence, no File will be produced)
  Standard_EXPORT Handle(TCollection_HAsciiString) FileRoot (const Handle(IFSelect_Dispatch)& disp) const;
  
  //! Defines a File Prefix
  Standard_EXPORT void SetFilePrefix (const Standard_CString name);
  
  //! Defines a Default File Root Name. Clears it is <name> = ""
  //! Returns True if OK, False if <name> already set for a Dispatch
  Standard_EXPORT Standard_Boolean SetDefaultFileRoot (const Standard_CString name);
  
  //! Defines a File Extension
  Standard_EXPORT void SetFileExtension (const Standard_CString name);
  
  //! Defines a Root for a Dispatch
  //! If <name> is empty, clears Root Name
  //! This has as effect to inhibit the production of File by <disp>
  //! Returns False if <disp> is not in the WorkSession or if a
  //! root name is already defined for it
  Standard_EXPORT Standard_Boolean SetFileRoot (const Handle(IFSelect_Dispatch)& disp, const Standard_CString name);
  
  //! Extracts File Root Name from a given complete file name
  //! (uses OSD_Path)
  Standard_EXPORT Standard_CString GiveFileRoot (const Standard_CString file) const;
  
  //! Completes a file name as required, with Prefix and Extension
  //! (if defined; for a non-defined item, completes nothing)
  Standard_EXPORT Standard_CString GiveFileComplete (const Standard_CString file) const;
  
  //! Erases all stored data from the File Evaluation
  //! (i.e. ALL former naming information are lost)
  Standard_EXPORT void ClearFile();
  
  //! Performs and stores a File Evaluation. The Results are a List
  //! of produced Models and a List of names (Strings), in parallel
  //! Fills LastRunCheckList
  Standard_EXPORT void EvaluateFile();
  
  //! Returns the count of produced Models
  Standard_EXPORT Standard_Integer NbFiles() const;
  
  //! Returns a Model, given its rank in the Evaluation List
  Standard_EXPORT Handle(Interface_InterfaceModel) FileModel (const Standard_Integer num) const;
  
  //! Returns the name of a file corresponding to a produced Model,
  //! given its rank in the Evaluation List
  Standard_EXPORT TCollection_AsciiString FileName (const Standard_Integer num) const;
  
  //! Commands file sending to clear the list of already sent files,
  //! commands to record a new one if <record> is True
  //! This list is managed by the ModelCopier when SendSplit is called
  //! It allows a global exploitation of the set of sent files
  Standard_EXPORT void BeginSentFiles (const Standard_Boolean record);
  
  //! Returns the list of recorded sent files, or a Null Handle is
  //! recording has not been enabled
  Standard_EXPORT Handle(TColStd_HSequenceOfHAsciiString) SentFiles() const;
  
  //! Performs creation of derived files from the input Model
  //! Takes its data (sub-models and names), from result EvaluateFile
  //! if active, else by dynamic Evaluation (not stored)
  //! After SendSplit, result of EvaluateFile is Cleared
  //! Fills LastRunCheckList
  //!
  //! Works with the WorkLibrary which acts on specific type of Model
  //! and can work with File Modifiers (managed by the Model Copier)
  //! and a ModelCopier, which can work with Model Modifiers
  //! Returns False if, either WorkLibrary has failed on at least
  //! one sub-file, or the Work Session is badly conditioned
  //! (no Model defined, or FileNaming not in phase with ShareOut)
  Standard_EXPORT Standard_Boolean SendSplit();
  
  //! Returns an Evaluation of the whole ShareOut definition : i.e.
  //! how the entities of the starting model are forecast to be sent
  //! to various files :  list of packets according the dispatches,
  //! effective lists of roots for each packet (which determine the
  //! content of the corresponding file); plus evaluation of which
  //! entities are : forgotten (sent into no file), duplicated (sent
  //! into more than one file), sent into a given file.
  //! See the class PacketList for more details.
  Standard_EXPORT Handle(IFSelect_PacketList) EvalSplit() const;
  
  //! Returns the list of Entities sent in files, accourding the
  //! count of files each one has been sent (these counts are reset
  //! by SetModel or SetRemaining(Forget) ) stored in Graph Status
  //! <count> = -1 (default) is for ENtities sent at least once
  //! <count> = 0 is for the Remaining List (entities not yet sent)
  //! <count> = 1 is for entities sent in one and only one file
  //! (the ideal case)
  //! Remaining Data are computed on each Sending/Copying output
  //! files (see methods EvaluateFile and SendSplit)
  //! Graph Status is 0 for Remaining Entity, <count> for Sent into
  //! <count> files
  //! This status is set to 0 (not yet sent) for all by SetModel
  //! and by SetRemaining(mode=Forget,Display)
  Standard_EXPORT Interface_EntityIterator SentList (const Standard_Integer count = -1) const;
  
  //! Returns the greater count of different files in which any of
  //! the starting entities could be sent.
  //! Before any file output, this count is 0.
  //! Ideal count is 1. More than 1 means that duplications occur.
  Standard_EXPORT Standard_Integer MaxSendingCount() const;
  
  //! Processes Remaining data (after having sent files), mode :
  //! Forget  : forget remaining info (i.e. clear all "Sent" status)
  //! Compute : compute and keep remaining (does nothing if :
  //! remaining is empty or if no files has been sent)
  //! Display : display entities recorded as remaining
  //! Undo    : restore former state of data (after Remaining(1) )
  //! Returns True if OK, False else (i.e. mode = 2 and Remaining
  //! List is either empty or takes all the entities, or mode = 3
  //! and no former computation of remaining data was done)
  Standard_EXPORT Standard_Boolean SetRemaining (const IFSelect_RemainMode mode);
  
  //! Sends the starting Model into one file, without splitting,
  //! managing remaining data or anything else.
  //! <computegraph> true commands the Graph to be recomputed before
  //! sending : required when a Model is filled in several steps
  //!
  //! The Model and File Modifiers recorded to be applied on sending
  //! files are.
  //! Returns a status of execution :
  //! Done if OK,
  //! Void if no data available,
  //! Error if errors occurred (work library is not defined), errors during translation
  //! Fail if exception during translation is raised
  //! Stop if no disk space or disk, file is write protected
  //! Fills LastRunCheckList
  Standard_EXPORT IFSelect_ReturnStatus SendAll (const Standard_CString filename, const Standard_Boolean computegraph = Standard_False);
  
  //! Sends a part of the starting Model into one file, without
  //! splitting. But remaining data are managed.
  //! <computegraph> true commands the Graph to be recomputed before
  //! sending : required when a Model is filled in several steps
  //!
  //! The Model and File Modifiers recorded to be applied on sending
  //! files are.
  //! Returns a status : Done if OK,  Fail if error during send,
  //! Error : WorkLibrary not defined, Void : selection list empty
  //! Fills LastRunCheckList
  Standard_EXPORT IFSelect_ReturnStatus SendSelected (const Standard_CString filename, const Handle(IFSelect_Selection)& sel, const Standard_Boolean computegraph = Standard_False);
  
  //! Writes the current Interface Model globally to a File, and
  //! returns a write status which can be :
  //! Done OK, Fail file could not be written, Error no norm is selected
  //! Remark  : It is a simple, one-file writing, other operations are
  //! available (such as splitting ...) which calls SendAll
  Standard_EXPORT IFSelect_ReturnStatus WriteFile (const Standard_CString filename);
  
  //! Writes a sub-part of the current Interface Model to a File,
  //! as defined by a Selection <sel>, recomputes the Graph, and
  //! returns a write status which can be :
  //! Done OK, Fail file could not be written, Error no norm is selected
  //! Remark  : It is a simple, one-file writing, other operations are
  //! available (such as splitting ...) which calls SendSelected
  Standard_EXPORT IFSelect_ReturnStatus WriteFile (const Standard_CString filename, const Handle(IFSelect_Selection)& sel);
  
  //! Returns the count of Input Selections known for a Selection,
  //! or 0 if <sel> not in the WorkSession. This count is one for a
  //! SelectDeduct / SelectExtract kind, two for SelectControl kind,
  //! variable for a SelectCombine (Union/Intersection), zero else
  Standard_EXPORT Standard_Integer NbSources (const Handle(IFSelect_Selection)& sel) const;
  
  //! Returns the <num>th Input Selection of a Selection
  //! (see NbSources).
  //! Returns a Null Handle if <sel> is not in the WorkSession or if
  //! <num> is out of the range <1-NbSources>
  //! To obtain more details, see the method Sources
  Standard_EXPORT Handle(IFSelect_Selection) Source (const Handle(IFSelect_Selection)& sel, const Standard_Integer num = 1) const;
  
  //! Returns True if <sel> a Reversed SelectExtract, False else
  Standard_EXPORT Standard_Boolean IsReversedSelectExtract (const Handle(IFSelect_Selection)& sel) const;
  
  //! Toggles the Sense (Direct <-> Reversed) of a SelectExtract
  //! Returns True if Done, False if <sel> is not a SelectExtract or
  //! is not in the WorkSession
  Standard_EXPORT Standard_Boolean ToggleSelectExtract (const Handle(IFSelect_Selection)& sel);
  
  //! Sets an Input Selection (as <input>) to a SelectExtract or
  //! a SelectDeduct (as <sel>).
  //! Returns True if Done, False if <sel> is neither a
  //! SelectExtract nor a SelectDeduct, or not in the WorkSession
  Standard_EXPORT Standard_Boolean SetInputSelection (const Handle(IFSelect_Selection)& sel, const Handle(IFSelect_Selection)& input);
  
  //! Sets an Input Selection, Main if <formain> is True, Second else
  //! (as <sc>) to a SelectControl (as <sel>). Returns True if Done,
  //! False if <sel> is not a SelectControl, or <sc> or <sel> is not
  //! in the WorkSession
  Standard_EXPORT Standard_Boolean SetControl (const Handle(IFSelect_Selection)& sel, const Handle(IFSelect_Selection)& sc, const Standard_Boolean formain = Standard_True);
  
  //! Adds an input selection to a SelectCombine (Union or Inters.).
  //! Returns new count of inputs for this SelectCombine if Done or
  //! 0 if <sel> is not kind of SelectCombine, or if <seladd> or
  //! <sel> is not in the WorkSession
  //! By default, adding is done at the end of the list
  //! Else, it is an insertion to rank <atnum> (useful for Un-ReDo)
  Standard_EXPORT Standard_Integer CombineAdd (const Handle(IFSelect_Selection)& selcomb, const Handle(IFSelect_Selection)& seladd, const Standard_Integer atnum = 0);
  
  //! Removes an input selection from a SelectCombine (Union or
  //! Intersection). Returns True if done, False if <selcomb> is not
  //! kind of SelectCombine or <selrem> is not source of <selcomb>
  Standard_EXPORT Standard_Boolean CombineRemove (const Handle(IFSelect_Selection)& selcomb, const Handle(IFSelect_Selection)& selrem);
  
  //! Creates a new Selection, of type SelectPointed, its content
  //! starts with <list>. A name must be given (can be empty)
  Standard_EXPORT Handle(IFSelect_Selection) NewSelectPointed (const Handle(TColStd_HSequenceOfTransient)& list, const Standard_CString name);
  
  //! Changes the content of a Selection of type SelectPointed
  //! According <mode> : 0  set <list> as new content (clear former)
  //! 1  : adds <list> to actual content
  //! -1  : removes <list> from actual content
  //! Returns True if done, False if <sel> is not a SelectPointed
  Standard_EXPORT Standard_Boolean SetSelectPointed (const Handle(IFSelect_Selection)& sel, const Handle(TColStd_HSequenceOfTransient)& list, const Standard_Integer mode) const;
  
  //! Returns a Selection from a Name :
  //! - the name of a Selection : this Selection
  //! - the name of a Signature + criteria between (..) : a new
  //! Selection from this Signature
  //! - an entity or a list of entities : a new SelectPointed
  //! Else, returns a Null Handle
  Standard_EXPORT Handle(IFSelect_Selection) GiveSelection (const Standard_CString selname) const;
  
  //! Determines a list of entities from an object :
  //! <obj> already HSequenceOfTransient : returned itself
  //! <obj> Selection : its Result of Evaluation is returned
  //! <obj> an entity of the Model : a HSequence which contains it
  //! else, an empty HSequence
  //! <obj> the Model it self : ALL its content (not only the roots)
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) GiveList (const Handle(Standard_Transient)& obj) const;
  
  //! Computes a List of entities from two alphanums,
  //! first and second, as follows :
  //! if <first> is a Number or Label of an entity : this entity
  //! if <first> is a list of Numbers/Labels : the list of entities
  //! if <first> is the name of a Selection in <WS>, and <second>
  //! not defined, the standard result of this Selection
  //! else, let's consider "first second" : this whole phrase is
  //! split by blanks, as follows (RECURSIVE CALL) :
  //! - the leftest term is the final selection
  //! - the other terms define the result of the selection
  //! - and so on (the "leftest minus one" is a selection, of which
  //! the input is given by the remaining ...)
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) GiveList (const Standard_CString first, const Standard_CString second = "") const;
  
  //! Computes a List of entities from the model as follows
  //! <first> being a Selection or a combination of Selections,
  //! <ent> being an entity or a list
  //! of entities (as a HSequenceOfTransient) :
  //! the standard result of this selection applied to this list
  //! if <ent> is Null, the standard definition of the selection is
  //! used (which contains a default input selection)
  //! if <selname> is erroneous, a null handle is returned
  //!
  //! REMARK : selname is processed as <first second> of preceding
  //! GiveList
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) GiveListFromList (const Standard_CString selname, const Handle(Standard_Transient)& ent) const;
  
  //! Combines two lists and returns the result, according to mode :
  //! <mode> < 0 : entities in <l1> AND NOT in <l2>
  //! <mode> = 0 : entities in <l1> AND in <l2>
  //! <mode> > 0 : entities in <l1> OR  in <l2>
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) GiveListCombined (const Handle(TColStd_HSequenceOfTransient)& l1, const Handle(TColStd_HSequenceOfTransient)& l2, const Standard_Integer mode) const;
  
  //! Loads data from a check iterator to query status on it
  Standard_EXPORT void QueryCheckList (const Interface_CheckIterator& chl);
  
  //! Determines check status for an entity regarding last call to
  //! QueryCheckList :
  //! -1 : <ent> unknown in the model, ignored
  //! 0 : no check at all, immediate or inherited thru Graph
  //! 1 : immediate warning (no fail), no inherited check
  //! 2 : immediate fail, no inherited check
  //! +10 : idem but some inherited warning (no fail)
  //! +20 : idem but some inherited fail
  Standard_EXPORT Standard_Integer QueryCheckStatus (const Handle(Standard_Transient)& ent) const;
  
  //! Determines if <entdad> is parent of <entson> (in the graph),
  //! returns : -1 if no; 0 if <entdad> = <entson>
  //! 1 if immediate parent, > 1 if parent, gives count of steps
  Standard_EXPORT Standard_Integer QueryParent (const Handle(Standard_Transient)& entdad, const Handle(Standard_Transient)& entson) const;
  
  //! Sets a list of Parameters, i.e. TypedValue, to be handled
  //! through an Editor
  //! The two lists are parallel, if <params> is longer than <uses>,
  //! surnumeral parameters are for general use
  //!
  //! EditForms are created to handle these parameters (list, edit)
  //! on the basis of a ParamEditor  xst-params-edit
  //!
  //! A use number dispatches the parameter to a given EditForm
  //! EditForms are defined as follows
  //! Name                Use   Means
  //! xst-params          all   All Parameters (complete list)
  //! xst-params-general  1     Generals
  //! xst-params-load     2     LoadFile (no Transfer)
  //! xst-params-send     3     SendFile (Write, no Transfer)
  //! xst-params-split    4     Split
  //! xst-param-read      5     Transfer on Reading
  //! xst-param-write     6     Transfer on Writing
  Standard_EXPORT void SetParams (const NCollection_Vector<Handle(Standard_Transient)>& params, const NCollection_Vector<Standard_Integer>& uselist);
  
  //! Traces the Statics attached to a given use number
  //! If <use> is given positive (normal), the trace is embedded
  //! with a header and a trailer
  //! If <use> is negative, just values are printed
  //! (this allows to make compositions)
  //! Remark : use number  5 commands use -2 to be traced
  //! Remark : use numbers 4 and 6 command use -3 to be traced
  Standard_EXPORT void TraceStatics (const Standard_Integer use, const Standard_Integer mode = 0) const;
  
  //! Dumps contents of the ShareOut (on "cout")
  Standard_EXPORT void DumpShare() const;
  
  //! Lists the Labels of all Items of the WorkSession
  //! If <label> is defined, lists labels which contain it
  Standard_EXPORT void ListItems (const Standard_CString label = "") const;
  
  //! Lists the Modifiers of the session (for each one, displays
  //! its Label). Listing is done following Ranks (Modifiers are
  //! invoked following their ranks)
  //! Model Modifiers if <formodel> is True, File Modifiers else
  Standard_EXPORT void ListFinalModifiers (const Standard_Boolean formodel) const;
  
  //! Lists a Selection and its Sources (see SelectionIterator),
  //! given its rank in the list
  Standard_EXPORT void DumpSelection (const Handle(IFSelect_Selection)& sel) const;
  
  //! Lists the content of the Input Model (if there is one)
  //! According level : 0 -> gives only count of Entities and Roots
  //! 1 -> Lists also Roots;  2 -> Lists all Entities (by TraceType)
  //! 3 -> Performs a call to CheckList (Fails) and lists the result
  //! 4 -> as 3 but all CheckList (Fails + Warnings)
  //! 5,6,7  : as 3 but resp. Count,List,Labels by Fail
  //! 8,9,10 : as 4 but resp. Count,List,Labels by message
  Standard_EXPORT void DumpModel (const Standard_Integer level, Standard_OStream& S);
  
  //! Dumps the current Model (as inherited DumpModel), on currently
  //! defined Default Trace File (default is standard output)
  Standard_EXPORT void TraceDumpModel (const Standard_Integer mode);
  
  //! Dumps a starting entity according to the current norm.
  //! To do this, it calls DumpEntity from WorkLibrary.
  //! <level> is to be interpreted for each norm : see specific
  //! classes of WorkLibrary for it. Generally, 0 if for very basic
  //! (only type ...), greater values give more and more details.
  Standard_EXPORT void DumpEntity (const Handle(Standard_Transient)& ent, const Standard_Integer level, Standard_OStream& S) const;
  
  //! Prints main information about an entity : its number, type,
  //! validity (and checks if any), category, shareds and sharings..
  //! mutable because it can recompute checks as necessary
  Standard_EXPORT void PrintEntityStatus (const Handle(Standard_Transient)& ent, Standard_OStream& S);
  
  //! Dumps an entity from the current Model as inherited DumpEntity
  //! on currently defined Default Trace File
  //! (<level> interpreted according to the Norm, see WorkLibrary)
  Standard_EXPORT void TraceDumpEntity (const Handle(Standard_Transient)& ent, const Standard_Integer level) const;
  
  //! Prints a CheckIterator to the current Trace File, controlled
  //! with the current Model
  //! complete or fails only, according to <failsonly>
  //! <mode> defines the mode of printing
  //! 0 : sequential, according entities; else with a CheckCounter
  //! 1 : according messages, count of entities
  //! 2 : id but with list of entities, designated by their numbers
  //! 3 : as 2 but with labels of entities
  Standard_EXPORT void PrintCheckList (Standard_OStream& S,
                                       const Interface_CheckIterator& checklist,
                                       const Standard_Boolean failsonly,
                                       const IFSelect_PrintCount mode) const;
  
  //! Prints a SignatureList to the current Trace File, controlled
  //! with the current Model
  //! <mode> defines the mode of printing (see SignatureList)
  Standard_EXPORT void PrintSignatureList (Standard_OStream& S,
                                           const Handle(IFSelect_SignatureList)& signlist,
                                           const IFSelect_PrintCount mode) const;
  
  //! Displays the list of Entities selected by a Selection (i.e.
  //! the result of EvalSelection).
  Standard_EXPORT void EvaluateSelection (const Handle(IFSelect_Selection)& sel) const;
  
  //! Displays the result of applying a Dispatch on the input Model
  //! (also shows Remainder if there is)
  //! <mode> = 0 (default), displays nothing else
  //! <mode> = 1 : displays also duplicated entities (because of
  //! this dispatch)
  //! <mode> = 2 : displays the entities of the starting Model
  //! which are not taken by this dispatch (forgotten entities)
  //! <mode> = 3 : displays both duplicated and forgotten entities
  //! Remark : EvaluateComplete displays these data evaluated for
  //! for all the dispatches, if there are several
  Standard_EXPORT void EvaluateDispatch (const Handle(IFSelect_Dispatch)& disp, const Standard_Integer mode = 0) const;
  
  //! Displays the effect of applying the ShareOut on the input
  //! Model.
  //! <mode> = 0 (default) : displays only roots for each packet,
  //! <mode> = 1 : displays all entities for each packet, plus
  //! duplicated entities
  //! <mode> = 2 : same as <mode> = 1, plus displays forgotten
  //! entities (which are in no packet at all)
  Standard_EXPORT void EvaluateComplete (const Standard_Integer mode = 0) const;
  
  //! Internal method which displays an EntityIterator
  //! <mode> 0 gives short display (only entity numbers)
  //! 1 gives a more complete trace (1 line per Entity)
  //! (can be used each time a trace has to be output from a list)
  //! 2 gives a form suitable for givelist : (n1,n2,n3...)
  Standard_EXPORT void ListEntities (const Interface_EntityIterator& iter, const Standard_Integer mode, Standard_OStream& S) const;

  DEFINE_STANDARD_RTTIEXT(IFSelect_WorkSession,Standard_Transient)

 protected:

  Handle(Interface_HGraph) thegraph;
  Interface_CheckIterator thecheckrun;
  TColStd_IndexedDataMapOfTransientTransient theitems;
  NCollection_DataMap<TCollection_AsciiString, Handle(Standard_Transient)> thenames;

 private:

  Standard_Boolean theerrhand;
  Handle(IFSelect_ShareOut) theshareout;
  Handle(IFSelect_WorkLibrary) thelibrary;
  Handle(Interface_Protocol) theprotocol;
  Handle(Interface_InterfaceModel) myModel;
  TCollection_AsciiString theloaded;
  Handle(Interface_GTool) thegtool;
  Standard_Boolean thecheckdone;
  Interface_CheckIterator thechecklist;
  TCollection_AsciiString thecheckana;
  Handle(IFSelect_ModelCopier) thecopier;
  Handle(Interface_InterfaceModel) theoldel;
  Standard_Boolean themodelstat;
};

#endif // _IFSelect_WorkSession_HeaderFile
