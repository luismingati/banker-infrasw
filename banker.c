#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct ColsLines {
  int columns;
  int lines;
} ColsLines;

typedef struct {
  char command[3];  
  int customer;
  int *resources;
} Command;

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

  fseek(file, 0, SEEK_END);
  if (ftell(file) == 0) {
    printf("Fail to read commands.txt\n");
    exit(1);
  }
  rewind(file);

  char line[2000];
  int prevColumnCount = 0;
  int firstLine = 1;

  while (fgets(line, sizeof(line), file) != NULL) {
    int columnCount = 0;
    int spaceCount = 0;

    for (int i = 0; i < strlen(line); i++) {
      if (line[i] == ' ') {
        spaceCount++;
        if (spaceCount > 1) {
          fclose(file);
          printf("Fail to read commands.txt\n");
          exit(1);
        }
      } else {
        spaceCount = 0;
      }
    }
    
    char *token = strtok(line, " \n");

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
    printf("Fail to read customer.txt\n");
    exit(1);
  }

  fseek(file, 0, SEEK_END);

  if (ftell(file) == 0) {
    printf("Fail to read customer.txt\n");
    exit(1);
  }

  rewind(file);

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
      if (!((line[i] >= '0' && line[i] <= '9') || line[i] == ',' || line[i] == '\n')) {
        isValid = 0;
        break;
      }

      if (line[i] == ',') {
        if (!lastIsNum) {
          isValid = 0;
          break;
        }
        columnCount++;
        lastIsNum = 0;
      } else if (line[i] >= '0' && line[i] <= '9') {
        lastIsNum = 1;
      } 
    }

    if (lastIsNum) {
      columnCount++;
    } else if (line[length-1] != '\n') { 
      isValid = 0;
      break;
    }

    if (lineCount > 1 && columnCount != prevColumnCount) {
      isValid = 0;
      break;
    }

    prevColumnCount = columnCount;
    if (!isValid) {
      break;
    }
  }

  fclose(file);

  if (!isValid) {
    printf("Fail to read customer.txt\n");
    exit(1);
  }

  ColsLines colsLines;
  colsLines.columns = prevColumnCount;
  colsLines.lines = lineCount;
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

int customerHasPermission(int customer_num, int *resources, int numResources, int **maximum, int **allocation, int **need, int *available, int numCustomers) {
  for (int i = 0; i < numResources; i++) {
    if (resources[i] > need[customer_num][i]) {
      return 0;
    }
  }
  return 1;
}

int customerHasResources(int customer_num, int *resources, int numResources, int **maximum, int **allocation, int **need, int *available, int numCustomers) {
  for (int i = 0; i < numResources; i++) {
    if (resources[i] > available[i]) {
      return 0;
    }
  }
  return 1;
}

int isSafe(int **allocation, int **need, int *available, int numCustomers, int numResources) {
  int work[numResources];
  int finish[numCustomers];

  for (int i = 0; i < numResources; i++) {
    work[i] = available[i];
  }

  for (int i = 0; i < numCustomers; i++) {
    finish[i] = 0;
  }

  int count = 0;
  while (count < numCustomers) {
    int found = 0;
    for (int i = 0; i < numCustomers; i++) {
      if (finish[i] == 0) {
        int j;
        for (j = 0; j < numResources; j++) {
          if (need[i][j] > work[j]) {
            break;
          }
        }
        if (j == numResources) {
          for (int k = 0; k < numResources; k++) {
            work[k] += allocation[i][k];
          }
          found = 1;
          finish[i] = 1;
          count++;
        }
      }
    }
    if (found == 0) {
      return 0;
    }
  }
  return 1;
}

void allocateResources(int customer_num, int *resources, int numResources, int **maximum, int **allocation, int **need, int *available, int numCustomers, FILE *file) {
  if(!customerHasPermission(customer_num, resources, numResources, maximum, allocation, need, available, numCustomers)) {
    for (int i = 0; i < numResources; i++) {
      if (resources[i] > need[customer_num][i]) {
        fprintf(file, "The customer %d request ", customer_num);
        for (int j = 0; j < numResources; j++) {
          fprintf(file, "%d ", resources[j]);
        }
        fprintf(file, "was denied because it exceeds its maximum need\n");
        return;
      }
    }
  }

  if(!customerHasResources(customer_num, resources, numResources, maximum, allocation, need, available, numCustomers)) {
    for (int i = 0; i < numResources; i++) {
      if (resources[i] > available[i]) {
        fprintf(file, "The resources ");
        for (int j = 0; j < numResources; j++) {
          fprintf(file, "%d ", available[j]);
        }
        fprintf(file, "are not enough for customer %d request ", customer_num);
        for (int j = 0; j < numResources; j++) {
          fprintf(file, "%d ", resources[j]);
        }
        fprintf(file, "\n");
        return;
      }
    }
    return;
  }

  for (int i = 0; i < numResources; i++) {
    available[i] = available[i] - resources[i];
    allocation[customer_num][i] = allocation[customer_num][i] + resources[i];
    need[customer_num][i] = need[customer_num][i] - resources[i];
  }
  
  if(isSafe(allocation, need, available, numCustomers, numResources)) {
    fprintf(file, "Allocate to customer %d the resources ", customer_num);
    for (int i = 0; i < numResources; i++) {
      fprintf(file, "%d ", resources[i]);
    }
    fprintf(file, "\n");
    return;
  } else {
    fprintf(file, "The customer %d request ", customer_num);
    for (int j = 0; j < numResources; j++) {
      fprintf(file, "%d ", resources[j]);
    }
    fprintf(file, "was denied because it results in an unsafe state \n");
    for (int i = 0; i < numResources; i++) {
      available[i] = available[i] + resources[i];
      allocation[customer_num][i] = allocation[customer_num][i] - resources[i];
      need[customer_num][i] = need[customer_num][i] + resources[i];
    }
  }
}

void printCurrentState(int **maximum, int **allocation, int **need, int *available, int numCustomers, int numResources, FILE *file) {
  fprintf(file, "MAXIMUM | ALLOCATION | NEED\n");
  for (int i = 0; i < numCustomers; i++) {
    for (int j = 0; j < numResources; j++) {
      fprintf(file, "%d ", maximum[i][j]);
      if (j == numResources - 1) {
        fprintf(file, "  | ");
      }
    }

    for (int j = 0; j < numResources; j++) {
      fprintf(file, "%d ", allocation[i][j]);
      if (j == numResources - 1) {
        fprintf(file, "  | ");
      }
    }

    for (int j = 0; j < numResources; j++) {
      fprintf(file, "%d ", need[i][j]);
      if (j == numResources - 1) {
        fprintf(file, "\n");
      }
    }
  }

  fprintf(file, "AVAILABLE ");
  for (int i = 0; i < numResources; i++) {
    fprintf(file, "%d ", available[i]);
  }
  fprintf(file, "\n");
}


int canRelease(int customer_num, int *resources, int numResources, int **maximum, int **allocation, int **need, int *available, int numCustomers) {
  for (int i = 0; i < numResources; i++) {
    if (resources[i] > allocation[customer_num][i]) {
      return 0;
    }
  }
  return 1;
}

void releaseResources(int customer_num, int *resources, int numResources, int **maximum, int **allocation, int **need, int *available, int numCustomers, FILE *file) {
  if(!canRelease(customer_num, resources, numResources, maximum, allocation, need, available, numCustomers)) {
    for (int i = 0; i < numResources; i++) {
      if (resources[i] > allocation[customer_num][i]) {
        fprintf(file, "The customer %d released ", customer_num);
        for (int j = 0; j < numResources; j++) {
          fprintf(file, "%d ", resources[j]);
        }
        fprintf(file, "was denied because it exceeds its maximum allocation \n");
        return;
      }
    }
  }

  for (int i = 0; i < numResources; i++) {
    available[i] += resources[i];
    allocation[customer_num][i] -= resources[i];
    need[customer_num][i] += resources[i];
  }

  fprintf(file, "Release to customer %d the resources ", customer_num);
  for (int i = 0; i < numResources; i++) {
    fprintf(file, "%d ", resources[i]);
  }
  fprintf(file, "\n");
}

void executeCommand(char *command, int numResources, int **maximum, int **allocation, int **need, int *available, int numCustomers, FILE *file) {
  char cmdType[3];
  int customer_num, resources[numResources];

  sscanf(command, "%s", cmdType);

  if (strcmp(cmdType, "RQ") == 0) {
    sscanf(command, "%s %d", cmdType, &customer_num);
    char *token = strtok(command, " ");
    token = strtok(NULL, " ");

    for (int i = 0; i < numResources; i++) {
      token = strtok(NULL, " ");
      if (token == NULL) {
        printf("Invalid resource format in command: %s\n", command);
        return;
      }
      resources[i] = atoi(token);
    }
    allocateResources(customer_num, resources, numResources, maximum, allocation, need, available, numCustomers, file);
  } else if (strcmp(cmdType, "RL") == 0) {
      sscanf(command, "%s %d", cmdType, &customer_num);
      char *token = strtok(command, " ");
      token = strtok(NULL, " "); 

      for (int i = 0; i < numResources; i++) {
        token = strtok(NULL, " ");
        if (token == NULL) {
          printf("Invalid resource format in command: %s\n", command);
          return;
        }
        resources[i] = atoi(token);
      }
      releaseResources(customer_num, resources, numResources, maximum, allocation, need, available, numCustomers, file);
  } else if (strcmp(cmdType, "*") == 0) {
      printCurrentState(maximum, allocation, need, available, numCustomers, numResources, file);
  }
}

void readCommandFile(int numResources, int **maximum, int **allocation, int **need, int *available, int numCustomers) {
  FILE *file = fopen("commands.txt", "r");
  FILE *output = fopen("result.txt", "w");
  char command[256];

  while (fgets(command, sizeof(command), file) != NULL) {
    executeCommand(command, numResources, maximum, allocation, need, available, numCustomers, output);
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
  int *available = (int *)malloc(customerColumns.columns * sizeof(int));
  for (int i = 0; i < customerColumns.lines; i++) {
    maximum[i] = (int *)malloc(customerColumns.columns * sizeof(int));
    allocation[i] = (int *)malloc(customerColumns.columns * sizeof(int));
    need[i] = (int *)malloc(customerColumns.columns * sizeof(int));
  }
  for(int i = 0; i < customerColumns.columns; i++) {
    available[i] = atoi(argv[i+1]);
  }
  initializeMatrixData(maximum, allocation, need, customerColumns.columns, customerColumns.lines);

  readCommandFile(customerColumns.columns, maximum, allocation, need, available, customerColumns.lines);

  return 0;
}
