with Ada.Text_IO;           -- gives you Put_Line for printing
with Ada.Directories;       -- gives you Exists to check files and folders
with Ada.Command_Line;      -- gives you Set_Exit_Status to return pass/fail to Perl

procedure Integrity_Check is

   -- base path where the ecosystem lives
   -- Ada requires all variables declared before begin
   DB_Path       : constant String := "DB/data/database.db";
   PDF_Store     : constant String := "DB/pdf_store";
   Contacts_Dir  : constant String := "Contacts";
   Photos_Dir    : constant String := "Photos";

   All_OK : Boolean := True;

begin
   Ada.Text_IO.Put_Line ("--- Integrity Check ---");

   -- check database file
   if Ada.Directories.Exists (DB_Path) then
      Ada.Text_IO.Put_Line ("OK: database found");
   else
      Ada.Text_IO.Put_Line ("FAIL: database missing - " & DB_Path);
      All_OK := False;
   end if;

   -- check pdf_store folder
   if Ada.Directories.Exists (PDF_Store) then
      Ada.Text_IO.Put_Line ("OK: pdf_store found");
   else
      Ada.Text_IO.Put_Line ("FAIL: pdf_store missing - " & PDF_Store);
      All_OK := False;
   end if;

   -- check contacts folder
   if Ada.Directories.Exists (Contacts_Dir) then
      Ada.Text_IO.Put_Line ("OK: contacts folder found");
   else
      Ada.Text_IO.Put_Line ("FAIL: contacts folder missing - " & Contacts_Dir);
      All_OK := False;
   end if;

   -- check photos folder
   if Ada.Directories.Exists (Photos_Dir) then
      Ada.Text_IO.Put_Line ("OK: photos folder found");
   else
      Ada.Text_IO.Put_Line ("FAIL: photos folder missing - " & Photos_Dir);
      All_OK := False;
   end if;

   -- report final result and set exit code for Perl launcher
   if All_OK then
      Ada.Text_IO.Put_Line ("Integrity check passed.");
      Ada.Command_Line.Set_Exit_Status (0);   -- 0 = success, Perl sees this
   else
      Ada.Text_IO.Put_Line ("Integrity check FAILED.");
      Ada.Command_Line.Set_Exit_Status (1);   -- 1 = failure, Perl launcher catches this
   end if;

end Integrity_Check;
