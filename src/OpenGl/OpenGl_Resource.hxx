// File:      OpenGl_Resource.hxx
// Created:   18.03.11 9:20:00
// Author:    Anton POLETAEV

#ifndef _OPENGL_RESOURCE_H
#define _OPENGL_RESOURCE_H

#include <OpenGl_ResourceCleaner.hxx>
#include <MMgt_TShared.hxx>
#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <Handle_MMgt_TShared.hxx>

class Standard_Transient;
class Handle(Standard_Type);
class Handle(MMgt_TShared);
class OpenGl_ResourceCleaner;

//! Class represents basic OpenGl memory resource, which
//! could be removed only if appropriate context is avaliable;
//! The cleaning procedure is done by OpenGl_ResourceCleaner
class OpenGl_Resource : public MMgt_TShared
{

public:

  //! Constructor
  OpenGl_Resource() : myId(0) { }
 
  //! Constructor 
  OpenGl_Resource(GLuint theId) : myId(theId) { }

  //! Copy constructor
  OpenGl_Resource(const OpenGl_Resource& theBase) : myId(theBase.myId) { }

  //! Copy operation
  OpenGl_Resource& operator = (const OpenGl_Resource& theBase)
  {
    this->myId = theBase.myId;
    return *this; 
  }

  //! Destructor
  virtual ~OpenGl_Resource() { }

  //! method clean() is accessible only by OpenGl_ResourceCleaner
  friend class OpenGl_ResourceCleaner;

protected:

  //! Clean procedure, should be called only by OpenGl_ResourceCleaner;
  //! Each type of resource has its own cleaning procedure
  virtual void Clean() = 0;

protected:

  GLuint myId; // Id of OpenGl memory resource

};

DEFINE_STANDARD_HANDLE(OpenGl_Resource,MMgt_TShared)

#endif
