// Created on: 2016-04-07
// Copyright (c) 2016 OPEN CASCADE SAS
// Created by: Oleg AGASHIN
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

#include <IMeshTools_MeshBuilder.hxx>
#include <IMeshData_Face.hxx>
#include <OSD_Parallel.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IMeshTools_MeshBuilder, Message_Algorithm)

//=================================================================================================

IMeshTools_MeshBuilder::IMeshTools_MeshBuilder() {}

//=================================================================================================

IMeshTools_MeshBuilder::IMeshTools_MeshBuilder(const Handle(IMeshTools_Context)& theContext)
    : myContext(theContext)
{
}

//=================================================================================================

IMeshTools_MeshBuilder::~IMeshTools_MeshBuilder() {}

//=================================================================================================

void IMeshTools_MeshBuilder::Perform(const Message_ProgressRange& theRange)
{
  ClearStatus();

  const Handle(IMeshTools_Context)& aContext = GetContext();
  if (aContext.IsNull())
  {
    SetStatus(Message_Fail1);
    return;
  }

  Message_ProgressScope aPS(theRange, "Mesh Perform", 10);

  if (aContext->BuildModel())
  {
    if (aContext->DiscretizeEdges())
    {
      if (aContext->HealModel())
      {
        if (aContext->PreProcessModel())
        {
          if (aContext->DiscretizeFaces(aPS.Next(9)))
          {
            if (aContext->PostProcessModel())
            {
              SetStatus(Message_Done1);
            }
            else
            {
              SetStatus(Message_Fail7);
            }
          }
          else
          {
            if (!aPS.More())
            {
              SetStatus(Message_Fail8);
              aContext->Clean();
              return;
            }
            SetStatus(Message_Fail6);
          }
        }
        else
        {
          SetStatus(Message_Fail5);
        }
      }
      else
      {
        SetStatus(Message_Fail4);
      }
    }
    else
    {
      SetStatus(Message_Fail3);
    }
  }
  else
  {
    const Handle(IMeshTools_ModelBuilder)& aModelBuilder = aContext->GetModelBuilder();

    if (aModelBuilder.IsNull())
    {
      SetStatus(Message_Fail1);
    }
    else
    {
      // Is null shape or another problem?
      SetStatus(aModelBuilder->GetStatus().IsSet(Message_Fail1) ? Message_Warn1 : Message_Fail2);
    }
  }
  aPS.Next(1);
  aContext->Clean();
}
