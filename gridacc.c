#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <omp.h>

int const C_BOUNDARY = 2;	// accounting for 0's added to grid boundary
int const C_COLUMN = 4 + C_BOUNDARY;
int const C_ROW = 5 + C_BOUNDARY;
int const C_GENERATIONS = 6;
int const C_SEED = 4;
int const C_HIGH = 14;

int gridA[C_ROW][C_COLUMN] = { 0 };
int gridB[C_ROW][C_COLUMN] = { 0 };        


/* helper function to get value of all neighbors of a cell */

int computeNeighbors(int grid[][C_COLUMN], int x, int y)
{
	int sum = 0;
	int newVal = 0;
	int oldVal = grid[x][y];	
	
	for (int i = x-1; i < x+2; i++)
		for (int k = y-1; k < y+2; k++)
			sum += grid[i][k];

	if (sum % 10 == 0)
		newVal = 0;
	else if (sum < 50)
		newVal = oldVal + 3;
	else if (sum > 50 && sum < 150)
		if (oldVal - 3 < 0)
			newVal = 0;
		else
			newVal = oldVal - 3;
	else if (sum > 150)
		newVal = 1;

        /* busy work */ 
	for (int k = 0; k < 1000; k++)
        for (int l = 0; l < 1000; l++);
                
    return newVal;
}

/* fillGrid populates the array with random integers */

void fillGrid(int grid[][C_COLUMN], int high)
{
	for (int i = 1; i < C_ROW-1; i++)
		for (int k = 1; k < C_COLUMN-1; k++)
			grid[i][k] = rand() % high;
}



void printGrid(int grid[][C_COLUMN])
{
	for (int i = 1; i < C_ROW-1; i++) {
		for (int k = 1; k < C_COLUMN-1; k++)
			printf("%3d", grid[i][k]);
		printf("\n\n");
	}
	puts("");
}

int main(void)
{		
    srand(C_SEED);
    fillGrid(gridA, C_HIGH);
    printGrid(gridA); 
        
    for (int g = 1; g < C_GENERATIONS; g++) {
	printf("Gen%d--\n", g);
	printGrid(gridA);
        #pragma acc parallel loop copyin(gridA[0:C_ROW][0:C_COLUMN], gridB[0:C_ROW][C_COLUMN]) copyout(gridB[0:C_ROW][0:C_COLUMN])
        for (int i = 1; i < C_ROW-1; i++)
			for (int j = 1; j < C_COLUMN-1; j++)    	
				gridB[i][j] = computeNeighbors(gridA, i, j); 
        memcpy(gridA, gridB, sizeof gridA);	
    }
       
    printf("Gen %d--\n", C_GENERATIONS);
	printGrid(gridB);

	return 0;
}
