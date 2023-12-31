/// @file       /src/dsm/headers/Itinerary.hpp
/// @brief      Defines the Itinerary class.
///
/// @details    This file contains the definition of the Itinerary class.
///             The Itinerary class represents an itinerary in the network. It is templated
///             by the type of the itinerary's id, which must be an unsigned integral type.

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
    requires std::unsigned_integral<Id>
  class Itinerary {
  private:
    Id m_id;
    SparseMatrix<Id, bool> m_path;
    std::pair<Id, Id> m_trip;

  public:
    Itinerary() = delete;
    /// @brief Construct a new Itinerary object
    /// @param source The itinerary's source
    /// @param destination The itinerary's destination
    Itinerary(Id id, Id source, Id destination);
    /// @brief Construct a new Itinerary object
    /// @param trip An std::pair containing the itinerary's source and destination
    explicit Itinerary(Id id, std::pair<Id, Id> trip)
        : m_id{id}, m_trip{std::move(trip)} {}
    /// @brief Construct a new Itinerary<Id>:: Itinerary object
    /// @param source The itinerary's source
    /// @param destination The itinerary's destination
    /// @param path An adjacency matrix made by a SparseMatrix representing the itinerary's path
    Itinerary(Id id, Id source, Id destination, SparseMatrix<Id, bool> path);
    /// @brief Construct a new Itinerary<Id>:: Itinerary object
    /// @param trip An std::pair containing the itinerary's source and destination
    /// @param path An adjacency matrix made by a SparseMatrix representing the itinerary's path
    Itinerary(Id id, std::pair<Id, Id> trip, SparseMatrix<Id, bool> path);

    /// @brief Set the itinerary's source
    /// @param source The itinerary's source
    void setSource(Id source);
    /// @brief Set the itinerary's destination
    /// @param destination The itinerary's destination
    void setDestination(Id destination);
    /// @brief Set the itinerary's path
    /// @param path An adjacency matrix made by a SparseMatrix representing the itinerary's path
    /// @throw std::invalid_argument, if the itinerary's source or destination is not in the path's
    void setPath(SparseMatrix<Id, bool> path);

    /// @brief Get the itinerary's id
    /// @return Id, The itinerary's id
    Id id() const;
    /// @brief Get the itinerary's source
    /// @return Id, The itinerary's source
    Id source() const;
    /// @brief Get the itinerary's destination
    /// @return Id, The itinerary's destination
    Id destination() const;
    /// @brief Get the itinerary's trip
    /// @return std::pair<Id, Id>, An std::pair containing the itinerary's source and destination
    const std::pair<Id, Id>& trip() const;
    /// @brief Get the itinerary's path
    /// @return SparseMatrix<Id, bool>, An adjacency matrix made by a SparseMatrix representing the
    /// itinerary's path
    const SparseMatrix<Id, bool>& path() const;
  };

  template <typename Id>
    requires std::unsigned_integral<Id>
  Itinerary<Id>::Itinerary(Id id, Id source, Id destination)
      : m_id{id}, m_trip{std::make_pair(source, destination)} {}
  template <typename Id>
    requires std::unsigned_integral<Id>
  Itinerary<Id>::Itinerary(Id id, Id source, Id destination, SparseMatrix<Id, bool> path)
      : m_id{id}, m_path{std::move(path)}, m_trip{std::make_pair(source, destination)} {}
  template <typename Id>
    requires std::unsigned_integral<Id>
  Itinerary<Id>::Itinerary(Id id, std::pair<Id, Id> trip, SparseMatrix<Id, bool> path)
      : m_id{id}, m_path{std::move(path)}, m_trip{std::move(trip)} {}

  template <typename Id>
    requires std::unsigned_integral<Id>
  void Itinerary<Id>::setSource(Id source) {
    m_trip.first = source;
  }
  template <typename Id>
    requires std::unsigned_integral<Id>
  void Itinerary<Id>::setDestination(Id destination) {
    m_trip.second = destination;
  }
  template <typename Id>
    requires std::unsigned_integral<Id>
  void Itinerary<Id>::setPath(SparseMatrix<Id, bool> path) {
    // if (!(m_trip.first < path.size()) || !(m_trip.second < path.size())) {
    //   std::string errorMsg{"Error at line " + std::to_string(__LINE__) + " in file " + __FILE__ + ": " +
    //                        "The itinerary's source or destination is not in the path's range"};
    //   throw std::invalid_argument(errorMsg);
    // }
    m_path = std::move(path);
  }

  template <typename Id>
    requires std::unsigned_integral<Id>
  Id Itinerary<Id>::id() const {
    return m_id;
  }
  template <typename Id>
    requires std::unsigned_integral<Id>
  Id Itinerary<Id>::source() const {
    return m_trip.first;
  }
  template <typename Id>
    requires std::unsigned_integral<Id>
  Id Itinerary<Id>::destination() const {
    return m_trip.second;
  }
  template <typename Id>
    requires std::unsigned_integral<Id>
  const std::pair<Id, Id>& Itinerary<Id>::trip() const {
    return m_trip;
  }
  template <typename Id>
    requires std::unsigned_integral<Id>
  const SparseMatrix<Id, bool>& Itinerary<Id>::path() const {
    return m_path;
  }

};  // namespace dsm

#endif
