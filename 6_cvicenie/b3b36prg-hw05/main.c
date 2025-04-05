#include <stdio.h>
#include <stdlib.h>

#define TU_SOM printf("%s", "TU SOM\n");

enum{
  OK = 0,
  ERROR = 100
};

typedef struct
{
  int **data;
  int rows;
  int cols;
} Matrix;


int **read_matrix(int *row, int *col);
int read_matrices_operators(Matrix **matrices, char **operators, int *num_matrices, int *num_operators);
void print_matrix(int **matrix, int rows, int cols);
void free_matrix(int **matrix, int rows);

int main(int argc, char *argv[])
{
  Matrix *matrices = NULL;
  char *operators = NULL;
  int num_matrices = 0;
  int num_operators = 0;

  if (read_matrices_operators(&matrices, &operators, &num_matrices, &num_operators) != 0)
    return ERROR;

  for (int i = 0; i < num_matrices; ++i){
    print_matrix(matrices[i].data, matrices[i].rows, matrices[i].cols);
    if (i < num_operators) printf("%c", operators[i]);
    putchar('\n');
  }
  
  for (int i = 0; i < num_matrices; ++i){
    free_matrix(matrices[i].data, matrices[i].rows);
  }
  free(matrices);
  free(operators);

  return OK;
}

int read_matrices_operators(Matrix **matrices, char **operators, int *num_matrices, int *num_operators)
{
  while (1)
  {
    Matrix matrix;
    matrix.data = read_matrix(&matrix.rows, &matrix.cols);
    if (matrix.data == NULL){
      if (feof(stdin)) {
        break;
      } else {
        fprintf(stderr, "Error reading matrix!\n");
        return ERROR;
    }
    }
      
    Matrix *temp_matrices = realloc(*matrices, (*num_matrices + 1) * sizeof(Matrix));   // TODO: Increasing array size like this is not the best
    if (temp_matrices == NULL){
      for (int i = 0; i < *num_matrices; ++i){
        free_matrix((*matrices)[i].data, (*matrices)[i].rows);
      }
      free(matrices);
      free(operators);
    }
    
    *matrices = temp_matrices;
    (*matrices)[*num_matrices] = matrix;
    (*num_matrices)++;

    // Reading operator
    char operator;
    if (scanf(" %c", &operator) != 1){
      break;
    }

    char *temp_operators = realloc(*operators, (*num_operators + 1) * sizeof(char));
    if (temp_operators == NULL){
      for (int i = 0; i < *num_matrices; ++i){
        free_matrix((*matrices)[i].data, (*matrices)[i].rows);
      }
      free(matrices);
      free(operators);
    } 

    *operators = temp_operators;
    (*operators)[*num_operators] = operator;
    (*num_operators)++;
  }

  return OK;
}

int **read_matrix(int *rows, int *cols)
{
  int ret = scanf("%d %d", rows, cols);

  if (ret == EOF || ret == 0) {
    // End of input reached
    return NULL;
  }
  if (ret != 2){
    fprintf(stderr, "Error: Chybny vstup!\n");
    return NULL;
  }

  // Allocate memory for matrix
  int **matrix = (int**)malloc(*rows * sizeof(int*));
  if (matrix == NULL){
    fprintf(stderr, "Memory allocation error!");
    return NULL;
  }

  for (int i = 0; i < *rows; ++i){
    // Initialize matrix
    matrix[i] = (int*)malloc(*cols * sizeof(int));
    if (matrix[i] == NULL){
      fprintf(stderr, "Memory allocation error!");
      for (int j = 0; j < *rows; ++j){
        free(matrix[i]);
      }
      return NULL;
    }
  }

  // Read elements of the matrix
  for (int i = 0; i < *rows; ++i){
    for (int j = 0; j < *cols; ++j){
      if (scanf("%d", &matrix[i][j]) != 1){
        fprintf(stderr, "Error: Chybny vstup!\n");
        for (int k = 0; k < *rows; ++k) free(matrix[k]);
        free(matrix);
        return NULL;
      }
    }
  }
  
  return matrix;
}

void free_matrix(int **matrix, int rows)
{
  for (int i = 0; i < rows; ++i){
    free(matrix[i]);
  }
  free(matrix);
}

void print_matrix(int **matrix, int rows, int cols)
{
  for (int i = 0; i < rows; ++i){
    for (int j = 0; j < cols; ++j){
      if (j == cols - 1) printf("%d\n", matrix[i][j]);
      else printf("%d ", matrix[i][j]);
    }
  }
}
