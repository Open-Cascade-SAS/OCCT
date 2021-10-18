// Created on: 1993-11-04
// Created by: Christian CAILLET
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _IFSelect_SessionDumper_HeaderFile
#define _IFSelect_SessionDumper_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <Standard_Boolean.hxx>
class IFSelect_SessionFile;
class TCollection_AsciiString;


class IFSelect_SessionDumper;
DEFINE_STANDARD_HANDLE(IFSelect_SessionDumper, Standard_Transient)

//! A SessionDumper is called by SessionFile. It takes into
//! account a set of classes (such as Selections, Dispatches ...).
//! SessionFile writes the Type (as defined by cdl) of each Item
//! and its general Parameters. It manages the names of the Items.
//!
//! A SessionDumper must be able to Write the Parameters which are
//! own of each Item it takes into account, given its Class, then
//! to Recognize the Type and Read its Own Parameters to create
//! an Item of this Type with these own Parameters.
//!
//! Then, there must be defined one sub-type of SessionDumper per
//! consistent set of classes (e.g. a package).
//!
//! By Own Parameters, understand Parameters given at Creation Time
//! if there are, or specific of a given class, apart from those
//! defined at superclass levels (e.g. Final Selection for a
//! Dispatch, Input Selection for a SelectExtract or SelectDeduct,
//! Direct Status for a SelectExtract, etc...).
//!
//! The Parameters are those stored in a WorkSession, they can be
//! of Types : IntParam, HAsciiString (for TextParam), Selection,
//! Dispatch.
//!
//! SessionDumpers are organized in a Library which is used by
//! SessionFile. They are put at Creation Time in this Library.
class IFSelect_SessionDumper : public Standard_Transient
{

public:

  
  //! Returns the First item of the Library of Dumper. The Next ones
  //! are then obtained by Next on the returned items
  Standard_EXPORT static Handle(IFSelect_SessionDumper) First();
  
  //! Returns the Next SesionDumper in the Library. Returns a Null
  //! Handle at the End.
  Standard_EXPORT Handle(IFSelect_SessionDumper) Next() const;
  
  //! Writes the Own Parameters of a given Item, if it forecast to
  //! manage its Type.
  //! Returns True if it has recognized the Type of the Item (in
  //! this case, it is assumed to have written the Own Parameters if
  //! there are some), False else : in that case, SessionFile will
  //! try another SessionDumper in the Library.
  //! WriteOwn can use these methods from SessionFile : SendVoid,
  //! SendItem, SendText, and if necessary, WorkSession.
  Standard_EXPORT virtual Standard_Boolean WriteOwn (IFSelect_SessionFile& file, const Handle(Standard_Transient)& item) const = 0;
  
  //! Recognizes a Type (given as <type>) then Creates an Item of
  //! this Type with the Own Parameter, as required.
  //! Returns True if it has recognized the Type (in this case, it
  //! is assumed to have created the Item, returned as <item>),
  //! False else : in that case, SessionFile will try another
  //! SessionDumper in the Library.
  //! ReadOwn can use these methods from SessionFile to access Own
  //! Parameters : NbOwnParams, IsVoid, IsText, TextValue, ItemValue
  Standard_EXPORT virtual Standard_Boolean ReadOwn (IFSelect_SessionFile& file, const TCollection_AsciiString& type, Handle(Standard_Transient)& item) const = 0;




  DEFINE_STANDARD_RTTIEXT(IFSelect_SessionDumper,Standard_Transient)

protected:

  
  //! The Initialization puts a just created SessionDumper in the
  //! Library of SessionDumper. Then, it suffices to create once
  //! a SessionDumper to fill the Library with it
  Standard_EXPORT IFSelect_SessionDumper();



private:


  Handle(IFSelect_SessionDumper) thenext;


};







#endif // _IFSelect_SessionDumper_HeaderFile
