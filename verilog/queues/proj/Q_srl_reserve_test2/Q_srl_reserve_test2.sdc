# Synplicity, Inc. constraint file
# /home/cs/eylon/brass/tdfc/verilog/queues/proj/Q_srl_reserve_test2/Q_srl_reserve_test2.sdc
# Written on Thu Aug 26 19:38:34 2004
# by Synplify Pro, 7.6.1      Scope Editor

#
# Clocks
#

#
# Clock to Clock
#

#
# Inputs/Outputs
#
define_input_delay -disable      -default -improve 0.00 -route 0.00
define_output_delay -disable     -default -improve 0.00 -route 0.00
define_output_delay              {i_b}  0.50 -improve 0.00 -route 0.00
define_input_delay -disable      {i_d[255:0]} -improve 0.00 -route 0.00
define_input_delay -disable      {i_v} -improve 0.00 -route 0.00
define_input_delay -disable      {o_b} -improve 0.00 -route 0.00
define_output_delay              {o_d[255:0]}  0.50 -improve 0.00 -route 0.00
define_output_delay              {o_v}  0.50 -improve 0.00 -route 0.00
define_input_delay -disable      {reset} -improve 0.00 -route 0.00

#
# Registers
#

#
# Multicycle Path
#

#
# False Path
#

#
# Delay Path
#

#
# Attributes
#

#
# Compile Points
#

#
# Other Constraints
#
