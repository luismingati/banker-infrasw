#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

int checkCustomer() {
  FILE *file = fopen("customer.txt", "r");
  if (file == NULL) {
    printf("Fail to read customer.txt\n");
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
          printf("Fail to read customer.txt\n");
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
      printf("Fail to read customer.txt\n");
      exit(1);
    }

    if (lineCount > 1 && columnCount != prevColumnCount) {
      printf("Fail to read customer.txt\n");
      exit(1);
    }

    prevColumnCount = columnCount;
    if (!isValid) {
      printf("Fail to read customer.txt\n");
      exit(1);
    }
  }

  if(!isValid) {
    printf("Fail to read customer.txt\n");
    exit(1);
  }
  return prevColumnCount;
  fclose(file);
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

int main(int argc, char *argv[]) {
  int customerColumns = 0, commandsColumns = 0;
  commandsColumns = checkCommands();
  customerColumns = checkCustomer();
  checkCompatibilityCustomerAndCLI(customerColumns, argc);
  checkCompatibilityCommandsAndCLI(commandsColumns, argc);
  
  return 0;
}
