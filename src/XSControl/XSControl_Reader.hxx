// Created on: 1997-05-14
// Created by: Christian CAILLET
// Copyright (c) 1997-1999 Matra Datavision
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

#ifndef _XSControl_Reader_HeaderFile
#define _XSControl_Reader_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TColStd_SequenceOfTransient.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <Standard_CString.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <TColStd_HSequenceOfTransient.hxx>
#include <Standard_Integer.hxx>
#include <IFSelect_PrintCount.hxx>
#include <Message_ProgressRange.hxx>

class XSControl_WorkSession;
class Interface_InterfaceModel;
class Standard_Transient;
class TopoDS_Shape;


//! A groundwork to convert a shape to data which complies
//! with a particular norm. This data can be that of a whole
//! model or that of a specific list of entities in the model.
//! You specify the list using a single selection or a
//! combination of selections. A selection is an operator which
//! computes a list of entities from a list given in input. To
//! specify the input, you can use:
//! - A predefined selection such as "xst-transferrable-roots"
//! - A filter based on a  signature.
//! A signature is an operator which returns a string from an
//! entity according to its type.
//! For example:
//! - "xst-type" (CDL)
//! - "iges-level"
//! - "step-type".
//! A filter can be based on a signature by giving a value to
//! be matched by the string returned. For example,
//! "xst-type(Curve)".
//! If no list is specified, the selection computes its list of
//! entities from the whole model. To use this class, you have to
//! initialize the transfer norm first, as shown in the example below.
//! Example:
//! Control_Reader reader;
//! IFSelect_ReturnStatus status = reader.ReadFile (filename.);
//! When using IGESControl_Reader or STEPControl_Reader - as the
//! above example shows - the reader initializes the norm directly.
//! Note that loading the file only stores the data. It does
//! not translate this data. Shapes are accumulated by
//! successive transfers. The last shape is cleared by:
//! - ClearShapes which allows you to handle a new batch
//! - TransferRoots which restarts the list of shapes from scratch.
class XSControl_Reader 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a Reader from scratch (creates an empty WorkSession)
  //! A WorkSession or a Controller must be provided before running
  Standard_EXPORT XSControl_Reader();
  
  //! Creates a Reader from scratch, with a norm name which
  //! identifies a Controller
  Standard_EXPORT XSControl_Reader(const Standard_CString norm);
  
  //! Creates a Reader from an already existing Session, with a
  //! Controller already set
  //! Virtual destructor
  Standard_EXPORT XSControl_Reader(const Handle(XSControl_WorkSession)& WS, const Standard_Boolean scratch = Standard_True);

  //! Empty virtual destructor
  virtual ~XSControl_Reader() {}
  
  //! Sets a specific norm to <me>
  //! Returns True if done, False if <norm> is not available
  Standard_EXPORT Standard_Boolean SetNorm (const Standard_CString norm);
  
  //! Sets a specific session to <me>
  Standard_EXPORT void SetWS (const Handle(XSControl_WorkSession)& WS, const Standard_Boolean scratch = Standard_True);
  
  //! Returns the session used in <me>
  Standard_EXPORT Handle(XSControl_WorkSession) WS() const;
  
  //! Loads a file and returns the read status
  //! Zero for a Model which compies with the Controller
  Standard_EXPORT virtual IFSelect_ReturnStatus ReadFile (const Standard_CString filename);

  //! Loads a file from stream and returns the read status
  Standard_EXPORT virtual IFSelect_ReturnStatus ReadStream(const Standard_CString theName, std::istream& theIStream);
  
  //! Returns the model. It can then be consulted (header, product)
  Standard_EXPORT Handle(Interface_InterfaceModel) Model() const;
  
  //! Returns a list of entities from the IGES or STEP file
  //! according to the following rules:
  //! - if first and second are empty strings, the whole file is selected.
  //! - if first is an entity number or label, the entity referred to is selected.
  //! - if first is a list of entity numbers/labels separated by commas, the entities referred to are selected,
  //! - if first is the name of a selection in the worksession and second is not defined,
  //! the list contains the standard output for that selection.
  //! - if first is the name of a selection and second is defined, the criterion defined
  //! by second is applied to the result of the first selection.
  //! A selection is an operator which computes a list of entities from a list given in
  //! input according to its type. If no list is specified, the selection computes its
  //! list of entities from the whole model.
  //! A selection can be:
  //! - A predefined selection (xst-transferrable-mode)
  //! - A filter based on a signature
  //! A Signature is an operator which returns a string from an entity according to its type. For example:
  //! - "xst-type" (CDL)
  //! - "iges-level"
  //! - "step-type".
  //! For example, if you wanted to select only the advanced_faces in a STEP file you
  //! would use the following code:
  //! Example
  //! Reader.GiveList("xst-transferrable-roots","step-type(ADVANCED_FACE)");
  //! Warning
  //! If the value given to second is incorrect, it will simply be ignored.
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) GiveList (const Standard_CString first = "", const Standard_CString second = "");
  
  //! Computes a List of entities from the model as follows
  //! <first> being a Selection, <ent> being an entity or a list
  //! of entities (as a HSequenceOfTransient) :
  //! the standard result of this selection applied to this list
  //! if <first> is erroneous, a null handle is returned
  Standard_EXPORT Handle(TColStd_HSequenceOfTransient) GiveList (const Standard_CString first, const Handle(Standard_Transient)& ent);
  
  //! Determines the list of root entities which are candidate for
  //! a transfer to a Shape, and returns the number
  //! of entities in the list
  Standard_EXPORT virtual Standard_Integer NbRootsForTransfer();
  
  //! Returns an IGES or STEP root
  //! entity for translation. The entity is identified by its
  //! rank in a list.
  Standard_EXPORT Handle(Standard_Transient) RootForTransfer (const Standard_Integer num = 1);
  
  //! Translates a root identified by the rank num in the model.
  //! false is returned if no shape is produced.
  Standard_EXPORT Standard_Boolean TransferOneRoot (const Standard_Integer num = 1,
                                                    const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  //! Translates an IGES or STEP
  //! entity identified by the rank num in the model.
  //! false is returned if no shape is produced.
  Standard_EXPORT Standard_Boolean TransferOne (const Standard_Integer num,
                                                const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  //! Translates an IGES or STEP
  //! entity in the model. true is returned if a shape is
  //! produced; otherwise, false is returned.
  Standard_EXPORT Standard_Boolean TransferEntity (const Handle(Standard_Transient)& start,
                                                   const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  //! Translates a list of entities.
  //! Returns the number of IGES or STEP entities that were
  //! successfully translated. The list can be produced with GiveList.
  //! Warning - This function does not clear the existing output shapes.
  Standard_EXPORT Standard_Integer TransferList (const Handle(TColStd_HSequenceOfTransient)& list,
                                                 const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  //! Translates all translatable
  //! roots and returns the number of successful translations.
  //! Warning - This function clears existing output shapes first.
  Standard_EXPORT Standard_Integer TransferRoots(const Message_ProgressRange& theProgress = Message_ProgressRange());
  
  //! Clears the list of shapes that
  //! may have accumulated in calls to TransferOne or TransferRoot.C
  Standard_EXPORT void ClearShapes();
  
  //! Returns the number of shapes produced by translation.
  Standard_EXPORT Standard_Integer NbShapes() const;
  
  //! Returns the shape resulting
  //! from a translation and identified by the rank num.
  //! num equals 1 by default. In other words, the first shape
  //! resulting from the translation is returned.
  Standard_EXPORT TopoDS_Shape Shape (const Standard_Integer num = 1) const;
  
  //! Returns all of the results in
  //! a single shape which is:
  //! - a null shape if there are no results,
  //! - a shape if there is one result,
  //! - a compound containing the resulting shapes if there are more than one.
  Standard_EXPORT TopoDS_Shape OneShape() const;
  
  //! Prints the check list attached to loaded data, on the Standard
  //! Trace File (starts at std::cout)
  //! All messages or fails only, according to <failsonly>
  //! mode = 0 : per entity, prints messages
  //! mode = 1 : per message, just gives count of entities per check
  //! mode = 2 : also gives entity numbers
  Standard_EXPORT void PrintCheckLoad (const Standard_Boolean failsonly, const IFSelect_PrintCount mode) const;

  //! Prints the check list attached to loaded data.
  Standard_EXPORT void PrintCheckLoad (Standard_OStream& theStream, const Standard_Boolean failsonly, const IFSelect_PrintCount mode) const;
  
  //! Displays check results for the
  //! last translation of IGES or STEP entities to Open CASCADE
  //! entities. Only fail messages are displayed if failsonly is
  //! true. All messages are displayed if failsonly is
  //! false. mode determines the contents and the order of the
  //! messages according to the terms of the IFSelect_PrintCount enumeration.
  Standard_EXPORT void PrintCheckTransfer (const Standard_Boolean failsonly, const IFSelect_PrintCount mode) const;

  //! Displays check results for the last translation of IGES or STEP entities to Open CASCADE entities.
  Standard_EXPORT void PrintCheckTransfer (Standard_OStream& theStream, const Standard_Boolean failsonly, const IFSelect_PrintCount mode) const;
  
  //! Displays the statistics for
  //! the last translation. what defines the kind of
  //! statistics that are displayed as follows:
  //! - 0 gives general statistics (number of translated roots,
  //! number of warnings, number of fail messages),
  //! - 1 gives root results,
  //! - 2 gives statistics for all checked entities,
  //! - 3 gives the list of translated entities,
  //! - 4 gives warning and fail messages,
  //! - 5 gives fail messages only.
  //! The use of mode depends on the value of what. If what is 0,
  //! mode is ignored. If what is 1, 2 or 3, mode defines the following:
  //! - 0 lists the numbers of IGES or STEP entities in the respective model
  //! - 1 gives the number, identifier, type and result
  //! type for each IGES or STEP entity and/or its status
  //! (fail, warning, etc.)
  //! - 2 gives maximum information for each IGES or STEP entity (i.e. checks)
  //! - 3 gives the number of entities per type of IGES or STEP entity
  //! - 4 gives the number of IGES or STEP entities per result type and/or status
  //! - 5 gives the number of pairs (IGES or STEP or result type and status)
  //! - 6 gives the number of pairs (IGES or STEP or result type
  //! and status) AND the list of entity numbers in the IGES or STEP model.
  //! If what is 4 or 5, mode defines the warning and fail
  //! messages as follows:
  //! - if mode is 0 all warnings and checks per entity are returned
  //! - if mode is 2 the list of entities per warning is returned.
  //! If mode is not set, only the list of all entities per warning is given.
  Standard_EXPORT void PrintStatsTransfer (const Standard_Integer what, const Standard_Integer mode = 0) const;

  //! Displays the statistics for the last translation.
  Standard_EXPORT void PrintStatsTransfer (Standard_OStream& theStream, const Standard_Integer what, const Standard_Integer mode = 0) const;
  
  //! Gives statistics about Transfer
  Standard_EXPORT void GetStatsTransfer (const Handle(TColStd_HSequenceOfTransient)& list, Standard_Integer& nbMapped, Standard_Integer& nbWithResult, Standard_Integer& nbWithFail) const;




protected:

  
  //! Returns a sequence of produced shapes
  Standard_EXPORT TopTools_SequenceOfShape& Shapes();


  Standard_Boolean therootsta;
  TColStd_SequenceOfTransient theroots;


private:



  Handle(XSControl_WorkSession) thesession;
  TopTools_SequenceOfShape theshapes;


};







#endif // _XSControl_Reader_HeaderFile
