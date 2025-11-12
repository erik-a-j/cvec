
use strict;
use warnings;
use open ':std', ':encoding(UTF-8)';

my $file = "md/README_DEV.md";
my $ofile = "md/README.md";

open(my $in, '<:encoding(UTF-8)', $file) or die "Cannot read $file: $!";
open(my $out, '>:encoding(UTF-8)', $ofile) or die "Cannot write $ofile: $!";

while (my $line = <$in>) {
    if ($line =~ m{^(.*?)<!--\s*include:(.*?):(.*?)(?::(.*?))?\s*-->(.*?)$}) {
        my ($before, $path, $start_pat, $end_pat, $after) = ($1, $2, $3, $4, $5);
		print {$out} $before;

        my $start_re = qr/$start_pat/;
        my $end_re   = defined $end_pat ? qr{$end_pat} : undef;

        open(my $src, '<:encoding(UTF-8)', $path)
          or die "Cannot read $path: $!";
		
        my $inside = 0;
		while (my $sline = <$src>) {
			chomp($sline);
            if (!$inside) {
                if ($sline =~ $start_re) {
                    if (!defined $end_re) {
                        print {$out} $sline;
                        last;
                    }
                    $inside = 1;
                }
            }
            elsif ($inside) {
                print {$out} $sline;
                last if $sline =~ $end_re;
            }
        }
        close $src;
		print {$out} $after;
	} else {
		print {$out} $line;
	}
}

close $in;
close $out;