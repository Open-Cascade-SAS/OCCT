# test performance of display of heavy scene involving multiple interactive
# objects, on example of 1000 spheres

#Category: Visualization
#Title: Display of complex scene and animation

pload MODELING
pload VISUALIZATION

vinit View1 w=1024 h=1024
vclear

# parameter NB defines number of spheres by each coordinate
set NB 10
puts "Creating [expr $NB * $NB * $NB] spheres..."
set slist {}
for {set i 0} {$i < $NB} {incr i} {
  for {set j 0} {$j < $NB} {incr j} {
    for {set k 0} {$k < $NB} {incr k} {
      psphere s$i$j$k 1.
      lappend slist s$i$j$k
      ttranslate s$i$j$k 3.*$i 3.*$j 3.*$k
    }
  }
}

puts "Measuring FPS of display of spheres as separate objects..."
vaxo
vsetdispmode 1
eval vdisplay $slist
vfit

# measure FPS
puts [set fps_separate [vfps]]
vclear

puts "Measuring FPS of display of spheres as single object..."
eval compound $slist c
vdisplay c

# measure FPS
puts [set fps_compound [vfps]]
vclear

# redisplay individual spheres, trying to avoid unnecessary internal updates
#vfrustumculling 0 ;# try to disable updates of frustum culling structures
eval vdisplay -mutable $slist

# auxiliary procedure to make random update of variable
proc upd {theValueName theDeltaName theTime theToRand} {
  upvar $theValueName aValue
  upvar $theDeltaName aDelta

  # set colors to corner spheres
  if { $theToRand == 1 } {
    set aValue [expr $aValue + $aDelta * $theTime / 100.0]
    set aDelta [expr 0.5 * (rand() - 0.5)]
    return $aValue
  }

  set aRes [expr $aValue + $aDelta * $theTime / 100.0]
}

# move corner spheres in cycle
proc animateSpheres {{theDuration 10.0}} {
  set nb [expr $::NB - 1]

  # set colors to corner spheres
  for {set i 0} {$i < $::NB} {incr i $nb} {
    for {set j 0} {$j < $::NB} {incr j $nb} {
      for {set k 0} {$k < $::NB} {incr k $nb} {
#       vaspects -noupdate s$i$j$k -setcolor red -setmaterial plastic
        vaspects -noupdate s$i$j$k -setcolor red
        set x$i$j$k  0.0
        set y$i$j$k  0.0
        set z$i$j$k  0.0
        set dx$i$j$k 0.0
        set dy$i$j$k 0.0
        set dz$i$j$k 0.0
      }
    }
  }

  set aDuration 0.0
  set aPrevRand 0.0
  set aTimeFrom [clock clicks -milliseconds]
  uplevel #0 chrono anAnimTimer reset
  uplevel #0 chrono anAnimTimer start
  set toRand 1
  for {set aFrameIter 1} { $aFrameIter > 0 } {incr aFrameIter} {
    set aCurrTime [expr [clock clicks -milliseconds] - $aTimeFrom]
    if { $aCurrTime >= [expr $theDuration * 1000.0] } {
      puts "Nb Frames: $aFrameIter"
      puts "Duration:  [expr $aCurrTime * 0.001] s"
      set fps [expr ($aFrameIter - 1) / ($aDuration * 0.001) ]
      puts "FPS:       $fps"
      uplevel #0 chrono anAnimTimer stop
      uplevel #0 chrono anAnimTimer show
      return $fps
    }

    set aRandTime [expr $aCurrTime - $aPrevRand]
    if { $aRandTime > 1000 } {
      set toRand 1
      set aPrevRand $aCurrTime
    }

    #puts "PTS: $aCurrTime ms"
    for {set i 0} {$i < $::NB} {incr i $nb} {
      for {set j 0} {$j < $::NB} {incr j $nb} {
        for {set k 0} {$k < $::NB} {incr k $nb} {
          uplevel #0 vsetlocation -noupdate s$i$j$k [upd x$i$j$k dx$i$j$k $aRandTime $toRand] [upd y$i$j$k dy$i$j$k $aRandTime $toRand] [upd z$i$j$k dz$i$j$k $aRandTime $toRand] 
        }
      }
    }
    uplevel #0 vrepaint
    set aDuration [expr [clock clicks -milliseconds] - $aTimeFrom]
    set toRand 0

    # sleep 1 ms allowing the user to interact with the viewer
    after 1 set waiter 1
    vwait waiter
  }
}

puts "Animating movements of corner spheres (10 sec)..."
puts "(you can interact with the view during the process)"
set fps_animation [animateSpheres 10.0]

puts ""
puts "Performance counters (FPS = \"Frames per second\"):"
puts ""
puts "Spheres as separate interactive objects:"
puts "  Actual FPS: [lindex $fps_separate 1]"
puts "  FPS estimate by CPU load: [expr 1000. / [lindex $fps_separate 3]]"
puts ""
puts "Spheres as one interactive object (compound):"
puts "  Actual FPS: [lindex $fps_compound 1]"
puts "  FPS estimate by CPU load: [expr 1000. / [lindex $fps_compound 3]]"
puts ""
puts "Animation FPS: $fps_animation"
puts ""
puts "Scene contains [lindex [trinfo c] 3] triangles"
puts ""
puts "Print 'animateSpheres 10.0' to restart animation"