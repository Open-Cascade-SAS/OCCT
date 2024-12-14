# parse list of tests to skip (tests_to_skip)

set current_folder [file dirname [info script]]

set f [open "$current_folder/tests_to_skip" r]
set tests_to_skip "[read $f]"
close $f

testgrid * -exclude "$tests_to_skip"

exit
