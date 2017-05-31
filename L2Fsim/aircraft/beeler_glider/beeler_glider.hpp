#ifndef L2FSIM_BEELER_GLIDER_HPP_
#define L2FSIM_BEELER_GLIDER_HPP_

#include <L2Fsim/aircraft/aircraft.hpp>
#include <L2Fsim/aircraft/beeler_glider/beeler_glider_state.hpp>
#include <L2Fsim/aircraft/beeler_glider/beeler_glider_command.hpp>
#include <L2Fsim/utils/quaternion.hpp>
#include <L2Fsim/utils/utils.hpp>
#include <vector>
#include <cmath>

/**
 * @file beeler_glider.hpp
 * @version 1.1
 * @since 1.0
 *
 * Beeler's glider model
 * Equations derived from:
 * Beeler, Moerder and Cox. A Flight Dynamics Model for a Small Glider
 * in Ambient Winds. NASA/TM-2003-212665. 2003.
 * Model validity:
 * - wingspan in [1.52m; 3.55m]
 * - aspect ratio in [6; 16]
 * - weight in [0.23kg; 5.44kg]
 * Notations:
 * (x,y,z) glider position in an Earth-based coordinate system
 * V ground speed of the glider
 * khi azimuth angle (angle between the north and the projection on xy-plan of the velocity vector)
 * gamma elevation angle (angle between the velocity vector and its projection on the xy-plan)
 * sigma bank angle (rotation allong the velocity vector)
 * alpha angle of attack
 * beta sideslip angle
 *
 * @note the earth frame corresponds to the directions of the north, east and downward for the x, y and z axis. However the z notation used in the code corresponds to the altitude i.e. '-z'
 * @note khi, gamma and sigma form an euler sequence leading to the velocity frame
 */

namespace L2Fsim {

class beeler_glider : public aircraft {
public:
    /**
     * Attributes
     * @param {beeler_glider_state} s; the state of the aircraft
     * @param {beeler_glider_command} u; the command of the aircraft
     * @param {double} mass; mass in [0.23kg; 5.44kg]
     * @param {double} wingspan; wing span in [1.52m; 3.55m]
     * @param {double} aspect_ratio; aspect ratio in [6; 16]
     * @param {double} ARv; aspect ratio of vertical tail
     * @param {double} Sf; fuselage area
     * @param {double} lt; fuselage moment arm length
     * @param {double} Vh; horizontal tail volume ratio
     * @param {double} Vv; vertical tail volume ratio
     * @param {double} c_; mean aerodynamic cord
     * @param {double} S; wing surface area
     * @param {double} St; horizontal tail surface
     * @param {double} Sv; vertical tail surface
     * @param {double} e; Oswald efficiency number
     * @param {double} Re; Reynolds number
     * @param {double} a0; lift curve slope
     * @param {double} alpha0; zero point
     * @param {double} Cd0, Cdl; minimum wing profile drag
     * @param {double} Clmin, Cl_alpha, Cl_beta; minimum lift
     */
    beeler_glider_state s;
    beeler_glider_command u;
    double mass;
    double wingspan;
    double aspect_ratio;
    double ARv = 0.5*aspect_ratio;
	//double Sf = 0.01553571429*wingspan*wingspan + .01950357142*wingspan - 0.01030412685; // polynomial regression - unused
    double lt = 0.28*wingspan;
    double Vh = 0.4;
    double Vv = 0.02;
    double c_ = 1.03 * wingspan / aspect_ratio;
    double S = wingspan * wingspan / aspect_ratio;
    //double St = Vh * c_ * S / lt; //unused
    double Sv = Vv * wingspan * S / lt;
    double e = 0.95;
    //double Re = 150000.; //unused
    double a0 = 0.1*(180./M_PI);
    double alpha0 = -2.5*(M_PI/180.);
    double Cd0 = 0.01;
    double Cdl = 0.05;
    double Clmin = 0.4;
    double Cl_alpha = a0/(1.+a0/(M_PI*e*aspect_ratio));
    double Cc_beta = (a0/(1.+a0/(M_PI*e*ARv))) * (Sv/S);
	//double Cdf = .008; //unused
	//double Cdt = .01; //unused
	//double Cde = .002; //unused

    /**
     * Constructor
     * @param {state} _s: initial state
     * @param {command} _u: initial command
     * @param {double} m; mass
     * @param {double} ws; wing span
     * @param {double} ar; aspect ratio
     */
    beeler_glider(
        beeler_glider_state _s,
        beeler_glider_command _u,
        double m=1.36,
        double ws=1.524,
        double ar=16.) :
        s(_s),
        u(_u),
        mass(m),
        wingspan(ws),
        aspect_ratio(ar)
    {}

    /** @brief Get a reference on the state */
    state & get_state() override {return s;}

	/**
	 * @brief Set the state of the aircraft
	 * @param {const beeler_glider_state &} _s; input state
	 */
	void set_state(const beeler_glider_state &_s) {s = _s;}

    /** @brief Get a reference on the command */
    command & get_command() override {return u;}

	/**
	 * @brief Set the command of the aircraft
	 * @param {const beeler_glider_command &} _u; input command
	 */
	void set_command(const beeler_glider_command &_u) {u = _u;}

    double get_distance_to_center() override {
        return sqrt(s.x * s.x + s.y * s.y);
    }

    /** @brief Apply the command i.e. modify the state attribute of the aircraft accordingly to the command */
    aircraft & apply_command() override {
        s.alpha += u.dalpha;
        s.beta += u.dbeta;
        s.sigma += u.dsigma;
        return *this;
    }

    /**
     * @brief Compute the time derivative of the input state
     * @param {const flight_zone &} fz; flight zone
     * @param {const double} t; current time
     * @param {state &} _s; updated state
     * @warning dynamic cast from state to beeler_glider_state
     */
    aircraft & update_state_dynamic(flight_zone &fz, const double t, state &_s) override {
        beeler_glider_state &s = dynamic_cast <beeler_glider_state &> (_s);
        double lift=0., drag=0., sideforce=0.;
        double V = s.V;
        double gamma = s.gamma;
        double khi = s.khi;
        double sigma = s.sigma;

		double cosgamma = cos(gamma);
		double singamma = sin(gamma);
		double cossigma = cos(sigma);
		double sinsigma = sin(sigma);

        calc_aero_forces(fz, t, lift, drag, sideforce);
        s.xdot = V * cosgamma * cos(khi);
        s.ydot = V * cosgamma * sin(khi);
        s.zdot = V * singamma;
        s.Vdot = - drag / mass - 9.81 * singamma;
        s.gammadot = (lift * cossigma + sideforce * sinsigma) / (mass * V) - 9.81 * cosgamma / V;
        s.khidot = (lift * sinsigma - sideforce * cossigma) / (mass * V * cosgamma);
        return *this;
    }

    /**
     * @brief Check if the state vector contains values that are out of the model's range of validity
     * @return true if the aircraft still is in its validity model
     */
    bool is_in_model() override {
        double gm = s.gamma;
        double alpgm = s.alpha + gm;
        double mam = s.max_angle_magnitude;
        if(s.z < 0.) {
            std::cout << "STOP: altitude 'z' < 0" << std::endl;
            return false;
        }
        else if(gm > mam) {
            std::cout << "STOP: elevation angle 'gamma' > " << mam << " (rad)" << std::endl;
            return false;
        }
        else if(gm < -mam) {
            std::cout << "STOP: elevation angle 'gamma' < " << -mam << " (rad)" << std::endl;
            return false;
        }
        else if(alpgm > mam) {
            std::cout << "STOP: inclination angle 'gamma+alpha' > " << mam << " (rad)" << std::endl;
            return false;
        }
        else if(alpgm < -mam) {
            std::cout << "STOP: inclination angle 'gamma+alpha' < " << -mam << " (rad)" << std::endl;
            return false;
        }
        return true;
    }

    /** @brief Return the saved data at each time step */
    std::vector<double> get_save() override {
        return s.get_save();
    }

protected:

    /**
     * @brief Compute lift, drag and sideforce
     * @param {flight_zone &} fz; flight zone
     * @param {const double} t; current time
     * @param {double &} lift, drag, sideforce; aerodynamic forces
     */
    void calc_aero_forces(flight_zone &fz,
                        const double t,
                        double &lift,
                        double &drag,
                        double &sideforce) {

        // Retrieve aircraft's state
        double x = s.x;
        double y = s.y;
        double z = s.z;
        double V = s.V;
        double gamma = s.gamma;
        double khi = s.khi;
        double alpha = s.alpha;
        double beta = s.beta;
        double sigma = s.sigma;
		//double xdot = s.xdot;
		//double ydot = s.ydot;
		//double zdot = s.zdot;
		double cos_gamma = cos(gamma);
		double cos_alpha = cos(alpha);
		double sin_alpha = sin(alpha);
		double cos_beta = cos(beta);
		double sin_beta = sin(beta);

        /** Relative wind */
        std::vector<double> w(3);
        fz.wind(x, y, z, t, w);

        /** Wind relative velocity */
        std::vector<double> V_w(3);
        std::vector<double> X_w(3);

        /** Wind relative angles */
        double alpha_w, beta_w, gamma_w, khi_w, sigma_w;

        /** Aerodynamic force coefficients with wind */
        double Cc_w, Cl_w, Cd_w;

        /** Rotation matrices and quaternions */
        quaternion rviq; // quaternion version of the Euler rotation sequence {khi,gamma,sigma}
        quaternion rwiq; // quaternion version of the Euler rotation sequence {khi_w,gamma_w,sigma_w}

        std::vector<double> rbv(9); // rotation from velocity frame to body frame R_BV
        std::vector<double> rbv1(9); // rotation of alpha
        std::vector<double> rbv2(9); // rotation of beta
        quaternion rbvq; //quaternion version R_BV
        quaternion rbv1q;
        quaternion rbv2q;

        std::vector<double> m(9); // M matrix, used to retrieve wind relative angles
        std::vector<double> m11(9);
        std::vector<double> m12(9);
        quaternion mq; // quaternion version of M
        quaternion m11q;
        quaternion m12q;

        V_w.at(0) = V * cos_gamma * cos(khi) - w.at(0);
        V_w.at(1) = V * cos_gamma * sin(khi) - w.at(1);
        V_w.at(2) = V * sin(gamma) - w.at(2);

		double V_w_2norm = sqrt(V_w.at(0)*V_w.at(0) + V_w.at(1)*V_w.at(1) + V_w.at(2)*V_w.at(2));

        X_w.at(0) = V_w.at(0) / V_w_2norm;
        X_w.at(1) = V_w.at(1) / V_w_2norm;
        X_w.at(2) = V_w.at(2) / V_w_2norm;

        // Calculation of gamma_w and khi_w
        gamma_w = asin(X_w.at(2)); // taking into account the signe change
		double cos_gamma_w = cos(gamma_w);
		double sin_gamma_w = sin(gamma_w);

        if(X_w.at(0) / cos_gamma_w > 1) {
            khi_w = 0;
        }else if(X_w.at(0) / cos_gamma_w < -1){
            khi_w = M_1_PI;
        }else{
            khi_w = sgn(X_w.at(1) / cos_gamma_w) * acos(X_w.at(0) / cos_gamma_w);
        }
		double cos_khi_w = cos(khi_w);
		double sin_khi_w = sin(khi_w);

        // Calculation of alpha_w, beta_w and sigma_w : use of rotation matrices and quaternions

        rviq.fromEuler(khi, gamma, sigma);

        rbv1.at(0) = cos_alpha;
        rbv1.at(2) = sin_alpha;
        rbv1.at(4) = 1.;
        rbv1.at(6) = -sin_alpha;
        rbv1.at(8) = cos_alpha;
        rbv1.at(1) = rbv1.at(3) = rbv1.at(5) = rbv1.at(7) = 0.;

        rbv2.at(0) = cos_beta;
        rbv2.at(1) = sin_beta;
        rbv2.at(3) = -sin_beta;
        rbv2.at(4) = cos_beta;
        rbv2.at(8) = 1.;
        rbv2.at(2) = rbv2.at(5) = rbv2.at(6) = rbv2.at(7) = 0.;

        rbv1q.fromRotationMatrix(rbv1);
        rbv2q.fromRotationMatrix(rbv2);
        rbvq = rbv1q;
        rbvq.multRight(rbv2q);

        m11.at(0) = cos_gamma_w;
        m11.at(2) = -sin_gamma_w;
        m11.at(4) = 1.;
        m11.at(6) = sin_gamma_w;
        m11.at(8) = cos_gamma_w;
        m11.at(1) = m11.at(3) = m11.at(5) = m11.at(7) = 0.;

        m12.at(0) = cos_khi_w;
        m12.at(1) = sin_khi_w;
        m12.at(3) = -sin_khi_w;
        m12.at(4) = cos_khi_w;
        m12.at(8) = 1.;
        m12.at(2) = m12.at(5) = m12.at(6) = m12.at(7) = 0.;

        m11q.fromRotationMatrix(m11);
        m12q.fromRotationMatrix(m12);
        mq = m11q;
        mq.multRight(m12q);
        mq.multRight(rviq);
        mq.multRight(rbvq);
        mq.toRotationMatrix(m);

        alpha_w = asin(m.at(2));
		double cos_alpha_w = cos(alpha_w);

        if(m.at(8) / cos_alpha_w > 1.) {
            sigma_w = 0.;
        }else if(m.at(8) / cos_alpha_w < -1.){
            sigma_w = M_1_PI;
        }else{
            sigma_w = sgn(-m.at(5) / cos_alpha_w) * acos(m.at(8) / cos_alpha_w);
        }

        if(m.at(0) / cos_alpha_w > 1.) {
            beta_w = 0.;
        }else if(m.at(0) / cos_alpha_w < -1.){
            beta_w = M_1_PI;
        }else{
            beta_w = sgn(m.at(1) / cos_alpha_w) * acos(m.at(0) / cos_alpha_w);
        }

        // Calc of the aerodynamic force coefficients with wind
        Cc_w = Cc_beta * beta_w;
        Cl_w = Cl_alpha * (alpha_w - alpha0);
        Cd_w = Cd0 + Cdl * (Cl_w - Clmin) * (Cl_w - Clmin) + Cl_w * Cl_w / (M_PI*e*aspect_ratio) + Cc_w * Cc_w / (M_PI*e*aspect_ratio) * (S / Sv);

        // Dynamic pressure
        double q = 0.5 * 1.225 * V_w_2norm * V_w_2norm;
		double qS = q*S;

        // Calc of the aerodynamic forces in wind frame
        double drag_w = qS * Cd_w;
        double sideforce_w = qS * Cc_w;
        double lift_w = qS * Cl_w;
		std::vector<double> forces_w(3); // forces in the wind frame
		forces_w.at(0) = -drag_w;
		forces_w.at(1) = -sideforce_w;
		forces_w.at(2) = -lift_w;

		// Transformation to the velocity frame
        rwiq.fromEuler(khi_w, gamma_w, sigma_w);
		rviq.invert(); // rviq is rivq
		rviq.multRight(rwiq); //rviq is rivq*rwiq
		rviq.rotateVector(forces_w);

		drag = -forces_w.at(0);
		sideforce = -forces_w.at(1);
		lift = -forces_w.at(2);
    }
};

}

#endif
