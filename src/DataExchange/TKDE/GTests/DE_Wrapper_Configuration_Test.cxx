// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <gtest/gtest.h>

#include <DEBREP_ConfigurationNode.hxx>
#include <DE_ConfigurationContext.hxx>
#include <DE_ConfigurationNode.hxx>
#include <DEGLTF_ConfigurationNode.hxx>
#include <DEIGES_ConfigurationNode.hxx>
#include <DEOBJ_ConfigurationNode.hxx>
#include <DEPLY_ConfigurationNode.hxx>
#include <DESTEP_ConfigurationNode.hxx>
#include <DESTL_ConfigurationNode.hxx>
#include <DEVRML_ConfigurationNode.hxx>
#include <DEXCAF_ConfigurationNode.hxx>
#include <DE_Wrapper.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_List.hxx>
#include <TCollection_AsciiString.hxx>

namespace
{
static occ::handle<DE_Wrapper> NewWrapper()
{
  occ::handle<DE_Wrapper> aWrapper = new DE_Wrapper();
  EXPECT_TRUE(aWrapper->Bind(new DEVRML_ConfigurationNode()));
  EXPECT_TRUE(aWrapper->Bind(new DEIGES_ConfigurationNode()));
  EXPECT_TRUE(aWrapper->Bind(new DEOBJ_ConfigurationNode()));
  EXPECT_TRUE(aWrapper->Bind(new DEGLTF_ConfigurationNode()));
  EXPECT_TRUE(aWrapper->Bind(new DEPLY_ConfigurationNode()));
  EXPECT_TRUE(aWrapper->Bind(new DEXCAF_ConfigurationNode()));
  EXPECT_TRUE(aWrapper->Bind(new DESTEP_ConfigurationNode()));
  EXPECT_TRUE(aWrapper->Bind(new DESTL_ConfigurationNode()));
  EXPECT_TRUE(aWrapper->Bind(new DEBREP_ConfigurationNode()));
  return aWrapper;
}

static bool SameConfiguration(const TCollection_AsciiString& theFirst,
                              const TCollection_AsciiString& theSecond)
{
  occ::handle<DE_ConfigurationContext> aFirstContext  = new DE_ConfigurationContext();
  occ::handle<DE_ConfigurationContext> aSecondContext = new DE_ConfigurationContext();
  if (!aFirstContext->Load(theFirst) || !aSecondContext->Load(theSecond))
  {
    return false;
  }

  const NCollection_DataMap<TCollection_AsciiString, TCollection_AsciiString>& aFirstMap =
    aFirstContext->GetInternalMap();
  const NCollection_DataMap<TCollection_AsciiString, TCollection_AsciiString>& aSecondMap =
    aSecondContext->GetInternalMap();
  if (aFirstMap.Extent() != aSecondMap.Extent())
  {
    return false;
  }

  for (NCollection_DataMap<TCollection_AsciiString, TCollection_AsciiString>::Iterator anIterator(
         aFirstMap);
       anIterator.More();
       anIterator.Next())
  {
    TCollection_AsciiString aValue;
    if (!aSecondMap.Find(anIterator.Key(), aValue) || !aValue.IsEqual(anIterator.Value()))
    {
      return false;
    }
  }
  return true;
}

static TCollection_AsciiString ChangeProviderValues(const TCollection_AsciiString& theConfiguration,
                                                    const char*                    theValue)
{
  TCollection_AsciiString aResult;
  TCollection_AsciiString aLine;
  for (int anIndex = 1; anIndex <= theConfiguration.Length(); ++anIndex)
  {
    const char aCharacter = theConfiguration.Value(anIndex);
    if (aCharacter != '\n')
    {
      aLine += aCharacter;
    }

    if (aCharacter == '\n' || anIndex == theConfiguration.Length())
    {
      TCollection_AsciiString aTrimmedLine(aLine);
      aTrimmedLine.LeftAdjust();
      if (aTrimmedLine.StartsWith("provider."))
      {
        const int aColon = aLine.Location(1, ':', 1, aLine.Length());
        if (aColon > 0)
        {
          aResult += aLine.SubString(1, aColon);
          aResult += " ";
          aResult += theValue;
          aResult += " \t\n";
        }
      }
      else if (aTrimmedLine.StartsWith("global."))
      {
        aResult += aLine;
        aResult += "\n";
      }
      aLine.Clear();
    }
  }
  return aResult;
}

static const char* InitialConfiguration()
{
  return R"OCCT_A3(
global.priority.VRML :	 OCC
global.priority.IGES :	 OCC
global.priority.OBJ :	 OCC
global.priority.GLTF :	 OCC
global.priority.PLY :	 OCC
global.priority.XCAF :	 OCC
global.priority.STEP :	 OCC
global.priority.STL :	 OCC
global.priority.BREP :	 OCC
global.general.length.unit :	 1
global.general.system.unit :	 1
provider.VRML.OCC.read.file.unit :	 1
provider.VRML.OCC.read.file.coordinate.system :	 1
provider.VRML.OCC.read.system.coordinate.system :	 0
provider.VRML.OCC.read.fill.incomplete :	 1
provider.VRML.OCC.writer.version :	 2
provider.VRML.OCC.write.representation.type :	 1
provider.IGES.OCC.read.iges.bspline.continuity :	 1
provider.IGES.OCC.read.precision.mode :	 0
provider.IGES.OCC.read.precision.val :	 0.0001
provider.IGES.OCC.read.maxprecision.mode :	 0
provider.IGES.OCC.read.maxprecision.val :	 1
provider.IGES.OCC.read.stdsameparameter.mode :	 0
provider.IGES.OCC.read.surfacecurve.mode :	 0
provider.IGES.OCC.read.encoderegularity.angle :	 0.572958
provider.IGES.OCC.read.bspline.approxd1.mode :	 0
provider.IGES.OCC.read.fau_lty.entities :	 0
provider.IGES.OCC.read.onlyvisible :	 0
provider.IGES.OCC.read.color :	 1
provider.IGES.OCC.read.name :	 1
provider.IGES.OCC.read.layer :	 1
provider.IGES.OCC.write.brep.mode :	 0
provider.IGES.OCC.write.convertsurface.mode :	 0
provider.IGES.OCC.write.header.author :
provider.IGES.OCC.write.header.company :
provider.IGES.OCC.write.header.product :
provider.IGES.OCC.write.header.receiver :
provider.IGES.OCC.write.precision.mode :	 0
provider.IGES.OCC.write.precision.val :	 0.0001
provider.IGES.OCC.write.plane.mode :	 0
provider.IGES.OCC.write.offset :	 0
provider.IGES.OCC.write.color :	 1
provider.IGES.OCC.write.name :	 1
provider.IGES.OCC.write.layer :	 1
provider.IGES.OCC.healing.tolerance3d :	 1e-06
provider.IGES.OCC.healing.max.tolerance3d :	 1
provider.IGES.OCC.healing.min.tolerance3d :	 1e-07
provider.IGES.OCC.healing.free.shell :	 -1
provider.IGES.OCC.healing.free.face :	 -1
provider.IGES.OCC.healing.free.wire :	 -1
provider.IGES.OCC.healing.same.parameter :	 -1
provider.IGES.OCC.healing.solid :	 -1
provider.IGES.OCC.healing.shell.orientation :	 -1
provider.IGES.OCC.healing.create.open.solid :	 1
provider.IGES.OCC.healing.shell :	 -1
provider.IGES.OCC.healing.face.orientation :	 -1
provider.IGES.OCC.healing.face :	 -1
provider.IGES.OCC.healing.wire :	 -1
provider.IGES.OCC.healing.orientation :	 -1
provider.IGES.OCC.healing.add.natural.bound :	 -1
provider.IGES.OCC.healing.missing.seam :	 -1
provider.IGES.OCC.healing.small.area.wire :	 -1
provider.IGES.OCC.healing.remove.small.area.face :	 -1
provider.IGES.OCC.healing.intersecting.wires :	 -1
provider.IGES.OCC.healing.loop.wires :	 -1
provider.IGES.OCC.healing.split.face :	 -1
provider.IGES.OCC.healing.auto.correct.precision :	 1
provider.IGES.OCC.healing.modify.topology :	 0
provider.IGES.OCC.healing.modify.geometry :	 1
provider.IGES.OCC.healing.closed.wire :	 1
provider.IGES.OCC.healing.preference.pcurve :	 1
provider.IGES.OCC.healing.reorder.edges :	 -1
provider.IGES.OCC.healing.remove.small.edges :	 -1
provider.IGES.OCC.healing.connected.edges :	 -1
provider.IGES.OCC.healing.edge.curves :	 -1
provider.IGES.OCC.healing.add.degenerated.edges :	 -1
provider.IGES.OCC.healing.add.lacking.edges :	 -1
provider.IGES.OCC.healing.selfintersection :	 -1
provider.IGES.OCC.healing.remove.loop :	 -1
provider.IGES.OCC.healing.reversed2d :	 -1
provider.IGES.OCC.healing.remove.pcurve :	 -1
provider.IGES.OCC.healing.remove.curve3d :	 -1
provider.IGES.OCC.healing.add.pcurve :	 -1
provider.IGES.OCC.healing.add.curve3d :	 -1
provider.IGES.OCC.healing.correct.order.in.seam :	 -1
provider.IGES.OCC.healing.shifted :	 -1
provider.IGES.OCC.healing.edge.same.parameter :	 0
provider.IGES.OCC.healing.notched.edges :	 -1
provider.IGES.OCC.healing.tail :	 -1
provider.IGES.OCC.healing.max.tail.angle :	 -1
provider.IGES.OCC.healing.max.tail.width :	 -1
provider.IGES.OCC.healing.selfintersecting.edge :	 -1
provider.IGES.OCC.healing.intersecting.edges :	 -1
provider.IGES.OCC.healing.nonadjacent.intersecting.edges :	 -1
provider.IGES.OCC.healing.vertex.position :	 0
provider.IGES.OCC.healing.vertex.tolerance :	 -1
provider.OBJ.OCC.file.length.unit :	 1
provider.OBJ.OCC.system.cs :	 0
provider.OBJ.OCC.file.cs :	 1
provider.OBJ.OCC.read.single.precision :	 0
provider.OBJ.OCC.read.create.shapes :	 0
provider.OBJ.OCC.read.root.prefix :
provider.OBJ.OCC.read.fill.doc :	 1
provider.OBJ.OCC.read.fill.incomplete :	 1
provider.OBJ.OCC.read.memory.limit.mib :	 -1
provider.OBJ.OCC.write.comment :
provider.OBJ.OCC.write.author :
provider.GLTF.OCC.file.length.unit :	 1
provider.GLTF.OCC.system.cs :	 0
provider.GLTF.OCC.file.cs :	 1
provider.GLTF.OCC.read.single.precision :	 1
provider.GLTF.OCC.read.create.shapes :	 0
provider.GLTF.OCC.read.root.prefix :
provider.GLTF.OCC.read.fill.doc :	 1
provider.GLTF.OCC.read.fill.incomplete :	 1
provider.GLTF.OCC.read.memory.limit.mib :	 -1
provider.GLTF.OCC.read.parallel :	 0
provider.GLTF.OCC.read.skip.empty.nodes :	 1
provider.GLTF.OCC.read.load.all.scenes :	 0
provider.GLTF.OCC.read.use.mesh.name.as.fallback :	 1
provider.GLTF.OCC.read.skip.late.data.loading :	 0
provider.GLTF.OCC.read.keep.late.data :	 1
provider.GLTF.OCC.read.print.debug.message :	 0
provider.GLTF.OCC.read.apply.scale :	 1
provider.GLTF.OCC.write.comment :
provider.GLTF.OCC.write.author :
provider.GLTF.OCC.write.trsf.format :	 0
provider.GLTF.OCC.write.node.name.format :	 3
provider.GLTF.OCC.write.mesh.name.format :	 1
provider.GLTF.OCC.write.forced.uv.export :	 0
provider.GLTF.OCC.write.embed.textures.in.glb :	 1
provider.GLTF.OCC.write.merge.faces :	 0
provider.GLTF.OCC.write.split.indices16 :	 0
provider.PLY.OCC.file.length.unit :	 1
provider.PLY.OCC.system.cs :	 0
provider.PLY.OCC.file.cs :	 1
provider.PLY.OCC.write.normals :	 1
provider.PLY.OCC.write.colors :	 1
provider.PLY.OCC.write.tex.coords :	 0
provider.PLY.OCC.write.part.id :	 1
provider.PLY.OCC.write.face.id :	 0
provider.PLY.OCC.write.comment :
provider.PLY.OCC.write.author :
provider.XCAF.OCC.read.append.mode :	 0
provider.XCAF.OCC.read.skip.values :
provider.XCAF.OCC.read.values :
provider.STEP.OCC.read.iges.bspline.continuity :	 1
provider.STEP.OCC.read.precision.mode :	 0
provider.STEP.OCC.read.precision.val :	 0.0001
provider.STEP.OCC.read.maxprecision.mode :	 0
provider.STEP.OCC.read.maxprecision.val :	 1
provider.STEP.OCC.read.stdsameparameter.mode :	 0
provider.STEP.OCC.read.surfacecurve.mode :	 0
provider.STEP.OCC.read.encoderegularity.angle :	 0.572958
provider.STEP.OCC.angleunit.mode :	 0
provider.STEP.OCC.read.product.mode :	 1
provider.STEP.OCC.read.product.context :	 1
provider.STEP.OCC.read.shape.repr :	 1
provider.STEP.OCC.read.tessellated :	 1
provider.STEP.OCC.read.assembly.level :	 1
provider.STEP.OCC.read.shape.relationship :	 1
provider.STEP.OCC.read.shape.aspect :	 1
provider.STEP.OCC.read.constructivegeom.relationship :	 0
provider.STEP.OCC.read.stepcaf.subshapes.name :	 0
provider.STEP.OCC.read.codepage :	 4
provider.STEP.OCC.read.nonmanifold :	 0
provider.STEP.OCC.read.ideas :	 0
provider.STEP.OCC.read.all.shapes :	 0
provider.STEP.OCC.read.root.transformation :	 1
provider.STEP.OCC.read.color :	 1
provider.STEP.OCC.read.name :	 1
provider.STEP.OCC.read.layer :	 1
provider.STEP.OCC.read.props :	 1
provider.STEP.OCC.read.metadata :	 1
provider.STEP.OCC.read.productmetadata :	 0
provider.STEP.OCC.write.precision.mode :	 0
provider.STEP.OCC.write.precision.val :	 0.0001
provider.STEP.OCC.write.assembly :	 2
provider.STEP.OCC.write.schema :	 4
provider.STEP.OCC.write.tessellated :	 2
provider.STEP.OCC.write.product.name :
provider.STEP.OCC.write.surfacecurve.mode :	 1
provider.STEP.OCC.write.unit :	 2
provider.STEP.OCC.write.vertex.mode :	 0
provider.STEP.OCC.write.stepcaf.subshapes.name :	 0
provider.STEP.OCC.write.color :	 1
provider.STEP.OCC.write.nonmanifold :	 0
provider.STEP.OCC.write.name :	 1
provider.STEP.OCC.write.layer :	 1
provider.STEP.OCC.write.material :	 1
provider.STEP.OCC.write.vismaterial :	 0
provider.STEP.OCC.write.metadata :  1
provider.STEP.OCC.write.props :	 1
provider.STEP.OCC.write.model.type :	 0
provider.STEP.OCC.write.cleanduplicates : 0
provider.STEP.OCC.write.scaling.trsf : 1
provider.STEP.OCC.healing.tolerance3d :	 1e-06
provider.STEP.OCC.healing.max.tolerance3d :	 1
provider.STEP.OCC.healing.min.tolerance3d :	 1e-07
provider.STEP.OCC.healing.free.shell :	 -1
provider.STEP.OCC.healing.free.face :	 -1
provider.STEP.OCC.healing.free.wire :	 -1
provider.STEP.OCC.healing.same.parameter :	 -1
provider.STEP.OCC.healing.solid :	 -1
provider.STEP.OCC.healing.shell.orientation :	 -1
provider.STEP.OCC.healing.create.open.solid :	 0
provider.STEP.OCC.healing.shell :	 -1
provider.STEP.OCC.healing.face.orientation :	 -1
provider.STEP.OCC.healing.face :	 -1
provider.STEP.OCC.healing.wire :	 -1
provider.STEP.OCC.healing.orientation :	 -1
provider.STEP.OCC.healing.add.natural.bound :	 -1
provider.STEP.OCC.healing.missing.seam :	 -1
provider.STEP.OCC.healing.small.area.wire :	 -1
provider.STEP.OCC.healing.remove.small.area.face :	 -1
provider.STEP.OCC.healing.intersecting.wires :	 -1
provider.STEP.OCC.healing.loop.wires :	 -1
provider.STEP.OCC.healing.split.face :	 -1
provider.STEP.OCC.healing.auto.correct.precision :	 1
provider.STEP.OCC.healing.modify.topology :	 0
provider.STEP.OCC.healing.modify.geometry :	 1
provider.STEP.OCC.healing.closed.wire :	 1
provider.STEP.OCC.healing.preference.pcurve :	 1
provider.STEP.OCC.healing.reorder.edges :	 -1
provider.STEP.OCC.healing.remove.small.edges :	 -1
provider.STEP.OCC.healing.connected.edges :	 -1
provider.STEP.OCC.healing.edge.curves :	 -1
provider.STEP.OCC.healing.add.degenerated.edges :	 -1
provider.STEP.OCC.healing.add.lacking.edges :	 -1
provider.STEP.OCC.healing.selfintersection :	 -1
provider.STEP.OCC.healing.remove.loop :	 -1
provider.STEP.OCC.healing.reversed2d :	 -1
provider.STEP.OCC.healing.remove.pcurve :	 -1
provider.STEP.OCC.healing.remove.curve3d :	 -1
provider.STEP.OCC.healing.add.pcurve :	 -1
provider.STEP.OCC.healing.add.curve3d :	 -1
provider.STEP.OCC.healing.correct.order.in.seam :	 -1
provider.STEP.OCC.healing.shifted :	 -1
provider.STEP.OCC.healing.edge.same.parameter :	 0
provider.STEP.OCC.healing.notched.edges :	 -1
provider.STEP.OCC.healing.tail :	 0
provider.STEP.OCC.healing.max.tail.angle :	 0
provider.STEP.OCC.healing.max.tail.width :	 -1
provider.STEP.OCC.healing.selfintersecting.edge :	 -1
provider.STEP.OCC.healing.intersecting.edges :	 -1
provider.STEP.OCC.healing.nonadjacent.intersecting.edges :	 -1
provider.STEP.OCC.healing.vertex.position :	 0
provider.STEP.OCC.healing.vertex.tolerance :	 -1
provider.STL.OCC.read.merge.angle :	 90
provider.STL.OCC.read.brep :	 0
provider.STL.OCC.write.ascii :	 1
provider.BREP.OCC.write.binary :	 1
provider.BREP.OCC.write.version.binary :	 4
provider.BREP.OCC.write.version.ascii :	 3
provider.BREP.OCC.write.triangles :	 1
provider.BREP.OCC.write.normals :	 1

)OCCT_A3";
}

static const char* TwoFormatsConfiguration()
{
  return R"OCCT_A4(
global.priority.VRML :	 OCC
global.priority.IGES :	 OCC
global.priority.OBJ :	 OCC
global.priority.GLTF :	 OCC
global.priority.PLY :	 OCC
global.priority.XCAF :	 OCC
global.priority.STEP :	 OCC
global.priority.STL :	 OCC
global.priority.BREP :	 OCC
global.general.length.unit :	 1
global.general.system.unit :	 1
provider.IGES.OCC.read.iges.bspline.continuity :	 1
provider.IGES.OCC.read.precision.mode :	 0
provider.IGES.OCC.read.precision.val :	 0.0001
provider.IGES.OCC.read.maxprecision.mode :	 0
provider.IGES.OCC.read.maxprecision.val :	 1
provider.IGES.OCC.read.stdsameparameter.mode :	 0
provider.IGES.OCC.read.surfacecurve.mode :	 0
provider.IGES.OCC.read.encoderegularity.angle :	 0.572958
provider.IGES.OCC.read.bspline.approxd1.mode :	 0
provider.IGES.OCC.read.fau_lty.entities :	 0
provider.IGES.OCC.read.onlyvisible :	 0
provider.IGES.OCC.read.color :	 1
provider.IGES.OCC.read.name :	 1
provider.IGES.OCC.read.layer :	 1
provider.IGES.OCC.write.brep.mode :	 0
provider.IGES.OCC.write.convertsurface.mode :	 0
provider.IGES.OCC.write.header.author :
provider.IGES.OCC.write.header.company :
provider.IGES.OCC.write.header.product :
provider.IGES.OCC.write.header.receiver :
provider.IGES.OCC.write.precision.mode :	 0
provider.IGES.OCC.write.precision.val :	 0.0001
provider.IGES.OCC.write.plane.mode :	 0
provider.IGES.OCC.write.offset :	 0
provider.IGES.OCC.write.color :	 1
provider.IGES.OCC.write.name :	 1
provider.IGES.OCC.write.layer :	 1
provider.IGES.OCC.healing.tolerance3d :	 1e-06
provider.IGES.OCC.healing.max.tolerance3d :	 1
provider.IGES.OCC.healing.min.tolerance3d :	 1e-07
provider.IGES.OCC.healing.free.shell :	 -1
provider.IGES.OCC.healing.free.face :	 -1
provider.IGES.OCC.healing.free.wire :	 -1
provider.IGES.OCC.healing.same.parameter :	 -1
provider.IGES.OCC.healing.solid :	 -1
provider.IGES.OCC.healing.shell.orientation :	 -1
provider.IGES.OCC.healing.create.open.solid :	 1
provider.IGES.OCC.healing.shell :	 -1
provider.IGES.OCC.healing.face.orientation :	 -1
provider.IGES.OCC.healing.face :	 -1
provider.IGES.OCC.healing.wire :	 -1
provider.IGES.OCC.healing.orientation :	 -1
provider.IGES.OCC.healing.add.natural.bound :	 -1
provider.IGES.OCC.healing.missing.seam :	 -1
provider.IGES.OCC.healing.small.area.wire :	 -1
provider.IGES.OCC.healing.remove.small.area.face :	 -1
provider.IGES.OCC.healing.intersecting.wires :	 -1
provider.IGES.OCC.healing.loop.wires :	 -1
provider.IGES.OCC.healing.split.face :	 -1
provider.IGES.OCC.healing.auto.correct.precision :	 1
provider.IGES.OCC.healing.modify.topology :	 0
provider.IGES.OCC.healing.modify.geometry :	 1
provider.IGES.OCC.healing.closed.wire :	 1
provider.IGES.OCC.healing.preference.pcurve :	 1
provider.IGES.OCC.healing.reorder.edges :	 -1
provider.IGES.OCC.healing.remove.small.edges :	 -1
provider.IGES.OCC.healing.connected.edges :	 -1
provider.IGES.OCC.healing.edge.curves :	 -1
provider.IGES.OCC.healing.add.degenerated.edges :	 -1
provider.IGES.OCC.healing.add.lacking.edges :	 -1
provider.IGES.OCC.healing.selfintersection :	 -1
provider.IGES.OCC.healing.remove.loop :	 -1
provider.IGES.OCC.healing.reversed2d :	 -1
provider.IGES.OCC.healing.remove.pcurve :	 -1
provider.IGES.OCC.healing.remove.curve3d :	 -1
provider.IGES.OCC.healing.add.pcurve :	 -1
provider.IGES.OCC.healing.add.curve3d :	 -1
provider.IGES.OCC.healing.correct.order.in.seam :	 -1
provider.IGES.OCC.healing.shifted :	 -1
provider.IGES.OCC.healing.edge.same.parameter :	 0
provider.IGES.OCC.healing.notched.edges :	 -1
provider.IGES.OCC.healing.tail :	 -1
provider.IGES.OCC.healing.max.tail.angle :	 -1
provider.IGES.OCC.healing.max.tail.width :	 -1
provider.IGES.OCC.healing.selfintersecting.edge :	 -1
provider.IGES.OCC.healing.intersecting.edges :	 -1
provider.IGES.OCC.healing.nonadjacent.intersecting.edges :	 -1
provider.IGES.OCC.healing.vertex.position :	 0
provider.IGES.OCC.healing.vertex.tolerance :	 -1
provider.STEP.OCC.read.iges.bspline.continuity :	 1
provider.STEP.OCC.read.precision.mode :	 0
provider.STEP.OCC.read.precision.val :	 0.0001
provider.STEP.OCC.read.maxprecision.mode :	 0
provider.STEP.OCC.read.maxprecision.val :	 1
provider.STEP.OCC.read.stdsameparameter.mode :	 0
provider.STEP.OCC.read.surfacecurve.mode :	 0
provider.STEP.OCC.read.encoderegularity.angle :	 0.572958
provider.STEP.OCC.angleunit.mode :	 0
provider.STEP.OCC.read.product.mode :	 1
provider.STEP.OCC.read.product.context :	 1
provider.STEP.OCC.read.shape.repr :	 1
provider.STEP.OCC.read.tessellated :	 1
provider.STEP.OCC.read.assembly.level :	 1
provider.STEP.OCC.read.shape.relationship :	 1
provider.STEP.OCC.read.shape.aspect :	 1
provider.STEP.OCC.read.constructivegeom.relationship :	 0
provider.STEP.OCC.read.stepcaf.subshapes.name :	 0
provider.STEP.OCC.read.codepage :	 4
provider.STEP.OCC.read.nonmanifold :	 0
provider.STEP.OCC.read.ideas :	 0
provider.STEP.OCC.read.all.shapes :	 0
provider.STEP.OCC.read.root.transformation :	 1
provider.STEP.OCC.read.color :	 1
provider.STEP.OCC.read.name :	 1
provider.STEP.OCC.read.layer :	 1
provider.STEP.OCC.read.props :	 1
provider.STEP.OCC.read.metadata :	 1
provider.STEP.OCC.read.productmetadata :	 0
provider.STEP.OCC.write.precision.mode :	 0
provider.STEP.OCC.write.precision.val :	 0.0001
provider.STEP.OCC.write.assembly :	 2
provider.STEP.OCC.write.schema :	 4
provider.STEP.OCC.write.tessellated :	 2
provider.STEP.OCC.write.product.name :
provider.STEP.OCC.write.surfacecurve.mode :	 1
provider.STEP.OCC.write.unit :	 2
provider.STEP.OCC.write.vertex.mode :	 0
provider.STEP.OCC.write.stepcaf.subshapes.name :	 0
provider.STEP.OCC.write.color :	 1
provider.STEP.OCC.write.nonmanifold :	 0
provider.STEP.OCC.write.name :	 1
provider.STEP.OCC.write.layer :	 1
provider.STEP.OCC.write.props :	 1
provider.STEP.OCC.write.material :	 1
provider.STEP.OCC.write.vismaterial :	 0
provider.STEP.OCC.write.metadata :  1
provider.STEP.OCC.write.model.type :	 0
provider.STEP.OCC.write.cleanduplicates : 0
provider.STEP.OCC.write.scaling.trsf : 1
provider.STEP.OCC.healing.tolerance3d :	 1e-06
provider.STEP.OCC.healing.max.tolerance3d :	 1
provider.STEP.OCC.healing.min.tolerance3d :	 1e-07
provider.STEP.OCC.healing.free.shell :	 -1
provider.STEP.OCC.healing.free.face :	 -1
provider.STEP.OCC.healing.free.wire :	 -1
provider.STEP.OCC.healing.same.parameter :	 -1
provider.STEP.OCC.healing.solid :	 -1
provider.STEP.OCC.healing.shell.orientation :	 -1
provider.STEP.OCC.healing.create.open.solid :	 0
provider.STEP.OCC.healing.shell :	 -1
provider.STEP.OCC.healing.face.orientation :	 -1
provider.STEP.OCC.healing.face :	 -1
provider.STEP.OCC.healing.wire :	 -1
provider.STEP.OCC.healing.orientation :	 -1
provider.STEP.OCC.healing.add.natural.bound :	 -1
provider.STEP.OCC.healing.missing.seam :	 -1
provider.STEP.OCC.healing.small.area.wire :	 -1
provider.STEP.OCC.healing.remove.small.area.face :	 -1
provider.STEP.OCC.healing.intersecting.wires :	 -1
provider.STEP.OCC.healing.loop.wires :	 -1
provider.STEP.OCC.healing.split.face :	 -1
provider.STEP.OCC.healing.auto.correct.precision :	 1
provider.STEP.OCC.healing.modify.topology :	 0
provider.STEP.OCC.healing.modify.geometry :	 1
provider.STEP.OCC.healing.closed.wire :	 1
provider.STEP.OCC.healing.preference.pcurve :	 1
provider.STEP.OCC.healing.reorder.edges :	 -1
provider.STEP.OCC.healing.remove.small.edges :	 -1
provider.STEP.OCC.healing.connected.edges :	 -1
provider.STEP.OCC.healing.edge.curves :	 -1
provider.STEP.OCC.healing.add.degenerated.edges :	 -1
provider.STEP.OCC.healing.add.lacking.edges :	 -1
provider.STEP.OCC.healing.selfintersection :	 -1
provider.STEP.OCC.healing.remove.loop :	 -1
provider.STEP.OCC.healing.reversed2d :	 -1
provider.STEP.OCC.healing.remove.pcurve :	 -1
provider.STEP.OCC.healing.remove.curve3d :	 -1
provider.STEP.OCC.healing.add.pcurve :	 -1
provider.STEP.OCC.healing.add.curve3d :	 -1
provider.STEP.OCC.healing.correct.order.in.seam :	 -1
provider.STEP.OCC.healing.shifted :	 -1
provider.STEP.OCC.healing.edge.same.parameter :	 0
provider.STEP.OCC.healing.notched.edges :	 -1
provider.STEP.OCC.healing.tail :	 0
provider.STEP.OCC.healing.max.tail.angle :	 0
provider.STEP.OCC.healing.max.tail.width :	 -1
provider.STEP.OCC.healing.selfintersecting.edge :	 -1
provider.STEP.OCC.healing.intersecting.edges :	 -1
provider.STEP.OCC.healing.nonadjacent.intersecting.edges :	 -1
provider.STEP.OCC.healing.vertex.position :	 0
provider.STEP.OCC.healing.vertex.tolerance :	 -1
)OCCT_A4";
}

static void ExpectConfiguration(const TCollection_AsciiString& theActual,
                                const TCollection_AsciiString& theExpected)
{
  EXPECT_TRUE(SameConfiguration(theExpected, theActual));
}
} // namespace

// de_wrapper/configuration/A1: provider values can be changed to one and saved stably.
TEST(DE_Wrapper_Configuration_Test, DeWrapperConfiguration_A1_ChangeToOne)
{
  occ::handle<DE_Wrapper>       aWrapper           = NewWrapper();
  const TCollection_AsciiString anOldConfiguration = aWrapper->Save();
  const TCollection_AsciiString aChangedConfiguration =
    ChangeProviderValues(anOldConfiguration, "1");
  ASSERT_TRUE(aWrapper->Load(aChangedConfiguration));
  ExpectConfiguration(aChangedConfiguration, aWrapper->Save());
}

// de_wrapper/configuration/A2: provider values can be changed to zero and saved stably.
TEST(DE_Wrapper_Configuration_Test, DeWrapperConfiguration_A2_ChangeToZero)
{
  occ::handle<DE_Wrapper>       aWrapper           = NewWrapper();
  const TCollection_AsciiString anOldConfiguration = aWrapper->Save();
  const TCollection_AsciiString aChangedConfiguration =
    ChangeProviderValues(anOldConfiguration, "0");
  ASSERT_TRUE(aWrapper->Load(aChangedConfiguration));
  ExpectConfiguration(aChangedConfiguration, aWrapper->Save());
}

// de_wrapper/configuration/A3: the initial configuration has the expected resource values.
TEST(DE_Wrapper_Configuration_Test, DeWrapperConfiguration_A3_InitialConfiguration)
{
  occ::handle<DE_Wrapper> aWrapper = NewWrapper();
  ExpectConfiguration(aWrapper->Save(), TCollection_AsciiString(InitialConfiguration()));
}

// de_wrapper/configuration/A4: filtering the saved configuration to STEP and IGES is stable.
TEST(DE_Wrapper_Configuration_Test, DeWrapperConfiguration_A4_TwoFormats)
{
  occ::handle<DE_Wrapper>                   aWrapper = NewWrapper();
  NCollection_List<TCollection_AsciiString> aFormats;
  aFormats.Append("STEP");
  aFormats.Append("IGES");
  ExpectConfiguration(aWrapper->Save(true, aFormats),
                      TCollection_AsciiString(TwoFormatsConfiguration()));
}

// de_wrapper/configuration/A5: loading an empty configuration preserves defaults.
TEST(DE_Wrapper_Configuration_Test, DeWrapperConfiguration_A5_EmptyLoad)
{
  occ::handle<DE_Wrapper>       aWrapper           = NewWrapper();
  const TCollection_AsciiString anOldConfiguration = aWrapper->Save();
  ASSERT_TRUE(aWrapper->Load(""));
  ExpectConfiguration(aWrapper->Save(), anOldConfiguration);
}
