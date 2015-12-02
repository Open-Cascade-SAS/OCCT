// Created on: 1997-07-30
// Created by: Denis PASCAL
// Copyright (c) 1997-1999 Matra Datavision
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

#include <DDataStd.hxx>

#include <DDF.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Appli.hxx>

#include <TDF_Data.hxx>
#include <TDF_Label.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Tool.hxx>

#include <TNaming_NamedShape.hxx>

#include <TDataXtd.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataXtd_Constraint.hxx>
#include <TDataXtd_PatternStd.hxx>
#include <TDataXtd_Position.hxx>
#include <TCollection_AsciiString.hxx>

#include <Standard_Macro.hxx>

//=======================================================================
//function : DDataStd_SetConstraint
//purpose  : SetConstraint (DF,entry,keyword,geometrie/value[,geometrie])",
//=======================================================================
static Standard_Integer DDataStd_SetConstraint (Draw_Interpretor& di,
                                                Standard_Integer nb, 
                                                const char** arg) 
{   
  if (nb < 5)
  {
    di << "usage: SetConstraint DF entry keyword geometrie [geometrie ...]\n";
    di << "or SetConstraint DF entry \"plane\" geometrie - to set plane for existing constraint\n";
    di << "or SetConstraint DF entry \"value\" value - to set value for existing constraint\n";
    return 1;
  }

  Handle(TDF_Data) DF;
  if (!DDF::GetDF(arg[1],DF)) return 1;

  TDF_Label L;     
  if (!DDF::FindLabel(DF,arg[2],L)) return 1;  

  TDataXtd_ConstraintEnum aCT;
  const char* aT = arg[3];

  if (strcmp(aT,"plane") == 0)
  {
    Handle(TDataXtd_Constraint) C;
    if (!L.FindAttribute(TDataXtd_Constraint::GetID(), C)) return 1;

    TDF_Label aLab;     
    if (!DDF::FindLabel(DF, arg[4], aLab)) return 1;
    Handle(TNaming_NamedShape) aSh;
    if (aLab.FindAttribute(TNaming_NamedShape::GetID(), aSh))
    {
      C->SetPlane(aSh);
    }
  }
  else if (strcmp(aT,"value") == 0)
  {
    Handle(TDataXtd_Constraint) C;
    if (!L.FindAttribute(TDataXtd_Constraint::GetID(), C)) return 1;

    TDF_Label aLab;     
    if (!DDF::FindLabel(DF, arg[4], aLab)) return 1;
    Handle(TDataStd_Real) aR;
    if (aLab.FindAttribute(TDataStd_Real::GetID(), aR))
    {
      C->SetValue(aR);
    }
  }
  else
  {
    Handle(TDataXtd_Constraint) C = TDataXtd_Constraint::Set(L);

    // planar constraints
    if (strcmp(aT,"rad") == 0)             aCT = TDataXtd_RADIUS;
    else if (strcmp(aT,"dia") == 0)        aCT = TDataXtd_DIAMETER;
    else if (strcmp(aT,"minr") == 0)       aCT = TDataXtd_MINOR_RADIUS;
    else if (strcmp(aT,"majr") == 0)       aCT = TDataXtd_MAJOR_RADIUS;
    else if (strcmp(aT,"tan") == 0)        aCT = TDataXtd_TANGENT;
    else if (strcmp(aT,"par") == 0)        aCT = TDataXtd_PARALLEL;
    else if (strcmp(aT,"perp") == 0)       aCT = TDataXtd_PERPENDICULAR;
    else if (strcmp(aT,"concentric") == 0) aCT = TDataXtd_CONCENTRIC;
    else if (strcmp(aT,"equal") == 0)      aCT = TDataXtd_COINCIDENT;
    else if (strcmp(aT,"dist") == 0)       aCT = TDataXtd_DISTANCE;
    else if (strcmp(aT,"angle") == 0)      aCT = TDataXtd_ANGLE;
    else if (strcmp(aT,"eqrad") == 0)      aCT = TDataXtd_EQUAL_RADIUS;
    else if (strcmp(aT,"symm") == 0)       aCT = TDataXtd_SYMMETRY;
    else if (strcmp(aT,"midp") == 0)       aCT = TDataXtd_MIDPOINT;
    else if (strcmp(aT,"eqdist") == 0)     aCT = TDataXtd_EQUAL_DISTANCE;
    else if (strcmp(aT,"fix") == 0)        aCT = TDataXtd_FIX;
    else if (strcmp(aT,"rigid") == 0)      aCT = TDataXtd_RIGID;
    // placement constraints
    else if (strcmp(aT,"from") == 0)   aCT = TDataXtd_FROM;
    else if (strcmp(aT,"axis") == 0)   aCT = TDataXtd_AXIS;
    else if (strcmp(aT,"mate") == 0)   aCT = TDataXtd_MATE;
    else if (strcmp(aT,"alignf") == 0) aCT = TDataXtd_ALIGN_FACES;
    else if (strcmp(aT,"aligna") == 0) aCT = TDataXtd_ALIGN_AXES;
    else if (strcmp(aT,"axesa") == 0)  aCT = TDataXtd_AXES_ANGLE;
    else if (strcmp(aT,"facesa") == 0) aCT = TDataXtd_FACES_ANGLE;
    else if (strcmp(aT,"round") == 0)  aCT = TDataXtd_ROUND;
    else if (strcmp(aT,"offset") == 0) aCT = TDataXtd_OFFSET;
    else
    {
      di << "DDataStd_SetConstraint : unknown type, must be one of:\n";
      di << "rad/dia/minr/majr/tan/par/perp/concentric/equal/dist/angle/eqrad/symm/midp/\n";
      di << "eqdist/fix/rigid or from/axis/mate/alignf/aligna/axesa/facesa/round/offset\n";
      di << "or plane/value\n";
      return 1;
    }

    // set type
    C->SetType(aCT);

    // retrieve and set geometries
    Standard_Integer i = 1, nbSh = nb - 4;
    Handle(TNaming_NamedShape) aSh;
    TDF_Label aLab;     

    for (i = 1; i <= nbSh; i++)
    {
      if (!DDF::FindLabel(DF, arg[i+3], aLab)) return 1;  
      if (aLab.FindAttribute(TNaming_NamedShape::GetID(), aSh))
      {
        C->SetGeometry(i, aSh);
      }
    }
  }
  return 0;
}

//=======================================================================
//function : DDataStd_GetConstraint
//purpose  : GetConstraints (document, label)
//=======================================================================
static Standard_Integer DDataStd_GetConstraint (Draw_Interpretor& di,
                                                Standard_Integer nb, 
                                                const char** arg)
{
  Handle(TDataXtd_Constraint) CTR;
  if (nb == 3)
  {   
    Handle(TDF_Data) DF;
    TDF_Label L;     
    if (!DDF::GetDF(arg[1],DF)) return 1;
    if (!DDF::FindLabel(DF,arg[2],L)) return 1;  
    if (L.FindAttribute(TDataXtd_Constraint::GetID(),CTR))
    {
      Standard_SStream aStream;
      DDataStd::DumpConstraint (CTR,aStream);
      di << aStream;
    }
    else
    {
      TDF_ChildIterator it (L,Standard_True);
      for (;it.More();it.Next())
      {
        const TDF_Label& current = it.Value();
        if (current.FindAttribute(TDataXtd_Constraint::GetID(),CTR))
        {
          Standard_SStream aStream;
          DDataStd::DumpConstraint (CTR,aStream);
          di << aStream;
        }
      }
    }
    return 0;
  }
  di << "DDataStd_GetConstraint : Error : not done\n";
  return 1;
}

//=======================================================================
//function : DDataStd_SetPattern
//purpose  : SetPattern (DF,entry,signature,NSentry[realEntry,intEntry[,NSentry,realEntry,intEntry]])
//=======================================================================
static Standard_Integer DDataStd_SetPattern (Draw_Interpretor& di,
                                             Standard_Integer nb, 
                                             const char** arg) 
{   
  if (nb < 5)
  {
    di << "usage: SetPattern (DF,entry,signature,NSentry[realEntry,intEntry[,NSentry,realEntry,intEntry]])\n";
    return 1;
  }

  Handle(TDF_Data) DF;
  if (!DDF::GetDF(arg[1],DF)) return 1;

  TDF_Label L;     
  if (!DDF::FindLabel(DF,arg[2],L)) return 1;  

  Handle(TDataXtd_PatternStd) aP = TDataXtd_PatternStd::Set(L);

  // set signature
  Standard_Integer signature = Draw::Atoi(arg[3]);
  aP->Signature(signature);

  TDF_Label aLab;     
  Handle(TNaming_NamedShape) TNS;
  Handle(TDataStd_Real) TReal;
  Handle(TDataStd_Integer) TInt;

  // set other parameters
  if (signature < 5)
  {
    if (nb < 7)
    {
      di<<"usage:\n";
      di<<"SetPattern (DF,entry,signature<=2,NSentry,realEntry,intEntry)\n";
      di<<"SetPattern (DF,entry,2<signature<5,NSentry,realEntry,intEntry,NSentry,realEntry,intEntry)\n";
      return 1;
    }

    // NSentry
    if (!DDF::FindLabel(DF, arg[4], aLab)) return 1;
    if (aLab.FindAttribute(TNaming_NamedShape::GetID(), TNS))
    {
      aP->Axis1(TNS);
    }

    // realEntry
    if (!DDF::FindLabel(DF, arg[5], aLab)) return 1;
    if (aLab.FindAttribute(TDataStd_Real::GetID(), TReal))
    {
      aP->Value1(TReal);
    }

    // intEntry
    if (!DDF::FindLabel(DF, arg[6], aLab)) return 1;
    if (aLab.FindAttribute(TDataStd_Integer::GetID(), TInt))
    {
      aP->NbInstances1(TInt);
    }

    if (signature > 2)
    {
      if (nb < 10)
      {
        di<<"usage:\n";
        di<<"SetPattern (DF,entry,2<signature<5,NSentry,realEntry,intEntry,NSentry,realEntry,intEntry)\n";
        return 1;
      }

      // NSentry
      if (!DDF::FindLabel(DF, arg[7], aLab)) return 1;
      if (aLab.FindAttribute(TNaming_NamedShape::GetID(), TNS))
      {
        aP->Axis2(TNS);
      }

      // realEntry
      if (!DDF::FindLabel(DF, arg[8], aLab)) return 1;
      if (aLab.FindAttribute(TDataStd_Real::GetID(), TReal))
      {
        aP->Value2(TReal);
      }

      // intEntry
      if (!DDF::FindLabel(DF, arg[9], aLab)) return 1;
      if (aLab.FindAttribute(TDataStd_Integer::GetID(), TInt))
      {
        aP->NbInstances2(TInt);
      }
    }
  }
  else
  {
    if (nb > 5)
    {
      di<<"usage: SetPattern (DF,entry,signature>=5,NSentry)\n";
      return 1;
    }

    if (!DDF::FindLabel(DF, arg[4], aLab)) return 1;
    if (aLab.FindAttribute(TNaming_NamedShape::GetID(), TNS))
    {
      aP->Mirror(TNS);
    }
  }

  return 0;
}

//=======================================================================
//function : DDataStd_DumpPattern
//purpose  : DumpPattern (DF, entry)
//=======================================================================
static Standard_Integer DDataStd_DumpPattern (Draw_Interpretor& di,
                                              Standard_Integer nb, 
                                              const char** arg)
{
  Handle(TDataXtd_PatternStd) CTR;
  if (nb == 3)
  {   
    Handle(TDF_Data) DF;
    TDF_Label L;     
    if (!DDF::GetDF(arg[1],DF)) return 1;
    if (!DDF::FindLabel(DF,arg[2],L)) return 1;  
    if (L.FindAttribute(TDataXtd_PatternStd::GetID(),CTR))
    {
//      Standard_SStream aStream;
//      CTR->Dump(aStream);
//      aStream << ends;
//      di << aStream.rdbuf()->str();

      TCollection_AsciiString S;
      TDF_Tool::Entry(CTR->Label(),S); 
      di << S.ToCString() << " signature = " << CTR->Signature();

      if (!CTR->Axis1().IsNull())
      {
        TDF_Tool::Entry(CTR->Axis1()->Label(),S); 
        di << " Axis1 (" << S.ToCString() << ")";
      }

      if (!CTR->Value1().IsNull())
      {
        TDF_Tool::Entry(CTR->Value1()->Label(),S); 
        di << " Val1 (" << S.ToCString() << ")";
      }

      if (!CTR->NbInstances1().IsNull())
      {
        TDF_Tool::Entry(CTR->NbInstances1()->Label(),S); 
        di << " NbIns1 (" << S.ToCString() << ")";
      }

      if (!CTR->Axis2().IsNull())
      {
        TDF_Tool::Entry(CTR->Axis2()->Label(),S); 
        di << " Axis2 (" << S.ToCString() << ")";
      }

      if (!CTR->Value2().IsNull())
      {
        TDF_Tool::Entry(CTR->Value2()->Label(),S); 
        di << " Val2 (" << S.ToCString() << ")";
      }

      if (!CTR->NbInstances2().IsNull())
      {
        TDF_Tool::Entry(CTR->NbInstances2()->Label(),S); 
        di << " NbIns2 (" << S.ToCString() << ")";
      }

      if (!CTR->Mirror().IsNull())
      {
        TDF_Tool::Entry(CTR->Mirror()->Label(),S); 
        di << " Mirror (" << S.ToCString() << ")";
      }
    }
    else
    {
      TDF_ChildIterator it (L,Standard_True);
      for (;it.More();it.Next())
      {
        const TDF_Label& current = it.Value();
        if (current.FindAttribute(TDataXtd_PatternStd::GetID(),CTR))
        {
          Standard_SStream aStream;
//          DDataStd::DumpPattern (CTR,aStream);
          CTR->Dump(aStream);
          di << aStream;
        }
      }
    }
    return 0;
  }
  di << "DDataStd_DumpPattern : Error : not done\n";
  return 1;
}

//=======================================================================
//function : DDataStd_SetPosition
//purpose  : SetPosition (DF, entry, X, Y, Z)
//=======================================================================
static Standard_Integer DDataStd_SetPosition (Draw_Interpretor& di,
                                             Standard_Integer nb, const char** arg) 
{     
  if (nb == 6)
  {    
    Handle(TDF_Data) DF;
    if (!DDF::GetDF(arg[1],DF)) return 1;
    TDF_Label L;
    DDF::AddLabel(DF, arg[2], L);

    Standard_Real X = Draw::Atof(arg[3]), Y = Draw::Atof(arg[4]), Z = Draw::Atof(arg[5]);
    gp_Pnt aPos (X, Y, Z);

    TDataXtd_Position::Set(L,aPos);  
    return 0;
  }
  di << "Usage: SetPosition (DF, entry, X, Y, Z)\n";
  return 1;
}

//=======================================================================
//function : DDataStd_GetPosition
//purpose  : GetPosition (DF, entry, X(out), Y(out), Z(out))
//=======================================================================
static Standard_Integer DDataStd_GetPosition (Draw_Interpretor& di,
                                             Standard_Integer nb, const char** arg) 
{     
  if (nb == 6)
  {    
    Handle(TDF_Data) DF;
    if (!DDF::GetDF(arg[1],DF)) return 1;
    TDF_Label L;
    DDF::AddLabel(DF, arg[2], L);

    gp_Pnt aPos;
    if (!TDataXtd_Position::Get(L, aPos))
    {
      di << "There is no TDataStd_Position attribute on this label\n";
      return -1;
    }

    Draw::Set(arg[3],TCollection_AsciiString(aPos.X()).ToCString());
    Draw::Set(arg[4],TCollection_AsciiString(aPos.Y()).ToCString());
    Draw::Set(arg[5],TCollection_AsciiString(aPos.Z()).ToCString());
    return 0;
  }
  di << "Usage: GetPosition (DF, entry, X(out), Y(out), Z(out))\n";
  return 1;
}


//=======================================================================
//function : ConstraintCommands
//purpose  : 
//=======================================================================
void DDataStd::ConstraintCommands (Draw_Interpretor& theCommands)

{  
  static Standard_Boolean done = Standard_False;
  if (done) return;
  done = Standard_True;
  const char* g = "DData : Standard Attribute Commands";
  
  theCommands.Add ("SetConstraint",
                   "SetConstraint (DF,entry,keyword,geometrie/value[,geometrie])",
                   __FILE__, DDataStd_SetConstraint, g);   
  
  theCommands.Add ("GetConstraint",
                   "GetConstraint (DF, entry)",
                   __FILE__, DDataStd_GetConstraint, g);
  
  theCommands.Add ("SetPattern",
                   "SetPattern (DF,entry,signature,NSentry[realEntry,intEntry[,NSentry,realEntry,intEntry]])",
                   __FILE__, DDataStd_SetPattern, g);   
  
  theCommands.Add ("DumpPattern",
                   "DumpPattern (DF, entry)",
                   __FILE__, DDataStd_DumpPattern, g);


  theCommands.Add ("SetPosition", 
                   "SetPosition (DF, entry, X, Y, Z)",
                   __FILE__, DDataStd_SetPosition, g);

  theCommands.Add ("GetPosition", 
                   "GetPosition (DF, entry, X(out), Y(out), Z(out))",
                   __FILE__, DDataStd_GetPosition, g);
}
