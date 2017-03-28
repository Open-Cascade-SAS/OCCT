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


#ifndef _ShapePersistent_TopoDS_HeaderFile
#define _ShapePersistent_TopoDS_HeaderFile

#include <ShapePersistent_TriangleMode.hxx>

#include <StdPersistent_TopoDS.hxx>
#include <StdPersistent_HArray1.hxx>
#include <StdLPersistent_HArray1.hxx>
#include <StdObject_Shape.hxx>
#include <StdObjMgt_TransientPersistentMap.hxx>

#include <TopoDS_TWire.hxx>
#include <TopoDS_TShell.hxx>
#include <TopoDS_TSolid.hxx>
#include <TopoDS_TCompSolid.hxx>
#include <TopoDS_TCompound.hxx>


class ShapePersistent_TopoDS : public StdPersistent_TopoDS
{
public:
  class HShape : public StdObjMgt_Persistent, public StdObject_Shape
  {
  public:
    //! Read persistent data from a file.
    Standard_EXPORT virtual void Read (StdObjMgt_ReadData& theReadData);
    //! Write persistent data to a file
    Standard_EXPORT virtual void Write (StdObjMgt_WriteData& theWriteData) const;
    //! Gets persistent child objects
    Standard_EXPORT virtual void PChildren(SequenceOfPersistent& theChildren) const;
    //! Returns persistent type name
    Standard_EXPORT virtual Standard_CString PName() const { return "PTopoDS_HShape"; }

  private:
    Handle(StdObjMgt_Persistent) myEntry;
  };

protected:
  class pTBase : public pTShape
  {
  public:
    inline Handle(TopoDS_TShape) Import() const
    {
      Handle(TopoDS_TShape) aTShape = createTShape();

      TopoDS_Shape aWrapperShape;
      aWrapperShape.TShape (aTShape);
      addShapes (aWrapperShape);

      setFlags (aTShape);

      return aTShape;
    }

  private:
    virtual Handle(TopoDS_TShape) createTShape() const = 0;

    virtual void addShapes (TopoDS_Shape& theParent) const = 0;

    void setFlags (const Handle(TopoDS_TShape)& theTShape) const;

  protected:
    template <class ShapesArray>
    void addShapesT (TopoDS_Shape& theParent) const;
  };

private:
  template <class Target>
  class pTSimple : public pTBase
  { 
    virtual Handle(TopoDS_TShape) createTShape() const; 
  public:
    inline Standard_CString PName() const;
  };

  template <class Persistent, class ShapesArray>
  class pTObject : public Persistent
  {
    virtual void addShapes (TopoDS_Shape& theParent) const
      { pTBase::addShapesT<ShapesArray> (theParent); }
  };

  template <class Persistent, class ShapesArray>
  struct tObjectT : public Delayed <DelayedBase<TShape, TopoDS_TShape, pTBase>,
                                    pTObject<Persistent, ShapesArray> > 
    { typedef pTObject<Persistent, ShapesArray> pTObjectT; };

protected:
  template <class Persistent>
  struct tObject : public tObjectT<Persistent, StdLPersistent_HArray1::Persistent> { };

  template <class Persistent>
  struct tObject1 : public tObjectT<Persistent, StdPersistent_HArray1::Shape1> { };

public:
  typedef tObject  <pTSimple<TopoDS_TWire>      > TWire;
  typedef tObject  <pTSimple<TopoDS_TShell>     > TShell;
  typedef tObject  <pTSimple<TopoDS_TSolid>     > TSolid;
  typedef tObject  <pTSimple<TopoDS_TCompSolid> > TCompSolid;
  typedef tObject  <pTSimple<TopoDS_TCompound>  > TCompound;

  typedef tObject1 <pTSimple<TopoDS_TWire>      > TWire1;
  typedef tObject1 <pTSimple<TopoDS_TShell>     > TShell1;
  typedef tObject1 <pTSimple<TopoDS_TSolid>     > TSolid1;
  typedef tObject1 <pTSimple<TopoDS_TCompSolid> > TCompSolid1;
  typedef tObject1 <pTSimple<TopoDS_TCompound>  > TCompound1;

public:
  //! Create a persistent object for a shape
  Standard_EXPORT static Handle(HShape) Translate (const TopoDS_Shape& theShape,
                                                   StdObjMgt_TransientPersistentMap& theMap,
                                                   ShapePersistent_TriangleMode theTriangleMode);
};

template<>
inline Standard_CString ShapePersistent_TopoDS::pTSimple<TopoDS_TWire>::PName() const 
  { return "PTopoDS_TWire"; }

template<>
inline Standard_CString ShapePersistent_TopoDS::pTSimple<TopoDS_TShell>::PName() const
  { return "PTopoDS_TShell"; }

template<>
inline Standard_CString ShapePersistent_TopoDS::pTSimple<TopoDS_TSolid>::PName() const
  { return "PTopoDS_TSolid"; }

template<>
inline Standard_CString ShapePersistent_TopoDS::pTSimple<TopoDS_TCompSolid>::PName() const
  { return "PTopoDS_TCompSolid"; }

template<>
inline Standard_CString ShapePersistent_TopoDS::pTSimple<TopoDS_TCompound>::PName() const
  { return "PTopoDS_TCompound"; }

#endif
