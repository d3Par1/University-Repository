/*
 * Task 1.11 - Student database (add, edit, delete, partial search)
 *
 * Compile: gcc -Wall -o task1_11 student_db.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STUDENTS 100
#define MAX_NAME 64
#define MAX_ADDR 128

typedef struct {
    int id;
    char name[MAX_NAME];
    char birthdate[16]; /* DD.MM.YYYY */
    char address[MAX_ADDR];
    int active;
} Student;

Student db[MAX_STUDENTS];
int db_count = 0;
int next_id = 1;

void add_student(void) {
    if (db_count >= MAX_STUDENTS) {
        printf("Database full!\n");
        return;
    }
    Student *s = &db[db_count];
    s->id = next_id++;
    s->active = 1;

    printf("Name: ");
    scanf(" %63[^\n]", s->name);
    printf("Birth date (DD.MM.YYYY): ");
    scanf(" %15s", s->birthdate);
    printf("Address: ");
    scanf(" %127[^\n]", s->address);

    db_count++;
    printf("Student added (ID: %d)\n", s->id);
}

int find_by_id(int id) {
    for (int i = 0; i < db_count; i++) {
        if (db[i].id == id && db[i].active) return i;
    }
    return -1;
}

void edit_student(void) {
    int id;
    printf("Enter student ID to edit: ");
    scanf("%d", &id);

    int idx = find_by_id(id);
    if (idx < 0) { printf("Not found!\n"); return; }

    Student *s = &db[idx];
    printf("Current: %s, %s, %s\n", s->name, s->birthdate, s->address);
    printf("New name (or . to keep): ");
    char buf[MAX_NAME];
    scanf(" %63[^\n]", buf);
    if (strcmp(buf, ".") != 0) strcpy(s->name, buf);

    printf("New birthdate (or . to keep): ");
    scanf(" %15s", buf);
    if (strcmp(buf, ".") != 0) strcpy(s->birthdate, buf);

    printf("New address (or . to keep): ");
    char addr[MAX_ADDR];
    scanf(" %127[^\n]", addr);
    if (strcmp(addr, ".") != 0) strcpy(s->address, addr);

    printf("Student updated.\n");
}

void delete_student(void) {
    int id;
    printf("Enter student ID to delete: ");
    scanf("%d", &id);

    int idx = find_by_id(id);
    if (idx < 0) { printf("Not found!\n"); return; }
    db[idx].active = 0;
    printf("Student %d deleted.\n", id);
}

void search_partial(void) {
    char query[MAX_NAME];
    printf("Search query: ");
    scanf(" %63[^\n]", query);

    printf("\nResults:\n");
    int found = 0;
    for (int i = 0; i < db_count; i++) {
        if (!db[i].active) continue;
        if (strstr(db[i].name, query) || strstr(db[i].address, query) ||
            strstr(db[i].birthdate, query)) {
            printf("  ID:%d | %s | %s | %s\n",
                   db[i].id, db[i].name, db[i].birthdate, db[i].address);
            found++;
        }
    }
    if (!found) printf("  No matches.\n");
}

void list_all(void) {
    printf("\n%-4s %-20s %-12s %-30s\n", "ID", "Name", "Birthdate", "Address");
    printf("--------------------------------------------------------------\n");
    for (int i = 0; i < db_count; i++) {
        if (!db[i].active) continue;
        printf("%-4d %-20s %-12s %-30s\n",
               db[i].id, db[i].name, db[i].birthdate, db[i].address);
    }
}

int main() {
    int choice;

    printf("=== Task 1.11: Student Database ===\n");

    while (1) {
        printf("\n1) Add  2) Edit  3) Delete  4) Search  5) List  0) Exit\n> ");
        if (scanf("%d", &choice) != 1) break;

        switch (choice) {
            case 1: add_student(); break;
            case 2: edit_student(); break;
            case 3: delete_student(); break;
            case 4: search_partial(); break;
            case 5: list_all(); break;
            case 0: printf("Bye!\n"); return 0;
            default: printf("Invalid choice.\n");
        }
    }

    return 0;
}
