// Created on: 2009-05-05
// Created by: Sergey ZARITCHNY
// Copyright (c) 2009-2014 OPEN CASCADE SAS
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

#include <DNaming_SelectionDriver.hxx>
#include <ModelDefinitions.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_Integer.hxx>
#include <TDF_Label.hxx>
#include <TFunction_Function.hxx>
#include <TFunction_Logbook.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_Selector.hxx>
#include <TNaming_Tool.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DNaming_SelectionDriver, TFunction_Driver)

// #define SEL_DEB 1
#ifdef OCCT_DEBUG
  #include <TDF_Tool.hxx>
  #include <NCollection_Map.hxx>
#endif
//=================================================================================================

DNaming_SelectionDriver::DNaming_SelectionDriver() = default;

//=======================================================================
// function : Validate
// purpose  : Validates labels of a function in <theLog>.
//=======================================================================
void DNaming_SelectionDriver::Validate(occ::handle<TFunction_Logbook>&) const {}

//=======================================================================
// function : MustExecute
// purpose  : Analyse in <theLog> if the loaded function must be
//           executed (i.e.arguments are modified) or not.
//=======================================================================
bool DNaming_SelectionDriver::MustExecute(const occ::handle<TFunction_Logbook>&) const
{
  return true;
}

#ifdef OCCT_DEBUG
  #include <BRepTools.hxx>

static void Write(const TopoDS_Shape& shape, const char* filename)
{
  std::ofstream save;
  save.open(filename);
  save << "DBRep_DrawableShape" << std::endl << std::endl;
  if (!shape.IsNull())
    BRepTools::Write(shape, save);
  save.close();
}
#endif
//=======================================================================
// function : Execute
// purpose  : Execute the function and push in <theLog> the impacted
//           labels (see method SetImpacted).
//=======================================================================
#include <TNaming_NamedShape.hxx>
#include <NCollection_List.hxx>
#include <Standard_Handle.hxx>
#include <TDF_Attribute.hxx>
#include <NCollection_Map.hxx>
#include <TCollection_AsciiString.hxx>

int DNaming_SelectionDriver::Execute(occ::handle<TFunction_Logbook>& theLog) const
{
  occ::handle<TFunction_Function> aFunction;
  Label().FindAttribute(TFunction_Function::GetID(), aFunction);
  if (aFunction.IsNull())
    return -1;

  TDF_Label aRLabel = RESPOSITION(aFunction);
  if (aRLabel.IsNull())
    return -1;

  bool                            aIsWire        = false;
  TopAbs_ShapeEnum                aPrevShapeType = TopAbs_SHAPE;
  occ::handle<TNaming_NamedShape> aNShape;
  if (aRLabel.FindAttribute(TNaming_NamedShape::GetID(), aNShape))
  {
    if (!aNShape.IsNull() && !aNShape->IsEmpty())
    {
      aPrevShapeType = aNShape->Get().ShapeType();
      if (aPrevShapeType == TopAbs_WIRE)
        aIsWire = true;
    }
  }

  TNaming_Selector aSelector(aRLabel);

  NCollection_Map<TDF_Label> aMap;
  theLog->GetValid(aMap);
#ifdef OCCT_DEBUG
  std::cout << "#E_DNaming_SelectionDriver:: Valid Label Map:" << std::endl;
  NCollection_Map<TDF_Label>::Iterator anItr(aMap);
  for (; anItr.More(); anItr.Next())
  {
    const TDF_Label&        aLabel = anItr.Key();
    TCollection_AsciiString anEntry;
    TDF_Tool::Entry(aLabel, anEntry);
    std::cout << "\tLabel = " << anEntry << std::endl;
  }
#endif
  //***
  //  TDF_IDFilter aFilterForReferers;
  //  aFilterForReferers.Keep(TNaming_NamedShape::GetID());
  //  TDF_IDFilter aFilterForReferences;
  //  aFilterForReferences.Keep(TNaming_NamedShape::GetID());
  //   NCollection_Map<TDF_Label> aMap1;
  //  TDF_Tool::OutReferences(aLabel, /*aFilterForReferers, aFilterForReferences, */outRefs);
  //***

  if (aSelector.Solve(aMap))
  {
    theLog->SetValid(aRLabel);
    occ::handle<TNaming_NamedShape> aNS;
    if (!aRLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS))
    {
      std::cout << "%%%WARNING: DNaming_SelectionDriver::NamedShape is not found" << std::endl;
    }
    else
    {
      if (aNS.IsNull())
      {
        std::cout << "%%%WARNING: DNaming_SelectionDriver::NamedShape is NULL" << std::endl;
      }
      else if (aNS->IsEmpty())
      {
        std::cout << "%%%WARNING: DNaming_SelectionDriver::NamedShape is EMPTY on Label = ";
        aNS->Label().EntryDump(std::cout);
        std::cout << std::endl;
      }
      else
      {
#ifdef OCCT_DEBUG
        Write(aNS->Get(), "Selection_Result.brep");
        std::cout << "TShape = " << aNS->Get().TShape().get() << std::endl;
#endif
        if (aIsWire && aNS->Get().ShapeType() == TopAbs_COMPOUND)
        {
          TopoDS_Shape aWireShape;
          TNaming_Tool::FindShape(aMap, aMap, aNS, aWireShape);
          TNaming_Builder aBuilder(aRLabel);
          aBuilder.Select(aWireShape, aWireShape);
          aFunction->SetFailure(DONE);
        }
        else if (aPrevShapeType == aNS->Get().ShapeType())
        {
          aFunction->SetFailure(DONE);
        }
        else
        {
#ifdef OCCT_DEBUG
          std::cout << "%%%WARNING: DNaming_SelectionDriver::Execute: The Shape after solving "
                       "changed type = "
                    << aNS->Get().ShapeType() << std::endl;
#endif
          aFunction->SetFailure(DONE);
        }
      }
    }
  }
  else
  {
    aFunction->SetFailure(NOTDONE);
    std::cout << "%%%WARNING: DNaming_SelectionDriver::Execute: Selection is Not solved !!!"
              << std::endl;
    return 1;
  }
  return 0;
}
