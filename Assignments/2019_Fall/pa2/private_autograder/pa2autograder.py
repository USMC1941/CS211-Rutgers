#! /usr/bin/python

import os
import sys
import glob
import time
import subprocess
import signal
import popen2
import time

dict_train = {"testA.txt": "trainA.txt", "testB.txt": "trainB.txt", "testC.txt": "trainC.txt", "testD.txt": "trainD.txt",
              "testE.txt": "trainE.txt", "testF.txt": "trainF.txt", "testG.txt": "trainG.txt", "testH.txt": "trainH.txt", "testI.txt": "trainI.txt"}

first_weight = {"testA.txt": 5.0, "testB.txt": 10.0, "testC.txt": 5.0, "testD.txt": 5.0,
                "testE.txt": 5.0, "testF.txt": 5.0, "testG.txt": 5.0, "testH.txt": 5.0, "testI.txt": 5.0}


list_tests = ["testA.txt", "testB.txt", "testC.txt", "testD.txt",
              "testE.txt", "testF.txt", "testG.txt", "testH.txt", "testI.txt"]

dict_results = {"testA.txt": "resultA.txt", "testB.txt": "resultB.txt", "testC.txt": "resultC.txt", "testD.txt": "resultD.txt",
                "testE.txt": "resultE.txt", "testF.txt": "resultF.txt", "testG.txt": "resultG.txt", "testH.txt": "resultH.txt", "testI.txt": "resultI.txt"}

test_cases_directory = ""


class ExperimentError(Exception):
    def __init__(self, command, output):
        self.command = command
        limit = 10000
        if(len(output) > limit):
            self.output = output[:limit/2] + \
                "\n\n...TRUNCATED...\n\n" + output[-limit/2:]
        else:
            self.output = output

    def __str__(self):
        return "ExperimentError:" + `self.command`


def run_command(command_string, input_string="", max_lines=0, timeout=120, verbose=False, echo=False, throw_exception=True, return_valgrind_output=False, user_program=False):

    if echo:
        print "executing:", command_string
    obj = popen2.Popen4(command_string)
    output = ""
    valgrind_output = ""
#    print "1"
    obj.tochild.write(input_string)
    obj.tochild.close()
    valgrind_prefix = "==%d==" % obj.pid
    maxSleep = timeout
#    print "2"
    if user_program:  # program may have an infinite loop
        while maxSleep > 0:
            time.sleep(0.25)
            maxSleep -= 1
            if obj.poll() != -1:
                break
        if maxSleep == 0 and obj.poll() == -1:
            os.kill(obj.pid, signal.SIGKILL)
            print command_string, " taking longer than expected. Killed."
            return ""
#    print "3"
    line = obj.fromchild.readline()
    while (line):
        if verbose == 1:
            print line,
        if line.startswith(valgrind_prefix):
            valgrind_output += line
        output += line
        line = obj.fromchild.readline()
    exit_status = obj.wait()
 #   print "4"
    if(max_lines != 0):
        lines = output.split("\n")
        output = string.join(lines[-max_lines:], "\n")

    if throw_exception and exit_status != 0:
        raise ExperimentError(command_string, output)

    if return_valgrind_output:
        return valgrind_output
    else:
        return output


def compare_string_file(ref_file, test_string, show_difference=False):
    test_list = test_string.split("\n")
    fd = open(ref_file)
    i = 0
    flag = True

    for line in fd:
        if i < len(test_list):
            if line.strip() != test_list[i].strip():
                val1 = int(test_list[i].strip())
                val2 = int(line.strip())
                if ((val1 + 1 == val2) or (val1 - 1 == val2)):
                    i += 1
                    continue
                print "Observed:", test_list[i].strip()
                print "Expected:", line.strip()
                flag = False
        elif len(line.strip()) > 0:
            print "Output missing: ", line
            flag = False
        i += 1

    fd.close()
    while(i < len(test_list)):
        if len(test_list[i].strip()) == 0:
            i += 1
            continue
        print "Extra output: ", test_list[i]
        i += 1
        flag = False

    return flag


def compare_string(ref, test):
    ref = ref.strip()
    test = test.strip()

    if(ref == test):
        return True
    if(ref == test.lower()):
        print "%s and %s are in different case. Please print your output in correct case." % (
            ref, test)
    return False


def make_executable(dirname):
    if os.path.isfile('Makefile') or os.path.isfile('makefile'):
        run_command("make clean", verbose=False)
        run_command("make", verbose=True)
    else:
        print "No Makefile found in", dirname
        print "Please submit a Makefile to receive full grade."
        run_command("gcc -o %s *.c *.h" % (dirname), verbose=False)


def third_grade(dirname):
    #    print "Grading", dirname
    score = 0
    prevdir = os.getcwd()
    os.chdir(dirname)
    try:
        make_executable(dirname)
    except:
        print "An exception occured trying to execute %s" % (dirname)
        os.chdir(prevdir)
        print "Score is ", score
        print ""
        print ""
        return score

    if not os.path.isfile("third"):
        print "Executable %s missing. Please check the compilation output." % (
            "third")
        os.chdir(prevdir)
        print "Score is ", score
        print ""
        print ""
        return score

    test_dir = test_cases_directory + 'third/'

    for test in list_tests:

        train_file = test_dir + dict_train[test]
        result_file = test_dir + dict_results[test]
        print "Grading test file %s." % (test)
        if not result_file:
            print "Found a test file %s. but no result file." % (test)
            continue

        if not train_file:
            print "Found a test file %s. but no train file." % (test)
            continue

        ret = ""
        try:
            command_string = "./third " + train_file + " " + test_dir + test
            ret = run_command(command_string, verbose=False)
        except:
            print "An exception occurred while executing ./learn %s %s" % (
                train_file, test_dir+test)
            print ret
            print "Score is ", score
            continue
        # time.sleep(10)

        if compare_string_file(result_file, ret, show_difference=True):
            score = score + first_weight[test]

        else:
            print "The output is not correct for input file %s." % (test)
    os.chdir(prevdir)
    print "Score is ", score
    return score


subdirectories = ['first', 'second', 'third']
final_release_scores = {'first': 0.0, 'second': 0.0, 'third': 0.0}


def grade_sudoku_problem(test_file, ref_file, test_string, show_difference=False):
    # ref_file is the file that has the solution
    # test_string is the output of the program that we are testing.
    test_string = test_string.strip()

    answer = ""
    with open(ref_file, 'r') as content_file:
        answer = content_file.read()
    answer = answer.strip()

    original = ""
    with open(test_file, 'r') as content_file:
        original = content_file.read()
    original = original.strip()

    # Detect if it's correct.
    # Answer says no-solution and test says no-solution
    if compare_string("no-solution", answer) and compare_string("no-solution", test_string):
        return True
    # Answer says no-solution but test does not say no-solution
    elif compare_string("no-solution", answer) and not compare_string("no-solution", test_string):
        print "Observed:"
        print test_string
        print "Expected:"
        print answer
        return False
    # Answer says not no-solution but test says no-solution
    elif not compare_string("no-solution", answer) and compare_string("no-solution", test_string):
        print "Observed:"
        print test_string
        print "Expected:"
        print answer
        return False
    # Answer says not no-solution and test says not no-solution
    else:
        answer_board = [l.strip().split() for l in answer.split("\n")]
        test_board = [l.strip().split() for l in test_string.split("\n")]
        original_board = [l.strip().split() for l in original.split("\n")]

        # 1. Make sure test_board is a 16x16 grid
        if len(test_board) != 16:
            print "Expected 16 rows. Observed %d rows." % (len(test_board))
            return False
        for i in range(0, 16):
            if len(test_board[i]) != 16:
                print "Expected 16 columns. Observed %d columns in row %d" % (
                    len(test_board[i]), i+1)
                return False

        # 2. check whether the test_board is based on the original board
        answer_is_correct = True
        for i in range(0, 16):
            for j in range(0, 16):
                if original_board[i][j] != '-':
                    if original_board[i][j] != test_board[i][j]:
                        answer_is_correct = False

        if answer_is_correct:
            for i in range(0, 16):
                for j in range(0, 16):
                    # check horizontally
                    for c in range(0, 16):
                        if c != j and test_board[i][j] == test_board[i][c]:
                            print "Duplicated in row %d, col %d and col %d " % (
                                i, c, j)
                            return False

                    # check vertivally
                    for r in range(0, 16):
                        if r != i and test_board[i][j] == test_board[r][j]:
                            print "Duplicated in col %d, row %d and row %d " % (
                                j, i, r)

                    # check small box
                    sr = i/4 * 4
                    sc = j/4 * 4
                    for ir in range(sr, sr+4):
                        for ic in range(sc, sc+4):
                            if ir != i and ic != j and test_board[i][j] == test_board[ir][ic]:
                                return False

            return True

        else:
            print "The answer is not based on the original testcase"
            return False


def second_grade(dirname):
    #    print "Grading second"
    score = 0
    prevdir = os.getcwd()
    os.chdir(dirname)
    try:
        make_executable(dirname)
    except:
        print "An exception occured trying to execute %s" % (dirname)
        os.chdir(prevdir)
        print "Score is ", score
        print ""
        print ""
        return score
    if not os.path.isfile("second"):
        print "Executable %s missing. Please check the compilation output." % (
            "second")
        os.chdir(prevdir)
        print "Score is ", score
        print ""
        print ""
        return score

    test_dir = test_cases_directory + 'second/'

    for testfile in sorted(os.listdir(test_dir)):
        if os.path.isdir(testfile) or not testfile.startswith("test"):
            continue
        resultfile = test_dir + "result"+testfile[4:len(testfile)]
        if not os.path.isfile(resultfile):
            print "Found a test file %s. but no associated result file." % (
                testfile)
            continue
        try:
            ret = run_command(
                "./%s %s" % (dirname, test_dir + testfile), user_program=True, verbose=False)
        except:
            print "An exception occurred while executing %s %s" % (
                dirname, testfile)
            os.chdir(prevdir)
            final_release_scores['second'] = score
            return
        testfile = test_dir + testfile
        if grade_sudoku_problem(testfile, resultfile, ret, show_difference=True):
            print "The output is correct for input file %s." % (testfile)
            score = score + 5

        else:
            print "The output is not correct for input file %s." % (testfile)

    print "Score is ", score
    final_release_scores['second'] = score

    os.chdir(prevdir)
    return


def first_grade(dirname):
    #    print "Grading extra credit"
    score = 0
    prevdir = os.getcwd()
    os.chdir(dirname)
    try:
        make_executable(dirname)
    except:
        print "An exception occured trying to execute %s" % (dirname)
        os.chdir(prevdir)
        print "Score is ", score
        print ""
        print ""
        return score
    if not os.path.isfile("first"):
        print "Executable %s missing. Please check the compilation output." % (
            "third")
        os.chdir(prevdir)
        print "Score is ", score
        print ""
        print ""
        return score

    test_dir = test_cases_directory + 'first/'

    for testfile in sorted(os.listdir(test_dir)):
        if os.path.isdir(testfile) or not testfile.startswith("test"):
            continue
        resultfile = test_dir + "result"+testfile[4:len(testfile)]
        if not os.path.isfile(resultfile):
            print "Found a test file %s. but no associated result file." % (
                testfile)
            continue
        try:
            ret = run_command(
                "./%s %s" % (dirname, test_dir + testfile), user_program=True, verbose=False)
        except:
            print "An exception occurred while executing %s %s" % (
                dirname, testfile)
            os.chdir(prevdir)
            final_release_scores['first'] = score
            return

        if compare_string_file(resultfile, ret, show_difference=True):
            print "The output is correct for input file %s." % (testfile)
            score = score + 10
        else:
            print "The output is not correct for input file %s." % (testfile)
#        print ""
    print "Score is ", score
    final_release_scores['first'] = score
    os.chdir(prevdir)
    return score


def global_grade(dirname):
    #print "dirname is ", dirname
    target = len(subdirectories)
    for subdir in subdirectories:
        #print "looking now for -", os.path.join(subdir), "-"
        if not os.path.isdir(os.path.join(subdir)):
            print "will skip ", subdir
            continue
        print subdir, " found!"

        total_score = 0.0
        if subdir == 'third':
            print "Grading third"
            first_score = third_grade(subdir)
            final_release_scores['third'] = first_score
            print "test score:", final_release_scores['third']
            total_score = total_score + first_score

        if subdir == 'second':
            print "Grading Extra Credit"
            second_grade(subdir)
        if subdir == 'first':
            print "Grading first"
            first_grade(subdir)

    final_score = total_score
    return total_score


if __name__ == '__main__':
    basepath = "pa2"
    tarmode = False  # by default check the directory
    total_score = 0

    test_cases_directory = os.getcwd() + "/testcases/"
    if len(sys.argv) > 1:
        if sys.argv[1].strip().endswith("tar"):
            tarmode = True

    if tarmode == False:
        if not os.path.isdir(basepath):
            print "pa2 is not present in this directory."
            sys.exit(1)
        else:
            print "Grading the content of pa2"
            os.chdir(basepath)
            global_grade(basepath)

    else:
        prevdir = os.getcwd()

        if os.path.exists("obj_temp"):
            #            	print "Deleting the directory obj_temp."
            run_command("rm -rf obj_temp", verbose=False)
        run_command("mkdir obj_temp", verbose=False)
        os.chdir("obj_temp")
        run_command("tar -xvf ../pa2.tar")

        if os.path.isdir("pa2"):
            os.chdir("pa2")
            global_grade("pa2")
        else:
            print "There is not directory named pa2 in pa2.tar."
            print "Please check your tar file."
        os.chdir(prevdir)

    total_score = final_release_scores['first'] + \
        final_release_scores['second'] + final_release_scores['third']
    print "Your TOTAL SCORE = ", total_score, "/125"
    print total_score
