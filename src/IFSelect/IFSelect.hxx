// Created on: 1992-09-21
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

#ifndef _IFSelect_HeaderFile
#define _IFSelect_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_CString.hxx>
class IFSelect_WorkSession;


//! Gives tools to manage Selecting a group of Entities
//! processed by an Interface, for instance to divide up an
//! original Model (from a File) to several smaller ones
//! They use description of an Interface Model as a graph
//!
//! Remark that this corresponds to the description of a
//! "scenario" of sharing out a File. Parts of this Scenario
//! are intended to be permanently stored. IFSelect provides
//! the Transient, active counterparts (to run the Scenario).
//! But a permanent one (either as Persistent Objects or as
//! interpretable Text) must be provided elsewhere.
class IFSelect 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Saves the state of a WorkSession from IFSelect, by using a
  //! SessionFile from IFSelect. Returns True if Done, False in
  //! case of Error on Writing. <file> gives the name of the File
  //! to be produced (this avoids to export the class SessionFile).
  Standard_EXPORT static Standard_Boolean SaveSession (const Handle(IFSelect_WorkSession)& WS, const Standard_CString file);
  
  //! Restore the state of a WorkSession from IFSelect, by using a
  //! SessionFile from IFSelect. Returns True if Done, False in
  //! case of Error on Writing. <file> gives the name of the File
  //! to be used (this avoids to export the class SessionFile).
  Standard_EXPORT static Standard_Boolean RestoreSession (const Handle(IFSelect_WorkSession)& WS, const Standard_CString file);




protected:





private:




friend class IFSelect_Signature;
friend class IFSelect_SignMultiple;
friend class IFSelect_SignType;
friend class IFSelect_SignCategory;
friend class IFSelect_SignValidity;
friend class IFSelect_SignAncestor;
friend class IFSelect_ShareOut;
friend class IFSelect_AppliedModifiers;
friend class IFSelect_ShareOutResult;
friend class IFSelect_PacketList;
friend class IFSelect_Dispatch;
friend class IFSelect_DispGlobal;
friend class IFSelect_DispPerOne;
friend class IFSelect_DispPerCount;
friend class IFSelect_DispPerSignature;
friend class IFSelect_DispPerFiles;
friend class IFSelect_SelectionIterator;
friend class IFSelect_Selection;
friend class IFSelect_SelectBase;
friend class IFSelect_SelectModelRoots;
friend class IFSelect_SelectModelEntities;
friend class IFSelect_SelectEntityNumber;
friend class IFSelect_SelectPointed;
friend class IFSelect_SelectCombine;
friend class IFSelect_SelectUnion;
friend class IFSelect_SelectIntersection;
friend class IFSelect_SelectControl;
friend class IFSelect_SelectDiff;
friend class IFSelect_SelectDeduct;
friend class IFSelect_SelectShared;
friend class IFSelect_SelectSharing;
friend class IFSelect_SelectAnyList;
friend class IFSelect_SelectInList;
friend class IFSelect_SelectSuite;
friend class IFSelect_SelectExtract;
friend class IFSelect_SelectUnknownEntities;
friend class IFSelect_SelectErrorEntities;
friend class IFSelect_SelectIncorrectEntities;
friend class IFSelect_SelectRoots;
friend class IFSelect_SelectRootComps;
friend class IFSelect_SelectRange;
friend class IFSelect_SelectAnyType;
friend class IFSelect_SelectType;
friend class IFSelect_SelectSignature;
friend class IFSelect_SelectFlag;
friend class IFSelect_SelectSent;
friend class IFSelect_SelectExplore;
friend class IFSelect_SelectSignedShared;
friend class IFSelect_SelectSignedSharing;
friend class IFSelect_IntParam;
friend class IFSelect_SignatureList;
friend class IFSelect_CheckCounter;
friend class IFSelect_SignCounter;
friend class IFSelect_GraphCounter;
friend class IFSelect_Editor;
friend class IFSelect_ParamEditor;
friend class IFSelect_EditForm;
friend class IFSelect_ListEditor;
friend class IFSelect_ContextModif;
friend class IFSelect_ContextWrite;
friend class IFSelect_Transformer;
friend class IFSelect_TransformStandard;
friend class IFSelect_ModelCopier;
friend class IFSelect_GeneralModifier;
friend class IFSelect_Modifier;
friend class IFSelect_ModifReorder;
friend class IFSelect_ModifEditForm;
friend class IFSelect_FileModifier;
friend class IFSelect_ModelModifier;
friend class IFSelect_WorkSession;
friend class IFSelect_WorkLibrary;
friend class IFSelect_SessionFile;
friend class IFSelect_SessionDumper;
friend class IFSelect_BasicDumper;
friend class IFSelect_Activator;
friend class IFSelect_SessionPilot;
friend class IFSelect_Act;
friend class IFSelect_Functions;

};







#endif // _IFSelect_HeaderFile
