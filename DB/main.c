
#include <stdio.h>    /* Includes standard input/output — gives you printf, fprintf, fgets, fopen, fclose.*/
#include <stdlib.h>   /* Includes standard library — gives you exit(), memory functions like malloc.*/
#include <string.h>   /* Includes string functions — gives you strcmp, strcpy, strlen, strrchr, strchr, strcspn, snprintf.*/
#include <sys/stat.h> /* Includes system file info — gives you stat() (to get file size) and mkdir() (to create directories). */
#include <time.h>     /* Includes time functions — gives you time(), strftime(), localtime() to get the current date and time */
#include <unistd.h>   /*Includes Unix system calls — gives you access() (check if file exists) and unlink() (delete a file).*/
#include <sqlite3.h>  /* Includes the SQLite database library — gives you all the sqlite3_* functions to work with the database.*/

#define STORE_DIR "pdf_store/" /* define creates a constant. Everywhere the code writes STORE_DIR, the compiler replaces it with "pdf_store/". \
 It's the folder where files are physically stored. */

sqlite3 *db; /* Declares a global variable called db. It's a pointer to the database connection. It's global so every function in the program can access it without passing it around.*/

/* ---------------- FUNCTION PROTOTYPES ---------------- */
/* These are function prototypes.
They tell the compiler "this function exists and here is what it takes and returns"
 before the actual function is defined below. This lets functions call each other in any order.*/
void init_database(void);
void ensure_dir(const char *path);
int copy_file(const char *src, const char *dst);
void add_pdf(const char *path, const char *group);
void create_group(const char *group_name);
void list_files(void);
void list_groups(void);
void list_group_files(const char *group);
void delete_file(const char *filename);
void delete_group(const char *group);
void remove_file_from_group(const char *filename, const char *group);
void add_file_to_another_group(const char *filename, const char *group);
void link_group_to_parent(const char *child_group, const char *parent_group);
void unlink_group_from_parent(const char *child_group, const char *parent_group);
void search_files(const char *keyword);
void star_file(const char *filename);
void unstar_file(const char *filename);
void list_starred_files(void);
void rename_file(const char *old_name, const char *new_name);
void show_file_info(const char *filename);
void list_recent_files(void);
void open_file(const char *filename);
void show_group_hierarchy(int group_id, const char *group_name, int indent_level);
void show_all_groups_with_files(void);
void print_intro(void);
void interactive_shell(void);
long file_size(const char *path);

/* ---------------- DATABASE FUNCTIONS ---------------- */

void init_database(void) /*Defines the init_database function. void means it returns nothing.
void inside the () means it takes no arguments.*/
{
    ensure_dir("data"); /* Calls ensure_dir to create the data/ folder if it doesn't exist yet,
    because the database file will live there.*/

    if (sqlite3_open("data/database.db", &db)) /*Tries to open (or create if it doesn't exist) the database file. &db passes the address of the global db variable so SQLite can fill it in.
    < If it fails (returns non-zero), the if triggers.*/
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        exit(1);
    }

    char *err_msg = NULL;
    const char *sql =
        "CREATE TABLE IF NOT EXISTS files ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT UNIQUE NOT NULL,"
        "size INTEGER,"
        "created_at TEXT,"
        "starred INTEGER DEFAULT 0"
        ");"
        "CREATE TABLE IF NOT EXISTS groups ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT UNIQUE NOT NULL"
        ");"
        "CREATE TABLE IF NOT EXISTS group_hierarchy ("
        "child_id INTEGER,"
        "parent_id INTEGER,"
        "PRIMARY KEY(child_id, parent_id),"
        "FOREIGN KEY(child_id) REFERENCES groups(id) ON DELETE CASCADE,"
        "FOREIGN KEY(parent_id) REFERENCES groups(id) ON DELETE CASCADE"
        ");"
        "CREATE TABLE IF NOT EXISTS file_groups ("
        "file_id INTEGER,"
        "group_id INTEGER,"
        "PRIMARY KEY(file_id, group_id),"
        "FOREIGN KEY(file_id) REFERENCES files(id) ON DELETE CASCADE,"
        "FOREIGN KEY(group_id) REFERENCES groups(id) ON DELETE CASCADE"
        ");";

    if (sqlite3_exec(db, sql, 0, 0, &err_msg) != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        exit(1);
    }
}

int get_file_id(const char *filename)
{
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id FROM files WHERE name = ?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK)
        return -1;

    sqlite3_bind_text(stmt, 1, filename, -1, SQLITE_STATIC);

    int file_id = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW)
        file_id = sqlite3_column_int(stmt, 0);

    sqlite3_finalize(stmt);
    return file_id;
}

int get_group_id(const char *group_name)
{
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id FROM groups WHERE name = ?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK)
        return -1;

    sqlite3_bind_text(stmt, 1, group_name, -1, SQLITE_STATIC);

    int group_id = -1;
    if (sqlite3_step(stmt) == SQLITE_ROW)
        group_id = sqlite3_column_int(stmt, 0);

    sqlite3_finalize(stmt);
    return group_id;
}

/* ---------------- UTILITIES ---------------- */

void print_intro(void)
{
    printf("\n");
    printf("------------ Research Ecosystem ----------------\n");
}

long file_size(const char *path)
{
    struct stat st;
    if (stat(path, &st) == 0)
        return st.st_size;
    return -1;
}

void ensure_dir(const char *path)
{
    char tmp[512];
    snprintf(tmp, sizeof(tmp), "%s", path);

    for (char *p = tmp + 1; *p; p++)
    {
        if (*p == '/')
        {
            *p = 0;
            mkdir(tmp, 0755);
            *p = '/';
        }
    }
    mkdir(tmp, 0755);
}

int copy_file(const char *src, const char *dst)
{
    FILE *in = fopen(src, "rb");
    if (!in)
    {
        printf("Error: Cannot open source file: %s\n", src);
        return 0;
    }

    FILE *out = fopen(dst, "wb");
    if (!out)
    {
        printf("Error: Cannot create destination file: %s\n", dst);
        fclose(in);
        return 0;
    }

    char buf[4096];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), in)) > 0)
        fwrite(buf, 1, n, out);

    fclose(in);
    fclose(out);
    return 1;
}

void open_file(const char *filename)
{
    char store_path[512];
    snprintf(store_path, sizeof(store_path), STORE_DIR "%s", filename);

    if (access(store_path, F_OK) != 0)
    {
        printf("File not found in store.\n");
        return;
    }

    printf("Opening: %s\n", filename);

    char command[1024];
    int result = -1;

#ifdef __APPLE__
    snprintf(command, sizeof(command), "open \"%s\"", store_path);
    result = system(command);
#elif __linux__
    // Try xdg-open first
    snprintf(command, sizeof(command), "xdg-open \"%s\" >/dev/null 2>&1 &", store_path);
    result = system(command);

    if (result != 0)
    {
        // Try evince
        snprintf(command, sizeof(command), "evince \"%s\" >/dev/null 2>&1 &", store_path);
        result = system(command);
    }

    if (result != 0)
    {
        // Try okular
        snprintf(command, sizeof(command), "okular \"%s\" >/dev/null 2>&1 &", store_path);
        result = system(command);
    }

    if (result != 0)
    {
        // Try firefox as last resort
        snprintf(command, sizeof(command), "firefox \"%s\" >/dev/null 2>&1 &", store_path);
        result = system(command);
    }

    if (result != 0)
    {
        printf("Could not open file. Please install: xdg-utils, evince, okular, or firefox\n");
        printf("Or open manually: %s\n", store_path);
    }
#else
    snprintf(command, sizeof(command), "start \"%s\"", store_path);
    result = system(command);
#endif
}

/* ---------------- CORE FUNCTIONS ---------------- */

void add_pdf(const char *path, const char *group_name)
{
    // Check if source file exists
    if (access(path, F_OK) != 0)
    {
        printf("Error: Source file not found: %s\n", path);
        printf("Hint: Use tab completion or check the path\n");
        return;
    }

    // Extract filename
    const char *filename = strrchr(path, '/');
    filename = filename ? filename + 1 : path;

    ensure_dir(STORE_DIR);

    // Copy file to store
    char stored_path[512];
    snprintf(stored_path, sizeof(stored_path), STORE_DIR "%s", filename);

    if (access(stored_path, F_OK) != 0)
    {
        if (!copy_file(path, stored_path))
        {
            printf("Error: Failed to copy file to store\n");
            return;
        }
        printf("Copied to store: %s\n", filename);
    }
    else
    {
        printf("File already in store: %s\n", filename);
    }

    // Get size from stored file
    long size = file_size(stored_path);
    if (size == -1)
    {
        printf("Error: Failed to get file size\n");
        return;
    }

    time_t now = time(NULL);
    char time_str[64];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));

    // Insert file into database
    sqlite3_stmt *stmt;
    const char *sql = "INSERT OR IGNORE INTO files (name, size, created_at) VALUES (?, ?, ?)";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK)
    {
        sqlite3_bind_text(stmt, 1, filename, -1, SQLITE_STATIC);
        sqlite3_bind_int64(stmt, 2, size);
        sqlite3_bind_text(stmt, 3, time_str, -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE && sqlite3_changes(db) > 0)
            printf("Added to database: %s (%ld bytes)\n", filename, size);
        else
            printf("Already in database: %s\n", filename);

        sqlite3_finalize(stmt);
    }

    // Create group if it doesn't exist
    create_group(group_name);

    // Link file to group
    int file_id = get_file_id(filename);
    int group_id = get_group_id(group_name);

    if (file_id != -1 && group_id != -1)
    {
        const char *link_sql = "INSERT OR IGNORE INTO file_groups (file_id, group_id) VALUES (?, ?)";
        if (sqlite3_prepare_v2(db, link_sql, -1, &stmt, 0) == SQLITE_OK)
        {
            sqlite3_bind_int(stmt, 1, file_id);
            sqlite3_bind_int(stmt, 2, group_id);

            if (sqlite3_step(stmt) == SQLITE_DONE && sqlite3_changes(db) > 0)
                printf("Linked to group: %s\n", group_name);
            else
                printf("Already linked to group: %s\n", group_name);

            sqlite3_finalize(stmt);
        }
    }
}

void create_group(const char *group_name)
{
    sqlite3_stmt *stmt;
    const char *sql = "INSERT OR IGNORE INTO groups (name) VALUES (?)";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK)
    {
        sqlite3_bind_text(stmt, 1, group_name, -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE && sqlite3_changes(db) > 0)
            printf("Created group: %s\n", group_name);

        sqlite3_finalize(stmt);
    }
}

void list_files(void)
{
    const char *sql = "SELECT id, name, size, created_at FROM files ORDER BY created_at DESC";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK)
    {
        fprintf(stderr, "Failed to fetch files\n");
        return;
    }

    printf("\n=== ALL FILES ===\n");
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        int id = sqlite3_column_int(stmt, 0);
        const char *name = (const char *)sqlite3_column_text(stmt, 1);
        long size = sqlite3_column_int64(stmt, 2);
        const char *created = (const char *)sqlite3_column_text(stmt, 3);

        printf("%d. %s (%ld bytes) - %s\n", id, name, size, created);
        count++;
    }

    if (count == 0)
        printf("No files in database.\n");

    sqlite3_finalize(stmt);
}

void list_groups(void)
{
    const char *sql = "SELECT id, name FROM groups ORDER BY name";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK)
    {
        fprintf(stderr, "Failed to fetch groups\n");
        return;
    }

    printf("\n=== ALL GROUPS ===\n");
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        int id = sqlite3_column_int(stmt, 0);
        const char *name = (const char *)sqlite3_column_text(stmt, 1);

        printf("%d. %s\n", id, name);
        count++;
    }

    if (count == 0)
        printf("No groups in database.\n");

    sqlite3_finalize(stmt);
}

void list_group_files(const char *group)
{
    const char *sql =
        "SELECT f.name, f.size, f.created_at "
        "FROM files f "
        "JOIN file_groups fg ON f.id = fg.file_id "
        "JOIN groups g ON fg.group_id = g.id "
        "WHERE g.name = ? "
        "ORDER BY f.name";

    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK)
    {
        fprintf(stderr, "Failed to fetch files in group\n");
        return;
    }

    sqlite3_bind_text(stmt, 1, group, -1, SQLITE_STATIC);

    printf("\n=== FILES IN GROUP: %s ===\n", group);
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const char *name = (const char *)sqlite3_column_text(stmt, 0);
        long size = sqlite3_column_int64(stmt, 1);
        const char *created = (const char *)sqlite3_column_text(stmt, 2);

        printf("• %s (%ld bytes) - %s\n", name, size, created);
        count++;
    }

    if (count == 0)
        printf("No files in this group.\n");

    sqlite3_finalize(stmt);
}

void show_group_hierarchy(int group_id, const char *group_name, int indent_level)
{
    // Print group with indentation
    for (int i = 0; i < indent_level; i++)
        printf("  ");
    printf("📁 %s\n", group_name);

    // Print files in this group
    const char *files_sql =
        "SELECT f.name FROM files f "
        "JOIN file_groups fg ON f.id = fg.file_id "
        "WHERE fg.group_id = ? ORDER BY f.name";

    sqlite3_stmt *files_stmt;
    if (sqlite3_prepare_v2(db, files_sql, -1, &files_stmt, 0) == SQLITE_OK)
    {
        sqlite3_bind_int(files_stmt, 1, group_id);

        while (sqlite3_step(files_stmt) == SQLITE_ROW)
        {
            const char *file_name = (const char *)sqlite3_column_text(files_stmt, 0);
            for (int i = 0; i < indent_level + 1; i++)
                printf("  ");
            printf("• %s\n", file_name);
        }

        sqlite3_finalize(files_stmt);
    }

    // Find and display child groups
    const char *child_sql =
        "SELECT g.id, g.name FROM groups g "
        "JOIN group_hierarchy gh ON g.id = gh.child_id "
        "WHERE gh.parent_id = ? ORDER BY g.name";
    sqlite3_stmt *child_stmt;

    if (sqlite3_prepare_v2(db, child_sql, -1, &child_stmt, 0) == SQLITE_OK)
    {
        sqlite3_bind_int(child_stmt, 1, group_id);

        while (sqlite3_step(child_stmt) == SQLITE_ROW)
        {
            int child_id = sqlite3_column_int(child_stmt, 0);
            const char *child_name = (const char *)sqlite3_column_text(child_stmt, 1);
            show_group_hierarchy(child_id, child_name, indent_level + 1);
        }

        sqlite3_finalize(child_stmt);
    }
}

void show_all_groups_with_files(void)
{
    printf("\n========================================\n");
    printf("         ALL GROUPS AND FILES          \n");
    printf("========================================\n\n");

    // Show only top-level groups (those not children of any other group)
    const char *sql =
        "SELECT id, name FROM groups "
        "WHERE id NOT IN (SELECT child_id FROM group_hierarchy) "
        "ORDER BY name";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK)
    {
        fprintf(stderr, "Failed to fetch groups\n");
        return;
    }

    int group_count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        int group_id = sqlite3_column_int(stmt, 0);
        const char *group_name = (const char *)sqlite3_column_text(stmt, 1);

        show_group_hierarchy(group_id, group_name, 0);
        printf("\n");
        group_count++;
    }

    if (group_count == 0)
        printf("No groups created yet.\n");
    else
    {
        // Count all groups including nested ones
        const char *count_sql = "SELECT COUNT(*) FROM groups";
        sqlite3_stmt *count_stmt;
        if (sqlite3_prepare_v2(db, count_sql, -1, &count_stmt, 0) == SQLITE_OK)
        {
            if (sqlite3_step(count_stmt) == SQLITE_ROW)
            {
                int total = sqlite3_column_int(count_stmt, 0);
                printf("Total groups: %d\n", total);
            }
            sqlite3_finalize(count_stmt);
        }
    }

    printf("========================================\n");
    sqlite3_finalize(stmt);
}

void delete_file(const char *filename)
{
    int file_id = get_file_id(filename);
    if (file_id == -1)
    {
        printf("File not found in database.\n");
        return;
    }

    // Delete from database (CASCADE will handle file_groups)
    sqlite3_stmt *stmt;
    const char *sql = "DELETE FROM files WHERE name = ?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK)
    {
        sqlite3_bind_text(stmt, 1, filename, -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE)
        {
            // Delete physical file
            char store_path[512];
            snprintf(store_path, sizeof(store_path), STORE_DIR "%s", filename);
            unlink(store_path);

            printf("Deleted file: %s\n", filename);
        }

        sqlite3_finalize(stmt);
    }
}

void delete_group(const char *group)
{
    int group_id = get_group_id(group);
    if (group_id == -1)
    {
        printf("Group not found.\n");
        return;
    }

    // Delete from database (CASCADE will handle file_groups)
    sqlite3_stmt *stmt;
    const char *sql = "DELETE FROM groups WHERE name = ?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK)
    {
        sqlite3_bind_text(stmt, 1, group, -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE)
            printf("Deleted group: %s\n", group);

        sqlite3_finalize(stmt);
    }
}

void remove_file_from_group(const char *filename, const char *group)
{
    int file_id = get_file_id(filename);
    int group_id = get_group_id(group);

    if (file_id == -1 || group_id == -1)
    {
        printf("File or group not found.\n");
        return;
    }

    sqlite3_stmt *stmt;
    const char *sql = "DELETE FROM file_groups WHERE file_id = ? AND group_id = ?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, file_id);
        sqlite3_bind_int(stmt, 2, group_id);

        if (sqlite3_step(stmt) == SQLITE_DONE && sqlite3_changes(db) > 0)
            printf("Removed %s from group %s\n", filename, group);
        else
            printf("File was not in that group.\n");

        sqlite3_finalize(stmt);
    }
}

void add_file_to_another_group(const char *filename, const char *group)
{
    int file_id = get_file_id(filename);
    int group_id = get_group_id(group);

    if (file_id == -1 || group_id == -1)
    {
        printf("File or group not found.\n");
        return;
    }

    sqlite3_stmt *stmt;
    const char *sql = "INSERT OR IGNORE INTO file_groups (file_id, group_id) VALUES (?, ?)";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, file_id);
        sqlite3_bind_int(stmt, 2, group_id);

        if (sqlite3_step(stmt) == SQLITE_DONE && sqlite3_changes(db) > 0)
            printf("Added %s to group %s\n", filename, group);
        else
            printf("Already in this group.\n");

        sqlite3_finalize(stmt);
    }
}

void link_group_to_parent(const char *child_group, const char *parent_group)
{
    int child_id = get_group_id(child_group);
    int parent_id = get_group_id(parent_group);

    if (child_id == -1)
    {
        printf("Child group not found.\n");
        return;
    }

    if (parent_id == -1)
    {
        printf("Parent group not found.\n");
        return;
    }

    // Check if trying to link group to itself
    if (child_id == parent_id)
    {
        printf("Cannot link a group to itself.\n");
        return;
    }

    // Add to group_hierarchy
    sqlite3_stmt *stmt;
    const char *sql = "INSERT OR IGNORE INTO group_hierarchy (child_id, parent_id) VALUES (?, ?)";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, child_id);
        sqlite3_bind_int(stmt, 2, parent_id);

        if (sqlite3_step(stmt) == SQLITE_DONE && sqlite3_changes(db) > 0)
            printf("Linked '%s' to parent '%s'\n", child_group, parent_group);
        else
            printf("Already linked or failed to link.\n");

        sqlite3_finalize(stmt);
    }
}

void unlink_group_from_parent(const char *child_group, const char *parent_group)
{
    int child_id = get_group_id(child_group);
    int parent_id = get_group_id(parent_group);

    if (child_id == -1 || parent_id == -1)
    {
        printf("Group not found.\n");
        return;
    }

    sqlite3_stmt *stmt;
    const char *sql = "DELETE FROM group_hierarchy WHERE child_id = ? AND parent_id = ?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, child_id);
        sqlite3_bind_int(stmt, 2, parent_id);

        if (sqlite3_step(stmt) == SQLITE_DONE && sqlite3_changes(db) > 0)
            printf("Unlinked '%s' from parent '%s'\n", child_group, parent_group);
        else
            printf("Not linked or failed to unlink.\n");

        sqlite3_finalize(stmt);
    }
}

void search_files(const char *keyword)
{
    const char *sql =
        "SELECT f.id, f.name, f.size, f.created_at, f.starred "
        "FROM files f "
        "WHERE f.name LIKE ? "
        "ORDER BY f.starred DESC, f.name";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK)
    {
        fprintf(stderr, "Failed to search files\n");
        return;
    }

    char pattern[256];
    snprintf(pattern, sizeof(pattern), "%%%s%%", keyword);
    sqlite3_bind_text(stmt, 1, pattern, -1, SQLITE_STATIC);

    printf("\n=== SEARCH RESULTS for '%s' ===\n", keyword);
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const char *name = (const char *)sqlite3_column_text(stmt, 1);
        long size = sqlite3_column_int64(stmt, 2);
        const char *created = (const char *)sqlite3_column_text(stmt, 3);
        int starred = sqlite3_column_int(stmt, 4);

        printf("%s %s (%ld bytes) - %s\n",
               starred ? "⭐" : "  ", name, size, created);
        count++;
    }

    if (count == 0)
        printf("No files found matching '%s'\n", keyword);
    else
        printf("\nFound %d file(s)\n", count);

    sqlite3_finalize(stmt);
}

void star_file(const char *filename)
{
    int file_id = get_file_id(filename);
    if (file_id == -1)
    {
        printf("File not found.\n");
        return;
    }

    sqlite3_stmt *stmt;
    const char *sql = "UPDATE files SET starred = 1 WHERE id = ?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, file_id);

        if (sqlite3_step(stmt) == SQLITE_DONE)
            printf("⭐ Starred: %s\n", filename);

        sqlite3_finalize(stmt);
    }
}

void unstar_file(const char *filename)
{
    int file_id = get_file_id(filename);
    if (file_id == -1)
    {
        printf("File not found.\n");
        return;
    }

    sqlite3_stmt *stmt;
    const char *sql = "UPDATE files SET starred = 0 WHERE id = ?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, file_id);

        if (sqlite3_step(stmt) == SQLITE_DONE)
            printf("Unstarred: %s\n", filename);

        sqlite3_finalize(stmt);
    }
}

void list_starred_files(void)
{
    const char *sql =
        "SELECT name, size, created_at FROM files "
        "WHERE starred = 1 "
        "ORDER BY name";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK)
    {
        fprintf(stderr, "Failed to fetch starred files\n");
        return;
    }

    printf("\n=== STARRED FILES ===\n");
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const char *name = (const char *)sqlite3_column_text(stmt, 0);
        long size = sqlite3_column_int64(stmt, 1);
        const char *created = (const char *)sqlite3_column_text(stmt, 2);

        printf("⭐ %s (%ld bytes) - %s\n", name, size, created);
        count++;
    }

    if (count == 0)
        printf("No starred files.\n");

    sqlite3_finalize(stmt);
}

void rename_file(const char *old_name, const char *new_name)
{
    int file_id = get_file_id(old_name);
    if (file_id == -1)
    {
        printf("File not found: %s\n", old_name);
        return;
    }

    // Check if new name already exists
    if (get_file_id(new_name) != -1)
    {
        printf("Error: A file with name '%s' already exists\n", new_name);
        return;
    }

    // Rename in database
    sqlite3_stmt *stmt;
    const char *sql = "UPDATE files SET name = ? WHERE id = ?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK)
    {
        sqlite3_bind_text(stmt, 1, new_name, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, file_id);

        if (sqlite3_step(stmt) == SQLITE_DONE)
        {
            // Rename physical file
            char old_path[512], new_path[512];
            snprintf(old_path, sizeof(old_path), STORE_DIR "%s", old_name);
            snprintf(new_path, sizeof(new_path), STORE_DIR "%s", new_name);

            if (rename(old_path, new_path) == 0)
                printf("Renamed '%s' to '%s'\n", old_name, new_name);
            else
                printf("Database updated but failed to rename physical file\n");
        }

        sqlite3_finalize(stmt);
    }
}

void show_file_info(const char *filename)
{
    int file_id = get_file_id(filename);
    if (file_id == -1)
    {
        printf("File not found: %s\n", filename);
        return;
    }

    // Get file info
    const char *file_sql = "SELECT name, size, created_at, starred FROM files WHERE id = ?";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, file_sql, -1, &stmt, 0) != SQLITE_OK)
    {
        fprintf(stderr, "Failed to fetch file info\n");
        return;
    }

    sqlite3_bind_int(stmt, 1, file_id);

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const char *name = (const char *)sqlite3_column_text(stmt, 0);
        long size = sqlite3_column_int64(stmt, 1);
        const char *created = (const char *)sqlite3_column_text(stmt, 2);
        int starred = sqlite3_column_int(stmt, 3);

        printf("\n========================================\n");
        printf("File: %s\n", name);
        printf("========================================\n");
        printf("Size: %ld bytes (%.2f MB)\n", size, size / 1024.0 / 1024.0);
        printf("Created: %s\n", created);
        printf("Starred: %s\n", starred ? "Yes ⭐" : "No");

        char store_path[512];
        snprintf(store_path, sizeof(store_path), STORE_DIR "%s", name);
        printf("Path: %s\n", store_path);

        sqlite3_finalize(stmt);

        // Get groups this file belongs to
        const char *groups_sql =
            "SELECT g.name FROM groups g "
            "JOIN file_groups fg ON g.id = fg.group_id "
            "WHERE fg.file_id = ? ORDER BY g.name";

        if (sqlite3_prepare_v2(db, groups_sql, -1, &stmt, 0) == SQLITE_OK)
        {
            sqlite3_bind_int(stmt, 1, file_id);

            printf("\nGroups:\n");
            int group_count = 0;
            while (sqlite3_step(stmt) == SQLITE_ROW)
            {
                const char *group_name = (const char *)sqlite3_column_text(stmt, 0);
                printf("  • %s\n", group_name);
                group_count++;
            }

            if (group_count == 0)
                printf("  (not in any group)\n");

            sqlite3_finalize(stmt);
        }

        printf("========================================\n");
    }
}

void list_recent_files(void)
{
    const char *sql =
        "SELECT name, size, created_at, starred FROM files "
        "ORDER BY created_at DESC LIMIT 10";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) != SQLITE_OK)
    {
        fprintf(stderr, "Failed to fetch recent files\n");
        return;
    }

    printf("\n=== RECENT FILES (Last 10) ===\n");
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const char *name = (const char *)sqlite3_column_text(stmt, 0);
        long size = sqlite3_column_int64(stmt, 1);
        const char *created = (const char *)sqlite3_column_text(stmt, 2);
        int starred = sqlite3_column_int(stmt, 3);

        printf("%s %s (%ld bytes) - %s\n",
               starred ? "⭐" : "  ", name, size, created);
        count++;
    }

    if (count == 0)
        printf("No files in database.\n");

    sqlite3_finalize(stmt);
}

/* ---------------- INTERACTIVE SHELL ---------------- */

void interactive_shell(void)
{
    char input[1024];

    while (1)
    {
        printf("\n> ");
        if (!fgets(input, sizeof(input), stdin))
            break;

        input[strcspn(input, "\n")] = 0;
        if (strlen(input) == 0)
            continue;

        char *argv[10];
        int argc = 0;

        // Parse command (first word)
        char *p = input;
        while (*p == ' ')
            p++; // skip leading spaces
        argv[argc++] = p;
        while (*p && *p != ' ')
            p++;
        if (*p)
            *p++ = 0;

        // For commands that need the rest as one argument (like open with spaces)
        while (*p == ' ')
            p++; // skip spaces
        if (*p && argc < 10)
        {
            argv[argc++] = p; // Rest of line as one argument
        }

        if (strcmp(argv[0], "exit") == 0)
            break;
        else if (strcmp(argv[0], "add") == 0 && argc >= 2)
        {
            // For add: split on LAST space - everything before is path, last word is group
            char *rest = argv[1];
            char *last_space = strrchr(rest, ' ');

            if (last_space)
            {
                *last_space = 0;
                char *path = rest;
                char *group = last_space + 1;
                while (*group == ' ')
                    group++;

                if (strlen(path) > 0 && strlen(group) > 0)
                    add_pdf(path, group);
                else
                    printf("Usage: add <path> <group>\n");
            }
            else
                printf("Usage: add <path> <group>\n");
        }
        else if (strcmp(argv[0], "mkdir") == 0 && argc == 2)
            create_group(argv[1]);
        else if (strcmp(argv[0], "list") == 0)
            list_files();
        else if (strcmp(argv[0], "listgroups") == 0)
            list_groups();
        else if (strcmp(argv[0], "listgroup") == 0 && argc == 2)
            list_group_files(argv[1]);
        else if (strcmp(argv[0], "groups") == 0)
            show_all_groups_with_files();
        else if (strcmp(argv[0], "delete") == 0 && argc == 2)
            delete_file(argv[1]);
        else if (strcmp(argv[0], "rmdir") == 0 && argc == 2)
            delete_group(argv[1]);
        else if (strcmp(argv[0], "rmgroup") == 0 && argc >= 2)
        {
            // Split into filename and group
            char *filename = argv[1];
            char *group = strchr(argv[1], ' ');
            if (group)
            {
                *group = 0;
                group++;
                while (*group == ' ')
                    group++;
                remove_file_from_group(filename, group);
            }
            else
                printf("Usage: rmgroup <file> <group>\n");
        }
        else if (strcmp(argv[0], "addto") == 0 && argc >= 2)
        {
            // Split into filename and group
            char *rest = argv[1];
            char *filename = rest;
            char *group = strchr(rest, ' ');
            if (group)
            {
                *group = 0;
                group++;
                while (*group == ' ')
                    group++;
                add_file_to_another_group(filename, group);
            }
            else
                printf("Usage: addto <file> <group>\n");
        }
        else if (strcmp(argv[0], "linkgroup") == 0 && argc >= 2)
        {
            // Split into child and parent group
            char *rest = argv[1];
            char *child = rest;
            char *parent = strchr(rest, ' ');
            if (parent)
            {
                *parent = 0;
                parent++;
                while (*parent == ' ')
                    parent++;
                link_group_to_parent(child, parent);
            }
            else
                printf("Usage: linkgroup <child-group> <parent-group>\n");
        }
        else if (strcmp(argv[0], "unlinkgroup") == 0 && argc >= 2)
        {
            // Split into child and parent group
            char *rest = argv[1];
            char *child = rest;
            char *parent = strchr(rest, ' ');
            if (parent)
            {
                *parent = 0;
                parent++;
                while (*parent == ' ')
                    parent++;
                unlink_group_from_parent(child, parent);
            }
            else
                printf("Usage: unlinkgroup <child-group> <parent-group>\n");
        }
        else if (strcmp(argv[0], "open") == 0 && argc == 2)
            open_file(argv[1]);
        else if (strcmp(argv[0], "search") == 0 && argc == 2)
            search_files(argv[1]);
        else if (strcmp(argv[0], "star") == 0 && argc == 2)
            star_file(argv[1]);
        else if (strcmp(argv[0], "unstar") == 0 && argc == 2)
            unstar_file(argv[1]);
        else if (strcmp(argv[0], "starred") == 0)
            list_starred_files();
        else if (strcmp(argv[0], "rename") == 0 && argc >= 2)
        {
            // Split into old name and new name
            char *rest = argv[1];
            char *old_name = rest;
            char *new_name = strchr(rest, ' ');
            if (new_name)
            {
                *new_name = 0;
                new_name++;
                while (*new_name == ' ')
                    new_name++;
                rename_file(old_name, new_name);
            }
            else
                printf("Usage: rename <old-name> <new-name>\n");
        }
        else if (strcmp(argv[0], "info") == 0 && argc == 2)
            show_file_info(argv[1]);
        else if (strcmp(argv[0], "recent") == 0)
            list_recent_files();
        else if (strcmp(argv[0], "help") == 0)
        {
            printf("\n=== Available Commands ===\n");
            printf("  add <path> <group>          - Add any file (PDF/image/doc) to group\n");
            printf("  addto <file> <group>        - Add existing file to another group\n");
            printf("  mkdir <group>               - Create a new group\n");
            printf("  list                        - List all files\n");
            printf("  listgroups                  - List all groups\n");
            printf("  listgroup <group>           - List files in a specific group\n");
            printf("  groups                      - Show all groups with their files\n");
            printf("  search <keyword>            - Search files by name\n");
            printf("  star <file>                 - Star/favorite a file\n");
            printf("  unstar <file>               - Remove star from file\n");
            printf("  starred                     - List all starred files\n");
            printf("  rename <old> <new>          - Rename a file\n");
            printf("  info <file>                 - Show detailed file information\n");
            printf("  recent                      - Show last 10 files added\n");
            printf("  delete <file>               - Delete file completely\n");
            printf("  rmdir <group>               - Delete a group\n");
            printf("  rmgroup <file> <group>      - Remove file from group\n");
            printf("  linkgroup <child> <parent>  - Link group to parent (multi-parent)\n");
            printf("  unlinkgroup <child> <parent>- Remove group from parent\n");
            printf("  open <file>                 - Open file\n");
            printf("  help                        - Show this help\n");
            printf("  exit                        - Exit program\n");
            printf("\n=== View Database Directly ===\n");
            printf("  sqlite3 data/database.db\n");
            printf("  > .mode column\n");
            printf("  > .headers on\n");
            printf("  > SELECT * FROM files;\n");
            printf("  > SELECT * FROM groups;\n");
            printf("  > .exit\n");
        }
        else
            printf("Invalid command. Type 'help' for available commands.\n");
    }

    printf("\nExiting the Research Ecosystem.\n");
}

/* ---------------- MAIN ---------------- */

int main(void)
{
    print_intro();
    init_database();
    ensure_dir(STORE_DIR);

    interactive_shell();

    sqlite3_close(db);
    return 0;
}