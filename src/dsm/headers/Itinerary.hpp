/// @file       /src/dsm/headers/Itinerary.hpp
/// @brief      Defines the Itinerary class.
///
/// @details    This file contains the definition of the Itinerary class.
///             The Itinerary class represents an itinerary in the network. It is templated
///             by the type of the itinerary's id, which must be an unsigned integral type.
///             An itinerary is defined by its id, its destination and the path to reach it.

#pragma once

#include "SparseMatrix.hpp"
#include "../utility/Typedef.hpp"

#include <concepts>
#include <utility>
#include <string>
#include <format>

namespace dsm {
  /// @brief The Itinerary class represents an itinerary in the network.
  /// @tparam Id The type of the itinerary's id. It must be an unsigned integral type.
  class Itinerary {
  private:
    Id m_id;
    SparseMatrix<bool> m_path;
    Id m_destination;

  public:
    /// @brief Construct a new Itinerary object
    /// @param destination The itinerary's destination
    inline Itinerary(Id id, Id destination);
    /// @brief Construct a new Itinerary object
    /// @param destination The itinerary's destination
    /// @param path An adjacency matrix made by a SparseMatrix representing the itinerary's path
    inline Itinerary(Id id, Id destination, SparseMatrix<bool> path);

    /// @brief Set the itinerary's destination
    /// @param destination The itinerary's destination
    inline void setDestination(Id destination);
    /// @brief Set the itinerary's path
    /// @param path An adjacency matrix made by a SparseMatrix representing the itinerary's path
    /// @throw std::invalid_argument, if the itinerary's source or destination is not in the path's
    inline void setPath(SparseMatrix<bool> path);

    /// @brief Get the itinerary's id
    /// @return Id, The itinerary's id
    inline Id id() const { return m_id; }
    /// @brief Get the itinerary's destination
    /// @return Id, The itinerary's destination
    inline Id destination() const { return m_destination; }
    /// @brief Get the itinerary's path
    /// @return SparseMatrix<Id, bool>, An adjacency matrix made by a SparseMatrix representing the
    /// itinerary's path
    inline const SparseMatrix<bool>& path() const { return m_path; }
  };

  Itinerary::Itinerary(Id id, Id destination)
      : m_id{id}, m_destination{destination} {}

  Itinerary::Itinerary(Id id, Id destination, SparseMatrix<bool> path)
      : m_id{id}, m_path{std::move(path)}, m_destination{destination} {}

  void Itinerary::setDestination(Id destination) {
    m_destination = destination;
    this->m_path.clear();
  }

  void Itinerary::setPath(SparseMatrix<bool> path) {
    if (path.getRowDim() != path.getColDim()) {
      throw std::invalid_argument(buildLog(
          std::format("The path's row ({}) and column ({}) dimensions must be equal.",
                      path.getRowDim(),
                      path.getColDim())));
    }
    if (path.getRowDim() < m_destination) {
      throw std::invalid_argument(
          buildLog(std::format("The path's row ({}) and column ({}) dimensions must be "
                               "greater than the itinerary's destination ({}).",
                               path.getRowDim(),
                               path.getColDim(),
                               m_destination)));
    }
    m_path = std::move(path);
  }
};  // namespace dsm
