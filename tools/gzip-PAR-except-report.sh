if [ -f PERIOD_PAR -o -f PERIOD_PAR.gz ] ; then \
  'ls' | egrep -v '^(make|make\.OUT|Makefile|tools|LUTFFS_.*|SLICES_.*|PERIOD_.*|ROUTEFRAC_.*|TIME_.*)(\.gz)?$' | xargs gzip -r
fi
