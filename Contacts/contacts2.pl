#!/usr/bin/perl
use strict;
use warnings;

my $storage = 'contacts';
mkdir $storage unless -d $storage;

while (1) {
    print "\n---OPTIONS---\n";
    print "1. Add contact\n";
    print "2. List all contacts\n";
    print "3. Delete a contact\n";
    print "4. Search a contact\n";
    print "5. Exit\n";
    print "Enter a choice: ";
    my $choice = <STDIN>;
    chomp $choice;

    if    ($choice == 1) { add_contact() }
    elsif ($choice == 2) { list_contacts() }
    elsif ($choice == 3) { delete_contact() }
    elsif ($choice == 4) { search_contact() }
    elsif ($choice == 5) { print "Exiting.\n"; exit }
    else                 { print "Invalid choice.\n" }
}

sub add_contact {
    print "Enter name: ";
    my $name = <STDIN>; chomp $name;
    return unless $name;

    print "Enter number: ";
    my $number = <STDIN>; chomp $number;

    print "Enter email: ";
    my $email = <STDIN>; chomp $email;

    print "Enter other info: ";
    my $other = <STDIN>; chomp $other;

    # filename is the contact name, spaces replaced with underscores
    (my $filename = $name) =~ s/ /_/g;
    my $path = "$storage/$filename.txt";

    if (-e $path) {
        print "Contact '$name' already exists.\n";
        return;
    }

    open(my $fh, '>', $path) or die "Cannot create contact: $!";
    print $fh "name=$name\n";
    print $fh "number=$number\n";
    print $fh "email=$email\n";
    print $fh "other=$other\n";
    close($fh);

    print "Contact '$name' saved.\n";
}

sub list_contacts {
    my @files = glob("$storage/*.txt");

    if (!@files) {
        print "No contacts found.\n";
        return;
    }

    print "\n--- ALL CONTACTS ---\n";
    for my $file (@files) {
        open(my $fh, '<', $file) or next;
        my %data;
        while (<$fh>) {
            chomp;
            my ($key, $val) = split(/=/, $_, 2);
            $data{$key} = $val;
        }
        close($fh);
        print "Name: $data{name} | Number: $data{number} | Email: $data{email} | Other: $data{other}\n";
    }
}

sub delete_contact {
    print "Enter name to delete: ";
    my $name = <STDIN>; chomp $name;

    (my $filename = $name) =~ s/ /_/g;
    my $path = "$storage/$filename.txt";

    if (!-e $path) {
        print "Contact '$name' not found.\n";
        return;
    }

    unlink $path;
    print "Contact '$name' deleted.\n";
}

sub search_contact {
    print "Enter name to search: ";
    my $keyword = <STDIN>; chomp $keyword;

    my @files = glob("$storage/*.txt");
    my $found = 0;

    for my $file (@files) {
        open(my $fh, '<', $file) or next;
        my %data;
        while (<$fh>) {
            chomp;
            my ($key, $val) = split(/=/, $_, 2);
            $data{$key} = $val;
        }
        close($fh);

        if ($data{name} =~ /\Q$keyword\E/i) {
            print "Name: $data{name} | Number: $data{number} | Email: $data{email} | Other: $data{other}\n";
            $found++;
        }
    }

    print "No contacts found matching '$keyword'.\n" unless $found;
}
