#ifndef _LIB_UTILITY_RATE_H_
#define _LIB_UTILITY_RATE_H_

#include "json/json.h"
#include "lib_util.h"
#include <map>

struct ur_month;
class UtilityRate
{
public:
	
	UtilityRate(){};

	/// Pass in the URDB response as a c++ string
	UtilityRate(std::string urdb_response);

	/// Pass in a diurnal (12x24) schedule for both the weekday and weekend energy charges, plus a matrix of corresponding rates
	UtilityRate(util::matrix_t<size_t> ecWeekday, 
				util::matrix_t<size_t> ecWeekend, 
				util::matrix_t<double> ecRatesMatrix);

	/// Parse urdb rate
	bool parseUrdbRate(std::string urdb_response);

	/// Get the energy rates matrix (period, tier, max usage in tier, max usage unit, buy rate ($/kWh), sell rate ($/kWh)
	util::matrix_t<double> getEnergyRatesMatrix();

	virtual ~UtilityRate() {/* nothing to do */ };

protected:
	
	/// URDB JSON representation
	Json::Value m_urdb;
	
	/// Energy charge schedule for weekdays
	util::matrix_t<size_t> m_ecWeekday;

	/// Energy charge schedule for weekends
	util::matrix_t<size_t> m_ecWeekend;

	/// Energy charge periods, tiers, maxes, units, buy rate (sell rate optional)
	util::matrix_t<double> m_ecRatesMatrix;

	/// Energy Tiers per period
	std::map<size_t, size_t> m_energyTiersPerPeriod;

	/// Energy schedule outputs
	std::vector<size_t> m_ec_tou_sched;

	/// Demand schedule outputs
	std::vector<size_t> m_dc_tou_sched;

	/// Utility rate properties by month
	std::vector<ur_month> m_month;

	/// Energy charge periods
	std::vector<size_t> m_ec_periods;

	/// Time step sell rate
	std::vector<double> m_ec_ts_sell_rate;

	/// Energy charge period tier numbers (initial)
	std::vector<std::vector<size_t> >  m_ec_periods_tiers_init;

	/// Demand charge period numbers
	std::vector<size_t> m_dc_tou_periods;

	/// Demand charge (TOU) period tier numbers
	std::vector<std::vector<size_t> >  m_dc_tou_periods_tiers;

	/// Demand charge (flat) tier numbers
	std::vector<std::vector<size_t> >  m_dc_flat_tiers;

	/// Annual Minimum Charge
	double m_annual_min_charge_dollar;

	/// Monthly minimum Charge 
	double m_monthly_min_charge_dollar;

	/// Monthly fixed charge
	double m_monthly_fixed_charge_dollar;

	/// Number of records annually
	size_t m_num_rec_yearly;
};

class UtilityRateCalculator : protected UtilityRate
{
public:
	/// Constructor for rate calculator where load will be input on the fly
	UtilityRateCalculator(UtilityRate * Rate, size_t stepsPerHour);

	/// Constructor for rate calculator where full load is known
	UtilityRateCalculator(UtilityRate * Rate, size_t stepsPerHour, std::vector<double> loadProfile);

	/// Parse the incoming data
	void initializeRate();

	/// Update the bill to include the load at the current timestep
	void updateLoad(double loadPower);

	/// Calculate the utility bill for the full load
	void calculateEnergyUsagePerPeriod();

	/// Get the energy rate at the given hour of year
	double getEnergyRate(size_t );

	/// Get the period for a given hour of year
	size_t getEnergyPeriod(size_t hourOfYear);

	virtual ~UtilityRateCalculator() {/* nothing to do*/ };

protected:

	/// The load profile to evaluate (kW)
	std::vector<double> m_loadProfile;
	
	/// The calculated electricity bill for the UtilityRate and load profile ($)
	double m_electricBill;

	/// The number of time steps per hour
	size_t m_stepsPerHour;

	/// The energy usage per period
	std::vector<double> m_energyUsagePerPeriod;
};

/// Structure containing parsed information about a utility rate over one month
struct ur_month
{
	// period numbers
	std::vector<int> ec_periods;
	std::vector<int> dc_periods;

	// track period numbers at 12a, 6a, 12p and 6p for rollover applications. Weekdays only considered
	std::vector<int> ec_rollover_periods;

	// monthly values
	double energy_net; // net energy use per month
	int hours_per_month; // hours per period per month
	util::matrix_t<double> ec_energy_use; // energy use period and tier

	// handle changing period tiers on monthly basis if kWh/kW
	std::vector<std::vector<int> >  ec_periods_tiers; // tier numbers
	util::matrix_t<double> ec_energy_surplus; // energy surplus - extra generated by system that is either sold or curtailed.

	// peak demand per period
	std::vector<double> dc_tou_peak;
	std::vector<int> dc_tou_peak_hour;
	double dc_flat_peak;
	int dc_flat_peak_hour;

	// energy tou charges
	util::matrix_t<double>  ec_tou_ub_init;
	util::matrix_t<double>  ec_tou_br_init;
	util::matrix_t<double>  ec_tou_sr_init;

	// may change based on units and year
	util::matrix_t<double>  ec_tou_ub;
	util::matrix_t<double>  ec_tou_br;
	util::matrix_t<double>  ec_tou_sr;
	util::matrix_t<int>  ec_tou_units;

	// calculated charges per period and tier
	util::matrix_t<double>  ec_charge;

	// demand tou charges
	util::matrix_t<double>  dc_tou_ub;
	util::matrix_t<double>  dc_tou_ch;

	// demand flat charges
	std::vector<double>  dc_flat_ub;
	std::vector<double>  dc_flat_ch;

	// calculated charges per period
	std::vector<double>  dc_tou_charge;
	double dc_flat_charge;
};





#endif // !_LIB_UTILITY_RATE_H_
