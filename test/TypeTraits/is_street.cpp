#include <cassert>
#include <cstdint>
#include <memory>

#include "is_street.hpp"

using dsm::is_street;
using dsm::is_street_v;
using dsm::Street;

// check the type trait
static_assert(is_street<Street<uint8_t, uint8_t>>::value);
static_assert(is_street<Street<uint16_t, uint16_t>>::value);
static_assert(is_street<Street<uint32_t, uint32_t>>::value);
static_assert(!is_street<int>::value);
static_assert(!is_street<unsigned int>::value);
static_assert(!is_street<std::string>::value);
static_assert(is_street<std::shared_ptr<Street<uint8_t, uint8_t>>>::value);
static_assert(is_street<std::shared_ptr<Street<uint16_t, uint16_t>>>::value);
static_assert(is_street<std::shared_ptr<Street<uint32_t, uint32_t>>>::value);
static_assert(!is_street<std::shared_ptr<int>>::value);
static_assert(!is_street<std::shared_ptr<unsigned int>>::value);
static_assert(!is_street<std::shared_ptr<std::string>>::value);

// check the template variable
static_assert(is_street_v<Street<uint8_t, uint8_t>>);
static_assert(is_street_v<Street<uint16_t, uint16_t>>);
static_assert(is_street_v<Street<uint32_t, uint32_t>>);
static_assert(!is_street_v<int>);
static_assert(!is_street_v<unsigned int>);
static_assert(!is_street_v<std::string>);
static_assert(is_street_v<std::shared_ptr<Street<uint8_t, uint8_t>>>);
static_assert(is_street_v<std::shared_ptr<Street<uint16_t, uint16_t>>>);
static_assert(is_street_v<std::shared_ptr<Street<uint32_t, uint32_t>>>);
static_assert(!is_street_v<std::shared_ptr<int>>);
static_assert(!is_street_v<std::shared_ptr<unsigned int>>);
static_assert(!is_street_v<std::shared_ptr<std::string>>);

int main() {}
