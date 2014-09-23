# send trivial stuff back to nachos to let it know a wish op is done
proc talkback {} {
    puts stdout "X"
    flush stdout
}

# move process to barrier
proc tobarrier id {
    .c delete t$id
    .c create oval [expr 0+$id*25] 25 [expr 25+$id*25] 49 -fill red -tags t$id
    talkback
} 

# move process past barrier
proc toend id {
    .c delete t$id
    .c create oval [expr 0+$id*25] 50 [expr 25+$id*25] 75 -fill green -tags t$id
    talkback
} 

# process is dead
proc todead id {
    .c delete t$id
    .c create oval [expr 0+$id*25] 50 [expr 25+$id*25] 75 -fill black -tags t$id
    talkback
} 

# Create canvas widget
canvas .c -width 500 -height 75 -bg white
pack .c -side top

# throw in the barrier line
.c create line 0 37 500 37 -width 5 -tag bar
# and the circles to denote processes
.c create oval 0   0 25  25 -tags t0
.c create oval 25  0 50  25 -tags t1
.c create oval 50  0 75  25 -tags t2
.c create oval 75  0 100 25 -tags t3
.c create oval 100 0 125 25 -tags t4
.c create oval 125 0 150 25 -tags t5
.c create oval 150 0 175 25 -tags t6
.c create oval 175 0 200 25 -tags t7
.c create oval 200 0 225 25 -tags t8
.c create oval 225 0 250 25 -tags t9
.c create oval 250 0 275 25 -tags t10
.c create oval 275 0 300 25 -tags t11
.c create oval 300 0 325 25 -tags t12
.c create oval 325 0 350 25 -tags t13
.c create oval 350 0 375 25 -tags t14
.c create oval 375 0 400 25 -tags t15
.c create oval 400 0 425 25 -tags t16
.c create oval 425 0 450 25 -tags t17
.c create oval 450 0 475 25 -tags t18
.c create oval 475 0 500 25 -tags t19
# Frame for holding buttons
frame .bf
pack  .bf -expand 1 -fill x

# Exit button
button .bf.exit -text "Exit" -command {exit}
pack .bf.exit -side left

talkback
