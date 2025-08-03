#include "error.h"
#include <fmt/core.h>

Error::Builder::Builder(Category category, int code) noexcept
    : category_{category}, code_{code} {
}

Error
Error::Builder::build() && noexcept {
  return Error{std::move(*this)};
}

Error::Error(Builder&& builder) noexcept
    : category{builder.category_}, code{builder.code_},
      message{std::move(builder.message_)}, details{
                                                std::move(builder.details_)} {
}

std::string
Error::to_string() const noexcept {
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

Error::Builder
err(int errnum) noexcept {
  return Error::Builder{Error::Category::errnum, errnum};
}

Error::Builder
err(KeroErrorCode code) noexcept {
  return Error::Builder{Error::Category::kero, static_cast<int>(code)};
}
