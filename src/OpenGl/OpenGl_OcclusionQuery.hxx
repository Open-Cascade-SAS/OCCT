// Created on: 2024-02-20
// Created by: Hossam Ali
// Copyright (c) 2024 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in
// OCCT distribution for complete text of the license and disclaimer of any
// warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef OpenGl_OcclusionQuery_HeaderFile
#define OpenGl_OcclusionQuery_HeaderFile

#include <OpenGl_Resource.hxx>
#include <OpenGl_GlCore15.hxx>

class OpenGl_Context;

DEFINE_STANDARD_HANDLE(OpenGl_OcclusionQuery, OpenGl_Resource)

//! Implementation of OpenGl view.
class OpenGl_OcclusionQuery : public OpenGl_Resource {
public:
  DEFINE_STANDARD_RTTIEXT(OpenGl_OcclusionQuery, OpenGl_Resource)

  //! Constructor.
  Standard_EXPORT OpenGl_OcclusionQuery();

  //! Default destructor.
  Standard_EXPORT virtual ~OpenGl_OcclusionQuery();
  
  //! Create the occlusion test Gl resources.
  Standard_EXPORT virtual void Create(const Handle(OpenGl_Context)& theCtx, GLenum theType);

  //! Begins occlusion query.
  Standard_EXPORT virtual void Begin(const Handle(OpenGl_Context) & theCtx);

  //! Ends occlusion query
  Standard_EXPORT virtual void End(const Handle(OpenGl_Context) & theCtx) const;

  //! Check if the query results ready for retrive or not.
  Standard_EXPORT virtual int IsResultsReady(const Handle(OpenGl_Context) & theCtx) const;

  //! Return the query results based on query type.
  Standard_EXPORT virtual int GetResults(const Handle(OpenGl_Context) & theCtx);

  //! Return is the query in use or not
  Standard_EXPORT bool IsInUse() { return inUse; }

  //! Return Query ID
  Standard_EXPORT unsigned int GetID() const {return myID;}; 

  //! Destroys query object.
  Standard_EXPORT virtual void Release (OpenGl_Context * theCtx) Standard_OVERRIDE;

  //! Returns estimated GPU memory usage - not implemented.
  virtual Standard_Size EstimatedDataSize() const Standard_OVERRIDE { return 0; }

private:
  GLuint   myID;                     //!< OpenGL query ID.
  GLenum   myType;                   //!< OpenGL query Type.
  bool     inUse;                    //!< store bool answer is the query still inprogress or finished.
  bool     started;                  //!< bool answer is the query stated before or this the first time.
};

#endif // _OpenGl_OcclusionQuery_Header