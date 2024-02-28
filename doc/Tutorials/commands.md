# Create system commands for the shell

If you want to create a system command for the MarkedRain OS (and Os2020), here are the steps:

## Step 1: Create your C file

In the source directory "src/kernel/apps/", create a new .C file that has your code. Example:

    int helloworld(int argc, char **argv) {
        puts("hello world my guy what you doin'?");
    }

## Step 2: Adding file to the built-in list

Open the file "src/kernel/apps/builtins.h" and declare a new integer function. Example:

    int helloworld(int argc, char **argv);

## Step 3: Adding file to the interpretor

Open the file "src/kernel/apps/shell.c" and find the "tryExecBuiltin" function. Within the builtins list, create an if condition for your command like this:

    if (strcmp(app, "helloworld") == 0)
        return enter(ctxt, helloworld, argc, argv);

or multiple conditions like this:

    if (strcmp(app, "hworld") == 0 || strcmp(app, "helloworld"))
        return enter(ctxt, helloworld, argc, argv);
