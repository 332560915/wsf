
/*
 * Copyright 2005-2008 WSO2, Inc. http://wso2.com
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <wsf_unit.h>

wsf_unit_bool_t quiet = WSFC_UNIT_FALSE;
wsf_unit_bool_t list_tests = WSFC_UNIT_FALSE;
wsf_unit_bool_t invert = WSFC_UNIT_FALSE;
FILE *log_file = NULL;
const wsf_unit_char_t **test_list = NULL;

static wsf_unit_bool_t
wsf_unit_test_exists(
    const wsf_unit_char_t *name);

static wsf_unit_bool_t
wsf_unit_test_required(
    const wsf_unit_char_t *name);

static void
wsf_unit_report_sub_suite(
    wsf_unit_sub_suite_t *sub_suite);

static void
wsf_unit_report_suite(
    wsf_unit_suite_t *suite);

struct wsf_unit_sub_suite
{
    wsf_unit_char_t *name;
    int total;
    int failed;
    wsf_unit_bool_t omit;
    int not_implemented;
    wsf_unit_sub_suite_t *next;
};

struct wsf_unit_suite
{
    wsf_unit_sub_suite_t *head;
    wsf_unit_sub_suite_t *tail;
};

struct wsf_unit_test_case
{
    wsf_unit_status_t status;
    wsf_unit_sub_suite_t *sub_suite;
};

WSFC_UNIT_EXTERN wsf_unit_suite_t *WSFC_UNIT_CALL
wsf_unit_suite_create(
    const wsf_unit_char_t *suite_name)
{
    wsf_unit_suite_t *suite = NULL;

    suite = (wsf_unit_suite_t *) malloc(sizeof(wsf_unit_suite_t));
    if (suite)
    {
        wsf_unit_status_t status = WSFC_UNIT_FAILURE;
        if (!suite_name)
        {
            return suite;
        }
        status = wsf_unit_suite_add_sub_suite(suite, suite_name);
        if (status)
        {
            return suite;
        }
        free(suite);
        suite = NULL;
    }
    else
    {
        wsf_unit_print_error_message("[error] %s\n", "No memmory available");
    }
    return suite;
}

WSFC_UNIT_EXTERN wsf_unit_status_t WSFC_UNIT_CALL
wsf_unit_suite_add_sub_suite(
    wsf_unit_suite_t * suite,
    const wsf_unit_char_t *suite_name)
{
    wsf_unit_sub_suite_t *sub_suite = NULL;
    const wsf_unit_char_t *tmp1 = NULL;
    const wsf_unit_char_t *tmp2 = NULL;

    if (!suite)
    {
        return WSFC_UNIT_FAILURE;
    }
    else if (suite->tail && !suite->tail->omit)
        /* Report Last sub_suite */
    {
        wsf_unit_report_sub_suite(suite->tail);
    }

    sub_suite =
        (wsf_unit_sub_suite_t *) malloc(sizeof(wsf_unit_sub_suite_t));

    if (!sub_suite)
    {
        wsf_unit_print_error_message("[error] %s\n", "No memmory available");
        return WSFC_UNIT_FAILURE;
    }
    sub_suite->name = NULL;
    sub_suite->total = 0;
    sub_suite->omit = WSFC_UNIT_FALSE;
    sub_suite->not_implemented = 0;
    sub_suite->failed = 0;
    sub_suite->next = NULL;

    if (suite_name)
    {
        tmp1 = strrchr(suite_name, '/');
        tmp2 = strrchr(suite_name, '.');
        if (!tmp1 || *tmp1)
        {
            tmp1 = suite_name;
        }
        else
        {
            tmp1++;
        }
    }
    if (tmp2)
    {
        sub_suite->name = calloc((int)(tmp2 - tmp1) + 1,
                                 sizeof(wsf_unit_char_t));
        memcpy(sub_suite->name, tmp1, (int)(tmp2 - tmp1));
    }
    else if (tmp1)
    {
        sub_suite->name = calloc((int)strlen(tmp1) + 1,
                                 sizeof(wsf_unit_char_t));
        memcpy(sub_suite->name, tmp1, (int)strlen(tmp1));
    }

    if (suite->tail)
    {
        suite->tail->next = sub_suite;
    }
    else
    {
        suite->head = sub_suite;
    }
    suite->tail = sub_suite;

    if (!wsf_unit_test_required(sub_suite->name))
    {
        sub_suite->omit = WSFC_UNIT_TRUE;
        if (list_tests)
        {
            wsf_unit_print_message("%s\n", sub_suite->name);
        }
        return WSFC_UNIT_SUCCESS;
    }

    if (!quiet)
    {
        wsf_unit_print_message("\n%-20s:  ", sub_suite->name);
    }

    return WSFC_UNIT_SUCCESS;
}

WSFC_UNIT_EXTERN void WSFC_UNIT_CALL
wsf_unit_suite_free(
    wsf_unit_suite_t * suite)
{
    if (suite)
    {
        if (suite->head)
        {
            wsf_unit_sub_suite_t *current = NULL;
            
            current = suite->head;

            while(current)
            {
                wsf_unit_sub_suite_t *temp = NULL;
                if (current->name)
                {
                    free(current->name);
                }
                temp = current->next;
                free(current);
                current = temp;
            }
        }
        else if (suite->tail)
        {
            if (suite->tail->name)
            {
                free(suite->tail->name);
            }
            free(suite->tail);
        }
        free(suite);
    }
}

WSFC_UNIT_EXTERN void WSFC_UNIT_CALL
wsf_unit_run_test(
    wsf_unit_suite_t * suite,
    WSFC_UNIT_TEST test,
    void *value)
{
    wsf_unit_test_case_t *test_case = NULL;
    wsf_unit_sub_suite_t *sub_suite = NULL;

    if (!wsf_unit_test_required(suite->tail->name))
    {
        return;
    }
    
    test_case = malloc(sizeof(wsf_unit_test_case_t));
    if (!test_case)
    {
        return;
    }
    test_case->status = WSFC_UNIT_SUCCESS;
    test_case->sub_suite = sub_suite;

    sub_suite = suite->tail;
    sub_suite->total++;

    test(test_case, value);
    if (!test_case->status)
    {
        sub_suite->failed++;
    } 
    free(test_case);
}

WSFC_UNIT_EXTERN void WSFC_UNIT_CALL
wsf_unit_print_error_message(
    const char *format,
    ...)
{
    va_list args;

    if (!quiet)
    {
        if (!log_file)
        {
            va_start(args, format);
            vfprintf(stderr, format, args);
            va_end(args);
            fflush(stderr);
        }
        else
        {
            va_start(args, format);
            vfprintf(log_file, format, args);
            va_end(args);
            fflush(log_file);
        }
    }
}

WSFC_UNIT_EXTERN void WSFC_UNIT_CALL
wsf_unit_print_message(
    const char *format,
    ...)
{
    va_list args;

    if (!log_file)
        {
            va_start(args, format);
            vfprintf(stdout, format, args);
            va_end(args);
            fflush(stdout);
        }
        else
        {
            va_start(args, format);
            vfprintf(log_file, format, args);
            va_end(args);
            fflush(log_file);
        }
}

static wsf_unit_bool_t
wsf_unit_test_exists(
    const wsf_unit_char_t *name)
{
    int i = -1;
    while(test_list[++i])
    {
        if (!strcmp(test_list[i], name))
        {
            return WSFC_UNIT_TRUE;
        }
    }
    return WSFC_UNIT_FALSE;
}

static wsf_unit_bool_t
wsf_unit_test_required(
    const wsf_unit_char_t *name)
{
    if (list_tests)
    {
        return WSFC_UNIT_FALSE;
    }
    else if (!test_list)
    {
        return WSFC_UNIT_TRUE;
    }
    else if ((wsf_unit_test_exists(name) && !invert)
        || (!(wsf_unit_test_exists(name) && invert)))
    {
        return WSFC_UNIT_TRUE;
    }
    return WSFC_UNIT_FALSE;
}

static void
wsf_unit_report_sub_suite(
    wsf_unit_sub_suite_t *sub_suite)
{
    if (sub_suite && !quiet)
    {
        if (sub_suite->failed == 0)
        {
            wsf_unit_print_message("SUCCESS\n");
        }
        else
        {
            wsf_unit_print_message("FAILED %d of %d\n",
                sub_suite->failed, sub_suite->total);
        }
    }
}

static void
wsf_unit_report_suite(
    wsf_unit_suite_t *suite)
{
    int failed = 0;
    int total = 0;
    int not_implemented = 0;
    wsf_unit_sub_suite_t *temp = NULL;

    if (!suite || list_tests)
    {
        return;
    }

    if (suite->tail && !suite->tail->omit)
    {
        wsf_unit_report_sub_suite(suite->tail);
    }

    for (temp = suite->head; temp; temp = temp->next)
    {
        failed += temp->failed;
        not_implemented += temp->not_implemented;
        total += temp->total;
    }

    if (failed == 0)
    {
        if (not_implemented == 0)
        {
            wsf_unit_print_message("\nAll Tests Passed\n");
        }
        else
        {
            wsf_unit_print_message("\nAll Implemented Tests Passed. Not Implemented: %d\n",
                   not_implemented);
        }
        return;
    }

    temp = suite->head;
    wsf_unit_print_message("\n---------------------------------------------------\n");
    wsf_unit_print_message("%-15s\t\tTotal\tFailed\tFailure %%\n", "Test Report");
    wsf_unit_print_message("---------------------------------------------------\n");
    while (temp)
    {
        float percent = ((float)temp->failed / (float)temp->total);
        wsf_unit_print_message("%-15s\t\t%5d\t%6d\t%6.2f%%\n", temp->name,
            temp->total, temp->failed, percent * 100);
        temp = temp->next;
    }
    wsf_unit_print_message("___________________________________________________\n\n");
    wsf_unit_print_message("%-15s\t\t%5d\t%6d\t%6.2f%%\n", "",
                    total, failed, ((float)failed / (float)total) * 100);
    wsf_unit_print_message("===================================================\n");
}

WSFC_UNIT_EXTERN void WSFC_UNIT_CALL
wsf_unit_assert_equals_int(
    wsf_unit_test_case_t *test_case,
    const char *message,
    const int expected,
    const int actual,
    const int line_no)
{
    if (!test_case->status)
    {
        return;
    }

    if (expected == actual)
    {
        return;
    }

    test_case->status = WSFC_UNIT_FAILURE;
    if (message)
    {
        wsf_unit_print_error_message("Line %d: %s\n%-20s:  ", line_no, message, "");
    }
    else
    {
        wsf_unit_print_error_message(
             "Line %d: expected <%d>, but saw <%d>\n%-20s:  ", line_no,
             expected, actual, "");   
    }
}

WSFC_UNIT_EXTERN void WSFC_UNIT_CALL
wsf_unit_assert_equals_float(
    wsf_unit_test_case_t *test_case,
    const char *message,
    const float expected,
    const float actual,
    const int line_no)
{
    if (!test_case->status)
    {
        return;
    }

    if (expected == actual)
    {
        return;
    }

    test_case->status = WSFC_UNIT_FAILURE;
    if (message)
    {
        wsf_unit_print_error_message("Line %d: %s\n%-20s:  ", line_no, message, "");
    }
    else
    {
        wsf_unit_print_error_message(
             "Line %d: expected <%f>, but saw <%f>\n%-20s:  ", line_no,
             expected, actual, "");   
    }
}

WSFC_UNIT_EXTERN void WSFC_UNIT_CALL
wsf_unit_assert_equals_double(
    wsf_unit_test_case_t *test_case,
    const char *message,
    const double expected,
    const double actual,
    const int line_no)
{
    if (!test_case->status)
    {
        return;
    }

    if (expected == actual)
    {
        return;
    }

    test_case->status = WSFC_UNIT_FAILURE;
    if (message)
    {
        wsf_unit_print_error_message("Line %d: %s\n%-20s:  ", line_no, message, "");
    }
    else
    {
        wsf_unit_print_error_message(
             "Line %d: expected <%f>, but saw <%f>\n%-20s:  ", line_no,
             expected, actual, "");   
    }
}

WSFC_UNIT_EXTERN void WSFC_UNIT_CALL
wsf_unit_assert_equals_char(
    wsf_unit_test_case_t *test_case,
    const char *message,
    const char expected,
    const char actual,
    const int line_no)
{
    if (!test_case->status)
    {
        return;
    }

    if (expected == actual)
    {
        return;
    }

    test_case->status = WSFC_UNIT_FAILURE;
    if (message)
    {
        wsf_unit_print_error_message("Line %d: %s\n%-20s:  ", line_no, message, "");
    }
    else
    {
        wsf_unit_print_error_message(
             "Line %d: expected <%c>, but saw <%c>\n%-20s:  ", line_no,
             expected, actual, "");   
    }
}

WSFC_UNIT_EXTERN void WSFC_UNIT_CALL
wsf_unit_assert_equals_string(
    wsf_unit_test_case_t *test_case,
    const char *message,
    const char *expected,
    const char *actual,
    const int line_no)
{
    if (!test_case->status)
    {
        return;
    }

    if (!expected && !actual)
    {
        return;
    }
    else if (expected && actual && !strcmp(expected, actual))
    {
        return;
    }

    test_case->status = WSFC_UNIT_FAILURE;
    if (message)
    {
        wsf_unit_print_error_message("Line %d: %s\n%-20s:  ", line_no, message, "");
    }
    else
    {
        wsf_unit_print_error_message(
             "Line %d: expected <%d>, but saw <%d>\n%-20s:  ", line_no,
             expected, actual, "");   
    }
}

WSFC_UNIT_EXTERN void WSFC_UNIT_CALL
wsf_unit_assert_equals_ptr(
    wsf_unit_test_case_t *test_case,
    const char *message,
    const void *expected,
    const void *actual,
    const int line_no)
{
    if (!test_case->status)
    {
        return;
    }

    if (expected == actual)
    {
        return;
    }

    test_case->status = WSFC_UNIT_FAILURE;
    if (message)
    {
        wsf_unit_print_error_message("Line %d: %s\n%-20s:  ", line_no, message, "");
    }
    else
    {
        wsf_unit_print_error_message(
             "Line %d: expected <%p>, but saw <%p>\n%-20s:  ", line_no,
             expected, actual, "");   
    }
}

WSFC_UNIT_EXTERN void WSFC_UNIT_CALL
wsf_unit_assert_equals_size(
    wsf_unit_test_case_t *test_case,
    const char *message,
    const size_t expected,
    const size_t actual,
    const int line_no)
{
    if (!test_case->status)
    {
        return;
    }

    if (expected == actual)
    {
        return;
    }

    test_case->status = WSFC_UNIT_FAILURE;
    if (message)
    {
        wsf_unit_print_error_message("Line %d: %s\n%-20s:  ", line_no, message, "");
    }
    else
    {
        wsf_unit_print_error_message(
             "Line %d: expected <size:%lu>, but saw <size:%lu>\n%-20s:  ", line_no,
             (unsigned long)expected, (unsigned long)actual, "");   
    }
}

WSFC_UNIT_EXTERN void WSFC_UNIT_CALL
wsf_unit_assert_not_equals_int(
    wsf_unit_test_case_t *test_case,
    const char *message,
    const int expected,
    const int actual,
    const int line_no)
{
    if (!test_case->status)
    {
        return;
    }

    if (expected != actual)
    {
        return;
    }

    test_case->status = WSFC_UNIT_FAILURE;
    if (message)
    {
        wsf_unit_print_error_message("Line %d: %s\n%-20s:  ", line_no, message, "");
    }
    else
    {
        wsf_unit_print_error_message(
             "Line %d: did not expect <%d>, but saw <%d>\n%-20s:  ", line_no,
             expected, actual, "");   
    }
}

WSFC_UNIT_EXTERN void WSFC_UNIT_CALL
wsf_unit_assert_not_equals_float(
    wsf_unit_test_case_t *test_case,
    const char *message,
    const float expected,
    const float actual,
    const int line_no)
{
    if (!test_case->status)
    {
        return;
    }

    if (expected != actual)
    {
        return;
    }

    test_case->status = WSFC_UNIT_FAILURE;
    if (message)
    {
        wsf_unit_print_error_message("Line %d: %s\n%-20s:  ", line_no, message, "");
    }
    else
    {
        wsf_unit_print_error_message(
             "Line %d: did not expect <%f>, but saw <%f>\n%-20s:  ", line_no,
             expected, actual, "");   
    }
}

WSFC_UNIT_EXTERN void WSFC_UNIT_CALL
wsf_unit_assert_not_equals_double(
    wsf_unit_test_case_t *test_case,
    const char *message,
    const double expected,
    const double actual,
    const int line_no)
{
    if (!test_case->status)
    {
        return;
    }

    if (expected != actual)
    {
        return;
    }

    test_case->status = WSFC_UNIT_FAILURE;
    if (message)
    {
        wsf_unit_print_error_message("Line %d: %s\n%-20s:  ", line_no, message, "");
    }
    else
    {
        wsf_unit_print_error_message(
             "Line %d: did not expect <%f>, but saw <%f>\n%-20s:  ", line_no,
             expected, actual, "");   
    }
}

WSFC_UNIT_EXTERN void WSFC_UNIT_CALL
wsf_unit_assert_not_equals_char(
    wsf_unit_test_case_t *test_case,
    const char *message,
    const char expected,
    const char actual,
    const int line_no)
{
    if (!test_case->status)
    {
        return;
    }

    if (expected != actual)
    {
        return;
    }

    test_case->status = WSFC_UNIT_FAILURE;
    if (message)
    {
        wsf_unit_print_error_message("Line %d: %s\n%-20s:  ", line_no, message, "");
    }
    else
    {
        wsf_unit_print_error_message(
             "Line %d: did not expect <%c>, but saw <%c>\n%-20s:  ", line_no,
             expected, actual, "");   
    }
}

WSFC_UNIT_EXTERN void WSFC_UNIT_CALL
wsf_unit_assert_not_equals_string(
    wsf_unit_test_case_t *test_case,
    const char *message,
    const char *expected,
    const char *actual,
    const int line_no)
{
    int count = 0;

    if (!test_case->status)
    {
        return;
    }

    if (!expected || !actual)
    {
        return;
    }
    count = (int)strlen(expected);
    if ((int)strlen(actual) < count)
    {
        count = (int)strlen(actual);
    }
    if (expected && actual && strncmp(expected, actual, count))
    {
        return;
    }

    test_case->status = WSFC_UNIT_FAILURE;
    if (message)
    {
        wsf_unit_print_error_message("Line %d: %s\n%-20s:  ", line_no, message, "");
    }
    else
    {
        wsf_unit_print_error_message(
             "Line %d: did not expect <%d>, but saw <%d>\n%-20s:  ", line_no,
             expected, actual, "");   
    }
}

WSFC_UNIT_EXTERN void WSFC_UNIT_CALL
wsf_unit_assert_not_equals_ptr(
    wsf_unit_test_case_t *test_case,
    const char *message,
    const void *expected,
    const void *actual,
    const int line_no)
{
    if (!test_case->status)
    {
        return;
    }

    if (expected != actual)
    {
        return;
    }

    test_case->status = WSFC_UNIT_FAILURE;
    if (message)
    {
        wsf_unit_print_error_message("Line %d: %s\n%-20s:  ", line_no, message, "");
    }
    else
    {
        wsf_unit_print_error_message(
             "Line %d: did not expect <%p>, but saw <%p>\n%-20s:  ", line_no,
             expected, actual, "");   
    }
}

WSFC_UNIT_EXTERN void WSFC_UNIT_CALL
wsf_unit_assert_not_equals_size(
    wsf_unit_test_case_t *test_case,
    const char *message,
    const size_t expected,
    const size_t actual,
    const int line_no)
{
    if (!test_case->status)
    {
        return;
    }

    if (expected != actual)
    {
        return;
    }

    test_case->status = WSFC_UNIT_FAILURE;
    if (message)
    {
        wsf_unit_print_error_message("Line %d: %s\n%-20s:  ", line_no, message, "");
    }
    else
    {
        wsf_unit_print_error_message(
             "Line %d: did not expect <size:%lu>, but saw <size:%lu>\n%-20s:  ", line_no,
             (unsigned long)expected, (unsigned long)actual, "");   
    }
}

WSFC_UNIT_EXTERN void WSFC_UNIT_CALL
wsf_unit_assert_null(
    wsf_unit_test_case_t *test_case,
    const char *message,
    const void *pointer,
    const int line_no)
{
    if (!test_case->status)
    {
        return;
    }

    if (!pointer)
    {
        return;
    }

    test_case->status = WSFC_UNIT_FAILURE;
    if (message)
    {
        wsf_unit_print_error_message("Line %d: %s\n%-20s:  ", line_no, message, "");
    }
    else
    {
        wsf_unit_print_error_message(
             "Line %d: expected <NULL>, but did not see <NULL>\n%-20s:  ", line_no, "");   
    }
}

WSFC_UNIT_EXTERN void WSFC_UNIT_CALL
wsf_unit_assert_not_null(
    wsf_unit_test_case_t *test_case,
    const char *message,
    const void *pointer,
    const int line_no)
{
    if (!test_case->status)
    {
        return;
    }

    if (pointer)
    {
        return;
    }

    test_case->status = WSFC_UNIT_FAILURE;
    if (message)
    {
        wsf_unit_print_error_message("Line %d: %s\n%-20s:  ", line_no, message, "");
    }
    else
    {
        wsf_unit_print_error_message(
             "Line %d: did not expect <NULL>, but saw <NULL>\n%-20s:  ", line_no, "");   
    }
}

WSFC_UNIT_EXTERN void WSFC_UNIT_CALL
wsf_unit_assert_true(
    wsf_unit_test_case_t *test_case,
    const char *message,
    const int condition,
    const int line_no)
{
    if (!test_case->status)
    {
        return;
    }

    if (condition)
    {
        return;
    }

    test_case->status = WSFC_UNIT_FAILURE;
    if (message)
    {
        wsf_unit_print_error_message("Line %d: %s\n%-20s:  ", line_no, message, "");
    }
    else
    {
        wsf_unit_print_error_message(
             "Line %d: expected <TRUE>, but saw <FALSE>\n%-20s:  ", line_no, "");   
    }
}

WSFC_UNIT_EXTERN void WSFC_UNIT_CALL
wsf_unit_assert_false(
    wsf_unit_test_case_t *test_case,
    const char *message,
    const int condition,
    const int line_no)
{
    if (!test_case->status)
    {
        return;
    }

    if (!condition)
    {
        return;
    }

    test_case->status = WSFC_UNIT_FAILURE;
    if (message)
    {
        wsf_unit_print_error_message("Line %d: %s\n%-20s:  ",line_no, message, "");
    }
    else
    {
        wsf_unit_print_error_message(
             "Line %d: expected <FALSE>, but saw <TRUE>\n%-20s:  ", line_no, "");   
    }
}

WSFC_UNIT_EXTERN void WSFC_UNIT_CALL
wsf_unit_not_implemented(
    wsf_unit_test_case_t *test_case,
    const char *message,
    const int line_no)
{
    test_case->sub_suite->not_implemented++;
    if (message)
    {
        wsf_unit_print_error_message("Line %d: %s\n%-20s:  ", line_no, message, "");
    }
}

WSFC_UNIT_EXTERN void WSFC_UNIT_CALL
wsf_unit_fail(
    wsf_unit_test_case_t *test_case,
    const char *message,
    const int line_no)
{
    if (!test_case->status)
    {
        return;
    }

    test_case->status = WSFC_UNIT_FAILURE;
    if (message)
    {
        wsf_unit_print_error_message("Line %d: %s\n%-20s:  ", line_no, message, "");
    }
    else
    {
        wsf_unit_print_error_message(
             "Line %d: Failed\n%-20s:  ", line_no, "");   
    }
}

WSFC_UNIT_EXTERN int WSFC_UNIT_CALL
wsf_unit_execute(
    const int argc,
    const char *const argv[],
    const wsf_unit_char_t *suite_name,
    const int testc,
    const wsf_unit_test_list_t testv[])
{
    int i = 0, j = 0;
    wsf_unit_bool_t list_provided = WSFC_UNIT_FALSE;
    wsf_unit_suite_t *suite = NULL;

    list_tests = WSFC_UNIT_FALSE;
    invert = WSFC_UNIT_FALSE;
    test_list = NULL;
    quiet = !isatty(STDOUT_FILENO);

    for (i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-h"))
        {
            printf("Usage: %s [options] [-f log_file] [test_names]\n", argv[0]);
            printf(" -x to skip provided test names. If you do not\n");
            printf("    provide this option, only the names provided\n");
            printf("    wil run\n");
            printf(" -l to list available tests instead of running\n");
            printf(" -q for a quite mode operation\n");
            printf(" -f to provide a log_file\n");
            return 0;
        }
        if (!list_tests && !strcmp(argv[i], "-l"))
        {
            list_tests = WSFC_UNIT_TRUE;
            continue;
        }
        if (!invert && !strcmp(argv[i], "-x"))
        {
            invert = WSFC_UNIT_TRUE;
            continue;
        }
        if (!quiet && !strcmp(argv[i], "-q"))
        {
            quiet = WSFC_UNIT_TRUE;
            continue;
        }
        if (!log_file && !strcmp(argv[i], "-f"))
        {
            if (++i < argc)
            {
                log_file = fopen(argv[i], "a+");
            }
            else
            {
                fprintf(stderr, "No log file provided.\n");
            }
            continue;
        }
        if (argv[i][0] == '-')
        {
            fprintf(stderr, "Invalid option: `%s'\n", argv[i]);
            exit(1);
        }
        if (!list_provided)
        {
            j = i;
            list_provided = WSFC_UNIT_TRUE;
        }
    }

    wsf_unit_print_message("\n---------------------------------------------------");
    wsf_unit_print_message("\nUnit Test Suite     :  %s", suite_name);
    wsf_unit_print_message("\nAvailable Test Sets :  %d", testc);
    wsf_unit_print_message("\n===================================================\n");

    if (list_provided)
    {
        /* Save as much as possible space */
        test_list = calloc(argc - j + 1, sizeof(char *));
        i = j;
        for (j = 0; i < argc; i++, j++)
        {
            test_list[j] = argv[i];
        }
    }

    suite = wsf_unit_suite_create(NULL);

    for (i = 0; i < testc; i++)
    {
        testv[i].execute(suite);
    }

    wsf_unit_report_suite(suite);
    wsf_unit_suite_free(suite);
    if (test_list)
    {
        free(test_list);
        test_list = NULL;
    }
    if (log_file)
    {
        fclose(log_file);
        log_file = NULL;
    }
    return 0;
}

