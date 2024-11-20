"""
IFJ Project
@brief Test file for the parser

@author Martin Valapka - xvalapm00
"""

import unittest
import subprocess
import os
import signal

GREEN = '\033[92m'
RED = '\033[91m'
BLUE = '\033[94m'
RESET = '\033[0m'

class SortedTestRunner(unittest.TextTestRunner):
    def __init__(self, **kwargs):
        kwargs['resultclass'] = SortedColoredTestResult
        kwargs['stream'] = open(os.devnull, 'w')
        super().__init__(**kwargs)
    
    def run(self, test):
        result = super().run(test)
        result.printResults()
        return result

class SortedColoredTestResult(unittest.TextTestResult):
    def __init__(self, stream, descriptions, verbosity):
        super().__init__(stream, descriptions, verbosity)
        self.successes = []
        self.all_failures = []
        self.all_errors = []
        self.all_skips = []
    
    def startTest(self, test):
        pass

    def stopTest(self, test):
        pass
    
    def addSuccess(self, test):
        self.testsRun += 1
        self.successes.append(test)
    
    def addFailure(self, test, err):
        self.testsRun += 1
        self.failures.append((test, self._exc_info_to_string(err, test)))
        self.all_failures.append((test, err))
    
    def addError(self, test, err):
        self.testsRun += 1
        self.errors.append((test, self._exc_info_to_string(err, test)))
        self.all_errors.append((test, err))
    
    def addSkip(self, test, reason):
        super().addSkip(test, reason)
        self.all_skips.append((test, reason))

    def printResults(self):
        print("\n========================  Test Results ========================\n")
        
        # Print successes first
        if self.successes:
            print("Successful Tests:")
            for test in self.successes:
                print(f"{GREEN}✓ {test.shortDescription() or test.id()}{RESET}")
        
        # Print failures
        if self.all_failures:
            print("\nFailed Tests:")
            for test, err in self.all_failures:
                print(f"{RED}✗ {test.shortDescription() or test.id()} (FAIL){RESET}")
                print(self._exc_info_to_string(err, test))
        
        # Print errors
        if self.all_errors:
            print("\nErrors:")
            for test, err in self.all_errors:
                print(f"{RED}✗ {test.shortDescription() or test.id()} (ERROR){RESET}")
                print(self._exc_info_to_string(err, test))
        
        # Print skips
        if self.all_skips:
            print("\nSkipped Tests:")
            for test, reason in self.all_skips:
                print(f"{BLUE}- {test.shortDescription() or test.id()} (SKIPPED: {reason}){RESET}")
        
        # Print summary
        print(f"\nTest Summary:")
        print(f"Ran {self.testsRun} tests")
        print(f"{GREEN}Successes: {len(self.successes)}{RESET}")
        print(f"{RED}Failures: {len(self.all_failures)}{RESET}")
        print(f"{RED}Errors: {len(self.all_errors)}{RESET}")
        print(f"{BLUE}Skipped: {len(self.all_skips)}{RESET}")

class SortedTestRunner(unittest.TextTestRunner):
    def __init__(self, **kwargs):
        kwargs['resultclass'] = SortedColoredTestResult
        super().__init__(**kwargs)
    
    def run(self, test):
        result = super().run(test)
        result.printResults()
        return result

class ParserTest(unittest.TestCase):
    def get_signal_description(self, returncode):
        """Convert return code to signal description"""
        if returncode < 0:
            if returncode == -11:
                return "SIGSEGV (Segmentation Fault)"
            elif returncode == -6:
                return "SIGABRT (Abort)"
            elif returncode == -8:
                return "SIGFPE (Floating Point Exception)"
            else:
                return f"Signal {-returncode}"
        return str(returncode)

    def run_parser(self, input_content):
        with open('test_input.txt', 'w') as f:
            f.write(input_content)
        result = subprocess.run(['./program'], stdin=open('test_input.txt', 'r'),
                                capture_output=True, text=True)
        return result.returncode, result.stdout, result.stderr

    def assertParserSuccess(self, returncode, stdout="", stderr="", msg=None):
        """Assert that parser completed successfully"""
        if returncode != 0:
            signal_desc = self.get_signal_description(returncode)
            self.fail(f"Parser failed with return code {signal_desc}\n")

    def assertParserFailure(self, returncode, expected_code=None, stdout="", stderr="", msg=None):
        """Assert that parser failed with expected return code"""
        if returncode == 0:
            self.fail(f"Parser unexpectedly succeeded\nstdout: {stdout}\nstderr: {stderr}")
        if expected_code is not None and returncode != expected_code:
            signal_desc = self.get_signal_description(returncode)
            expected_desc = self.get_signal_description(expected_code)
            self.fail(f"Parser failed with return code {signal_desc}, expected {expected_desc}\nstdout: {stdout}\nstderr: {stderr}")

    def test_valid_prolog_and_main(self):
        """Test parser with valid prolog and main function"""
        code = '''
            const ifj = @import("ifj24.zig");
            pub fn main() void {
                // Main function code
            }
            '''
        True
        returncode, stdout, stderr = self.run_parser(code)
        self.assertParserSuccess(returncode, stdout, stderr)

    def test_missing_prolog(self):
        """Test parser detects missing prolog"""
        code = '''
                pub fn main() void {
                    // Main function code
                }
                '''
        False
        returncode, stdout, stderr = self.run_parser(code)
        self.assertParserFailure(returncode, stdout=stdout, stderr=stderr)

    def test_invalid_built_in_function_usage(self):
        """Test parser detects incorrect usage of built-in functions"""
        code = '''
            const ifj = @import("ifj24.zig");
            pub fn main() void {
                ifj.readstr();
            }
            '''
        False
        returncode, stdout, stderr = self.run_parser(code)
        self.assertParserFailure(returncode, stdout=stdout, stderr=stderr)

    def test_valid_variable_definition(self):
        """Test parser with valid variable definitions"""
        code = '''
                const ifj = @import("ifj24.zig");
                pub fn main() void {
                    const x = 42;
                    var y = "hello";
                }
                '''
        True
        returncode, stdout, stderr = self.run_parser(code)
        self.assertParserSuccess(returncode, stdout, stderr)

    def test_invalid_variable_assignment(self):
        """Test parser detects invalid variable assignment"""
        code = '''
            const ifj = @import("ifj24.zig");
            pub fn main() void {
                x = 42;  // 'x' not declared
            }
            '''
        False
        returncode, stdout, stderr = self.run_parser(code)
        self.assertParserFailure(returncode, stdout=stdout, stderr=stderr)

    def test_valid_function_definition(self):
        """Test parser with valid function definition"""
        code = '''
            const ifj = @import("ifj24.zig");
            pub fn add(a: i32, b: i32) i32 {
                return a + b;
            }
            pub fn main() void {
                var result = add(5, 3);
            }
            '''
        True
        returncode, stdout, stderr = self.run_parser(code)
        self.assertParserSuccess(returncode, stdout, stderr)

    def test_invalid_function_call(self):
        """Test parser detects invalid function call"""
        code = '''
            const ifj = @import("ifj24.zig");
            pub fn main() void {
                var result = add(5);
            }
            '''
        False
        returncode, stdout, stderr = self.run_parser(code)
        self.assertParserFailure(returncode, stdout=stdout, stderr=stderr)

    def test_if_statement(self):
        """Test parser with valid if statement"""
        code = '''
            const ifj = @import("ifj24.zig");
            pub fn main() void {
                var x = 10;
                if (x > 5) {
                    x = x - 1;
                }
            }
            '''
        True
        returncode, stdout, stderr = self.run_parser(code)
        self.assertParserSuccess(returncode, stdout, stderr)

    def test_while_loop(self):
        """Test parser with valid while loop"""
        code = '''
            const ifj = @import("ifj24.zig");
            pub fn main() void {
                var i = 0;
                while (i < 10) {
                    i = i + 1;
                }
            }
            '''
        True
        returncode, stdout, stderr = self.run_parser(code)
        self.assertParserSuccess(returncode, stdout, stderr)

    def test_missing_semicolon(self):
        """Test parser detects missing semicolon"""
        code = '''
            const ifj = @import("ifj24.zig");
            pub fn main() void {
                var x = 10
            }
            '''
        False
        returncode, stdout, stderr = self.run_parser(code)
        self.assertParserFailure(returncode, stdout=stdout, stderr=stderr)

    def test_invalid_syntax_in_prolog(self):
        """Test parser detects invalid syntax in prolog"""
        code = '''
            const ifj = @import("ifj24.zig"
            pub fn main() void {
                var x = 10;
            }
            '''
        False
        returncode, stdout, stderr = self.run_parser(code)
        self.assertParserFailure(returncode, stdout=stdout, stderr=stderr)

    def test_invalid_number_of_args_in_builtin_funcs(self):
        """Test parser detects invalid number of arguments in built-in functions"""
        test_cases = [
            (
                '''
                const ifj = @import("ifj24.zig");
                pub fn main() void {
                    const x = ifj.readint(42);
                }
                ''',
                "Extra argument to ifj.readint()",
                False
            ),
            (
                '''
                const ifj = @import("ifj24.zig");
                pub fn main() void {
                    const y = ifj.string("AHOJ");
                    const z = ifj.string("CAu");
                    const x = ifj.concat(y,);
                }
                ''',
                "Only one argument in ifj.concat()",
                False
            ),
            (
                '''
                const ifj = @import("ifj24.zig");
                pub fn main() void {
                    ifj.write("Hello", "World", "Extra");
                }
                ''',
                "Too many arguments in ifj.write()",
                False
            ),
            (
                '''
                const ifj = @import("ifj24.zig");
                pub fn main() void {
                    const a = ifj.readi32();
                }
                ''',
                "Correct number of arguments in ifj.readi32()",
                True
            )
        ]

        for code, description, should_succeed in test_cases:
            with self.subTest(description=description):
                returncode, stdout, stderr = self.run_parser(code)
                if should_succeed:
                    self.assertParserSuccess(returncode, stdout, stderr)
                else:
                    self.assertParserFailure(returncode, stdout=stdout, stderr=stderr)

    def test_missing_return_statement_in_void(self):
        """Test parser detects missing return statement in void function"""
        code = '''
            const ifj = @import("ifj24.zig");
            pub fn main() void {
                var result = 1;
            }
            '''
        True
        returncode, stdout, stderr = self.run_parser(code)
        self.assertParserSuccess(returncode, stdout=stdout, stderr=stderr)

    def test_unreachable_code(self):
        """Test parser detects unreachable code"""
        code = '''
            const ifj = @import("ifj24.zig");
            pub fn main() void {
                return;
                var x = 10;
            }
            '''
        False
        returncode, stdout, stderr = self.run_parser(code)
        self.assertParserFailure(returncode, stdout=stdout, stderr=stderr)

    def test_missing_return(self):
        """Test parser detects missing return statement"""
        code = '''
            const ifj = @import("ifj24.zig");
            pub fn add(a: i32, b: i32) i32 {
                a + b;
            }
            pub fn main() void {
                var result = add(5, 3);
            }
            '''
        False
        returncode, stdout, stderr = self.run_parser(code)
        self.assertParserFailure(returncode, stdout=stdout, stderr=stderr)

    def test_invalid_return_in_void(self):
        """Test parser detects invalid return statement in void function"""
        code = '''
            const ifj = @import("ifj24.zig");
            pub fn main() void {
                return 42;
            }
            '''
        False
        returncode, stdout, stderr = self.run_parser(code)
        self.assertParserFailure(returncode, stdout=stdout, stderr=stderr)

    def test_voluntary_return_in_void(self):
        """Test parser detects voluntary return in void function"""
        code = '''
            const ifj = @import("ifj24.zig");
            pub fn main() void {
                return;
            }
            '''
        True
        returncode, stdout, stderr = self.run_parser(code)
        self.assertParserSuccess(returncode, stdout, stderr)

    def missing_main(self):
        """Test parser detects missing main function"""
        code = '''
            const ifj = @import("ifj24.zig");
            pub fn add(a: i32, b: i32) i32 {
                return a + b;
            }
            '''
        False
        returncode, stdout, stderr = self.run_parser(code)
        self.assertParserFailure(returncode, stdout=stdout, stderr=stderr)

    def test_invalid_variable_definition(self):
        """Test parser detects invalid variable assignment"""
        code = '''
            const ifj = @import("ifj24.zig");
            pub fn main() void {
                const x = 42;
                a = 10;
            }
            '''
        False
        returncode, stdout, stderr = self.run_parser(code)
        self.assertParserFailure(returncode, stdout=stdout, stderr=stderr)

def missing_semicolons(self):
    """Test parser detects missing semicolons"""
    test_cases = [
        (
            '''
            const ifj = @import("ifj24.zig");
            pub fn main() void {
                const x = 42
            }
            ''',
            "Missing semicolon after variable declaration",
            False
        ),
        (
            '''
            const ifj = @import("ifj24.zig");
            pub fn main() void {
                y = ifj.string("AHOJ")
            }
            ''',
            "Missing semicolon after variable assignment with builtin function",
            False
        ),
        (
            '''
            const ifj = @import("ifj24.zig");
            pub fn main() void {
                x = x + 1
                return;
            }
            ''',
            "Missing semicolon after an expression in variable assignment",
            False
        ),
        (
            '''
            const ifj = @import("ifj24.zig");
            pub fn helper() void {
                return;
            }

            pub fn main() void {
                helper()
            }
            ''',
            "Missing semicolon after function call",
            False
        ),
        (
            '''
            const ifj = @import("ifj24.zig");
            pub fn main() void {
                return
            }
            ''',
            "Missing semicolon after return statement",
            False
        ),
        (
            '''
            const ifj = @import("ifj24.zig")
            pub fn main() void {
                return 0;
            }
            ''',
            "Missing semicolon after import statement",
            False
        ),
        (
            '''
            const ifj = @import("ifj24.zig");
            pub fn main() void {
                if (1==1) {
                    var z = 5
                }
            }
            ''',
            "Missing semicolon after variable declaration inside if statement",
            False
        ),
        (
            '''
            const ifj = @import("ifj24.zig");
            pub fn helper() void {
                return;
            }
            pub fn main() void {
                helper()
            }
            ''',
            "Missing semicolon in function call",
            False
        )
    ]
    for code, description, should_pass in test_cases:
        with self.subTest(description=description):
            returncode, stdout, stderr = self.run_parser(code)
            if should_pass:
                self.assertParserSuccess(returncode, stdout=stdout, stderr=stderr)
            else:
                self.assertParserFailure(returncode, stdout=stdout, stderr=stderr)

if __name__ == '__main__':
    unittest.main(testRunner=SortedTestRunner(verbosity=2))