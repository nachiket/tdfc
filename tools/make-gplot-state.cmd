(tail -6 mix=* || tail 6 mix=*) | grep state | perl -ne '/\((\d*(\.\d*)?)%\)/; printf("%1.1f $1\n",$n); $n+=0.1' > results-state.gplot
