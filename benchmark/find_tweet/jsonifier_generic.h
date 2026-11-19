#pragma once

#ifdef SIMDJSON_COMPETITION_JSONIFIER

#include "find_tweet.h"

namespace find_tweet {

namespace jsonifier_api {
using document = ::jsonifier::generic::generic_document<false>;
using error_code = ::jsonifier::generic::parse_statuses;
using object = ::jsonifier::generic::generic_object<false>;
using array = ::jsonifier::generic::generic_array<false>;
using field = ::jsonifier::generic::generic_field<false>;
using value = ::jsonifier::generic::generic_value<false>;
}

struct jsonifier_generic {
  using StringType=std::string_view;

  ::jsonifier::jsonifier_core<> parser{};

  bool run(simdjson::padded_string &json, uint64_t find_id, std::string_view &result) {
    jsonifier_api::document doc = parser.iterate(json);
    jsonifier_api::array statuses = doc.findField("statuses").getArray();
    if (statuses.error() != jsonifier_api::error_code::success) { return false; }
    for (jsonifier_api::value element : statuses) {
      jsonifier_api::object tweet = element.getObject();
      uint64_t id{};
      if (tweet.findField("id").getUint64(id) != jsonifier_api::error_code::success) { return false; }
      if (id == find_id) {
        return tweet.findField("text").getString(result) == jsonifier_api::error_code::success;
      }
    }
    return false;
  }
};

BENCHMARK_TEMPLATE(find_tweet, jsonifier_generic)->UseManualTime();

} // namespace find_tweet

#endif // SIMDJSON_COMPETITION_JSONIFIER
