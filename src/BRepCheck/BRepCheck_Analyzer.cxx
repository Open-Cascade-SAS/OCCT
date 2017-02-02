// Created on: 1995-12-08
// Created by: Jacques GOUSSARD
// Copyright (c) 1995-1999 Matra Datavision
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


#include <BRepCheck_Analyzer.hxx>
#include <BRepCheck_Edge.hxx>
#include <BRepCheck_Face.hxx>
#include <BRepCheck_ListIteratorOfListOfStatus.hxx>
#include <BRepCheck_Result.hxx>
#include <BRepCheck_Shell.hxx>
#include <BRepCheck_Solid.hxx>
#include <BRepCheck_Vertex.hxx>
#include <BRepCheck_Wire.hxx>
#include <Standard_ErrorHandler.hxx>
#include <Standard_Failure.hxx>
#include <Standard_NoSuchObject.hxx>
#include <Standard_NullObject.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_MapOfShape.hxx>

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================
void BRepCheck_Analyzer::Init(const TopoDS_Shape& S,
         const Standard_Boolean B)
{
  if (S.IsNull()) {
    throw Standard_NullObject();
  }
  myShape = S;
  myMap.Clear();
  Put(S,B);
  Perform(S);
}
//=======================================================================
//function : Put
//purpose  : 
//=======================================================================
void BRepCheck_Analyzer::Put(const TopoDS_Shape& S,
                             const Standard_Boolean B)
{
  if (!myMap.IsBound(S)) {
    Handle(BRepCheck_Result) HR;
    switch (S.ShapeType()) {
    case TopAbs_VERTEX:
      HR = new BRepCheck_Vertex(TopoDS::Vertex(S));
      break;
    case TopAbs_EDGE:
      HR = new BRepCheck_Edge(TopoDS::Edge(S));
      Handle(BRepCheck_Edge)::DownCast(HR)->GeometricControls(B);
      break;
    case TopAbs_WIRE:
      HR = new BRepCheck_Wire(TopoDS::Wire(S));
      Handle(BRepCheck_Wire)::DownCast(HR)->GeometricControls(B);
      break;
    case TopAbs_FACE:
      HR = new BRepCheck_Face(TopoDS::Face(S));
      Handle(BRepCheck_Face)::DownCast(HR)->GeometricControls(B);
      break;
    case TopAbs_SHELL:
      HR = new BRepCheck_Shell(TopoDS::Shell(S));
      break;
    case TopAbs_SOLID:
      HR = new BRepCheck_Solid(TopoDS::Solid(S));
      break;
    case TopAbs_COMPSOLID:
    case TopAbs_COMPOUND:
      break;
    default:
      break;
    }
    myMap.Bind(S,HR);
    for(TopoDS_Iterator theIterator(S);theIterator.More();theIterator.Next()) {
      Put(theIterator.Value(),B); // performs minimum on each shape
    }
  }
}
//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================
void BRepCheck_Analyzer::Perform(const TopoDS_Shape& S)
{
  for(TopoDS_Iterator theIterator(S);theIterator.More();theIterator.Next()) 
    Perform(theIterator.Value());
  
  //
  TopAbs_ShapeEnum styp;
  TopExp_Explorer exp;
  //
  styp = S.ShapeType();
  
  switch (styp) 
  {
  case TopAbs_VERTEX: 
    // modified by NIZHNY-MKK  Wed May 19 16:56:16 2004.BEGIN
    // There is no need to check anything.
    //       if (myShape.IsSame(S)) {
    //  myMap(S)->Blind();
    //       }
    // modified by NIZHNY-MKK  Wed May 19 16:56:23 2004.END
  
    break;
  case TopAbs_EDGE:
    {
      Handle(BRepCheck_Result)& aRes = myMap(S);

      try
      {
        BRepCheck_Status ste = Handle(BRepCheck_Edge)::
          DownCast(aRes)->CheckPolygonOnTriangulation(TopoDS::Edge(S));

        if(ste != BRepCheck_NoError)
        {
          Handle(BRepCheck_Edge)::DownCast(aRes)->SetStatus(ste);
        }
      }
      catch(Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
        cout<<"BRepCheck_Analyzer : ";
        anException.Print(cout);  
        cout<<endl;
#endif
        (void)anException;
        if ( ! myMap(S).IsNull() )
        {
          myMap(S)->SetFailStatus(S);
        }

        if ( ! aRes.IsNull() )
        {
          aRes->SetFailStatus(exp.Current());
          aRes->SetFailStatus(S);
        }
      }

      TopTools_MapOfShape MapS;
      
      for (exp.Init(S,TopAbs_VERTEX);exp.More(); exp.Next())
      {
        const TopoDS_Shape& aVertex = exp.Current();
        try
        {
          OCC_CATCH_SIGNALS
          if (MapS.Add(aVertex))
            myMap(aVertex)->InContext(S);
        }
        catch(Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
          cout<<"BRepCheck_Analyzer : ";
          anException.Print(cout);  
          cout<<endl;
#endif
          (void)anException;
          if ( ! myMap(S).IsNull() )
            myMap(S)->SetFailStatus(S);

          Handle(BRepCheck_Result) aResOfVertex = myMap(aVertex);

          if ( !aResOfVertex.IsNull() )
          {
            aResOfVertex->SetFailStatus(aVertex);
            aResOfVertex->SetFailStatus(S);
          }
        }//catch(Standard_Failure)
      }//for (exp.Init(S,TopAbs_VERTEX);exp.More(); exp.Next())
    }
    break;
  case TopAbs_WIRE:
    {
    }
    break;
  case TopAbs_FACE:
    {
      TopTools_MapOfShape MapS;
      for (exp.Init(S,TopAbs_VERTEX);exp.More(); exp.Next())
      {
        try
        {
          OCC_CATCH_SIGNALS
          if (MapS.Add(exp.Current()))
          {
            myMap(exp.Current())->InContext(S);
          }
        }
        catch(Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
          cout<<"BRepCheck_Analyzer : ";
          anException.Print(cout);  
          cout<<endl;
#endif
          (void)anException;
          if ( ! myMap(S).IsNull() )
          {
            myMap(S)->SetFailStatus(S);
          }
          
          Handle(BRepCheck_Result) aRes = myMap(exp.Current());

          if ( ! aRes.IsNull() )
          {
            aRes->SetFailStatus(exp.Current());
            aRes->SetFailStatus(S);
          }
        }
      }

      Standard_Boolean performwire = Standard_True;
      Standard_Boolean isInvalidTolerance = Standard_False;
      MapS.Clear();
      for (exp.Init(S,TopAbs_EDGE);exp.More(); exp.Next())
      {
        try
        {
          OCC_CATCH_SIGNALS
          if (MapS.Add(exp.Current()))
          {
            Handle(BRepCheck_Result)& res = myMap(exp.Current());
            res->InContext(S);
            if (performwire)
            {
              for ( res->InitContextIterator();
                    res->MoreShapeInContext();
                    res->NextShapeInContext())
              {
                if(res->ContextualShape().IsSame(S))
                  break;
              }

              BRepCheck_ListIteratorOfListOfStatus itl(res->StatusOnShape());
              for (; itl.More(); itl.Next())
              {
                BRepCheck_Status ste = itl.Value();
                if (ste == BRepCheck_NoCurveOnSurface  ||
                    ste == BRepCheck_InvalidCurveOnSurface ||
                    ste == BRepCheck_InvalidRange ||
                    ste == BRepCheck_InvalidCurveOnClosedSurface)
                {
                  performwire = Standard_False;
                  break;
                }
              }
            }
          }
        }
        catch(Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
          cout<<"BRepCheck_Analyzer : ";
          anException.Print(cout);  
          cout<<endl;
#endif
          (void)anException;
          if ( ! myMap(S).IsNull() )
          {
            myMap(S)->SetFailStatus(S);
          }

          Handle(BRepCheck_Result) aRes = myMap(exp.Current());

          if ( ! aRes.IsNull() )
          {
            aRes->SetFailStatus(exp.Current());
            aRes->SetFailStatus(S);
          }
        }
      }

      Standard_Boolean orientofwires = performwire;
      for (exp.Init(S,TopAbs_WIRE);exp.More(); exp.Next())
      {
        try
        {
          OCC_CATCH_SIGNALS
          Handle(BRepCheck_Result)& res = myMap(exp.Current());
          res->InContext(S);
          if (orientofwires)
          {
            for ( res->InitContextIterator();
                  res->MoreShapeInContext();
                  res->NextShapeInContext())
            {
              if(res->ContextualShape().IsSame(S))
              {
                break;
              }
            }
            BRepCheck_ListIteratorOfListOfStatus itl(res->StatusOnShape());
            for (; itl.More(); itl.Next())
            {
              BRepCheck_Status ste = itl.Value();
              if (ste != BRepCheck_NoError)
              {
                orientofwires = Standard_False;
                break;
              }
            }
          }
        }
        catch(Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
          cout<<"BRepCheck_Analyzer : ";
          anException.Print(cout);  
          cout<<endl;
#endif
          (void)anException;
          if ( ! myMap(S).IsNull() )
          {
            myMap(S)->SetFailStatus(S);
          }

          Handle(BRepCheck_Result) aRes = myMap(exp.Current());

          if ( ! aRes.IsNull() )
          {
            aRes->SetFailStatus(exp.Current());
            aRes->SetFailStatus(S);
          }
        }
      }

      try
      {
        OCC_CATCH_SIGNALS
        if(isInvalidTolerance)
        {
          Handle(BRepCheck_Face)::
              DownCast(myMap(S))->SetStatus(BRepCheck_InvalidToleranceValue);
        }
        else if (performwire)
        {
          if (orientofwires)
          {
            Handle(BRepCheck_Face)::DownCast(myMap(S))->
                        OrientationOfWires(Standard_True);// on enregistre
          }
          else
          {
            Handle(BRepCheck_Face)::DownCast(myMap(S))->SetUnorientable();
          }
        }
        else
        {
          Handle(BRepCheck_Face)::DownCast(myMap(S))->SetUnorientable();
        }
      }
      catch(Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
        cout<<"BRepCheck_Analyzer : ";
        anException.Print(cout);  
        cout<<endl;
#endif
        (void)anException;
        if ( ! myMap(S).IsNull() )
        {
          myMap(S)->SetFailStatus(S);
        }

        for (exp.Init(S,TopAbs_WIRE);exp.More(); exp.Next())
        {
          Handle(BRepCheck_Result) aRes = myMap(exp.Current());
          
          if ( ! aRes.IsNull() )
          {
            aRes->SetFailStatus(exp.Current());
            aRes->SetFailStatus(S);
            myMap(S)->SetFailStatus(exp.Current());
          }
        }
      }
    }
    break;
    
  case TopAbs_SHELL:   
    break;

  case TopAbs_SOLID:
    {
      exp.Init(S,TopAbs_SHELL);
      for (; exp.More(); exp.Next())
        {
          const TopoDS_Shape& aShell=exp.Current();
          try 
            {
              OCC_CATCH_SIGNALS
                myMap(aShell)->InContext(S);
            }
          catch(Standard_Failure const& anException) {
#ifdef OCCT_DEBUG
              cout<<"BRepCheck_Analyzer : ";
              anException.Print(cout);  
              cout<<endl;
#endif
              (void)anException;
              if ( ! myMap(S).IsNull() )
                {
                  myMap(S)->SetFailStatus(S);
                }
              
              //
              Handle(BRepCheck_Result) aRes = myMap(aShell);
              if (!aRes.IsNull() )
                {
                  aRes->SetFailStatus(exp.Current());
                  aRes->SetFailStatus(S);
                }
            }//catch(Standard_Failure)
        }//for (; exp.More(); exp.Next())
    }
  break;//case TopAbs_SOLID
  default:
    break;
  }//switch (styp) {
}


//=======================================================================
//function : IsValid
//purpose  : 
//=======================================================================

Standard_Boolean BRepCheck_Analyzer::IsValid(const TopoDS_Shape& S) const
{
  if (!myMap(S).IsNull()) {
    BRepCheck_ListIteratorOfListOfStatus itl;
    itl.Initialize(myMap(S)->Status());
    if (itl.Value() != BRepCheck_NoError) { // a voir
      return Standard_False;
    }
  }

  for(TopoDS_Iterator theIterator(S);theIterator.More();theIterator.Next()) {
    if (!IsValid(theIterator.Value())) {
      return Standard_False;
    }
  }

  switch (S.ShapeType()) {
  case TopAbs_EDGE:
    {
      return ValidSub(S,TopAbs_VERTEX);
    }
//    break;
  case TopAbs_FACE:
    {
      Standard_Boolean valid = ValidSub(S,TopAbs_WIRE);
      valid = valid && ValidSub(S,TopAbs_EDGE);
      valid = valid && ValidSub(S,TopAbs_VERTEX);
      return valid;
    }

//    break;
  case TopAbs_SHELL:
//    return ValidSub(S,TopAbs_FACE);
    break;
  case TopAbs_SOLID:
//    return ValidSub(S,TopAbs_EDGE);
//    break;
    return ValidSub(S,TopAbs_SHELL);
    break;
  default:
    break;
  }

  return Standard_True;
}

//=======================================================================
//function : ValidSub
//purpose  : 
//=======================================================================

Standard_Boolean BRepCheck_Analyzer::ValidSub
   (const TopoDS_Shape& S,
    const TopAbs_ShapeEnum SubType) const
{
  BRepCheck_ListIteratorOfListOfStatus itl;
  TopExp_Explorer exp;
  for (exp.Init(S,SubType);exp.More(); exp.Next()) {
//  for (TopExp_Explorer exp(S,SubType);exp.More(); exp.Next()) {
    const Handle(BRepCheck_Result)& RV = myMap(exp.Current());
    for (RV->InitContextIterator();
         RV->MoreShapeInContext(); 
         RV->NextShapeInContext()) {
      if (RV->ContextualShape().IsSame(S)) {
        break;
      }
    }

    if(!RV->MoreShapeInContext()) break;

    for (itl.Initialize(RV->StatusOnShape()); itl.More(); itl.Next()) {
      if (itl.Value() != BRepCheck_NoError) {
        return Standard_False;
      }
    }
  }
  return Standard_True ;
}
