// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

#include <IGESData_IGESDumper.ixx>
#include <Interface_InterfaceError.hxx>
#include <Interface_Macros.hxx>
#include <Interface_MSG.hxx>
#include <Interface_EntityIterator.hxx>
#include <IGESData_SpecificLib.hxx>
#include <IGESData_SpecificModule.hxx>
#include <TCollection_HAsciiString.hxx>
#include <Message_Messenger.hxx>

IGESData_IGESDumper::IGESData_IGESDumper
  (const Handle(IGESData_IGESModel)& model,
   const Handle(IGESData_Protocol)& protocol)
      : thelib (protocol)
      {  themodel = model;  }


    void  IGESData_IGESDumper::PrintDNum
  (const Handle(IGESData_IGESEntity)& ent, const Handle(Message_Messenger)& S) const 
{
//   Affichage garanti sur  12 caracteres 12345/D24689
  Standard_Integer num = 0;
  if (!ent.IsNull()) {
    if (themodel.IsNull())                          S<<"    D???    ";
    else if ( (num = themodel->Number(ent)) == 0)   S<<"    0:D?????";
// Interface_InterfaceError::Raise  ("IGESDumper : PrintDNum");
    else {
      S<<Interface_MSG::Blanks(num,9)<<num<<":D"<<2*num-1<<Interface_MSG::Blanks(2*num-1,9);
    }
  }
  else                                              S<<"  D0(Null)  ";
}

    void  IGESData_IGESDumper::PrintShort
  (const Handle(IGESData_IGESEntity)& ent, const Handle(Message_Messenger)& S) const 
{
//  PrintDNum(ent,S);
  if (!ent.IsNull()) {
    Standard_Integer num = 0;
    if (!themodel.IsNull()) num = themodel->Number(ent);
    if (num > 0) S<<num<<":D"<<2*num-1;
    S << "  Type:" << ent->TypeNumber() << "  Form:" << ent->FormNumber()
      << Interface_MSG::Blanks (ent->FormNumber(),3)
      << " Class:"<< Interface_InterfaceModel::ClassName(ent->DynamicType()->Name());
  }
}


    void  IGESData_IGESDumper::Dump
  (const Handle(IGESData_IGESEntity)& ent, const Handle(Message_Messenger)& S,
   const Standard_Integer own, const Standard_Integer attached) const 
{
  Standard_Integer att = attached;
  Standard_Integer diratt = 1;
  if (own < 3) diratt = own - 1;
  if (att == 0)  att = diratt;  // -1 signifie : ne rien sortir

  if (own < 0) return;
  if (own > 1) S<<"\n";
  if (ent.IsNull())  {  S<<"(Null)";  if (own > 1) S<<"\n";  return;  }
  if (own == 0)  {  PrintDNum  (ent,S);  return;  }    // affichage auxiliaire
  if (own == 1)  {  PrintShort (ent,S);  return;  }    // affichage auxiliaire

  if (own > 0) S<<"****    Dump IGES, level "<<own<<" ( ";
  switch (own) {
  case 0 :    break;
  case 1 :    S<<"D.E. Number + Type + Name";  break;
  case 2 :    S<<"D.E. Number + Type, Name + Transf, View";  break;
  case 3 :    S<<"Complete Directory Part";  break;
  case 4 :    S<<"Directory Part + Parameters, no Arrays";  break;
  case 5 :    S<<"Complete";  break;
  default :   S<<"Complete + Transformed Values";  break;
  }
  S<<" )    ****"<<endl;

// ****    Entity 1234:D2467  **  Type:102 Form:56  **  CompositeCurve  **
  S<<"\n"<<"****    Entity ";  PrintShort (ent,S);  S<<endl;

  S<<"              Directory Part"<<"\n";
// **      Status :   Blank:1    Subordinate:2    UseFlag:3    Hierarchy:4
  if (own >= 2)
    S <<"**      Status Number :   Blank:"<<ent->BlankStatus()
      <<"    Subordinate:"<<ent->SubordinateStatus()
      <<"    UseFlag:"<<ent->UseFlag()
      <<"    Hierarchy:"<<ent->HierarchyStatus()<<"\n";
  if (own >= 1) {
    if (ent->HasShortLabel()) S<<"****    Label         :"<<ent->ShortLabel()->ToCString();
    if (ent->HasSubScriptNumber()) S<<"    SubScript:"<<ent->SubScriptNumber();
    if (ent->HasShortLabel())  S<<endl;

    if (ent->HasTransf())
      {  S<<"**      Transf.Matrix :";  PrintDNum(ent->Transf(),S); S<<"\n"; }
    IGESData_DefList  viewkind = ent->DefView();
    if (viewkind == IGESData_DefOne)
      {  S<<"**      View          :";  PrintDNum (ent->View(),S);  S<<"\n"; }
    if (viewkind == IGESData_DefSeveral)
      {  S<<"**      View (List)   :";  PrintDNum (ent->ViewList(),S);  S<<"\n"; }
     
  }

  if (own >= 2) {

    if (ent->HasStructure())
      {  S<<"**      Structure     :"; PrintDNum (ent->Structure(),S);  S<<"\n";  }

    S<<"\n"<<"              Graphic Attributes"<<"\n";
    if (ent->DefLineFont() == IGESData_DefValue)
      {  S<<"**      LineFont Value:"<<ent->RankLineFont()<<"\n"; }
    else if (ent->DefLineFont() == IGESData_DefReference)
      {  S<<"**      LineFont Ref  :";  PrintDNum (ent->LineFont(),S);  S<<"\n"; }

    if (ent->Level() > 0)  S<<"**      Level Value   :"<<ent->Level()<<"\n";
    else if (ent->Level() < 0)
      {  S<<"**      Level List    :";  PrintDNum(ent->LevelList(),S);  S<<"\n"; }

    if (ent->HasLabelDisplay())
      {  S<<"**      Label Display :";  PrintDNum (ent->LabelDisplay(),S);  S<<"\n"; }
    if (ent->LineWeightNumber() != 0) {
      S   <<"**      LineWeight Num:"<<ent->LineWeightNumber();
      if (diratt > 0) S<<"    ->  Value:"<<ent->LineWeight();
      S<<endl;
    }
    if (ent->DefColor() == IGESData_DefValue)
      {  S<<"**      Color Value   :"<<ent->RankColor();  }
    else if (ent->DefColor() == IGESData_DefReference)
      {  S<<"**      Color Ref     :";  PrintDNum (ent->Color(),S); }
    S<<endl;

    if (own > 3) {
      S<<"****             Own Data             ****"<<"\n\n";
      OwnDump(ent,S,own);
    }
  }

//   Donnees attachees : Properties, Associativities, et Sharings
  if (att < 0) return;
  Interface_EntityIterator iter = ent->Properties();
  Standard_Integer nb = iter.NbEntities();
  Standard_Boolean iasuit = (nb > 0);
  if (nb > 0) {
    S<<"\n"<<"****     Properties (nb:"<<nb<<")          ****"<<"\n";
    for (;iter.More(); iter.Next()) {
      DeclareAndCast(IGESData_IGESEntity,ent2,iter.Value());
      Dump (ent2,S,att,-1);
    }
  }
  iter = ent->Associativities();
  nb = iter.NbEntities();
  if (nb > 0) iasuit = Standard_True;
  if (nb > 0) {
    S<<"\n"<<"****   Associativities (nb:"<<nb<<")        ****"<<"\n";
    for (;iter.More(); iter.Next()) {
      DeclareAndCast(IGESData_IGESEntity,ent2,iter.Value());
      Dump(ent2,S,att,-1);
    }
  }
  if (iasuit) {  if (att <= 1) S << "\n";  }
  S<<"\n"<<"****             End of Dump          ****"<<"\n"<<endl;
}



    void  IGESData_IGESDumper::OwnDump
  (const Handle(IGESData_IGESEntity)& ent, const Handle(Message_Messenger)& S,
   const Standard_Integer own) const 
{
  Handle(IGESData_SpecificModule) module;  Standard_Integer CN;
  if (thelib.Select(ent,module,CN))
    module->OwnDump(CN,ent,*this,S,own);
  else if (themodel.IsNull())
    S <<"  ****  Dump impossible. Type "<<ent->DynamicType()->Name()<<endl;
  else
    S <<"  ****  Dump Impossible, n0:id:"<<themodel->Number(ent)<<":D"
      <<themodel->DNum(ent)<<" Type "<<ent->DynamicType()->Name()<<endl;
}
