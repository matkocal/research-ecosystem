use strict;
use warnings;
use File::Copy;

my $storage = 'photos';
mkdir $storage unless -d $storage;
 

opendir(my $dh, $storage) or die "Cannot open dir $!\n";
my @files = grep { -f "$storage/$_" } readdir($dh);

closedir($dh);

my @starred;



sub add_photos {

print "Enter file name to add: ";
my $name = <STDIN>;
chomp $name;
die "File not found: " unless -f $name;
copy ($name, "$storage/$name") or die "could not copy: ";
push @files, $name;
print "file was saved\n";
print"\n";

}

sub list_starred {

print "\nStarred:\n";
print "$_" for @starred;
print"\n";

}


while (1) {
print "---OPTIONS---\n";
print "0. Add a file\n";
print "1. List all files\n";
print "2. Open  a file\n";
print "3. Delete a file\n";
print "4. Rename a file\n";
print "5. Star a file\n";
print "6. List starred files\n";
print "7. Exit\n";

print "Enter choice: ";

my $choice = <STDIN>;

chomp $choice;

add_photos() if $choice == 0;
list_photos() if $choice == 1;
delete_photo() if $choice == 3;
rename_photo() if $choice == 4;
open_photo() if $choice == 2;
star_photo() if $choice == 5;
list_starred() if $choice == 6;
last if $choice == 7;
}

sub list_photos {

print "\nGALLERY:\n";
print "$_\n" for @files;
print "\n";

}


sub open_photo {

print "Enter file name to open: ";
my $name = <STDIN>;
chomp $name;
if ($^O eq 'darwin') {
system("open '$name'");

} else {
system("xdg-open '$name'");
}
}

sub delete_photo {

print "Enter file name to delete: ";
my $name = <STDIN>;
chomp $name;
unlink "$storage/$name" or die "Could not delete :(";
@files = grep { $_ ne $name } @files;
print "\n";

}




sub rename_photo {
print "Enter file to rename: ";
my $old  = <STDIN>;
chomp $old;
print "Enter new name: ";
my $new = <STDIN>;
chomp $new;
rename "$storage/$old", "$storage/$new" or die "Could not rename :(";


}



sub star_photo {

print "Enter a file to star: ";
my $name = <STDIN>;
chomp $name;
push @starred, $name;
print "$name has been starred\n";
print"\n";

}

