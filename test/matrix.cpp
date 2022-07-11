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

    Eigen::RowVectorXf vec(4);
    vec << -0.87, 1, 0, -1;

    Eigen::MatrixXf expected (4,4);
    expected   << 0, -1, 3, -0.03, 0, 2, 0, 3, 0.51, -0.22, 1, 2, -0.87, 1, 0, -1;
    sp::append_row(actual, vec);
    

    test::assert_allclose(actual, expected, result, "append_row");

    return result;
}