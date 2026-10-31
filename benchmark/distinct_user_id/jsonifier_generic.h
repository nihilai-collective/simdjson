#pragma once

#ifdef SIMDJSON_COMPETITION_JSONIFIER

#include "distinct_user_id.h"

namespace distinct_user_id {

namespace jsonifier_api = ::jsonifier::generic;

struct jsonifier_generic {
  ::jsonifier::jsonifier_core<> parser{};

  bool run(simdjson::padded_string &json, std::vector<uint64_t> &result) {
    using error_type = std::underlying_type_t<jsonifier_api::error_code>;
    jsonifier_api::document doc = parser.iterate(json);
    jsonifier_api::array statuses = doc.findField("statuses").getArray();
    error_type errors = static_cast<error_type>(statuses.error());
    for (jsonifier_api::value element : statuses) {
      jsonifier_api::object tweet = element.getObject();
      uint64_t id{};
      errors |= static_cast<error_type>(tweet.findField("user").findField("id").getUint64(id));
      result.push_back(id);
      jsonifier_api::value retweet = tweet.findField("retweeted_status");
      if (retweet.error() == jsonifier_api::error_code::success) {
        errors |= static_cast<error_type>(retweet.findField("user").findField("id").getUint64(id));
        result.push_back(id);
      } else {
        errors |= static_cast<error_type>(retweet.error() != jsonifier_api::error_code::no_such_field);
      }
    }
    return errors == static_cast<error_type>(jsonifier_api::error_code::success);
  }
};

BENCHMARK_TEMPLATE(distinct_user_id, jsonifier_generic)->UseManualTime();

} // namespace distinct_user_id

#endif // SIMDJSON_COMPETITION_JSONIFIER
