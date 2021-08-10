#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte) \
  (byte & 0x100 ? '1' : '0'), \
  (byte & 0x080 ? '1' : '0'), \
  (byte & 0x040 ? '1' : '0'), \
  (byte & 0x020 ? '1' : '0'), \
  (byte & 0x010 ? '1' : '0'), \
  (byte & 0x008 ? '1' : '0'), \
  (byte & 0x004 ? '1' : '0'), \
  (byte & 0x002 ? '1' : '0'), \
  (byte & 0x001 ? '1' : '0')

int binaryConvert(int visibleNum, int binary);      //set the nth bit of binary where n = (visibleNum - 1) [ie. first bit => n = 0]
void box(int arr[9][9], int nums[9][9], int column, int row);   //same as column but with the 3x3 boxes
void check(int arr[9][9], int nums[9][9], int *zeroes); //'checks' nums and updates arr if there is only one possible solution to a given square (ie only one of the nine bits is set)
void column(int arr[9][9], int nums[9][9], int column); //goes through the column of a given square and updates nums by clearing bits as needed (ie if there is a one, clear all rightmost bits in that column)
void doubleCheck(int arr[9][9], int nums[9][9]);
void initializeNums(int arr[9][9], int nums[9][9]);     //initialize all values of num that correspond to all elements of arr that are equivalent to 0 to be 111111111 (or 511 decimal)
void initializeZero(int nums[9][9]);        //initialize the 2D array of 9 bit numbers that represent each possible number that could be in its related square
void printPuzzle10(int arr[9][9]);      //prints the puzzle in base 10 (decimal)
void printPuzzle02(int nums[9][9]);     //prints the puzzle in base 2 (binary)
void row(int arr[9][9], int nums[9][9], int row);   //same as column but with rows
void rowColCheck(int arr[9][9], int nums[9][9]);

void test(int *x);

int main() {
    FILE *inputptr;
    int i, j, k, in, zero0, zeroes = 0;
    int arr[9][9], nums[9][9];
    char buffer[11];
    inputptr = fopen("puzzle1.txt", "r");
    for (i = 0; i < 9; i++) {
        fgets(buffer, 11, inputptr);
        sscanf(buffer, "%d", &in);      //stores a row of values into in (thus in would be a 9 digit integer)
        for (j = 8; j >= 0; j--) {      //iterates through each digit and divides by 10 until the leftmost digit
            if (in % 10 == 0) { zeroes++; }     //counts how many zeroes remain in the puzzle
            arr[i][j] = in % 10;        //extract rightmost digit
            in /= 10;               //shift left
        }
        printf("\n");
    }
    fclose(inputptr);
    printf("Original, %d zeroes\n", zeroes);
    printPuzzle10(arr);

    initializeZero(nums);
    initializeNums(arr, nums);

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            box(arr, nums, (i*3), (j*3));
        }
    }

    for (i = 0; i < 9; i++) {
        column(arr, nums, i);
    }

    for (i = 0; i < 9; i++) {
        row(arr, nums, i);
    }

    //for (k = 0; k < 20; k++) {
    k = 0;
    while (zeroes > 0) {
        zero0 = zeroes;
        k++;

        check(arr, nums, &zeroes);
        printf("check %d, %d zeroes\n", k, zeroes);
        printPuzzle10(arr);
        //printPuzzle02(nums);

        if (zero0 == zeroes) {
            doubleCheck(arr, nums);
            check(arr, nums, &zeroes);
            if (zero0 == zeroes) {
                rowColCheck(arr, nums);
                check(arr, nums, &zeroes);
                if (zero0 == zeroes) { return 0; }
            }
        }  //if there has been no change, the program is unable to solve it and we exit the program

    }
    return 0;
}

void rowColCheck(int arr[9][9], int nums[9][9]) {
    int i, j, k, a, b, n = 1, m = 1, ones, hiddenSingle;
    for (i = 0; i < 9;) {
        for (j = 0; j < 9;) {
            if (arr[i][j] == 0) {   //if the element is not 0 then the square is already solved or was given to us
                hiddenSingle = nums[i][j];
                for (a = 0; a < 9; a++) {
                    if (arr[a][j] == 0 && a != i) {
                        hiddenSingle &= ~(nums[a][j]);
                        if (hiddenSingle == 0) { break; }
                    }
                }
                ones = 0;
                if (hiddenSingle != 0) {
                    for (k = 0; k < 9; k++) {
                        if (hiddenSingle & (1 << k)) { ones++; }
                    }
                    if (ones == 1) {
                        nums[i][j] &= hiddenSingle;
                        return;
                    }
                }


                hiddenSingle = nums[i][j];
                for (a = 0; a < 9; a++) {
                    if (arr[i][a] == 0 && a != j) {
                        hiddenSingle &= ~(nums[i][a]);
                        if (hiddenSingle == 0) { break; }
                    }
                }
                ones = 0;
                if (hiddenSingle != 0) {
                    for (k = 0; k < 9; k++) {
                        if (hiddenSingle & (1 << k)) { ones++; }
                    }
                    if (ones == 1) {
                        nums[i][j] &= hiddenSingle;
                        return;
                    }
                }
            }
            if ((j + 1) % (3 * n) == 0) {
                i++;
                j = 3 * (n - 1);
                if (i % (3 * m) == 0) {
                    i = 3 * (m - 1);
                    j = 3 * n;
                    n++;
                    if (n > 3) {
                        n = 1;
                        m++;
                        j = 0;
                        i = 3 * (m - 1);
                    }
                }
            }
            else { j++; }
        }
    }
}

void doubleCheck(int arr[9][9], int nums[9][9]) {
    int i, j, k, a, b, n = 1, m = 1, column, row, ones, hiddenSingle;
    for (i = 0; i < 9;) {
        for (j = 0; j < 9;) {
            if (arr[i][j] == 0) {   //if the element is not 0 then the square is already solved or was given to us
                hiddenSingle = nums[i][j];
                column = 3 * (i / 3);   //0 if 0 <= i < 3, 3 if 3 <= i < 6, and 6 if 6 <= i < 9
                row = 3 * (j / 3);      //0 if 0 <= j < 3, 3 if 3 <= j < 6, and 6 if 6 <= j < 9
                for (a = column; a < column + 3; a++) {                 // for each element in the puzzle, go through every other element in the same 3x3 box
                    for (b = row; b < row + 3; b++) {
                        if (arr[a][b] == 0 && !(a == (i) && b == j)) {
                            hiddenSingle &= ~(nums[a][b]);
                            if (hiddenSingle == 0) { break; }
                        }
                    }
                    if (hiddenSingle == 0) { break; }
                }
                ones = 0;
                if (hiddenSingle != 0) {
                    for (k = 0; k < 9; k++) {
                        if (hiddenSingle & (1 << k)) { ones++; }
                    }
                    if (ones == 1) {
                        nums[i][j] &= hiddenSingle;
                        return;
                    }
                }
            }
            if ((j + 1) % (3 * n) == 0) {
                i++;
                j = 3 * (n - 1);
                if (i % (3 * m) == 0) {
                    i = 3 * (m - 1);
                    j = 3 * n;
                    n++;
                    if (n > 3) {
                        n = 1;
                        m++;
                        j = 0;
                        i = 3 * (m - 1);
                    }
                }
            }
            else { j++; }
        }
    }
}

void check(int arr[9][9], int nums[9][9], int *zeroes) {
    int i, j, k, bit = 0, ones = 0;
    for (i = 0; i < 9; i++) {
        for (j = 0; j < 9; j++) {
            for (k = 0; k < 9; k++) {
                if ((nums[i][j] & (1 << k)) > 0) {  //checks the number of set bits in each element of nums[i][j]
                    ones++;
                    bit = k;    //sets the value of bit to be the index of the leftmost set bit (hopefully the only set bit)
                }
            }
            if (ones == 1) {    //makes sure there isn't more than one set bit, otherwise we aren't sure what number the square is
                arr[i][j] = bit + 1;    //since the bit represents the index, we want to add one (since indexes go from 0-8 and the sudoku puzzle goes from 1-9)
                *zeroes -= 1;       //subtract from total number of zeroes since we found a square
                nums[i][j] = 0;         //reset the element in nums[i][j]
                box(arr, nums, 3 * (j / 3), 3 * (i / 3));
                column(arr, nums, j);
                row(arr, nums, i);
            }
            ones = 0;   //reset variables
            bit = 0;
        }
    }
}

void box(int arr[9][9], int nums[9][9], int column, int row) {
    int i, j, visibleNum = 0;    //visibleNum = all possible numbers that are present in a 3x3 box (if a corresponding bit is 0/cleared, the 3x3 box is missing that number)
    for (i = column; i < column + 3; i++) {         //find all non zero numbers in a given 3x3 box
        for (j = row; j < row + 3; j++) {
            if (arr[i][j] != 0) { visibleNum |= 1 << (arr[i][j] - 1); }     //if a square already has a value, set the corresponding bit in visibleNum
        }
    }
    for (i = column; i < column + 3; i++) {         //go through all zero values in a given 3x3 box
        for (j = row; j < row + 3; j++) {
            if (arr[i][j] == 0) {
                nums[i][j] = binaryConvert(visibleNum, nums[i][j]);
            }
        }
    }
}

void column(int arr[9][9], int nums[9][9], int column) {
    int i, visibleNum = 0;
    for (i = 0; i < 9; i++) {
        if (arr[i][column] != 0) { visibleNum |= 1 << (arr[i][column] - 1);}
    }
    for (i = 0; i < 9; i++) {
        if (arr[i][column] == 0) { nums[i][column] = binaryConvert(visibleNum, nums[i][column]); }
    }
}

void row(int arr[9][9], int nums[9][9], int row) {
    int i, visibleNum = 0;
    for (i = 0; i < 9; i++) {
        if (arr[row][i] != 0) { visibleNum |= 1 << (arr[row][i] - 1);}
    }
    for (i = 0; i < 9; i++) {
        if (arr[row][i] == 0) { nums[row][i] = binaryConvert(visibleNum, nums[row][i]); }
    }
}

int binaryConvert(int visibleNum, int binary) { //sets the bits in nums array to show potential candidates
    int i;
    for (i = 0; i < 9; i++) {
        if ((visibleNum & (1 << i)) > 0) { binary &= ~(1 << i); }
    }
    return binary;
}

void printPuzzle10(int arr[9][9]) {
    int i, j;
    for (i = 0; i < 9; i++) {
        for (j = 0; j < 9; j++) {
            if (arr[i][j] == 0) { printf(" "); }
            else { printf("%d", arr[i][j]); }
            printf(" ");
            if (j == 2 || j == 5) { printf("| "); }
        }
        printf("\n");
        if (i == 2 || i == 5) { printf("---------------------\n"); }
    }
    printf("\n\n");
}

void printPuzzle02(int nums[9][9]) {
    int i, j;
    for (i = 0; i < 9; i++) {
        for (j = 0; j < 9; j++) {
            printf(BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(nums[i][j]));
            printf(" ");
        }
        printf("\n");
    }
    printf("\n");
}

void initializeNums(int arr[9][9], int nums[9][9]) {
    int i, j, k;
    for (i = 0; i < 9; i++) {
        for (j = 0; j < 9; j ++) {
            if (arr[i][j] == 0) {
                for (k = 0; k < 9; k++) {
                    nums[i][j] |= 1 << k;
                }
            }
        }
    }
}

void initializeZero(int nums[9][9]) {
    int i, j, k;
    for (i = 0; i < 9; i++) {
        for (j = 0; j < 9; j ++) {
            for (k = 0; k < 9; k++) {
                nums[i][j] = 0;
            }
        }
    }
}

// setting bits: nums[i][j] |= 1 << i         0 is first/rightmost bit
// clearing bits: nums[i][j] &= ~(1 << i)

/* STRATEGIES
 *
 * DONE   Naked Single: if a square has only one potential number, it must be that number (duh)
 *    Hidden Single: if a square in a row, column, or 3x3 has multiple numbers where one of which is the only instance of that number then it must be that number
 *    Naked Pair: if 2 squares in a row/column (in the same 3x3) both have the same 2 potential numbers, eliminate all other instances of those numbers in the row/column and 3x3
 *    Pointing Pair/Triple: same as naked pair but with only one number and can apply for two OR three squares in a row/column in the same 3x3
 *    Claiming Pair/Triple: if there are 2+ potential numbers left in a row/column, eliminate all other instances in the same 3x3
 *    Naked Triple: if 3 cells in a row/column (not necessarily in a 3x3) have the same set/subset of potential numbers, eliminate all other instances of those numbers in the row/column
 *    X-Wing: 2 parallel Naked Pairs, eliminate all instances of those numbers in both rows and columns
 *    Hidden Pair: same as Hidden Single but with 2 potential numbers
 *    Naked Quad: Same as Naked Triple but with 4 squares in a row/column
 */

/*  for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            box(arr, nums, (i*3), (j*3));
        }
    }
    printf("3x3 boxes\n--------------------------\n");
    printPuzzle02(nums);
    for (i = 0; i < 9; i++) {
        column(arr, nums, i);
    }
    printf("columns\n--------------------------\n");
    printPuzzle02(nums);
    for (i = 0; i < 9; i++) {
        row(arr, nums, i);
    }
    printf("rows\n--------------------------\n");
    printPuzzle02(nums);

    return 0;*/
