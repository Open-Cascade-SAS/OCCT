// Created on: 2002-02-22
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

#ifndef _Message_ProgressSentry_HeaderFile
#define _Message_ProgressSentry_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
#include <Standard_Real.hxx>
class Message_ProgressIndicator;
class TCollection_HAsciiString;


//! This class is a tool allowing to manage opening/closing
//! scopes in the ProgressIndicator in convenient and safe way.
//!
//! Its main features are:
//! - Set all parameters for the current scale on the given
//! ProgressIndicator and open a new scope at one line
//! - Iterator-like interface to opening next scopes and
//! check for user break
//! - Automatic scope closing in destructor
//! - Safe for NULL ProgressIndicator (just does nothing)
//!
//! Example of usage in nested process:
//!
//! @code{.cpp}
//!   Handle(Draw_ProgressIndicator) aProgress = ...;
//!
//!   // Outer cycle
//!   Message_ProgressSentry anOuter (aProgress, "Outer", 0, nbOuter, 1);
//!   for (int i = 0; i < nbOuter && anOuter.More(); i++, anOuter.Next())
//!   {
//!     // Inner cycle
//!     Message_ProgressSentry anInner (aProgress, "Inner", 0, nbInner, 1);
//!     for (int j = 0; j < nbInner && anInner.More(); j++, anInner.Next())
//!     {
//!       // Cycle body
//!     }
//!   }
//! @endcode

class Message_ProgressSentry 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT Message_ProgressSentry(const Handle(Message_ProgressIndicator)& PI, const Standard_CString name, const Standard_Real min, const Standard_Real max, const Standard_Real step, const Standard_Boolean isInf = Standard_False, const Standard_Real newScopeSpan = 0.0);
  
  //! Creates an instance of ProgressSentry attaching it to
  //! the specified ProgressIndicator, selects parameters of
  //! the current scale, and opens a new scope with specified
  //! span (equal to step by default)
  Standard_EXPORT Message_ProgressSentry(const Handle(Message_ProgressIndicator)& PI, const Handle(TCollection_HAsciiString)& name, const Standard_Real min, const Standard_Real max, const Standard_Real step, const Standard_Boolean isInf = Standard_False, const Standard_Real newScopeSpan = 0.0);
  
  //! Moves progress indicator to the end of the current scale
  //! and relieves sentry from its duty. Methods other than Show()
  //! will do nothing after this one is called.
    void Relieve();
~Message_ProgressSentry()
{
  Relieve();
}
  
    void Next (const Standard_CString name = 0) const;
  
    void Next (const Standard_Real span, const Standard_CString name = 0) const;
  
  //! Closes current scope and opens next one
  //! with either specified or default span
    void Next (const Standard_Real span, const Handle(TCollection_HAsciiString)& name) const;
  
  //! Returns False if ProgressIndicator signals UserBreak
    Standard_Boolean More() const;
  
  //! Forces update of progress indicator display
    void Show() const;




protected:





private:



  Handle(Message_ProgressIndicator) myProgress;
  Standard_Boolean myActive;


};


#include <Message_ProgressSentry.lxx>





#endif // _Message_ProgressSentry_HeaderFile
