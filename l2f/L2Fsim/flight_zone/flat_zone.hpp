#ifndef L2FSIM_FLAT_ZONE_HPP_
#define L2FSIM_FLAT_ZONE_HPP_

#include <vector>

/**
 * @file flat_zone.hpp
 * @brief The class flat_zone is a derived class of flight_zone implementing a flat ground at sea level (z=0) with a horizontal wind
 * @version 1.1
 * @since 1.0
 */

namespace L2Fsim {

class flat_zone : public flight_zone {
protected:
    /**
     * Attributes
     * @param {double} windx; global wind velocity along the x-axis of the earth frame
     * @param {double} windy; global wind velocity along the y-axis of the earth frame
     */
	double windx, windy;

public:
    /** Constructor */
    flat_zone(double _windx=0., double _windy=0.) :
        windx(_windx),
        windy(_windy)
    {}

    /** Destructor */
    virtual ~flat_zone() = default;

    /**
	 * Compute the wind vector w, at point (x,y,z,t)
     * @param {double} x, y, z; coordinates in the earth frame
     * @param {double} t; time
	 * @param {std::vector<double>} w; wind vector (windx,windy,windz)
	 */
	virtual flat_zone& wind(double x, double y, double z, double t, std::vector<double> &w) override {
        (void)x; (void)y; (void)z; (void)t; // unused by default
        std::vector<double>(3,0.).swap(w);
        w.at(0) = windx;
        w.at(1) = windy;
        return *this;
	}

};

}

#endif
