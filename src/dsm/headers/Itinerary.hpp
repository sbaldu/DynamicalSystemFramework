/// @file       /src/dsm/headers/Itinerary.hpp
/// @brief      Defines the Itinerary class.
///
/// @details    This file contains the definition of the Itinerary class.
///             The Itinerary class represents an itinerary in the network. It is templated
///             by the type of the itinerary's id, which must be an unsigned integral type.
///             An itinerary is defined by its id, its destination and the path to reach it.

#ifndef Itinerary_hpp
#define Itinerary_hpp

#include "SparseMatrix.hpp"

#include <concepts>
#include <utility>
#include <string>

namespace dsm {
  /// @brief The Itinerary class represents an itinerary in the network.
  /// @tparam Id The type of the itinerary's id. It must be an unsigned integral type.
  template <typename Id>
    requires(std::unsigned_integral<Id>)
  class Itinerary {
  private:
    Id m_id;
    SparseMatrix<Id, bool> m_path;
    Id m_destination;

  public:
    Itinerary() = delete;
    /// @brief Construct a new Itinerary object
    /// @param destination The itinerary's destination
    Itinerary(Id id, Id destination);
    /// @brief Construct a new Itinerary<Id>:: Itinerary object
    /// @param destination The itinerary's destination
    /// @param path An adjacency matrix made by a SparseMatrix representing the itinerary's path
    Itinerary(Id id, Id destination, SparseMatrix<Id, bool> path);

    /// @brief Set the itinerary's destination
    /// @param destination The itinerary's destination
    void setDestination(Id destination);
    /// @brief Set the itinerary's path
    /// @param path An adjacency matrix made by a SparseMatrix representing the itinerary's path
    /// @throw std::invalid_argument, if the itinerary's source or destination is not in the path's
    void setPath(SparseMatrix<Id, bool> path);

    /// @brief Get the itinerary's id
    /// @return Id, The itinerary's id
    Id id() const noexcept { return m_id; }
    /// @brief Get the itinerary's destination
    /// @return Id, The itinerary's destination
    Id destination() const noexcept { return m_destination; }
    /// @brief Get the itinerary's path
    /// @return SparseMatrix<Id, bool>, An adjacency matrix made by a SparseMatrix representing the
    /// itinerary's path
    const SparseMatrix<Id, bool>& path() const noexcept { return m_path; }
  };

  template <typename Id>
    requires(std::unsigned_integral<Id>)
  Itinerary<Id>::Itinerary(Id id, Id destination)
      : m_id{id}, m_destination{destination} {}
  template <typename Id>
    requires(std::unsigned_integral<Id>)
  Itinerary<Id>::Itinerary(Id id, Id destination, SparseMatrix<Id, bool> path)
      : m_id{id}, m_path{std::move(path)}, m_destination{destination} {}

  template <typename Id>
    requires(std::unsigned_integral<Id>)
  void Itinerary<Id>::setDestination(Id destination) {
    m_destination = destination;
    this->m_path.clear();
  }
  template <typename Id>
    requires(std::unsigned_integral<Id>)
  void Itinerary<Id>::setPath(SparseMatrix<Id, bool> path) {
    if (path.getRowDim() != path.getColDim()) {
      throw std::invalid_argument(
          buildLog("The path's row and column dimensions must be equal."));
    }
    if (path.getRowDim() < m_destination) {
      throw std::invalid_argument(
          buildLog("The path's row and column dimensions must be greater than the "
                   "itinerary's destination."));
    }
    m_path = std::move(path);
  }
};  // namespace dsm

#endif
