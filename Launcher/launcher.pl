use strict;
use warnings;

use Cwd 'abs_path';
use File::Basename;

my $base = dirname(abs_path($0));

print "Running integrity check...\n";
my $integrity_result = system("$base/integrity/integrity_check");

if ($integrity_result != 0) {
    print "Integrity check failed. Ecosystem may be corrupted.\n";
    print "Proceed anyway? (y/n): ";
    my $answer = <STDIN>;
    chomp $answer;
    exit if $answer ne 'y';
}

print "Integrity check passed.\n\n";

while (1) {

print "--- research_ecosystem ---\n";
    print "1. DB\n";
    print "2. Photos\n";
    print "3. Contacts\n";
    print "4. Radio\n";
    print "5. Exit\n";
    print "Choose: ";

my $choice = <STDIN>;
    chomp $choice;

if    ($choice == 1) { system("$base/DB/CastorimorphaDB") }
    elsif ($choice == 2) { system("perl $base/Photos/Photos.pl") }
    elsif ($choice == 3) { system("perl $base/Contacts/contacts2.pl") }
    elsif ($choice == 4) { system("cd $base/radio_go_app && go run radio.go") }
    elsif ($choice == 5) { print "Exiting.\n"; exit }
    else                 { print "Invalid choice.\n" }

}
