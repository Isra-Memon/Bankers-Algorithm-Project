/* Banker's Algorithm Deadlock Avoidance Simulator
 * Build: gcc -std=c11 -Wall -Wextra -pedantic bankers.c -o bankers
 */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_PROC 10
#define MAX_RES 10
#define LINE 256

typedef struct {
    int n, m;
    int available[MAX_RES];
    int allocation[MAX_PROC][MAX_RES];
    int maximum[MAX_PROC][MAX_RES];
    int need[MAX_PROC][MAX_RES];
    bool loaded;
} System;

static bool read_int(const char *prompt, int min, int max, int *out) {
    char line[LINE], extra;
    long value;
    for (;;) {
        printf("%s", prompt);
        if (!fgets(line, sizeof line, stdin)) return false;
        if (!strchr(line, '\n') && !feof(stdin)) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF) {}
            puts("Input too long. Try again.");
            continue;
        }
        if (sscanf(line, " %ld %c", &value, &extra) == 1 &&
            value >= min && value <= max) {
            *out = (int)value;
            return true;
        }
        printf("Enter an integer from %d to %d.\n", min, max);
    }
}

static void calculate_need(System *s) {
    for (int i = 0; i < s->n; ++i)
        for (int j = 0; j < s->m; ++j)
            s->need[i][j] = s->maximum[i][j] - s->allocation[i][j];
}

static bool safe(const System *s, int sequence[MAX_PROC], bool verbose) {
    int work[MAX_RES];
    bool finished[MAX_PROC] = {false};
    int count = 0;
    for (int j = 0; j < s->m; ++j) work[j] = s->available[j];
    while (count < s->n) {
        bool progress = false;
        for (int i = 0; i < s->n; ++i) {
            if (finished[i]) continue;
            bool possible = true;
            for (int j = 0; j < s->m; ++j)
                if (s->need[i][j] > work[j]) possible = false;
            if (!possible) continue;
            if (verbose) {
                printf("P%d completes; work before:", i);
                for (int j = 0; j < s->m; ++j) printf(" %d", work[j]);
            }
            for (int j = 0; j < s->m; ++j)
                work[j] += s->allocation[i][j];
            if (verbose) {
                printf("; work after:");
                for (int j = 0; j < s->m; ++j) printf(" %d", work[j]);
                putchar('\n');
            }
            sequence[count++] = i;
            finished[i] = true;
            progress = true;
        }
        if (!progress) break;
    }
    return count == s->n;
}

static void show_sequence(const System *s, const int seq[MAX_PROC]) {
    printf("Safe sequence: ");
    for (int i = 0; i < s->n; ++i)
        printf("%sP%d", i ? " -> " : "", seq[i]);
    putchar('\n');
}

static void display(const System *s) {
    puts("\nProcess | Allocation | Maximum | Need");
    for (int i = 0; i < s->n; ++i) {
        printf("P%-7d| ", i);
        for (int j = 0; j < s->m; ++j) printf("%d ", s->allocation[i][j]);
        printf("| ");
        for (int j = 0; j < s->m; ++j) printf("%d ", s->maximum[i][j]);
        printf("| ");
        for (int j = 0; j < s->m; ++j) printf("%d ", s->need[i][j]);
        putchar('\n');
    }
    printf("Available: ");
    for (int j = 0; j < s->m; ++j) printf("%d ", s->available[j]);
    putchar('\n');
}

static bool input_data(System *s) {
    System tmp = {0};
    int x;
    if (!read_int("Processes (1-10): ", 1, MAX_PROC, &tmp.n) ||
        !read_int("Resource types (1-10): ", 1, MAX_RES, &tmp.m))
        return false;
    for (int j = 0; j < tmp.m; ++j) {
        char prompt[80];
        snprintf(prompt, sizeof prompt, "Available R%d: ", j);
        if (!read_int(prompt, 0, 1000000, &tmp.available[j])) return false;
    }
    for (int i = 0; i < tmp.n; ++i)
        for (int j = 0; j < tmp.m; ++j) {
            char prompt[80];
            snprintf(prompt, sizeof prompt, "Allocation P%d R%d: ", i, j);
            if (!read_int(prompt, 0, 1000000, &x)) return false;
            tmp.allocation[i][j] = x;
        }
    for (int i = 0; i < tmp.n; ++i)
        for (int j = 0; j < tmp.m; ++j) {
            char prompt[80];
            snprintf(prompt, sizeof prompt, "Maximum P%d R%d (min %d): ",
                     i, j, tmp.allocation[i][j]);
            if (!read_int(prompt, tmp.allocation[i][j], 1000000, &x)) return false;
            tmp.maximum[i][j] = x;
        }
    calculate_need(&tmp);
    tmp.loaded = true;
    *s = tmp;
    puts("Data saved successfully.");
    return true;
}

static void sample(System *s) {
    static const int a[5][3] = {
        {0,1,0}, {2,0,0}, {3,0,2}, {2,1,1}, {0,0,2}
    };
    static const int max[5][3] = {
        {7,5,3}, {3,2,2}, {9,0,2}, {2,2,2}, {4,3,3}
    };
    memset(s, 0, sizeof *s);
    s->n = 5; s->m = 3;
    s->available[0] = 3; s->available[1] = 3; s->available[2] = 2;
    for (int i = 0; i < 5; ++i)
        for (int j = 0; j < 3; ++j) {
            s->allocation[i][j] = a[i][j];
            s->maximum[i][j] = max[i][j];
        }
    calculate_need(s);
    s->loaded = true;
    puts("Standard sample loaded.");
}

static bool request(System *s) {
    int pid, req[MAX_RES], seq[MAX_PROC];
    if (!read_int("Process ID: ", 0, s->n - 1, &pid)) return false;
    for (int j = 0; j < s->m; ++j) {
        char prompt[80];
        snprintf(prompt, sizeof prompt, "Request R%d: ", j);
        if (!read_int(prompt, 0, 1000000, &req[j])) return false;
    }
    for (int j = 0; j < s->m; ++j)
        if (req[j] > s->need[pid][j]) {
            puts("DENIED: Request exceeds remaining need.");
            return true;
        }
    for (int j = 0; j < s->m; ++j)
        if (req[j] > s->available[j]) {
            puts("WAIT: Insufficient available resources.");
            return true;
        }
    for (int j = 0; j < s->m; ++j) {
        s->available[j] -= req[j];
        s->allocation[pid][j] += req[j];
        s->need[pid][j] -= req[j];
    }
    if (safe(s, seq, false)) {
        puts("GRANTED: System remains safe.");
        show_sequence(s, seq);
    } else {
        for (int j = 0; j < s->m; ++j) {
            s->available[j] += req[j];
            s->allocation[pid][j] -= req[j];
            s->need[pid][j] += req[j];
        }
        puts("DENIED: Unsafe state; allocation rolled back.");
    }
    return true;
}

int main(void) {
    System s = {0};
    int choice, seq[MAX_PROC];
    puts("=== BANKER'S ALGORITHM SIMULATOR ===");
    for (;;) {
        puts("\n1. Enter data\n2. Display matrices\n3. Calculate Need matrix\n"
             "4. Check safety (step-by-step)\n5. Request resources\n"
             "6. Load sample data\n7. Exit");
        if (!read_int("Choice: ", 1, 7, &choice)) break;
        if (choice == 7) break;
        if (choice == 1) {
            if (!input_data(&s)) break;
            continue;
        }
        if (choice == 6) { sample(&s); continue; }
        if (!s.loaded) { puts("Enter or load data first."); continue; }
        switch (choice) {
            case 2: display(&s); break;
            case 3: calculate_need(&s); display(&s); break;
            case 4:
                if (safe(&s, seq, true)) {
                    puts("SAFE STATE");
                    show_sequence(&s, seq);
                } else {
                    puts("UNSAFE STATE (not necessarily deadlocked).");
                }
                break;
            case 5:
                if (!request(&s)) return 0;
                break;
            default: break;
        }
    }
    puts("Goodbye.");
    return 0;
}
