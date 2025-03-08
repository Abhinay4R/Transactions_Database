#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 100

// Dihedral permutation table
static const int permuatation_table[8][10] = {
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
    {1, 5, 7, 6, 2, 8, 3, 0, 9, 4},
    {5, 8, 0, 3, 7, 9, 6, 1, 4, 2},
    {8, 9, 1, 6, 0, 4, 3, 5, 2, 7},
    {9, 4, 5, 3, 1, 2, 6, 8, 7, 0},
    {4, 2, 8, 6, 5, 7, 3, 9, 0, 1},
    {2, 7, 9, 3, 8, 0, 6, 4, 1, 5},
    {7, 0, 4, 6, 9, 1, 3, 2, 5, 8}
};

// Non-commutative multiplication table (D5 group)
static const int d5_mult_table[10][10] = {
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9},
    {1, 2, 3, 4, 0, 6, 7, 8, 9, 5},
    {2, 3, 4, 0, 1, 7, 8, 9, 5, 6},
    {3, 4, 0, 1, 2, 8, 9, 5, 6, 7},
    {4, 0, 1, 2, 3, 9, 5, 6, 7, 8},
    {5, 9, 8, 7, 6, 0, 4, 3, 2, 1},
    {6, 5, 9, 8, 7, 1, 0, 4, 3, 2},
    {7, 6, 5, 9, 8, 2, 1, 0, 4, 3},
    {8, 7, 6, 5, 9, 3, 2, 1, 0, 4},
    {9, 8, 7, 6, 5, 4, 3, 2, 1, 0}
};

// Inverse lookup table
static const int inv_table[10] = {0, 4, 3, 2, 1, 5, 9, 8, 7, 6};

// Implementation of f(a,b)
static int f(int a, int b) {
    return d5_mult_table[a][permuatation_table[0][b]];
}

int calculate_check_digit(int number) {
    if (number < 10000 || number > 99999) return -1;
    
    int a1 = (number / 10000) % 10;
    int a2 = (number / 1000) % 10;
    int a3 = (number / 100) % 10;
    int a4 = (number / 10) % 10;
    int a5 = number % 10;
    
    // Calculate c = f(a1, f(a2, f(a3, f(a4, a5))))
    int c = f(a1, f(a2, f(a3, f(a4, a5))));
    
    // Return inverse of c
    return inv_table[c];
}
// Function to check if an ID is a valid five-digit number (10000-99999)
int is_valid_id(int num) {
    return num >= 10000 && num <= 99999;
}

int main() {
    FILE *input_file = fopen("input.txt", "r");
    FILE *output_file = fopen("output.txt", "w");
    if (!input_file || !output_file) {
        printf("Error opening file.\n");
        return 1;
    }

    char line[MAX_LINE_LENGTH];

    // Skip the first line of input file
    fgets(line, sizeof(line), input_file);

    // Write header to the output file
    fprintf(output_file, "TID   ; SID   ; BID    #This is the output format\n");

    while (fgets(line, sizeof(line), input_file)) {
        char tid[6], sid[6], bid[6], extra[MAX_LINE_LENGTH];

        // Strict format check: Ensures exactly 3 five-digit numbers with no extra characters
        if (sscanf(line, " %5[0-9] , %5[0-9] , %5[0-9] %s", tid, sid, bid, extra) != 3) {
            fprintf(output_file, "INVALID ENTRY\n");
            continue;
        }

        int tid_num = atoi(tid);
        int sid_num = atoi(sid);
        int bid_num = atoi(bid);

        if (!is_valid_id(tid_num) || !is_valid_id(sid_num) || !is_valid_id(bid_num)) {
            fprintf(output_file, "INVALID ENTRY\n");
            continue;
        }

        int tid_check = calculate_check_digit(tid_num);
        int sid_check = calculate_check_digit(sid_num);
        int bid_check = calculate_check_digit(bid_num);

        fprintf(output_file, "%05d%d, %05d%d, %05d%d\n", tid_num, tid_check, sid_num, sid_check, bid_num, bid_check);
    }

    fclose(input_file);
    fclose(output_file);
    printf("Input has been validated. Please check output.txt\n");
    return 0;
}
