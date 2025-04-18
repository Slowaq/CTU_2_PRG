#include <stdio.h>
#include <stdlib.h>

#define TU_SOM printf("%s", "TU SOM\n");

enum{
  OK = 0,
  FINE = 1,  // Used in functions so that program doesnt check for 0 on failiture
  ERROR = 100
};

typedef struct
{
  int **data;
  int rows;
  int cols;
} Matrix;

typedef struct
{
  int *output_values;
  char *output_types;
  int output_top;
} PostfixOutput;

int **safe_alloc(int r, int c);
void cleanup_and_exit(Matrix *mats, int nm, char *ops, int *vals, char *types);
int **allocate_matrix(int rows, int cols);
int **read_matrix(int *row, int *col);
int read_matrices_operators(Matrix **matrices, char **operators, int *num_matrices, int *num_operators);
void print_matrix(int **matrix, int rows, int cols);
void free_matrix(int **matrix, int rows);
int precedence(char operator);
void postfix(char **operators, int *num_matrices, int *num_operators, PostfixOutput *out);
void sum(Matrix *matrix1, Matrix *matrix2, int **output);
void subtract(Matrix *matrix1, Matrix *matrix2, int **output);
void multiply(Matrix *matrix1, Matrix *matrix2, int **output);
int check_dimensions(Matrix *matrix1, Matrix *matrix2, char operator);
int **eval(Matrix *matrix1, Matrix *matrix2, char operator);
Matrix *postfix_eval(Matrix **matrices, char **operators, int num_matrices, int num_operators, PostfixOutput postfix);

int main(int argc, char *argv[])
{
  Matrix *matrices = NULL;
  char *operators = NULL;
  int num_matrices = 0;
  int num_operators = 0;

  if (read_matrices_operators(&matrices, &operators, &num_matrices, &num_operators) != 0)
    return ERROR;

  // Allocate memory for output of postfix conversion
  int *output_values = (int*)malloc(sizeof(int) * (num_matrices + num_operators));   // Storing tokens of postfix expression
  char *output_types = (char*)malloc(sizeof(char) * (num_matrices + num_operators)); // 'm' - matrix, 'o' - operator

  if (!output_values || !output_types) {
    fprintf(stderr, "Memory allocation failed\n");
    cleanup_and_exit(matrices, num_matrices, operators, output_values, output_types);
  }

  PostfixOutput postfix_output = {.output_values = output_values, .output_types = output_types, .output_top = 0};

  postfix(&operators, &num_matrices, &num_operators, &postfix_output);

  Matrix *result = postfix_eval(&matrices, &operators, num_matrices, num_operators, postfix_output);
  if (result == NULL) {
    fprintf(stderr, "Error: Chybny vstup!\n");
    cleanup_and_exit(matrices, num_matrices, operators, output_values, output_types);
  } else {
    printf("%d %d\n", (*result).rows, (*result).cols);
    print_matrix((*result).data, (*result).rows, (*result).cols);
  }
    
  for (int i = 0; i < num_matrices; ++i){
    free_matrix(matrices[i].data, matrices[i].rows);
  }
  free(matrices);
  free(operators);
  free(output_values);
  free(output_types);

  return OK;
}

int **safe_alloc(int r, int c) 
{
  int **m = allocate_matrix(r, c);
  if (!m) { fprintf(stderr, "Alloc error\n"); return NULL; }
  return m;
}

void cleanup_and_exit(Matrix *mats, int nm, char *ops, int *vals, char *types) 
{
  for (int i = 0; i < nm; i++) free_matrix(mats[i].data, mats[i].rows);
  free(mats);
  free(ops);
  free(vals);
  free(types);
  exit(ERROR);
}

int **allocate_matrix(int rows, int cols)
{
  // Allocate memory for matrix
  int **output = (int**)malloc(rows * sizeof(int*));
  if (output == NULL){
    fprintf(stderr, "Memory allocation error!");
    return NULL;
  }

  for (int i = 0; i < rows; ++i){
    // Initialize matrix
    output[i] = (int*)malloc(cols * sizeof(int));
    if (output[i] == NULL){
      fprintf(stderr, "Memory allocation error!");
      for (int j = 0; j < rows; ++j){
        free(output[i]);
      }
      free(output);
      return NULL;
    }
  }
  return output;
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
        //fprintf(stderr, "Error reading matrix!\n");
        return ERROR;
      }
    }
      
    Matrix *temp_matrices = realloc(*matrices, (*num_matrices + 1) * sizeof(Matrix));   // TODO: Increasing array size like this is not the best
    if (temp_matrices == NULL){
      fprintf(stderr, "Memory allocation failed\n");
      cleanup_and_exit(*matrices, *num_matrices, *operators, NULL, NULL);
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
      fprintf(stderr, "Memory allocation failed");
      cleanup_and_exit(*matrices, *num_matrices, *operators, NULL, NULL);
      return ERROR;

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

  int **matrix = allocate_matrix(*rows, *cols);
  if (matrix == NULL) return NULL;

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

int precedence(char operator)
{
  if (operator == '*') return 3;
  if (operator == '+') return 2;
  if (operator == '-') return 2;
  return ERROR;
}

void postfix(char **operators, int *num_matrices, int *num_operators, PostfixOutput *out)
{
  /*
    This method takes in array of matrices and array of operators, where operator at
    position i corresponds to matrices at positions (i - 1) and i and converts it to 
    postfix notation using Shunting yard algorith.

    Result is stored in out structure, other variables are not tempered with
  */

  char operator_stack[*num_operators];
  int operator_top = 0;

  for (int i = 0; i < *num_matrices; ++i){
    (*out).output_values[(*out).output_top] = i;
    (*out).output_types[(*out).output_top] = 'm';
    (*out).output_top++;

    if (i < *num_operators){     // Since there is one less # of operators than # of matrices
      char current_op = (*operators)[i];
      while (operator_top > 0 && precedence(operator_stack[operator_top - 1]) >= precedence(current_op)){
        (*out).output_values[(*out).output_top] = operator_stack[--operator_top];
        (*out).output_types[(*out).output_top] = 'o';
        (*out).output_top++;
      }
      operator_stack[operator_top++] = current_op;
    }
  }

  // Pop remaining operators
  while (operator_top > 0) {
    (*out).output_values[(*out).output_top] = operator_stack[--operator_top];
    (*out).output_types[(*out).output_top] = 'o';
    (*out).output_top++;
  }
}

void sum(Matrix *matrix1, Matrix *matrix2, int **output)
{
  for (int i = 0; i < (*matrix1).rows; ++i){
    for (int j = 0; j < (*matrix1).cols; ++j){
      output[i][j] = (*matrix1).data[i][j] + (*matrix2).data[i][j];
    }
  }
}

void subtract(Matrix *matrix1, Matrix *matrix2, int **output)
{
  for (int i = 0; i < (*matrix1).rows; ++i){
    for (int j = 0; j < (*matrix1).cols; ++j){
      output[i][j] = (*matrix1).data[i][j] - (*matrix2).data[i][j];
    }
  }
}

void multiply(Matrix *matrix1, Matrix *matrix2, int **output)
{
  for (int i = 0; i < (*matrix1).rows; ++i){
    for (int j = 0; j < (*matrix2).cols; ++j){
      int sum = 0;
      for (int k = 0; k < (*matrix1).cols; ++k){
        sum += (*matrix1).data[i][k] * (*matrix2).data[k][j];
      }
      output[i][j] = sum;
    }
  }
}

int check_dimensions(Matrix *matrix1, Matrix *matrix2, char operator)
{
  if (operator == '*'){
    if ((*matrix1).cols != (*matrix2).rows){
      //fprintf(stderr, "Invalid matrix dimensions!");
      return 0;
    }
  } else{
    if ((*matrix1).rows != (*matrix2).rows || (*matrix1).cols != (*matrix2).cols){
      //fprintf(stderr, "Invalid matrix dimensions!");
      return 0;
    }
  }
  return FINE;
}

int **eval(Matrix *matrix1, Matrix *matrix2, char operator)
{
  if (operator == '+'){
    if (check_dimensions(matrix1, matrix2, operator) == 0) return NULL;

    int **output = safe_alloc((*matrix1).rows, (*matrix2).cols);
    if (output)
    {
      sum(matrix1, matrix2, output);
      return output;
    }
  } 
  
  else if (operator == '-'){
    if (check_dimensions(matrix1, matrix2, operator) == 0) return NULL;

    int **output = safe_alloc((*matrix1).rows, (*matrix2).cols);
    if (output)
    {
      subtract(matrix1, matrix2, output);
      return output;
    }
  } 
  
  else if (operator == '*'){
    if (check_dimensions(matrix1, matrix2, operator) == 0) return NULL;

    int **output = safe_alloc((*matrix1).rows, (*matrix2).cols);
    if (output)
    {
      multiply(matrix1, matrix2, output);
      return output;
    }
  }

  return NULL;
}

Matrix *postfix_eval(Matrix **matrices, char **operators, int num_matrices, int num_operators, PostfixOutput postfix) // TODO: Memory clean-up!!
{
  // Create a stack to hold pointers to Matrix structures.
  Matrix **stack = malloc(sizeof(Matrix*) * (num_matrices + num_operators));
  if (!stack) {
      fprintf(stderr, "Memory allocation failed for stack\n");
      return NULL;
  }
  int stack_top = 0;

  for (int token = 0; token < postfix.output_top; ++token){
    if (postfix.output_types[token] == 'm'){
      int index = postfix.output_values[token];
      stack[stack_top++] = &((*matrices)[index]);
    }
    else if (postfix.output_types[token] == 'o'){
      if (stack_top < 2){
        //fprintf(stderr, "Insufficient number of matrices!");
        free(stack);
        return NULL;
      }
      
      Matrix *right = stack[--stack_top];
      Matrix *left = stack[--stack_top];
      char operator = (char)postfix.output_values[token];
      int **result_data = eval(left, right, operator);
      if (result_data == NULL){
        //fprintf(stderr, "Calculation did not proceed!\n");
        free(stack);
        return NULL;
      }

      Matrix *result = malloc(sizeof(Matrix));
      if (!result) {
          fprintf(stderr, "Memory allocation failed for result matrix\n");
          free(stack);
          return NULL;
      }
      if (operator == '*') {
          (*result).rows = (*left).rows;
          (*result).cols = (*right).cols;
      } else {
          (*result).rows = (*left).rows;
          (*result).cols = (*left).cols;
      }
      (*result).data = result_data;

      stack[stack_top++] = result;
    }
  }
  if (stack_top != 1){
    //fprintf(stderr, "Error: Stack did not end with a single result\n");
    free(stack);
    return NULL;
  }

  Matrix *final = stack[0];
  free(stack);
  return final;
}
