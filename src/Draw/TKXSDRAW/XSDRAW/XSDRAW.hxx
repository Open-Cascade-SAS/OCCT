// Created on: 1995-03-14
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

#ifndef _XSDRAW_HeaderFile
#define _XSDRAW_HeaderFile

#include <Standard.hxx>
#include <Standard_Handle.hxx>

#include <Draw_Interpretor.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
#include <XSControl_WorkSession.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>
class IFSelect_SessionPilot;
class XSControl_WorkSession;
class XSControl_Controller;
class Interface_Protocol;
class Interface_InterfaceModel;
class Standard_Transient;
class Transfer_TransientProcess;
class Transfer_FinderProcess;
class XSControl_TransferReader;
class TCollection_AsciiString;
class TDocStd_Document;

//! Basic package to work functions of X-STEP (IFSelect & Co)
//! under control of DRAW
//!
//! Works with some "static" data : a SessionPilot (used to run)
//! with its WorkSession and Model and TransferReader, a
//! FinderProcess
class XSDRAW
{
public:
  //! Takes variables to/from the DRAW session
  //! Implements ProgressIndicator for DRAW
  //! Changes the name under which a command of xstep is known by
  //! Draw. This allows to avoid collisions
  //! To be called before LoadDraw or any other xstep initialisation
  Standard_EXPORT static void ChangeCommand(const char* const oldname,
                                            const char* const newname);

  //! Removes a command from the interpretation list of Draw
  //! To be called before LoadDraw or any other xstep initialisation
  Standard_EXPORT static void RemoveCommand(const char* const oldname);

  //! Defines the basic context to work with a X-STEP Session :
  //! it performs the basic inits, also records the Controller
  //! If the Controller is not yet set, it must be set after
  //! (call to SetController)
  //! LoadSession is called by LoadDraw
  //! Returns True the first time, False if already called
  Standard_EXPORT static bool LoadSession();

  //! Defines the context for using a X-STEP Session under DRAW
  //! Once the various INITs have been done, a call to LoadDraw
  //! records the defined commands for the X-STEP SessionPilot,
  //! into the DRAW interpretation list.
  //! "Set" commands are accessed under command xset
  //! SDS>xset name command ...
  //! Other commands can be accessed directly or under command xstep
  //! SDS>command ... and SDS>xstep command ... are equivalent
  //!
  //! Only the command xinit is accessed directly only:
  //! SDS>xinit (from the already defined Controller)
  //! SDS>xinit iges (first defines the Controller as for "iges")
  //!
  //! It also records the function to be called by DRAW (not
  //! declared because specific).
  //! And it defines the context variables, i.e. a WorkSession, then
  //! it calls SetController with the currently defined Controller
  //! Remark : at least, the standard commands are recorded
  //!
  //! See also Controller : it is part of the Context, but it must
  //! be precised separately
  Standard_EXPORT static void LoadDraw(Draw_Interpretor& theCommands);

  //! Allows to execute a xstep-draw command from C++ program
  //! Fixed form: Execute("command args...");
  //! Form with a variable text part : add %s for the variable :
  //! Execute ("command args %s args..",var) [var is a CString]
  //! Returns the same value as returned by call from DRAW
  Standard_EXPORT static int Execute(const char* const command,
                                                  const char* const var = "");

  //! Returns the SessionPilot (can be used for direct call)
  Standard_EXPORT static occ::handle<IFSelect_SessionPilot> Pilot();

  //! Updates the WorkSession defined in AddDraw (through Pilot)
  //! It is from XSControl, it brings functionalities for Transfers
  Standard_EXPORT static void SetSession(const occ::handle<XSControl_WorkSession>& theSession);

  //! Returns the WorkSession defined in AddDraw (through Pilot)
  //! It is from XSControl, it brings functionalities for Transfers
  Standard_EXPORT static const occ::handle<XSControl_WorkSession> Session();

  //! Defines a Controller for the command "xinit" and applies it
  //! (i.e. calls its method Customise)
  Standard_EXPORT static void SetController(const occ::handle<XSControl_Controller>& control);

  //! Returns the Controller, a Null Handle if not yet defined
  Standard_EXPORT static occ::handle<XSControl_Controller> Controller();

  //! Sets a norm by its name (controller recorded as <normname> )
  //! Returns True if done, False if this norm is unknown
  Standard_EXPORT static bool SetNorm(const char* const normname);

  //! Returns the actually defined Protocol
  Standard_EXPORT static occ::handle<Interface_Protocol> Protocol();

  //! Returns the Model of the Session (it is Session()->Model() )
  Standard_EXPORT static occ::handle<Interface_InterfaceModel> Model();

  //! Sets a Model in session (it is Session()->SetModel(model) )
  //! If <file> is defined, SetLoadedFile is also done
  Standard_EXPORT static void SetModel(const occ::handle<Interface_InterfaceModel>& model,
                                       const char* const                  file = "");

  //! Produces a new model (from the Controller), can be Null
  //! Does not set it in the session
  Standard_EXPORT static occ::handle<Interface_InterfaceModel> NewModel();

  //! Returns the entity n0 <num> of the Model of the Session
  //! (it is StartingEntity)
  //! Null Handle if <num> is not suitable
  Standard_EXPORT static occ::handle<Standard_Transient> Entity(const int num);

  //! Returns the number of an entity in the Model (StartingNumber)
  //! 0 if <ent> unknown in the model, or null
  Standard_EXPORT static int Number(const occ::handle<Standard_Transient>& ent);

  //! Sets a TransferProcess in order to analyse it (see Activator)
  //! It can be either a FinderProcess or a TransientProcess, in
  //! that case a new TransferReader is created on it
  Standard_EXPORT static void SetTransferProcess(const occ::handle<Standard_Transient>& TP);

  //! Returns the TransferProcess : TransientProcess detained by
  //! the TransferReader
  Standard_EXPORT static occ::handle<Transfer_TransientProcess> TransientProcess();

  //! Returns the FinderProcess, detained by the TransferWriter
  Standard_EXPORT static occ::handle<Transfer_FinderProcess> FinderProcess();

  //! Initialises a TransferReader, according to mode:
  //! 0 nullifies it, 1 clears it (not nullify)
  //! 2 sets it with TransientProcess & Model
  //! 3 idem plus roots of TransientProcess
  //! Remark : called with 0 at least at each SetModel/NewModel
  Standard_EXPORT static void InitTransferReader(const int mode);

  //! Returns the current TransferReader, can be null
  //! It detains the TransientProcess
  Standard_EXPORT static occ::handle<XSControl_TransferReader> TransferReader();

  //! Takes the name of an entity, either as argument, or (if <name>
  //! is empty) on keyboard, and returns the entity
  //! name can be a label or a number (in alphanumeric), it is
  //! searched by NumberFromLabel from WorkSession.
  //! If <name> doesn't match en entity, a Null Handle is returned
  Standard_EXPORT static occ::handle<Standard_Transient> GetEntity(const char* const name = "");

  //! Same as GetEntity, but returns the number in the model of the
  //! entity. Returns 0 for null handle
  Standard_EXPORT static int GetEntityNumber(const char* const name = "");

  //! Evaluates and returns a list of entity, from :
  //! keyboard if <first> and <second> are empty, see below
  //! first if second is empty : can be a number/label of an entity
  //! or the name of a selection to be evaluated (standard)
  //! first : name of a selection, evaluated from a list defined by
  //! second
  //! In case of failure, returns a Null Handle
  Standard_EXPORT static occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> GetList(
    const char* const first  = "",
    const char* const second = "");

  //! Analyses given file name and variable name, with a default
  //! name for variables. Returns resulting file name and variable
  //! name plus status "file to read"(True) or "already read"(False)
  //! In the latter case, empty resfile means no file available
  //!
  //! If <file> is null or empty or equates ".", considers Session
  //! and returned status is False
  //! Else, returns resfile = file and status is True
  //! If <var> is neither null nor empty, resvar = var
  //! Else, the root part of <resfile> is considered, if defined
  //! Else, <def> is taken
  Standard_EXPORT static bool FileAndVar(const char* const   file,
                                                     const char* const   var,
                                                     const char* const   def,
                                                     TCollection_AsciiString& resfile,
                                                     TCollection_AsciiString& resvar);

  //! Analyses a name as designating Shapes from DRAW variables or
  //! XSTEP transfer (last Transfer on Reading). <name> can be :
  //! "*" : all the root shapes produced by last Transfer (Read)
  //! i.e. considers roots of the TransientProcess
  //! a name : a name of a variable DRAW
  //!
  //! Returns the count of designated Shapes. Their list is put in
  //! <list>. If <list> is null, it is firstly created. Then it is
  //! completed (Append without Clear) by the Shapes found
  //! Returns 0 if no Shape could be found
  Standard_EXPORT static int MoreShapes(occ::handle<NCollection_HSequence<TopoDS_Shape>>& list,
                                                     const char* const             name);

  //! Extracts length unit from the static interface or document.
  //! Document unit has the highest priority.
  //! @return length unit in MM. 1.0 by default
  Standard_EXPORT static double GetLengthUnit(
    const occ::handle<TDocStd_Document>& theDoc = nullptr);

  //! Returns available work sessions with their associated files.
  Standard_EXPORT static XSControl_WorkSessionMap& WorkSessionList();

  //! Binds session and name into map recursively.
  //! Recursively means extract sub-sessions from main session.
  //! @param[in] theWS the session object
  //! @param[in] theName the session file name
  //! @param[out] theMap collection to keep session info
  Standard_EXPORT static void CollectActiveWorkSessions(const occ::handle<XSControl_WorkSession>& theWS,
                                                        const TCollection_AsciiString& theName,
                                                        XSControl_WorkSessionMap&      theMap);

  //! Binds current session with input name.
  //! @param[in] theName the session file name
  Standard_EXPORT static void CollectActiveWorkSessions(const TCollection_AsciiString& theName);

  //! Loads all Draw commands of XSDRAW. Used for plugin.
  Standard_EXPORT static void Factory(Draw_Interpretor& theDI);
};

#endif // _XSDRAW_HeaderFile
