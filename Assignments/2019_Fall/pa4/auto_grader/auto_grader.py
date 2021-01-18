#!/usr/bin/env

import os
import sys
import glob
import time
import subprocess
import signal
import popen2
import time

subdirectories = ['first']
# if a program has single liner input and output, we put all test cases in single file. Otherwise, we have a file for test and associated file with results
formats = {'first': 'file'}
weight = {'first': 1}
total = {'first': 100}
runtime = {'first': 2050}
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


def run_command(command_string, input_string="", max_lines=0, timeout=20, verbose=False, echo=False, throw_exception=True, return_valgrind_output=False, user_program=False):

    if echo:
        print "executing:", command_string
    obj = popen2.Popen4(command_string)
    output = ""
    valgrind_output = ""

    obj.tochild.write(input_string)
    obj.tochild.close()
    valgrind_prefix = "==%d==" % obj.pid
    maxSleep = timeout

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
    line = obj.fromchild.readline()
    while (line):
        if verbose == 1:
            print line,
        if line.startswith(valgrind_prefix):
            valgrind_output += line
        output += line
        line = obj.fromchild.readline()
    exit_status = obj.wait()

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
#        print "here"
        run_command("make", verbose=True)
    else:
        print "No Makefile found in", dirname
        print "Please submit a Makefile to receive full grade."
        run_command("gcc -o %s *.c *.h" % (dirname), verbose=False)


def file_grade(dirname):
    print "Grading file", dirname
    prevdir = os.getcwd()
    os.chdir(dirname)
    myscore = 0
    try:
        make_executable(dirname)
    except:
        print "An exception occured trying to execute %s" % (dirname)

        os.chdir(prevdir)
        print "my score is ", myscore
        print ""
        print ""
        return

    if not os.path.isfile(dirname):
        print "Executable %s missing. Please check the compilation output." % (
            dirname)
        os.chdir(prevdir)
        print "my score is ", myscore
        print ""
        print ""
        return

    output = "output"
    input = "input"
    alaki = ".DS_Store"
    input_dir = test_cases_directory + dirname + "/" + input
    output_dir = test_cases_directory + dirname + "/" + output

    for testfile in sorted(os.listdir(input_dir)):

        if os.path.isdir(testfile) or not testfile.startswith("trace"):
            continue

        for resultfile in sorted(os.listdir(output_dir)):

            if resultfile.startswith("one") and (not testfile[len(resultfile)-4] == "1"):
                continue
            elif resultfile.startswith("two") and (not testfile[len(resultfile)-4] == "2"):
                continue
            elif resultfile.startswith("thr") and (not testfile[len(resultfile)-4] == "3"):
                continue
            elif resultfile.startswith("fou") and (not testfile[len(resultfile)-4] == "4"):
                continue
            else:
                print ""

            if resultfile[len(resultfile)-5] == "2":
                try:
                    run_command("./%s 512 16 fifo direct 1 %s > %s" % (dirname, input_dir + "/" +
                                                                       testfile, resultfile), timeout=runtime[dirname], user_program=True, echo=True)
                except:
                    print "An exception occurred while executing %s %s" % (
                        dirname, testfile)
                    os.chdir(prevdir)
                    print "my score is ", myscore
                    # return
            elif resultfile[len(resultfile)-5] == "1":
                try:
                    run_command("./%s 512 16 fifo direct 2 %s > %s" % (dirname, input_dir + "/" +
                                                                       testfile, resultfile), timeout=runtime[dirname], user_program=True, echo=True)
                except:
                    print "An exception occurred while executing %s %s" % (
                        dirname, testfile)
                    os.chdir(prevdir)
                    print "myScore is ", myscore
                    # return
            elif resultfile[len(resultfile)-5] == "4":
                try:
                    run_command("./%s 512 16 fifo assoc:4 1 %s > %s" % (dirname, input_dir + "/" +
                                                                        testfile, resultfile), timeout=runtime[dirname], user_program=True, echo=True)
                except:
                    print "An exception occurred while executing %s %s" % (
                        dirname, testfile)
                    os.chdir(prevdir)
                    print "my score is ", myscore
                    # return
            elif resultfile[len(resultfile)-5] == "3":
                try:
                    run_command("./%s 512 8 fifo assoc:8 1 %s > %s" % (dirname, input_dir + "/" +
                                                                       testfile, resultfile), timeout=runtime[dirname], user_program=True, echo=True)
                except:
                    print "An exception occurred while executing %s %s" % (
                        dirname, testfile)
                    os.chdir(prevdir)
                    print "myScore is ", myscore
                    # return
            elif resultfile[len(resultfile)-5] == "6":
                try:
                    run_command("./%s 1024 1 fifo assoc:2 2 %s > %s" % (dirname, input_dir + "/" +
                                                                        testfile, resultfile), timeout=runtime[dirname], user_program=True, echo=True)
                except:
                    print "An exception occurred while executing %s %s" % (
                        dirname, testfile)
                    os.chdir(prevdir)
                    print "myScore is ", myscore
                    # return
            elif resultfile[len(resultfile)-5] == "5":
                try:
                    run_command("./%s 512 4 fifo assoc:2 1 %s > %s" % (dirname, input_dir + "/" +
                                                                       testfile, resultfile), timeout=runtime[dirname], user_program=True, echo=True)
                except:
                    print "An exception occurred while executing %s %s" % (
                        dirname, testfile)
                    os.chdir(prevdir)
                    print "myScore is ", myscore

            elif resultfile[len(resultfile)-5] == "8":
                try:
                    run_command("./%s 128 32 fifo assoc 2 %s > %s" % (dirname, input_dir + "/" +
                                                                      testfile, resultfile), timeout=runtime[dirname], user_program=True, echo=True)
                except:
                    print "An exception occurred while executing %s %s" % (
                        dirname, testfile)
                    os.chdir(prevdir)
                    print "myScore is ", myscore
                    # return
            elif resultfile[len(resultfile)-5] == "7":
                try:
                    run_command("./%s 512 32 fifo assoc 4 %s > %s" % (dirname, input_dir + "/" +
                                                                      testfile, resultfile), timeout=runtime[dirname], user_program=True, echo=True)
                except:
                    print "An exception occurred while executing %s %s" % (
                        dirname, testfile)
                    os.chdir(prevdir)
                    print "myScore is ", myscore
                    # return
            elif resultfile[len(resultfile)-5] == "a":
                try:
                    run_command("./%s 512 4 lru assoc:2 2 %s > %s" % (dirname, input_dir + "/" +
                                                                      testfile, resultfile), timeout=runtime[dirname], user_program=True, echo=True)
                except:
                    print "An exception occurred while executing %s %s" % (
                        dirname, testfile)
                    os.chdir(prevdir)
                    print "myScore is ", myscore
                    # return
            elif resultfile[len(resultfile)-5] == "9":
                try:
                    run_command("./%s 1024 2 lru assoc 4 %s > %s" % (dirname, input_dir + "/" +
                                                                     testfile, resultfile), timeout=runtime[dirname], user_program=True, echo=True)
                except:
                    print "An exception occurred while executing %s %s" % (
                        dirname, testfile)
                    os.chdir(prevdir)
                    print "myScore is ", myscore

            else:
                print "Wrong result file"

            command = "diff -B -w " + output_dir + "/" + \
                resultfile + " " + resultfile + " >/dev/null"
            if os.system(command) == 0:
                print resultfile + " is correct"
                myscore = myscore + 3.125

                print "score is " + str(myscore)
            else:
                print resultfile + " is not correct"

    print "Your final score is ", myscore

    print ""
    print ""

    os.chdir(prevdir)


def global_grade(dirname):

    target = len(subdirectories)
    for subdir in subdirectories:

        if not os.path.isdir(os.path.join(subdir)):

            continue
        print subdir, " found!"
        if subdir in formats and formats[subdir] == 'line':
            print "single grade format \n"
            single_grade(subdir)

        elif subdir in formats and formats[subdir] == 'file':
            file_grade(subdir)


if __name__ == '__main__':
    basepath = "pa4"
    tarmode = False  # by default check the directory

    test_cases_directory = os.getcwd() + "/testcases/"
    if len(sys.argv) > 1:
        if sys.argv[1].strip().endswith("tar"):
            tarmode = True

    if tarmode == False:
        if not os.path.isdir(basepath):
            print "pa4 is not present in this directory."
            sys.exit(1)
        else:
            prevdir = os.getcwd()
#	    print "dir is ", prevdir
            if os.path.exists("obj_temp"):
                #            	print "Deleting the directory obj_temp."
                run_command("rm -rf obj_temp", verbose=False)
            run_command("mkdir obj_temp", verbose=False)
            os.chdir("obj_temp")
            run_command("cp -r ../pa4 .")
            if os.path.isdir("pa4"):
                os.chdir("pa4")
                print "Grading the content of pa4."

            global_grade("pa4")

    else:
        prevdir = os.getcwd()
        print "dir is ", prevdir
        if not os.path.exists("pa4.tar"):
            print "Expecting pa4.tar in current directory. Current directory is %s" % (
                prevdir)
            print "Please make sure you created pa4.tar in the right directory"
            sys.exit(1)
        if os.path.exists("obj_temp"):
            print "Deleting the directory obj_temp."
            run_command("rm -rf obj_temp", verbose=False)
        run_command("mkdir obj_temp", verbose=False)
        os.chdir("obj_temp")
        run_command("tar -xvf ../pa4.tar")
        if os.path.isdir("pa4"):
            os.chdir("pa4")
            global_grade("pa4")
        else:
            print "There is not directory named pa4 in pa4.tar."
            print "Please check your tar file."
        os.chdir(prevdir)
