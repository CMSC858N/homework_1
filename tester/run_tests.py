import unittest
from gradescope_utils.autograder_utils.json_test_runner import JSONTestRunner
from test_generator import find_data_directories, build_test_class, TestMetaclass
import json

OKGREEN = '\033[92m'
WARNING = '\033[93m'
FAIL = '\033[91m'
ENDC = '\033[0m'
BOLD = '\033[1m'

if __name__ == '__main__':
    suite = unittest.TestSuite()

    for name in find_data_directories():
        klass = build_test_class(name)
        suite.addTest(klass(TestMetaclass.test_name(name)))

    with open('results/results.json', 'w') as f:
        JSONTestRunner(visibility='visible', stream=f).run(suite)

    with open('results/results.json', 'r') as f:
        json_object = json.load(f)
        tests_sorted = sorted(json_object["tests"],key=lambda x: x["name"])
        for test in tests_sorted:
            name, status = test["name"], test["status"]
            if status == "passed":
                print(BOLD + name + ":" + ENDC, OKGREEN + status + ENDC)
            else:
                print(BOLD + name + ":" + ENDC, FAIL + status + ENDC)
                print("Output:",test["output"])
