// Created on: 1995-03-16
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

#ifndef _XSControl_FuncShape_HeaderFile
#define _XSControl_FuncShape_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <TopTools_HSequenceOfShape.hxx>
#include <Standard_CString.hxx>
class XSControl_WorkSession;
class TCollection_AsciiString;

//! Defines additional commands for XSControl to :
//! - control of initialisation (xinit, xnorm, newmodel)
//! - analyse of the result of a transfer (recorded in a
//! TransientProcess for Read, FinderProcess for Write) :
//! statistics, various lists (roots,complete,abnormal), what
//! about one specific entity, producing a model with the
//! abnormal result
//!
//! This appendix of XSControl is compiled separately to distinguish
//! basic features from user callable forms
class XSControl_FuncShape
{
public:
  DEFINE_STANDARD_ALLOC

  //! Defines and loads all functions which work on shapes for XSControl (as ActFunc)
  Standard_EXPORT static void Init();

  //! Analyses a name as designating Shapes from a Vars or from
  //! XSTEP transfer (last Transfer on Reading). <name> can be :
  //! "*" : all the root shapes produced by last Transfer (Read)
  //! i.e. considers roots of the TransientProcess
  //! a name : a name of a variable DRAW
  //!
  //! Returns the count of designated Shapes. Their list is put in
  //! <list>. If <list> is null, it is firstly created. Then it is
  //! completed (Append without Clear) by the Shapes found
  //! Returns 0 if no Shape could be found
  Standard_EXPORT static Standard_Integer MoreShapes(const Handle(XSControl_WorkSession)& session,
                                                     Handle(TopTools_HSequenceOfShape)&   list,
                                                     const Standard_CString               name);

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
  Standard_EXPORT static Standard_Boolean FileAndVar(const Handle(XSControl_WorkSession)& session,
                                                     const Standard_CString               file,
                                                     const Standard_CString               var,
                                                     const Standard_CString               def,
                                                     TCollection_AsciiString&             resfile,
                                                     TCollection_AsciiString&             resvar);

protected:
private:
};

#endif // _XSControl_FuncShape_HeaderFile
