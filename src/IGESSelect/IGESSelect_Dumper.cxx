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


#include <IFSelect_IntParam.hxx>
#include <IFSelect_SessionFile.hxx>
#include <IGESSelect_AutoCorrect.hxx>
#include <IGESSelect_ChangeLevelList.hxx>
#include <IGESSelect_ChangeLevelNumber.hxx>
#include <IGESSelect_ComputeStatus.hxx>
#include <IGESSelect_DispPerDrawing.hxx>
#include <IGESSelect_DispPerSingleView.hxx>
#include <IGESSelect_Dumper.hxx>
#include <IGESSelect_FloatFormat.hxx>
#include <IGESSelect_RebuildDrawings.hxx>
#include <IGESSelect_RebuildGroups.hxx>
#include <IGESSelect_SelectBypassGroup.hxx>
#include <IGESSelect_SelectDrawingFrom.hxx>
#include <IGESSelect_SelectFromDrawing.hxx>
#include <IGESSelect_SelectFromSingleView.hxx>
#include <IGESSelect_SelectLevelNumber.hxx>
#include <IGESSelect_SelectName.hxx>
#include <IGESSelect_SelectSingleViewFrom.hxx>
#include <IGESSelect_SelectVisibleStatus.hxx>
#include <IGESSelect_SetGlobalParameter.hxx>
#include <IGESSelect_SetVersion5.hxx>
#include <IGESSelect_SplineToBSpline.hxx>
#include <IGESSelect_UpdateCreationDate.hxx>
#include <IGESSelect_UpdateLastChange.hxx>
#include <Interface_Macros.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(IGESSelect_Dumper,IFSelect_SessionDumper)

//#include <IGESSelect_SelectIGESTypeForm.hxx>
IGESSelect_Dumper::IGESSelect_Dumper ()    {  }


   Standard_Boolean  IGESSelect_Dumper::WriteOwn
  (IFSelect_SessionFile& file, const Handle(Standard_Transient)& item) const
{
  if (item.IsNull()) return Standard_False;
  Handle(Standard_Type) type = item->DynamicType();
  if (type == STANDARD_TYPE(IGESSelect_DispPerSingleView))    return Standard_True;
  if (type == STANDARD_TYPE(IGESSelect_DispPerDrawing))       return Standard_True;
/*  if (type == STANDARD_TYPE(IGESSelect_SelectIGESTypeForm)) {
    DeclareAndCast(IGESSelect_SelectIGESTypeForm,sf,item);
    Standard_Boolean exact = sf->IsExact();
    TCollection_AsciiString text = sf->SignatureText();  // attention, 1-2 term
    if (exact) file.SendText("exact");
    else       file.SendText("contains");
    file.SendText(text.ToCString());
    return Standard_True;
  } */
  if (type == STANDARD_TYPE(IGESSelect_SelectVisibleStatus))  return Standard_True;
  if (type == STANDARD_TYPE(IGESSelect_SelectLevelNumber)) {
    DeclareAndCast(IGESSelect_SelectLevelNumber,sl,item);
    Handle(IFSelect_IntParam) lev = sl->LevelNumber();
    file.SendItem(lev);
    return Standard_True;
  }
  if (type == STANDARD_TYPE(IGESSelect_SelectName)) {
    DeclareAndCast(IGESSelect_SelectName,sn,item);
    Handle(TCollection_HAsciiString) name = sn->Name();
    file.SendItem(name);
    return Standard_True;
  }
  if (type == STANDARD_TYPE(IGESSelect_SelectFromSingleView)) return Standard_True;
  if (type == STANDARD_TYPE(IGESSelect_SelectFromDrawing))    return Standard_True;
  if (type == STANDARD_TYPE(IGESSelect_SelectSingleViewFrom)) return Standard_True;
  if (type == STANDARD_TYPE(IGESSelect_SelectDrawingFrom))    return Standard_True;
  if (type == STANDARD_TYPE(IGESSelect_SelectBypassGroup))    return Standard_True;

  if (type == STANDARD_TYPE(IGESSelect_FloatFormat)) {
    DeclareAndCast(IGESSelect_FloatFormat,ff,item);
    Standard_Boolean zerosup,  hasrange;
    Standard_Real    rangemin, rangemax;
    TCollection_AsciiString mainform,forminrange;
    ff->Format (zerosup,mainform,hasrange,forminrange,rangemin,rangemax);
    file.SendText((char*)(zerosup ? "zerosup" : "nozerosup"));
    file.SendText(mainform.ToCString());
    if (hasrange) {
      char flotrange[20];
      file.SendText(forminrange.ToCString());
      Sprintf(flotrange,"%f",rangemin);
      file.SendText(flotrange);
      Sprintf(flotrange,"%f",rangemax);
      file.SendText(flotrange);
    }
    return Standard_True;
  }

  if (type == STANDARD_TYPE(IGESSelect_UpdateCreationDate))   return Standard_True;
  if (type == STANDARD_TYPE(IGESSelect_UpdateLastChange))     return Standard_True;
  if (type == STANDARD_TYPE(IGESSelect_SetVersion5))          return Standard_True;
  if (type == STANDARD_TYPE(IGESSelect_SetGlobalParameter)) {
    DeclareAndCast(IGESSelect_SetGlobalParameter,sp,item);
    Standard_Integer np = sp->GlobalNumber();
    Handle(TCollection_HAsciiString) val = sp->Value();
    char intext[10];
    sprintf(intext,"%d",np);
    file.SendText(intext);
    file.SendItem(val);
    return Standard_True;
  }
  if (type == STANDARD_TYPE(IGESSelect_AutoCorrect))          return Standard_True;
  if (type == STANDARD_TYPE(IGESSelect_ComputeStatus))        return Standard_True;
  if (type == STANDARD_TYPE(IGESSelect_RebuildDrawings))      return Standard_True;
  if (type == STANDARD_TYPE(IGESSelect_RebuildGroups))        return Standard_True;
  if (type == STANDARD_TYPE(IGESSelect_ChangeLevelList)) {
    DeclareAndCast(IGESSelect_ChangeLevelList,cl,item);
    file.SendItem(cl->OldNumber());
    file.SendItem(cl->NewNumber());
    return Standard_True;
  }
  if (type == STANDARD_TYPE(IGESSelect_ChangeLevelNumber)) {
    DeclareAndCast(IGESSelect_ChangeLevelNumber,cl,item);
    file.SendItem(cl->OldNumber());
    file.SendItem(cl->NewNumber());
    return Standard_True;
  }

  if (type == STANDARD_TYPE(IGESSelect_SplineToBSpline)) {
    Standard_Boolean tryc2 =
      GetCasted(IGESSelect_SplineToBSpline,item)->OptionTryC2();
    file.SendText((char*)(tryc2 ? "TryC2" : "Normal"));
    return Standard_True;
  }
  return Standard_False;
}


    Standard_Boolean  IGESSelect_Dumper::ReadOwn
  (IFSelect_SessionFile& file, const TCollection_AsciiString& type,
   Handle(Standard_Transient)& item) const
{
  if (type.IsEqual("IGESSelect_DispPerSingleView"))
    {  item = new IGESSelect_DispPerSingleView;      return Standard_True;  }
  if (type.IsEqual("IGESSelect_DispPerDrawing"))
    {  item = new IGESSelect_DispPerDrawing;         return Standard_True;  }

  if (type.IsEqual("IGESSelect_SelectIGESTypeForm")) {
    if (file.NbParams() < 2) return Standard_False;
    //Standard_Boolean exact; //szv#4:S4163:12Mar99 not needed
    const TCollection_AsciiString exname = file.ParamValue(1);
    if (exname.Length() < 1) return Standard_False;
    if      (exname.Value(1) == 'e') {} //szv#4:S4163:12Mar99 `exact = Standard_True` not needed
    else if (exname.Value(1) == 'c') {} //szv#4:S4163:12Mar99 `exact = Standard_False` not needed
    else  return Standard_False;
    // Attention, 2 termes possibles pour la signature
    char sig[40];
    if (file.NbParams() == 2) sprintf(sig,"%s",file.ParamValue(2).ToCString());
    else sprintf(sig,"%s %s",file.ParamValue(2).ToCString(),file.ParamValue(3).ToCString());
//    item = new IGESSelect_SelectIGESTypeForm(sig,exact);
//    return Standard_True;
  }
  if (type.IsEqual("IGESSelect_SelectVisibleStatus"))
    {  item = new IGESSelect_SelectVisibleStatus;    return Standard_True;  }

  if (type.IsEqual("IGESSelect_SelectLevelNumber")) {
    if (file.NbParams() < 1) return Standard_False;
    Handle(IGESSelect_SelectLevelNumber) sl = new IGESSelect_SelectLevelNumber;
    DeclareAndCast(IFSelect_IntParam,lev,file.ItemValue(1));
    sl->SetLevelNumber(lev);
    item = sl;
    return Standard_True;
  }
  if (type.IsEqual("IGESSelect_SelectName")) {
    if (file.NbParams() < 1) return Standard_False;
    Handle(IGESSelect_SelectName) sn = new IGESSelect_SelectName;
    Handle(TCollection_HAsciiString) name = sn->Name();
    item = sn;
    return Standard_True;
  }
  if (type.IsEqual("IGESSelect_SelectFromSingleView"))
    {  item = new IGESSelect_SelectFromSingleView;   return Standard_True;  }
  if (type.IsEqual("IGESSelect_SelectFromDrawing"))
    {  item = new IGESSelect_SelectFromDrawing;      return Standard_True;  }
  if (type.IsEqual("IGESSelect_SelectSingleViewFrom"))
    {  item = new IGESSelect_SelectSingleViewFrom;   return Standard_True;  }
  if (type.IsEqual("IGESSelect_SelectDrawingFrom"))
    {  item = new IGESSelect_SelectDrawingFrom;      return Standard_True;  }
  if (type.IsEqual("IGESSelect_SelectBypassGroup"))
    {  item = new IGESSelect_SelectBypassGroup;      return Standard_True;  }

  if (type.IsEqual("IGESSelect_FloatFormat")) {
    if (file.NbParams() < 2) return Standard_False;
    Handle(IGESSelect_FloatFormat) ff = new IGESSelect_FloatFormat;
    Standard_Boolean zerosup;
    const TCollection_AsciiString zsup = file.ParamValue(1);
    if (zsup.Length() < 1) return Standard_False;
    if      (zsup.Value(1) == 'z') zerosup = Standard_True;
    else if (zsup.Value(1) == 'n') zerosup = Standard_False;
    else  return Standard_False;
    ff->SetFormat(file.ParamValue(2).ToCString());
    ff->SetZeroSuppress(zerosup);
    if (file.NbParams() >= 5) {
      //char flotrange[20]; //szv#4:S4163:12Mar99 unused
      Standard_Real    rangemin, rangemax;
      rangemin = Atof (file.ParamValue(4).ToCString());
      rangemax = Atof (file.ParamValue(5).ToCString());
      ff->SetFormatForRange (file.ParamValue(3).ToCString(),rangemin,rangemax);
    }
    item = ff;
    return Standard_True;
  }

  if (type.IsEqual("IGESSelect_UpdateCreationDate"))
    {  item = new IGESSelect_UpdateCreationDate;     return Standard_True;  }
  if (type.IsEqual("IGESSelect_UpdateLastChange"))
    {  item = new IGESSelect_UpdateLastChange;       return Standard_True;  }
  if (type.IsEqual("IGESSelect_SetVersion5"))
    {  item = new IGESSelect_SetVersion5;            return Standard_True;  }
  if (type.IsEqual("IGESSelect_SetGlobalParameter")) {
    if (file.NbParams() < 2) return Standard_False;
    Standard_Integer np = atoi(file.ParamValue(1).ToCString());
    DeclareAndCast(TCollection_HAsciiString,val,file.ItemValue(2));
    Handle(IGESSelect_SetGlobalParameter) sp =
      new IGESSelect_SetGlobalParameter(np);
    sp->SetValue(val);
    item = sp;
    return Standard_True;
  }

  if (type.IsEqual("IGESSelect_AutoCorrect"))
    {  item = new IGESSelect_AutoCorrect;            return Standard_True;  }
  if (type.IsEqual("IGESSelect_ComputeStatus"))
    {  item = new IGESSelect_ComputeStatus;          return Standard_True;  }
  if (type.IsEqual("IGESSelect_RebuildDrawings"))
    {  item = new IGESSelect_RebuildDrawings;        return Standard_True;  }
  if (type.IsEqual("IGESSelect_RebuildGroups"))
    {  item = new IGESSelect_RebuildGroups;          return Standard_True;  }

  if (type.IsEqual("IGESSelect_ChangeLevelList")) {
    if (file.NbParams() < 2) return Standard_False;
    Handle(IGESSelect_ChangeLevelList) cl = new IGESSelect_ChangeLevelList;
    DeclareAndCast(IFSelect_IntParam,oldpar,file.ItemValue(1));
    DeclareAndCast(IFSelect_IntParam,newpar,file.ItemValue(2));
    cl->SetOldNumber(oldpar);
    cl->SetNewNumber(newpar);
    item = cl;
    return Standard_True;
  }
  if (type.IsEqual("IGESSelect_ChangeLevelNumber")) {
    if (file.NbParams() < 2) return Standard_False;
    Handle(IGESSelect_ChangeLevelNumber) cl = new IGESSelect_ChangeLevelNumber;
    DeclareAndCast(IFSelect_IntParam,oldpar,file.ItemValue(1));
    DeclareAndCast(IFSelect_IntParam,newpar,file.ItemValue(2));
    cl->SetOldNumber(oldpar);
    cl->SetNewNumber(newpar);
    item = cl;
    return Standard_True;
  }

  if (type.IsEqual("IGESSelect_SplineToBSpline")) {
    if (file.NbParams() < 1) return Standard_False;
    Standard_Boolean tryc2;
    const TCollection_AsciiString tc2 = file.ParamValue(1);
    if (tc2.Length() < 1) return Standard_False;
    if      (tc2.Value(1) == 'T') tryc2 = Standard_True;
    else if (tc2.Value(1) == 'N') tryc2 = Standard_False;
    else  return Standard_False;
    item = new IGESSelect_SplineToBSpline(tryc2);
    return Standard_True;
  }
  return Standard_False;
}
