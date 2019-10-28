#!/usr/bin/env perl
#
# Perl script to generate the Arduino IDE keywords.txt file
# from the output of a Doxygen Tag File (GENERATE_TAGFILE)
#
# Copyright (c) 2018, Nicholas Humfrey
# License: The 3-Clause BSD License
#

use XML::XPath;
use XML::XPath::XMLParser;

my $inputfile = $ARGV[0] || die "Usage: generate-keywords.pl tags.xml\n";
my $xp = XML::XPath->new(filename => $inputfile);

# We use hashes as a way of maintaining a unique list
my %classes = ();
my %functions = ();
my %constants = ();


my $compounds = $xp->find("/tagfile/compound");
foreach my $compound ($compounds->get_nodelist) {
  my $kind = $compound->getAttribute('kind');
  if ($kind eq 'file') {
    # Ignore the example files
    next if ($xp->find('./path', $compound)->string_value =~ /\/examples\//);

    my $classNodes = $xp->find("./class", $compound);
    foreach my $class ($classNodes->get_nodelist) {
      if ($class->getAttribute('kind') eq 'class') {
          $classes{$class->string_value} = 1;
      }
    }

    my $memberNodes = $xp->find("./member", $compound);
    foreach my $member ($memberNodes->get_nodelist) {
      if ($member->getAttribute('kind') eq 'define') {
        my $name = $xp->find('./name', $member)->string_value;
        $constants{$name} = 1;
      }
    }
  } elsif ($kind eq 'class') {
    my $name = $xp->find('./name', $compound)->string_value;
    next unless (exists $classes{$name});

    my $memberNodes = $xp->find("./member", $compound);
    foreach my $member ($memberNodes->get_nodelist) {
      my $kind = $member->getAttribute('kind');
      my $name = $xp->find('./name', $member)->string_value;
      my $type = $xp->find('type', $member)->string_value;
      if ($kind eq 'function') {
        next if $name =~ /^operator\W/;
        next if exists $classes{$name};
        $functions{$name} = 1;
      } elsif ($kind eq 'variable' and $type =~ /^const /) {
        $constants{$name} = 1;
      }
    }
  }
}

print "#######################################\n";
print "# Syntax Coloring Map\n";
print "#######################################\n";
print "\n";

print "#######################################\n";
print "# Datatypes (KEYWORD1)\n";
print "#######################################\n";

foreach my $class (sort keys %classes) {
  print "$class\tKEYWORD1\n";
}

print "\n\n";
print "#######################################\n";
print "# Methods and Functions (KEYWORD2)\n";
print "#######################################\n";

foreach my $function (sort keys %functions) {
  print "$function\tKEYWORD2\n";
}

print "\n\n";
print "#######################################\n";
print "# Constants (LITERAL1)\n";
print "#######################################\n";

foreach my $constant (sort keys %constants) {
  print "$constant\tLITERAL1\n";
}

print "\n"
