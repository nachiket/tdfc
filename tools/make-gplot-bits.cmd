(tail -6 mix=* || tail 6 mix=*) | grep bits | perl -ne '/\((\d*(\.\d*)?) per page\)/; printf("%1.1f $1\n",$n); $n+=.1' > results-bits.gplot
