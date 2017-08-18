// Created on: 2002-02-20
// Created by: Andrey BETENEV
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _Message_ProgressIndicator_HeaderFile
#define _Message_ProgressIndicator_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <Message_SequenceOfProgressScale.hxx>
#include <Standard_Transient.hxx>
#include <Standard_CString.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
class TCollection_HAsciiString;
class Message_ProgressScale;


class Message_ProgressIndicator;
DEFINE_STANDARD_HANDLE(Message_ProgressIndicator, Standard_Transient)

//! Defines abstract interface from program to the "user".
//! This includes progress indication and user break mechanisms.
//!
//! The process that uses the progress indicator interacts with it as
//! with a scale whose range and step can be configured according to
//! the nature of the process.
//! The scale can be made "infinite", which means it will grow
//! non-linearly, and end of scale will be approached asymptotically at
//! infinite number of steps. In that case the range defines
//! a number of steps corresponding to position at 1/2 of scale.
//! The current position can be either set directly (in a range from
//! current position to maximum scale value), or incremented step
//! by step.
//!
//! Progress indication mechanism is adapted for convenient
//! usage in hiererchical processes that require indication of
//! progress at several levels of the process nesting.
//! For that purpose, it is possible to create restricted sub-scope of
//! indication by specifying part of a current scale to be
//! used by the subprocess.
//! When subprocess works with progress indicator in the restricted
//! scope, it has the same interface to a scale, while actually it
//! deals only with part of the whole scale.
//! 
//! The recommended way to implement progress indication in the algorithm
//! is to use class Message_ProgressSentry that provides iterator-like
//! interface for incrementing progress and opening nested scopes.
//!
//! NOTE:
//! Currently there is no support for concurrent progress
//! indicator that could be useful in multithreaded applications.
//!
//! The user break is implemented as virtual function that should
//! return True in case if break signal from the user is received.
//!
//! The derived class should take care of visualisation of the
//! progress indicator (e.g. show total position at the graphical bar,
//! print scopes in text mode, or else), and for implementation
//! of user break mechanism (if necessary).

class Message_ProgressIndicator : public Standard_Transient
{

public:

  
  //! Drops all scopes and sets scale from 0 to 100, step 1
  //! This scale has name "Step"
  Standard_EXPORT virtual void Reset();
  
    void SetName (const Standard_CString name);
  
  //! Set (optional) name for scale
    void SetName (const Handle(TCollection_HAsciiString)& name);
  
  //! Set range for current scale
    void SetRange (const Standard_Real min, const Standard_Real max);
  
  //! Set step for current scale
    void SetStep (const Standard_Real step);
  
  //! Set or drop infinite mode for the current scale
    void SetInfinite (const Standard_Boolean isInf = Standard_True);
  
    void SetScale (const Standard_CString name, const Standard_Real min, const Standard_Real max, const Standard_Real step, const Standard_Boolean isInf = Standard_False);
  
  //! Set all parameters for current scale
  Standard_EXPORT void SetScale (const Standard_Real min, const Standard_Real max, const Standard_Real step, const Standard_Boolean isInf = Standard_False);
  
  //! Returns all parameters for current scale
  Standard_EXPORT void GetScale (Standard_Real& min, Standard_Real& max, Standard_Real& step, Standard_Boolean& isInf) const;
  
  Standard_EXPORT void SetValue (const Standard_Real val);
  
  //! Set and get progress value at current scale
  //! If the value to be set is more than currently set one, or out
  //! of range for the current scale, it is limited by that range
  Standard_EXPORT Standard_Real GetValue() const;
  
    void Increment();
  
  //! Increment the progress value by the default of specified step
    void Increment (const Standard_Real step);
  
    Standard_Boolean NewScope (const Standard_CString name = 0);
  
    Standard_Boolean NewScope (const Handle(TCollection_HAsciiString)& name);
  
    Standard_Boolean NewScope (const Standard_Real span, const Standard_CString name = 0);
  
  //! Creates new scope on a part of a current scale from current
  //! position with span either equal to default step, or specified
  //! The scale for the new scope will have specified name and
  //! ranged from 0 to 100 with step 1
  //! Returns False if something is wrong in arguments or in current
  //! position of progress indicator; scope is opened anyway
  Standard_EXPORT Standard_Boolean NewScope (const Standard_Real span, const Handle(TCollection_HAsciiString)& name);
  
  //! Close the current scope and thus return to previous scale
  //! Updates position to be at the end of the closing scope
  //! Returns False if no scope is opened
  Standard_EXPORT Standard_Boolean EndScope();
  
    Standard_Boolean NextScope (const Standard_CString name = 0);
  
  //! Optimized version of { return EndScope() && NewScope(); }
  Standard_EXPORT Standard_Boolean NextScope (const Standard_Real span, const Standard_CString name = 0);
  
  //! Should return True if user has send a break signal.
  //! Default implementation returns False.
  Standard_EXPORT virtual Standard_Boolean UserBreak();
  
  //! Update presentation of the progress indicator
  //! Called when progress position is changed
  //! Flag force is intended for forcing update in case if it is
  //! optimized; all internal calls from ProgressIndicator are
  //! done with this flag equal to False
  Standard_EXPORT virtual Standard_Boolean Show (const Standard_Boolean force = Standard_True) = 0;
  
  //! Returns total progress position on the basic scale
  //! ranged from 0. to 1.
    Standard_Real GetPosition() const;
  
  //! Returns current number of opened scopes
  //! This number is always >=1 as top-level scale is always present
  Standard_Integer GetNbScopes() const;
  
  //! Returns data for scale of index-th scope
  //! The first scope is current one, the last is the top-level one
  const Message_ProgressScale& GetScope (const Standard_Integer index) const;




  DEFINE_STANDARD_RTTIEXT(Message_ProgressIndicator,Standard_Transient)

protected:

  
  //! Constructor, just calls own Reset() (not yet redefined)
  Standard_EXPORT Message_ProgressIndicator();



private:


  Standard_Real myPosition;
  Message_SequenceOfProgressScale myScopes;


};


#include <Message_ProgressIndicator.lxx>





#endif // _Message_ProgressIndicator_HeaderFile
