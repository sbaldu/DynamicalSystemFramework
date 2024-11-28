
#include "Itinerary.hpp"

namespace dsm {
  Itinerary::Itinerary(Id id, Id destination) : m_id{id}, m_destination{destination} {}

  Itinerary::Itinerary(Id id, Id destination, SparseMatrix<bool> path)
      : m_id{id}, m_path{std::move(path)}, m_destination{destination} {}

  void Itinerary::setDestination(Id destination) {
    m_destination = destination;
    m_path.clear();
  }

  void Itinerary::setPath(SparseMatrix<bool> path) {
    if (path.getRowDim() != path.getColDim()) {
      pConsoleLogger->critical(
          "The path's row ({}) and column ({}) dimensions must be equal.",
          path.getRowDim(),
          path.getColDim());
      std::abort();
    }
    if (path.getRowDim() < m_destination) {
      pConsoleLogger->critical(
          "The path's row ({}) and column ({}) dimensions must be greater than the "
          "itinerary's destination ({}).",
          path.getRowDim(),
          path.getColDim(),
          m_destination);
      std::abort();
    }
    m_path = std::move(path);
  }

};  // namespace dsm
