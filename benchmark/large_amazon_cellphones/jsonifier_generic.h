#pragma once

#ifdef SIMDJSON_COMPETITION_JSONIFIER

#include "large_amazon_cellphones.h"

namespace large_amazon_cellphones {

namespace jsonifier_api = ::jsonifier::generic;

struct jsonifier_generic {
  using StringType = std::string;

  ::jsonifier::jsonifier_core<> parser{};

  bool run(simdjson::padded_string &json, std::map<StringType, brand> &result) {
    using error_type = std::underlying_type_t<jsonifier_api::error_code>;
    auto stream = parser.iterateMany(json);
    auto i = stream.begin();
    ++i;
    error_type errors{};
    for (; i != stream.end(); ++i) {
      auto doc = *i;
      auto fields = doc.getArray();
      errors |= static_cast<error_type>(fields.error());
      size_t index{0};
      std::string_view name;
      double rating{};
      uint64_t reviews{};
      for (auto value : fields) {
        switch (index) {
        case 1:
          errors |= static_cast<error_type>(value.getString(name));
          break;
        case 5:
          errors |= static_cast<error_type>(value.getDouble(rating));
          break;
        case 7:
          errors |= static_cast<error_type>(value.getUint64(reviews));
          break;
        default:
          break;
        }
        index++;
      }

      StringType copy(name);
      auto x = result.find(copy);
      if (x == result.end()) {
        result.emplace(copy, large_amazon_cellphones::brand{
          rating * reviews,
          reviews
        });
      } else {
        x->second.cumulative_rating += rating * reviews;
        x->second.reviews_count += reviews;
      }
    }

    errors |= static_cast<error_type>(stream.error());
    return errors == static_cast<error_type>(jsonifier_api::error_code::success);
  }
};

BENCHMARK_TEMPLATE(large_amazon_cellphones, jsonifier_generic)->UseManualTime();

} // namespace large_amazon_cellphones

#endif // SIMDJSON_COMPETITION_JSONIFIER
