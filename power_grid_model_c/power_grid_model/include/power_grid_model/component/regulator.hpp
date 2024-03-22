// SPDX-FileCopyrightText: Contributors to the Power Grid Model project <powergridmodel@lfenergy.org>
//
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "base.hpp"

#include "../auxiliary/input.hpp"

namespace power_grid_model {

class Regulator : public Base {
  public:
    static constexpr char const* name = "regulator";

    ID regulated_object() const { return regulated_object_; };

    // regulator always energized
    bool energized(bool /* is_connected_to_source */) const final { return true; }
    ComponentType math_model_type() const final { return ComponentType::regulator; }
    bool status() const { return status_; }

    void set_status(bool status) { status_ = status; }

    auto inverse(std::convertible_to<RegulatorUpdate> auto update_data) const {
        assert(update_data.id == id());
        set_if_not_nan(update_data.status, static_cast<IntS>(status_));
        return update_data;
    }

  protected:
    // constructor
    explicit Regulator(RegulatorInput const& regulator_input)
        : Base{regulator_input},
          regulated_object_{regulator_input.regulated_object},
          status_{regulator_input.status != 0} {}

    Regulator(Regulator const&) = default;
    Regulator(Regulator&&) = default;
    Regulator& operator=(Regulator const&) = default;
    Regulator& operator=(Regulator&&) = default;

  public:
    ~Regulator() override = default;

  private:
    ID regulated_object_;
    bool status_;
};

} // namespace power_grid_model