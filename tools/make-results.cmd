#!csh
foreach result ( results-bits results-bits-max results-state results-streams results-streams-max )
  cat mix*/$result.gplot | sort > $result-100.gplot
end
