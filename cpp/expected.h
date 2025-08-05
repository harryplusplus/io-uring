#ifndef KERO_EXPECTED_H_
#define KERO_EXPECTED_H_

#include <tl/expected.hpp>

namespace kero {

template <typename T, typename E>
using expected = tl::expected<T, E>;

template <typename E>
using unexpected = tl::unexpected<E>;

}  // namespace kero

#endif  // KERO_EXPECTED_H_