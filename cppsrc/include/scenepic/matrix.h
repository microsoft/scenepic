// ----------------------------------------------------------------------------
//
// matrix.h -- Shared definitions and functions for vectors and matrices
//
// Copyright (C) 2019 Microsoft
//
// For conditions of distribution and use, see copyright notice in LICENSE
//
// ----------------------------------------------------------------------------

#ifndef _SCENEPIC_MATRIX_H_
#define _SCENEPIC_MATRIX_H_

#include <cstdint>
#include <limits>
#include <string>
#include <Eigen/Core>
#include <Eigen/Sparse>

#include "base64.h"
#include "compression.h"

namespace scenepic
{
typedef Eigen::Matrix4f Transform;
typedef Eigen::Matrix<float, 3, 3, Eigen::RowMajor> Intrinsic;
typedef Eigen::Matrix<float, 4, 4, Eigen::RowMajor> Extrinsic;
typedef Eigen::SparseMatrix<float> SparseMatrix;
typedef Eigen::RowVector3f Vector;
typedef Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor> VectorBuffer;
typedef Eigen::MatrixX4f WorldVectorMatrix;
typedef Eigen::Matrix<std::uint32_t, 1, 3> Triangle;
typedef Eigen::Matrix<std::uint32_t, Eigen::Dynamic, 3, Eigen::RowMajor> TriangleBuffer;
typedef Eigen::Matrix<std::uint16_t, Eigen::Dynamic, 3, Eigen::RowMajor> TriangleShortBuffer;
typedef Eigen::Matrix<std::uint32_t, 1, 2> Line;
typedef Eigen::Matrix<std::uint32_t, Eigen::Dynamic, 2, Eigen::RowMajor> LineBuffer;
typedef Eigen::Matrix<std::uint16_t, Eigen::Dynamic, 2, Eigen::RowMajor> LineShortBuffer;
typedef Eigen::RowVector4f Quaternion;
typedef Eigen::RowVector2f UV;
typedef Eigen::Matrix<float, Eigen::Dynamic, 2, Eigen::RowMajor> UVBuffer;
typedef Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor> ColorBuffer;
typedef Eigen::Matrix<float, Eigen::Dynamic, 4, Eigen::RowMajor> QuaternionBuffer;
typedef Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> InstanceBuffer;
typedef Eigen::Vector2f Coordinate;
typedef Eigen::Matrix<float, 1, 4> Circle;
typedef Eigen::Matrix<std::uint8_t, 1, 8> Style;
typedef Eigen::Matrix<std::uint16_t, 1, 3> PolyLine;
typedef Eigen::Matrix<float, Eigen::Dynamic, 1> ValueBuffer;
typedef Eigen::Matrix<float, Eigen::Dynamic, 2, Eigen::RowMajor> CoordinateBuffer;
typedef Eigen::Matrix<float, Eigen::Dynamic, 4, Eigen::RowMajor> CircleBuffer;
typedef Eigen::Matrix<std::uint8_t, Eigen::Dynamic, 8, Eigen::RowMajor> StyleBuffer;
typedef Eigen::Matrix<std::uint16_t, Eigen::Dynamic, 3, Eigen::RowMajor> PolyLineBuffer;
typedef Eigen::Matrix<std::uint8_t, Eigen::Dynamic, 1> IndexVector;

typedef Eigen::Matrix<float, 1, Eigen::Dynamic> Vertex;
typedef Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> VertexBuffer;
typedef Eigen::Matrix<std::uint16_t, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> FixedPointVertexBuffer;
typedef Eigen::Block<VertexBuffer, Eigen::Dynamic, Eigen::Dynamic, false> VertexBlock;
typedef Eigen::Block<const VertexBuffer, Eigen::Dynamic, Eigen::Dynamic, false> ConstVertexBlock;

typedef Eigen::Ref<const VectorBuffer> ConstVectorBufferRef;
typedef Eigen::Ref<const TriangleBuffer> ConstTriangleBufferRef;
typedef Eigen::Ref<const UVBuffer> ConstUVBufferRef;
typedef Eigen::Ref<const ColorBuffer> ConstColorBufferRef;
typedef Eigen::Ref<const QuaternionBuffer> ConstQuaternionBufferRef;
typedef Eigen::Ref<const VertexBuffer> ConstVertexBufferRef;
typedef Eigen::Ref<TriangleBuffer> TriangleBufferRef;
typedef Eigen::Ref<UVBuffer> UVBufferRef;
typedef Eigen::Ref<ColorBuffer> ColorBufferRef;
typedef Eigen::Ref<VectorBuffer> VectorBufferRef;
typedef Eigen::Ref<VertexBuffer> VertexBufferRef;

const VectorBuffer& VectorBufferNone();
const ColorBuffer& ColorBufferNone();
const UVBuffer& UVBufferNone();
const QuaternionBuffer& QuaternionBufferNone();
const Vector& VectorNone();

/** Appends a row to a matrix.
 *  \tparam Matrix an Eigen Matrix type
 *  \tparam Vector an Eigen Vector type congruent with the Matrix type
 *  \param matrix the matrix which will gain a row at the bottom
 *  \param row the row to append
 */
template <typename Matrix, typename Vector>
void append_row(Matrix &matrix, const Vector &row)
{
    matrix.conservativeResize(matrix.rows() + 1, Eigen::NoChange);
    matrix.row(matrix.rows() - 1) = row;
}

/** Appends a matrix to the bottom of another matrix.
 *  \tparam Matrix an Eigen Matrix type
 *  \param top the matrix which will gain rows at the bottom
 *  \param bottom the matrix to append
 */
template <typename Matrix>
void append_matrix(Matrix &top, const Matrix &bottom)
{
    top.conservativeResize(top.rows() + bottom.rows(), Eigen::NoChange);
    top.bottomRows(bottom.rows()) = bottom;
}

/** Convert a matrix to a JSON-friendly representation, i.e. a Base64 binary string of the
 *  row-major coefficient order.
 *  \tparam Matrix an Eigen Matrix type
 *  \param matrix the matrix to convert to a string
 *  \return the Base64 of the raw matrix binary
 */
template <typename Matrix>
std::string matrix_to_json(const Matrix &matrix)
{
    std::vector<std::uint8_t> bytes = compress_matrix(matrix);
    std::uint32_t length = static_cast<std::uint32_t>(bytes.size());
    return base64_encode(bytes.data(), static_cast<unsigned int>(length));
}

inline Eigen::Array<std::uint32_t, Eigen::Dynamic, 1> arange(std::uint32_t start, std::uint32_t end, std::uint32_t step = 1)
{
    auto length = (end - start) / step;
    return Eigen::Array<std::uint32_t, Eigen::Dynamic, 1>::LinSpaced(length, start, end);
}

template <typename Derived>
Eigen::Array<typename Derived::Scalar, Eigen::Dynamic, 1> roll(const Eigen::ArrayBase<Derived> &values, Eigen::Index steps)
{
    Eigen::Array<typename Derived::Scalar, Eigen::Dynamic, 1> result(values.rows());
    auto remainder = values.rows() - steps;
    result.head(steps) = values.tail(steps);
    result.tail(remainder) = values.head(remainder);
    return result;
}

template <typename Derived>
Eigen::Matrix<typename Derived::Scalar, Derived::RowsAtCompileTime, Derived::ColsAtCompileTime> rowwise_cumsum(const Eigen::MatrixBase<Derived>& values)
{
    typedef Eigen::Matrix<typename Derived::Scalar, Derived::RowsAtCompileTime, Derived::ColsAtCompileTime> MatrixType;
    MatrixType result = MatrixType::Zero(values.rows(), values.cols());

    result.row(0) = values.row(0);
    for(Eigen::Index row=1; row < result.rows(); ++row)
    {
        result.row(row) = result.row(row - 1) + values.row(row);
    }

    return result;
}

template <typename Derived>
Eigen::Matrix<typename Derived::Scalar, Derived::RowsAtCompileTime, Derived::ColsAtCompileTime> random(Eigen::Index rows, Eigen::Index cols, float min, float max)
{
    typedef Eigen::Matrix<typename Derived::Scalar, Derived::RowsAtCompileTime, Derived::ColsAtCompileTime> MatrixType;
    MatrixType result = MatrixType::Random(rows, cols);
    float offset = 1 + min;
    float scale = (max - min) / 2;
    result = result.array() + offset;
    result = result * scale;
    return result;
}

template <typename Derived>
Eigen::Matrix<typename Derived::Scalar, Derived::RowsAtCompileTime, Derived::ColsAtCompileTime> random(float min, float max)
{
    return random<typename Derived>(Derived::RowsAtCompileTime, Derived::ColsAtCompileTime, min, max);
}

} // namespace scenepic

#endif