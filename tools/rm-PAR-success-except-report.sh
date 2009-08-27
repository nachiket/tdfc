if [ -f PERIOD_PAR -o -f PERIOD_PAR.gz ] ; then \
  'ls' | egrep -v '^(make|make\.OUT|Makefile|tools|LUTFFS_.*|SLICES_.*|PERIOD_.*|ROUTEFRAC_.*|TIME_.*|EXPANSIONS_.*|INTERRUPTED|TOOBIG)(\.(gz|bz|bz2))?$' | xargs rm -rf
fi
