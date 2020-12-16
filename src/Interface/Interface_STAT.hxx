// Created on: 1996-02-15
// Created by: Christian CAILLET
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

#ifndef _Interface_STAT_HeaderFile
#define _Interface_STAT_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <TColStd_HSequenceOfAsciiString.hxx>
#include <TColStd_HSequenceOfReal.hxx>
#include <TColStd_HSequenceOfInteger.hxx>
#include <Standard_CString.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class TCollection_HAsciiString;


//! This class manages statistics to be queried asynchronously.
//! Way of use :
//! An operator describes a STAT form then fills it according to
//! its progression. This produces a state of advancement of the
//! process. This state can then be queried asynchronously :
//! typically it is summarised as a percentage. There are also
//! an identification of the current state, and information on
//! processed volume.
//!
//! A STAT form can be described once for all (as static).
//! It describes the stream of the process (see later), in terms
//! of phases, cycles, steps, with estimated weights. But it
//! brings no current data.
//!
//! One STAT at a time is active for filling and querying. It is
//! used to control phasing, weighting ... Specific data for
//! execution are given when running on active STAT : counts of
//! items ... Data for query are then recorded and can be accessed
//! at any time, asynchronously.
//!
//! A STAT is organised as follows :
//! - it can be split into PHASES (by default, there is none, and
//! all process takes place in one "default" phase)
//! - each phase is identified by a name and is attached a weight
//! -> the sum of the weights is used to compute relative weights
//! - for each phase, or for the unique default phase if none :
//! -- the process works on a list of ITEMS
//! -- by default, all the items are processed in once
//! -- but this list can be split into CYCLES, each one takes
//! a sub-list : the weight of each cycle is related to its
//! count of items
//! -- a cycle can be split into STEPS, by default there are none
//! then one "default step" is considered
//! -- each step is attached a weight
//! -> the sum of the weights of steps is used to compute relative
//! weights of the steps in each cycle
//! -> all the cycles of a phase have the same organisation
//!
//! Hence, when defining the STAT form, the phases have to be
//! described. If no weight is precisely known, give 1. for all...
//! No phase description will give only one "default" phase
//! For each phase, a typical cycle can be described by its steps.
//! Here too, for no weight precisely known, give 1. for all...
//!
//! For executing, activate a STAT to begin count. Give counts of
//! items and cycles for the first phase (for the unique default
//! one if no phasing is described)
//! Else, give count of items and cycles for each new phase.
//! Class methods allow also to set next cycle (given count of
//! items), next step in cycle (if more then one), next item in
//! step.
class Interface_STAT 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a STAT form. At start, one default phase is defined,
  //! with one default step. Then, it suffises to start with a
  //! count of items (and cycles if several) then record items,
  //! to have a queryable report.
  Standard_EXPORT Interface_STAT(const Standard_CString title = "");
  
  //! used when starting
  Standard_EXPORT Interface_STAT(const Interface_STAT& other);

  //! Assignment
  Interface_STAT& operator= (const Interface_STAT& theOther)
  {
    theOther.Internals (thetitle, thetotal, thephnam, thephw, thephdeb,thephfin, thestw);
    return *this;
  }

  //! Returns fields in once, without copying them, used for copy
  //! when starting
  Standard_EXPORT void Internals (Handle(TCollection_HAsciiString)& tit, Standard_Real& total, Handle(TColStd_HSequenceOfAsciiString)& phn, Handle(TColStd_HSequenceOfReal)& phw, Handle(TColStd_HSequenceOfInteger)& phdeb, Handle(TColStd_HSequenceOfInteger)& phfin, Handle(TColStd_HSequenceOfReal)& stw) const;
  
  //! Adds a new phase to the description.
  //! The first one after Create replaces the default unique one
  Standard_EXPORT void AddPhase (const Standard_Real weight, const Standard_CString name = "");
  
  //! Adds a new step for the last added phase, the default unique
  //! one if no AddPhase has already been added
  //! Warning : AddStep before the first AddPhase are cancelled
  Standard_EXPORT void AddStep (const Standard_Real weight = 1);
  
  //! Returns global description (cumulated weights of all phases,
  //! count of phases,1 for default, and title)
  Standard_EXPORT void Description (Standard_Integer& nbphases, Standard_Real& total, Standard_CString& title) const;
  
  //! Returns description of a phase, given its rank
  //! (n0 for first step, count of steps, default gives one;
  //! weight, name)
  Standard_EXPORT void Phase (const Standard_Integer num, Standard_Integer& n0step, Standard_Integer& nbstep, Standard_Real& weight, Standard_CString& name) const;
  
  //! Returns weight of a Step, related to the cumul given for the
  //! phase.
  //! <num> is given by <n0step> + i, i between 1 and <nbsteps>
  //! (default gives n0step < 0 then weight is one)
  Standard_EXPORT Standard_Real Step (const Standard_Integer num) const;
  
  //! Starts a STAT on its first phase (or its default one)
  //! <items> gives the total count of items, <cycles> the count of
  //! cycles
  //! If <cycles> is more than one, the first Cycle must then be
  //! started by NextCycle (NextStep/NextItem are ignored).
  //! If it is one, NextItem/NextStep can then be called
  Standard_EXPORT void Start (const Standard_Integer items, const Standard_Integer cycles = 1) const;
  
  //! Starts a default STAT, with no phase, no step, ready to just
  //! count items.
  //! <items> gives the total count of items
  //! Hence, NextItem is available to directly count
  Standard_EXPORT static void StartCount (const Standard_Integer items, const Standard_CString title = "");
  
  //! Commands to resume the preceding phase and start a new one
  //! <items> and <cycles> as for Start, but for this new phase
  //! Ignored if count of phases is already passed
  //! If <cycles> is more than one, the first Cycle must then be
  //! started by NextCycle (NextStep/NextItem are ignored).
  //! If it is one, NextItem/NextStep can then be called
  Standard_EXPORT static void NextPhase (const Standard_Integer items, const Standard_Integer cycles = 1);
  
  //! Changes the parameters of the phase to start
  //! To be used before first counting (i.e. just after NextPhase)
  //! Can be used by an operator which has to reajust counts on run
  Standard_EXPORT static void SetPhase (const Standard_Integer items, const Standard_Integer cycles = 1);
  
  //! Commands to resume the preceding cycle and start a new one,
  //! with a count of items
  //! Ignored if count of cycles is already passed
  //! Then, first step is started (or default one)
  //! NextItem can be called for the first step, or NextStep to pass
  //! to the next one
  Standard_EXPORT static void NextCycle (const Standard_Integer items);
  
  //! Commands to resume the preceding step of the cycle
  //! Ignored if count of steps is already passed
  //! NextItem can be called for this step, NextStep passes to next
  Standard_EXPORT static void NextStep();
  
  //! Commands to add an item in the current step of the current
  //! cycle of the current phase
  //! By default, one item per call, can be overpassed
  //! Ignored if count of items of this cycle is already passed
  Standard_EXPORT static void NextItem (const Standard_Integer nbitems = 1);
  
  //! Commands to declare the process ended (hence, advancement is
  //! forced to 100 %)
  Standard_EXPORT static void End();
  
  //! Returns an identification of the STAT :
  //! <phase> True (D) : the name of the current phase
  //! <phase> False : the title of the current STAT
  Standard_EXPORT static Standard_CString Where (const Standard_Boolean phase = Standard_True);
  
  //! Returns the advancement as a percentage :
  //! <phase> True : inside the current phase
  //! <phase> False (D) : relative to the whole process
  Standard_EXPORT static Standard_Integer Percent (const Standard_Boolean phase = Standard_False);




protected:





private:



  Handle(TCollection_HAsciiString) thetitle;
  Standard_Real thetotal;
  Handle(TColStd_HSequenceOfAsciiString) thephnam;
  Handle(TColStd_HSequenceOfReal) thephw;
  Handle(TColStd_HSequenceOfInteger) thephdeb;
  Handle(TColStd_HSequenceOfInteger) thephfin;
  Handle(TColStd_HSequenceOfReal) thestw;


};







#endif // _Interface_STAT_HeaderFile
