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

#include <Interface_EntityIterator.hxx>
#include <Interface_ShareTool.hxx>
#include "RWStepShape_RWFaceBound.pxx"
#include <StepData_StepReaderData.hxx>
#include <StepData_StepWriter.hxx>
#include <StepShape_EdgeLoop.hxx>
#include <StepShape_FaceBound.hxx>
#include <StepShape_Loop.hxx>
#include <StepShape_OrientedEdge.hxx>

RWStepShape_RWFaceBound::RWStepShape_RWFaceBound() {}

void RWStepShape_RWFaceBound::ReadStep(const occ::handle<StepData_StepReaderData>& data,
                                       const int                 num,
                                       occ::handle<Interface_Check>&               ach,
                                       const occ::handle<StepShape_FaceBound>&     ent) const
{

  // --- Number of Parameter Control ---

  if (!data->CheckNbParams(num, 3, ach, "face_bound"))
    return;

  // --- inherited field : name ---

  occ::handle<TCollection_HAsciiString> aName;
  // szv#4:S4163:12Mar99 `bool stat1 =` not needed
  data->ReadString(num, 1, "name", ach, aName);

  // --- own field : bound ---

  occ::handle<StepShape_Loop> aBound;
  // szv#4:S4163:12Mar99 `bool stat2 =` not needed
  data->ReadEntity(num, 2, "bound", ach, STANDARD_TYPE(StepShape_Loop), aBound);

  // --- own field : orientation ---

  bool aOrientation;
  // szv#4:S4163:12Mar99 `bool stat3 =` not needed
  data->ReadBoolean(num, 3, "orientation", ach, aOrientation);

  //--- Initialisation of the read entity ---

  ent->Init(aName, aBound, aOrientation);
}

void RWStepShape_RWFaceBound::WriteStep(StepData_StepWriter&               SW,
                                        const occ::handle<StepShape_FaceBound>& ent) const
{

  // --- inherited field name ---

  SW.Send(ent->Name());

  // --- own field : bound ---

  SW.Send(ent->Bound());

  // --- own field : orientation ---

  SW.SendBoolean(ent->Orientation());
}

void RWStepShape_RWFaceBound::Share(const occ::handle<StepShape_FaceBound>& ent,
                                    Interface_EntityIterator&          iter) const
{

  iter.GetOneItem(ent->Bound());
}

void RWStepShape_RWFaceBound::Check(const occ::handle<StepShape_FaceBound>& ent,
                                    const Interface_ShareTool&         aShto,
                                    occ::handle<Interface_Check>&           ach) const
{
  bool           theFBOri2 = true;
  bool           theFBOri1 = ent->Orientation();
  occ::handle<StepShape_EdgeLoop> theEL1    = occ::down_cast<StepShape_EdgeLoop>(ent->Bound());
  if (!theEL1.IsNull())
  {
    int nbEdg = theEL1->NbEdgeList();
    for (int i = 1; i <= nbEdg; i++)
    {
      occ::handle<StepShape_OrientedEdge> theOE1  = theEL1->EdgeListValue(i);
      occ::handle<StepShape_Edge>         theEdg1 = theOE1->EdgeElement();
      Interface_EntityIterator       myShRef = aShto.Sharings(theEdg1);
      myShRef.SelectType(STANDARD_TYPE(StepShape_OrientedEdge), true);
      int nbRef = myShRef.NbEntities();
      if (nbRef == 1)
      {
        //      ach.AddWarning("EdgeCurve only once referenced");
      }
      else if (nbRef == 2)
      {
        occ::handle<StepShape_OrientedEdge> theOE2;
        occ::handle<StepShape_OrientedEdge> refOE1 =
          occ::down_cast<StepShape_OrientedEdge>(myShRef.Value());
        myShRef.Next();
        occ::handle<StepShape_OrientedEdge> refOE2 =
          occ::down_cast<StepShape_OrientedEdge>(myShRef.Value());
        if (theOE1 == refOE1)
          theOE2 = refOE2;
        else if (theOE1 == refOE2)
          theOE2 = refOE1;

        // get the FaceBound orientation for theOE2

        bool sharOE2 = aShto.IsShared(theOE2);
        if (!sharOE2)
        {
#ifdef OCCT_DEBUG
          std::cout << "OrientedEdge2 not shared" << std::endl;
#endif
        }
        else
        {
          myShRef = aShto.Sharings(theOE2);
          myShRef.SelectType(STANDARD_TYPE(StepShape_EdgeLoop), true);
          myShRef.Start();
          occ::handle<StepShape_EdgeLoop> theEL2 = occ::down_cast<StepShape_EdgeLoop>(myShRef.Value());
          bool           sharEL2 = aShto.IsShared(theEL2);
          if (!sharEL2)
          {
#ifdef OCCT_DEBUG
            std::cout << "EdgeLoop2 not shared" << std::endl;
#endif
          }
          else
          {
            myShRef = aShto.Sharings(theEL2);
            myShRef.SelectType(STANDARD_TYPE(StepShape_FaceBound), true);
            myShRef.Start();
            occ::handle<StepShape_FaceBound> theFB2 =
              occ::down_cast<StepShape_FaceBound>(myShRef.Value());
            if (!theFB2.IsNull())
            {
              theFBOri2 = theFB2->Orientation();
            }
            else
            {
#ifdef OCCT_DEBUG
              std::cout << "EdgeLoop not referenced by FaceBound" << std::endl;
#endif
            }
          }
        }

        // "cumulate" the FaceBound orientation with the OrientedEdge orientation

        bool theOEOri1 = theFBOri1 ? theOE1->Orientation() : !(theOE1->Orientation());
        bool theOEOri2 = theFBOri2 ? theOE2->Orientation() : !(theOE2->Orientation());

        // the orientation of the OrientedEdges must be opposite

        if (theOEOri1 == theOEOri2)
        {
          ach->AddFail("ERROR: non 2-manifold topology");
        }
      } // end if(nbRef == 2)
    } // end for(i=1; i<=nbEdg; ...)
  } // end if(!theEL1.IsNull)
  else
  {
#ifdef OCCT_DEBUG
    std::cout << "no EdgeLoop in FaceBound" << std::endl;
#endif
  }
}
