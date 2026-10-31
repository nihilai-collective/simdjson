#pragma once

#ifdef SIMDJSON_COMPETITION_JSONIFIER

#include "find_tweet.h"

namespace find_tweet {

namespace jsonifier_api = ::jsonifier::generic;

struct jsonifier_generic {
  using StringType=std::string_view;

  ::jsonifier::jsonifier_core<> parser{};

  bool run(simdjson::padded_string &json, uint64_t find_id, std::string_view &result) {
    using error_type = std::underlying_type_t<jsonifier_api::error_code>;
    jsonifier_api::document doc = parser.iterate(json);
    jsonifier_api::array statuses = doc.findField("statuses").getArray();
    error_type errors = static_cast<error_type>(statuses.error());
    for (jsonifier_api::value element : statuses) {
      jsonifier_api::object tweet = element.getObject();
      uint64_t id{};
      errors |= static_cast<error_type>(tweet.findField("id").getUint64(id));
      if (id == find_id) {
        errors |= static_cast<error_type>(tweet.findField("text").getString(result));
        return errors == static_cast<error_type>(jsonifier_api::error_code::success);
      }
    }
    return false;
  }
};

BENCHMARK_TEMPLATE(find_tweet, jsonifier_generic)->UseManualTime();

} // namespace find_tweet

#endif // SIMDJSON_COMPETITION_JSONIFIER
