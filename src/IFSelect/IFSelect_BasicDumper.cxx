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


#include <IFSelect_BasicDumper.hxx>
#include <IFSelect_DispGlobal.hxx>
#include <IFSelect_DispPerCount.hxx>
#include <IFSelect_DispPerOne.hxx>
#include <IFSelect_IntParam.hxx>
#include <IFSelect_Modifier.hxx>
#include <IFSelect_SelectDiff.hxx>
#include <IFSelect_SelectEntityNumber.hxx>
#include <IFSelect_SelectErrorEntities.hxx>
#include <IFSelect_SelectIncorrectEntities.hxx>
#include <IFSelect_SelectIntersection.hxx>
#include <IFSelect_SelectModelEntities.hxx>
#include <IFSelect_SelectModelRoots.hxx>
#include <IFSelect_SelectPointed.hxx>
#include <IFSelect_SelectRange.hxx>
#include <IFSelect_SelectRootComps.hxx>
#include <IFSelect_SelectRoots.hxx>
#include <IFSelect_SelectShared.hxx>
#include <IFSelect_SelectSharing.hxx>
#include <IFSelect_SelectUnion.hxx>
#include <IFSelect_SelectUnknownEntities.hxx>
#include <IFSelect_SessionFile.hxx>
#include <IFSelect_ShareOut.hxx>
#include <IFSelect_TransformStandard.hxx>
#include <IFSelect_WorkSession.hxx>
#include <Interface_Macros.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_BasicDumper,IFSelect_SessionDumper)

//#include <IFSelect_SelectTextType.hxx>
#define FIRSTCHAR 1
// Param litteral "own" sous la forme  :"<val>" -> first = 3
// A present, forme simplifiee         : <val>  directement -> first = 1



IFSelect_BasicDumper::IFSelect_BasicDumper ()  {  }

    Standard_Boolean  IFSelect_BasicDumper::WriteOwn
  (IFSelect_SessionFile& file, const Handle(Standard_Transient)& item) const
{
  Handle(Standard_Type) type = item->DynamicType();
  if (type == STANDARD_TYPE(IFSelect_SelectModelRoots))      return Standard_True;
  if (type == STANDARD_TYPE(IFSelect_SelectModelEntities))   return Standard_True;
  if (type == STANDARD_TYPE(IFSelect_SelectEntityNumber)) {
    DeclareAndCast(IFSelect_SelectEntityNumber,sen,item);
    file.SendItem(sen->Number());
    return Standard_True;
  }
  if (type == STANDARD_TYPE(IFSelect_SelectPointed))         return Standard_True;
  if (type == STANDARD_TYPE(IFSelect_SelectUnion))           return Standard_True;
  if (type == STANDARD_TYPE(IFSelect_SelectIntersection))    return Standard_True;
  if (type == STANDARD_TYPE(IFSelect_SelectDiff))            return Standard_True;
  if (type == STANDARD_TYPE(IFSelect_SelectUnknownEntities)) return Standard_True;
  if (type == STANDARD_TYPE(IFSelect_SelectErrorEntities))   return Standard_True;
  if (type == STANDARD_TYPE(IFSelect_SelectIncorrectEntities)) return Standard_True;
  if (type == STANDARD_TYPE(IFSelect_SelectRoots))           return Standard_True;
  if (type == STANDARD_TYPE(IFSelect_SelectRootComps))       return Standard_True;
  if (type == STANDARD_TYPE(IFSelect_SelectRange)) {
    DeclareAndCast(IFSelect_SelectRange,sra,item);
    file.SendItem(sra->Lower());
    file.SendItem(sra->Upper());
    return Standard_True;
  }
  if (type == STANDARD_TYPE(IFSelect_SelectShared))          return Standard_True;
  if (type == STANDARD_TYPE(IFSelect_SelectSharing))         return Standard_True;

  if (type == STANDARD_TYPE(IFSelect_DispPerOne))            return Standard_True;
  if (type == STANDARD_TYPE(IFSelect_DispGlobal))            return Standard_True;
  if (type == STANDARD_TYPE(IFSelect_DispPerCount)) {
    DeclareAndCast(IFSelect_DispPerCount,dpc,item);
    file.SendItem(dpc->Count());
    return Standard_True;
  }

  if (type == STANDARD_TYPE(IFSelect_TransformStandard)) {
    DeclareAndCast(IFSelect_TransformStandard,trs,item);
    if (trs->CopyOption()) file.SendText("copy");
    else                   file.SendText("onthespot");
    Standard_Integer nbm = trs->NbModifiers();
    for (Standard_Integer i = 1; i <= nbm; i ++)
      file.SendItem(trs->Modifier(i));
  }

  return Standard_False;
}

    Standard_Boolean  IFSelect_BasicDumper::ReadOwn
  (IFSelect_SessionFile& file, const TCollection_AsciiString& type,
   Handle(Standard_Transient)& item) const
{
  if (type.IsEqual("IFSelect_SelectModelRoots"))
    {  item = new IFSelect_SelectModelRoots ();     return Standard_True;  }
  if (type.IsEqual("IFSelect_SelectModelEntities"))
    {  item = new IFSelect_SelectModelEntities ();  return Standard_True;  }
  if (type.IsEqual("IFSelect_SelectEntityNumber")) {
    Handle(IFSelect_SelectEntityNumber) sen =
      new IFSelect_SelectEntityNumber ();
    sen->SetNumber (GetCasted(IFSelect_IntParam,file.ItemValue(1)));
    item = sen;
    return Standard_True;
  }
  if (type.IsEqual("IFSelect_SelectPointed"))
    {  item = new IFSelect_SelectPointed;  return Standard_True;  }
  if (type.IsEqual("IFSelect_SelectUnion"))
    {  item = new IFSelect_SelectUnion;  return Standard_True;  }
  if (type.IsEqual("IFSelect_SelectIntersection"))
    {  item = new IFSelect_SelectIntersection;  return Standard_True;  }
  if (type.IsEqual("IFSelect_SelectDiff"))
    {  item = new IFSelect_SelectDiff;  return Standard_True;  }
  if (type.IsEqual("IFSelect_SelectUnknownEntities"))
    {  item = new IFSelect_SelectUnknownEntities;  return Standard_True;  }
  if (type.IsEqual("IFSelect_SelectErrorEntities"))
    {  item = new IFSelect_SelectErrorEntities;  return Standard_True;  }
  if (type.IsEqual("IFSelect_SelectIncorrectEntities"))
    {  item = new IFSelect_SelectIncorrectEntities;  return Standard_True;  }
  if (type.IsEqual("IFSelect_SelectRoots"))
    {  item = new IFSelect_SelectRoots;  return Standard_True;  }
  if (type.IsEqual("IFSelect_SelectRootComps"))
    {  item = new IFSelect_SelectRootComps;  return Standard_True;  }
  if (type.IsEqual("IFSelect_SelectRange")) {
    Handle(IFSelect_SelectRange) sra = new IFSelect_SelectRange;
    sra->SetRange (GetCasted(IFSelect_IntParam,file.ItemValue(1)),
		   GetCasted(IFSelect_IntParam,file.ItemValue(2)) );
    item = sra;
    return Standard_True;
  }
  if (type.IsEqual("IFSelect_SelectTextType")) {
    const TCollection_AsciiString exname = file.ParamValue(1);
    if (exname.Length() < FIRSTCHAR) return Standard_False;
    if      (exname.Value(FIRSTCHAR) == 'e') {}
    else if (exname.Value(FIRSTCHAR) == 'c') {}
    else  return Standard_False;
//    item = new IFSelect_SelectTextType (file.TextValue(2).ToCString(),exact);
//    return Standard_True;
  }
  if (type.IsEqual("IFSelect_SelectShared"))
    {  item = new IFSelect_SelectShared;  return Standard_True;  }
  if (type.IsEqual("IFSelect_SelectSharing"))
    {  item = new IFSelect_SelectSharing;  return Standard_True;  }

  if (type.IsEqual("IFSelect_DispPerOne"))
    {  item = new IFSelect_DispPerOne;  return Standard_True;  }
  if (type.IsEqual("IFSelect_DispGlobal"))
    {  item = new IFSelect_DispGlobal;  return Standard_True;  }
  if (type.IsEqual("IFSelect_DispPerCount")) {
    Handle(IFSelect_DispPerCount) dpc = new IFSelect_DispPerCount;
    dpc->SetCount (GetCasted(IFSelect_IntParam,file.ItemValue(1)) );
    item = dpc;
    return Standard_True;
  }

  if (type.IsEqual("IFSelect_TransformStandard")) {
    Standard_Boolean copyoption;
    const TCollection_AsciiString copyname = file.ParamValue(1);
    if (copyname.Length() < FIRSTCHAR) return Standard_False;
    if      (copyname.Value(FIRSTCHAR) == 'c') copyoption = Standard_True;
    else if (copyname.Value(FIRSTCHAR) == 'o') copyoption = Standard_False;
    else  return Standard_False;
    Handle(IFSelect_TransformStandard) trs = new IFSelect_TransformStandard;
    trs->SetCopyOption(copyoption);
    Standard_Integer nbp = file.NbParams();
    for (Standard_Integer i = 2; i <= nbp; i ++) {
      DeclareAndCast(IFSelect_Modifier,modif,file.ItemValue(i));
      if (!modif.IsNull()) trs->AddModifier(modif);
    }
    item = trs;
    return Standard_True;
  }

  return Standard_False;
}
