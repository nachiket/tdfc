#!/bin/csh
#
# Run Q_srl_reserve*.v through synplify with different depth/width parameters
#
# Synplify 7.1
# Eylon Caspi,  3/22/04, 7/23/04

set verilog_file_out   = Q_srl_reserve_test.v
set verilog_module_in  = Q_srl_reserve
set verilog_module_out = Q_srl_reserve_test

# foreach verilog (Q_srl_reserve_prenone.v Q_srl_reserve_prefull.v Q_srl_reserve_prezero.v Q_srl_reserve_preboth.v Q_srl_reserve_prenone_preresv_SIMPLE.v Q_srl_reserve_prefull_preresv_SIMPLE.v Q_srl_reserve_prezero_preresv_SIMPLE.v Q_srl_reserve_preboth_preresv_SIMPLE.v Q_srl_reserve_prenone_preresv.v Q_srl_reserve_prefull_preresv.v Q_srl_reserve_prezero_preresv.v Q_srl_reserve_preboth_preresv.v)
foreach verilog (Q_srl_reserve_prenone.v Q_srl_reserve_prefull.v Q_srl_reserve_prezero.v Q_srl_reserve_preboth.v Q_srl_reserve_prenone_preresv_SIMPLE.v Q_srl_reserve_prefull_preresv_SIMPLE.v)
# foreach depth (2 4 8 16 32 64 128 256)
#   foreach width (1 2 4 8 16 32 64 128 256)
  foreach depth (2 4 8 16 32 64 128)
    foreach width (1 2 4 8 16 32 64 128)
      if (($depth == 128) && ($width >= 256)) \
        continue					# - too slow
      if (($depth == 256) && ($width >= 128)) \
        continue					# - too slow
      @ halfreserve = $depth / 2
      foreach reserve (0 2 ${halfreserve})
        if ($reserve >= $depth)		      \
	  continue					# - illegal
        echo "Testing  { $verilog $depth $width $reserve }  --  forming $verilog_file_out"
	mv $verilog_file_out ${verilog_file_out}.bak
        perl -npe "s/module $verilog_module_in/module $verilog_module_out/;   \
		   s/parameter depth *=.*;/parameter depth = $depth;/;        \
		   s/parameter width *=.*;/parameter width = $width;/;        \
		   s/parameter reserve *=.*;/parameter reserve = $reserve;/;" \
		  < $verilog > $verilog_file_out
        foreach rev (rev_1__speed rev_2__area rev_3__no_optim rev_4__200mhz)
	  echo "Testing  { $verilog $depth $width $reserve }  --  compiling $rev"
	  set synplify_dir = proj/$verilog_module_out
	  set logfile      = $synplify_dir/$rev/$verilog_module_out.log
	  set cmd = "/bin/nice -19 synplify_pro -batch $synplify_dir/*.prj -impl $rev -log $logfile"
	  echo $cmd
	  $cmd
	  echo "Testing  { $verilog $depth $width $reserve }  --  results $rev"
	  perl -e							    \
	    'while (<>) {						    \
	       if (/(clock|system) +(\d+(.\d+)?) +MHz +(\d+(.\d)?)/i)	    \
	         { if ($speed==0 || $speed>$4) { $speed=$4; } }		    \
	       elsif (/^Total +LUTs: +(\d+)/i)				    \
	         { if ($area==0  || $area<$1)  { $area=$1;  } }		    \
	       elsif (/^Register bits not including I\/Os: +(\d+)/i)	    \
	         { if ($regs==0  || $regs<$1)  { $regs=$1;  } }		    \
	       elsif (/^Output Ports:/)					    \
	         { if (\!$do_outs && \!$done_outs) { $do_outs=1;    } }	    \
	       elsif (/^i_b +.*\) +((\d+(\.\d+)?)|NA) +(\d+(\.\d+)?)/)      \
	         { if (  $do_outs && \!$done_outs) { $delay_i_b=$4; } }	    \
	       elsif (/^o_v +.*\) +((\d+(\.\d+)?)|NA) +(\d+(\.\d+)?)/)      \
	         { if (  $do_outs && \!$done_outs) { $delay_o_v=$4; } }	    \
	       elsif (/^========/)					    \
	         { if (  $do_outs && \!$done_outs) { $done_outs=1;  } }	    \
	     }								    \
	     print "$speed MHz	$area LUTs	$regs FFs	" .	    \
		   "$delay_i_b ns i_b	$delay_o_v ns o_v\n" '		    \
	     $synplify_dir/$rev/${verilog_module_out}.srr
	end
      end
    end
  end
end
