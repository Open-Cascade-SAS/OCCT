// Created on: 1993-08-26
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

#ifndef _IFSelect_ModelCopier_HeaderFile
#define _IFSelect_ModelCopier_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <IFSelect_SequenceOfInterfaceModel.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
#include <IFSelect_SequenceOfAppliedModifiers.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HSequenceOfHAsciiString.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_CString.hxx>
class IFSelect_ShareOut;
class TCollection_AsciiString;
class Interface_InterfaceModel;
class IFSelect_AppliedModifiers;
class Interface_CheckIterator;
class IFSelect_ShareOutResult;
class IFSelect_WorkLibrary;
class Interface_Protocol;
class Interface_CopyTool;
class Interface_Graph;
class Interface_EntityIterator;


class IFSelect_ModelCopier;
DEFINE_STANDARD_HANDLE(IFSelect_ModelCopier, Standard_Transient)

//! This class performs the Copy operations involved by the
//! description of a ShareOut (evaluated by a ShareOutResult)
//! plus, if there are, the Modifications on the results, with
//! the help of Modifiers. Each Modifier can work on one or more
//! resulting packets, accoding its criteria : it operates on a
//! Model once copied and filled with the content of the packet.
//!
//! Modifiers can be :
//! - Model Modifiers, inheriting from the specific class Modifier
//! able to run on the content of a Model (header or entities),
//! activated by the ModelCopier itself
//! - File Modifiers, inheriting directly from GeneralModifier,
//! intended to be activated under the control of a WorkLibrary,
//! once the Model has been produced (i.e. to act on output
//! format, or other specific file features)
//!
//! The Copy operations can be :
//! - immediately put to files : for each packet, a Model is
//! created and filled, then the file is output, at that's all
//! - memorized : for each packet, a Model is created and filled,
//! it is memorized with the corresponding file name.
//! it is possible to query the result of memorization (list of
//! produced Models and their file names)
//! -> it is also possible to send it into the files :
//! once files are written, the result is cleared
//!
//! In addition, a list of really written files is managed :
//! A first call to BeginSentFiles clears the list and commands,
//! either to begin a new list, or to stop recording it. A call
//! to SentFiles returns the list (if recording has been required)
//! This list allows to globally exploit the set of produced files
//!
//! Remark : For operations which concern specific Entities, see
//! also in package IFAdapt : a sub-class of ModelCopier allows
//! to work with EntityModifier, in addition to Modifier itself
//! which still applies to a whole copied Model.
class IFSelect_ModelCopier : public Standard_Transient
{

public:

  
  //! Creates an empty ModelCopier
  Standard_EXPORT IFSelect_ModelCopier();
  
  //! Sets the ShareOut, which is used to define Modifiers to apply
  Standard_EXPORT void SetShareOut (const Handle(IFSelect_ShareOut)& sho);
  
  //! Clears the list of produced Models
  Standard_EXPORT void ClearResult();
  
  //! Records a new File to be sent, as a couple
  //! (Name as AsciiString, Content as InterfaceModel)
  //! Returns True if Done, False if <filename> is already attached
  //! to another File
  Standard_EXPORT Standard_Boolean AddFile (const TCollection_AsciiString& filename, const Handle(Interface_InterfaceModel)& content);
  
  //! Changes the Name attached to a File which was formerly defined
  //! by a call to AddFile
  //! Returns True if Done, False else : if <num> out of range or if
  //! the new <filename> is already attached to another File
  //! Remark : Giving an empty File Name is equivalent to ClearFile
  Standard_EXPORT Standard_Boolean NameFile (const Standard_Integer num, const TCollection_AsciiString& filename);
  
  //! Clears the Name attached to a File which was formerly defined
  //! by a call to AddFile. This Clearing can be undone by a call to
  //! NameFile (with same <num>)
  //! Returns True if Done, False else : if <num> is out of range
  Standard_EXPORT Standard_Boolean ClearFile (const Standard_Integer num);
  
  //! Sets a list of File Modifiers to be applied on a file
  Standard_EXPORT Standard_Boolean SetAppliedModifiers (const Standard_Integer num, const Handle(IFSelect_AppliedModifiers)& applied);
  
  //! Clears the list of File Modifiers to be applied on a file
  Standard_EXPORT Standard_Boolean ClearAppliedModifiers (const Standard_Integer num);
  
  //! Performs the Copy Operations, which include the Modifications
  //! defined by the list of Modifiers. Memorizes the result, as a
  //! list of InterfaceModels with the corresponding FileNames
  //! They can then be sent, by the method Send, or queried
  //! Copy calls internal method Copying.
  //! Returns the produced CheckList
  Standard_EXPORT Interface_CheckIterator Copy (IFSelect_ShareOutResult& eval, const Handle(IFSelect_WorkLibrary)& WL, const Handle(Interface_Protocol)& protocol);
  
  //! Sends the formerly defined results (see method Copy) to files,
  //! then clears it
  //! Remark : A Null File Name cause file to be not produced
  Standard_EXPORT Interface_CheckIterator SendCopied (const Handle(IFSelect_WorkLibrary)& WL, const Handle(Interface_Protocol)& protocol);
  
  //! Performs the Copy Operations (which include the Modifications)
  //! and Sends the result on files, without memorizing it.
  //! (the memorized result is ignored : neither queried not filled)
  Standard_EXPORT Interface_CheckIterator Send (IFSelect_ShareOutResult& eval, const Handle(IFSelect_WorkLibrary)& WL, const Handle(Interface_Protocol)& protocol);
  
  //! Sends a model (defined in <G>) into one file, without managing
  //! remaining data, already sent files, etc. Applies the Model and
  //! File Modifiers.
  //! Returns True if well done, False else
  Standard_EXPORT Interface_CheckIterator SendAll (const Standard_CString filename, const Interface_Graph& G, const Handle(IFSelect_WorkLibrary)& WL, const Handle(Interface_Protocol)& protocol);
  
  //! Sends a part of a model into one file. Model is gotten from
  //! <G>, the part is defined in <iter>.
  //! Remaining data are managed and can be later be worked on.
  //! Returns True if well done, False else
  Standard_EXPORT Interface_CheckIterator SendSelected (const Standard_CString filename, const Interface_Graph& G, const Handle(IFSelect_WorkLibrary)& WL, const Handle(Interface_Protocol)& protocol, const Interface_EntityIterator& iter);
  
  //! Produces a Model copied from the Remaining List as <newmod>
  //! <newmod> is a Null Handle if this list is empty
  //! <WL> performs the copy by using <TC>
  //! <TC> is assumed to have been defined with the starting model
  //! same as defined by <G>.
  Standard_EXPORT void CopiedRemaining (const Interface_Graph& G, const Handle(IFSelect_WorkLibrary)& WL, Interface_CopyTool& TC, Handle(Interface_InterfaceModel)& newmod);
  
  //! Updates Graph status for remaining data, for each entity :
  //! - Entities just Sent to file or Copied (by CopiedRemaining)
  //! have their status set to 1
  //! - the other keep their former status (1 for Send/Copied,
  //! 0 for Remaining)
  //! These status are computed by Copying/Sending/CopiedRemaining
  //! Then, SetRemaining updates graph status, and mustr be called
  //! just after one of these method has been called
  //! Returns True if done, False if remaining info if not in phase
  //! which the Graph (not same counts of items)
  Standard_EXPORT Standard_Boolean SetRemaining (Interface_Graph& CG) const;
  
  //! Returns the count of Files produced, i.e. the count of Models
  //! memorized (produced by the mmethod Copy) with their file names
  Standard_EXPORT Standard_Integer NbFiles() const;
  
  //! Returns the File Name for a file given its rank
  //! It is empty after a call to ClearFile on same <num>
  Standard_EXPORT TCollection_AsciiString FileName (const Standard_Integer num) const;
  
  //! Returns the content of a file before sending, under the form
  //! of an InterfaceModel, given its rank
  Standard_EXPORT Handle(Interface_InterfaceModel) FileModel (const Standard_Integer num) const;
  
  //! Returns the list of File Modifiers to be applied on a file
  //! when it will be sent, as computed by CopiedModel :
  //! If it is a null handle, no File Modifier has to be applied.
  Standard_EXPORT Handle(IFSelect_AppliedModifiers) AppliedModifiers (const Standard_Integer num) const;
  
  //! Begins a sequence of recording the really sent files
  //! <sho> : the default file numbering is cleared
  //! If <record> is False, clears the list and stops recording
  //! If <record> is True, clears the list and commands recording
  //! Creation time corresponds to "stop recording"
  Standard_EXPORT void BeginSentFiles (const Handle(IFSelect_ShareOut)& sho, const Standard_Boolean record);
  
  //! Adds the name of a just sent file, if BeginSentFiles
  //! has commanded recording; else does nothing
  //! It is called by methods SendCopied Sending
  Standard_EXPORT void AddSentFile (const Standard_CString filename);
  
  //! Returns the list of recorded names of sent files. Can be empty
  //! (if no file has been sent). Returns a Null Handle if
  //! BeginSentFiles has stopped recording.
  Standard_EXPORT Handle(TColStd_HSequenceOfHAsciiString) SentFiles() const;




  DEFINE_STANDARD_RTTIEXT(IFSelect_ModelCopier,Standard_Transient)

protected:

  
  //! Internal routine which does the effective Copy. It allows to
  //! work, either with a standard CopyTool, or a specialised one
  //! Copying itself is done by <WL> which uses a CopyTool
  Standard_EXPORT Interface_CheckIterator Copying (IFSelect_ShareOutResult& eval, const Handle(IFSelect_WorkLibrary)& WL, const Handle(Interface_Protocol)& protocol, Interface_CopyTool& TC);
  
  //! Internal routine which does the effective Send. It allows to
  //! work, either with a standard CopyTool, or a specialised one
  Standard_EXPORT Interface_CheckIterator Sending (IFSelect_ShareOutResult& eval, const Handle(IFSelect_WorkLibrary)& WL, const Handle(Interface_Protocol)& protocol, Interface_CopyTool& TC);
  
  //! Performs the Copy of a unitary Packet
  //! Input parameters are :
  //! <G> is the graph which defines the starting entities, it
  //! contains the original InterfaceModel
  //! <WL> performs the copy by using <TC>
  //! <protocol> is the used protocol (can be usefull for Modifiers)
  //! <topcopy> is the list of Entities which are the Roots of the
  //! packet to be copied
  //! <filename> is the name of the file which will receive it
  //! <dispid> is the Identifier of the Dispatch which have produced
  //! this packet, <numod> is the rank of the packet for this
  //! Dispatch
  //! <TC> is a CopyTool, which performs the copy
  //!
  //! Returned values (as arguments) are :
  //! <newmod> is the result of the copy, as a new InterfaceModel on
  //! which Model Modifiers have already been applied (if there are)
  //! <applied> determines the File Modifiers which remain to be
  //! applied (when the file itself will be output) : for each File
  //! Modifier recorded in <me>, <applied>'s Value is :
  //! - Null if this Modifier has not to be applied
  //! - an empty list if this Modifier has to be applied without
  //! distinguishing specific entities
  //! - a list of numbers of entities in <model> if this Modifier
  //! concerns particularly these entities (which are the results
  //! of copying the result of its input selection)
  //! <checks> is the produced Check List (by Modifiers as required)
  //!
  //! Warning : File Modifiers are evaluated at the time of Copy itself
  //! If their list is changed between this Copy and the Sending
  //! itself of the file, these changes are ignored
  Standard_EXPORT void CopiedModel (const Interface_Graph& G, const Handle(IFSelect_WorkLibrary)& WL, const Handle(Interface_Protocol)& protocol, const Interface_EntityIterator& topcopy, const TCollection_AsciiString& filename, const Standard_Integer dispnum, const Standard_Integer numod, Interface_CopyTool& TC, Handle(Interface_InterfaceModel)& newmod, Handle(IFSelect_AppliedModifiers)& applied, Interface_CheckIterator& checks) const;



private:


  IFSelect_SequenceOfInterfaceModel thefilemodels;
  TColStd_SequenceOfAsciiString thefilenames;
  IFSelect_SequenceOfAppliedModifiers theapplieds;
  Handle(IFSelect_ShareOut) theshareout;
  Handle(TColStd_HArray1OfInteger) theremain;
  Handle(TColStd_HSequenceOfHAsciiString) thesentfiles;


};







#endif // _IFSelect_ModelCopier_HeaderFile
