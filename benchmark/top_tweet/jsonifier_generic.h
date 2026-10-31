#pragma once

#ifdef SIMDJSON_COMPETITION_JSONIFIER

#include "top_tweet.h"

namespace top_tweet {

namespace jsonifier_api = ::jsonifier::generic;

struct jsonifier_generic {
  using StringType=std::string_view;

  ::jsonifier::jsonifier_core<> parser{};

  bool run(simdjson::padded_string &json, int64_t max_retweet_count, top_tweet_result<StringType> &result) {
    using error_type = std::underlying_type_t<jsonifier_api::error_code>;
    result.retweet_count = -1;
    jsonifier_api::value screen_name, text;

    jsonifier_api::document doc = parser.iterate(json);
    jsonifier_api::array statuses = doc.findField("statuses").getArray();
    error_type errors = static_cast<error_type>(statuses.error());
    for (jsonifier_api::value element : statuses) {
      jsonifier_api::object tweet = element.getObject();
      jsonifier_api::value tweet_text = tweet.findField("text");
      jsonifier_api::value tweet_screen_name = tweet.findField("user").findField("screen_name");
      int64_t retweet_count{};
      errors |= static_cast<error_type>(tweet.findField("retweet_count").getInt64(retweet_count));
      if (retweet_count <= max_retweet_count && retweet_count >= result.retweet_count) {
        result.retweet_count = retweet_count;
        text = tweet_text;
        screen_name = tweet_screen_name;
      }
    }

    if (result.retweet_count == -1) { return false; }
    errors |= static_cast<error_type>(screen_name.getString(result.screen_name));
    errors |= static_cast<error_type>(text.getString(result.text));
    return errors == static_cast<error_type>(jsonifier_api::error_code::success);
  }
};

BENCHMARK_TEMPLATE(top_tweet, jsonifier_generic)->UseManualTime();

} // namespace top_tweet

#endif // SIMDJSON_COMPETITION_JSONIFIER
