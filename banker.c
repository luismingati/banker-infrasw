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
    fclose(file);
    exit(1);
  }

  fseek(file, 0, SEEK_END);
  if (ftell(file) == 0) {
    printf("Fail to read commands.txt\n");
    fclose(file);
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
          printf("Fail to read commands.txt\n");
          fclose(file);
          exit(1);
        }
      } else {
        spaceCount = 0;
      }
    }
    
    char *token = strtok(line, " \n");

    if (token == NULL) {
      printf("Fail to read commands.txt\n");
      fclose(file);
      exit(1);
    }

    if (strcmp(token, "*") == 0) {
      if (strtok(NULL, " \n") != NULL) {
        printf("Fail to read commands.txt\n");
        fclose(file);
        exit(1);
      }
      continue; 
    }

    if (!(strcmp(token, "RQ") == 0 || strcmp(token, "RL") == 0)) {
      printf("Fail to read commands.txt\n");
      fclose(file);
      exit(1);
    }

    columnCount++; 

    while ((token = strtok(NULL, " \n")) != NULL) {
      if (!isNumber(token)) {
        printf("Fail to read commands.txt\n");
        fclose(file);
        exit(1);
      }
      columnCount++;
    }

    if (!firstLine && (columnCount != prevColumnCount)) {
      printf("Fail to read commands.txt\n");
      fclose(file);
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

int customerHasPermission(int customerNum, int *resources, int numResources, int **need) {
  for (int i = 0; i < numResources; i++) {
    if (resources[i] > need[customerNum][i]) {
      return 0;
    }
  }
  return 1;
}

int customerHasResources( int *resources, int numResources, int *available) {
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

void allocateResources(int customerNum, int *resources, int numResources, int **allocation, int **need, int *available, int numCustomers, FILE *file) {
  if(!customerHasPermission(customerNum, resources, numResources, need)) {
    for (int i = 0; i < numResources; i++) {
      if (resources[i] > need[customerNum][i]) {
        fprintf(file, "The customer %d request ", customerNum);
        for (int j = 0; j < numResources; j++) {
          fprintf(file, "%d ", resources[j]);
        }
        fprintf(file, "was denied because exceed its maximum need\n");
        return;
      }
    }
  }

  if(!customerHasResources( resources, numResources, available)) {
    for (int i = 0; i < numResources; i++) {
      if (resources[i] > available[i]) {
        fprintf(file, "The resources ");
        for (int j = 0; j < numResources; j++) {
          fprintf(file, "%d ", available[j]);
        }
        fprintf(file, "are not enough to customer %d request ", customerNum);
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
    allocation[customerNum][i] = allocation[customerNum][i] + resources[i];
    need[customerNum][i] = need[customerNum][i] - resources[i];
  }
  
  if(isSafe(allocation, need, available, numCustomers, numResources)) {
    fprintf(file, "Allocate to customer %d the resources ", customerNum);
    for (int i = 0; i < numResources; i++) {
      fprintf(file, "%d ", resources[i]);
    }
    fprintf(file, "\n");
    return;
  } else {
    fprintf(file, "The customer %d request ", customerNum);
    for (int j = 0; j < numResources; j++) {
      fprintf(file, "%d ", resources[j]);
    }
    fprintf(file, "was denied because result in an unsafe state\n");
    for (int i = 0; i < numResources; i++) {
      available[i] = available[i] + resources[i];
      allocation[customerNum][i] = allocation[customerNum][i] - resources[i];
      need[customerNum][i] = need[customerNum][i] + resources[i];
    }
  }
}

int calcMaxDigit(int ** matrix, int numCustomers, int numResources) {
  int totalMaxDigits = 0;
  for (int j = 0; j < numResources; j++) {
    int colMaxDigits = 0;
    for (int i = 0; i < numCustomers; i++) {
      int digits = 0;
      int number = matrix[i][j];
      do {
        digits++;
        number /= 10;
      } while (number != 0);
      if (digits > colMaxDigits) {
        colMaxDigits = digits;
      }
    }
    totalMaxDigits += colMaxDigits;
  }
  return totalMaxDigits;
}


int * maxDecimalDigits(int ** matrix, int numCustomers, int numResources) {
  int * max = (int *)malloc(numResources * sizeof(int));
  for (int j = 0; j < numResources; j++) {
    max[j] = 0; 
  }
  for (int i = 0; i < numCustomers; i++) {
    for (int j = 0; j < numResources; j++) {
      int digits = 0;
      int number = matrix[i][j];
      do {
        digits++;
        number /= 10;
      } while (number != 0);

      if (digits > max[j]) {
        max[j] = digits;
      }
    }
  }

  return max;
}

void printCurrentState(int **maximum, int **allocation, int **need, int *available, int numCustomers, int numResources, FILE *file){
  fprintf(file, "MAXIMUM ");
  int max = calcMaxDigit(maximum, numCustomers, numResources);
  for(int i = 8; i < max + numResources; i++) {
    fprintf(file, " ");
  }
  fprintf(file, "| ALLOCATION ");
  int alloc = calcMaxDigit(allocation, numCustomers, numResources);
  for(int i = 11; i < alloc + numResources; i++) {
    fprintf(file, " ");
  }
  fprintf(file, "| NEED\n");

  int * maxDecimalDigitsMaximum = maxDecimalDigits(maximum, numCustomers, numResources);
  int * maxDecimalDigitsAllocation = maxDecimalDigits(allocation, numCustomers, numResources);
  int * maxDecimalDigitsNeed = maxDecimalDigits(need, numCustomers, numResources);

  for(int i = 0; i < numCustomers; i++) {
    for(int j = 0; j < numResources; j++) {
      fprintf(file, "%*d ",maxDecimalDigitsMaximum[j], maximum[i][j]);
    }
    for(int j = max+numResources; j < 8; j++) {
      fprintf(file, " ");
    }
    fprintf(file, "| ");
    for(int j = 0; j < numResources; j++) {
      fprintf(file, "%*d ",maxDecimalDigitsAllocation[j], allocation[i][j]);
    }
    for(int j = alloc+numResources; j < 11; j++) {
      fprintf(file, " ");
    }
    fprintf(file, "| ");
    for(int j = 0; j < numResources; j++) {
      fprintf(file, "%*d ",maxDecimalDigitsNeed[j], need[i][j]);
    }
    fprintf(file, "\n");
  }
  fprintf(file, "AVAILABLE ");
  for(int i = 0; i < numResources; i++) {
    fprintf(file, "%d ", available[i]);
  }
  fprintf(file, "\n");
}

int canRelease(int customerNum, int *resources, int numResources, int **allocation) {
  for (int i = 0; i < numResources; i++) {
    if (resources[i] > allocation[customerNum][i]) {
      return 0;
    }
  }
  return 1;
}

void releaseResources(int customerNum, int *resources, int numResources, int **allocation, int **need, int *available, FILE *file) {
  if(!canRelease(customerNum, resources, numResources, allocation)) {
    for (int i = 0; i < numResources; i++) {
      if (resources[i] > allocation[customerNum][i]) {
        fprintf(file, "The customer %d released ", customerNum);
        for (int j = 0; j < numResources; j++) {
          fprintf(file, "%d ", resources[j]);
        }
        fprintf(file, "was denied because exceed its maximum allocation\n");
        return;
      }
    }
  }

  for (int i = 0; i < numResources; i++) {
    available[i] += resources[i];
    allocation[customerNum][i] -= resources[i];
    need[customerNum][i] += resources[i];
  }

  fprintf(file, "Release from customer %d the resources ", customerNum);
  for (int i = 0; i < numResources; i++) {
    fprintf(file, "%d ", resources[i]);
  }
  fprintf(file, "\n");
}

void executeCommand(char *command, int numResources, int **maximum, int **allocation, int **need, int *available, int numCustomers, FILE *file) {
  char cmdType[3];
  int customerNum, resources[numResources];

  sscanf(command, "%s", cmdType);

  if (strcmp(cmdType, "RQ") == 0) {
    sscanf(command, "%s %d", cmdType, &customerNum);
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
    allocateResources(customerNum, resources, numResources, allocation, need, available, numCustomers, file);
  } else if (strcmp(cmdType, "RL") == 0) {
      sscanf(command, "%s %d", cmdType, &customerNum);
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
      releaseResources(customerNum, resources, numResources, allocation, need, available, file);
  } else if (strcmp(cmdType, "*") == 0) {
      printCurrentState(maximum, allocation, need, available, numCustomers, numResources, file);
  }
}

void readCommandFile(int numResources, int **maximum, int **allocation, int **need, int *available, int numCustomers) {
  FILE *file = fopen("commands.txt", "r");
  FILE *output = fopen("result.txt", "w");
  char command[2000];

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
