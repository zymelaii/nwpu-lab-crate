#! /usr/bin/perl

# Find the function name from the value of the EIP (instruction pointer)
# register from a Bochs crash report.  Uses the kernel symbol
# map (kernel.syms) produced by compiling the kernel.

use strict qw(refs vars);
use FileHandle;

if (scalar(@ARGV) != 2){
	print STDERR "Usage: eipToFunction kernel.syms <eip value>\n";
	print STDERR "   eip value should be in hex\n";
	exit 1;
}

my $syms = shift @ARGV;
my $eip = hex(shift @ARGV);

my @text = ();

my $fh = new FileHandle("<$syms");
(defined $fh) || die "Couldn't open $syms: $!\n";
while (<$fh>) {
	#print $_;
	if (/^([0-9A-Fa-f]+)\s+[Tt]\s+(\S+)\s*$/) {
		push @text, [hex($1), $2];
	}
}
$fh->close();
#print scalar(@text),"\n";

@text = sort { $a->[0] <=> $b->[0] } @text;

my $last = undef;

foreach my $entry (@text) {
	last if ($eip < $entry->[0]);
	$last = $entry;
}
printf("%s\n",(defined $last) ? $last->[1] : "not found");

# vim:ts=4
