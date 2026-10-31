#pragma once

#ifdef SIMDJSON_COMPETITION_JSONIFIER

#include "large_random.h"

namespace large_random {

namespace jsonifier_api = ::jsonifier::generic;

struct jsonifier_generic {
  static constexpr diff_flags DiffFlags = diff_flags::NONE;

  ::jsonifier::jsonifier_core<> parser{};

  bool run(simdjson::padded_string &json, std::vector<point> &result) {
    using error_type = std::underlying_type_t<jsonifier_api::error_code>;
    jsonifier_api::document doc = parser.iterate(json);
    jsonifier_api::array coordinates = doc.getArray();
    error_type errors = static_cast<error_type>(coordinates.error());
    for (jsonifier_api::value element : coordinates) {
      jsonifier_api::object coordinate = element.getObject();
      double x{}, y{}, z{};
      errors |= static_cast<error_type>(coordinate.findField("x").getDouble(x));
      errors |= static_cast<error_type>(coordinate.findField("y").getDouble(y));
      errors |= static_cast<error_type>(coordinate.findField("z").getDouble(z));
      result.emplace_back(json_benchmark::point{x, y, z});
    }
    return errors == static_cast<error_type>(jsonifier_api::error_code::success);
  }
};

BENCHMARK_TEMPLATE(large_random, jsonifier_generic)->UseManualTime();

} // namespace large_random

#endif // SIMDJSON_COMPETITION_JSONIFIER
