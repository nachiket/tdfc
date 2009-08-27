#!csh
set outfile=results-bits-max.gplot
rm -f $outfile
touch $outfile
@ mix = 0
foreach file ( mix=* )
  setenv MIX_COEF $mix
  cat $file | perl -e 'foreach (<>) { if (/cut_df_bits=(\d+)/ && $max<$1) {$max=$1} } printf("%1.1f $max\n",$ENV{"MIX_COEF"}/10)' >>$outfile
  @ mix = $mix + 1
end
unsetenv MIX_COEF
