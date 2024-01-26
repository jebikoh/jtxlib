#include <check.h>
#include <stdlib.h>

#include "../include/util.h"

START_TEST(test_init_vec3) {
  Vec3 v;
  init_vec3(&v, 1, 2, 3);
  ck_assert(v.x == 1);
  ck_assert(v.y == 2);
  ck_assert(v.z == 3);
}

START_TEST(test_add_vec3) {
  Vec3 a, b;
  init_vec3(&a, 1, 2, 3);
  init_vec3(&b, 4, 5, 6);
  add_vec3(&a, &b);
  ck_assert(a.x == 5);
  ck_assert(a.y == 7);
  ck_assert(a.z == 9);
}

START_TEST(test_sub_vec3) {
  Vec3 a, b;
  init_vec3(&a, 5, 4, 3);
  init_vec3(&b, 1, 2, 3);
  sub_vec3(&a, &b);
  ck_assert(a.x == 4);
  ck_assert(a.y == 2);
  ck_assert(a.z == 0);
}

START_TEST(test_normalize_vec3) {
  Vec3 a;
  init_vec3(&a, 3.0, 4.0, 0.0);
  normalize_vec3(&a);
  float length = sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
  ck_assert_float_eq_tol(length, 1.0, 0.0001);
}

START_TEST(test_deg_to_rad) {
  ck_assert_float_eq_tol(deg_to_rad(90.0), M_PI / 2.0, 0.0001);
}

START_TEST(test_rmat_90_x) {
  Mat4 m;
  build_rotation(&m, deg_to_rad(90.0), X);
  ck_assert_float_eq_tol(m.m[0][0], 1.0, 0.0001);
  ck_assert_float_eq_tol(m.m[0][1], 0.0, 0.0001);
  ck_assert_float_eq_tol(m.m[0][2], 0.0, 0.0001);
  ck_assert_float_eq_tol(m.m[0][3], 0.0, 0.0001);
  ck_assert_float_eq_tol(m.m[1][0], 0.0, 0.0001);
  ck_assert_float_eq_tol(m.m[1][1], 0.0, 0.0001);
  ck_assert_float_eq_tol(m.m[1][2], -1.0, 0.0001);
  ck_assert_float_eq_tol(m.m[1][3], 0.0, 0.0001);
  ck_assert_float_eq_tol(m.m[2][0], 0.0, 0.0001);
  ck_assert_float_eq_tol(m.m[2][1], 1.0, 0.0001);
  ck_assert_float_eq_tol(m.m[2][2], 0.0, 0.0001);
  ck_assert_float_eq_tol(m.m[2][3], 0.0, 0.0001);
  ck_assert_float_eq_tol(m.m[3][0], 0.0, 0.0001);
  ck_assert_float_eq_tol(m.m[3][1], 0.0, 0.0001);
  ck_assert_float_eq_tol(m.m[3][2], 0.0, 0.0001);
  ck_assert_float_eq_tol(m.m[3][3], 1.0, 0.0001);
}

START_TEST(test_rmat_90_y) {
  Mat4 m;
  build_rotation(&m, deg_to_rad(90.0), Y);
  ck_assert_float_eq_tol(m.m[0][0], 0.0, 0.0001);
  ck_assert_float_eq_tol(m.m[0][1], 0.0, 0.0001);
  ck_assert_float_eq_tol(m.m[0][2], 1.0, 0.0001);
  ck_assert_float_eq_tol(m.m[0][3], 0.0, 0.0001);
  ck_assert_float_eq_tol(m.m[1][0], 0.0, 0.0001);
  ck_assert_float_eq_tol(m.m[1][1], 1.0, 0.0001);
  ck_assert_float_eq_tol(m.m[1][2], 0.0, 0.0001);
  ck_assert_float_eq_tol(m.m[1][3], 0.0, 0.0001);
  ck_assert_float_eq_tol(m.m[2][0], -1.0, 0.0001);
  ck_assert_float_eq_tol(m.m[2][1], 0.0, 0.0001);
  ck_assert_float_eq_tol(m.m[2][2], 0.0, 0.0001);
  ck_assert_float_eq_tol(m.m[2][3], 0.0, 0.0001);
  ck_assert_float_eq_tol(m.m[3][0], 0.0, 0.0001);
  ck_assert_float_eq_tol(m.m[3][1], 0.0, 0.0001);
  ck_assert_float_eq_tol(m.m[3][2], 0.0, 0.0001);
  ck_assert_float_eq_tol(m.m[3][3], 1.0, 0.0001);
}

START_TEST(test_rmat_90_z) {
  Mat4 m;
  build_rotation(&m, deg_to_rad(90.0), Z);
  ck_assert_float_eq_tol(m.m[0][0], 0.0, 0.0001);
  ck_assert_float_eq_tol(m.m[0][1], -1.0, 0.0001);
  ck_assert_float_eq_tol(m.m[0][2], 0.0, 0.0001);
  ck_assert_float_eq_tol(m.m[0][3], 0.0, 0.0001);
  ck_assert_float_eq_tol(m.m[1][0], 1.0, 0.0001);
  ck_assert_float_eq_tol(m.m[1][1], 0.0, 0.0001);
  ck_assert_float_eq_tol(m.m[1][2], 0.0, 0.0001);
  ck_assert_float_eq_tol(m.m[1][3], 0.0, 0.0001);
  ck_assert_float_eq_tol(m.m[2][0], 0.0, 0.0001);
  ck_assert_float_eq_tol(m.m[2][1], 0.0, 0.0001);
  ck_assert_float_eq_tol(m.m[2][2], 1.0, 0.0001);
  ck_assert_float_eq_tol(m.m[2][3], 0.0, 0.0001);
  ck_assert_float_eq_tol(m.m[3][0], 0.0, 0.0001);
  ck_assert_float_eq_tol(m.m[3][1], 0.0, 0.0001);
  ck_assert_float_eq_tol(m.m[3][2], 0.0, 0.0001);
  ck_assert_float_eq_tol(m.m[3][3], 1.0, 0.0001);
}

Suite *util_suite(void) {
  Suite *s;
  TCase *tc_core;

  s = suite_create("util");

  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_init_vec3);
  tcase_add_test(tc_core, test_add_vec3);
  tcase_add_test(tc_core, test_sub_vec3);
  tcase_add_test(tc_core, test_normalize_vec3);
  tcase_add_test(tc_core, test_deg_to_rad);
  tcase_add_test(tc_core, test_rmat_90_x);
  tcase_add_test(tc_core, test_rmat_90_y);
  tcase_add_test(tc_core, test_rmat_90_z);
  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {
  int number_failed;
  Suite *s;
  SRunner *sr;

  s = util_suite();
  sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);

  srunner_free(sr);

  return number_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}