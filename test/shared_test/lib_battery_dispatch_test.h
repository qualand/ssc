#ifndef __LIB_BATTERY_DISPATCH_TEST_H__
#define __LIB_BATTERY_DISPATCH_TEST_H__

#include <gtest/gtest.h>
#include <lib_util.h>
#include <lib_battery_dispatch.h>
#include <lib_battery_powerflow.h>
#include <lib_ondinv.h>
#include <lib_power_electronics.h>
#include <lib_pvinv.h>
#include <lib_sandia.h>
#include <lib_shared_inverter.h>

#include "lib_battery_properties.h"

/// Structure for battery dispatch test settings
struct DispatchProperties
{

	// Generic dispatch
	int dispatchChoice;
	int currentChoice;
	double currentChargeMax;
	double currentDischargeMax;
	double powerChargeMax;
	double powerDischargeMax;
	double minimumModeTime;
	int meterPosition;
	size_t * schedWeekday;
	util::matrix_t<size_t> scheduleWeekday;
	util::matrix_t<size_t> scheduleWeekend;
	std::vector<bool> canCharge;
	std::vector<bool> canDischarge;
	std::vector<bool> canGridcharge;
	std::map<size_t, double> percentDischarge;
	std::map<size_t, double> percentGridcharge;

	// Front of meter auto dispatch
	std::vector<double> ppaRate;
	UtilityRate * ur{nullptr};

    sandia_inverter_t* sandia;
    partload_inverter_t* partload;
    ond_inverter* ond;
    SharedInverter* m_sharedInverter;

	/// Constructor for dispatch properties
	DispatchProperties()
	{

		// dispatch
		dispatchChoice = 4; // custom dispatch
		currentChoice = dispatch_t::CURRENT_CHOICE::RESTRICT_POWER;
		currentChargeMax = 100;
		currentDischargeMax = 100;
		powerChargeMax = 50;
		powerDischargeMax = 50;
		minimumModeTime = 0.1;
		meterPosition = dispatch_t::METERING::BEHIND;

		schedWeekday = new size_t[24 * 12];

		int i = 0;
		for (int m = 0; m < 12; m++) {
			for (int h = 0; h < 24; h++) {
				schedWeekday[i] = 1;
				if (h > 11 && h < 19) {
					schedWeekday[i] = 3;
				}
				i++;
			}
		}
		scheduleWeekday.assign(schedWeekday, 12, 24);
		scheduleWeekend.assign(schedWeekday, 12, 24);

		for (int p = 0; p < 6; p++) {
			canCharge.push_back(1);
			canDischarge.push_back(1);
			canGridcharge.push_back(0);
			percentDischarge[p] = 100;
		}
		// dispatch FOM
		for (size_t i = 0; i < 8760; i++) {
			ppaRate.push_back(1.0);
		}

        // inverter
        int numberOfInverters = 40; // Match the inverter AC capacity to the battery capacity in the FOM tests. May want a different inverter for BTM
        sandia = new sandia_inverter_t();
        partload = new partload_inverter_t();
        ond = new ond_inverter();
        sandia->C0 = -2.445577e-8;
        sandia->C1 = 1.2e-5;
        sandia->C2 = 0.001461;
        sandia->C3 = -0.00151;
        sandia->Paco = 77000;
        sandia->Pdco = 791706.4375;
        sandia->Vdco = 614;
        sandia->Pso = 2859.5;
        sandia->Pntare = 0.99;
        m_sharedInverter = new SharedInverter(SharedInverter::SANDIA_INVERTER, numberOfInverters, sandia, partload, ond);

	}
	/// Destructor
	~DispatchProperties() {
		if (schedWeekday) {
			delete schedWeekday;
		}
        if (m_sharedInverter) {
            delete m_sharedInverter;
            m_sharedInverter = nullptr;
        }
        if (sandia) {
            delete sandia;
            sandia = nullptr;
        }
        if (partload) {
            delete partload;
            partload = nullptr;
        }
        if (ond) {
            delete ond;
            ond = nullptr;
        }
	}
};

/**
* \class BatteryDispatchTest
*
* Test Manual battery dispatch algorithm
*
*/
class ManualTest_lib_battery_dispatch : public BatteryProperties , public DispatchProperties
{
protected:

	capacity_lithium_ion_t * capacityModel;
	voltage_dynamic_t * voltageModel;
	thermal_t * thermalModel;
	lifetime_calendar_t * calendarModel;
	lifetime_cycle_t * cycleModel;
	lifetime_t * lifetimeModel;
	losses_t * lossModel;
	battery_t * batteryModel;
	BatteryPower * batteryPower;

	dispatch_manual_t * dispatchManual{nullptr};

	double P_pv;
	double V_pv;
	double P_load;
	double P_clipped;

	/*! Variables to store forecast data */
	std::vector<double> pv_prediction;
	std::vector<double> load_prediction;
	std::vector<double> cliploss_prediction;

public:

	void SetUp()
	{
		// For Manual Dispatch Test
		BatteryProperties::SetUp();
		capacityModel = new capacity_lithium_ion_t(q, SOC_init, SOC_max, SOC_min);
		voltageModel = new voltage_dynamic_t(n_series, n_strings, Vnom_default, Vfull, Vexp, Vnom, Qfull, Qexp, Qnom,
                                             C_rate, resistance, dtHour);
		cycleModel = new lifetime_cycle_t(cycleLifeMatrix);
		calendarModel = new lifetime_calendar_t(calendarChoice, calendarLifeMatrix, dtHour);
		lifetimeModel = new lifetime_t(cycleModel, calendarModel, replacementOption, replacementCapacity);
		thermalModel = new thermal_t(1.0, mass, length, width, height, Cp, h, T_room, capacityVsTemperature);
		lossModel = new losses_t(dtHour, lifetimeModel, thermalModel, capacityModel, lossChoice, monthlyLosses, monthlyLosses, monthlyLosses, fullLosses);
		batteryModel = new battery_t(dtHour, chemistry);
		batteryModel->initialize(capacityModel, voltageModel, lifetimeModel, thermalModel, lossModel);

				P_pv = P_load = V_pv = P_clipped = 0;
	}
	void TearDown()
	{
		BatteryProperties::TearDown();
        delete capacityModel;
        delete voltageModel;
        delete cycleModel;
        delete calendarModel;
        delete lifetimeModel;
        delete thermalModel;
        delete lossModel;
        delete batteryModel;
        delete dispatchManual;
	}
};

/**
* \class BatteryDispatchTest
*
* Automatic behind the meter battery dispatch algorithms
*
*/
class AutoBTMTest_lib_battery_dispatch : public BatteryProperties , public DispatchProperties
{
protected:

    capacity_lithium_ion_t * capacityModel;
    voltage_dynamic_t * voltageModel;
    thermal_t * thermalModel;
    lifetime_calendar_t * calendarModel;
    lifetime_cycle_t * cycleModel;
    lifetime_t * lifetimeModel;
    losses_t * lossModel;
    battery_t * batteryModel;
    BatteryPower * batteryPower;

    dispatch_automatic_behind_the_meter_t * dispatchAutoBTM{nullptr};

    double P_pv;
    double V_pv;
    double P_load;
    double P_clipped;

    /*! Variables to store forecast data */
    std::vector<double> pv_prediction;
    std::vector<double> load_prediction;
    std::vector<double> cliploss_prediction;

public:

    void SetUp()
    {
        // For Manual Dispatch Test
        BatteryProperties::SetUp();
        capacityModel = new capacity_lithium_ion_t(q, SOC_init, SOC_max, SOC_min);
        voltageModel = new voltage_dynamic_t(n_series, n_strings, Vnom_default, Vfull, Vexp, Vnom, Qfull, Qexp, Qnom,
                                             C_rate, resistance, dtHour);
        cycleModel = new lifetime_cycle_t(cycleLifeMatrix);
        calendarModel = new lifetime_calendar_t(calendarChoice, calendarLifeMatrix, dtHour);
        lifetimeModel = new lifetime_t(cycleModel, calendarModel, replacementOption, replacementCapacity);
        thermalModel = new thermal_t(1.0, mass, length, width, height, Cp, h, T_room, capacityVsTemperature);
        lossModel = new losses_t(dtHour, lifetimeModel, thermalModel, capacityModel, lossChoice, monthlyLosses, monthlyLosses, monthlyLosses, fullLosses);
        batteryModel = new battery_t(dtHour, chemistry);
        batteryModel->initialize(capacityModel, voltageModel, lifetimeModel, thermalModel, lossModel);

        dispatchAutoBTM = new dispatch_automatic_behind_the_meter_t(batteryModel, dtHour, SOC_min, SOC_max, currentChoice, currentChargeMax,
                                                                    currentDischargeMax, powerChargeMax, powerDischargeMax, powerChargeMax, powerDischargeMax, 0, 0, 0, 1, 24, 1, true, true, false, false);

        P_pv = P_load = V_pv = P_clipped = 0;
    }
    void TearDown()
    {
        BatteryProperties::TearDown();
        delete capacityModel;
        delete voltageModel;
        delete cycleModel;
        delete calendarModel;
        delete lifetimeModel;
        delete thermalModel;
        delete lossModel;
        delete batteryModel;
        delete dispatchAutoBTM;
    }
};

/**
* \class BatteryDispatchTest
*
* Test automatic front of the meter
*
*/
class AutoFOMTest_lib_battery_dispatch : public BatteryProperties , public DispatchProperties
{
protected:


    thermal_t * thermalModel;
    lifetime_calendar_t * calendarModel;
    lifetime_cycle_t * cycleModel;
    lifetime_t * lifetimeModel;
    BatteryPower * batteryPower;

    capacity_lithium_ion_t * capacityModelFOM;
    voltage_dynamic_t * voltageModelFOM;
    losses_t * lossModelFOM;
    battery_t *batteryModelFOM;

    dispatch_automatic_front_of_meter_t * dispatchAutoFOM{nullptr};


    double P_pv;
    double V_pv;
    double P_load;
    double P_clipped;

    /*! Variables to store forecast data */
    std::vector<double> pv_prediction;
    std::vector<double> load_prediction;
    std::vector<double> cliploss_prediction;

public:

    void SetUp()
    {
        // For Debugging Input Battery Target front of meter minute time steps
        BatteryProperties::SetUp();

        double dtHourFOM = 1.0 / 60.0;
        capacityModelFOM = new capacity_lithium_ion_t(2.25 * 444, 63.3475, 95, 15);
        voltageModelFOM = new voltage_dynamic_t(139, 444, 3.6, 4.10, 4.05, 3.4, 2.25, 0.04, 2.00, 0.2, 0.2, dtHourFOM);
        cycleModel = new lifetime_cycle_t(cycleLifeMatrix);
        calendarModel = new lifetime_calendar_t(calendarChoice, calendarLifeMatrix, dtHour);
        lifetimeModel = new lifetime_t(cycleModel, calendarModel, replacementOption, replacementCapacity);
        thermalModel = new thermal_t(1.0, mass, length, width, height, Cp, h, T_room, capacityVsTemperature);
        lossModelFOM = new losses_t(dtHourFOM, lifetimeModel, thermalModel, capacityModelFOM, lossChoice, monthlyLosses, monthlyLosses, monthlyLosses, fullLossesMinute);
        batteryModelFOM = new battery_t(dtHourFOM, chemistry);
        batteryModelFOM->initialize(capacityModelFOM, voltageModelFOM, lifetimeModel, thermalModel, lossModelFOM);
        dispatchAutoFOM = new dispatch_automatic_front_of_meter_t(batteryModelFOM, dtHourFOM, 15, 95, 1, 999, 999, 500, 500, 500, 500, 1, 3, 0, 1, 24, 1, true, true, false, true, 0, 0, 0, 0, ppaRate, ur, 98, 98, 98);

        P_pv = P_load = V_pv = P_clipped = 0;
    }
    void TearDown()
    {
        BatteryProperties::TearDown();
        delete capacityModelFOM;
        delete voltageModelFOM;
        delete cycleModel;
        delete calendarModel;
        delete lifetimeModel;
        delete thermalModel;
        delete lossModelFOM;
        delete batteryModelFOM;
        delete dispatchAutoFOM;
    }

};


/**
* \class BatteryDispatchTest
*
* Automatic front of meter with DC coupling of the battery dispatch algorithms
*
*/
class AutoFOMDC_lib_battery_dispatch : public BatteryProperties , public DispatchProperties
{
protected:


    thermal_t * thermalModel;
    lifetime_calendar_t * calendarModel;
    lifetime_cycle_t * cycleModel;
    lifetime_t * lifetimeModel;
    BatteryPower * batteryPower;

    capacity_lithium_ion_t * capacityModelDC;
    voltage_dynamic_t * voltageModelDC;
    losses_t * lossModelDC;
    battery_t *batteryModelDC;

    dispatch_automatic_front_of_meter_t * dispatchAutoDC{ nullptr };

    double P_pv;
    double V_pv;
    double P_load;
    double P_clipped;

    double dtHourDC;

    /*! Variables to store forecast data */
    std::vector<double> pv_prediction;
    std::vector<double> load_prediction;
    std::vector<double> cliploss_prediction;

public:

    void SetUp()
    {
        // For testing Automated Front-of-meter DC-coupled
        BatteryProperties::SetUp();

        dtHourDC = 1.0;
        capacityModelDC = new capacity_lithium_ion_t(2.25 * 133227, 50, 100, 10);
        voltageModelDC = new voltage_dynamic_t(139, 133227, 3.6, 4.10, 4.05, 3.4, 2.25, 0.04, 2.00, 0.2, 0.2, dtHourDC);
        cycleModel = new lifetime_cycle_t(cycleLifeMatrix);
        calendarModel = new lifetime_calendar_t(calendarChoice, calendarLifeMatrix, dtHour);
        lifetimeModel = new lifetime_t(cycleModel, calendarModel, replacementOption, replacementCapacity);
        thermalModel = new thermal_t(1.0, mass, length, width, height, Cp, h, T_room, capacityVsTemperature);
        lossModelDC = new losses_t(dtHourDC, lifetimeModel, thermalModel, capacityModelDC, lossChoice);
        batteryModelDC = new battery_t(dtHourDC, chemistry);
        batteryModelDC->initialize(capacityModelDC, voltageModelDC, lifetimeModel, thermalModel, lossModelDC);

        P_pv = P_load = V_pv = P_clipped = 0;
    }
    void TearDown()
    {
        BatteryProperties::TearDown();
        delete capacityModelDC;
        delete voltageModelDC;
        delete cycleModel;
        delete calendarModel;
        delete lifetimeModel;
        delete thermalModel;
        delete lossModelDC;
        delete batteryModelDC;
        delete dispatchAutoDC;
    }

};


#endif
