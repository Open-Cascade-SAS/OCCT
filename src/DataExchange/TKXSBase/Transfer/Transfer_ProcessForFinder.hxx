// Created on: 1992-02-03
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

#ifndef _Transfer_ProcessForFinder_HeaderFile
#define _Transfer_ProcessForFinder_HeaderFile

#include <TColStd_IndexedMapOfInteger.hxx>
#include <Transfer_HSequenceOfFinder.hxx>
#include <Transfer_TransferMapOfProcessForFinder.hxx>
#include <Message_ProgressRange.hxx>

class Message_Messenger;
class Transfer_Finder;
class Transfer_Binder;
class Transfer_ActorOfProcessForFinder;
class Interface_InterfaceError;
class Transfer_TransferFailure;
class Transfer_FindHasher;
class Transfer_IteratorOfProcessForFinder;
class Message_Msg;
class Interface_Check;
class Standard_Transient;
class Interface_CheckIterator;

class Transfer_ProcessForFinder;
DEFINE_STANDARD_HANDLE(Transfer_ProcessForFinder, Standard_Transient)

class Transfer_ProcessForFinder : public Standard_Transient
{

public:
  //! Sets TransferProcess at initial state. Gives an Initial size
  //! (indicative) for the Map when known (default is 10000).
  //! Sets default trace file as a printer and default trace level
  //! (see Message_TraceFile).
  Standard_EXPORT Transfer_ProcessForFinder(const Standard_Integer nb = 10000);

  //! Sets TransferProcess at initial state. Gives an Initial size
  //! (indicative) for the Map when known (default is 10000).
  //! Sets a specified printer.
  Standard_EXPORT Transfer_ProcessForFinder(const Handle(Message_Messenger)& printer,
                                            const Standard_Integer           nb = 10000);

  //! Resets a TransferProcess as ready for a completely new work.
  //! Clears general data (roots) and the Map
  Standard_EXPORT void Clear();

  //! Rebuilds the Map and the roots to really remove Unbound items
  //! Because Unbind keeps the entity in place, even if not bound
  //! Hence, working by checking new items is meaningless if a
  //! formerly unbound item is rebound
  Standard_EXPORT void Clean();

  //! Resizes the Map as required (if a new reliable value has been
  //! determined). Acts only if <nb> is greater than actual NbMapped
  Standard_EXPORT void Resize(const Standard_Integer nb);

  //! Defines an Actor, which is used for automatic Transfer
  //! If already defined, the new Actor is cumulated
  //! (see SetNext from Actor)
  Standard_EXPORT void SetActor(const Handle(Transfer_ActorOfProcessForFinder)& actor);

  //! Returns the defined Actor. Returns a Null Handle if
  //! not set.
  Standard_EXPORT Handle(Transfer_ActorOfProcessForFinder) Actor() const;

  //! Returns the Binder which is linked with a starting Object
  //! It can either bring a Result (Transfer done) or none (for a
  //! pre-binding).
  //! If no Binder is linked with <start>, returns a Null Handle
  //! Considers a category number, by default 0
  Standard_EXPORT Handle(Transfer_Binder) Find(const Handle(Transfer_Finder)& start) const;

  //! Returns True if a Result (whatever its form) is Bound with
  //! a starting Object. I.e., if a Binder with a Result set,
  //! is linked with it
  //! Considers a category number, by default 0
  Standard_EXPORT Standard_Boolean IsBound(const Handle(Transfer_Finder)& start) const;

  //! Returns True if the result of the transfer of an object is
  //! already used in other ones. If it is, Rebind cannot change it.
  //! Considers a category number, by default 0
  Standard_EXPORT Standard_Boolean IsAlreadyUsed(const Handle(Transfer_Finder)& start) const;

  //! Creates a Link a starting Object with a Binder. This Binder
  //! can either bring a Result (effective Binding) or none (it can
  //! be set later : pre-binding).
  //! Considers a category number, by default 0
  Standard_EXPORT void Bind(const Handle(Transfer_Finder)& start,
                            const Handle(Transfer_Binder)& binder);

  //! Changes the Binder linked with a starting Object for its
  //! unitary transfer. This it can be useful when the exact form
  //! of the result is known once the transfer is widely engaged.
  //! This can be done only on first transfer.
  //! Considers a category number, by default 0
  Standard_EXPORT void Rebind(const Handle(Transfer_Finder)& start,
                              const Handle(Transfer_Binder)& binder);

  //! Removes the Binder linked with a starting object
  //! If this Binder brings a non-empty Check, it is replaced by
  //! a VoidBinder. Also removes from the list of Roots as required.
  //! Returns True if done, False if <start> was not bound
  //! Considers a category number, by default 0
  Standard_EXPORT Standard_Boolean Unbind(const Handle(Transfer_Finder)& start);

  //! Returns a Binder for a starting entity, as follows :
  //! Tries to Find the already bound one
  //! If none found, creates a VoidBinder and Binds it
  Standard_EXPORT Handle(Transfer_Binder) FindElseBind(const Handle(Transfer_Finder)& start);

  //! Sets Messenger used for outputting messages.
  Standard_EXPORT void SetMessenger(const Handle(Message_Messenger)& messenger);

  //! Returns Messenger used for outputting messages.
  //! The returned object is guaranteed to be non-null;
  //! default is Message::Messenger().
  Standard_EXPORT Handle(Message_Messenger) Messenger() const;

  //! Sets trace level used for outputting messages:
  //! <trace> = 0 : no trace at all
  //! <trace> = 1 : handled exceptions and calls to AddError
  //! <trace> = 2 : also calls to AddWarning
  //! <trace> = 3 : also traces new Roots
  //! (uses method ErrorTrace).
  //! Default is 1 : Errors traced
  Standard_EXPORT void SetTraceLevel(const Standard_Integer tracelev);

  //! Returns trace level used for outputting messages.
  Standard_EXPORT Standard_Integer TraceLevel() const;

  //! New name for AddFail (Msg)
  Standard_EXPORT void SendFail(const Handle(Transfer_Finder)& start, const Message_Msg& amsg);

  //! New name for AddWarning (Msg)
  Standard_EXPORT void SendWarning(const Handle(Transfer_Finder)& start, const Message_Msg& amsg);

  //! Adds an information message
  //! Trace is filled if trace level is at least 3
  Standard_EXPORT void SendMsg(const Handle(Transfer_Finder)& start, const Message_Msg& amsg);

  //! Adds an Error message to a starting entity (to the check of
  //! its Binder of category 0, as a Fail)
  Standard_EXPORT void AddFail(const Handle(Transfer_Finder)& start,
                               const Standard_CString         mess,
                               const Standard_CString         orig = "");

  //! (other name of AddFail, maintained for compatibility)
  Standard_EXPORT void AddError(const Handle(Transfer_Finder)& start,
                                const Standard_CString         mess,
                                const Standard_CString         orig = "");

  //! Adds an Error Message to a starting entity from the definition
  //! of a Msg (Original+Value)
  Standard_EXPORT void AddFail(const Handle(Transfer_Finder)& start, const Message_Msg& amsg);

  //! Adds a Warning message to a starting entity (to the check of
  //! its Binder of category 0)
  Standard_EXPORT void AddWarning(const Handle(Transfer_Finder)& start,
                                  const Standard_CString         mess,
                                  const Standard_CString         orig = "");

  //! Adds a Warning Message to a starting entity from the definition
  //! of a Msg (Original+Value)
  Standard_EXPORT void AddWarning(const Handle(Transfer_Finder)& start, const Message_Msg& amsg);

  Standard_EXPORT void Mend(const Handle(Transfer_Finder)& start, const Standard_CString pref = "");

  //! Returns the Check attached to a starting entity. If <start>
  //! is unknown, returns an empty Check
  //! Adds a case name to a starting entity
  //! Adds a case value to a starting entity
  //! Returns the complete case list for an entity. Null Handle if empty
  //! In the list of mapped items (between 1 and NbMapped),
  //! searches for the first item which follows <num0>(not included)
  //! and which has an attribute named <name>
  //! Attributes are brought by Binders
  //! Hence, allows such an iteration
  //!
  //! for (num = TP->NextItemWithAttribute(name,0);
  //! num > 0;
  //! num = TP->NextItemWithAttribute(name,num) {
  //! .. process mapped item <num>
  //! }
  //! Returns the type of an Attribute attached to binders
  //! If this name gives no Attribute, returns ParamVoid
  //! If this name gives several different types, returns ParamMisc
  //! Else, returns the effective type (ParamInteger, ParamReal,
  //! ParamIdent, or ParamText)
  //! Returns the list of recorded Attribute Names, as a Dictionary
  //! of Integer : each value gives the count of items which bring
  //! this attribute name
  //! By default, considers all the attribute names
  //! If <rootname> is given, considers only the attribute names
  //! which begin by <rootname>
  Standard_EXPORT Handle(Interface_Check) Check(const Handle(Transfer_Finder)& start) const;

  //! Binds a starting object with a Transient Result.
  //! Uses a SimpleBinderOfTransient to work. If there is already
  //! one but with no Result set, sets its Result.
  //! Considers a category number, by default 0
  Standard_EXPORT void BindTransient(const Handle(Transfer_Finder)&    start,
                                     const Handle(Standard_Transient)& res);

  //! Returns the Result of the Transfer of an object <start> as a
  //! Transient Result.
  //! Returns a Null Handle if there is no Transient Result
  //! Considers a category number, by default 0
  //! Warning : Supposes that Binding is done with a SimpleBinderOfTransient
  Standard_EXPORT const Handle(Standard_Transient)& FindTransient(
    const Handle(Transfer_Finder)& start) const;

  //! Prepares an object <start> to be bound with several results.
  //! If no Binder is yet attached to <obj>, a MultipleBinder
  //! is created, empty. If a Binder is already set, it must
  //! accept Multiple Binding.
  //! Considers a category number, by default 0
  Standard_EXPORT void BindMultiple(const Handle(Transfer_Finder)& start);

  //! Adds an item to a list of results bound to a starting object.
  //! Considers a category number, by default 0, for all results
  Standard_EXPORT void AddMultiple(const Handle(Transfer_Finder)&    start,
                                   const Handle(Standard_Transient)& res);

  //! Searches for a transient result attached to a starting object,
  //! according to its type, by criterium IsKind(atype)
  //!
  //! In case of multiple result, explores the list and gives in
  //! <val> the first transient result IsKind(atype)
  //! Returns True and fills <val> if found
  //! Else, returns False (<val> is not touched, not even nullified)
  //!
  //! This syntactic form avoids to do DownCast : if a result is
  //! found with the good type, it is loaded in <val> and can be
  //! immediately used, well initialised
  Standard_EXPORT Standard_Boolean FindTypedTransient(const Handle(Transfer_Finder)& start,
                                                      const Handle(Standard_Type)&   atype,
                                                      Handle(Standard_Transient)&    val) const;

  //! Safe variant for arbitrary type of argument
  template <class T>
  Standard_Boolean FindTypedTransient(const Handle(Transfer_Finder)& start,
                                      const Handle(Standard_Type)&   atype,
                                      Handle(T)&                     val) const
  {
    Handle(Standard_Transient) aVal = val;
    return FindTypedTransient(start, atype, aVal) && !(val = Handle(T)::DownCast(aVal)).IsNull();
  }

  //! Searches for a transient result recorded in a Binder, whatever
  //! this Binder is recorded or not in <me>
  //!
  //! This is strictly equivalent to the class method GetTypedResult
  //! from class SimpleBinderOfTransient, but is just lighter to call
  //!
  //! Apart from this, works as FindTypedTransient
  Standard_EXPORT Standard_Boolean GetTypedTransient(const Handle(Transfer_Binder)& binder,
                                                     const Handle(Standard_Type)&   atype,
                                                     Handle(Standard_Transient)&    val) const;

  //! Safe variant for arbitrary type of argument
  template <class T>
  Standard_Boolean GetTypedTransient(const Handle(Transfer_Binder)& start,
                                     const Handle(Standard_Type)&   atype,
                                     Handle(T)&                     val) const
  {
    Handle(Standard_Transient) aVal = val;
    return GetTypedTransient(start, atype, aVal) && !(val = Handle(T)::DownCast(aVal)).IsNull();
  }

  //! Returns the maximum possible value for Map Index
  //! (no result can be bound with a value greater than it)
  Standard_EXPORT Standard_Integer NbMapped() const;

  //! Returns the Starting Object bound to an Index,
  Standard_EXPORT const Handle(Transfer_Finder)& Mapped(const Standard_Integer num) const;

  //! Returns the Index value bound to a Starting Object, 0 if none
  Standard_EXPORT Standard_Integer MapIndex(const Handle(Transfer_Finder)& start) const;

  //! Returns the Binder bound to an Index
  //! Considers a category number, by default 0
  Standard_EXPORT Handle(Transfer_Binder) MapItem(const Standard_Integer num) const;

  //! Declares <obj> (and its Result) as Root. This status will be
  //! later exploited by RootResult, see below (Result can be
  //! produced at any time)
  Standard_EXPORT void SetRoot(const Handle(Transfer_Finder)& start);

  //! Enable (if <stat> True) or Disables (if <stat> False) Root
  //! Management. If it is set, Transfers are considered as stacked
  //! (a first Transfer commands other Transfers, and so on) and
  //! the Transfers commanded by an external caller are "Root".
  //! Remark : SetRoot can be called whatever this status, on every
  //! object.
  //! Default is set to True.
  Standard_EXPORT void SetRootManagement(const Standard_Boolean stat);

  //! Returns the count of recorded Roots
  Standard_EXPORT Standard_Integer NbRoots() const;

  //! Returns a Root Entity given its number in the list (1-NbRoots)
  Standard_EXPORT const Handle(Transfer_Finder)& Root(const Standard_Integer num) const;

  //! Returns the Binder bound with a Root Entity given its number
  //! Considers a category number, by default 0
  Standard_EXPORT Handle(Transfer_Binder) RootItem(const Standard_Integer num) const;

  //! Returns the index in the list of roots for a starting item,
  //! or 0 if it is not recorded as a root
  Standard_EXPORT Standard_Integer RootIndex(const Handle(Transfer_Finder)& start) const;

  //! Returns Nesting Level of Transfers (managed by methods
  //! TranscriptWith & Co). Starts to zero. If no automatic Transfer
  //! is used, it remains to zero. Zero means Root Level.
  Standard_EXPORT Standard_Integer NestingLevel() const;

  //! Resets Nesting Level of Transfers to Zero (Root Level),
  //! whatever its current value.
  Standard_EXPORT void ResetNestingLevel();

  //! Tells if <start> has been recognized as good candidate for
  //! Transfer. i.e. queries the Actor and its Nexts
  Standard_EXPORT Standard_Boolean Recognize(const Handle(Transfer_Finder)& start) const;

  //! Performs the Transfer of a Starting Object, by calling
  //! the method TransferProduct (see below).
  //! Mapping and Roots are managed : nothing is done if a Result is
  //! already Bound, an exception is raised in case of error.
  Standard_EXPORT Handle(Transfer_Binder) Transferring(
    const Handle(Transfer_Finder)& start,
    const Message_ProgressRange&   theProgress = Message_ProgressRange());

  //! Same as Transferring but does not return the Binder.
  //! Simply returns True in case of success (for user call)
  Standard_EXPORT Standard_Boolean
    Transfer(const Handle(Transfer_Finder)& start,
             const Message_ProgressRange&   theProgress = Message_ProgressRange());

  //! Allows controls if exceptions will be handled
  //! Transfer Operations
  //! <err> False : they are not handled with try {} catch {}
  //! <err> True  : they are
  //! Default is False: no handling performed
  Standard_EXPORT void SetErrorHandle(const Standard_Boolean err);

  //! Returns error handling flag
  Standard_EXPORT Standard_Boolean ErrorHandle() const;

  //! Method called when trace is asked
  //! Calls PrintTrace to display information relevant for starting
  //! objects (which can be redefined)
  //! <level> is Nesting Level of Transfer (0 = root)
  //! <mode> controls the way the trace is done :
  //! 0 neutral, 1 for Error, 2 for Warning message, 3 for new Root
  Standard_EXPORT void StartTrace(const Handle(Transfer_Binder)& binder,
                                  const Handle(Transfer_Finder)& start,
                                  const Standard_Integer         level,
                                  const Standard_Integer         mode) const;

  //! Prints a short information on a starting object. By default
  //! prints its Dynamic Type. Can be redefined
  Standard_EXPORT virtual void PrintTrace(const Handle(Transfer_Finder)& start,
                                          Standard_OStream&              S) const;

  //! Returns True if we are surely in a DeadLoop. Evaluation is not
  //! exact, it is a "majorant" which must be computed fast.
  //! This "majorant" is : <alevel> greater than NbMapped.
  Standard_EXPORT Standard_Boolean IsLooping(const Standard_Integer alevel) const;

  //! Returns, as an iterator, the log of root transfer, i.e. the
  //! created objects and Binders bound to starting roots
  //! If withstart is given True, Starting Objects are also returned
  Standard_EXPORT Transfer_IteratorOfProcessForFinder
    RootResult(const Standard_Boolean withstart = Standard_False) const;

  //! Returns, as an Iterator, the entire log of transfer (list of
  //! created objects and Binders which can bring errors)
  //! If withstart is given True, Starting Objects are also returned
  Standard_EXPORT Transfer_IteratorOfProcessForFinder
    CompleteResult(const Standard_Boolean withstart = Standard_False) const;

  //! Returns Binders which are neither "Done" nor "Initial",
  //! that is Error,Loop or Run (abnormal states at end of Transfer)
  //! Starting Objects are given in correspondence in the iterator
  Standard_EXPORT Transfer_IteratorOfProcessForFinder AbnormalResult() const;

  //! Returns a CheckList as a list of Check : each one is for a
  //! starting entity which have either check (warning or fail)
  //! messages are attached, or are in abnormal state : that case
  //! gives a specific message
  //! If <erronly> is True, checks with Warnings only are ignored
  Standard_EXPORT Interface_CheckIterator CheckList(const Standard_Boolean erronly) const;

  //! Returns, as an Iterator, the log of transfer for one object
  //! <level> = 0 : this object only
  //! and if <start> is a scope owner (else, <level> is ignored) :
  //! <level> = 1 : object plus its immediate scoped ones
  //! <level> = 2 : object plus all its scoped ones
  Standard_EXPORT Transfer_IteratorOfProcessForFinder
    ResultOne(const Handle(Transfer_Finder)& start,
              const Standard_Integer         level,
              const Standard_Boolean         withstart = Standard_False) const;

  //! Returns a CheckList for one starting object
  //! <level> interpreted as by ResultOne
  //! If <erronly> is True, checks with Warnings only are ignored
  Standard_EXPORT Interface_CheckIterator CheckListOne(const Handle(Transfer_Finder)& start,
                                                       const Standard_Integer         level,
                                                       const Standard_Boolean erronly) const;

  //! Returns True if no check message is attached to a starting
  //! object. <level> interpreted as by ResultOne
  //! If <erronly> is True, checks with Warnings only are ignored
  Standard_EXPORT Standard_Boolean IsCheckListEmpty(const Handle(Transfer_Finder)& start,
                                                    const Standard_Integer         level,
                                                    const Standard_Boolean         erronly) const;

  //! Removes Results attached to (== Unbinds) a given object and,
  //! according <level> :
  //! <level> = 0 : only it
  //! <level> = 1 : it plus its immediately owned sub-results(scope)
  //! <level> = 2 : it plus all its owned sub-results(scope)
  Standard_EXPORT void RemoveResult(const Handle(Transfer_Finder)& start,
                                    const Standard_Integer         level,
                                    const Standard_Boolean         compute = Standard_True);

  //! Computes a number to be associated to a starting object in
  //! a check or a check-list
  //! By default, returns 0; can be redefined
  Standard_EXPORT virtual Standard_Integer CheckNum(const Handle(Transfer_Finder)& start) const;

  DEFINE_STANDARD_RTTI_INLINE(Transfer_ProcessForFinder, Standard_Transient)

protected:
private:
  //! Same as Find but stores the last access to the map, for a
  //! faster access on next calls (as Bind does too)
  //! Considers a category number, by default 0
  //! C++ : return const &
  Standard_EXPORT Handle(Transfer_Binder) FindAndMask(const Handle(Transfer_Finder)& start);

  //! Internal action of Transfer, called by Transferring, with or
  //! without ErrorHandle. It invokes the Actor to work (set by
  //! SetActor), and tries its Nexts if no result is produced,
  //! until a Non Null Binder is produced.
  //! But keep in mind that a Null Binder can always be returned
  //! if a Starting Entity has not been recognized at all.
  Standard_EXPORT Handle(Transfer_Binder) TransferProduct(
    const Handle(Transfer_Finder)& start,
    const Message_ProgressRange&   theProgress = Message_ProgressRange());

  Standard_Boolean                         theerrh;
  Standard_Integer                         thetrace;
  Handle(Message_Messenger)                themessenger;
  Standard_Integer                         thelevel;
  Standard_Integer                         therootl;
  Standard_Boolean                         therootm;
  TColStd_IndexedMapOfInteger              theroots;
  Handle(Transfer_Finder)                  thelastobj;
  Handle(Transfer_Binder)                  thelastbnd;
  Standard_Integer                         theindex;
  Handle(Transfer_ActorOfProcessForFinder) theactor;
  Transfer_TransferMapOfProcessForFinder   themap;
};

#endif // _Transfer_ProcessForFinder_HeaderFile
