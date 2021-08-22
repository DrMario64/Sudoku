/*
 * Author: Lucas Alm
 * Completed 20 AUG 2021
 *
 * This program reads the contents of a text file that contains 81 integers with no spaces and new lines every 9 digits.
 * It then fills in each 0 as if the txt file were a sudoku puzzle. That is, no two of the same numbers can share the same
 * row, column, or 3x3 box.  This program prints out the original puzzle and the number of blank spaces that need to be
 * solved, then once it is finished, it either prints out that the puzzle is impossible (or the program cannot solve it)
 * or it prints out the completed puzzle and how many iterations (times needed to go through the entire puzzle to narrow
 * down possible candidates for each individual square) it needed to complete it.
 *
 * This is all done by creating a 2D array (nums) that is a 1:1 to the actual array of integers. However, nums is an array
 * of 9 bit integers where each bit is associated with a potential candidate for its square. So if a square has all 9 possible
 * candidates, its nums value would be 511 and as the program narrows down possible candidates, this number will get smaller
 * since bits will be cleared as a square gets less and less candidates.
 */
#include <stdio.h>

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
void HiddenSingleBox(int arr[9][9], int nums[9][9]);    //goes through the puzzle until it finds a hidden single (a square in a row/column/box that can only be one number, put has other candidates as well)
void initializeNums(int arr[9][9], int nums[9][9]);     //initialize all values of num that correspond to all elements of arr that are equivalent to 0 to be 111111111 (or 511 decimal)
void initializeZero(int nums[9][9]);        //initialize the 2D array of 9 bit numbers that represent each possible number that could be in its related square
void printPuzzle10(int arr[9][9]);      //prints the puzzle in base 10 (decimal)
void printPuzzle02(int nums[9][9]);     //prints the puzzle in base 2 (binary). Used for debugging.
void row(int arr[9][9], int nums[9][9], int row);   //same as column but with rows
void HiddenSingleRowCol(int arr[9][9], int nums[9][9]);     //same as HiddenSingleBox but only checks the row and column
void ClaimingPairBox(int arr[9][9], int nums[9][9]);    //goes through the puzzle until it finds two pairs of candidates that are not found in any other squares in the same 3x3 box
void ClaimingPairRowCol(int arr[9][9], int nums[9][9]);     //same as ClaimingPairBox but only checks the row and column

void test(int *x);

int main() {
    FILE *inputptr;
    int i, j, checks, in, zeroCheck, zeroes = 0;
    int arr[9][9], nums[9][9];
    char buffer[11];
    inputptr = fopen("puzzle1.txt", "r");
    for (i = 0; i < 9; i++) {
        fgets(buffer, 11, inputptr);
        sscanf(buffer, "%d", &in);      //stores a row of values into in (thus in would be a 9 digit integer)
        for (j = 8; j >= 0; j--) {      //iterates through each digit and divides by 10 until the leftmost digit
            if (in % 10 == 0) { zeroes++; }     //counts how many zeroes remain in the puzzle
            arr[i][j] = in % 10;        //extract rightmost digit
            in /= 10;               //shift right
        }
        printf("\n");
    }
    fclose(inputptr);
    printf("Original Puzzle: %d blank squares\n", zeroes);    //prints the original puzzle and the total number of empty squares
    printPuzzle10(arr);

    initializeZero(nums);       //initialize all values in nums to zero that correspond to nonzero values in arr
    initializeNums(arr, nums);  //set bits of all values in nums that correspond to zero values in arr

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            box(arr, nums, (i*3), (j*3));   //go through and narrow down possible values for each square in each 3x3 box
        }
    }

    for (i = 0; i < 9; i++) {
        column(arr, nums, i);   //go through and narrow down possible values for each square in each column
    }

    for (i = 0; i < 9; i++) {
        row(arr, nums, i);  //go through and narrow down possible values for each square in each row
    }

    checks = 0;         //the number of "checks" or iterations the puzzle has gone through
    while (zeroes > 0) {
        zeroCheck = zeroes;
        checks++;

        check(arr, nums, &zeroes);

        /*printf("check %d, %d zeroes\n", checks, zeroes);              USED FOR DEBUGGING
        printPuzzle10(arr);
        printPuzzle02(nums);*/


        /* we could go through each method in every iteration, however that would be extremely inefficient. It is much
         * better to rely heavily on the simpler algorithms/common case more often then resort to the more complicated
         * ones if they fail.
         */
        if (zeroCheck == zeroes) {  //continuously check to see if the number of zeroes in the puzzle has stayed the same.
            HiddenSingleBox(arr, nums);
            check(arr, nums, &zeroes);
            if (zeroCheck == zeroes) {
                HiddenSingleRowCol(arr, nums);
                check(arr, nums, &zeroes);
                if (zeroCheck == zeroes) {
                    ClaimingPairBox(arr, nums);
                    check(arr, nums, &zeroes);
                    if (zeroCheck == zeroes) {
                        ClaimingPairRowCol(arr, nums);
                        check(arr, nums, &zeroes);
                        if (zeroCheck == zeroes) {
                            printf("Puzzle could not be completed. Iterations: %d\n", checks);
                            printPuzzle10(arr);
                            return 0;
                        }
                    }
                }
            }
        }  //if there has been no change, the program is unable to solve it and we exit the program
    }
    printf("Puzzle completed! Iterations: %d\n", checks);
    printPuzzle10(arr);
    return 0;
}

void ClaimingPairRowCol(int arr[9][9], int nums[9][9]) {
    int i, j, k, a, b, c, ones, changed, claimingPair;
    for (i = 0; i < 9; i++) {
        for (j = 0; j < 9; j++) {
            if (arr[i][j] == 0) {   //if the element is not 0 then the square is already solved or was given to us
                for (a = 0; a < 9; a++) {
                    claimingPair = 0;   //reset claimingPair
                    if (a != j && arr[i][a] == 0) {     //make sure that its a different square and that its not a square that is already solved
                        claimingPair = nums[i][j] & nums[i][a];     //set all bits in claimingPair that occur in both nums[i][j] and nums[i][a]
                        if (claimingPair == 0) { break; }       //if the two squares have nothing in common, exit the loop and compare the next pair
                        for (b = 0; b < 9; b++) {                 // for each square, go through every other square in the same row
                            if (arr[i][b] == 0 && !(b == a || b == j)) {  //make sure the square isn't already solved and isn't one of the same squares being checked
                                claimingPair &= nums[i][b];      //if there is a claiming pair, no other squares will have the same 2 bits set
                                if (claimingPair == 0) { break; }   //break if no claiming pair
                            }
                            if (claimingPair == 0) { break; }   //break out of this for loop as well
                        }
                    }
                    ones = 0;
                    if (claimingPair != 0) {
                        for (k = 0; k < 9; k++) {
                            if (claimingPair & (1 << k)) { ones++; }    //count all of the set bits
                        }
                        if (ones == 2) {
                            changed = 0;
                            nums[i][j] &= claimingPair;     //in case these squares had other possible values, we set them equal to only the bits in claiming pair
                            nums[i][a] &= claimingPair;     //same thing for the second square being checked
                            for (c = 0; c < 9; c++) {
                                if (arr[i][c] == 0 && !(c == a || c == j)) {  //make sure the square isn't already solved and isn't one of the same squares being checked
                                    if (nums[i][c] != (nums[i][c] & ~(claimingPair))) { changed++; }    //increment changed if a square had its possible values altered
                                    nums[i][c] &= ~(claimingPair);  //clear all bits that nums[i][c] has in common with the claiming pair
                                }
                            }
                            if (changed > 0) { return; }
                        }
                    }
                }

                for (a = 0; a < 9; a++) {
                    claimingPair = 0;
                    if (a != i && arr[a][j] == 0) {     //make sure that its a different square and that its not a square that is already solved
                        claimingPair = nums[i][j] & nums[a][j];     //set all bits in claimingPair that occur in both nums[i][j] and nums[a][j]
                        if (claimingPair == 0) { break; }       //if the two squares have nothing in common, exit the loop and compare the next pair
                        for (b = 0; b < 9; b++) {                 // for each square, go through every other square in the same column
                            if (arr[b][j] == 0 && !(b == a || b == i)) {  //make sure the square isn't already solved and isn't one of the same squares being checked
                                claimingPair &= nums[b][j];      //if there is a claiming pair, no other squares will have the same 2 bits set
                                if (claimingPair == 0) { break; }   //break if no claiming pair
                            }
                            if (claimingPair == 0) { break; }   //break out of this for loop as well
                        }
                    }
                    ones = 0;
                    if (claimingPair != 0) {
                        for (k = 0; k < 9; k++) {
                            if (claimingPair & (1 << k)) { ones++; }    //count all of the set bits
                        }
                        if (ones == 2) {
                            changed = 0;
                            nums[i][j] &= claimingPair;     //in case these squares had other possible values, we set them equal to only the bits in claiming pair
                            nums[a][j] &= claimingPair;     //same thing for the second square being checked
                            for (c = 0; c < 9; c++) {
                                if (arr[c][j] == 0 && !(c == a || c == i)) {  //make sure the square isn't already solved and isn't one of the same squares being checked
                                    if (nums[c][j] != (nums[c][j] & ~(claimingPair))) { changed++; }    //increment changed if a square had its possible values altered
                                    nums[c][j] &= ~(claimingPair);  //clear all bits that nums[c][j] has in common with the claiming pair
                                }
                            }
                            if (changed > 0) { return; }
                        }
                    }
                }
            }
        }
    }
}


void ClaimingPairBox(int arr[9][9], int nums[9][9]) {
    int i, j, k, a, b, c, d, n = 1, m = 1, column, row, ones, changed, claimingPair;
    for (i = 0; i < 9;) {
        for (j = 0; j < 9;) {
            if (arr[i][j] == 0) {   //if the element is not 0 then the square is already solved or was given to us
                row = 3 * (i / 3);   //0 if 0 <= i < 3, 3 if 3 <= i < 6, and 6 if 6 <= i < 9
                column = 3 * (j / 3);      //0 if 0 <= j < 3, 3 if 3 <= j < 6, and 6 if 6 <= j < 9
                for (a = row; a < row + 3; a++) {
                    for (b = column; b < column + 3; b++) {
                        claimingPair = 0;   //reset claimingPair
                        if ((a != i || b != j) && arr[a][b] == 0) {
                            claimingPair = nums[i][j] & nums[a][b];
                            if (claimingPair == 0) { break; }       //if the two squares have nothing in common, exit the loop and compare the next pair
                            for (c = row; c < row + 3; c++) {                 // for each square, go through every other square in the same 3x3 box
                                for (d = column; d < column + 3; d++) {
                                    if (arr[c][d] == 0 && !((c == a && d == b) || (c == i && d == j))) {  //make sure the square isn't already solved and isn't one of the same squares being checked
                                        claimingPair &= nums[c][d];      //if there is a claiming pair, no other squares will have the same 2 bits set
                                        if (claimingPair == 0) { break; }   //break if no claiming pair
                                    }
                                }
                                if (claimingPair == 0) { break; }   //break out of this for loop as well
                            }
                            ones = 0;
                            if (claimingPair != 0) {    //nonzero claimingPair means we have at least one unique bit (Ideally we have 2 unique bits)
                                for (k = 0; k < 9; k++) {
                                    if (claimingPair & (1 << k)) { ones++; }    //find the set bit
                                }
                                if (ones == 2) {
                                    changed = 0;
                                    nums[i][j] &= claimingPair;
                                    nums[a][b] &= claimingPair;
                                    for (c = row; c < row + 3; c++) {                 // for each square, go through every other square in the same 3x3 box
                                        for (d = column; d < column + 3; d++) {
                                            if (arr[c][d] == 0 && !((c == a && d == b) || (c == i && d == j))) {  //make sure the square isn't already solved and isn't one of the same squares being checked
                                                if (nums[c][d] != (nums[c][d] & ~(claimingPair))) { changed++; }
                                                nums[c][d] &= ~(claimingPair);
                                            }
                                        }
                                    }
                                    if (changed > 0) { return; }
                                }
                            }
                        }
                    }
                }
            }

            /*
             * This code snippet allows the program to iterate through each square and its respective 3x3 box. No matter
             * what square it is checking, it can calculate what 3x3 box it is in using "row = 3 * (i / 3)" and
             * "column = 3 * (j / 3)" like on lines 208 and 209. Then we want to start at the top left square in that box
             * and move across, then down to the next row of 3 but back at the first column, over again, then down to the
             * third and final row of the box and then through the next two columns.
             *
             * n is the indicator for the column of 3x3 boxes. n = 1 means a first/left box, n = 2 means a middle box, and
             * n = 3 means a right box. Likewise, m is the indicator for the row of 3x3 boxes. m = 1 means a top box, m = 2
             * a middle box, and m = 3 a bottom box.
             */
            if ((j + 1) % (3 * n) == 0) {   //makes sure the next j value is not divisible by 3/6/9 (if it is then we crossed over to the next box, boxes go from 0-2, 3-5, and 6-8)
                i++;        //go to the next row
                j = 3 * (n - 1);    //j = 0 when n = 1, 3 when n = 2, and 6 when n = 3
                if (i % (3 * m) == 0) {     //makes sure the current i value is not divisible by 3/6/9 (if it is then we crossed over to the next box)
                    i = 3 * (m - 1);    //i = 0 when n = 1, 3 when n = 2, and 6 when n = 3
                    j = 3 * n;      //j = 3 when n = 1, 6 when n = 2, and 9 when n = 3
                    n++;    //go to the next column of boxes
                    if (n > 3) {       //since there are only 3 boxesin each row, n cannot be 4
                        n = 1;      //reset n
                        m++;    //go to the next row of boxes
                        j = 0;      //j always needs to be set to 0 because m just got incremented so we know we are on the left hand size of the puzzle again
                        i = 3 * (m - 1);    //adjust i accordingly
                        if (m > 3) { return; }  //since there are only 3 boxes in each column, m cannot be 4
                    }
                }
            }
            else { j++; }
        }
    }
}

void HiddenSingleRowCol(int arr[9][9], int nums[9][9]) {
    int i, j, k, a, ones, hiddenSingle;
    for (i = 0; i < 9; i++) {
        for (j = 0; j < 9; j++) {
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
        }
    }
}

void HiddenSingleBox(int arr[9][9], int nums[9][9]) {
    int i, j, k, a, b, n = 1, m = 1, column, row, ones, hiddenSingle;
    for (i = 0; i < 9;) {
        for (j = 0; j < 9;) {
            if (arr[i][j] == 0) {   //if the element is not 0 then the square is already solved or was given to us
                hiddenSingle = nums[i][j];
                row = 3 * (i / 3);   //0 if 0 <= i < 3, 3 if 3 <= i < 6, and 6 if 6 <= i < 9
                column = 3 * (j / 3);      //0 if 0 <= j < 3, 3 if 3 <= j < 6, and 6 if 6 <= j < 9
                for (a = row; a < row + 3; a++) {                 // for each element in the puzzle, go through every other element in the same 3x3 box
                    for (b = column; b < column + 3; b++) {
                        if (arr[a][b] == 0 && !(a == (i) && b == j)) {  //make sure the square isnt already solved and isnt the same square being checked
                            hiddenSingle &= ~(nums[a][b]);      //if there is a hidden single, no other squares will have that bit set
                            if (hiddenSingle == 0) { break; }   //break if no hidden single
                        }
                    }
                    if (hiddenSingle == 0) { break; }
                }
                ones = 0;
                if (hiddenSingle != 0) {    //nonzero hidden single means we have at least one unique bit (more than one unique bit means we messed up but its always good to check anyways)
                    for (k = 0; k < 9; k++) {
                        if (hiddenSingle & (1 << k)) { ones++; }    //find the set bit
                    }
                    if (ones == 1) {
                        nums[i][j] &= hiddenSingle;
                        return;
                    }
                }
            }
            if ((j + 1) % (3 * n) == 0) {   //makes sure the next j value is not divisible by 3/6/9 (if it is then we crossed over to the next box, boxes go from 0-2, 3-5, and 6-8)
                i++;                //go to the next row
                j = 3 * (n - 1);         //j = 0 when n = 1, 3 when n = 2, and 6 when n = 3
                if (i % (3 * m) == 0) {     //makes sure the current i value is not divisible by 3/6/9 (if it is then we crossed over to the next box)
                    i = 3 * (m - 1);      //i = 0 when n = 1, 3 when n = 2, and 6 when n = 3
                    j = 3 * n;         //j = 3 when n = 1, 6 when n = 2, and 9 when n = 3
                    n++;    //go to the next column of boxes
                    if (n > 3) {        //n cannot be greater than 3
                        n = 1;      //reset n
                        m++;            //go to the next row of boxes
                        j = 0;      //j always needs to be set to 0 because m just got incremented so we know we are on the left hand size of the puzzle again
                        i = 3 * (m - 1);    //adjust i accordingly
                        if (m > 3) { return; }      //since there are only 3 boxes in each column, m cannot be 4
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
        //ones = 0;
    }
}

void box(int arr[9][9], int nums[9][9], int column, int row) {
    int i, j, visibleNum = 0;    //visibleNum = all possible numbers that are present in a 3x3 box (if a corresponding bit is 0/cleared, the 3x3 box is missing that number)
    for (i = row; i < row + 3; i++) {         //find all non zero numbers in a given 3x3 box
        for (j = column; j < column + 3; j++) {
            if (arr[i][j] != 0) { visibleNum |= 1 << (arr[i][j] - 1); }     //if a square already has a value, set the corresponding bit in visibleNum
        }
    }
    for (i = row; i < row + 3; i++) {         //go through all zero values in a given 3x3 box
        for (j = column; j < column + 3; j++) {
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
    printf("     ------------------------\n");
    for (i = 0; i < 9; i++) {
        printf("     |");
        for (j = 0; j < 9; j++) {
            if (arr[i][j] == 0) { printf(" "); }
            else { printf("%d", arr[i][j]); }
            printf(" ");
            if (j == 2 || j == 5 || j == 8) { printf("| "); }
        }
        printf("\n");
        if (i == 2 || i == 5) { printf("     ------------------------\n"); }
    }
    printf("     ------------------------\n");
    printf("\n\n");
}

void printPuzzle02(int nums[9][9]) {
    int i, j;
    for (i = 0; i < 9; i++) {
        for (j = 0; j < 9; j++) {
            printf(BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(nums[i][j]));
            printf(" ");
            if (j == 2 || j == 5) { printf("| "); }
        }
        printf("\n");
        if (i == 2 || i == 5) { printf("---------------------------------------------------------------------------------------------\n"); }
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

// setting bits: nums[i][j] |= 1 << k         0 is first/rightmost bit
// clearing bits: nums[i][j] &= ~(1 << k)

/* STRATEGIES
 *
 * DONE   Naked Single: if a square has only one potential number, it must be that number (duh)
 * DONE   Hidden Single: if a square in a row, column, or 3x3 has multiple numbers where one of which is the only instance of that number then it must be that number
 * DONE   Naked Pair: if 2 squares in a row/column (in the same 3x3) both have the same 2 potential numbers, eliminate all other instances of those numbers in the row/column and 3x3
 *    Pointing Pair/Triple: same as naked pair but with only one number and can apply for two OR three squares in a row/column in the same 3x3
 * DONE*   Claiming Pair/Triple: if there are 2+ potential numbers left in a row/column, eliminate all other instances in the same 3x3
 *    Naked Triple: if 3 cells in a row/column (not necessarily in a 3x3) have the same set/subset of potential numbers, eliminate all other instances of those numbers in the row/column
 * DONE**   X-Wing: 2 parallel Naked Pairs, eliminate all instances of those numbers in both rows and columns
 * DONE   Hidden Pair: same as Hidden Single but with 2 potential numbers
 *    Naked Quad: Same as Naked Triple but with 4 squares in a row/column
 *
 *    *only Pair has been implemented
 *    **by technicality: X wing is simply 4 squares that create a box of claiming pairs, which the program can handle
 */
