// Created on: 1996-07-08
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

#ifndef _STEPControl_Reader_HeaderFile
#define _STEPControl_Reader_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <XSControl_Reader.hxx>
#include <Standard_Integer.hxx>
#include <StepData_ConfParameters.hxx>
#include <TColStd_SequenceOfAsciiString.hxx>
#include <TColStd_Array1OfAsciiString.hxx>
#include <TColStd_Array1OfReal.hxx>
class XSControl_WorkSession;
class StepData_StepModel;
class StepRepr_RepresentationContext;


//! Reads STEP files, checks them and translates their contents
//! into Open CASCADE models. The STEP data can be that of
//! a whole model or that of a specific list of entities in the model.
//! As in XSControl_Reader, you specify the list using a selection.
//! For the translation of iges files it is possible to use next sequence:
//! To change translation parameters
//! class Interface_Static should be used before beginning of
//! translation  (see STEP Parameters and General Parameters)
//! Creation of reader - STEPControl_Reader reader;
//! To load s file in a model use method reader.ReadFile("filename.stp")
//! To print load results reader.PrintCheckLoad(failsonly,mode)
//! where mode is equal to the value of enumeration IFSelect_PrintCount
//! For definition number of candidates :
//! Standard_Integer nbroots = reader. NbRootsForTransfer();
//! To transfer entities from a model the following methods can be used:
//! for the whole model - reader.TransferRoots();
//! to transfer a list of entities: reader.TransferList(list);
//! to transfer one entity Handle(Standard_Transient)
//! ent = reader.RootForTransfer(num);
//! reader.TransferEntity(ent), or
//! reader.TransferOneRoot(num), or
//! reader.TransferOne(num), or
//! reader.TransferRoot(num)
//! To obtain the result the following method can be used:
//! reader.NbShapes() and reader.Shape(num); or reader.OneShape();
//! To print the results of transfer use method:
//! reader.PrintCheckTransfer(failwarn,mode);
//! where printfail is equal to the value of enumeration
//! IFSelect_PrintFail, mode see above; or reader.PrintStatsTransfer();
//! Gets correspondence between a STEP entity and a result
//! shape obtained from it.
//! Handle(XSControl_WorkSession)
//! WS = reader.WS();
//! if ( WS->TransferReader()->HasResult(ent) )
//! TopoDS_Shape shape = WS->TransferReader()->ShapeResult(ent);
class STEPControl_Reader  : public XSControl_Reader
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a reader object with an empty STEP model.
  Standard_EXPORT STEPControl_Reader();
  
  //! Creates a Reader for STEP from an already existing Session
  //! Clears the session if it was not yet set for STEP
  Standard_EXPORT STEPControl_Reader(const Handle(XSControl_WorkSession)& WS, const Standard_Boolean scratch = Standard_True);
  
  //! Returns the model as a StepModel.
  //! It can then be consulted (header, product)
  Standard_EXPORT Handle(StepData_StepModel) StepModel() const;

  //! Loads a file and returns the read status
  //! Zero for a Model which compies with the Controller
  Standard_EXPORT virtual IFSelect_ReturnStatus ReadFile(const Standard_CString filename) Standard_OVERRIDE;

  //! Loads a file from stream and returns the read status
  Standard_EXPORT virtual IFSelect_ReturnStatus ReadStream(const Standard_CString theName,
                                                           std::istream& theIStream) Standard_OVERRIDE;

  //! Loads a file and returns the read status
  //! Zero for a Model which compies with the Controller
  Standard_EXPORT IFSelect_ReturnStatus ReadFile(const Standard_CString filename,
                                                 const StepData_ConfParameters& theParams);

  //! Loads a file from stream and returns the read status
  Standard_EXPORT IFSelect_ReturnStatus ReadStream(const Standard_CString theName,
                                                   const StepData_ConfParameters& theParams,
                                                   std::istream& theIStream);
  
  //! Transfers a root given its rank in the list of candidate roots
  //! Default is the first one
  //! Returns True if a shape has resulted, false else
  //! Same as inherited TransferOneRoot, kept for compatibility
  Standard_EXPORT Standard_Boolean TransferRoot (const Standard_Integer num = 1,
                                                 const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  //! Determines the list of root entities from Model which are candidate for
  //! a transfer to a Shape (type of entities is PRODUCT)
  Standard_EXPORT virtual Standard_Integer NbRootsForTransfer() Standard_OVERRIDE;
  
  //! Returns sequence of all unit names for shape representations
  //! found in file
  Standard_EXPORT void FileUnits (TColStd_SequenceOfAsciiString& theUnitLengthNames, TColStd_SequenceOfAsciiString& theUnitAngleNames, TColStd_SequenceOfAsciiString& theUnitSolidAngleNames);

  //! Sets system length unit used by transfer process.
  //! Performs only if a model is not NULL
  Standard_EXPORT void SetSystemLengthUnit(const Standard_Real theLengthUnit);

  //! Returns system length unit used by transfer process.
  //! Performs only if a model is not NULL
  Standard_EXPORT Standard_Real SystemLengthUnit() const;


protected:





private:

  
  //! Returns  units for length , angle and solidangle for shape representations
  Standard_EXPORT Standard_Boolean findUnits (const Handle(StepRepr_RepresentationContext)& theReprContext, TColStd_Array1OfAsciiString& theNameUnits, TColStd_Array1OfReal& theFactorUnits);




};







#endif // _STEPControl_Reader_HeaderFile
