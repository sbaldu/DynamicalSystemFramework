//! SparseMatrix class v1.7.0 by Grufoony

//!  This class implements a sparse matrix. The matrix is stored in a compressed
//!  row format. ++ 20 requiered.

#ifndef SparseMatrix_hpp
#define SparseMatrix_hpp

#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <vector>

namespace dmf {

  template <typename Index, typename T>
  class SparseMatrix {
    std::unordered_map<Index, T> _matrix;
    Index _rows, _cols;
    T _defaultReturn;

  public:
    SparseMatrix();

    /// @brief SparseMatrix constructor
    /// @param rows number of rows
    /// @param cols number of columns
    /// @throw std::invalid_argument if rows or cols are < 0
    SparseMatrix(Index rows, Index cols);

    /// @brief SparseMatrix constructor - colum
    /// @param index number of rows
    /// @throw std::invalid_argument if index is < 0
    SparseMatrix(Index index);

    /// @brief insert a value in the matrix
    /// @param i row index
    /// @param j column index
    /// @param value value to insert
    /// @throw std::out_of_range if the index is out of range
    void insert(Index i, Index j, T value);

    /// @brief insert a value in the matrix
    /// @param i index
    /// @param value value to insert
    /// @throw std::out_of_range if the index is out of range
    void insert(Index i, T value);

    /// @brief insert a value in the matrix. If the element already exist, it
    /// overwrites it
    /// @param i row index
    /// @param j column index
    /// @param value value to insert
    /// @throw std::out_of_range if the index is out of range
    void insert_or_assign(Index i, Index j, T value);

    /// @brief insert a value in the matrix. If the element already exist, it
    /// overwrites it
    /// @param index index in vectorial form
    /// @param value value to insert
    /// @throw std::out_of_range if the index is out of range
    void insert_or_assign(Index index, T value);

    /// @brief remove a value from the matrix
    /// @param i row index
    /// @param j column index
    /// @throw std::out_of_range if the index is out of range
    /// @throw std::runtime_error if the element is not found
    void erase(Index i, Index j);

    /// @brief remove a row from the matrix
    /// @param index row index
    /// @throw std::out_of_range if the index is out of range
    void eraseRow(Index index);

    /// @brief remove a column from the matrix
    /// @param index column index
    /// @throw std::out_of_range if the index is out of range
    void eraseColumn(Index index);

    /// @brief empty the matrix and set the dimensions to zero
    void clear();

    /// @brief check if the element is non zero
    /// @param i row index
    /// @param j column index
    /// @return true if the element is non zero
    /// @throw std::out_of_range if the index is out of range
    bool contains(Index i, Index j) const;

    /// @brief check if the element is non zero
    /// @param index index in vectorial form
    /// @return true if the element is non zero
    /// @throw std::out_of_range if the index is out of range
    bool contains(Index const index) const;

    /// @brief get the input degree of all nodes
    /// @return a SparseMatrix vector with the input degree of all nodes
    /// @throw std::runtime_error if the matrix is not square
    SparseMatrix<Index, int> getDegreeVector();

    /// @brief get the strength of all nodes
    /// @return a SparseMatrix vector with the strength of all nodes
    /// @throw std::runtime_error if the matrix is not square
    SparseMatrix<Index, double> getStrengthVector();

    /// @brief get the laplacian matrix
    /// @return the laplacian matrix
    /// @throw std::runtime_error if the matrix is not square
    SparseMatrix<Index, int> getLaplacian();

    /// @brief get a row as a row vector
    /// @param index row index
    /// @return a row vector
    /// @throw std::out_of_range if the index is out of range
    SparseMatrix getRow(Index index) const;

    /// @brief get a column as a column vector
    /// @param index column index
    /// @return a column vector
    /// @throw std::out_of_range if the index is out of range
    SparseMatrix getCol(Index index) const;

    /// @brief get a matrix of double with every row normalized to 1
    /// @return a matrix of double
    SparseMatrix<Index, double> getNormRows() const;

    /// @brief get a matrix of double with every column normalized to 1
    /// @return a matrix of double
    SparseMatrix<Index, double> getNormCols() const;

    /// @brief get the number of rows
    /// @return number of rows
    Index getRowDim() const;

    /// @brief get the number of columns
    /// @return number of columns
    Index getColDim() const;

    /// @brief get the number of non zero elements in the matrix
    /// @return number of non zero elements
    Index size() const;

    /// @brief get the maximum number of elements in the matrix
    /// @return maximum number of elements
    Index max_size() const;

    /// @brief symmetrize the matrix
    void symmetrize();

    /// @brief return the begin iterator of the matrix
    /// @return the begin iterator
    typename std::unordered_map<Index, T>::const_iterator begin() const;

    /// @brief return the end iterator of the matrix
    /// @return the end iterator
    typename std::unordered_map<Index, T>::const_iterator end() const;

    /// @brief access an element of the matrix
    /// @param i row index
    /// @param j column index
    /// @return the element
    /// @throw std::out_of_range if the index is out of range
    const T& operator()(Index i, Index j) const;

    /// @brief access an element of the matrix
    /// @param i row index
    /// @param j column index
    /// @return the element
    /// @throw std::out_of_range if the index is out of range
    T& operator()(Index i, Index j);

    /// @brief access an element of the matrix
    /// @param index index in vectorial form
    /// @return the element
    /// @throw std::out_of_range if the index is out of range
    const T& operator()(Index index) const;

    /// @brief access an element of the matrix
    /// @param index index in vectorial form
    /// @return the element
    /// @throw std::out_of_range if the index is out of range
    T& operator()(Index index);

    /// @brief sum of two matrices
    /// @param other the other matrix
    /// @return the sum of the two matrices
    /// @throw std::runtime_error if the dimensions do not match
    template <typename I, typename U>
    SparseMatrix<Index, T> operator+(const SparseMatrix<I, U>& other) {
      if (this->_rows != other._rows || this->_cols != other._cols) {
        throw std::runtime_error("SparseMatrix: dimensions do not match");
      }
      auto result = SparseMatrix<Index, T>(this->_rows, this->_cols);
      std::unordered_map<Index, bool> unique;
      for (auto& it : this->_matrix) {
        unique.insert_or_assign(it.first, true);
      }
      for (auto& it : other._matrix) {
        unique.insert_or_assign(it.first, true);
      }
      for (auto& it : unique) {
        result.insert(it.first / this->_cols, it.first % this->_cols, (*this)(it.first) + other(it.first));
      }
      return result;
    }

    /// @brief difference of two matrices
    /// @param other the other matrix
    /// @return the difference of the two matrices
    /// @throw std::runtime_error if the dimensions do not match
    template <typename I, typename U>
    SparseMatrix<Index, T> operator-(const SparseMatrix<I, U>& other) {
      if (this->_rows != other._rows || this->_cols != other._cols) {
        throw std::runtime_error("SparseMatrix: dimensions do not match");
      }
      auto result = SparseMatrix(this->_rows, this->_cols);
      std::unordered_map<Index, bool> unique;
      for (auto& it : this->_matrix) {
        unique.insert_or_assign(it.first, true);
      }
      for (auto& it : other._matrix) {
        unique.insert_or_assign(it.first, true);
      }
      for (auto& it : unique) {
        result.insert(it.first / this->_cols, it.first % this->_cols, (*this)(it.first) - other(it.first));
      }
      return result;
    }

    /// @brief transpose the matrix
    /// @return the transposed matrix
    SparseMatrix operator++();

    /// @brief sum of two matrices
    /// @param other the other matrix
    /// @return the sum of the two matrices
    /// @throw std::runtime_error if the dimensions do not match
    template <typename I, typename U>
    SparseMatrix& operator+=(const SparseMatrix<I, U>& other);

    /// @brief difference of two matrices
    /// @param other the other matrix
    /// @return the difference of the two matrices
    /// @throw std::runtime_error if the dimensions do not match
    template <typename I, typename U>
    SparseMatrix& operator-=(const SparseMatrix<I, U>& other);
  };

  template <typename Index, typename T>
  SparseMatrix<Index, T>::SparseMatrix()
      : _matrix{std::unordered_map<Index, T>()}, _rows{}, _cols{}, _defaultReturn{0} {}

  template <typename Index, typename T>
  SparseMatrix<Index, T>::SparseMatrix(Index rows, Index cols) : _matrix{std::unordered_map<Index, T>()} {
    rows < 0 || cols < 0 ? throw std::invalid_argument("SparseMatrix: rows and cols must be > 0")
                         : _rows = rows,
                           _cols = cols;
    _defaultReturn = 0;
  }

  template <typename Index, typename T>
  SparseMatrix<Index, T>::SparseMatrix(Index index) : _matrix{std::unordered_map<Index, T>()} {
    index < 0 ? throw std::invalid_argument("SparseMatrix: index must be > 0") : _rows = index, _cols = 1;
    _defaultReturn = 0;
  }

  template <typename Index, typename T>
  void SparseMatrix<Index, T>::insert(Index i, Index j, T value) {
    if (i >= _rows || j >= _cols || i < 0 || j < 0) {
      throw std::out_of_range("Index out of range");
    }
    _matrix.emplace(std::make_pair(i * _cols + j, value));
  }

  template <typename Index, typename T>
  void SparseMatrix<Index, T>::insert(Index i, T value) {
    if (i >= _rows * _cols || i < 0) {
      throw std::out_of_range("Index out of range");
    }
    _matrix.emplace(std::make_pair(i, value));
  }

  template <typename Index, typename T>
  void SparseMatrix<Index, T>::insert_or_assign(Index i, Index j, T value) {
    if (i >= _rows || j >= _cols || i < 0 || j < 0) {
      throw std::out_of_range("Index out of range");
    }
    _matrix.insert_or_assign(i * _cols + j, value);
  }

  template <typename Index, typename T>
  void SparseMatrix<Index, T>::insert_or_assign(Index index, T value) {
    if (index < 0 || index > _rows * _cols - 1) {
      throw std::out_of_range("Index out of range");
    }
    _matrix.insert_or_assign(index, value);
  }

  template <typename Index, typename T>
  void SparseMatrix<Index, T>::erase(Index i, Index j) {
    if (i >= _rows || j >= _cols || i < 0 || j < 0) {
      throw std::out_of_range("Index out of range");
    }
    _matrix.find(i * _cols + j) != _matrix.end()
        ? _matrix.erase(i * _cols + j)
        : throw std::runtime_error("SparseMatrix: element not found");
  }

  template <typename Index, typename T>
  void SparseMatrix<Index, T>::eraseRow(Index index) {
    if (index < 0 || index > _rows - 1) {
      throw std::out_of_range("Index out of range");
    }
    for (Index i = 0; i < _cols; ++i) {
      _matrix.erase(index * _cols + i);
    }
    std::unordered_map<Index, T> new_matrix = {};
    for (auto const& [key, value] : _matrix) {
      if (key / _cols < index) {
        new_matrix.emplace(std::make_pair(key, value));
      } else {
        new_matrix.emplace(std::make_pair(key - _cols, value));
      }
    }
    --_rows;
    _matrix = new_matrix;
  }

  template <typename Index, typename T>
  void SparseMatrix<Index, T>::eraseColumn(Index index) {
    if (index < 0 || index > _cols - 1) {
      throw std::out_of_range("Index out of range");
    }
    for (Index i = 0; i < _rows; ++i) {
      _matrix.erase(i * _cols + index);
    }
    std::unordered_map<Index, T> new_matrix = {};
    for (auto const& [key, value] : _matrix) {
      if (key % _cols < index) {
        new_matrix.emplace(std::make_pair(key - key / _cols, value));
      } else {
        new_matrix.emplace(std::make_pair(key / _cols * (_cols - 1) + key % _cols - 1, value));
      }
    }
    --_cols;
    _matrix = new_matrix;
  }

  template <typename Index, typename T>
  void SparseMatrix<Index, T>::clear() {
    _matrix.clear();
    _rows = 0;
    _cols = 0;
  }

  template <typename Index, typename T>
  bool SparseMatrix<Index, T>::contains(Index i, Index j) const {
    if (i >= _rows || j >= _cols || i < 0 || j < 0) {
      throw std::out_of_range("Index out of range");
    }
    return _matrix.contains(i * _cols + j);
  }

  template <typename Index, typename T>
  bool SparseMatrix<Index, T>::contains(Index const index) const {
    if (index < 0 || index > _rows * _cols - 1) {
      throw std::out_of_range("Index out of range");
    }
    return _matrix.contains(index);
  }

  template <typename Index, typename T>
  SparseMatrix<Index, int> SparseMatrix<Index, T>::getDegreeVector() {
    if (_rows != _cols) {
      throw std::runtime_error("SparseMatrix: getDegreeVector only works on square matrices");
    }
    auto degreeVector = SparseMatrix<Index, int>(_rows, 1);
    for (auto& i : _matrix) {
      degreeVector.insert_or_assign(i.first / _cols, 0, degreeVector(i.first / _cols, 0) + 1);
    }
    return degreeVector;
  }

  template <typename Index, typename T>
  SparseMatrix<Index, double> SparseMatrix<Index, T>::getStrengthVector() {
    if (_rows != _cols) {
      throw std::runtime_error("SparseMatrix: getStrengthVector only works on square matrices");
    }
    auto strengthVector = SparseMatrix<Index, double>(_rows, 1);
    for (auto& i : _matrix) {
      strengthVector.insert_or_assign(i.first / _cols, 0, strengthVector(i.first / _cols, 0) + i.second);
    }
    return strengthVector;
  }

  template <typename Index, typename T>
  SparseMatrix<Index, int> SparseMatrix<Index, T>::getLaplacian() {
    if (_rows != _cols) {
      throw std::runtime_error("SparseMatrix: getLaplacian only works on square matrices");
    }
    auto laplacian = SparseMatrix<Index, int>(_rows, _cols);
    for (auto& i : _matrix) {
      laplacian.insert_or_assign(i.first / _cols, i.first % _cols, -1);
    }
    auto degreeVector = this->getDegreeVector();
    for (Index i = 0; i < _rows; ++i) {
      laplacian.insert_or_assign(i, i, degreeVector(i, 0));
    }
    return laplacian;
  }

  template <typename Index, typename T>
  SparseMatrix<Index, T> SparseMatrix<Index, T>::getRow(Index index) const {
    if (index >= _rows || index < 0) {
      throw std::out_of_range("Index out of range");
    }
    SparseMatrix row(1, _cols);
    for (auto& it : _matrix) {
      if (it.first / _cols == index) {
        row.insert(it.first % _cols, it.second);
      }
    }
    return row;
  }

  template <typename Index, typename T>
  SparseMatrix<Index, T> SparseMatrix<Index, T>::getCol(Index index) const {
    if (index >= _cols || index < 0) {
      throw std::out_of_range("Index out of range");
    }
    SparseMatrix col(_rows, 1);
    for (auto& it : _matrix) {
      if (it.first % _cols == index) {
        col.insert(it.first / _cols, it.second);
      }
    }
    return col;
  }

  template <typename Index, typename T>
  SparseMatrix<Index, double> SparseMatrix<Index, T>::getNormRows() const {
    SparseMatrix<Index, double> normRows(_rows, _cols);
    for (Index index = 0; index < _rows; ++index) {
      auto row = this->getRow(index);
      double sum = 0.;
      for (auto& it : row) {
        sum += std::abs(it.second);
      }
      sum < std::numeric_limits<double>::epsilon() ? sum = 1. : sum = sum;
      for (auto& it : row) {
        normRows.insert(it.first + index * _cols, it.second / sum);
      }
    }
    return normRows;
  }

  template <typename Index, typename T>
  SparseMatrix<Index, double> SparseMatrix<Index, T>::getNormCols() const {
    SparseMatrix<Index, double> normCols(_rows, _cols);
    for (Index index = 0; index < _cols; ++index) {
      auto col = this->getCol(index);
      double sum = 0.;
      for (auto& it : col) {
        sum += std::abs(it.second);
      }
      sum < std::numeric_limits<double>::epsilon() ? sum = 1. : sum = sum;
      for (auto& it : col) {
        normCols.insert(it.first * _cols + index, it.second / sum);
      }
    }
    return normCols;
  }

  template <typename Index, typename T>
  Index SparseMatrix<Index, T>::getRowDim() const {
    return this->_rows;
  }

  template <typename Index, typename T>
  Index SparseMatrix<Index, T>::getColDim() const {
    return this->_cols;
  }

  template <typename Index, typename T>
  Index SparseMatrix<Index, T>::size() const {
    return _matrix.size();
  }

  template <typename Index, typename T>
  Index SparseMatrix<Index, T>::max_size() const {
    return this->_rows * this->_cols;
  }

  template <typename Index, typename T>
  void SparseMatrix<Index, T>::symmetrize() {
    *this += this->operator++();
  }

  template <typename Index, typename T>
  typename std::unordered_map<Index, T>::const_iterator SparseMatrix<Index, T>::begin() const {
    return _matrix.begin();
  }

  template <typename Index, typename T>
  typename std::unordered_map<Index, T>::const_iterator SparseMatrix<Index, T>::end() const {
    return _matrix.end();
  }

  template <typename Index, typename T>
  const T& SparseMatrix<Index, T>::operator()(Index i, Index j) const {
    if (i >= _rows || j >= _cols || i < 0 || j < 0) {
      throw std::out_of_range("Index out of range");
    }
    auto const& it = _matrix.find(i * _cols + j);
    return it != _matrix.end() ? it->second : _defaultReturn;
  }

  template <typename Index, typename T>
  T& SparseMatrix<Index, T>::operator()(Index i, Index j) {
    if (i >= _rows || j >= _cols || i < 0 || j < 0) {
      throw std::out_of_range("Index out of range");
    }
    auto const& it = _matrix.find(i * _cols + j);
    return it != _matrix.end() ? it->second : _defaultReturn;
  }

  template <typename Index, typename T>
  const T& SparseMatrix<Index, T>::operator()(Index index) const {
    if (index >= _rows * _cols || index < 0) {
      throw std::out_of_range("Index out of range");
    }
    auto const& it = _matrix.find(index);
    return it != _matrix.end() ? it->second : _defaultReturn;
  }

  template <typename Index, typename T>
  T& SparseMatrix<Index, T>::operator()(Index index) {
    if (index >= _rows * _cols || index < 0) {
      throw std::out_of_range("Index out of range");
    }
    auto const& it = _matrix.find(index);
    return it != _matrix.end() ? it->second : _defaultReturn;
  }

  template <typename Index, typename T>
  SparseMatrix<Index, T> SparseMatrix<Index, T>::operator++() {
    auto transpost = SparseMatrix(this->_cols, this->_rows);
    for (auto& it : _matrix) {
      transpost.insert(it.first % _cols, it.first / _cols, it.second);
    }
    return transpost;
  }

  template <typename Index, typename T>
  template <typename I, typename U>
  SparseMatrix<Index, T>& SparseMatrix<Index, T>::operator+=(const SparseMatrix<I, U>& other) {
    if (this->_rows != other._rows || this->_cols != other._cols) {
      throw std::runtime_error("SparseMatrix: dimensions do not match");
    }
    for (auto& it : other._matrix) {
      this->contains(it.first) ? this->insert_or_assign(it.first, this->operator()(it.first) + it.second)
                               : this->insert(it.first, it.second);
    }
    return *this;
  }

  template <typename Index, typename T>
  template <typename I, typename U>
  SparseMatrix<Index, T>& SparseMatrix<Index, T>::operator-=(const SparseMatrix<I, U>& other) {
    if (this->_rows != other._rows || this->_cols != other._cols) {
      throw std::runtime_error("SparseMatrix: dimensions do not match");
    }
    for (auto& it : other._matrix) {
      this->contains(it.first) ? this->insert_or_assign(it.first, this->operator()(it.first) - it.second)
                               : this->insert(it.first, -it.second);
    }
    return *this;
  }
};  // namespace dmf

#endif
