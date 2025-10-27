#!/usr/bin/env perl
use strict;
use warnings;
use File::Basename;
use Cwd qw(abs_path);
use File::Path qw(make_path);

# Make all standard handles UTF-8 (so your notes can contain Unicode cleanly)
use open ':std', ':encoding(UTF-8)';

my @headers = <STDIN>;     # grab all lines
chomp @headers;            # remove trailing newlines
@headers = grep { length } @headers;

my $dir    = dirname(abs_path($0));
my $srcdir = "$dir/../src";
my $incdir = "$srcdir/include";
my $docdir = "$srcdir/doc";
my $outdir = "$dir/../out/cvec/include";
make_path($outdir) unless -d $outdir;

# --- NEW: where the param notes live ---
my $key_val_file = "$docdir/key_val.txt";

# --- NEW: loader for "key: value" or "key=value" pairs (UTF-8) ---
sub load_param_notes {
  my ($path) = @_;
  my %h;
  open(my $fh, '<:encoding(UTF-8)', $path) or die "Cannot open $path: $!";
  while (my $line = <$fh>) {
    chomp $line;
    $line =~ s/^\s+|\s+$//g;               # trim
    next if $line eq '' || $line =~ /^[#;]/;

    # key[:=]value — key is non-space up to first ':' or '='
    my ($k, $v) = $line =~ /\A([^:=\s]+)\s*[:=]\s*(.+)\z/ or next;

    # Allow escaped sequences
    $v =~ s/\\n/\n/g;
    $v =~ s/\\t/\t/g;

    $h{$k} = $v;                           # later duplicates overwrite earlier ones
  }
  close $fh;
  return %h;
}
my %PARAM_NOTES = -e $key_val_file ? load_param_notes($key_val_file) : ();
# ---------------------------
# 1) Define filters for doc_str processing
# ---------------------------
my @DOC_FILTERS = (
  sub {  # Append note inline after "@param <name> ..." for any configured name
    my ($s) = @_;
    return $s unless %PARAM_NOTES;

    # Build alternation of param names (longer first; escape metachars)
    my @names = sort { length($b) <=> length($a) } keys %PARAM_NOTES;
    return $s unless @names;
    my $alternation = join '|', map { quotemeta($_) } @names;

    $s =~ s{
      ^([ \t]*\*[ \t]*\@param[ \t]+)   # $1: prefix like " * @param "
      ($alternation)[ \t]*$            # $2: the param name
    }{
      my ($prefix, $name) = ($1, $2);
      my $note = $PARAM_NOTES{$name} // '';
      $note ne '' ? ($prefix . $name . ' ' . $note) : ($prefix . $name)
    }egmx;

    return $s;
  },
);
sub process_doc_str {
  my ($s) = @_;
  for my $f (@DOC_FILTERS) { $s = $f->($s) }
  return $s;
}

HEADER:
for my $h (@headers) {
  my $h_file   = "$incdir/$h";
  my $doc_file = "$docdir/h/${h}doc";
  my $out_file = "$outdir/$h";

  # Load the doc file (skip header if doc is missing)
  open(my $df, '<:encoding(UTF-8)', $doc_file) or do {
    warn "skip: no doc for $h ($doc_file): $!";
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

    #my ($id) = $doc_id =~ m{//\s*(\S+)};
    #$doc_str =~ s{
    #  ^([ \t]*\*[ \t]*\@brief)[ \t]*([^\n]*)
    #}{
    #  "$1 __${id}__: $2"
    #}emx if defined $id;

    $doc_str = process_doc_str($doc_str);
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