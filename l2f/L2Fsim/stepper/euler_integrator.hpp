#ifndef L2FSIM_EULER_INTEGRATOR_HPP_
#define L2FSIM_EULER_INTEGRATOR_HPP_

#include <L2Fsim/stepper/stepper.hpp>
#include <L2Fsim/utils/utils.hpp>
#include <cstdio>
#include <cstdlib>

namespace L2Fsim {

class euler_integrator : public stepper {
public:
    /**
     * Attributes
     * @param {double} dt; width of the sub integration step
     * @param {std::string} state_log_filename; file name for log
     */
    double dt;
    std::string state_log_path = "data/state.dat";
    std::string wind_log_path = "data/wind.dat";

	/** Constructor */
	euler_integrator(double _dt=.001) : dt(_dt) {}

    /**
     * Euler update operator
     * Modifies the static variables of the state according to the corresponding simulation configuration
     * @warning The dynamic part of the state is set to the value used for the update (cf Euler, RK4, etc.)
     * @param {aircraft &} ac; aircraft
     * @param {flight_zone &} fz; flight zone
     * @param {const double} current_time; current time
     * @param {const double} dt; integration step
     */
    /*
    void euler_update(
        aircraft &ac,
        flight_zone &fz,
        const double current_time,
        const double dt)
    {
        ac.update_state_dynamic(fz,current_time,ac.get_state());
        ac.get_state().apply_dynamic(dt);
    }
    */

    /**
     * Transition function
     * @note Perform a transition given: an aircraft model with a correct state and command; an atmospheric model; the current time; the time-step-width and the sub-time-step-width
     * @note static method for use within an external simulator
     * @param {aircraft &} ac; aircraft model
     * @param {flight_zone &} fz; atmosphere model
     * @param {double &} current_time; current time
     * @param {const double &} time_step_width; time-step-width
     * @param {const double &} dt; sub-time-step-width
     */
    static void transition_function(
        aircraft &ac,
        flight_zone &fz,
        double &current_time,
        const double &time_step_width,
        const double &dt)
    {
        ac.apply_command();
        for(int n=0; n<time_step_width/dt; ++n) {
            //euler_update(ac,fz,current_time,dt);
            ac.update_state_dynamic(fz,current_time,ac.get_state());
            ac.get_state().apply_dynamic(dt);
            current_time += dt;
            ac.get_state().update_time(current_time);
        }
    }

    /**
     * Stepping operator
     * @param {flight_zone &} fz; flight zone
     * @param {aircraft &} ac; aircraft
     * @param {pilot &} pl; pilot
     * @param {double &} current_time; current time
     * @param {const double &} time_step_width; period of time during which we perform integration
     */
    void operator()(
        flight_zone &fz,
        aircraft &ac,
        pilot &pl,
        double &current_time,
        const double &time_step_width) override
    {
        /// 1. Apply the policy and store the command into command attribute of aircraft
        if (ac.get_distance_to_center() > 1200.) {
            pl.out_of_range(ac.get_state(),ac.get_command());
        } else {
            pl(ac.get_state(),ac.get_command());
        }

        /// 2. Save the data
        /// @note output files should be cleared first
        save_vector(ac.get_state().get_save(),state_log_path,std::ofstream::app);
        std::vector<double> w;
        fz.wind(ac.get_state().getx(),
            ac.get_state().gety(),
            ac.get_state().getz(),
            ac.get_state().gett(),w);
        save_vector(w,wind_log_path,std::ofstream::app);

        /// 3. Apply the transition with Euler method
        transition_function(ac,fz,current_time,time_step_width,dt);

        /// 4. Check aircraft's configuration validity
        ac.is_in_model();
    }
};

}

#endif
