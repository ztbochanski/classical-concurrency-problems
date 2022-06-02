
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
// main -- process command line
//
int main(int argc, char *argv[])
{

  // option args for a valid problem must be entered, warn otherwise
  if (argc == 1)
  {
    fprintf(stderr, "usage: conprobs - enter a valid option -p, -d, -b\n");
    return 1;
  }

  // parse argument array and process arguments
  for (int i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "-p") == 0)
    {
      if (argc <= 2)
      {
        fprintf(stderr, "usage: conprobs -p, enter a valid option -n, -c\n");
        return 1;
      }
      // -p consumer producer problem
    }
    else if (strcmp(argv[i], "-d") == 0)
    {
      // -d philosopher's problem
      int option = atoi(argv[++i]);
    }
    else if (strcmp(argv[i], "-b") == 0)
    {
      // -b potion brewers problem
      int option = atoi(argv[++i]);
    }
    else
    {
      fprintf(stderr, "invalid argument, options: -p, -d, -b\n");
      exit(1);
    }
  }
}
