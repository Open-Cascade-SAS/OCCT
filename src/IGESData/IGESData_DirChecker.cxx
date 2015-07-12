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


#include <IGESData_ColorEntity.hxx>
#include <IGESData_DefList.hxx>
#include <IGESData_DirChecker.hxx>
#include <IGESData_IGESEntity.hxx>
#include <IGESData_LabelDisplayEntity.hxx>
#include <IGESData_LevelListEntity.hxx>
#include <IGESData_LineFontEntity.hxx>
#include <IGESData_ViewKindEntity.hxx>
#include <Interface_Check.hxx>
#include <Message_Msg.hxx>

#include <stdio.h>
//  Pour Correct :
// MGE 23/07/98
// Chaque critere est par defaut inhibe
//=======================================================================
//function : IGESData_DirChecker
//purpose  : 
//=======================================================================
IGESData_DirChecker::IGESData_DirChecker ()
{
  thetype = theform1 = theform2 = 0;
  thestructure = thelinefont = thelineweig = thecolor = IGESData_ErrorRef;
  thegraphier = -100;  // ne pas tester GraphicsIgnored
  theblankst = thesubordst = theuseflag = thehierst = -100;  // ne pas tester
}


//=======================================================================
//function : IGESData_DirChecker
//purpose  : 
//=======================================================================

IGESData_DirChecker::IGESData_DirChecker (const Standard_Integer atype)
{
  thetype = atype; theform1 = 0; theform2 = -1;  // test de forme inhibe
  thestructure = thelinefont = thelineweig = thecolor = IGESData_ErrorRef;
  thegraphier = -100;  // ne pas tester GraphicsIgnored
  theblankst = thesubordst = theuseflag = thehierst = -100;  // ne pas tester
}


//=======================================================================
//function : IGESData_DirChecker
//purpose  : 
//=======================================================================

IGESData_DirChecker::IGESData_DirChecker(const Standard_Integer atype,
                                         const Standard_Integer aform)
{
  thetype = atype; theform1 = theform2 = aform;  // forme : valeur requise
  thestructure = thelinefont = thelineweig = thecolor = IGESData_ErrorRef;
  thegraphier = -100;  // ne pas tester GraphicsIgnored
  theblankst = thesubordst = theuseflag = thehierst = -100;  // ne pas tester
}


//=======================================================================
//function : IGESData_DirChecker
//purpose  : 
//=======================================================================

IGESData_DirChecker::IGESData_DirChecker(const Standard_Integer atype,
                                         const Standard_Integer aform1,
                                         const Standard_Integer aform2)
{
  thetype = atype; theform1 = aform1; theform2 = aform2;  // forme : [...]
  thestructure = thelinefont = thelineweig = thecolor = IGESData_ErrorRef;
  thegraphier = -100;  // ne pas tester GraphicsIgnored
  theblankst = thesubordst = theuseflag = thehierst = -100;  // ne pas tester
}


//=======================================================================
//function : IsSet
//purpose  : 
//=======================================================================

Standard_Boolean IGESData_DirChecker::IsSet () const 
{
  return isitset;
}


//=======================================================================
//function : SetDefault
//purpose  : 
//=======================================================================

void IGESData_DirChecker::SetDefault ()
{
  Structure(IGESData_DefVoid);
}    // Option par defaut


//=======================================================================
//function : Structure
//purpose  : 
//=======================================================================

void IGESData_DirChecker::Structure (const IGESData_DefType crit)
{
  isitset = Standard_True;
  thestructure = crit;
}


//=======================================================================
//function : LineFont
//purpose  : 
//=======================================================================

void IGESData_DirChecker::LineFont (const IGESData_DefType crit)
{
  isitset = Standard_True;
  thelinefont = crit;
}


//=======================================================================
//function : LineWeight
//purpose  : 
//=======================================================================

void IGESData_DirChecker::LineWeight (const IGESData_DefType crit)
{
  isitset = Standard_True;
  thelineweig = crit;
}


//=======================================================================
//function : Color
//purpose  : 
//=======================================================================

void IGESData_DirChecker::Color (const IGESData_DefType crit)
{
  isitset = Standard_True;
  thecolor = crit;
}


//=======================================================================
//function : GraphicsIgnored
//purpose  : 
//=======================================================================

void IGESData_DirChecker::GraphicsIgnored (const Standard_Integer hierarchy)
{
  isitset = Standard_True;
  thegraphier = hierarchy;
}


//=======================================================================
//function : BlankStatusIgnored
//purpose  : 
//=======================================================================

void IGESData_DirChecker::BlankStatusIgnored ()
{
  isitset = Standard_True;
  theblankst = -10;
}


//=======================================================================
//function : BlankStatusRequired
//purpose  : 
//=======================================================================

void IGESData_DirChecker::BlankStatusRequired (const Standard_Integer val)
{
  isitset = Standard_True;
  theblankst = val;
}


//=======================================================================
//function : SubordinateStatusIgnored
//purpose  : 
//=======================================================================

void IGESData_DirChecker::SubordinateStatusIgnored ()
{
  isitset = Standard_True;
  thesubordst = -10;
}


//=======================================================================
//function : SubordinateStatusRequired
//purpose  : 
//=======================================================================

void IGESData_DirChecker::SubordinateStatusRequired(const Standard_Integer val)
{
  isitset = Standard_True;
  thesubordst = val;
}


//=======================================================================
//function : UseFlagIgnored
//purpose  : 
//=======================================================================

void IGESData_DirChecker::UseFlagIgnored ()
{
  isitset = Standard_True;
  theuseflag = -10;
}


//=======================================================================
//function : UseFlagRequired
//purpose  : 
//=======================================================================

void IGESData_DirChecker::UseFlagRequired (const Standard_Integer val)
{
  isitset = Standard_True;
  theuseflag = val;
}


//=======================================================================
//function : HierarchyStatusIgnored
//purpose  : 
//=======================================================================

void IGESData_DirChecker::HierarchyStatusIgnored ()
{
  isitset = Standard_True;
  thehierst = -10;
}


//=======================================================================
//function : HierarchyStatusRequired
//purpose  : 
//=======================================================================

void IGESData_DirChecker::HierarchyStatusRequired (const Standard_Integer val)
{
  isitset = Standard_True;
  thehierst = val;
}


//=======================================================================
//function : Check
//purpose  : 
//=======================================================================

void IGESData_DirChecker::Check(Handle(Interface_Check)& ach,
                                const Handle(IGESData_IGESEntity)& ent) const 
{
  // MGE 23/07/98
  // =====================================
  //Message_Msg Msg58 ("XSTEP_58");
  //Message_Msg Msg59 ("XSTEP_59");
  //Message_Msg Msg60 ("XSTEP_60");
  //Message_Msg Msg65 ("XSTEP_65");
  //Message_Msg Msg66 ("XSTEP_66");
  //Message_Msg Msg67 ("XSTEP_67");
  //Message_Msg Msg68 ("XSTEP_68");
  //Message_Msg Msg69 ("XSTEP_69");
  //Message_Msg Msg70 ("XSTEP_70");
  //Message_Msg Msg71 ("XSTEP_71");
  // =====================================

  if (thetype != 0)
  {
    // Sending of message : Entity Type Number field is incorrect.
    if (ent->TypeNumber() != thetype) {
      Message_Msg Msg58 ("XSTEP_58");
      ach->SendFail(Msg58);
    }

    // Sending of message : Form Number field is incorrect.
    if (theform1 <= theform2) {
      if (ent->FormNumber() < theform1 || ent->FormNumber() > theform2) {
        Message_Msg Msg71 ("XSTEP_71");
	ach->SendFail(Msg71);
      }
    }
  }
 
  // Sending of message : Structure field is undefined.
  if (thestructure == IGESData_DefReference && !ent->HasStructure()) {
    Message_Msg Msg59 ("XSTEP_59");
    ach->SendFail (Msg59);
  }

  if (thegraphier == -1 || thegraphier == ent->HierarchyStatus()) { }
  else {
    IGESData_DefType df = ent->DefLineFont();
   
    // Sending of message : Line Font Pattern field is incorrect
    if (df == IGESData_ErrorVal || df == IGESData_ErrorRef) {
      Message_Msg Msg60 ("XSTEP_60");
      ach->SendFail (Msg60);
    }
    else if (thelinefont == IGESData_DefValue && df != IGESData_DefValue) {
      Message_Msg Msg60 ("XSTEP_60");
      ach->SendWarning (Msg60);
    }

    Standard_Integer dlw = ent->LineWeightNumber();
    Message_Msg Msg69 ("XSTEP_69");
    // Sending of message : Line Weight Number is undefined.
    if (thelineweig == IGESData_DefValue && dlw == 0) {
//      Message_Msg Msg69 ("XSTEP_69");
      ach->SendWarning (Msg69);
    }

    df = ent->DefColor();

    // Sending of message : Color Number field is incorrect.
    if (df == IGESData_ErrorVal || df == IGESData_ErrorRef) {
//      Message_Msg Msg69 ("XSTEP_69");
      ach->SendFail (Msg69);
    }
    else if (thecolor == IGESData_DefValue && df != IGESData_DefValue) {
//      Message_Msg Msg69 ("XSTEP_69");
      ach->SendWarning (Msg69);
    }
  }

  Standard_Integer st = ent->BlankStatus();

  // Sending of message : Blank Status field is incorrect.
  if (st < 0 || st > 1) {
    Message_Msg Msg65 ("XSTEP_65");
    ach->SendFail (Msg65);
  }

  st = ent->SubordinateStatus();
  
  // Sending of message : Subordinate Entity Switch field is incorrect.
  if (st < 0 || st > 3) {
    Message_Msg Msg66 ("XSTEP_66");
    ach->SendFail(Msg66);
  }

  st = ent->UseFlag();

  // Send of message : Entity Use Flag is incorrect.
  if (st < 0 || st > 5) {
    Message_Msg Msg67 ("XSTEP_67");
    ach->SendFail(Msg67);
  }

  st = ent->HierarchyStatus();

  //Sending of message : Hierarchy field is incorrect.
  if (st < 0 || st > 2) {
    Message_Msg Msg68 ("XSTEP_68");
    ach->SendFail(Msg68);
  }

}


//=======================================================================
//function : CheckTypeAndForm
//purpose  : 
//=======================================================================

void IGESData_DirChecker::CheckTypeAndForm(Handle(Interface_Check)& ach,
                                           const Handle(IGESData_IGESEntity)& ent) const 
{
// CKY 30 NOV 2001 : This method is called for immediate check on reading
//   But an entity which can be read has ben already recognized.
//   To produce a FAIL here is interpreted as "FAIL ON LOADING", which is
//   not true (the entity has been recognized then properly loaded)
//  Consequence (among other) : the entity is not explored by graph of dep.s
//   so numerous "false roots" are detected
//  Alternative to switch Fail to Warning here, should be to withdraw the calls
//   to that method by all the "ReadOwn" methods, but it's heavier
// ANYWAY, the full Check method produces a Fail if Type/Form is not in scope,
//   so it is well interpreted as "Syntactic error"

  // MGE 23/07/98
  // =====================================
//  Message_Msg Msg58 ("XSTEP_58");   
//  Message_Msg Msg71 ("XSTEP_71");
  // =====================================
  //char mess[80]; //szv#4:S4163:12Mar99 unused
  if (thetype != 0) 
  {
    if (ent->TypeNumber() != thetype){
      Message_Msg Msg58 ("XSTEP_58");   
      ach->SendWarning(Msg58);
    }

    if (theform1 <= theform2) 
      if (ent->FormNumber() < theform1 || ent->FormNumber() > theform2) {
	Message_Msg Msg71 ("XSTEP_71");
	ach->SendWarning(Msg71);

      }
  }
}


//=======================================================================
//function : Correct
//purpose  : 
//=======================================================================

Standard_Boolean  IGESData_DirChecker::Correct
  (const Handle(IGESData_IGESEntity)& ent) const 
{
  Standard_Boolean done = Standard_False;
  Standard_Integer type = ent->TypeNumber();
  Standard_Integer form = ent->FormNumber();
  if (thetype != 0) {
    if (theform1 >= 0 && theform1 == theform2 && theform1 != form)
      {  ent->InitTypeAndForm (thetype,theform1);  done = Standard_True;  }
    else if (thetype != type)
      {  ent->InitTypeAndForm (thetype,form);      done = Standard_True;  }
  }

  Handle(IGESData_IGESEntity)      structure;    // par defaut Nulle
  if (thestructure != IGESData_DefVoid) structure = ent->Structure();
  Handle(IGESData_ViewKindEntity)  nulview;
  Handle(IGESData_LineFontEntity)  nulfont;
  Handle(IGESData_LevelListEntity) nulevel;
  Handle(IGESData_ColorEntity)     nulcolor;
  Handle(IGESData_LabelDisplayEntity) label;    // par defaut Nulle
  if (thegraphier != -1) label = ent->LabelDisplay();
  Standard_Integer linew = 0;
  if (thegraphier != -1 && thelineweig  != IGESData_DefVoid)
    linew = ent->LineWeightNumber();

  if (thegraphier == -1 ||
      (ent->RankLineFont() != 0 && thelinefont  == IGESData_DefVoid) )
    {  ent->InitLineFont(nulfont);    done = Standard_True;  }
  if (thegraphier == -1 ||
      (ent->RankColor() != 0    && thecolor     == IGESData_DefVoid) )
    {  ent->InitColor(nulcolor);      done = Standard_True;  }
  if (thegraphier == -1 &&
      (!ent->View().IsNull() || ent->Level() != 0) )
    {  ent->InitView(nulview);  ent->InitLevel(nulevel);  done = Standard_True;  }
  if ((thegraphier == -1 &&
       (!ent->LabelDisplay().IsNull() || ent->LineWeightNumber() != 0)) ||
      (ent->HasStructure() && thestructure == IGESData_DefVoid) ) // combines :
    {  ent->InitMisc (structure,label,linew);       done = Standard_True;  }

  Standard_Boolean force = Standard_False;
  Standard_Integer stb = ent->BlankStatus();
  Standard_Integer sts = ent->SubordinateStatus();
  Standard_Integer stu = ent->UseFlag();
  Standard_Integer sth = ent->HierarchyStatus();
  if (theblankst  >= 0 && theblankst != stb)
    {  force = Standard_True;  stb = theblankst;   }
  if (thesubordst >= 0 && thesubordst != sts)
    {  force = Standard_True;  sts = thesubordst;  }
  if (theuseflag  >= 0 && theuseflag != stu)
    {  force = Standard_True;  stu = theuseflag;   }
  if (thehierst   >= 0 && thehierst != sth)
    {  force = Standard_True;  sth = thehierst;  }
  if (force) {  ent->InitStatus(stb,sts,stu,sth);      done = Standard_True;  }
  return done;
}
