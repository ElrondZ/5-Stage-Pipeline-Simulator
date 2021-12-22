import os
import shutil
import subprocess
import time
import filecmp
from os.path import exists
from sys import platform

def assembler(curDir):
    # Covert assembly in Instructions.txt to imem.txt
    print("Assembler current directory: ", curDir)

def wc_count(file_name):
    out = subprocess.getoutput("wc -l %s" % file_name)
    return int(out.split()[0])

def delete_extra(*args):
    currpath = os.getcwd()
    for item in args:
        if exists(os.path.join(currpath, item)):
            os.remove(os.path.join(currpath, item))


def runtest(num, filename):
    currpath = os.getcwd() # Current Directory, which both testcases and cpp files are 
    testDir = os.path.join(currpath, str(num))
    print("Test case: ", num, " folder path: ", testDir)

    # remove old files
    delete_extra("dmemresult.txt", "stateresult.txt", "RFresult.txt", "imem.txt", "dmem.txt")

    # Copy dmem.txt, imem.txt to local folder
    shutil.copy(os.path.join(testDir, "dmem.txt"), currpath)
    shutil.copy(os.path.join(testDir, "imem.txt"), currpath)

    # Execute the program
    try:
        run_process = subprocess.Popen(filename, stderr=subprocess.STDOUT)
        time.sleep(1)
        if run_process.poll() != 0:
            try:
                run_process.terminate()
            except:
                return False
    except:
        delete_extra("dmemresult.txt", "stateresult.txt", "RFresult.txt", "imem.txt", "dmem.txt")
        return False
    
    # Compare result

    testRFresult = filecmp.cmp(os.path.join(currpath, "RFresult.txt"), os.path.join(testDir, "RFresult_grading.txt"))
    testDMEMresult = filecmp.cmp(os.path.join(currpath, "dmemresult.txt"), os.path.join(testDir, "dmemresult_grading.txt"))
    testSTATEresult = wc_count(os.path.join(currpath,"stateresult.txt")) == wc_count(os.path.join(testDir, "stateresult_grading.txt"))
    delete_extra("dmemresult.txt", "stateresult.txt", "RFresult.txt", "imem.txt", "dmem.txt")

    print("RF compare result: ", testRFresult, ", DMEM compare result: ", testDMEMresult, ", state compare result: ", testSTATEresult)
    return testRFresult & testDMEMresult & testSTATEresult

def runall(filename):
    testresults = []
    for i in range(1, 20):
        testresult = runtest(i, filename)
        print("Test result: ", testresult)
        testresults.append(testresult)
    print("==========================\nAll results: ")
    for i in range(1, 20):
        if(testresults[i-1]):
            print("Testcase ", str(i), " : SUCCESS")
        else:
            print("Testcase ", str(i), " : FAILED")
    print("==========================")
    delete_extra("pipeline.exe", "./pipeline")

if __name__ == '__main__':
    print("ECE 6913 Lab2 Test")
    print("Platform: ", platform)
    if platform == 'win32':
        filename = 'pipeline.exe'
    else:
        filename = './pipeline'
    path = os.getcwd()
    print("Current path: ", path)
    upperDir = os.path.abspath(os.path.dirname(os.getcwd()))
    print("Upper directory: ", upperDir)

    make_process = subprocess.Popen("make", stderr=subprocess.STDOUT)
    if make_process.wait() != 0:
        print("Make fail")
        try:
            make_process.terminate()
        except:
            pass
    runall(filename)
    # runtest(8, filename)
