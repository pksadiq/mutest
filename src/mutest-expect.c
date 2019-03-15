/* mutest-expect.c: Expected result
 *
 * µTest - Copyright 2019  Emmanuele Bassi
 *
 * SPDX-License-Identifier: MIT
 */

#include "mutest-private.h"

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include <float.h>

static mutest_expect_res_t *
mutest_collect_true (mutest_expect_type_t value_type MUTEST_UNUSED,
                     mutest_collect_type_t collect_type MUTEST_UNUSED,
                     va_list args MUTEST_UNUSED)
{
  mutest_expect_res_t *retval = mutest_expect_res_alloc (MUTEST_EXPECT_BOOLEAN);

  retval->expect.v_bool = true;

  return retval;
}

static mutest_expect_res_t *
mutest_collect_false (mutest_expect_type_t value_type MUTEST_UNUSED,
                      mutest_collect_type_t collect_type MUTEST_UNUSED,
                      va_list args MUTEST_UNUSED)
{
  mutest_expect_res_t *retval = mutest_expect_res_alloc (MUTEST_EXPECT_BOOLEAN);

  retval->expect.v_bool = false;

  return retval;
}

static mutest_expect_res_t *
mutest_collect_null (mutest_expect_type_t value_type MUTEST_UNUSED,
                     mutest_collect_type_t collect_type MUTEST_UNUSED,
                     va_list args MUTEST_UNUSED)
{
  mutest_expect_res_t *retval = mutest_expect_res_alloc (MUTEST_EXPECT_POINTER);

  retval->expect.v_pointer = NULL;

  return retval;
}

static mutest_expect_res_t *
mutest_collect_nan (mutest_expect_type_t value_type MUTEST_UNUSED,
                    mutest_collect_type_t collect_type MUTEST_UNUSED,
                    va_list args MUTEST_UNUSED)
{
  mutest_expect_res_t *retval = mutest_expect_res_alloc (MUTEST_EXPECT_FLOAT);

  retval->expect.v_float.value = NAN;
  retval->expect.v_float.tolerance = DBL_EPSILON;

  return retval;
}

static mutest_expect_res_t *
mutest_collect_infinity (mutest_expect_type_t value_type MUTEST_UNUSED,
                         mutest_collect_type_t collect_type MUTEST_UNUSED,
                         va_list args MUTEST_UNUSED)
{
  mutest_expect_res_t *retval = mutest_expect_res_alloc (MUTEST_EXPECT_FLOAT);

  retval->expect.v_float.value = INFINITY;
  retval->expect.v_float.tolerance = DBL_EPSILON;

  return retval;
}

static mutest_expect_res_t *
mutest_collect_boolean (mutest_expect_type_t value_type MUTEST_UNUSED,
                        mutest_collect_type_t collect_type MUTEST_UNUSED,
                        va_list args)
{
  mutest_expect_res_t *retval = mutest_expect_res_alloc (MUTEST_EXPECT_BOOLEAN);

  int val = va_arg (args, int);
  retval->expect.v_bool = !!val;

  return retval;
}

static mutest_expect_res_t *
mutest_collect_number (mutest_expect_type_t value_type,
                       mutest_collect_type_t collect_type,
                       va_list args)
{
  bool collect_int = (collect_type & MUTEST_COLLECT_INT) != 0;
  bool collect_float = (collect_type & MUTEST_COLLECT_FLOAT) != 0;
  bool collect_precision = (collect_type & MUTEST_COLLECT_PRECISION) != 0;
  bool collect_matching = (collect_type & MUTEST_COLLECT_MATCHING_TYPE) != 0;
  bool collect_range = (collect_type & MUTEST_COLLECT_RANGE) != 0;

  mutest_expect_res_t *retval = NULL;

  if (collect_matching && collect_range)
    {
      if (value_type == MUTEST_EXPECT_FLOAT)
        retval = mutest_expect_res_alloc (MUTEST_EXPECT_FLOAT_RANGE);
      else if (value_type == MUTEST_EXPECT_INT)
        retval = mutest_expect_res_alloc (MUTEST_EXPECT_INT_RANGE);
    }
  else if (collect_matching)
    retval = mutest_expect_res_alloc (value_type);
  else if (collect_range)
    {
      if (collect_float)
        retval = mutest_expect_res_alloc (MUTEST_EXPECT_FLOAT_RANGE);
      else if (collect_int)
        retval = mutest_expect_res_alloc (MUTEST_EXPECT_INT_RANGE);
    }
  else
    {
      if (collect_float)
        retval = mutest_expect_res_alloc (MUTEST_EXPECT_FLOAT);
      else if (collect_int)
        retval = mutest_expect_res_alloc (MUTEST_EXPECT_INT);
    }

  if (retval == NULL)
    mutest_assert_if_reached ("invalid value");

  switch (retval->expect_type)
    {
    case MUTEST_EXPECT_INVALID:
      mutest_assert_if_reached ("invalid value");
      break;

    case MUTEST_EXPECT_INT:
      retval->expect.v_int = va_arg (args, int);
      break;

    case MUTEST_EXPECT_FLOAT:
      retval->expect.v_float.value = va_arg (args, double);
      if (collect_precision)
        retval->expect.v_float.tolerance = va_arg (args, double);
      else
        retval->expect.v_float.tolerance = DBL_EPSILON;
      break;

    case MUTEST_EXPECT_INT_RANGE:
      retval->expect.v_irange.min = va_arg (args, int);
      retval->expect.v_irange.max = va_arg (args, int);

      if (retval->expect.v_irange.min > retval->expect.v_irange.max)
        mutest_assert_if_reached ("invalid range");
      break;

    case MUTEST_EXPECT_FLOAT_RANGE:
      retval->expect.v_frange.min = va_arg (args, double);
      retval->expect.v_frange.max = va_arg (args, double);

      if (retval->expect.v_frange.min > retval->expect.v_frange.max)
        mutest_assert_if_reached ("invalid range");
      break;

    case MUTEST_EXPECT_BOOLEAN:
    case MUTEST_EXPECT_STR:
    case MUTEST_EXPECT_POINTER:
      mutest_assert_if_reached ("invalid number");
      break;
    }

  return retval;
}

static mutest_expect_res_t *
mutest_collect_string (mutest_expect_type_t value_type MUTEST_UNUSED,
                       mutest_collect_type_t collect_type MUTEST_UNUSED,
                       va_list args)
{
  mutest_expect_res_t *retval = mutest_expect_res_alloc (MUTEST_EXPECT_STR);

  char *str = va_arg (args, char *);
  retval->expect.v_str.str = strdup (str);
  retval->expect.v_str.len = str != NULL ? strlen (str) : 0;

  return retval;
}

static mutest_expect_res_t *
mutest_collect_pointer (mutest_expect_type_t value_type MUTEST_UNUSED,
                        mutest_collect_type_t collect_type MUTEST_UNUSED,
                        va_list args)
{
  mutest_expect_res_t *retval = mutest_expect_res_alloc (MUTEST_EXPECT_POINTER);

  retval->expect.v_pointer = va_arg (args, void *);

  return retval;
}

static mutest_expect_res_t *
mutest_collect_scalar (mutest_expect_type_t value_type,
                       mutest_collect_type_t collect_type,
                       va_list args)
{
  bool collect_matching = (collect_type & MUTEST_COLLECT_MATCHING_TYPE) != 0;
  bool collect_string = (collect_type & MUTEST_COLLECT_STRING) != 0;
  bool collect_int = (collect_type & MUTEST_COLLECT_INT) != 0;
  bool collect_float = (collect_type & MUTEST_COLLECT_FLOAT) != 0;
  bool collect_boolean = (collect_type & MUTEST_COLLECT_BOOLEAN) != 0;
  bool collect_pointer = (collect_type & MUTEST_COLLECT_POINTER) != 0;

  /* We need to have the matching type */
  if (!collect_matching)
    mutest_assert_if_reached ("no specified value type");

  if (value_type == MUTEST_EXPECT_BOOLEAN && collect_boolean)
    return mutest_collect_boolean (value_type, collect_type, args);

  if ((value_type == MUTEST_EXPECT_INT && collect_int) ||
      (value_type == MUTEST_EXPECT_FLOAT && collect_float))
    return mutest_collect_number (value_type, collect_type, args);

  if (value_type == MUTEST_EXPECT_STR && collect_string)
    return mutest_collect_string (value_type, collect_type, args);

  if (value_type == MUTEST_EXPECT_POINTER && collect_pointer)
    return mutest_collect_pointer (value_type, collect_type, args);

  return NULL;
}

static const struct {
  mutest_matcher_func_t matcher;
  mutest_collect_type_t collect_rule;
  mutest_collect_func_t collector;
  const char *repr;
} matchers[] = {
  /* Unary matchers */
  { mutest_to_be_true, MUTEST_COLLECT_NONE, mutest_collect_true, "true" },
  { mutest_to_be_false, MUTEST_COLLECT_NONE, mutest_collect_false, "false" },
  { mutest_to_be_null, MUTEST_COLLECT_NONE, mutest_collect_null, "null" },
  { mutest_to_be_nan, MUTEST_COLLECT_NONE, mutest_collect_nan, "NaN" },
  { mutest_to_be_positive_infinity, MUTEST_COLLECT_NONE, mutest_collect_infinity, "+∞" },
  { mutest_to_be_negative_infinity, MUTEST_COLLECT_NONE, mutest_collect_infinity, "-∞" },

  /* Numeric matchers */
  { mutest_to_be_close_to,
    MUTEST_COLLECT_NUMBER | MUTEST_COLLECT_PRECISION | MUTEST_COLLECT_MATCHING_TYPE,
    mutest_collect_number,
    NULL,
  },
  { mutest_to_be_greater_than,
    MUTEST_COLLECT_NUMBER | MUTEST_COLLECT_MATCHING_TYPE,
    mutest_collect_number,
    NULL,
  },
  { mutest_to_be_greater_than_or_equal,
    MUTEST_COLLECT_NUMBER | MUTEST_COLLECT_MATCHING_TYPE,
    mutest_collect_number,
    NULL,
  },
  { mutest_to_be_less_than_or_equal,
    MUTEST_COLLECT_NUMBER | MUTEST_COLLECT_MATCHING_TYPE,
    mutest_collect_number,
    NULL,
  },
  { mutest_to_be_less_than,
    MUTEST_COLLECT_NUMBER | MUTEST_COLLECT_MATCHING_TYPE,
    mutest_collect_number,
    NULL,
  },
  { mutest_to_be_in_range,
    MUTEST_COLLECT_NUMBER | MUTEST_COLLECT_MATCHING_TYPE | MUTEST_COLLECT_RANGE,
    mutest_collect_number,
    NULL,
  },

  /* String matchers */
  { mutest_to_start_with_string,
    MUTEST_COLLECT_STRING,
    mutest_collect_string,
    NULL,
  },
  { mutest_to_end_with_string,
    MUTEST_COLLECT_STRING,
    mutest_collect_string,
    NULL,
  },
  { mutest_to_contain_string,
    MUTEST_COLLECT_STRING,
    mutest_collect_string,
    NULL,
  },

  /* Generic scalar matcher */
  { mutest_to_be,
    MUTEST_COLLECT_SCALAR | MUTEST_COLLECT_MATCHING_TYPE,
    mutest_collect_scalar,
    NULL,
  },

  /* Legacy matchers */
  { mutest_to_be_int_value,
    MUTEST_COLLECT_INT,
    mutest_collect_number,
    NULL,
  },
  { mutest_to_be_in_int_range,
    MUTEST_COLLECT_INT | MUTEST_COLLECT_RANGE,
    mutest_collect_number,
    NULL,
  },
  { mutest_to_be_float_value,
    MUTEST_COLLECT_FLOAT | MUTEST_COLLECT_PRECISION,
    mutest_collect_number,
    NULL,
  },
  { mutest_to_be_in_float_range,
    MUTEST_COLLECT_FLOAT | MUTEST_COLLECT_RANGE,
    mutest_collect_number,
    NULL,
  },
  { mutest_to_be_string,
    MUTEST_COLLECT_STRING,
    mutest_collect_string,
    NULL,
  },
  { mutest_to_be_pointer,
    MUTEST_COLLECT_POINTER,
    mutest_collect_pointer,
    NULL,
  },
};

static const size_t n_matchers = sizeof (matchers) / sizeof (matchers[0]);

void
mutest_expect_full (const char *file,
                    int line,
                    const char *func_name,
                    const char *description,
                    mutest_expect_res_t *value,
                    mutest_matcher_func_t first_matcher_func,
                    ...)
{
  if (description == NULL)
    mutest_assert_if_reached ("invalid description");

  if (value == NULL)
    mutest_assert_if_reached ("invalid data pointer");

  if (first_matcher_func == NULL)
    mutest_assert_if_reached ("invalid matcher");

  mutest_expect_t e = {
    .description = description,
    .value = value,
    .file = file,
    .line = line,
    .func_name = func_name,
    .result = MUTEST_RESULT_PASS,
  };

  va_list args;

  va_start (args, first_matcher_func);

  mutest_matcher_func_t matcher_func = first_matcher_func;

  while (matcher_func != NULL)
    {
      mutest_expect_res_t *check = NULL;
      bool negate = false;
      const char *repr = NULL;

      if (matcher_func == mutest_skip)
        {
          e.result = MUTEST_RESULT_SKIP;
          break;
        }

      if (matcher_func == mutest_not)
        {
          negate = true;
          matcher_func = va_arg (args, void *);
        }

      for (size_t i = 0; i < n_matchers; i++)
        {
          if (matchers[i].matcher == matcher_func)
            {
              repr = matchers[i].repr;
              check = matchers[i].collector (value->expect_type,
                                             matchers[i].collect_rule,
                                             args);
              break;
            }
        }

      /* If we're using a custom matcher then we collect
       * the value as a pointer instead of unpacking raw
       * arguments. The ownership of the comparison value
       * is transferred to us in any case.
       */
      if (check == NULL)
        check = va_arg (args, mutest_expect_res_t *);

      bool res = matcher_func (&e, check);

      res = negate ? !res : res;

      if (!res)
        mutest_print_expect_fail (&e, negate, check, repr);

      if (e.result == MUTEST_RESULT_PASS)
        e.result = res ? MUTEST_RESULT_PASS : MUTEST_RESULT_FAIL;

      mutest_expect_res_free (check);

      matcher_func = va_arg (args, void *);
    }

  va_end (args);

  mutest_spec_add_result (mutest_get_current_spec (), &e);

  mutest_print_expect (&e);

  mutest_expect_res_free (value);
}

mutest_expect_res_t *
mutest_expect_value (mutest_expect_t *e)
{
  if (e == NULL)
    mutest_assert_if_reached ("invalid expectation");

  return e->value;
}
