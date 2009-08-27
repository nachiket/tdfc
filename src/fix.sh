#!/bin/zsh

for i in Q_*.h; do
mv $i.old $i;
cat $i | perl -lane 'BEGIN{$inside=0;} $line=$_; if(/"\\/) {$inside=1;} elsif(/";/) {$inside=0;} else {}; if($inside==1) {print $line,"\\n\\";} else {print $line;}' > $i.patched
mv $i $i.old
mv $i.patched $i
done;

