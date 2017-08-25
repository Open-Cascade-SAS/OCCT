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
    if { ${get_value} == "?" } {
      set next_arg_index [expr $narg + 1]
      if { $next_arg_index < [llength $args] && ! [regexp {^-[^0-9]} [lindex $args $next_arg_index]] } {
        set ${check_result} "[lindex $args $next_arg_index]"
        set narg ${next_arg_index}
      } else {
        set ${check_result} "true"
      }
    } elseif {${get_value}} {
      incr narg
      if { $narg < [llength $args] && ! [regexp {^-[^0-9]} [lindex $args $narg]] } {
        set ${check_result} "[lindex $args $narg]"
      } else {
        error "Option ${check_result} requires argument"
      }
    } else {
      set ${check_result} "true"
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
  Returns max tolerance of the shape and prints error message if specified
  criteria are not satisfied.

  Use: checkmaxtol shape [options...]

  Options specify criteria for checking the maximal tolerance value:
    -ref <value>: check it to be equal to reference value.
    -min_tol <value>: check it to be not greater than specified value.
    -source <list of shapes>: check it to be not greater than 
            maximal tolerance of specified shape(s)
    -multi_tol <value>: additional multiplier for value specified by -min_tol 
               or -shapes options.
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

# auxiliary: check all arguments
proc _check_args { args {options {}} {command_name ""}} {
  # check arguments
  for {set narg 0} {${narg} < [llength ${args}]} {incr narg} {
    set arg [lindex ${args} ${narg}]
    set toContinue 0
    foreach option ${options} {
      set option_name            [lindex ${option} 0]
      set variable_to_save_value [lindex ${option} 1]
      set get_value              [lindex ${option} 2]
      set local_value ""
      if { [_check_arg ${option_name} local_value ${get_value}] } {
        upvar ${variable_to_save_value} ${variable_to_save_value}
        set ${variable_to_save_value} ${local_value}
        set toContinue 1
      }
    }
    if {${toContinue}} { continue }
    # unsupported option
    if { [regexp {^-} ${arg}] } {
      error "Error: unsupported option \"${arg}\""
    }
    error "Error: cannot interpret argument ${narg} (${arg})"
  }
  foreach option ${options} {
    set option_name            [lindex ${option} 0]
    set variable_to_save_value [lindex ${option} 1]
    set should_exist           [lindex ${option} 3]
    if {![info exists ${variable_to_save_value}] && ${should_exist} == 1} {
      error "Error: wrong using of command '${command_name}', '${option_name}' option is required"
    }
  }
}

help checkprops {
  Procedure includes commands to compute length, area and volume of input shape.

  Use: checkprops shapename [options...]
  Allowed options are:
    -l LENGTH: command lprops, computes the mass properties of all edges in the shape with a linear density of 1
    -s AREA: command sprops, computes the mass properties of all faces with a surface density of 1 
    -v VOLUME: command vprops, computes the mass properties of all solids with a density of 1
    -eps EPSILON: the epsilon defines relative precision of computation
    -deps DEPSILON: the epsilon defines relative precision to compare corresponding values
    -equal SHAPE: compare area\volume\length of input shapes. Puts error if its are not equal
    -notequal SHAPE: compare area\volume\length of input shapes. Puts error if its are equal
    -skip: count shared shapes only once, skipping repeatitions
  Options -l, -s and -v are independent and can be used in any order. Tolerance epsilon is the same for all options.
}

proc checkprops {shape args} {
    puts "checkprops ${shape} ${args}"
    upvar ${shape} ${shape}

    if {![isdraw ${shape}] || [regexp "${shape} is a \n" [whatis ${shape}]]} {
        puts "Error: The command cannot be built"
        return
    }

    set length -1
    set area -1
    set volume -1
    set epsilon 1.0e-4
    set compared_equal_shape -1
    set compared_notequal_shape -1
    set equal_check 0
    set skip 0
    set depsilon 1e-2

    set options {{"-eps" epsilon 1}
                 {"-equal" compared_equal_shape 1}
                 {"-notequal" compared_notequal_shape 1}
                 {"-skip" skip 0}
                 {"-deps" depsilon 1}}

    if { [regexp {\-[not]*equal} $args] } {
        lappend options {"-s" area 0}
        lappend options {"-l" length 0}
        lappend options {"-v" volume 0}
        set equal_check 1
    } else {
        lappend options {"-s" area 1}
        lappend options {"-l" length 1}
        lappend options {"-v" volume 1}
    }
    _check_args ${args} ${options} "checkprops"

    if { ${length} != -1 || ${equal_check} == 1 } {
        lappend CommandNames {lprops}
        set equal_check 0
    }
    if { ${area} != -1 || ${equal_check} == 1 } {
        lappend CommandNames {sprops}
        set equal_check 0
    }
    if { ${volume} != -1 || ${equal_check} == 1 } {
        lappend CommandNames {vprops}
        set equal_check 0
    }

    set skip_option ""
    if { $skip } {
        set skip_option "-skip"
    }
    
    foreach CommandName ${CommandNames} {
        switch $CommandName {
            "lprops"    { set mass ${length}; set prop "length" }
            "sprops"    { set mass ${area}; set prop "area" }
            "vprops"    { set mass ${volume}; set prop "volume" }
        }
        regexp {Mass +: +([-0-9.+eE]+)} [eval ${CommandName} ${shape} ${epsilon} $skip_option] full m

        if { ${compared_equal_shape} != -1 } {
            upvar ${compared_equal_shape} ${compared_equal_shape}
            regexp {Mass +: +([-0-9.+eE]+)} [eval ${CommandName} ${compared_equal_shape} ${epsilon} $skip_option] full compared_m
            if { $compared_m != $m } {
                puts "Error: Shape ${compared_equal_shape} is not equal to shape ${shape}"
            }
        }

        if { ${compared_notequal_shape} != -1 } {
            upvar ${compared_notequal_shape} ${compared_notequal_shape}
            regexp {Mass +: +([-0-9.+eE]+)} [eval ${CommandName} ${compared_notequal_shape} ${epsilon} $skip_option] full compared_m
            if { $compared_m == $m } {
                puts "Error: Shape ${compared_notequal_shape} is equal shape to ${shape}"
            }
        }

        if { ${compared_equal_shape} == -1 && ${compared_notequal_shape} == -1 } {
            if { [string compare "$mass" "empty"] != 0 } {
                if { $m == 0 } {
                    puts "Error : The command is not valid. The $prop is 0."
                }
                if { $mass > 0 } {
                    puts "The expected $prop is $mass"
                }
                #check of change of area is < 1%
                if { ($mass != 0 && [expr 1.*abs($mass - $m)/$mass] > $depsilon) || ($mass == 0 && $m != 0) } {
                    puts "Error : The $prop of result shape is $m"
                }
            } else {
                if { $m != 0 } {
                    puts "Error : The command is not valid. The $prop is $m"
                }
            }
        }
    }
}

help checkdump {
  Procedure includes command to parse output dump and compare it with reference values.

  Use: checkdump shapename [options...]
  Allowed options are:
    -name NAME: list of parsing parameters (e.g. Center, Axis, etc)
    -ref VALUE: list of reference values for each parameter in NAME 
    -eps EPSILON: the epsilon defines relative precision of computation
}

proc checkdump {shape args} {
    puts "checkdump ${shape} ${args}"
    upvar ${shape} ${shape}

    set ddump -1
    set epsilon -1
    set options {{"-name" params 1}
                 {"-ref" ref 1}
                 {"-eps" epsilon 1}
                 {"-dump" ddump 1}}

    if { ${ddump} == -1 } {
        set ddump [dump ${shape}]
    }
    _check_args ${args} ${options} "checkdump"

    set index 0
    foreach param ${params} {
        set pattern "${param}\\s*:\\s*" 
        set number_pattern "(\[-0-9.+eE\]+)\\s*" 
        set ref_values ""
        set local_ref ${ref}
        if { [llength ${params}] > 1 } {
            set local_ref [lindex ${ref} ${index}]
        }
        foreach item ${local_ref} {
            if { ![regexp "$pattern$number_pattern" $ddump full res] } {
                puts "Error: cheked parameter ${param} is not listed in dump"
                break
            }
            lappend ref_values $res 
            set pattern "${pattern}${res},\\s*" 
            ## without precision
            if { ${epsilon} == -1 } {
                if { ${item} != ${res} } {
                    puts "Error: parameter ${param} - current value (${res}) is not equal to reference value (${item})"
                } else {
                    puts "OK: parameter ${param} - current value (${res}) is equal to reference value (${item})"
                }
            ## with precision
            } else {
                set precision 0.0000001
                if { ( abs($res) > $precision ) || ( abs($item) > $precision ) } {
                    if { ($item != 0 && [expr 1.*abs($item - $res)/$item] > $epsilon) || ($item == 0 && $res != 0) } {
                        puts "Error: The $param of the resulting shape is $res and the expected $param is $item"
                    } else {
                        puts "OK: parameter ${param} - current value (${res}) is equal to reference value (${item})"
                    }
                }
            }
        }
        incr index
    }
}

help checklength {
  Procedure includes commands to compute length of input curve.

  Use: checklength curvename [options...]
  Allowed options are:
    -l LENGTH: command length, computes the length of input curve with precision of computation
    -eps EPSILON: the epsilon defines relative precision of computation
    -equal CURVE: compare length of input curves. Puts error if its are not equal
    -notequal CURVE: compare length of input curves. Puts error if its are equal
}

proc checklength {shape args} {
    puts "checklength ${shape} ${args}"
    upvar ${shape} ${shape}

    if {![isdraw ${shape}] || [regexp "${shape} is a \n" [whatis ${shape}]]} {
        puts "Error: The command cannot be built"
        return
    }

    set length -1
    set epsilon 1.0e-4
    set compared_equal_shape -1
    set compared_notequal_shape -1
    set equal_check 0

    set options {{"-eps" epsilon 1}
                 {"-equal" compared_equal_shape 1}
                 {"-notequal" compared_notequal_shape 1}}

    if { [regexp {\-[not]*equal} $args] } {
        lappend options {"-l" length 0}
        set equal_check 1
    } else {
        lappend options {"-l" length 1}
    }
    _check_args ${args} ${options} "checkprops"

    if { ${length} != -1 || ${equal_check} == 1 } {
        set CommandName length
        set mass $length
        set prop "length"
        set equal_check 0
    }

    regexp "The +length+ ${shape} +is +(\[-0-9.+eE\]+)" [${CommandName} ${shape} ${epsilon}] full m

    if { ${compared_equal_shape} != -1 } {
        upvar ${compared_equal_shape} ${compared_equal_shape}
        regexp "The +length+ ${compared_equal_shape} +is +(\[-0-9.+eE\]+)" [${CommandName} ${compared_equal_shape} ${epsilon}] full compared_m
        if { $compared_m != $m } {
            puts "Error: length of shape ${compared_equal_shape} is not equal to shape ${shape}"
        }
    }

    if { ${compared_notequal_shape} != -1 } {
        upvar ${compared_notequal_shape} ${compared_notequal_shape}
        regexp "The +length+ ${compared_notequal_shape} +is +(\[-0-9.+eE\]+)" [${CommandName} ${compared_notequal_shape} ${epsilon}] full compared_m
        if { $compared_m == $m } {
            puts "Error: length of shape ${compared_notequal_shape} is equal shape to ${shape}"
        }
    }

    if { ${compared_equal_shape} == -1 && ${compared_notequal_shape} == -1 } {
        if { [string compare "$mass" "empty"] != 0 } {
            if { $m == 0 } {
                puts "Error : The command is not valid. The $prop is 0."
            }
            if { $mass > 0 } {
                puts "The expected $prop is $mass"
            }
            #check of change of area is < 1%
            if { ($mass != 0 && [expr 1.*abs($mass - $m)/$mass] > 0.01) || ($mass == 0 && $m != 0) } {
                puts "Error : The $prop of result shape is $m"
            }
        } else {
            if { $m != 0 } {
                puts "Error : The command is not valid. The $prop is $m"
            }
        }
    }
}

help checkview {
  Display shape in selected viewer.

  Use: checkview [options...]
  Allowed options are:
    -display shapename: display shape with name 'shapename'
    -3d: display shape in 3d viewer
    -2d [ v2d / smallview ]: display shape in 2d viewer (default viewer is a 'smallview')
    -vdispmode N: it is possible to set vdispmode for 3d viewer (default value is 1)
    -screenshot: procedure will try to make screenshot of already created viewer
    -path <path>: location of saved screenshot of viewer

    Procedure can check some property of shape (length, area or volume) and compare it with some value N:
      -l [N]
      -s [N]
      -v [N]
    If current property is equal to value N, shape is marked as valid in procedure.
    If value N is not given procedure will mark shape as valid if current property is non-zero.
    -with {a b c}: display shapes 'a' 'b' 'c' together with 'shape' (if shape is valid)
    -otherwise {d e f}: display shapes 'd' 'e' 'f' instead of 'shape' (if shape is NOT valid)
    Note that one of two options -2d/-3d is required.
}

proc checkview {args} {
  puts "checkview ${args}"

  set 3dviewer 0
  set 2dviewer false
  set shape ""
  set PathToSave ""
  set dispmode 1
  set isScreenshot 0
  set check_length false
  set check_area false
  set check_volume false
  set otherwise {}
  set with {}

  set options {{"-3d" 3dviewer 0}
               {"-2d" 2dviewer ?}
               {"-display" shape 1}
               {"-path" PathToSave 1}
               {"-vdispmode" dispmode 1}
               {"-screenshot" isScreenshot 0}
               {"-otherwise" otherwise 1}
               {"-with" with 1}
               {"-l" check_length ?}
               {"-s" check_area ?}
               {"-v" check_volume ?}}

  # check arguments
  _check_args ${args} ${options} "checkview"

  if { ${PathToSave} == "" } {
    set PathToSave "./photo.png"
  }

  if { ${3dviewer} == 0 && ${2dviewer} == false } {
    error "Error: wrong using of command 'checkview', please use -2d or -3d option"
  }

  if { ${isScreenshot} } {
    if { ${3dviewer} } {
      vdump ${PathToSave}
    } else {
      xwd ${PathToSave}
    }
    return
  }

  set mass 0
  set isBAD 0
  upvar ${shape} ${shape}
  if {[isdraw ${shape}]} {
    # check area
    if { [string is boolean ${check_area}] } {
      if { ${check_area} } {
        regexp {Mass +: +([-0-9.+eE]+)} [sprops ${shape}] full mass
      }
    } else {
      set mass ${check_area}
    }
    # check length
    if { [string is boolean ${check_length}] } {
      if { ${check_length} } {
        regexp {Mass +: +([-0-9.+eE]+)} [lprops ${shape}] full mass
      }
    } else {
      set mass ${check_length}
    }
    # check volume
    if { [string is boolean ${check_volume}] } {
      if { ${check_volume} } {
        regexp {Mass +: +([-0-9.+eE]+)} [vprops ${shape}] full mass
      }
    } else {
      set mass ${check_volume}
    }
  } else {
    set isBAD 1
  }
  if { ${3dviewer} } {
    vinit
    vclear
  } elseif { ([string is boolean ${2dviewer}] && ${2dviewer}) || ${2dviewer} == "smallview"} {
    smallview
    clear
  } elseif { ${2dviewer} == "v2d"} {
    v2d
    2dclear
  }
  if {[isdraw ${shape}]} {
    if { ( ${check_area} == false && ${check_length} == false && ${check_volume} == false ) || ( ${mass} != 0 ) } {
      foreach s ${with} {
        upvar ${s} ${s}
      }
      lappend with ${shape}
      if { ${3dviewer} } {
        vdisplay {*}${with}
      } else {
        donly {*}${with}
      }
    } else {
      set isBAD 1
    }
  } else {
    set isBAD 1
  }

  if { ${isBAD} && [llength ${otherwise}] } {
    foreach s ${otherwise} {
      upvar ${s} ${s}
    }
    if { ${3dviewer} } {
      vdisplay {*}${otherwise}
    } else {
      donly {*}${otherwise}
    }
  }

  if { ${3dviewer} } {
    vsetdispmode ${dispmode}
    vfit
    vdump ${PathToSave}
  } else {
    if { ([string is boolean ${2dviewer}] && ${2dviewer}) || ${2dviewer} == "smallview"} {
      fit
    } elseif { ${2dviewer} == "v2d"} {
      2dfit
    }
    xwd ${PathToSave}
  }

}

help checktrinfo {
  Compare maximum deflection, number of nodes and triangles in "shape" mesh with given reference data

  Use: checktrinfo shapename [options...]
  Allowed options are:
    -tri [N]:  compare current number of triangles in "shapename" mesh with given reference data.
               If reference value N is not given and current number of triangles is equal to 0
               procedure checktrinfo will print an error.
    -nod [N]:  compare current number of nodes in "shapename" mesh with given reference data.
               If reference value N is not givenand current number of nodes is equal to 0
               procedure checktrinfo will print an error.
    -defl [N]: compare current value of maximum deflection in "shapename" mesh with given reference data
               If reference value N is not given and current maximum deflection is equal to 0
               procedure checktrinfo will print an error.
    -max_defl N:     compare current value of maximum deflection in "shapename" mesh with max possible value
    -tol_abs_tri N:  absolute tolerance for comparison of number of triangles (default value 0)
    -tol_rel_tri N:  relative tolerance for comparison of number of triangles (default value 0)
    -tol_abs_nod N:  absolute tolerance for comparison of number of nodes (default value 0)
    -tol_rel_nod N:  relative tolerance for comparison of number of nodes (default value 0)
    -tol_abs_defl N: absolute tolerance for deflection comparison (default value 0)
    -tol_rel_defl N: relative tolerance for deflection comparison (default value 0)
    -ref [trinfo a]: compare deflection, number of triangles and nodes in "shapename" and in "a"
}
proc checktrinfo {shape args} {
    puts "checktrinfo ${shape} ${args}"
    upvar ${shape} ${shape}

    if {![isdraw ${shape}] || [regexp "${shape} is a \n" [whatis ${shape}]]} {
        puts "Error: The command cannot be built"
        return
    }

    set ref_nb_triangles false
    set ref_nb_nodes false
    set ref_deflection false
    set tol_abs_defl 0
    set tol_rel_defl 0
    set tol_abs_tri 0
    set tol_rel_tri 0
    set tol_abs_nod 0
    set tol_rel_nod 0
    set max_defl -1
    set ref_info ""

    set options {{"-tri" ref_nb_triangles ?}
                 {"-nod" ref_nb_nodes ?}
                 {"-defl" ref_deflection ?}
                 {"-tol_abs_defl" tol_abs_defl 1}
                 {"-tol_rel_defl" tol_rel_defl 1}
                 {"-tol_abs_tri" tol_abs_tri 1}
                 {"-tol_rel_tri" tol_rel_tri 1}
                 {"-tol_abs_nod" tol_abs_nod 1}
                 {"-tol_rel_nod" tol_rel_nod 1}
                 {"-max_defl" max_defl 1}
                 {"-ref" ref_info 1}}

    _check_args ${args} ${options} "checktrinfo"

    # get current number of triangles and nodes, value of max deflection
    set tri_info [trinfo ${shape}]
    set triinfo_pattern "(\[0-9\]+) +triangles.*\[^0-9]\(\[0-9\]+) +nodes.*deflection +(\[-0-9.+eE\]+)"
    if {![regexp "${triinfo_pattern}" ${tri_info} dump cur_nb_triangles cur_nb_nodes cur_deflection]} {
        puts "Error: command trinfo prints empty info"
    }

    # get reference values from -ref option
    if { "${ref_info}" != ""} {
        if {![regexp "${triinfo_pattern}" ${ref_info} dump ref_nb_triangles ref_nb_nodes ref_deflection]} {
            puts "Error: reference information gived by -ref option is wrong"
        }
    }

    # check number of triangles
    if { [string is boolean ${ref_nb_triangles}] } {
        if { ${cur_nb_triangles} <= 0 && ${ref_nb_triangles} } {
            puts "Error: Number of triangles is equal to 0"
        }
    } else {
        if {[regexp {!([-0-9.+eE]+)} $ref_nb_triangles full ref_nb_triangles_value]} {
            if  {${ref_nb_triangles_value} == ${cur_nb_triangles} } {
                puts "Error: Number of triangles is equal to ${ref_nb_triangles_value} but it should not"
            }
        } else {
            checkreal "Number of triangles" ${cur_nb_triangles} ${ref_nb_triangles} ${tol_abs_tri} ${tol_rel_tri}
        }
    }

    # check number of nodes
    if { [string is boolean ${ref_nb_nodes}] } {
        if { ${cur_nb_nodes} <= 0 && ${ref_nb_nodes} } {
            puts "Error: Number of nodes is equal to 0"
        }
    } else {
        if {[regexp {!([-0-9.+eE]+)} $ref_nb_nodes full ref_nb_nodes_value]} {
            if  {${ref_nb_nodes_value} == ${cur_nb_nodes} } {
                puts "Error: Number of nodes is equal to ${ref_nb_nodes_value} but it should not"
            }
        } else {
            checkreal "Number of nodes" ${cur_nb_nodes} ${ref_nb_nodes} ${tol_abs_nod} ${tol_rel_nod}
        }
    }

    # check deflection
    if { [string is boolean ${ref_deflection}] } {
        if { ${cur_deflection} <= 0 && ${ref_deflection} } {
            puts "Error: Maximal deflection is equal to 0"
        }
    } else {
        checkreal "Maximal deflection" ${cur_deflection} ${ref_deflection} ${tol_abs_defl} ${tol_rel_defl}
    }

    if { ${max_defl} != -1 && ${cur_deflection} > ${max_defl} } {
        puts "Error: Maximal deflection is too big"
    }
}

help checkplatform {
  Return name of current platform if no options are given.

  Use: checkplatform [options...]
  Allowed options are:
    -windows : return 1 if current platform is 'Windows', overwise return 0
    -linux   : return 1 if current platform is 'Linux', overwise return 0
    -osx     : return 1 if current platform is 'MacOS X', overwise return 0

  Only one option can be used at once.
  If no option is given, procedure will return the name of current platform.
}
proc checkplatform {args} {
    set check_for_windows false
    set check_for_linux false
    set check_for_macosx false

    set options {{"-windows" check_for_windows 0}
                 {"-linux" check_for_linux 0}
                 {"-osx" check_for_macosx 0}}

    _check_args ${args} ${options} "checkplatform"

    if { [regexp "indows" $::tcl_platform(os)] } {
        set current_platform Windows
    } elseif { $::tcl_platform(os) == "Linux" } {
        set current_platform Linux
    } elseif { $::tcl_platform(os) == "Darwin" } {
        set current_platform MacOS
    }

    # no args are given
    if { !${check_for_windows} && !${check_for_linux} && !${check_for_macosx}} {
        return ${current_platform}
    }

    # check usage of proc checkplatform
    if { [expr [string is true ${check_for_windows}] + [string is true ${check_for_linux}] + [string is true ${check_for_macosx}] ] > 1} {
        error "Error: wrong usage of command checkplatform, only single option can be used at once"
    }

    # checking for Windows platform
    if { ${check_for_windows} && ${current_platform} == "Windows" } {
        return 1
    }

    # checking for Mac OS X platforms
    if { ${check_for_linux} && ${current_platform} == "Linux" } {
        return 1
    }

    # checking for Mac OS X platforms
    if { ${check_for_macosx} && ${current_platform} == "MacOS" } {
        return 1
    }

    # current platform is not equal to given as argument platform, return false
    return 0
}
