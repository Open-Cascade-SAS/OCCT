// Created on: 1994-09-01
// Created by: Marie Jose MARTZ
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _IGESToBRep_Reader_HeaderFile
#define _IGESToBRep_Reader_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <TopTools_SequenceOfShape.hxx>
#include <Standard_Integer.hxx>
#include <Standard_CString.hxx>
#include <Standard_Real.hxx>
class IGESData_IGESModel;
class IGESToBRep_Actor;
class Transfer_TransientProcess;
class TopoDS_Shape;


//! A simple way to read geometric IGES data.
//! Encapsulates reading file and calling transfer tools
class IGESToBRep_Reader 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a Reader
  Standard_EXPORT IGESToBRep_Reader();
  
  //! Loads a Model from a file.Returns 0 if success.
  //! returns 1 if the file could not be opened,
  //! returns -1 if an error occurred while the file was being loaded.
  Standard_EXPORT Standard_Integer LoadFile (const Standard_CString filename);
  
  //! Specifies a Model to work on
  //! Also clears the result and Done status, sets TransientProcess
  Standard_EXPORT void SetModel (const Handle(IGESData_IGESModel)& model);
  
  //! Returns the Model to be worked on.
  Standard_EXPORT Handle(IGESData_IGESModel) Model() const;
  
  //! Allows to set an already defined TransientProcess
  //! (to be called after LoadFile or SetModel)
  Standard_EXPORT void SetTransientProcess (const Handle(Transfer_TransientProcess)& TP);
  
  //! Returns the TransientProcess
  Standard_EXPORT Handle(Transfer_TransientProcess) TransientProcess() const;
  
  //! Returns "theActor"
  Standard_EXPORT Handle(IGESToBRep_Actor) Actor() const;
  
  //! Clears the results between two translation operations.
  Standard_EXPORT void Clear();
  
  //! Checks the IGES file that was
  //! loaded into memory. Displays error messages in the default
  //! message file if withprint is true. Returns True if no fail
  //! message was found and False if there was at least one fail message.
  Standard_EXPORT Standard_Boolean Check (const Standard_Boolean withprint) const;
  
  //! Translates root entities in an
  //! IGES file. Standard_True is the default value and means that only
  //! visible root entities are translated. Standard_False
  //! translates all of the roots (visible and invisible).
  Standard_EXPORT void TransferRoots (const Standard_Boolean onlyvisible = Standard_True);
  
  //! Transfers an Entity given its rank in the Model (Root or not)
  //! Returns True if it is recognized as Geom-Topol.
  //! (But it can have failed : see IsDone)
  Standard_EXPORT Standard_Boolean Transfer (const Standard_Integer num);
  
  //! Returns True if the LAST Transfer/TransferRoots was a success
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns the Tolerance which has been actually used, converted
  //! in millimeters
  //! (either that from File or that from Session, according the mode)
  Standard_EXPORT Standard_Real UsedTolerance() const;
  
  //! Returns the number of shapes produced by the translation.
  Standard_EXPORT Standard_Integer NbShapes() const;
  
  //! Returns the num the resulting shape in a translation operation.
  Standard_EXPORT TopoDS_Shape Shape (const Standard_Integer num = 1) const;
  
  //! Returns all of the results in a
  //! single shape which is:
  //! - a null shape if there are no results,
  //! - a shape if there is one result,
  //! - a compound containing the resulting shapes if there are several.
  Standard_EXPORT TopoDS_Shape OneShape() const;




protected:





private:



  Handle(IGESData_IGESModel) theModel;
  Standard_Boolean theDone;
  TopTools_SequenceOfShape theShapes;
  Handle(IGESToBRep_Actor) theActor;
  Handle(Transfer_TransientProcess) theProc;


};







#endif // _IGESToBRep_Reader_HeaderFile
