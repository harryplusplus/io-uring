#ifndef KERO_WITH_H_
#define KERO_WITH_H_

template <typename Resource, typename Result, typename Acquire,
          typename Release, typename On>
Result With(Acquire&& acquire, Release&& release, On&& on) noexcept {
  auto res = std::forward<Acquire>(acquire)();
  const Result result = std::forward<On>(on)(res.value());
  std::forward<Release>(release)(std::move(resource));
  return result;
}

#endif  // KERO_WITH_H_