# Copyright (c) 2013-2014 OPEN CASCADE SAS
#
# This file is part of Open CASCADE Technology software library.
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License version 2.1 as published
# by the Free Software Foundation, with special exception defined in the file
# OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
# distribution for complete text of the license and disclaimer of any warranty.
#
# Alternatively, this file may be used under the terms of Open CASCADE
# commercial license or contractual agreement.

############################################################################
# This file defines scripts for verification of OCCT tests.
# It provides top-level commands starting with 'check'.
# Type 'help check*' to get their synopsys.
# See OCCT Tests User Guide for description of the test system.
#
# Note: procedures with names starting with underscore are for internal use
# inside the test system.
############################################################################

help checkcolor {
  Check pixel color.
  Use: checkcolor x y red green blue
  x y - pixel coordinates
  red green blue - expected pixel color (values from 0 to 1)
  Function check color with tolerance (5x5 area)
}
# Procedure to check color using command vreadpixel with tolerance
proc checkcolor { coord_x coord_y rd_get gr_get bl_get } {
    puts "Coordinate x = $coord_x"
    puts "Coordinate y = $coord_y"
    puts "RED color of RGB is $rd_get"
    puts "GREEN color of RGB is $gr_get"
    puts "BLUE color of RGB is $bl_get"

    if { $coord_x <= 1 || $coord_y <= 1 } {
      puts "Error : minimal coordinate is x = 2, y = 2. But we have x = $coord_x y = $coord_y"
      return -1
    }

    set color ""
    catch { [set color "[vreadpixel ${coord_x} ${coord_y} rgb]"] }
    if {"$color" == ""} {
      puts "Error : Pixel coordinates (${position_x}; ${position_y}) are out of view"
    }
    set rd [lindex $color 0]
    set gr [lindex $color 1]
    set bl [lindex $color 2]
    set rd_int [expr int($rd * 1.e+05)]
    set gr_int [expr int($gr * 1.e+05)]
    set bl_int [expr int($bl * 1.e+05)]
    set rd_ch [expr int($rd_get * 1.e+05)]
    set gr_ch [expr int($gr_get * 1.e+05)]
    set bl_ch [expr int($bl_get * 1.e+05)]

    if { $rd_ch != 0 } {
      set tol_rd [expr abs($rd_ch - $rd_int)/$rd_ch]
    } else {
      set tol_rd $rd_int
    }
    if { $gr_ch != 0 } {
      set tol_gr [expr abs($gr_ch - $gr_int)/$gr_ch]
    } else {
      set tol_gr $gr_int
    }
    if { $bl_ch != 0 } {
      set tol_bl [expr abs($bl_ch - $bl_int)/$bl_ch]
    } else {
      set tol_bl $bl_int
    }

    set status 0
    if { $tol_rd > 0.2 } {
      puts "Warning : RED light of additive color model RGB is invalid"
      set status 1
    }
    if { $tol_gr > 0.2 } {
      puts "Warning : GREEN light of additive color model RGB is invalid"
      set status 1
    }
    if { $tol_bl > 0.2 } {
      puts "Warning : BLUE light of additive color model RGB is invalid"
      set status 1
    }

    if { $status != 0 } {
      puts "Warning : Colors of default coordinate are not equal"
    }

    global stat
    if { $tol_rd > 0.2 || $tol_gr > 0.2 || $tol_bl > 0.2 } {
      set info [_checkpoint $coord_x $coord_y $rd_ch $gr_ch $bl_ch]
      set stat [lindex $info end]
      if { ${stat} != 1 } {
          puts "Error : Colors are not equal in default coordinate and in the near coordinates too"
          return $stat
      } else {
          puts "Point with valid color was found"
          return $stat
      }
    } else {
      set stat 1
    }
}

# Procedure to check color in the point near default coordinate
proc _checkpoint {coord_x coord_y rd_ch gr_ch bl_ch} {
    set x_start [expr ${coord_x} - 2]
    set y_start [expr ${coord_y} - 2]
    set mistake 0
    set i 0
    while { $mistake != 1 && $i <= 5 } {
      set j 0
      while { $mistake != 1 && $j <= 5 } {
          set position_x [expr ${x_start} + $j]
          set position_y [expr ${y_start} + $i]
          puts $position_x
          puts $position_y

          set color ""
          catch { [set color "[vreadpixel ${position_x} ${position_y} rgb]"] }
          if {"$color" == ""} {
            puts "Warning : Pixel coordinates (${position_x}; ${position_y}) are out of view"
            incr j
            continue
          }
          set rd [lindex $color 0]
          set gr [lindex $color 1]
          set bl [lindex $color 2]
          set rd_int [expr int($rd * 1.e+05)]
          set gr_int [expr int($gr * 1.e+05)]
          set bl_int [expr int($bl * 1.e+05)]

          if { $rd_ch != 0 } {
            set tol_rd [expr abs($rd_ch - $rd_int)/$rd_ch]
          } else {
            set tol_rd $rd_int
          }
          if { $gr_ch != 0 } {
            set tol_gr [expr abs($gr_ch - $gr_int)/$gr_ch]
          } else {
            set tol_gr $gr_int
          }
          if { $bl_ch != 0 } {
            set tol_bl [expr abs($bl_ch - $bl_int)/$bl_ch]
          } else {
            set tol_bl $bl_int
          }

          if { $tol_rd > 0.2 || $tol_gr > 0.2 || $tol_bl > 0.2 } {
            puts "Warning : Point with true color was not found near default coordinates"
            set mistake 0
          } else {
            set mistake 1
          }
          incr j
      }
      incr i
    }
    return $mistake
}

# auxiliary: check argument
proc _check_arg {check_name check_result {get_value 0}} {
  upvar ${check_result} ${check_result}
  upvar arg arg
  upvar narg narg
  upvar args args
  if { $arg == ${check_name} } {
    if {${get_value}} {
      incr narg
      if { $narg < [llength $args] && ! [regexp {^-} [lindex $args $narg]] } {
        set ${check_result} "[lindex $args $narg]"
      } else {
        error "Option ${check_result} requires argument"
      }
    } else {
      set ${check_result} 1
    }
    return 1
  }
  return 0
}

help checknbshapes {
  Compare number of sub-shapes in "shape" with given reference data

  Use: checknbshapes shape [options...]
  Allowed options are:
    -vertex N
    -edge N
    -wire N
    -face N
    -shell N
    -solid N
    -compsolid N
    -compound N
    -shape N
    -t: compare the number of sub-shapes in "shape" counting
        the same sub-shapes with different location as different sub-shapes.
    -m msg: print "msg" in case of error
    -ref [nbshapes a]: compare the number of sub-shapes in "shape" and in "a".
                       -vertex N, -edge N and other options are stil working.
}
proc checknbshapes {shape args} {
  puts "checknbshapes ${shape} ${args}"
  upvar ${shape} ${shape}

  set nbVERTEX -1
  set nbEDGE -1
  set nbWIRE -1
  set nbFACE -1
  set nbSHELL -1
  set nbSOLID -1
  set nbCOMPSOLID -1
  set nbCOMPOUND -1
  set nbSHAPE -1

  set message ""
  set count_locations 0
  set ref_info ""

  for {set narg 0} {$narg < [llength $args]} {incr narg} {
    set arg [lindex $args $narg]
    if {[_check_arg "-vertex" nbVERTEX 1] ||
        [_check_arg "-edge" nbEDGE 1] ||
        [_check_arg "-wire" nbWIRE 1] ||
        [_check_arg "-face" nbFACE 1] ||
        [_check_arg "-shell" nbSHELL 1] ||
        [_check_arg "-solid" nbSOLID 1] ||
        [_check_arg "-compsolid" nbCOMPSOLID 1] ||
        [_check_arg "-compound" nbCOMPOUND 1] ||
        [_check_arg "-shape" nbSHAPE 1] ||
        [_check_arg "-t" count_locations] ||
        [_check_arg "-m" message 1] ||
        [_check_arg "-ref" ref_info 1]
       } {
      continue
    }
    # unsupported option
    if { [regexp {^-} $arg] } {
      error "Error: unsupported option \"$arg\""
    }
    error "Error: cannot interpret argument $narg ($arg)"
  }

  if { ${count_locations} == 0 } {
    set nb_info [nbshapes ${shape}]
  } else {
    set nb_info [nbshapes ${shape} -t]
  }

  set EntityList {VERTEX EDGE WIRE FACE SHELL SOLID COMPSOLID COMPOUND SHAPE}

  foreach Entity ${EntityList} {
    set expr_string "${Entity} +: +(\[-0-9.+eE\]+)"
    set to_compare {}
    # get number of elements from ${shape}
    if { [regexp "${expr_string}" ${nb_info} full nb_entity2] } {
      lappend to_compare ${nb_entity2}
    } else {
      error "Error : command \"nbshapes ${shape}\" gives an empty result"
    }
    # get number of elements from options -vertex -edge and so on
    set nb_entity1 [set nb${Entity}]
    if { ${nb_entity1} != -1 } {
      lappend to_compare ${nb_entity1}
    }
    # get number of elements from option -ref
    if { [regexp "${expr_string}" ${ref_info} full nb_entity_ref] } {
      lappend to_compare ${nb_entity_ref}
    }
    # skip comparing if no reference data was given
    if {[llength $to_compare] == 1} {
      continue
    }
    # compare all values, if they are equal, length of sorted list "to_compare"
    # (with key -unique) should be equal to 1
    set to_compare [lsort -dictionary -unique $to_compare]
    if { [llength $to_compare] != 1 } {
      puts "Error : ${message} is WRONG because number of ${Entity} entities in shape \"${shape}\" is ${nb_entity2}"
    } else {
      puts "OK : ${message} is GOOD because number of ${Entity} entities is equal to number of expected ${Entity} entities"
    }
  }
}

# Procedure to check equality of two reals with tolerance (relative and absolute)
help checkreal {
  Compare value with expected
  Use: checkreal name value expected tol_abs tol_rel
}
proc checkreal {name value expected tol_abs tol_rel} {
    if { abs ($value - $expected) > $tol_abs + $tol_rel * abs ($expected) } {
        puts "Error: $name = $value is not equal to expected $expected"
    } else {
        puts "Check of $name OK: value = $value, expected = $expected"
    }
    return
}

help checkfreebounds {
  Compare number of free edges with ref_value

  Use: checkfreebounds shape ref_value [options...]
  Allowed options are:
    -tol N: used tolerance (default -0.01)
    -type N: used type, possible values are "closed" and "opened" (default "closed")
}
proc checkfreebounds {shape ref_value args} {
  puts "checkfreebounds ${shape} ${ref_value} ${args}"
  upvar ${shape} ${shape}

  set tol -0.01
  set type "closed"

  for {set narg 0} {$narg < [llength $args]} {incr narg} {
    set arg [lindex $args $narg]
    if {[_check_arg "-tol" tol 1] ||
        [_check_arg "-type" type 1]
       } {
      continue
    }
    # unsupported option
    if { [regexp {^-} $arg] } {
      error "Error: unsupported option \"$arg\""
    }
    error "Error: cannot interpret argument $narg ($arg)"
  }

  if {"$type" != "closed" && "$type" != "opened"} {
    error "Error : wrong -type key \"${type}\""
  }

  freebounds ${shape} ${tol}
  set free_edges [llength [explode ${shape}_[string range $type 0 0] e]]

  if { ${ref_value} == -1 } {
    puts "Error : Number of free edges is UNSTABLE"
    return
  }

  if { ${free_edges} != ${ref_value} } {
    puts "Error : Number of free edges is not equal to reference data"
  } else {
    puts "OK : Number of free edges is ${free_edges}"
  }
}

help checkmaxtol {
  Compare max tolerance of shape with reference value.
  Command returns max tolerance of the shape.

  Use: checkmaxtol shape [options...]
  Allowed options are:
    -ref: reference value of maximum tolerance.
    -source: list of shapes to compare with, e.g.: -source {shape1 shape2 shape3}
    -min_tol: minimum tolerance for comparison.
    -multi_tol: tolerance multiplier.
}

proc checkmaxtol {shape args} {
  puts "checkmaxtol ${shape} ${args}"
  upvar ${shape} ${shape}

  set ref_value ""
  set source_shapes {}
  set min_tol 0
  set tol_multiplier 0

  # check arguments
  for {set narg 0} {$narg < [llength $args]} {incr narg} {
    set arg [lindex $args $narg]
    if {[_check_arg "-min_tol" min_tol 1] ||
        [_check_arg "-multi_tol" tol_multiplier 1] ||
        [_check_arg "-source" source_shapes 1] ||
        [_check_arg "-ref" ref_value 1]
       } {
      continue
    }
    # unsupported option
    if { [regexp {^-} $arg] } {
      error "Error: unsupported option \"$arg\""
    }
    error "Error: cannot interpret argument $narg ($arg)"
  }

  # get max tol of shape
  set max_tol 0
  if {[regexp "Tolerance MAX=(\[-0-9.+eE\]+)" [tolerance ${shape}] full maxtol_temp]} {
    set max_tol ${maxtol_temp}
  } else {
    error "Error: cannot get tolerances of shape \"${shape}\""
  }

  # find max tol of source shapes
  foreach source_shape ${source_shapes} {
    upvar ${source_shape} ${source_shape}
    set _src_max_tol [checkmaxtol ${source_shape}]
    if { [expr ${_src_max_tol} > ${min_tol} ] } {
      set min_tol ${_src_max_tol}
    }
  }
  # apply -multi_tol option
  if {${tol_multiplier}} {
    set min_tol [expr ${tol_multiplier} * ${_src_max_tol}]
  }
  # compare max tol of source shapes with checking tolerance
  if { ${min_tol} && [expr ${max_tol} > ${min_tol}] } {
    puts "Error: tolerance of \"${shape}\" (${max_tol}) is greater than checking tolerance (${min_tol})"
  }
  if { ${ref_value} != "" } {
    checkreal "Max tolerance" ${max_tol} ${ref_value} 0.0001 0.01
  }
  return ${max_tol}
}

help checkfaults {
  Compare faults number of given shapes.

  Use: checkfaults shape source_shape [ref_value=0]
}
proc checkfaults {shape source_shape {ref_value 0}} {
  puts "checkfaults ${shape} ${source_shape} ${ref_value}"
  upvar $shape $shape
  upvar $source_shape $source_shape
  set cs_a [checkshape $source_shape]
  set nb_a 0
  if {[regexp {Faulty shapes in variables faulty_([0-9]*) to faulty_([0-9]*)} $cs_a full nb_a_begin nb_a_end]} {
    set nb_a [expr $nb_a_end - $nb_a_begin +1]
  }
  set cs_r [checkshape $shape]
  set nb_r 0
  if {[regexp {Faulty shapes in variables faulty_([0-9]*) to faulty_([0-9]*)} $cs_r full nb_r_begin nb_r_end]} {
    set nb_r [expr $nb_r_end - $nb_r_begin +1]
  }
  puts "Number of faults for the initial shape is $nb_a."
  puts "Number of faults for the resulting shape is $nb_r."

  if { ${ref_value} == -1 } {
    puts "Error : Number of faults is UNSTABLE"
    return
  }

  if { $nb_r > $nb_a } {
    puts "Error : Number of faults is $nb_r"
  }
}
