#pragma once

#ifdef SIMDJSON_COMPETITION_JSONIFIER

#include "partial_tweets.h"

namespace partial_tweets {

namespace jsonifier_api = ::jsonifier::generic;

struct jsonifier_generic {
  using StringType=std::string_view;

  ::jsonifier::jsonifier_core<> parser{};

  simdjson_inline bool nullable_int(jsonifier_api::value value, uint64_t &out) {
    if (value.isNull()) { out = 0; return true; }
    return value.getUint64(out) == jsonifier_api::error_code::success;
  }

 bool run(simdjson::padded_string &json,
           std::vector<tweet<std::string_view>> &result) {
    using error_type = std::underlying_type_t<jsonifier_api::error_code>;
    jsonifier_api::document doc = parser.iterate(json);
    jsonifier_api::array statuses = doc.findField("statuses").getArray();
    error_type errors = static_cast<error_type>(statuses.error());
    for (jsonifier_api::value element : statuses) {
      jsonifier_api::object tweet = element.getObject();
      std::string_view created_at, text, screen_name;
      uint64_t id{}, in_reply_to_status_id{}, user_id{}, retweet_count{},
          favorite_count{};
      errors |= static_cast<error_type>(
          tweet.findField("created_at").getString(created_at));
      errors |= static_cast<error_type>(tweet.findField("id").getUint64(id));
      errors |=
          static_cast<error_type>(tweet.findField("text").getString(text));
      errors |= static_cast<error_type>(!nullable_int(
          tweet.findField("in_reply_to_status_id"), in_reply_to_status_id));
      jsonifier_api::object user = tweet.findField("user").getObject();
      errors |=
          static_cast<error_type>(user.findField("id").getUint64(user_id));
      errors |= static_cast<error_type>(
          user.findField("screen_name").getString(screen_name));
      errors |= static_cast<error_type>(
          tweet.findField("retweet_count").getUint64(retweet_count));
      errors |= static_cast<error_type>(
          tweet.findField("favorite_count").getUint64(favorite_count));
      result.emplace_back(
          partial_tweets::tweet<std::string_view>{created_at,
                                                  id,
                                                  text,
                                                  in_reply_to_status_id,
                                                  {user_id, screen_name},
                                                  retweet_count,
                                                  favorite_count});
    }
    return errors ==
           static_cast<error_type>(jsonifier_api::error_code::success);
  }
};

BENCHMARK_TEMPLATE(partial_tweets, jsonifier_generic)->UseManualTime();

} // namespace partial_tweets

#endif // SIMDJSON_COMPETITION_JSONIFIER
