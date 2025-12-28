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

#ifndef _Interface_Check_HeaderFile
#define _Interface_Check_HeaderFile

#include <Standard.hxx>

#include <TCollection_HAsciiString.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
#include <Interface_CheckStatus.hxx>
class Message_Msg;
class TCollection_HAsciiString;

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

//! Defines a Check, as a list of Fail or Warning Messages under
//! a literal form, which can be empty. A Check can also bring an
//! Entity, which is the Entity to which the messages apply
//! (this Entity may be any Transient Object).
//!
//! Messages can be stored in two forms : the definitive form
//! (the only one by default), and another form, the original
//! form, which can be different if it contains values to be
//! inserted (integers, reals, strings)
//! The original form can be more suitable for some operations
//! such as counting messages
class Interface_Check : public Standard_Transient
{

public:
  //! Allows definition of a Sequence. Used also for Global Check
  //! of an InterfaceModel (which stores global messages for file)
  Standard_EXPORT Interface_Check();

  //! Defines a Check on an Entity
  Standard_EXPORT Interface_Check(const occ::handle<Standard_Transient>& anentity);

  //! New name for AddFail (Msg)
  Standard_EXPORT void SendFail(const Message_Msg& amsg);

  //! Records a new Fail message
  Standard_EXPORT void AddFail(const occ::handle<TCollection_HAsciiString>& amess);

  //! Records a new Fail message under two forms : final,original
  Standard_EXPORT void AddFail(const occ::handle<TCollection_HAsciiString>& amess,
                               const occ::handle<TCollection_HAsciiString>& orig);

  //! Records a new Fail message given as "error text" directly
  //! If <orig> is given, a distinct original form is recorded
  //! else (D), the original form equates <amess>
  Standard_EXPORT void AddFail(const char* amess, const char* orig = "");

  //! Records a new Fail from the definition of a Msg (Original+Value)
  Standard_EXPORT void AddFail(const Message_Msg& amsg);

  //! Returns True if Check brings at least one Fail Message
  Standard_EXPORT bool HasFailed() const;

  //! Returns count of recorded Fails
  Standard_EXPORT int NbFails() const;

  //! Returns Fail Message as a String
  //! Final form by default, Original form if <final> is False
  Standard_EXPORT const occ::handle<TCollection_HAsciiString>& Fail(
    const int num,
    const bool final = true) const;

  //! Same as above, but returns a CString (to be printed ...)
  //! Final form by default, Original form if <final> is False
  Standard_EXPORT const char* CFail(const int num,
                                         const bool final = true) const;

  //! Returns the list of Fails, for a frontal-engine logic
  //! Final forms by default, Original forms if <final> is False
  //! Can be empty
  Standard_EXPORT occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> Fails(
    const bool final = true) const;

  //! New name for AddWarning
  Standard_EXPORT void SendWarning(const Message_Msg& amsg);

  //! Records a new Warning message
  Standard_EXPORT void AddWarning(const occ::handle<TCollection_HAsciiString>& amess);

  //! Records a new Warning message under two forms : final,original
  Standard_EXPORT void AddWarning(const occ::handle<TCollection_HAsciiString>& amess,
                                  const occ::handle<TCollection_HAsciiString>& orig);

  //! Records a Warning message given as "warning message" directly
  //! If <orig> is given, a distinct original form is recorded
  //! else (D), the original form equates <amess>
  Standard_EXPORT void AddWarning(const char* amess, const char* orig = "");

  //! Records a new Warning from the definition of a Msg (Original+Value)
  Standard_EXPORT void AddWarning(const Message_Msg& amsg);

  //! Returns True if Check brings at least one Warning Message
  Standard_EXPORT bool HasWarnings() const;

  //! Returns count of recorded Warning messages
  Standard_EXPORT int NbWarnings() const;

  //! Returns Warning message as a String
  //! Final form by default, Original form if <final> is False
  Standard_EXPORT const occ::handle<TCollection_HAsciiString>& Warning(
    const int num,
    const bool final = true) const;

  //! Same as above, but returns a CString (to be printed ...)
  //! Final form by default, Original form if <final> is False
  Standard_EXPORT const char* CWarning(const int num,
                                            const bool final = true) const;

  //! Returns the list of Warnings, for a frontal-engine logic
  //! Final forms by default, Original forms if <final> is False
  //! Can be empty
  Standard_EXPORT occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> Warnings(
    const bool final = true) const;

  //! Records an information message
  //! This does not change the status of the Check
  Standard_EXPORT void SendMsg(const Message_Msg& amsg);

  //! Returns the count of recorded information messages
  Standard_EXPORT int NbInfoMsgs() const;

  //! Returns information message as a String
  Standard_EXPORT const occ::handle<TCollection_HAsciiString>& InfoMsg(
    const int num,
    const bool final = true) const;

  //! Same as above, but returns a CString (to be printed ...)
  //! Final form by default, Original form if <final> is False
  Standard_EXPORT const char* CInfoMsg(const int num,
                                            const bool final = true) const;

  //! Returns the list of Info Msg, for a frontal-engine logic
  //! Final forms by default, Original forms if <final> is False
  //! Can be empty
  Standard_EXPORT occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> InfoMsgs(
    const bool final = true) const;

  //! Returns the Check Status : OK, Warning or Fail
  Standard_EXPORT Interface_CheckStatus Status() const;

  //! Tells if Check Status complies with a given one
  //! (i.e. also status for query)
  Standard_EXPORT bool Complies(const Interface_CheckStatus status) const;

  //! Tells if a message is brought by a Check, as follows :
  //! <incl> = 0 : <mess> exactly matches one of the messages
  //! <incl> < 0 : <mess> is contained by one of the messages
  //! <incl> > 0 : <mess> contains one of the messages
  //! For <status> : for CheckWarning and CheckFail, considers only
  //! resp. Warning or Check messages. for CheckAny, considers all
  //! other values are ignored (answer will be false)
  Standard_EXPORT bool Complies(const occ::handle<TCollection_HAsciiString>& mess,
                                            const int                  incl,
                                            const Interface_CheckStatus             status) const;

  //! Returns True if a Check is devoted to an entity; else, it is
  //! global (for InterfaceModel's storing of global error messages)
  Standard_EXPORT bool HasEntity() const;

  //! Returns the entity on which the Check has been defined
  Standard_EXPORT const occ::handle<Standard_Transient>& Entity() const;

  //! Clears a check, in order to receive information from transfer
  //! (Messages and Entity)
  Standard_EXPORT void Clear();

  //! Clears the Fail Messages (for instance to keep only Warnings)
  Standard_EXPORT void ClearFails();

  //! Clears the Warning Messages (for instance to keep only Fails)
  Standard_EXPORT void ClearWarnings();

  //! Clears the Info Messages
  Standard_EXPORT void ClearInfoMsgs();

  //! Removes the messages which comply with <mess>, as follows :
  //! <incl> = 0 : <mess> exactly matches one of the messages
  //! <incl> < 0 : <mess> is contained by one of the messages
  //! <incl> > 0 : <mess> contains one of the messages
  //! For <status> : for CheckWarning and CheckFail, considers only
  //! resp. Warning or Check messages. for CheckAny, considers all
  //! other values are ignored (nothing is done)
  //! Returns True if at least one message has been removed, False else
  Standard_EXPORT bool Remove(const occ::handle<TCollection_HAsciiString>& mess,
                                          const int                  incl,
                                          const Interface_CheckStatus             status);

  //! Mends messages, according <pref> and <num>
  //! According to <num>, works on the whole list of Fails if = 0(D)
  //! or only one Fail message, given its rank
  //! If <pref> is empty, converts Fail(s) to Warning(s)
  //! Else, does the conversion but prefixes the new Warning(s) but
  //! <pref> followed by a semi-column
  //! Some reserved values of <pref> are :
  //! "FM" : standard prefix "Mended" (can be translated)
  //! "CF" : clears Fail(s)
  //! "CW" : clears Warning(s) : here, <num> refers to Warning list
  //! "CA" : clears all messages : here, <num> is ignored
  Standard_EXPORT bool Mend(const char* pref,
                                        const int num = 0);

  //! Receives an entity result of a Transfer
  Standard_EXPORT void SetEntity(const occ::handle<Standard_Transient>& anentity);

  //! same as SetEntity (old form kept for compatibility)
  //! Warning : Does nothing if Entity field is not yet clear
  Standard_EXPORT void GetEntity(const occ::handle<Standard_Transient>& anentity);

  //! Copies messages stored in another Check, cumulating
  //! Does not regard other's Entity. Used to cumulate messages
  Standard_EXPORT void GetMessages(const occ::handle<Interface_Check>& other);

  //! Copies messages converted into Warning messages
  //! If failsonly is true, only Fails are taken, and converted
  //! else, Warnings are taken too. Does not regard Entity
  //! Used to keep Fail messages as Warning, after a recovery
  Standard_EXPORT void GetAsWarning(const occ::handle<Interface_Check>& other,
                                    const bool         failsonly);

  //! Prints the messages of the check to an Messenger
  //! <level> = 1 : only fails
  //! <level> = 2 : fails and warnings
  //! <level> = 3 : all (fails, warnings, info msg)
  //! <final> : if positive (D) prints final values of messages
  //! if negative, prints originals
  //! if null, prints both forms
  Standard_EXPORT void Print(Standard_OStream&      S,
                             const int level,
                             const int final = 1) const;

  //! Prints the messages of the check to the default trace file
  //! By default, according to the default standard level
  //! Else, according level (see method Print)
  Standard_EXPORT void Trace(const int level = -1,
                             const int final = 1) const;

  DEFINE_STANDARD_RTTIEXT(Interface_Check, Standard_Transient)

private:
  occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> thefails;
  occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> thefailo;
  occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> thewarns;
  occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> thewarno;
  occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> theinfos;
  occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> theinfoo;
  occ::handle<Standard_Transient>              theent;
};

#endif // _Interface_Check_HeaderFile
