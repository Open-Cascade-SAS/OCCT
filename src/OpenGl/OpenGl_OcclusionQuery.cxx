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

#include <OpenGl_OcclusionQuery.hxx>

#include <OpenGl_Context.hxx>
#include <OpenGl_GlCore15.hxx>

IMPLEMENT_STANDARD_RTTIEXT(OpenGl_OcclusionQuery, OpenGl_Resource)

// =======================================================================
// function :
// purpose  :
// =======================================================================
OpenGl_OcclusionQuery::OpenGl_OcclusionQuery(): myID(0), inUse(false), started(false)
{
}

// =======================================================================
// function :
// purpose  :
// =======================================================================
OpenGl_OcclusionQuery::~OpenGl_OcclusionQuery() 
{
  Release(NULL);
}
// =======================================================================
// function : Create()
// purpose  :
// =======================================================================
void OpenGl_OcclusionQuery::Create(const Handle(OpenGl_Context) & theCtx,
                                   GLenum theQueryType) {
  theCtx->core15->glGenQueries(1, &myID);
  myType = theQueryType;
  inUse = false;
  started = false;
}
// =======================================================================
// function : Begin()
// purpose  :
// =======================================================================
void OpenGl_OcclusionQuery::Begin(const Handle(OpenGl_Context) & theCtx) 
{
  inUse = true;
  started = true;
  theCtx->core15->glBeginQuery(myType, myID); 
}
 
// =======================================================================
// function : End()
// purpose  :
// =======================================================================
void OpenGl_OcclusionQuery::End(const Handle(OpenGl_Context) & theCtx) const
{
  theCtx->core15->glEndQuery(myType);
}

// =======================================================================
// function : isResultsReady()
// purpose  :
// =======================================================================
int OpenGl_OcclusionQuery::IsResultsReady(const Handle(OpenGl_Context) & theCtx) const
{
  // check if the query started before check for results
  if(!started)
    return false;

  GLint aReady = 0;
  theCtx->core15->glGetQueryObjectiv(myID, GL_QUERY_RESULT_AVAILABLE, &aReady);
  return aReady;
  }

// =======================================================================
// function : GetResults()
// purpose  :
// =======================================================================
int OpenGl_OcclusionQuery::GetResults(const Handle(OpenGl_Context) & theCtx)
{
  inUse = false;
  GLint aResult;
  theCtx->core15->glGetQueryObjectiv(myID, GL_QUERY_RESULT, &aResult); 
  return aResult;
}

// =======================================================================
// function : Release()
// purpose  : Destroy the query opengl resources 
// =======================================================================
void OpenGl_OcclusionQuery::Release(OpenGl_Context *theCtx) {
  if (myID == 0)
    return;
  if (theCtx != NULL) {
    theCtx->core15->glDeleteQueries(1, &myID);
    myID = 0;
    started = false;
  }
}