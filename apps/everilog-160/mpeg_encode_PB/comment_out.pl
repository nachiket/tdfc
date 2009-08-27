#!/usr/bin/perl

$finish = 0;

while ($line = <STDIN>) {
    if ($line =~ /else/) {
	print "$line #if 0\n";
	    $finish = 1;
    } elsif (($finish == 1) && ($line =~ /\}/)) {
	print "#endif\n$line";
	$finish = 0;
    } else {
	print $line;
    }
}
