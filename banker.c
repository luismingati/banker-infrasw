#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct ColsLines {
  int columns;
  int lines;
} ColsLines;



int isNumber(char *str) {
  for (int i = 0; str[i] != '\0'; i++) {
    if (!isdigit(str[i]) && str[i] != '-') {
      return 0;
    }
  }
  return 1;
}

int checkCommands() {
  FILE *file = fopen("commands.txt", "r");
  if (file == NULL) {
    printf("Fail to read commands.txt\n");
    exit(1);
  }

  char line[2000];
  int prevColumnCount = 0;
  int firstLine = 1;

  while (fgets(line, sizeof(line), file) != NULL) {
    char *token = strtok(line, " \n");
    
    int columnCount = 0;

    if (token == NULL) {
      printf("Fail to read commands.txt\n");
      exit(1);
    }

    if (strcmp(token, "*") == 0) {
      if (strtok(NULL, " \n") != NULL) {
        printf("Fail to read commands.txt\n");
        exit(1);
      }
      continue; 
    }

    if (!(strcmp(token, "RQ") == 0 || strcmp(token, "RL") == 0)) {
      printf("Fail to read commands.txt\n");
      exit(1);
    }

    columnCount++; 

    while ((token = strtok(NULL, " \n")) != NULL) {
      if (!isNumber(token)) {
        printf("Fail to read commands.txt\n");
        exit(1);
      }
      columnCount++;
    }

    if (!firstLine && (columnCount != prevColumnCount)) {
      printf("Fail to read commands.txt\n");
      exit(1);
    }

    prevColumnCount = columnCount;
    firstLine = 0;
  }
  return prevColumnCount;
  fclose(file);
}

ColsLines checkCustomer() {
  FILE *file = fopen("customer.txt", "r");
  if (file == NULL) {
    printf("1Fail to read customer.txt\n");
    exit(1);
  }

  char line[2000];
  int prevColumnCount = 0;
  int lineCount = 0;
  int isValid = 1;

  while (fgets(line, sizeof(line), file)) {
    lineCount++;
    int length = strlen(line);
    int columnCount = 0;
    int lastIsNum = 0;

    for (int i = 0; i < length; i++) {
      if (line[i] == ',') {
        if (!lastIsNum) {
          printf("2Fail to read customer.txt\n");
          exit(1);
        }
        columnCount++;
        lastIsNum = 0;
      } else if (line[i] >= '0' && line[i] <= '9') {
        lastIsNum = 1;
      } 
    }

    if (lastIsNum) {
      columnCount++;
    } else {
      printf("3Fail to read customer.txt\n");
      exit(1);
    }

    if (lineCount > 1 && columnCount != prevColumnCount) {
      printf("4Fail to read customer.txt\n");
      exit(1);
    }

    prevColumnCount = columnCount;
    if (!isValid) {
      printf("5Fail to read customer.txt\n");
      exit(1);
    }
  }


  if(!isValid) {
    printf("6Fail to read customer.txt\n");
    exit(1);
  }

  ColsLines colsLines;
  colsLines.columns = prevColumnCount;
  colsLines.lines = lineCount;

  printf("columns: %d\n", colsLines.columns);
  printf("lines: %d\n", colsLines.lines);
  fclose(file);
  return colsLines;
}

void checkCompatibilityCustomerAndCLI(int columns, int argc) {
  if (columns != argc - 1) {
    printf("Incompatibility between customer.txt and command line\n");
    exit(1);
  }
}
void checkCompatibilityCommandsAndCLI(int columns, int argc) {
  if (columns-2 != argc - 1) {
    printf("Incompatibility between commands.txt and command line\n");
    exit(1);
  }
}

// void initializeMatrix(int ** maximum, int ** alocation, int ** need, int * available, int cols, int lines) {
//   for(int i = 0; i < lines; i++) {
//     for(int j = 0; j < cols; j++) {
//       alocation[i][j] = 0;
//     }
//   }
// }

void initializeMatrixData(int ** maximum, int ** allocation, int ** need, int cols, int lines) {
  FILE *file = fopen("customer.txt", "r");

  char line[2000];
  int lineNum = 0;

  while (fgets(line, sizeof(line), file)) {
    char *token = strtok(line, ",");
    int colNum = 0;
    while (token != NULL && colNum < cols) {
      maximum[lineNum][colNum] = atoi(token);
      need[lineNum][colNum] = atoi(token);
      allocation[lineNum][colNum] = 0;
      token = strtok(NULL, ",");
      colNum++;
    }
    lineNum++;
  }

  fclose(file);
}


int main(int argc, char *argv[]) {
  int commandsColumns;
  ColsLines customerColumns;

  commandsColumns = checkCommands();
  customerColumns = checkCustomer();

  checkCompatibilityCustomerAndCLI(customerColumns.columns, argc);
  checkCompatibilityCommandsAndCLI(commandsColumns, argc);

  int **maximum = (int **)malloc(customerColumns.lines * sizeof(int *));
  int **allocation = (int **)malloc(customerColumns.lines * sizeof(int *));
  int **need = (int **)malloc(customerColumns.lines * sizeof(int *));
  int *available = (int *)malloc(customerColumns.lines * sizeof(int));
  for (int i = 0; i < customerColumns.lines; i++) {
    maximum[i] = (int *)malloc(customerColumns.columns * sizeof(int));
    allocation[i] = (int *)malloc(customerColumns.columns * sizeof(int));
    need[i] = (int *)malloc(customerColumns.columns * sizeof(int));
  }
  for(int i = 0; i < customerColumns.columns; i++) {
    available[i] = atoi(argv[i+1]);
  }
  initializeMatrixData(maximum, allocation, need, customerColumns.columns, customerColumns.lines);
  
  return 0;
}
