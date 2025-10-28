#!/usr/bin/env perl
use strict;
use warnings;
use Getopt::Long qw(:config bundling);
use Pod::Usage;
use FindBin qw($Bin);
use File::Spec;
use File::Path qw(make_path);
use open ':std', ':encoding(UTF-8)';

my $dir    = $Bin;
my $srcdir  = File::Spec->catdir($dir, '..', 'src');
my $incdir  = File::Spec->catdir($srcdir, 'include');
my $docdir  = File::Spec->catdir($srcdir, 'doc');
my $outdir = '';
my $header_suffix = '';
my $verbose = 0;
my $help = 0;

GetOptions(
  'outdir=s'        => \$outdir,
  'header-suffix=s' => \$header_suffix,
  'verbose|v'       => \$verbose,
  'help|h'          => \$help,
) or pod2usage(2);
pod2usage(1) if $help;

$outdir =~ s/^\s+|\s+$//g;
$outdir =~ s{/$}{};

$outdir or pod2usage("Usage: \@headers | $0 --outdir <dir> [-v]");

my @headers = <STDIN>;     # grab all lines
chomp @headers;            # remove trailing newlines
@headers = grep { length } @headers;

my $api_header_file = "$outdir/cvec.h";
my $header_leaf_dir = "cvec";
my $outheader_dir = "$outdir/$header_leaf_dir";
make_path($outheader_dir) unless -d $outheader_dir;

my %api_header = (
  hguard_beg => "#ifndef CVEC_H\n#define CVEC_H\n\n",
  hguard_end => "\n#endif\n",
);

HEADER:
for my $h (@headers) {
  (my $h_out_name = $h) =~ s/\.h$/$header_suffix\.h/;
  if ($h !~ /cvec_api/) {
    $api_header{$h} = "#include \"$header_leaf_dir/$h_out_name\"\n";
  }
  my $h_file   = "$incdir/$h";
  my $doc_file = "$docdir/h/${h}doc";
  my $out_file = "$outheader_dir/$h_out_name";

  # Load the doc file (skip header if doc is missing)
  open(my $df, '<:encoding(UTF-8)', $doc_file) or do {
    warn "skip: no doc for $h ($doc_file): $!" if $h !~ /cvec_api/;
    if (open(my $in,  '<:encoding(UTF-8)', $h_file)
        && open(my $out, '>:encoding(UTF-8)', $out_file)) {
      print {$out} do { local $/; <$in> };
      close $in; close $out;
    }
    next HEADER;
  };
  my $doc = do { local $/; <$df> };
  close $df;
  $doc =~ s/^\/\/\s*clang-format off\s*\n+//;

  my %replace_for; # doc_id (exact text line) => processed doc_str

  # Parse blocks:  first line is the doc_id, then a /* ... */ block
  while ($doc =~ /(.*?)\n(.*?\*\/)\n*/sg) {
    my ($doc_id, $doc_str) = ($1, $2);
    $doc_str .= "\n" unless $doc_str =~ /\n\z/;
    $replace_for{$doc_id} = $doc_str;
  }

  # If nothing parsed, just copy input -> output and continue
  if (!%replace_for) {
    if (open(my $in,  '<:encoding(UTF-8)', $h_file)
        && open(my $out, '>:encoding(UTF-8)', $out_file)) {
      print {$out} do { local $/; <$in> };
      close $in; close $out;
    }
    next HEADER;  # <- was exit 0 before
  }

  # Build alternation for exact doc_id hits (longest first)
  my @ids = sort { length($b) <=> length($a) } keys %replace_for;
  my $alternation = join '|', map { quotemeta($_) } @ids;
  my $id_re = qr/($alternation)/;

  open(my $in,  '<:encoding(UTF-8)', $h_file)   or do { warn "skip $h: $!"; next HEADER };
  open(my $out, '>:encoding(UTF-8)', $out_file) or die "Cannot write $out_file: $!";

  while (my $line = <$in>) {
    if (my ($hit) = $line =~ /$id_re/) {
      print {$out} $replace_for{$hit};
    } else {
      print {$out} $line;
    }
  }
  close $in;
  close $out;
}

open(my $out, '>:encoding(UTF-8)', $api_header_file) or die "Cannot write $api_header_file: $!";
print {$out} $api_header{hguard_beg};

open(my $in, '<:encoding(UTF-8)', "$docdir/cvec.hdoc") or die "Cannot write $docdir/cvec.hdoc: $!";
print {$out} do { local $/; <$in> }; print {$out} "\n\n";
close $in;

my @ordered = grep { exists $api_header{$_} } @headers;
print {$out} @api_header{@ordered};

print {$out} $api_header{hguard_end};
close $out;


__END__

=head1 SYNOPSIS

@headers | make_headers.pl --outdir=<dir> [--verbose|-v] [--help|-h]

=head1 OPTIONS

  --outdir=DIR   Specify output directory (required)
  --verbose, -v  Print extra info
  --help, -h     Show this message