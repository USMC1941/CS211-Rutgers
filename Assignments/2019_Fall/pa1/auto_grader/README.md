# pa1 Autograder: Introduction to C

This is your `auto_grader` directory. This directory contains your
`auto_grader` to test your assignment.

There are two modes available for testing your assignment with the
`auto_grader`.

## First mode

Testing with a `pa1` folder

1. Lets say you have a `pa1` folder with the directory structure as
   described in the assignment.

2. Copy the folder to the directory of the autograder

3. Run the autograder with the following command

   ```sh
   $ python auto_grader.py
   ```

   It will run your programs and print your scores.

## Second Mode

This mode is to test your final submission (i.e, `pa1.tar`)

1. Copy `pa1.tar` to the auto_grader directory

2. Run the auto_grader with `pa1.tar` as the argument.

   The command line is

   ```sh
   $ python auto_grader.py pa1.tar
   ```

## Scoring

The autograder will print out information about the compilation and
the testing process. At the end, if your assignment is completely
correct, the score will something similar to what is given below.

```
You scored
5.0 in second
5.0 in fourth
5.0 in third
5.0 in sixth
10.0 in ninth
2.5 in seventh
7.5 in eighth
5.0 in fifth
5.0 in first
Yoor TOTAL SCORE = 50.0 /50
Your assignment will be graded for another 50 points with test cases not given to you
```

## FAQs

1. Your program should return `0` from `main` and every place you exit.
2. Your `pa1` folder and `pa1.tar` should have the correct directory structure.
3. You should not have extra print statements and new lines in your output.
4. You should not touch the `testcases` directory. If you create any temporary file in the `testcases` directory, the autograder will not work.
