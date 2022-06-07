#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>

int n, m, n_sec, m_sec;
char n_string[20], m_string[20], n_sec_string[20], m_sec_string[20];
char number_string[512], dimensions[50] = "", spaces[2048];
char number_char = '@';
int index_ = -1;
int row = 0;
int number, numbers[512];
bool first_number = true;
bool new_number = true;
bool new_space = false;
int second_matrix = 0;
char c;
int k = 0, s = 0;

int get_dimensions(FILE *f1, FILE *f2)
{
	do
	{
		first_number = true;

		do
		{
			if (!second_matrix)
			{
				c = fgetc(f1);
			}
			else
			{
				c = fgetc(f2);
			}

			if (c < '0' || c > '9')
			{
				if (!second_matrix)
				{
					first_number = false;
					strncat(dimensions, &c, 1);
				}
				else
				{
					first_number = false;
				}
			}
			else
			{
				if (!second_matrix)
				{
					if (first_number)
					{
						strncat(n_string, &c, 1);
						strncat(dimensions, &c, 1);
					}
					else
					{
						strncat(m_string, &c, 1);
						strncat(dimensions, &c, 1);
					}
				}
				else
				{
					if (first_number)
					{
						strncat(n_sec_string, &c, 1);
					}
					else
					{
						strncat(m_sec_string, &c, 1);
					}
				}
			}

		} while (c != '\n');

		second_matrix++;
	} while (second_matrix < 2);

	if (strcmp(n_string, n_sec_string) || strcmp(m_string, m_sec_string))
	{
		printf("Error: matrix must have the same size.\n\n");

		return 1;
	}

	n = atoi(n_string);
	m = atoi(m_string);

	return 0;
}

void get_matrices(FILE *f1, FILE *f2, int matrix1[][m], int matrix2[][m])
{
	do
	{
		do
		{
			if (!second_matrix)
			{
				c = fgetc(f1);
			}
			else
			{
				c = fgetc(f2);
			}

			if (c == '\n')
			{
				if (!second_matrix)
				{
					strncat(spaces, &c, 1);
				}

				if (new_space)
				{
					number = atoi(number_string);
					numbers[index_] = number;
					memset(number_string, 0, sizeof(number_string));
					new_space = false;
				}

				for (int i = 0; i <= index_; i++)
				{
					if (!second_matrix)
					{
						matrix1[row][i] = numbers[i];
					}
					else
					{
						matrix2[row][i] = numbers[i];
					}
				}

				row++;
				index_ = -1;
				memset(numbers, 0, sizeof(numbers));
				new_number = true;
				new_space = false;
			}
			else
			{
				if (c < '0' || c > '9')
				{
					if (!second_matrix)
					{
						strncat(spaces, &c, 1);
					}

					new_number = true;

					if (new_space)
					{
						number = atoi(number_string);
						numbers[index_] = number;
						memset(number_string, 0, sizeof(number_string));
						new_space = false;
					}
				}
				else
				{
					if (new_number)
					{
						if (!second_matrix)
						{
							strncat(spaces, &number_char, 1);
						}

						strncat(number_string, &c, 1);
						index_++;
					}
					else
					{
						strncat(number_string, &c, 1);
					}

					new_number = false;
					new_space = true;
				}
			}
		} while (c != EOF);

		second_matrix++;
		memset(number_string, 0, sizeof(number_string));
		memset(numbers, 0, sizeof(numbers));
		index_ = -1;
		row = 0;
	} while (second_matrix < 2);
}

void print_result(int (*result_matrix)[n])
{
	printf("%s", dimensions);
	while (spaces[s] != EOF)
	{
		if (spaces[s] == '@')
		{
			printf("%d", result_matrix[row][k]);
			k++;
		}
		else
		{
			if (spaces[s] == '\n')
			{
				row++;
				k = 0;
			}

			printf("%c", spaces[s]);
		}

		s++;
	}
	printf("\n");
}

int main(int argc, char *argv[])
{
	FILE *f1, *f2;

	if (argc != 3)
	{
		printf("Error: please enter two files.\n\n");
	}

	//OPEN FILES
	f1 = fopen(argv[1], "r");
	f2 = fopen(argv[2], "r");

	if (NULL == f1)
	{
		printf("Error: first file can't be opened.\n\n");

		return 1;
	}

	if (NULL == f2)
	{
		printf("Error: second file can't be opened.\n\n");

		return 1;
	}

	//GET DIMENSIONS
	if (get_dimensions(f1, f2))
	{
		return 1;
	}

	second_matrix = 0;

	//SETUP SHARED MEMORY
	int(*matrix1)[m] = mmap(NULL, n * sizeof(int), PROT_READ | PROT_WRITE,
							MAP_SHARED | MAP_ANONYMOUS, 0, 0);
	int(*matrix2)[m] = mmap(NULL, n * sizeof(int), PROT_READ | PROT_WRITE,
							MAP_SHARED | MAP_ANONYMOUS, 0, 0);
	int(*result_matrix)[n] = mmap(NULL, m * sizeof(int), PROT_READ | PROT_WRITE,
								  MAP_SHARED | MAP_ANONYMOUS, 0, 0);

	if (matrix1 == MAP_FAILED || matrix2 == MAP_FAILED || result_matrix == MAP_FAILED)
	{
		printf("Error: failed to setup shared memory.\n\n");

		return 1;
	}

	//GET MATRICES
	get_matrices(f1, f2, matrix1, matrix2);

	//CLOSE FILES
	fclose(f1);
	fclose(f2);

	//CREATE CHILD PROCESSES
	for (int i = 0; i < m; i++)
	{
		pid_t pid;

		if ((pid = fork()) < 0)
		{
			printf("Error: fork failed.\n\n");

			return 1;
		}

		//CALCULATE SUM OF TWO COLUMNS PER CHILD PROCESS
		if (pid == 0)
		{
			for (int j = 0; j < m; j++)
			{
				if (j % m == i)
				{
					for (int k = 0; k < n; k++)
					{
						result_matrix[k][j] = matrix1[k][j] + matrix2[k][j];
					}
				}
			}

			return 0;
		}
	}

	//WAIT FOR CHILD PROCESSES TO FINISH
	for (int i = 0; i < m; i++)
	{
		if (waitpid(-1, NULL, 0) < 0)
		{
			printf("Error: waitpid failed.\n\n");

			return 1;
		}
	}

	//PRINT RESULT
	print_result(result_matrix);

	//RELEASE SHARED MEMORY
	if (munmap(matrix1, sizeof(matrix1)) < 0 || munmap(matrix2, sizeof(matrix2)) < 0 || munmap(result_matrix, sizeof(result_matrix)) < 0)
	{
		printf("Error: failed to release shared memory.\n\n");

		return 1;
	}

	return 0;
}
