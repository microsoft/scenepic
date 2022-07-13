// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "matrix.h"

#include "scenepic_tests.h"
#include "transforms.h"


namespace sp = scenepic;

int test_matrix()
{
    int result = EXIT_SUCCESS;

    Eigen::MatrixXf actual(3,4);
    actual << 0, -1, 3, -0.03, 0, 2, 0, 3, 0.51, -0.22, 1, 2;

    sp::Color4 vec;
    vec << -0.87, 1, 0, -1;

    Eigen::MatrixXf expected (4,4);
    expected << 0, -1, 3, -0.03, 0, 2, 0, 3, 0.51, -0.22, 1, 2, -0.87, 1, 0, -1;

    sp::append_row(actual, vec);
    test::assert_allclose(actual, expected, result, "append_row");

    Eigen::MatrixXf top(2,4);
    Eigen::MatrixXf bottom(2,4);
    top << 0, -1, 3, -0.03, 0, 2, 0, 3;
    bottom <<  0.51, -0.22, 1, 2, -0.87, 1, 0, -1;


    sp::append_matrix(top, bottom);
    test::assert_allclose(top, expected, result, "append_matrix");

    Eigen::Array<std::uint32_t, Eigen::Dynamic, 1> ExpectedArray(8);
    ExpectedArray<< 0, 2, 4, 6, 8, 10, 12, 14;
    test::assert_allclose(sp::arange(0, 17, 2), ExpectedArray, result, "arange");




    return result;
}