// Created on: 1998-09-07
// Created by: Christian CAILLET
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _IGESControl_ActorWrite_HeaderFile
#define _IGESControl_ActorWrite_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Transfer_ActorOfFinderProcess.hxx>
class Transfer_Finder;
class Transfer_Binder;
class Transfer_FinderProcess;


class IGESControl_ActorWrite;
DEFINE_STANDARD_HANDLE(IGESControl_ActorWrite, Transfer_ActorOfFinderProcess)

//! Actor to write Shape to IGES
class IGESControl_ActorWrite : public Transfer_ActorOfFinderProcess
{

public:

  
  Standard_EXPORT IGESControl_ActorWrite();
  
  //! Recognizes a ShapeMapper
  Standard_EXPORT virtual Standard_Boolean Recognize (const Handle(Transfer_Finder)& start) Standard_OVERRIDE;
  
  //! Transfers Shape to IGES Entities
  //!
  //! ModeTrans may be : 0 -> groups of Faces
  //! or 1 -> BRep
  Standard_EXPORT virtual Handle(Transfer_Binder) Transfer
                   (const Handle(Transfer_Finder)& start,
                    const Handle(Transfer_FinderProcess)& FP,
                    const Message_ProgressRange& theProgress = Message_ProgressRange()) Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(IGESControl_ActorWrite,Transfer_ActorOfFinderProcess)

protected:




private:




};







#endif // _IGESControl_ActorWrite_HeaderFile
