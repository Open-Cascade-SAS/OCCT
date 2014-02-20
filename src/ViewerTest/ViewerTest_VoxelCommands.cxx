// Created on: 2008-09-17
// Created by: Vlad Romashko
// Copyright (c) 2008-2014 OPEN CASCADE SAS
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

#include <ViewerTest.hxx>

#include <DBRep.hxx>
#include <Draw_Interpretor.hxx>
#include <Draw_Appli.hxx>

#include <Voxel_BoolDS.hxx>
#include <Voxel_ColorDS.hxx>
#include <Voxel_FloatDS.hxx>
#include <Voxel_OctBoolDS.hxx>
#include <Voxel_ROctBoolDS.hxx>
#include <Voxel_BooleanOperation.hxx>
#include <Voxel_FastConverter.hxx>

// A method parses the input parameters.
static Standard_Boolean GetInputParameters(Standard_Integer  nbargs, const char** args, 
                                           Standard_Real&    x,  Standard_Real&     y,  Standard_Real&     z, 
                                           Standard_Real&    dx, Standard_Real&     dy, Standard_Real&     dz, 
                                           Standard_Integer& nbx, Standard_Integer& nby, Standard_Integer& nbz)
{
  // Default values.
  nbx = 10; nby = 10; nbz = 10;
  x = 0.0; y = 0.0; z = 0.0;
  dx = 1.0; dy = 1.0; dz = 1.0;

  // "voxelboolds 0 0 0 1 1 1 100 100 100"
  if (nbargs == 10)
  {
    nbx = Draw::Atoi(args[7]);
    nby = Draw::Atoi(args[8]);
    nbz = Draw::Atoi(args[9]);
  }
  // "voxelboolds 0 0 0 1 1 1"
  if (nbargs == 7 || nbargs == 10)
  {
    dx = Draw::Atof(args[4]);
    dy = Draw::Atof(args[5]);
    dz = Draw::Atof(args[6]);
  }
  // "voxelboolds 0 0 0"
  if (nbargs == 4 || nbargs == 7 || nbargs == 10)
  {
    x = Draw::Atof(args[1]);
    y = Draw::Atof(args[2]);
    z = Draw::Atof(args[3]);
  }
  // "voxelboolds"
  if (nbargs == 1)
  {
    // use default parameters
  }

  return nbargs == 1 || nbargs == 4 || nbargs == 7 || nbargs == 10;
}

static Standard_Integer VOXELBOOLDS(Draw_Interpretor& di, Standard_Integer nbargs, const char** args)
{
  // Parse input parameters.
  Standard_Real      x, y, z, dx, dy, dz;
  Standard_Integer   ix, iy, iz, nbx, nby, nbz;
  if (!GetInputParameters(nbargs, args, x, y, z, dx, dy, dz, nbx, nby, nbz))
  {
    di << "Usage: voxelboolds [x, y, z, dx, dy, dz nbx, nby, nbz]";
    return 1;
  }

  // 1. BoolDS:
  Voxel_BoolDS ds(x, y, z, dx, dy, dz, nbx, nby, nbz);

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	if (ix & 0x01)
	  ds.Set(ix, iy, iz, Standard_False);
	else
	  ds.Set(ix, iy, iz, Standard_True);
      }
    }
  }

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	Standard_Boolean value = ds.Get(ix, iy, iz) == Standard_True;
	if (ix & 0x01)
	{
	  if (value != Standard_False)
	  {
	    di<<"error : voxelboolds";
	    return 1; // error
	  }
	}
	else
        {
	  if (value != Standard_True)
	  {
	    di<<"error : voxelboolds";
	    return 1; // error
	  }
	}
      }
    }
  }

  return 0;
}

static Standard_Integer VOXELCOLORDS(Draw_Interpretor& di, Standard_Integer nbargs, const char** args)
{
  // Parse input parameters.
  Standard_Real      x, y, z, dx, dy, dz;
  Standard_Integer   ix, iy, iz, nbx, nby, nbz;
  if (!GetInputParameters(nbargs, args, x, y, z, dx, dy, dz, nbx, nby, nbz))
  {
    di << "Usage: voxelcolords [x, y, z, dx, dy, dz nbx, nby, nbz]";
    return 1;
  }

  // 1. ColorDS:
  Voxel_ColorDS ds(x, y, z, dx, dy, dz, nbx, nby, nbz);
  
  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	if (ix & 0x01)
	  ds.Set(ix, iy, iz, 8);
	else
	  ds.Set(ix, iy, iz, 7);
      }
    }
  }

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	Standard_Byte value = ds.Get(ix, iy, iz);
	if (ix & 0x01)
	{
	  if (value != 8)
	  {
	    di<<"error : voxelcolords";
	    return 1;
	  }
	}
	else
	{
	  if (value != 7)
	  {
	    di<<"error : voxelcolords";
	    return 1;
	  }
	}
      }
    }
  }

  return 0;
}

static Standard_Integer VOXELFLOATDS(Draw_Interpretor& di, Standard_Integer nbargs, const char** args)
{
  // Parse input parameters.
  Standard_Real      x, y, z, dx, dy, dz;
  Standard_Integer   ix, iy, iz, nbx, nby, nbz;
  if (!GetInputParameters(nbargs, args, x, y, z, dx, dy, dz, nbx, nby, nbz))
  {
    di << "Usage: voxelfloatds [x, y, z, dx, dy, dz nbx, nby, nbz]";
    return 1;
  }
  
  // 1. FloatDS:
  Voxel_FloatDS ds(x, y, z, dx, dy, dz, nbx, nby, nbz);
  
  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	if (ix & 0x01)
	  ds.Set(ix, iy, iz, 8.8f);
	else
	  ds.Set(ix, iy, iz, 7.7f);
      }
    }
  }

  /*
     // Write the voxels
     Voxel_Writer writer;
     writer.SetFormat(Voxel_VFF_BINARY);
     writer.SetVoxels(ds);
     if (!writer.Write("W:\\OCC621\\samples\\standard\\voxeldemo\\f.vx"))
     {
     statusBar()->message( "Storage failed... sorry", 2000 );
     return;
     }
     
     // Read the voxels
     Voxel_Reader reader;
     if (!reader.Read("W:\\OCC621\\samples\\standard\\voxeldemo\\f.vx"))
     {
     statusBar()->message( "Open failed... sorry", 2000 );
     return;
     }
  */

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	float value = ds.Get(ix, iy, iz);
	if (ix & 0x01)
	{
	  if (value != 8.8f)
	  {
	    di<<"error : voxelfloatds";
	    return 1;
	  }
	}
	else
	{
	  if (value != 7.7f)
	  {
	    di<<"error : voxelfloatds";
	    return 1;
	  }
	}
      }
    }
  }
  
  return 0;
}

static Standard_Integer VOXELOCTBOOLDS(Draw_Interpretor& di, Standard_Integer nbargs, const char** args)
{
  // Parse input parameters.
  Standard_Real      x, y, z, dx, dy, dz;
  Standard_Integer   ix, iy, iz, nbx, nby, nbz;
  if (!GetInputParameters(nbargs, args, x, y, z, dx, dy, dz, nbx, nby, nbz))
  {
    di << "Usage: voxeloctboolds [x, y, z, dx, dy, dz nbx, nby, nbz]";
    return 1;
  }

  // 1. OctBoolDS:
  Voxel_OctBoolDS ds(x, y, z, dx, dy, dz, nbx, nby, nbz);

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	if (ix & 0x01)
	{
	  ds.Set(ix, iy, iz, Standard_True);
	}
	else
	{
	  for (Standard_Integer i = 0; i < 8; i++)
	  {
	    if (i & 0x01)
	      ds.Set(ix, iy, iz, i, Standard_True);
	    else
	      ds.Set(ix, iy, iz, i, Standard_False);
	  }
	}
      }
    }
  }

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	if (ix & 0x01)
        {
	  Standard_Boolean value = ds.Get(ix, iy, iz) == Standard_True;
	  if (value != Standard_True)
	    cout<<"Wrong value!"<<endl;
	}
	else
	{
	  for (Standard_Integer i = 0; i < 8; i++)
	  {
	    if (i & 0x01)
	    {
	      Standard_Boolean value = ds.Get(ix, iy, iz, i) == Standard_True;
	      if (value != Standard_True)
	      {
		di<<"error : voxeloctboolds";
		return 1;
	      }
	    }
	    else
	    {
	      Standard_Boolean value = ds.Get(ix, iy, iz, i) == Standard_True;
	      if (value != Standard_False)
	      {
		di<<"error : voxeloctboolds";
		return 1;
	      }
	    }
	  }
	}
      }
    }
  }

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	if (ix & 0x01)
	{
	  for (Standard_Integer i = 0; i < 8; i++)
	  {
	    ds.Set(ix, iy, iz, i, Standard_True);
	  }
	}
	else
	{
	  for (Standard_Integer i = 0; i < 8; i++)
	  {
	    ds.Set(ix, iy, iz, i, Standard_False);
	  }
	}
      }
    }
  }

  ds.OptimizeMemory();

  return 0;
}

static Standard_Integer VOXELROCTBOOLDS(Draw_Interpretor& di, Standard_Integer nbargs, const char** args)
{
  // Parse input parameters.
  Standard_Real      x, y, z, dx, dy, dz;
  Standard_Integer   ix, iy, iz, nbx, nby, nbz, i, j;
  if (!GetInputParameters(nbargs, args, x, y, z, dx, dy, dz, nbx, nby, nbz))
  {
    di << "Usage: voxelroctboolds [x, y, z, dx, dy, dz nbx, nby, nbz]";
    return 1;
  }

  // 1. ROctBoolDS:
  Voxel_ROctBoolDS ds(x, y, z, dx, dy, dz, nbx, nby, nbz);

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	ds.Set(ix, iy, iz, Standard_True);
      }
    }
  }

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	for (i = 0; i < 8; i++)
	{
	  for (j = 0; j < 8; j++)
	  {
	    ds.Set(ix, iy, iz, i, j, Standard_True);
	  }
	}
      }
    }
  }

  ds.OptimizeMemory();

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	if (ds.Deepness(ix, iy, iz) == 0)
	{
	  Standard_Boolean value = ds.Get(ix, iy, iz);
	  if (value != Standard_True)
	  {
	    di<<"error : voxelroctboolds";
	    return 1;
	  }
	}
	if (ds.Deepness(ix, iy, iz) == 1)
	{
	  for (i = 0; i < 8; i++)
	  {
	    Standard_Boolean value = ds.Get(ix, iy, iz, i);
	    if (value != Standard_True)
	    {
	      di<<"error : voxelroctboolds";
	      return 1;
	    }
	  }
	}
	if (ds.Deepness(ix, iy, iz) == 2)
	{
	  for (i = 0; i < 8; i++)
	  {
	    for (j = 0; j < 8; j++)
	    {
	      Standard_Boolean value = ds.Get(ix, iy, iz, i, j);
	      if (value != Standard_True)
	      {
		di<<"error : voxelroctboolds";
		return 1;
	      }
	    }
	  }
	}
      }
    }
  }

  return 0;
}

static Standard_Integer VOXELFUSEBOOLDS(Draw_Interpretor& di, Standard_Integer nbargs, const char** args)
{
  // Parse input parameters.
  Standard_Real      x, y, z, dx, dy, dz;
  Standard_Integer   ix, iy, iz, nbx, nby, nbz;
  if (!GetInputParameters(nbargs, args, x, y, z, dx, dy, dz, nbx, nby, nbz))
  {
    di << "Usage: voxelfuseboolds [x, y, z, dx, dy, dz nbx, nby, nbz]";
    return 1;
  }

  // 1. Set two BoolDS:
  Voxel_BoolDS ds1(x, y, z, dx, dy, dz, nbx, nby, nbz);
  Voxel_BoolDS ds2(x, y, z, dx, dy, dz, nbx, nby, nbz);

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	if (ix & 0x01)
	  ds2.Set(ix, iy, iz, Standard_False);
	else
	  ds2.Set(ix, iy, iz, Standard_True);
      }
    }
  }

  // 2. Fuse them

  Voxel_BooleanOperation fuser;
  if (!fuser.Fuse(ds1, ds2))
  {
    di<<"error : voxelfuseboolds";
    return 1;
  }

  // 3. Check result
  
  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	Standard_Boolean value = ds1.Get(ix, iy, iz) == Standard_True;
	if (ix & 0x01)
	{
	  if (value != Standard_False)
	  {
	    di<<"error : voxelfuseboolds";
	    return 1;
	  }
	}
	else
	{
	  if (value != Standard_True)
	  {
	    di<<"error : voxelfuseboolds";
	    return 1;
	  }
	}
      }
    }
  }

  return 0;
}

static Standard_Integer VOXELFUSECOLORDS(Draw_Interpretor& di, Standard_Integer nbargs, const char** args)
{
  // Parse input parameters.
  Standard_Real      x, y, z, dx, dy, dz;
  Standard_Integer   ix, iy, iz, nbx, nby, nbz;
  if (!GetInputParameters(nbargs, args, x, y, z, dx, dy, dz, nbx, nby, nbz))
  {
    di << "Usage: voxelfusecolords [x, y, z, dx, dy, dz nbx, nby, nbz]";
    return 1;
  }

  // 1. Set two ColorDS:
  Voxel_ColorDS ds1(x, y, z, dx, dy, dz, nbx, nby, nbz);
  Voxel_ColorDS ds2(x, y, z, dx, dy, dz, nbx, nby, nbz);
  
  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	ds1.Set(ix, iy, iz, 11);
      }
    }
  }
  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	if (ix & 0x01)
	  ds2.Set(ix, iy, iz, 3);
	else
	  ds2.Set(ix, iy, iz, 5);
      }
    }
  }
  
  // 2. Fuse them
  
  Voxel_BooleanOperation fuser;
  if (!fuser.Fuse(ds1, ds2))
  {
    di<<"error : voxelfusecolords";
    return 1;
  }

  // 3. Check result

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	Standard_Byte value = ds1.Get(ix, iy, iz);
	if (ix & 0x01)
	{
	  if (value != 14)
	  {
	    di<<"error : voxelfusecolords";
	    return 1;
	  }
	}
	else
	{
	  if (value != 15)
	  {
	    di<<"error : voxelfusecolords";
	    return 1;
	  }
	}
      }
    }
  }

  return 0;
}

static Standard_Integer VOXELFUSEFLOATDS(Draw_Interpretor& di, Standard_Integer nbargs, const char** args)
{
  // Parse input parameters.
  Standard_Real      x, y, z, dx, dy, dz;
  Standard_Integer   ix, iy, iz, nbx, nby, nbz;
  if (!GetInputParameters(nbargs, args, x, y, z, dx, dy, dz, nbx, nby, nbz))
  {
    di << "Usage: voxelfusefloatds [x, y, z, dx, dy, dz nbx, nby, nbz]";
    return 1;
  }

  // 1. Set two FloatDS:
  Voxel_FloatDS ds1(x, y, z, dx, dy, dz, nbx, nby, nbz);
  Voxel_FloatDS ds2(x, y, z, dx, dy, dz, nbx, nby, nbz);

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	ds1.Set(ix, iy, iz, 11.1f);
      }
    }
  }
  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	if (ix & 0x01)
	  ds2.Set(ix, iy, iz, 3.3f);
	else
	  ds2.Set(ix, iy, iz, 5.5f);
      }
    }
  }

  // 2. Fuse them
  
  Voxel_BooleanOperation fuser;
  if (!fuser.Fuse(ds1, ds2))
  {
    di<<"error : voxelfusefloatds";
    return 1;
  }

  // 3. Check result

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	float value = ds1.Get(ix, iy, iz);
	if (ix & 0x01)
	{
	  if (fabs(value - 14.4f) > 0.001)
	  {
	    di<<"error : voxelfusefloatds";
	    return 1;
	  }
	}
	else
	{
	  if (fabs(value - 16.6f) > 0.001)
	  {
	    di<<"error : voxelfusefloatds";
	    return 1;
	  }
	}
      }
    }
  }

  return 0;
}

static Standard_Integer VOXELCUTBOOLDS(Draw_Interpretor& di, Standard_Integer nbargs, const char** args)
{
  // Parse input parameters.
  Standard_Real      x, y, z, dx, dy, dz;
  Standard_Integer   ix, iy, iz, nbx, nby, nbz;
  if (!GetInputParameters(nbargs, args, x, y, z, dx, dy, dz, nbx, nby, nbz))
  {
    di << "Usage: voxelcutboolds [x, y, z, dx, dy, dz nbx, nby, nbz]";
    return 1;
  }

  // 1. Set two BoolDS:
  Voxel_BoolDS ds1(x, y, z, dx, dy, dz, nbx, nby, nbz);
  Voxel_BoolDS ds2(x, y, z, dx, dy, dz, nbx, nby, nbz);

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	ds1.Set(ix, iy, iz, Standard_True);
      }
    }
  }
  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	if (ix & 0x01)
	  ds2.Set(ix, iy, iz, Standard_False);
	else
	  ds2.Set(ix, iy, iz, Standard_True);
      }
    }
  }
  
  // 2. Cut them

  Voxel_BooleanOperation cutter;
  if (!cutter.Cut(ds1, ds2))
  {
    di<<"error : voxelcutboolds";
    return 1;
  }

  // 3. Check result

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	Standard_Boolean value = ds1.Get(ix, iy, iz) == Standard_True;
	if (ix & 0x01)
	{
	  if (value != Standard_True)
	  {
	    di<<"error : voxelcutboolds";
	    return 1;
	  }
	}
	else
	{
	  if (value != Standard_False)
	  {
	    di<<"error : voxelcutboolds";
	    return 1;
	  }
	}
      }
    }
  }
  
  return 0;
}

static Standard_Integer VOXELCUTCOLORDS(Draw_Interpretor& di, Standard_Integer nbargs, const char** args)
{
  // Parse input parameters.
  Standard_Real      x, y, z, dx, dy, dz;
  Standard_Integer   ix, iy, iz, nbx, nby, nbz;
  if (!GetInputParameters(nbargs, args, x, y, z, dx, dy, dz, nbx, nby, nbz))
  {
    di << "Usage: voxelcutcolords [x, y, z, dx, dy, dz nbx, nby, nbz]";
    return 1;
  }

  // 1. Set two ColorDS:
  Voxel_ColorDS ds1(x, y, z, dx, dy, dz, nbx, nby, nbz);
  Voxel_ColorDS ds2(x, y, z, dx, dy, dz, nbx, nby, nbz);

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	ds1.Set(ix, iy, iz, 11);
      }
    }
  }
  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	if (ix & 0x01)
	  ds2.Set(ix, iy, iz, 3);
	else
	  ds2.Set(ix, iy, iz, 5);
      }
    }
  }

  // 2. Cut them
  
  Voxel_BooleanOperation cutter;
  if (!cutter.Cut(ds1, ds2))
  {
    di<<"error : voxelcutcolords";
    return 1;
  }

  // 3. Check result

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	Standard_Byte value = ds1.Get(ix, iy, iz);
	if (ix & 0x01)
	{
	  if (value != 8)
	  {
	    di<<"error : voxelcutcolords";
	    return 1;
	  }
	}
	else
	{
	  if (value != 6)
	  {
	    di<<"error : voxelcutcolords";
	    return 1;
	  }
	}
      }
    }
  }

  return 0;
}

static Standard_Integer VOXELCUTFLOATDS(Draw_Interpretor& di, Standard_Integer nbargs, const char** args)
{
  // Parse input parameters.
  Standard_Real      x, y, z, dx, dy, dz;
  Standard_Integer   ix, iy, iz, nbx, nby, nbz;
  if (!GetInputParameters(nbargs, args, x, y, z, dx, dy, dz, nbx, nby, nbz))
  {
    di << "Usage: voxelcutfloatds [x, y, z, dx, dy, dz nbx, nby, nbz]";
    return 1;
  }

  // 1. Set two FloatDS:
  Voxel_FloatDS ds1(x, y, z, dx, dy, dz, nbx, nby, nbz);
  Voxel_FloatDS ds2(x, y, z, dx, dy, dz, nbx, nby, nbz);

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	ds1.Set(ix, iy, iz, 11.1f);
      }
    }
  }
  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	if (ix & 0x01)
	  ds2.Set(ix, iy, iz, 3.3f);
	else
	  ds2.Set(ix, iy, iz, 5.5f);
      }
    }
  }

  // 2. Cut them
  
  Voxel_BooleanOperation cutter;
  if (!cutter.Cut(ds1, ds2))
  {
    di<<"error : voxelcutcolords";
    return 1;
  }

  // 3. Check result

  for (ix = 0; ix < nbx; ix++)
  {
    for (iy = 0; iy < nby; iy++)
    {
      for (iz = 0; iz < nbz; iz++)
      {
	float value = ds1.Get(ix, iy, iz);
	if (ix & 0x01)
	{
	  if (fabs(value - 7.8f) > 0.001)
	  {
	    di<<"error : voxelcutcolords";
	    return 1;
	  }
	}
	else
	{
	  if (fabs(value - 5.6f) > 0.001)
	  {
	    di<<"error : voxelcutcolords";
	    return 1;
	  }
	}
      }
    }
  }

  return 0;
}

static Standard_Integer VOXELBOOLDSCONVERT(Draw_Interpretor& di, Standard_Integer nbargs, const char** args)
{
  TopoDS_Shape S;
  Standard_Integer nbx = 100, nby = 100, nbz = 100;
  Standard_Real deflection = 0.1;
  Standard_Boolean fast = Standard_True;
  Standard_Integer nbthreads = 1;
  Standard_Boolean usetriangulation = Standard_False;
  Standard_Integer fillInVolume = 0;

  if (nbargs < 2)
  {
    di << "Usage: voxelbooldsconvert shape [nbx nby nbz deflection 0|1(fast or accurate) nbthreads usetriangulation \
          0|1|2(fill-in volume: no|yes|yes using shape)]";
    return 1;
  }

  // Get shape for conversion.
  S = DBRep::Get(args[1]);

  if (nbargs > 2)
  {
    if (nbargs >= 5)
    {
      nbx = Draw::Atoi(args[2]);
      nby = Draw::Atoi(args[3]);
      nbz = Draw::Atoi(args[4]);
    }
    if (nbargs >= 6)
    {
      deflection = Draw::Atof(args[5]);
    }
    if (nbargs >= 7)
    {
      // 0 means fast,
      // 1 means accurate.
      fast = Draw::Atoi(args[6]) == 0 ? Standard_True : Standard_False;
    }
    if (nbargs >= 8)
    {
      nbthreads = Draw::Atoi(args[7]);
      if (nbthreads < 1)
      {
        di << "Wrong number of threads: 1 .. nb cores";
        return 1;
      }
      else if (nbthreads > 100) // seems too much...
      {
        di << "Too many threads...";
      }
    }
    if (nbargs >= 9)
    {
      usetriangulation = Draw::Atoi(args[8]) == 0 ? Standard_False : Standard_True;
    }
    if (nbargs >= 10)
    {
      fillInVolume = Draw::Atoi(args[9]);
      if (fillInVolume < 0  || fillInVolume > 2)
      {
        di << "Fill-in volume parameter accepts the values: 0 - no, 1 - yes, 2 - yes wusing shape";
        return 1;
      }
    }
  }

  // Call converter.
  Voxel_BoolDS ds;
  Standard_Boolean ret;
  Standard_Integer progress = 0;
  Voxel_FastConverter converter(S, ds, deflection, nbx, nby, nbz, nbthreads, usetriangulation);

  // Convert.
  if (fast)
    ret = converter.Convert(progress);
  else
    ret = converter.ConvertUsingSAT(progress);

  // Fill-in volume.
  if (ret)
  {
    switch (fillInVolume)
    {
    case 0:
      break;
    case 1:
      converter.FillInVolume(1, nbthreads);
      break;
    case 2:
      converter.FillInVolume(1, S, nbthreads);
      break;
    }
  }

  return ret == Standard_True ? 0 : 1;
}

void  ViewerTest::VoxelCommands(Draw_Interpretor& theCommands)
{
  const char* g = "Voxel draw-commands";
  // Basic commands.
  theCommands.Add("voxelboolds"     ,"voxelboolds [x y z dx dy dz nbx nby nbz]"     ,__FILE__,VOXELBOOLDS,g);
  theCommands.Add("voxelcolords"    ,"voxelcolords [x y z dx dy dz nbx nby nbz]"    ,__FILE__,VOXELCOLORDS,g);
  theCommands.Add("voxelfloatds"    ,"voxelfloatds [x y z dx dy dz nbx nby nbz]"    ,__FILE__,VOXELFLOATDS,g);
  theCommands.Add("voxeloctboolds"  ,"voxeloctboolds [x y z dx dy dz nbx nby nbz]"  ,__FILE__,VOXELOCTBOOLDS,g);
  theCommands.Add("voxelroctboolds" ,"voxelroctboolds [x y z dx dy dz nbx nby nbz]" ,__FILE__,VOXELROCTBOOLDS,g);
  // Boolean operations.
  theCommands.Add("voxelfuseboolds" ,"voxelfuseboolds [x y z dx dy dz nbx nby nbz]" ,__FILE__,VOXELFUSEBOOLDS,g);
  theCommands.Add("voxelfusecolords","voxelfusecolords [x y z dx dy dz nbx nby nbz]",__FILE__,VOXELFUSECOLORDS,g);
  theCommands.Add("voxelfusefloatds","voxelfusefloatds [x y z dx dy dz nbx nby nbz]",__FILE__,VOXELFUSEFLOATDS,g);
  theCommands.Add("voxelcutboolds"  ,"voxelcutboolds [x y z dx dy dz nbx nby nbz]"  ,__FILE__,VOXELCUTBOOLDS,g);
  theCommands.Add("voxelcutcolords" ,"voxelcutcolords [x y z dx dy dz nbx nby nbz]" ,__FILE__,VOXELCUTCOLORDS,g);
  theCommands.Add("voxelcutfloatds" ,"voxelcutfloatds [x y z dx dy dz nbx nby nbz]" ,__FILE__,VOXELCUTFLOATDS,g);
  // Conversion of a shape into voxels.
  theCommands.Add("voxelbooldsconvert",
                  "voxelbooldsconvert shape [nbx nby nbz deflection 0|1(fast or accurate) nbthreads usetriangulation \
                  0|1|2(fill-in volume: no|yes|yes using shape)]",
                  __FILE__,VOXELBOOLDSCONVERT,g);
}
