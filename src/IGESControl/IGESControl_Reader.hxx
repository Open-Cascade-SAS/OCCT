// Created on: 1996-09-06
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

#ifndef _IGESControl_Reader_HeaderFile
#define _IGESControl_Reader_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <XSControl_Reader.hxx>
#include <Standard_Integer.hxx>
#include <IFSelect_PrintFail.hxx>
#include <IFSelect_PrintCount.hxx>
class XSControl_WorkSession;
class IGESData_IGESModel;



//! Reads IGES files, checks them and translates their contents into Open CASCADE models.
//! The IGES data can be that of a whole model or that of a specific list of entities in the model.
//! As in XSControl_Reader, you specify the list using a selection.
//! For translation of iges files it is possible to use the following sequence:
//! To change parameters of translation
//! class Interface_Static should be used before the beginning of translation
//! (see IGES Parameters and General Parameters)
//! Creation of reader
//! IGESControl_Reader reader;
//! To load a file in a model use method:
//! reader.ReadFile("filename.igs")
//! To check a loading file use method Check:
//! reader.Check(failsonly); where failsonly is equal to Standard_True or
//! Standard_False;
//! To print the results of load:
//! reader.PrintCheckLoad(failsonly,mode) where mode is equal to the value of
//! enumeration IFSelect_PrintCount
//! To transfer entities from a model the following methods can be used:
//! for the whole model
//! reader.TransferRoots(onlyvisible); where onlyvisible is equal to
//! Standard_True or Standard_False;
//! To transfer a list of entities:
//! reader.TransferList(list);
//! To transfer one entity
//! reader.TransferEntity(ent) or reader.Transfer(num);
//! To obtain a result the following method can be used:
//! reader.IsDone()
//! reader.NbShapes() and reader.Shape(num); or reader.OneShape();
//! To print the results of transfer use method:
//! reader.PrintTransferInfo(failwarn,mode); where printfail is equal to the
//! value of enumeration IFSelect_PrintFail, mode see above.
//! Gets correspondence between an IGES entity and a result shape obtained therefrom.
//! reader.TransientProcess();
//! TopoDS_Shape shape =
//! TransferBRep::ShapeResult(reader.TransientProcess(),ent);
class IGESControl_Reader  : public XSControl_Reader
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a Reader from scratch
  Standard_EXPORT IGESControl_Reader();
  
  //! Creates a Reader from an already existing Session
  Standard_EXPORT IGESControl_Reader(const Handle(XSControl_WorkSession)& WS, const Standard_Boolean scratch = Standard_True);
  
  //! Set the transion of ALL Roots (if theReadOnlyVisible is False)
  //! or of Visible Roots (if theReadOnlyVisible is True)
    void SetReadVisible (const Standard_Boolean ReadRoot);
  
    Standard_Boolean GetReadVisible() const;
  
  //! Returns the model as a IGESModel.
  //! It can then be consulted (header, product)
  Standard_EXPORT Handle(IGESData_IGESModel) IGESModel() const;
  
  //! Determines the list of root entities from Model which are candidate for
  //! a transfer to a Shape (type of entities is PRODUCT)
  //! <theReadOnlyVisible> is taken into account to define roots
  Standard_EXPORT virtual Standard_Integer NbRootsForTransfer() Standard_OVERRIDE;
  
  //! Prints Statistics and check list for Transfer
  Standard_EXPORT void PrintTransferInfo (const IFSelect_PrintFail failwarn, const IFSelect_PrintCount mode) const;




protected:





private:



  Standard_Boolean theReadOnlyVisible;


};


#include <IGESControl_Reader.lxx>





#endif // _IGESControl_Reader_HeaderFile
