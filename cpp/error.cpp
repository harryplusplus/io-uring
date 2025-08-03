#include "error.h"

const char*
kero::Category::name() const noexcept {
  return "kero";
}

std::string
kero::Category::message(int condition) const noexcept {
  if (!is_errc(condition))
    return fmt::format("Unknown error {}", condition);
  return std::to_string(static_cast<Errc>(condition));
}

const kero::Category&
kero::category() noexcept {
  static Category instance;
  return instance;
}

kero::Error::Builder::Builder(std::error_code&& ec) noexcept
    : error_code_{std::move(ec)} {
}

kero::Error
kero::Error::Builder::build() && noexcept {
  return Error{std::move(*this)};
}

kero::Error::Error(Builder&& builder) noexcept
    : error_code_{std::move(builder.error_code_)},
      reason_{std::move(builder.reason_)}, details_{
                                               std::move(builder.details_)} {
}

std::string
std::to_string(const kero::Error& err) noexcept {
  std::string ret = fmt::format("Error{{category:{},code:{}",
                                ::to_string(category), std::to_string(code));

  switch (category) {
  case Category::errnum: {
    {
      const char* name = strerrorname_np(code);
      if (name)
        ret += fmt::format(",name:{}", name);
    }
    {
      const char* desc = strerrordesc_np(code);
      if (desc)
        ret += fmt::format(",desc:{}", desc);
    }
  } break;
  case Category::kero: {
    if (is_kero_error_code(code)) {
      auto name = to_name(static_cast<KeroErrorCode>(code));
      if (!name.empty())
        ret += fmt::format(",name:{}", name);
    }
  } break;
  }

  if (!message.empty())
    ret += fmt::format(",message:{}", message);

  for (auto it = details.begin(); it != details.end(); it++) {
    auto&& [key, val] = *it;
    ret += fmt::format(",{}:{}", key, val);
  }

  ret += "}";
  return ret;
}
