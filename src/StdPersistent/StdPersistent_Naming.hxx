// Copyright (c) 2015 OPEN CASCADE SAS
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


#ifndef _StdPersistent_Naming_HeaderFile
#define _StdPersistent_Naming_HeaderFile

#include <StdObjMgt_Attribute.hxx>
#include <StdObjMgt_Persistent.hxx>
#include <StdPersistent_HArray1.hxx>
#include <StdLPersistent_HArray1.hxx>
#include <StdLPersistent_HString.hxx>

#include <TNaming_NamedShape.hxx>
#include <TNaming_Naming.hxx>

class TNaming_Name;


class StdPersistent_Naming
{
public:
  class NamedShape : public StdObjMgt_Attribute<TNaming_NamedShape>
  {
  public:
    //! Read persistent data from a file.
    inline void Read (StdObjMgt_ReadData& theReadData)
      { theReadData >> myOldShapes >> myNewShapes >> myShapeStatus >> myVersion; }
    //! Read persistent data from a file.
    inline void Write (StdObjMgt_WriteData& theWriteData) const
      { theWriteData << myOldShapes << myNewShapes << myShapeStatus << myVersion; }
    //! Gets persistent child objects
    inline void PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
    {
      if (!myOldShapes.IsNull()) theChildren.Append(myOldShapes);
      if (!myNewShapes.IsNull()) theChildren.Append(myNewShapes);
    }
    //! Returns persistent type name
    inline Standard_CString PName() const { return "PNaming_NamedShape"; }

    //! Import transient attribuite from the persistent data.
    void Import (const Handle(TNaming_NamedShape)& theAttribute) const;

  private:
    Handle(StdPersistent_HArray1::Shape1) myOldShapes;
    Handle(StdPersistent_HArray1::Shape1) myNewShapes;
    Standard_Integer                      myShapeStatus;
    Standard_Integer                      myVersion;
  };

  class Name : public StdObjMgt_Persistent
  {
  public:
    //! Read persistent data from a file.
    Standard_EXPORT virtual void Read (StdObjMgt_ReadData& theReadData);
    //! Read persistent data from a file.
    Standard_EXPORT virtual void Write (StdObjMgt_WriteData& theWriteData) const;
    //! Gets persistent child objects
    inline void PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
    {
      if (!myArgs.IsNull()) theChildren.Append(myArgs);
      if (!myStop.IsNull()) theChildren.Append(myStop);
    }
    //! Returns persistent type name
    inline Standard_CString PName() const { return "PNaming_Name"; }

    //! Import transient object from the persistent data.
    Standard_EXPORT virtual void Import
      (TNaming_Name& theName, const Handle(TDF_Data)& theDF) const;

  private:
    Standard_Integer                           myType;
    Standard_Integer                           myShapeType;
    Handle(StdLPersistent_HArray1::Persistent) myArgs;
    Handle(StdObjMgt_Persistent)               myStop;
    Standard_Integer                           myIndex;
  };

  class Name_1 : public Name
  {
  public:
    //! Read persistent data from a file.
    Standard_EXPORT virtual void Read (StdObjMgt_ReadData& theReadData);
    //! Read persistent data from a file.
    Standard_EXPORT virtual void Write (StdObjMgt_WriteData& theWriteData) const;
    //! Gets persistent child objects
    inline void PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
    {
      Name::PChildren(theChildren);
      if (!myContextLabel.IsNull()) theChildren.Append(myContextLabel);
    }
    //! Returns persistent type name
    inline Standard_CString PName() const { return "PNaming_Name_1"; }

    //! Import transient object from the persistent data.
    Standard_EXPORT virtual void Import
      (TNaming_Name& theName, const Handle(TDF_Data)& theDF) const;

  private:
    Handle(StdLPersistent_HString::Ascii) myContextLabel;
  };

  class Name_2 : public Name_1
  {
  public:
    //! Read persistent data from a file.
    Standard_EXPORT virtual void Read (StdObjMgt_ReadData& theReadData);
    //! Read persistent data from a file.
    Standard_EXPORT virtual void Write (StdObjMgt_WriteData& theWriteData) const;
    //! Gets persistent child objects
    inline void PChildren(StdObjMgt_Persistent::SequenceOfPersistent& theChildren) const
      { Name_1::PChildren(theChildren); }
    //! Returns persistent type name
    inline Standard_CString PName() const { return "PNaming_Name_2"; }

    //! Import transient object from the persistent data.
    Standard_EXPORT virtual void Import
      (TNaming_Name& theName, const Handle(TDF_Data)& theDF) const;

  private:
    Standard_Integer myOrientation;
  };

  class Naming : public StdObjMgt_Attribute<TNaming_Naming>::SingleRef
  {
  public:
    //! Import transient attribuite from the persistent data.
    Standard_EXPORT virtual void ImportAttribute();
  };

  class Naming_1 : public Naming
  {
  public:
    //! Import transient attribuite from the persistent data.
    Standard_EXPORT virtual void ImportAttribute();
  };

  typedef Naming Naming_2;
};

#endif
