#!csh
perl -e 'foreach (<>) { if (/(\d+)\.(\d+) (\d+(\.\d+)?)/) { $i=$1*10+$2; if (defined($min[$i])==0 || $max[$i]<$3) {$max[$i]=$3} } } for ($n=0; $n<=10; $n++) {print "$n $max[$n]\n"}' $*
