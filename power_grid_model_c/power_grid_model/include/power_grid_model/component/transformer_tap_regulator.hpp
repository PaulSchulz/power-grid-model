// SPDX-FileCopyrightText: Contributors to the Power Grid Model project <powergridmodel@lfenergy.org>
//
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "base.hpp"
#include "regulator.hpp"

#include "../auxiliary/input.hpp"
#include "../auxiliary/output.hpp"
#include "../auxiliary/update.hpp"
#include "../calculation_parameters.hpp"
#include "../common/common.hpp"

namespace power_grid_model {

class TransformerTapRegulator : public Regulator {
  public:
    using InputType = TransformerTapRegulatorInput;
    using UpdateType = TransformerTapRegulatorUpdate;
    static constexpr char const* name = "transformer_tap_regulator";

    explicit TransformerTapRegulator(TransformerTapRegulatorInput const& transformer_tap_regulator_input,
                                     double u_rated)
        : Regulator{transformer_tap_regulator_input},
          control_side_{transformer_tap_regulator_input.control_side},
          u_rated_{u_rated},
          u_set_{transformer_tap_regulator_input.u_set},
          u_band_{transformer_tap_regulator_input.u_band},
          line_drop_compensation_r_{transformer_tap_regulator_input.line_drop_compensation_r},
          line_drop_compensation_x_{transformer_tap_regulator_input.line_drop_compensation_x} {}

    // update for transformer tap regulator, hide default update for branch
    void update(TransformerTapRegulatorUpdate const& update_data) {
        assert(update_data.id == id());
        set_status(update_data.status);
        u_set_ = update_data.u_set;
        u_band_ = update_data.u_band;
        line_drop_compensation_r_ = update_data.line_drop_compensation_r;
        line_drop_compensation_x_ = update_data.line_drop_compensation_x;
    }

    TransformerTapRegulatorUpdate inverse(TransformerTapRegulatorUpdate update_data) const {
        assert(update_data.id == id());

        update_data = Regulator::inverse(update_data);
        set_if_not_nan(update_data.u_set, u_set_);
        set_if_not_nan(update_data.u_band, u_band_);
        set_if_not_nan(update_data.line_drop_compensation_r, line_drop_compensation_r_);
        set_if_not_nan(update_data.line_drop_compensation_x, line_drop_compensation_x_);

        return update_data;
    }

    TransformerTapRegulatorOutput get_output(IntS const& tap_pos) const {
        TransformerTapRegulatorOutput output{};
        output.id = id();
        output.energized = energized(true);
        output.tap_pos = tap_pos;
        return output;
    }

    template <symmetry_tag sym> TransformerTapRegulatorCalcParam calc_param() const {
        TransformerTapRegulatorCalcParam param{};
        param.u_set = u_set_ / u_rated_;
        param.u_band = u_band_ / u_rated_;
        double z_base = u_rated_ * u_rated_ / base_power<sym>;
        DoubleComplex z_compensation{line_drop_compensation_r_, line_drop_compensation_x_};
        param.z_compensation = z_compensation / z_base;
        param.status = Regulator::status();
        return param;
    }

    // getter
    ControlSide control_side() const { return control_side_; }

  private:
    // transformer tap regulator parameters
    ControlSide control_side_;
    double u_rated_;
    double u_set_;
    double u_band_;
    double line_drop_compensation_r_;
    double line_drop_compensation_x_;
};

} // namespace power_grid_model
