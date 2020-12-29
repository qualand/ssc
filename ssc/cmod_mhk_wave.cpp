/*******************************************************************************************************
*  Copyright 2017 Alliance for Sustainable Energy, LLC
*
*  NOTICE: This software was developed at least in part by Alliance for Sustainable Energy, LLC
*  (�Alliance�) under Contract No. DE-AC36-08GO28308 with the U.S. Department of Energy and the U.S.
*  The Government retains for itself and others acting on its behalf a nonexclusive, paid-up,
*  irrevocable worldwide license in the software to reproduce, prepare derivative works, distribute
*  copies to the public, perform publicly and display publicly, and to permit others to do so.
*  copies to the public, perform publicly and display publicly, and to permit others to do so.
*
*  Redistribution and use in source and binary forms, with or without modification, are permitted
*  provided that the following conditions are met:
*
*  1. Redistributions of source code must retain the above copyright notice, the above government
*  rights notice, this list of conditions and the following disclaimer.
*
*  2. Redistributions in binary form must reproduce the above copyright notice, the above government
*  rights notice, this list of conditions and the following disclaimer in the documentation and/or
*  other materials provided with the distribution.
*
*  3. The entire corresponding source code of any redistribution, with or without modification, by a
*  research entity, including but not limited to any contracting manager/operator of a United States
*  National Laboratory, any institution of higher learning, and any non-profit organization, must be
*  made publicly available under this license for as long as the redistribution is made available by
*  the research entity.
*
*  4. Redistribution of this software, without modification, must refer to the software by the same
*  designation. Redistribution of a modified version of this software (i) may not refer to the modified
*  version by the same designation, or by any confusingly similar designation, and (ii) must refer to
*  the underlying software originally provided by Alliance as �System Advisor Model� or �SAM�. Except
*  to comply with the foregoing, the terms �System Advisor Model�, �SAM�, or any confusingly similar
*  designation may not be used to refer to any modified version of this software or any modified
*  version of the underlying software originally provided by Alliance without the prior written consent
*  of Alliance.
*
*  5. The name of the copyright holder, contributors, the United States Government, the United States
*  Department of Energy, or any of their employees may not be used to endorse or promote products
*  derived from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
*  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
*  FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER,
*  CONTRIBUTORS, UNITED STATES GOVERNMENT OR UNITED STATES DEPARTMENT OF ENERGY, NOR ANY OF THEIR
*  EMPLOYEES, BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
*  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
*  IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
*  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************************************/

#include "core.h"
#include "common.h"
#include <vector>
#include <algorithm> 

static var_info _cm_vtab_mhk_wave[] = {
	//   VARTYPE			DATATYPE			NAME									LABEL																UNITS           META            GROUP              REQUIRED_IF					CONSTRAINTS					UI_HINTS	
	{ SSC_INPUT,            SSC_NUMBER,         "wave_resource_model_choice",           "Hourly or JPD wave resource data",                                 "0/1",             "",             "MHKWave",          "",                         "INTEGER",                  "" },
    { SSC_INPUT,			SSC_MATRIX,			"wave_resource_matrix",					"Frequency distribution of wave resource as a function of Hs and Te","",			"",             "MHKWave",			"wave_resource_model_choice=0",						"",							"" },
    //{ SSC_INPUT,            SSC_MATRIX,         "wave_resource_time_series",            "Time series (3 hour?) wave resource data",                         "",             "",             "MHKWave",          "wave_resource_model_choice=1","",                      "" },
    { SSC_INPUT,            SSC_ARRAY,          "wave_significant_height",              "Significant wave height time series data",                         "m",            "",             "MHKWave",          "wave_resource_model_choice=1", "",                 ""   },
    { SSC_INPUT,            SSC_ARRAY,          "wave_energy_period",                   "Wave period time series data",                                     "s",            "",             "MHKWave",          "wave_resource_model_choice=1", "",                 ""   },
    { SSC_INPUT,			SSC_MATRIX,			"wave_power_matrix",					"Wave Power Matrix",												"",				"",             "MHKWave",			"*",						"",							"" },
//	{ SSC_INPUT,			SSC_NUMBER,			"annual_energy_loss",					"Total energy losses",												"%",			"",             "MHKWave",			"?=0",						"",							"" },
	//{ SSC_INPUT,			SSC_NUMBER,			"calculate_capacity",					"Calculate capacity outside UI?",									"0/1",			"",             "MHKWave",          "?=1",                      "INTEGER,MIN=0,MAX=1",      "" },
	{ SSC_INPUT,			SSC_NUMBER,			"number_devices",						"Number of wave devices in the system",								"",				"",             "MHKWave",          "?=1",                      "INTEGER",			    	"" },
	{ SSC_INPUT,			SSC_NUMBER,			"system_capacity",						"System Nameplate Capacity",										"kW",			"",				"MHKWave",			"?=0",						"",							"" },
	
	{ SSC_INPUT,			SSC_NUMBER,			"device_rated_power",				"Rated capacity of device",													"kW",			"",				"MHKWave",			"*",		"",						"" },
    { SSC_INPUT,			SSC_NUMBER,			"fixed_charge_rate",						"FCR from LCOE Cost page",									"",				"",             "MHKWave",         "?=1",                      "",				"" },
    { SSC_INPUT,			SSC_NUMBER,			"device_costs_total",						"Device costs",									"$",				"",             "MHKWave",         "?=1",                      "",				"" },
    { SSC_INPUT,			SSC_NUMBER,			"balance_of_system_cost_total",						"BOS costs",									"$",				"",             "MHKWave",         "?=1",                      "",				"" },
    { SSC_INPUT,			SSC_NUMBER,			"financial_cost_total",						"Financial costs",									"$",				"",             "MHKWave",         "?=1",                      "",				"" },
    { SSC_INPUT,			SSC_NUMBER,			"total_operating_cost",						"O&M costs",									"$",				"",             "MHKWave",         "?=1",                      "",				"" },
	// losses
	{ SSC_INPUT,			SSC_NUMBER,			"loss_array_spacing",				"Array spacing loss",													"%",			"",				"MHKWave",			"*",		"",						"" },
	{ SSC_INPUT,			SSC_NUMBER,			"loss_resource_overprediction",				"Resource overprediction loss",													"%",			"",				"MHKWave",			"*",		"",						"" },
	{ SSC_INPUT,			SSC_NUMBER,			"loss_transmission",				"Transmission losses",													"%",			"",				"MHKWave",			"*",		"",						"" },
	{ SSC_INPUT,			SSC_NUMBER,			"loss_downtime",				"Array/WEC downtime loss",													"%",			"",				"MHKWave",			"*",		"",						"" },
	{ SSC_INPUT,			SSC_NUMBER,			"loss_additional",				"Additional losses",													"%",			"",				"MHKWave",			"*",		"",						"" },



	{ SSC_OUTPUT,			SSC_NUMBER,			"device_average_power",					"Average power production of a single device",											"kW",			"",				"MHKWave",			"*",						"",							"" },
	{ SSC_OUTPUT,			SSC_NUMBER,			"annual_energy",						"Annual energy production of array",											"kWh",			"",				"MHKWave",			"*",						"",							"" },
    { SSC_OUTPUT,           SSC_ARRAY,          "hourly_energy",                        "Hourly energy production of array",                                            "kWh",          "", "MHKWave",          "wave_resource_model_choice=1",                        "",          "" },
    { SSC_OUTPUT,           SSC_ARRAY,          "sig_wave_height_index_mat",            "Wave height index locations for time series",                      "m",                         "", "MHKWave",          "wave_resource_model_choice=1",                        "",          "" },
    { SSC_OUTPUT,           SSC_ARRAY,          "energy_period_index_mat",            "Wave period index locations for time series",                      "s",                         "", "MHKWave",          "wave_resource_model_choice=1",                        "",          "" },
    { SSC_OUTPUT,           SSC_ARRAY,          "wave_power_index_mat",            "Wave power for time series",                      "kW",                         "", "MHKWave",          "wave_resource_model_choice=1",                        "",          "" },
    { SSC_OUTPUT,			SSC_NUMBER,			"capacity_factor",						"Capacity Factor",													"%",			"",				"MHKWave",			"*",						"",							"" },
	{ SSC_OUTPUT,			SSC_MATRIX,			"annual_energy_distribution",			"Annual energy production as function of Hs and Te",				"",				"",				"MHKWave",			"wave_resource_model_choice=0",						"",							"" },
    { SSC_OUTPUT,			SSC_NUMBER,			"wave_resource_start_height",			"Wave height at which first non-zero wave resource value occurs (m)",				"",				"",				"MHKWave",			"wave_resource_model_choice=0",						"",							"" },
    { SSC_OUTPUT,			SSC_NUMBER,			"wave_resource_start_period",			"Wave period at which first non-zero wave resource value occurs (s)",				"",				"",				"MHKWave",			"wave_resource_model_choice=0",						"",							"" },
    { SSC_OUTPUT,			SSC_NUMBER,			"wave_resource_end_height",			"Wave height at which last non-zero wave resource value occurs (m)",				"",				"",				"MHKWave",			"wave_resource_model_choice=0",						"",							"" },
    { SSC_OUTPUT,			SSC_NUMBER,			"wave_resource_end_period",			"Wave period at which last non-zero wave resource value occurs (s)",				"",				"",				"MHKWave",			"wave_resource_model_choice=0",						"",							"" },
    { SSC_OUTPUT,			SSC_NUMBER,			"wave_power_start_height",			"Wave height at which first non-zero WEC power output occurs (m)",				"",				"",				"MHKWave",			"wave_resource_model_choice=0",						"",							"" },
    { SSC_OUTPUT,			SSC_NUMBER,			"wave_power_start_period",			"Wave period at which first non-zero WEC power output occurs (s)",				"",				"",				"MHKWave",			"wave_resource_model_choice=0",						"",							"" },
    { SSC_OUTPUT,			SSC_NUMBER,			"wave_power_end_height",			"Wave height at which last non-zero WEC power output occurs (m)",				"",				"",				"MHKWave",			"wave_resource_model_choice=0",						"",							"" },
    { SSC_OUTPUT,			SSC_NUMBER,			"wave_power_end_period",			"Wave period at which last non-zero WEC power output occurs (s)",				"",				"",				"MHKWave",			"wave_resource_model_choice=0",						"",							"" },
    { SSC_OUTPUT,			SSC_NUMBER,			"total_capital_cost_kwh",           "Capital costs per unit annual energy",		"$/kWh",			"",				"MHKWave",			"*",						"",						"" },
    { SSC_OUTPUT,			SSC_NUMBER,			"total_device_cost_kwh",            "Device costs per unit annual energy",		"$/kWh",			"",				"MHKWave",			"*",						"",						"" },
    { SSC_OUTPUT,			SSC_NUMBER,			"total_bos_cost_kwh",               "Balance of system costs per unit annual energy",		"$/kWh",			"",				"MHKWave",			"*",						"",						"" },
    { SSC_OUTPUT,			SSC_NUMBER,			"total_financial_cost_kwh",         "Financial costs per unit annual energy",		"$/kWh",			"",				"MHKWave",			"*",						"",						"" },
    { SSC_OUTPUT,			SSC_NUMBER,			"total_om_cost_kwh",                "O&M costs per unit annual energy",		"$/kWh",			"",				"MHKWave",			"*",						"",						"" },
    { SSC_OUTPUT,			SSC_NUMBER,			"total_capital_cost_lcoe",          "Capital cost as percentage of overall LCOE",		"%",			"",				"MHKWave",			"*",						"",						"" },
    { SSC_OUTPUT,			SSC_NUMBER,			"total_device_cost_lcoe",           "Device cost",		"%",			"",				"MHKWave",			"*",						"",						"" },
    { SSC_OUTPUT,			SSC_NUMBER,			"total_bos_cost_lcoe",              "BOS cost",		"%",			"",				"MHKWave",			"*",						"",						"" },
    { SSC_OUTPUT,			SSC_NUMBER,			"total_financial_cost_lcoe",        "Financial cost",		"%",			"",				"MHKWave",			"*",						"",						"" },
    { SSC_OUTPUT,			SSC_NUMBER,			"total_om_cost_lcoe",               "O&M cost (annual)",		"%",			"",				"MHKWave",			"*",						"",						"" },
    var_info_invalid
};

class cm_mhk_wave : public compute_module
{
private:
public:
	cm_mhk_wave() {
		add_var_info(_cm_vtab_mhk_wave);
	}

	void exec() {


        // total loss
        double total_loss = as_double("loss_array_spacing")
            + as_double("loss_resource_overprediction")
            + as_double("loss_transmission")
            + as_double("loss_downtime")
            + as_double("loss_additional");


        util::matrix_t<double>  wave_power_matrix = as_matrix("wave_power_matrix");
        //Get the system capacity
        //double system_capacity = as_double("system_capacity");
        double annual_energy = 0, device_rated_capacity = 0, device_average_power = 0, capacity_factor = 0;
        //User either sets device_rated_capacity in the UI, or allows cmod to determine from power curve:
        device_rated_capacity = as_double("device_rated_power");

        //Read number of devices
        int number_devices = as_integer("number_devices");

        //Read and store wave resource and power matrix as a 2D matrix of vectors:
		

        if(as_integer("wave_resource_model_choice")==0) {

            util::matrix_t<double>  wave_resource_matrix = as_matrix("wave_resource_matrix");
		    
		    //Check to ensure size of wave_power_matrix == wave_resource_matrix :
		    if ( (wave_resource_matrix.ncols() !=  wave_power_matrix.ncols() ) || ( wave_resource_matrix.nrows() != wave_power_matrix.nrows() ) )
			    throw exec_error("mhk_wave", "Size of Power Matrix is not equal to Wave Resource Matrix");

		    //Checker to ensure frequency distribution adds to >= 99.5%:
		    double resource_vect_checker = 0;

		    //Allocate memory to store annual_energy_distribution:
		    ssc_number_t *p_annual_energy_dist = allocate("annual_energy_distribution", wave_resource_matrix.nrows(), wave_resource_matrix.ncols());
            
		    int k = 0;
		
		
		    //Create a vector to store 1st column values of resource and power curve. This is compared against
		    //the values of 1st column passed by user in UI:
		    //std::vector<double> _check_column{0, 0.25, 0.75, 1.25, 1.75, 2.25, 2.75, 3.25, 3.75, 4.25, 4.75, 5.25, 5.75, 6.25, 6.75, 7.25, 7.75, 8.25, 8.75, 9.25, 9.75};
		
		    for (size_t i = 0; i < (size_t)wave_power_matrix.nrows(); i++) {
			    for (size_t j = 0; j < (size_t)wave_power_matrix.ncols(); j++) {

				    //Store max power if not set in UI:
				    /*if(as_integer("calculate_capacity") > 0)
					    if (_power_vect[i][j] > system_capacity)
						    system_capacity = _power_vect[i][j];*/

				    //Calculate and allocate annual_energy_distribution:
				    if (j == 0 || i == 0)	//Where (i = 0) is the row header, and (j =  0) is the column header.
					    p_annual_energy_dist[k] = (ssc_number_t) wave_resource_matrix.at(i, j);
				    else {
					    p_annual_energy_dist[k] = (ssc_number_t)(wave_resource_matrix.at(i,j) * wave_power_matrix.at(i,j) * 8760.0 / 100.0);
					    annual_energy += p_annual_energy_dist[k];
					    device_average_power += (p_annual_energy_dist[k] / 8760);
					    //Checker to ensure frequency distribution adds to >= 99.5%:
					    resource_vect_checker += wave_resource_matrix.at(i,j);
				    }
				    k++;

			    }

			    /*//Throw exception if default header column (of power curve and resource) does not match user input header row:
			    if (_check_column[i] != wave_resource_matrix.at(i, 0))
				    throw compute_module::exec_error("mhk_wave", "Wave height bins of resource matrix don't match. Reset bins to default");
			    if (_check_column[i] != wave_power_matrix.at(i,0))
				    throw compute_module::exec_error("mhk_wave", "Wave height bins of power matrix don't match. Reset bins to default");*/
		    }

            if (resource_vect_checker < 99.5)
                throw exec_error("mhk_wave", "Probability vector does not add up to 100%.");


            double wave_resource_start_period = 0;
            double wave_resource_start_height = 0;
            double wave_resource_end_period = 0;
            double wave_resource_end_height = 0;
            double row_check = 0;
            for (size_t l = 0; l < (size_t)wave_power_matrix.nrows(); l++) {
                for (size_t m = 0; m < (size_t)wave_power_matrix.ncols(); m++) {

                    //Store max power if not set in UI:
                    /*if(as_integer("calculate_capacity") > 0)
                        if (_power_vect[i][j] > system_capacity)
                            system_capacity = _power_vect[i][j];*/

                            //Calculate and allocate annual_energy_distribution:
                    if ((ssc_number_t)wave_resource_matrix.at(l, m) != 0 && (ssc_number_t)wave_resource_matrix.at(l, m - 1) == 0 && (ssc_number_t)wave_resource_matrix.at(l, m + 1) != 0 && (m - 1) != 0)
                    {
                        if (wave_resource_start_period == 0)
                        {
                            wave_resource_start_period = wave_resource_matrix.at(0, m);
                            wave_resource_start_height = wave_resource_matrix.at(l, 0);
                        }
                    }
                    else if ((ssc_number_t)wave_resource_matrix.at(l, m) != 0 && (ssc_number_t)wave_resource_matrix.at(l, m - 1) != 0 && (ssc_number_t)wave_resource_matrix.at(l, m + 1) == 0 && (m - 1) != 0)
                    {
                        wave_resource_end_period = wave_resource_matrix.at(0, m);
                        wave_resource_end_height = wave_resource_matrix.at(l, 0);
                    }
                    else
                    {

                    }


                }

                /*//Throw exception if default header column (of power curve and resource) does not match user input header row:
                if (_check_column[i] != wave_resource_matrix.at(i, 0))
                    throw compute_module::exec_error("mhk_wave", "Wave height bins of resource matrix don't match. Reset bins to default");
                if (_check_column[i] != wave_power_matrix.at(i,0))
                    throw compute_module::exec_error("mhk_wave", "Wave height bins of power matrix don't match. Reset bins to default");*/
            }
            double wave_power_start_period = 0;
            double wave_power_start_height = 0;
            double wave_power_end_period = 0;
            double wave_power_end_height = 0;
            for (size_t n = 0; n < (size_t)wave_power_matrix.nrows(); n++) {
                for (size_t p = 0; p < (size_t)wave_power_matrix.ncols(); p++) {

                    //Store max power if not set in UI:
                    /*if(as_integer("calculate_capacity") > 0)
                        if (_power_vect[i][j] > system_capacity)
                            system_capacity = _power_vect[i][j];*/

                            //Calculate and allocate annual_energy_distribution:
                    if ((ssc_number_t)wave_power_matrix.at(n, p) != 0 && (ssc_number_t)wave_power_matrix.at(n, p - 1) == 0 && (ssc_number_t)wave_power_matrix.at(n, p + 1) != 0 && (p - 1) != 0)
                    {
                        if (wave_power_start_period == 0)
                        {
                            wave_power_start_period = wave_power_matrix.at(0, p);
                            wave_power_start_height = wave_power_matrix.at(n, 0);
                        }
                    }
                    else if ((ssc_number_t)wave_power_matrix.at(n, p) != 0 && (ssc_number_t)wave_power_matrix.at(n, p - 1) != 0 && (p - 1) != 0 && (ssc_number_t)wave_power_matrix.at(n, p + 1) == 0)
                    {
                        wave_power_end_period = wave_power_matrix.at(0, p);
                        wave_power_end_height = wave_power_matrix.at(n, 0);
                    }
                    else
                    {

                    }


                }

                /*//Throw exception if default header column (of power curve and resource) does not match user input header row:
                if (_check_column[i] != wave_resource_matrix.at(i, 0))
                    throw compute_module::exec_error("mhk_wave", "Wave height bins of resource matrix don't match. Reset bins to default");
                if (_check_column[i] != wave_power_matrix.at(i,0))
                    throw compute_module::exec_error("mhk_wave", "Wave height bins of power matrix don't match. Reset bins to default");*/
            }

            assign("wave_resource_start_height", var_data((ssc_number_t)wave_resource_start_height));
            assign("wave_resource_end_height", var_data((ssc_number_t)wave_resource_end_height));
            assign("wave_resource_start_period", var_data((ssc_number_t)wave_resource_start_period));
            assign("wave_resource_end_period", var_data((ssc_number_t)wave_resource_end_period));
            assign("wave_power_start_height", var_data((ssc_number_t)wave_power_start_height));
            assign("wave_power_end_height", var_data((ssc_number_t)wave_power_end_height));
            assign("wave_power_start_period", var_data((ssc_number_t)wave_power_start_period));
            assign("wave_power_end_period", var_data((ssc_number_t)wave_power_end_period));

        }

        if (as_integer("wave_resource_model_choice")==1) {

            //util::matrix_t<double>  wave_resource_time_series = as_matrix("Wave_resource_time_series");
            double wave_resource_time_series[2][2920] = {
                {10.3433,10.2281,10.0164,10.0484,10.6579,11.8318,13.4016,13.9962,14.396,14.2596,13.9598,13.6246,13.1635,12.5503,12.0703,11.807,11.4506,11.2637,10.9838,11.2927,11.601,11.8432,11.9576,12.0609,12.195,12.2534,12.0543,11.4879,11.1078,10.6651,10.654,11.7078,13.6484,14.7149,15.3777,15.9543,16.1514,16.0178,15.9601,15.9438,15.7364,15.4482,15.3158,15.0575,14.7617,14.6503,14.5618,14.4392,14.2871,13.8658,13.2012,12.1619,12.4317,12.6143,11.6991,11.1447,10.806,10.2408,10.9366,12.0116,11.8081,11.507,10.8474,10.2826,10.5631,10.9363,11.8423,12.2778,11.9511,12.2839,12.6327,12.0229,11.4942,10.9249,10.9521,10.7354,10.845,11.1625,11.7265,12.9744,14.04,14.594,14.3181,13.3377,12.5877,12.5009,12.6323,11.9917,11.8735,12.0256,12.1363,12.1022,11.2102,10.8481,9.9845,9.9298,10.2142,9.7671,9.6349,9.4902,10.1639,10.2488,10.5078,10.4922,11.0259,11.5923,11.6573,9.4859,9.2921,9.8024,10.2444,10.2561,9.9678,9.5819,9.7229,10.1115,10.9239,11.6499,11.7687,11.2765,10.8796,10.6349,10.3328,10.2802,10.3242,10.2253,10.0537,9.8864,9.7565,9.6938,9.6925,9.6727,9.3783,8.5812,8.1518,7.862,8.2038,8.6144,9.1613,9.2732,9.6475,9.9187,10.2591,10.8898,11.5156,12.1954,12.6418,13.0478,13.4226,13.825,14.1269,14.2936,14.2244,14.0076,13.6148,13.0223,12.7277,12.5308,12.4388,12.4213,12.7098,13.0926,13.1416,13.0422,12.9356,12.7998,12.6314,12.4098,12.1353,11.814,11.1178,11.1527,11.5624,11.8574,12.0879,13.565,14.6896,14.7878,14.2387,13.5552,12.8886,12.2692,11.4529,11.1261,11.0127,10.7492,9.9526,9.8407,9.7425,9.5283,9.4721,9.508,9.7318,9.6765,9.6074,9.5597,9.4676,9.2846,8.1653,8.6813,7.9957,7.4264,7.3906,7.2521,7.2062,7.2567,7.3255,7.5038,7.8654,8.2148,8.3368,8.1825,8.4282,8.8918,8.8896,8.737,8.5915,8.233,7.9726,8.3155,8.5634,8.3291,8.3703,8.9132,9.1143,9.4895,10.2736,10.8856,10.8883,11.5068,11.0715,11.1312,10.9427,11.1656,11.1519,11.0345,9.8101,9.9903,10.1775,9.9273,9.7844,9.54,9.317,9.2941,9.5222,9.8934,9.9148,9.6703,10.0567,10.6601,11.039,11.4144,12.0573,12.2994,11.3835,11.3549,11.8379,12.8788,13.7991,14.4199,14.7279,14.8003,14.7322,14.548,14.2856,13.9637,13.6527,13.3978,13.2121,12.9639,12.7212,12.5511,12.3998,12.1598,11.8453,11.5424,11.2605,10.829,10.4717,9.8401,9.4749,9.4634,9.4034,9.2669,8.9871,9.2586,10.149,10.6236,10.6243,10.7433,11.5175,12.8847,13.8227,14.2265,14.3882,14.4124,14.314,13.6466,12.7232,12.0754,11.6928,11.3678,10.9578,10.711,10.317,10.0983,10.1762,10.5136,11.3073,12.2661,12.8371,12.9884,12.927,12.7622,12.535,12.3004,12.1499,12.0617,12.0007,11.9724,11.9491,11.9066,11.9987,12.1822,12.8204,13.518,13.8994,14.1263,14.0435,13.7182,13.6372,13.644,13.4083,12.8927,11.9814,11.1937,10.2586,9.9731,10.0634,10.0705,9.7553,9.5477,9.1415,8.8184,8.7402,9.5272,9.9726,10.0013,9.8567,9.645,9.1827,8.9452,8.3866,7.7089,7.8681,8.5147,8.9376,9.1197,9.1299,9.0768,9.0579,7.6326,5.8395,5.9712,6.815,7.7568,8.2891,8.6998,8.9188,9.0253,9.3007,9.3941,9.1195,8.4462,7.9199,7.5977,7.713,8.1043,8.6406,8.9435,9.3835,9.5266,9.5149,9.597,9.8034,9.9509,9.9618,9.9574,9.9526,8.5874,8.3301,8.4141,8.629,8.6716,8.6142,8.7856,9.1065,9.5552,9.6178,9.6699,10.1146,10.5379,10.9594,11.1246,10.9945,10.7666,10.6951,10.6821,10.7007,10.8164,10.8635,10.8024,10.6983,10.5932,10.5141,10.4833,10.4983,10.567,10.6391,10.8083,10.8817,10.8644,10.7954,10.6655,10.4922,10.3042,10.1082,9.9039,9.6861,9.4821,9.0474,8.5557,8.293,8.5769,8.6959,8.6696,8.5198,8.7351,8.6451,8.4835,8.661,9.1198,9.5064,9.6916,9.7124,9.6691,9.6586,9.7438,9.7572,9.5371,9.0973,8.9184,8.8338,8.6755,8.6026,8.6377,8.4909,8.2712,8.2249,8.3548,8.5214,8.6018,8.5621,8.6225,8.806,8.8379,8.962,9.2215,9.2611,9.3965,9.6066,9.9874,10.4074,10.3367,10.3749,10.4246,10.3262,10.2724,10.2847,10.4368,10.2962,9.2738,8.9161,8.8347,8.7748,8.796,8.8649,9.0546,9.6255,9.5981,9.5137,9.545,9.8244,10.1424,10.479,10.676,10.543,9.8259,9.1377,8.7377,8.3105,8.1406,8.3233,8.5508,8.8587,8.9252,9.0324,9.1672,9.1262,8.8107,9.1019,9.5348,9.7841,9.8099,9.6846,9.5166,9.4676,9.6621,10.0932,10.5891,10.9858,11.4432,11.5801,11.6829,12.0697,12.2631,12.2028,12.3782,12.5002,11.9114,10.3115,9.2389,8.8296,8.9943,9.6049,9.508,9.041,9.1986,9.5699,10.1536,9.2636,9.6247,9.8916,10.2426,10.8058,11.2527,10.9871,11.0921,11.5504,12.3264,12.5417,12.5294,12.2408,10.5298,10.6897,10.6314,10.5806,10.6481,10.4758,10.476,10.6864,10.741,11.187,11.5354,11.7348,11.8564,11.9435,11.8549,11.3137,10.0822,9.6674,10.3259,10.4587,10.3003,9.9517,9.736,9.5562,9.8911,10.4785,11.0864,10.3795,10.322,11.2413,11.1964,11.2881,11.0336,10.8575,10.9488,10.8996,10.7884,10.6098,10.2588,10.2719,10.4754,10.4634,10.3414,10.2148,9.9975,9.794,9.8588,9.8463,9.8136,9.7992,9.8336,10.2717,11.4158,12.0936,12.0541,10.4103,10.1065,9.4724,9.7596,9.8233,9.6367,9.6857,9.6146,9.4725,9.5195,10.0068,10.7229,11.234,11.3821,11.384,11.3695,11.113,9.2428,9.0966,10.6984,10.5794,10.9376,11.3548,11.1884,11.1433,11.1524,11.6175,11.7561,11.7095,11.7202,11.7964,11.2811,10.8933,10.5322,9.97,9.4398,9.9255,10.5922,11.7184,12.4532,13.3898,13.8078,13.8509,13.8697,13.845,13.6406,13.2901,12.9149,12.5517,11.9621,10.1934,9.8946,9.8318,9.4894,9.3142,9.5148,9.6023,9.994,10.178,10.251,10.2976,10.3156,10.2366,10.0874,9.9047,9.6999,9.2962,8.8345,8.5724,8.8469,9.4115,9.8507,9.6257,9.0226,8.8115,8.8381,9.0137,9.0112,9.328,9.8838,9.9525,9.2925,9.445,9.9505,9.7554,9.7506,9.6481,9.8647,9.8515,9.5059,9.3658,9.1282,8.8676,8.8309,8.9366,9.3309,9.2397,9.1077,8.9803,8.8436,8.4033,7.878,7.5007,7.3666,6.9938,6.5407,6.4265,7.1223,7.7827,8.079,7.9575,7.9221,8.8106,10.0403,11.0446,11.5831,11.8897,12.1934,12.4758,12.6222,12.6305,12.5597,12.4377,12.2633,12.0337,11.7682,11.4826,11.1859,10.896,10.5024,10.2328,10.2729,10.3402,10.4095,10.4435,10.4316,10.4574,10.6394,10.8303,10.8218,10.5608,10.5466,10.2551,9.9728,10.2793,10.4187,10.4704,10.5261,10.5882,10.6459,10.6867,10.6967,10.1499,9.2864,9.0545,9.0193,8.7603,8.6266,8.6201,8.5983,9.4767,8.9368,8.6583,9.268,9.7259,9.0962,8.8918,8.9808,9.2065,9.8289,10.5569,11.4304,12.1333,12.6025,12.6978,12.0217,11.6148,11.319,11.2247,11.0046,10.9806,11.0204,11.0194,11.021,11.0137,11.0034,10.9888,10.9352,10.7914,10.06,8.7176,7.7693,7.9943,8.979,9.7983,9.903,10.7676,12.1333,12.7455,12.7675,12.3207,11.7883,11.5291,11.4841,11.3883,11.2298,11.0395,10.8335,10.6236,10.2853,9.5276,8.8461,8.6219,8.6333,8.5529,8.3893,8.038,7.7064,7.8435,8.3638,8.8029,8.9965,9.116,9.1076,9.0334,8.7644,8.5483,8.2194,7.5447,7.4105,7.9404,8.4872,9.1042,9.5862,9.5795,9.6688,10.0328,10.3618,10.7383,10.9278,10.5756,10.5751,10.8671,10.9422,10.8221,10.5848,10.0823,8.8568,9.4829,9.5452,9.7824,10.0746,10.2852,10.5533,10.7152,10.8843,11.0934,11.3278,11.5577,11.757,11.8452,11.852,11.8125,11.2658,9.0288,8.2413,8.4327,8.4241,8.8344,9.0956,9.5449,9.9713,10.3645,10.5959,10.6848,10.806,10.9707,11.1081,11.1763,11.201,11.1602,11.0133,10.7873,10.5385,10.2565,9.9684,9.799,9.6513,9.0207,8.8935,9.5214,10.0725,10.3131,10.2741,10.117,9.9836,9.2848,8.8533,8.7793,8.6482,8.5958,8.6403,8.7656,8.7767,8.7436,8.7153,8.808,8.8829,9.0011,9.2239,9.4387,9.667,9.9791,10.351,10.6268,10.8389,11.0378,11.2547,11.4509,11.5954,11.6547,11.653,11.6104,11.547,11.4778,11.4597,11.5344,11.6504,11.6893,11.4591,10.9064,9.8321,9.1907,9.3816,9.7738,9.9646,10.2544,10.5584,10.747,10.8895,10.9628,10.9545,10.917,10.9055,10.9229,10.9491,10.9552,10.8983,10.7573,10.5761,10.3917,10.2309,9.9349,9.6014,9.5544,9.5622,9.5417,9.4973,9.4387,9.3106,8.3528,7.9373,7.4901,7.5051,7.7161,7.8464,7.927,8.009,8.2025,8.3745,8.5569,8.7426,8.948,9.176,9.3435,9.4594,9.2385,8.5565,7.9963,7.9836,8.3352,8.3503,7.7316,7.297,6.8211,6.9192,6.8649,6.8243,6.8417,6.6332,6.5228,6.783,7.0205,7.2491,7.3367,7.4887,7.6214,7.8271,8.0808,8.2682,8.3054,8.2367,8.2294,8.3614,8.7077,9.2008,9.6414,9.973,10.1551,10.2734,10.5739,11.0115,11.411,11.5833,11.599,11.5476,11.2346,10.7624,9.2558,8.6956,8.7698,9.284,9.2045,9.3566,9.8099,10.8687,11.8194,12.2324,12.4326,12.2963,11.7522,10.8213,9.2686,8.5502,8.5201,9.1616,9.2426,9.4426,9.7259,10.012,10.2895,10.7061,11.0924,11.3726,11.567,11.6917,11.7266,11.6584,11.5049,11.228,10.7813,10.0262,9.3406,8.7376,8.7273,8.1734,7.9011,8.0374,8.0151,8.049,8.1096,8.188,8.3693,7.9418,7.2547,7.3791,7.4594,7.5781,7.5522,7.4315,7.4326,7.8163,8.2169,8.6263,8.7984,8.8092,8.8715,8.9833,9.0959,9.1468,8.8099,8.2366,7.9879,7.9388,7.9886,8.0608,8.053,7.952,7.7954,7.5872,7.3231,7.3222,7.3732,7.4689,7.5426,7.57,7.2289,7.2742,7.2921,7.4152,7.6632,7.7095,7.8124,7.7138,7.539,7.5908,7.7867,8.0131,8.2302,8.3474,8.4617,8.4372,8.127,8.3887,8.7068,8.9321,8.9879,8.9005,8.8011,8.6211,8.2255,8.4342,8.43,8.453,8.4076,8.285,8.0997,7.7255,7.4299,7.4789,7.551,7.5284,7.3725,7.4843,7.7412,7.8639,7.7846,7.7854,7.8234,7.8984,7.9311,8.0708,8.2572,8.3833,7.7296,7.5537,7.4974,7.7681,7.9866,8.0805,8.0476,8.1383,8.1493,8.3645,8.5492,8.6568,8.7504,8.8971,9.0909,9.2591,8.97,9.2578,9.5085,9.6742,9.7933,9.9244,10.0733,10.1939,9.0171,7.7927,7.547,7.8812,8.2769,8.5226,8.6585,7.752,6.992,6.9904,7.6456,9.1615,10.5482,11.036,11.0107,10.8046,10.1794,9.8954,9.8294,9.9423,9.7527,9.5873,9.2875,8.6506,8.2595,7.9307,7.9864,8.1267,8.1722,8.0924,8.0252,7.4733,7.1905,7.2119,7.3535,7.6211,7.8387,7.9622,8.07,7.5826,7.1985,7.273,7.4505,7.9414,8.3354,8.6914,9.0108,9.2286,9.0527,8.9311,9.0701,9.4009,9.6792,9.9,10.0908,10.2557,10.3304,10.4412,10.425,10.1261,9.4438,9.5699,9.9073,10.2181,10.5667,10.6167,10.8704,11.2632,11.5895,11.6949,11.5193,11.324,11.1789,10.9818,10.7174,10.7107,10.7045,10.4165,9.9741,9.7329,9.4746,8.939,8.7507,8.9019,9.1336,9.3716,9.4918,9.4194,9.0115,8.2728,8.0359,8.0047,8.0868,8.183,8.4458,8.5299,7.9745,8.3221,8.7287,8.9592,9.0764,9.139,9.1843,9.1314,8.5962,8.2093,7.8534,7.9211,8.2408,9.6476,11.164,11.4499,12.059,12.0129,11.7037,11.3597,11.1274,10.9087,10.7446,10.4878,9.9478,9.8404,9.7834,9.7255,9.457,9.2977,9.6047,10.0148,10.369,10.2371,10.0883,10.1058,10.1135,10.1001,10.0576,9.9924,10.0037,10.0479,10.1744,10.3339,10.1631,10.0986,10.2288,10.4135,10.9023,11.36,11.714,11.6861,11.3676,11.5086,12.1808,12.3016,11.1135,10.593,10.3666,10.0778,9.9408,9.7971,9.9331,10.5314,10.8854,11.185,11.467,11.608,10.5122,9.4518,10.1023,10.4432,10.423,10.1686,10.2123,10.2261,10.272,10.3275,10.5197,10.3025,10.1954,10.2167,10.3181,10.2647,10.1447,10.0121,9.8375,9.5439,8.5314,8.4525,8.2477,8.3476,8.491,8.1396,7.8634,7.8295,7.7556,7.6867,7.6862,7.4256,7.3681,7.5653,7.766,8.0004,8.2226,8.1739,8.1679,8.2932,8.3685,8.3111,8.2947,8.3327,7.8458,7.2328,7.2688,7.4819,7.6093,7.6544,7.6328,6.9688,6.0682,6.0186,6.1785,6.5276,6.7091,6.8391,6.8842,6.8579,6.4124,6.1462,6.1156,6.3512,6.5534,6.4645,6.2774,5.9467,6.1346,6.3953,6.552,6.6563,6.9122,7.1169,7.0559,6.7635,6.5046,6.7394,6.8295,6.9928,6.9015,6.8646,6.7369,6.4552,6.294,6.3819,6.4647,6.7063,6.9692,7.0056,6.9892,6.9099,6.1228,6.1406,6.2489,6.5116,6.7938,7.2407,7.782,8.0864,6.8119,6.7721,7.026,7.4871,7.8047,8.0041,8.149,8.3206,8.4909,8.6046,8.6559,8.6464,8.6253,8.5641,8.4989,8.4245,8.0868,8.0152,7.9609,8.144,8.2803,8.3056,8.244,8.3042,8.4409,8.5896,8.7477,9.0586,9.3807,9.5689,9.6215,9.6981,9.8719,10.0364,10.2083,10.4336,10.6964,10.9576,11.1237,11.225,11.253,10.9335,10.6121,10.5139,10.4388,10.4883,10.6393,10.7446,10.1451,9.4091,9.3336,9.3281,9.1425,9.1725,9.2634,9.4303,9.4352,9.5113,9.5461,9.5575,9.5157,9.4628,9.5292,9.6081,9.5491,9.5663,9.6499,9.6751,9.6736,9.6811,9.6907,9.4607,8.7237,7.9421,8.0612,8.2977,8.4656,8.5224,8.4899,8.3857,8.3294,8.4389,8.5313,8.645,8.7953,8.8688,8.9117,8.9797,8.9864,8.925,8.7771,8.7803,8.8532,8.9569,9.0333,8.8817,7.7227,7.2122,6.8449,6.9645,7.3347,7.708,8.058,8.1918,8.1559,8.6122,8.8443,8.944,8.9874,9.0148,9.0212,9.0741,9.1729,8.9673,8.5573,8.6036,8.6356,8.2951,7.846,7.2965,6.6555,6.588,6.4892,6.461,6.5778,6.717,6.6992,6.311,6.0201,6.3208,6.4658,6.5631,6.6527,6.7555,6.7963,6.4993,6.4883,6.8008,6.9407,7.2239,7.3703,7.5391,7.5781,7.5253,6.8507,6.8607,7.0377,7.2391,7.4775,7.732,7.9506,8.137,8.3523,8.4719,8.4348,8.3631,8.3263,8.3191,8.332,8.4058,8.5342,8.6066,8.575,8.4789,8.3551,8.2441,8.1724,8.1393,8.1336,8.1513,8.169,8.1779,8.1968,8.2604,8.339,8.2107,8.078,8.0992,8.1684,8.307,8.5274,8.7324,8.8403,8.8621,8.8242,8.7436,8.6296,8.4978,8.3656,8.2451,8.1381,8.0448,7.969,7.9021,7.8307,7.7404,7.5982,7.0665,6.7552,6.5935,5.785,5.7071,5.7187,5.7845,5.9714,6.1347,6.4016,6.3676,6.5631,7.7625,8.4605,8.2491,7.4414,7.5082,8.1629,8.4995,8.9437,9.1406,9.2388,9.3056,9.3504,9.3838,9.4143,9.1492,8.1117,7.9293,8.0022,8.2866,8.5164,8.6355,8.6905,8.4313,7.5425,7.4076,7.4995,7.5881,7.9163,7.8136,7.6468,7.7999,7.4718,7.3462,7.2913,7.2276,7.443,7.5504,7.5263,6.9903,6.4495,6.4686,6.4392,6.4681,6.4361,6.5691,6.5878,6.4907,6.6873,6.8641,7.0533,7.3574,7.6288,7.7979,8.0008,7.9074,7.3185,7.3436,7.229,7.5786,7.6458,7.6258,7.6052,7.5635,6.9471,6.7772,6.6932,6.733,6.9386,7.1929,7.3346,7.1264,6.2919,6.3382,6.6205,6.9694,7.2215,7.4813,7.5558,7.2569,6.9618,7.1115,7.1915,7.4456,7.7004,8.0454,8.501,8.89,8.9727,9.1381,9.2479,9.2468,9.2076,9.1175,7.6716,6.7856,6.5658,7.2196,7.3859,7.8471,8.4109,8.5982,8.6846,8.7608,8.766,8.7128,8.6994,8.6666,8.6181,8.5581,8.485,8.4085,8.3332,8.1763,7.9098,7.9117,7.8511,7.7818,7.9239,7.8804,7.4609,7.8391,8.2473,8.598,8.9335,9.2254,9.4233,9.548,9.6136,9.6483,9.6196,9.5798,9.5469,9.497,9.425,9.3439,9.1364,8.9695,8.9178,8.9909,9.0507,8.9115,8.8915,9.0459,9.1402,9.1379,9.0336,9.1743,9.3617,9.3822,9.3782,9.0922,8.0234,7.6394,7.7687,8.0919,8.3556,8.635,8.9795,9.3117,9.584,9.7644,9.8007,9.7316,9.6161,9.0323,8.753,8.9114,9.0533,9.1655,9.2578,9.3066,9.3143,9.2905,9.2358,9.1417,8.4897,7.4932,7.3893,7.6289,7.7927,8.0286,8.2103,8.364,8.4222,8.4049,8.3528,8.2783,8.2063,8.1515,8.1201,8.1067,8.0336,7.9605,7.9497,7.7881,7.705,7.8341,8.1517,8.5166,7.904,7.72,7.6858,7.7798,7.9369,8.1011,8.2136,8.2452,7.9065,7.9132,8.065,8.1592,8.2133,8.2411,8.2336,8.1926,8.1302,8.0357,7.8244,7.7521,7.8465,7.9673,8.0536,8.0303,6.6009,5.9771,6.212,6.3698,6.5243,6.6871,6.838,6.9935,7.0616,6.7969,6.6524,6.9685,7.2032,7.4427,7.5312,6.7975,5.5624,5.4928,5.671,5.8793,6.1629,6.3468,6.5813,6.5758,6.2569,6.5079,6.8339,7.0993,7.3318,7.4622,6.3921,6.6139,7.1438,7.5575,7.8101,7.9739,8.0349,8.0505,8.0642,8.0901,8.0847,8.2611,8.4118,8.5466,8.6586,8.7372,8.5797,8.3176,8.4001,7.7905,6.7899,6.9972,7.0933,7.419,7.7854,8.154,8.4625,8.6616,8.8945,9.1014,9.3328,9.6491,9.9802,10.1999,10.2115,10.1078,9.8204,9.708,9.7413,9.7485,9.7305,9.4719,8.3763,7.9756,8.0161,8.2318,8.4336,8.6005,8.7252,8.8146,8.8803,8.952,9.0532,8.9489,8.8332,8.9015,9.0594,9.1624,9.2547,9.3644,9.4817,9.5871,9.664,9.7156,9.7322,9.4553,9.3469,9.4076,9.443,9.4132,9.2588,9.1047,8.9472,8.7131,8.172,7.7233,7.4142,7.2325,6.7291,6.8478,7.307,7.9289,8.4224,8.8256,9.1374,9.3455,9.4726,9.5694,9.6654,9.7394,9.7808,9.7726,9.7408,9.6617,9.3845,8.9515,8.5,8.2907,7.6802,7.325,7.4118,7.492,7.4141,7.4275,7.5967,7.1395,6.3759,6.5336,6.6988,6.9077,7.4225,7.8706,8.2034,8.3872,7.828,7.6755,7.9461,8.1882,8.3796,8.5543,8.6954,8.7829,8.6307,8.4211,8.3278,8.5956,8.9177,9.0595,9.1159,8.8384,8.0074,7.5714,7.4513,7.685,8.123,8.3172,8.6015,8.5051,7.3282,7.4379,7.8282,8.5566,9.5739,10.3739,10.8677,10.9972,10.4959,10.3428,10.4187,10.4958,10.4695,10.3735,10.3116,10.2541,10.1271,10.0245,9.9053,9.7958,9.7007,9.6171,9.5398,9.4463,9.3272,9.2124,9.131,9.0649,8.9895,8.9002,8.8076,8.721,8.6463,8.5867,8.537,8.4806,8.3366,8.3951,8.4728,8.564,8.6511,8.7175,8.6835,8.7774,8.9342,9.1515,9.3477,9.4617,8.6939,8.2758,7.681,7.516,7.6643,7.7241,7.8704,7.9376,6.8286,6.9731,7.3818,7.7941,8.1247,8.3257,8.4449,8.4528,8.2978,8.8343,9.9789,11.0652,11.3038,11.2287,11.1391,11.1651,11.0859,10.9671,10.6545,10.4988,10.547,11.1639,12.7678,14.2121,14.8433,14.8437,14.5798,14.2387,13.878,13.6778,13.5182,13.3729,13.1926,12.9931,12.6975,12.3675,11.9434,11.1822,10.5707,10.2707,9.9706,10.8634,12.0241,12.5927,12.6355,12.4736,12.2362,11.9921,11.7772,11.5775,11.3565,11.1359,10.9439,10.6753,9.9392,8.9357,8.1673,8.4255,8.4773,8.3007,8.2267,8.58,9.0123,9.2238,9.5769,9.8203,10.2357,10.5563,10.6961,10.7361,10.6693,10.6654,10.5404,10.5773,10.9285,10.9662,10.7782,10.5152,10.3034,10.0815,9.8888,9.6727,9.1182,8.4167,8.8154,8.3184,7.9156,8.2003,8.3632,8.5146,8.7044,8.8841,9.0561,9.2036,9.3249,9.4331,9.5466,9.6679,9.8101,9.9897,10.1511,10.1409,10.0555,9.7952,9.8975,9.9866,10.1709,10.4294,10.2921,9.1489,9.3515,9.7639,9.8209,10.0672,10.224,10.4389,10.6484,10.8189,10.9025,10.8903,10.8122,10.6375,10.2729,10.0741,10.0346,10.0488,10.0184,9.9455,9.5456,9.2344,9.6187,9.8465,10.2948,10.7529,11.2296,11.4712,11.5685,11.519,11.3362,11.1154,10.8738,10.6267,10.3965,10.1889,9.9971,9.7857,9.5805,9.3797,9.2087,9.062,9.2337,9.8724,10.3479,10.6346,10.8288,10.9832,11.1218,11.2565,11.3612,11.3658,10.9961,10.3748,9.5476,8.8823,8.4011,8.5523,8.6707,9.2764,9.7651,10.3471,10.9862,11.5083,11.936,11.4378,11.259,11.1973,10.9238,10.7579,10.6236,10.9135,10.9795,10.9529,11.0145,11.1086,11.1734,11.1582,11.0521,10.8763,10.6501,10.4411,10.4733,10.6774,10.7857,10.7767,10.6033,10.6447,10.8158,11.0149,11.2591,11.4962,11.6867,11.7941,11.7837,11.6498,11.5209,11.3549,10.6904,8.3405,7.6956,7.9233,7.7079,7.7972,8.0997,8.209,8.2983,8.2345,8.2825,8.3099,8.0253,7.5705,6.6668,6.3292,6.5939,7.3645,8.5922,8.812,9.3365,10.2282,10.7299,10.8416,10.7928,10.7891,10.6159,10.1284,9.994,10.3316,10.414,10.2597,10.1957,10.265,10.287,10.1153,9.9234,9.8138,9.3797,8.8696,7.8971,8.688,9.891,11.0164,11.5811,11.9139,12.2852,12.3895,12.3758,12.5525,12.5889,12.4663,12.3036,12.122,11.8805,11.6121,11.3418,11.0368,10.7142,10.4215,10.1188,9.7606,9.4185,9.373,9.6708,9.961,10.1653,10.8094,12.7668,14.0567,14.0716,13.7612,13.3117,12.8108,12.3934,11.9593,11.5636,11.2369,10.8698,10.1411,8.1379,6.9183,7.6046,8.2894,8.8684,9.3419,9.7511,10.1015,10.3594,10.4774,10.4843,10.4802,10.5878,10.8869,11.3046,11.8037,12.356,12.6882,12.6627,12.4366,12.0935,11.7878,11.5262,11.2067,10.8867,10.61,10.072,9.4027,9.0685,9.046,8.9585,8.8058,8.8618,8.7965,8.5396,7.9516,7.9726,8.1028,8.1289,7.9379,7.8941,8.2726,8.5973,8.7284,8.7036,8.7279,8.8681,9.0984,9.4144,9.7457,9.6472,9.3885,9.5903,10.0673,10.4455,10.8476,11.316,11.8087,12.0922,11.9519,11.7704,11.6273,11.487,11.1832,10.963,10.1267,8.5158,7.9583,7.9457,7.9468,8.1812,8.4884,8.7897,9.0266,8.9386,8.5731,8.7213,8.9686,9.1312,9.2381,9.3072,9.2866,7.5744,7.7172,10.848,13.3275,14.4294,14.8477,14.7074,14.2019,13.3564,12.5287,11.9186,11.6539,11.4429,11.3609,11.308,11.2337,10.6805,9.1939,8.7015,8.6977,8.6088,8.3811,8.1853,8.0427,7.6442,7.6405,7.6638,7.7257,7.8308,8.2393,8.7849,8.9617,9.397,9.5708,9.6515,9.6752,9.6728,9.6257,9.5321,9.3585,9.1024,8.9138,8.9068,9.0681,9.1155,8.9506,7.3585,6.8193,7.0401,7.1449,7.2952,7.6663,8.1138,8.9061,9.3456,10.1631,12.02,13.4075,13.6751,13.5098,13.2492,12.9949,12.7038,12.3177,11.899,11.5052,10.078,9.0315,8.746,8.8851,8.9774,9.0236,9.0916,9.347,9.5997,9.7277,9.6935,9.7063,9.853,9.9739,9.9976,9.7343,9.1841,8.2686,7.637,7.9204,8.7449,10.443,11.2459,11.4042,11.2826,11.2818,11.3862,11.2688,10.8569,10.3298,9.3882,8.9617,8.6475,8.6255,8.493,8.444,8.4482,8.9086,9.2444,10.6963,10.9884,11.1154,11.1754,11.1367,11.0344,10.892,10.709,10.5272,10.3442,10.1722,10.0645,10.011,9.9633,9.8888,9.7912,9.679,9.5384,9.4076,9.3872,9.5023,9.5588,9.1874,9.281,10.5774,11.0831,11.3299,11.0101,10.2877,9.7344,9.6042,9.1536,8.9056,8.9661,8.5279,7.9885,8.1633,7.9653,8.0378,8.4762,9.253,9.9983,10.7442,11.2137,10.9635,10.7862,10.5868,10.5833,10.4408,10.2529,9.9381,9.6753,9.6392,9.6405,9.6033,9.4017,9.1048,9.7149,10.2888,10.4828,10.3901,10.4207,10.7289,10.953,10.8869,10.7435,10.5509,10.2062,10.3626,10.5839,10.5862,10.2365,10.0328,9.9479,9.8221,9.6779,9.4021,7.6411,7.4647,8.4593,8.9069,8.97,9.0526,8.9545,8.7377,8.4128,8.2632,8.2265,8.2285,7.9956,7.8517,7.8831,8.1708,8.6788,8.7745,9.0977,9.2596,9.6255,10.0394,10.409,10.4808,10.7604,10.9827,10.9573,10.5742,9.9952,9.2047,9.2936,9.5192,9.4975,9.0971,8.3079,7.9371,7.7267,7.8491,8.4361,9.4267,10.0356,10.4546,10.8607,11.2448,11.5566,11.7174,11.7039,11.5887,11.1194,8.5509,8.7085,9.9358,9.9526,9.4743,9.658,10.1877,10.5792,10.7207,10.7446,10.8026,10.9043,10.9467,10.8745,10.7326,10.5661,10.3048,9.9611,9.7377,9.5501,9.4042,9.3054,9.1975,9.0947,9.0713,9.2865,9.6186,9.7714,9.7881,9.7604,9.6906,9.5764,9.4087,9.3088,9.2516,9.0909,8.8271,7.9253,8.0365,8.0798,8.0881,7.9179,7.6521,8.1402,8.0153,7.6094,7.4038,7.4247,7.6048,7.9856,8.0619,7.9308,8.5378,8.9312,9.9034,10.4145,11.5017,11.5129,11.1093,10.5417,10.4785,10.4713,10.5229,9.647,10.7562,11.0759,11.5101,12.0367,12.6051,11.7151,10.7316,11.1192,11.9268,12.4404,13.5703,13.915,13.7191,13.438,13.1814,13.0347,12.7165,12.5779,12.4291,11.798,11.0541,10.2265,11.0436,11.7792,11.9177,12.2087,12.4237,12.7589,12.7732,12.6547,12.5401,12.411,12.2644,12.102,11.9318,11.7774,11.6638,11.5719,11.4685,11.1215,10.4347,9.6318,9.2085,9.1925,9.4517,9.0241,9.1384,8.904,9.0641,9.1767,9.8065,10.6391,11.4481,12.1568,12.743,13.3333,13.6006,13.5685,13.4047,13.1888,12.9464,12.7096,12.499,12.307,12.222,12.4557,12.8387,13.0879,13.3156,13.5646,13.8297,14.0241,14.2051,14.2456,14.1065,13.9847,13.8577,13.5344,12.9267,12.5809,12.3648,11.9433,11.1435,10.7586,10.7349,10.7571,10.9575,11.2295,11.7572,12.1456,12.3556,12.3357,12.2296,12.061,11.4848,11.4959,11.9377,13.2207,14.8635,15.5577,16.0968,15.8307,15.5234,15.2465,15.0156,14.8445,14.6613,14.4529,14.2215,13.9497,13.4435,13.1903,13.0056,12.7029,12.3981,12.1761,11.4143,11.9496,12.7506,13.3278,13.852,13.993,13.971,13.9119,13.9037,13.9448,13.9789,13.7795,13.4263,12.9147,12.152,11.8183,11.8302,11.3954,10.95,9.9688,9.6339,9.2055,8.927,9.5717,10.0355,10.9746,11.5304,11.9236,12.2828,12.6462,13.0126,13.3434,13.696,14.0533,14.3441,12.4765,11.5926,12.0589,11.9484,11.7601,11.5403,11.6777,12.3333,13.0528,13.5067},
                {2.35354,2.39468,2.45756,2.55913,2.66992,2.79343,2.9988,3.42362,3.66602,3.75656,3.69372,3.55869,3.36671,3.17675,3.03673,2.91887,2.87855,2.89741,3.09846,3.26238,3.25497,3.19482,3.09897,2.98373,2.88309,2.83845,2.83788,2.83134,2.73738,2.62302,2.52751,2.52131,2.79786,3.0196,3.22719,3.42808,3.51464,3.55614,3.62393,3.64521,3.61708,3.69987,3.82411,3.94248,3.96409,3.86072,3.7258,3.57155,3.40348,3.26201,3.1425,3.26383,3.62934,3.79384,3.67432,3.95006,4.1405,4.4207,4.55867,4.72121,4.87245,5.3395,5.01079,4.90489,4.33643,3.96221,3.83684,4.12878,5.9051,6.13829,5.43567,5.18039,5.16493,5.35426,4.94353,4.49224,4.21032,4.04533,4.06424,4.32858,4.64002,4.78606,4.89286,5.08703,5.16697,4.90063,4.50732,4.41993,4.23826,3.99863,3.78936,3.58724,3.61562,3.69989,3.80718,3.52904,3.13911,3.03771,2.99828,3.86578,4.71838,4.99366,4.44476,3.91445,3.45099,3.16771,3.06556,3.94566,5.00118,5.32753,4.16203,3.63959,3.4162,3.34608,3.20622,3.07216,3.07742,3.7294,4.4392,4.47647,3.96704,3.40772,3.03504,2.87351,2.83349,2.74854,2.60063,2.44795,2.32163,2.22582,2.1537,2.10985,2.15763,2.40142,2.72623,3.17294,3.31725,3.54136,3.85766,4.6007,5.05083,5.38993,5.71289,5.98142,5.28023,4.63909,4.39636,4.35686,4.42705,4.54153,4.67271,4.69139,4.54092,4.25106,3.95253,3.82421,3.81934,3.80849,3.75623,3.66808,3.53626,3.50978,3.5361,3.4953,3.46002,3.38145,3.25509,3.10578,2.9411,2.77822,2.7214,2.82941,3.21066,3.27506,3.14023,3.45055,4.07584,4.36212,4.17657,3.74828,3.29805,2.91099,2.63801,2.38202,2.19756,2.10529,2.11769,2.05084,1.9861,1.95194,1.89806,1.80442,1.66826,1.56114,1.46825,1.38876,1.32468,1.3052,1.41332,1.35049,1.61337,2.09029,1.9342,1.77267,1.69245,1.64944,1.71306,1.99195,1.92997,1.77031,1.72101,1.83624,1.94279,1.958,1.98472,1.99553,1.91651,1.8649,1.94292,2.08457,2.25706,2.62986,3.19676,3.57599,3.92533,4.64258,4.75552,5.09021,5.89174,6.02498,5.95337,5.23585,4.61938,4.05151,3.67917,3.3998,3.72884,5.87579,4.80474,3.82925,3.37477,3.20076,3.19525,4.46405,5.25157,5.2603,4.93565,5.18355,5.10364,4.60682,4.37019,4.29411,4.16877,4.11183,4.38487,4.45969,4.25837,4.09678,4.14672,4.2745,4.33813,4.30739,4.22146,4.08272,3.86867,3.58991,3.30417,3.15159,3.28502,3.44597,3.37986,3.2278,3.09739,2.96887,2.81901,2.64416,2.45755,2.29641,2.141,2.09125,2.15245,2.16516,2.17267,2.17147,2.15718,2.24469,2.48423,2.80238,2.89085,2.82972,2.83356,3.10025,3.53592,3.88788,4.07379,4.11759,4.05958,4.02076,4.01328,3.88136,3.59673,3.27567,2.99981,2.77578,2.74417,2.7742,2.75065,2.72872,2.72166,2.71611,2.67187,2.58138,2.48538,2.37649,2.26278,2.16487,2.06173,1.95004,1.85631,1.78473,1.72936,1.69544,1.73451,1.81701,1.94935,2.09243,2.2564,2.48754,2.66941,2.76353,2.79596,2.81247,2.79984,2.74834,2.75342,2.85219,3.14126,3.2082,2.97829,2.73603,2.67136,2.61349,2.62801,2.71213,2.96862,3.1282,3.23447,3.12454,2.85217,2.28603,1.96265,1.90616,1.87684,1.886,1.7133,1.4848,1.3502,1.26935,1.20698,1.14908,1.09454,1.19754,1.70701,2.27413,3.13765,3.669,3.42507,3.22076,3.22805,3.19396,2.99216,2.81321,2.69195,2.66939,2.70298,3.10322,3.25651,3.90504,4.53096,5.28714,5.43227,5.23485,4.55777,3.96046,3.541,3.26645,3.05272,2.86352,2.71622,3.08394,3.58418,4.03133,4.15765,4.25224,4.28533,4.19166,4.15124,3.93825,3.92394,3.78618,3.46023,3.28794,3.21448,3.14416,3.05612,2.96382,2.85995,2.77424,2.74223,2.73711,2.68617,2.58359,2.46528,2.35679,2.27763,2.24464,2.24647,2.27272,2.3414,2.41412,2.46561,2.4705,2.4285,2.35834,2.27217,2.175,2.06289,1.9357,1.80141,1.67599,1.6092,1.61138,1.65757,1.70374,1.8047,1.8732,1.88883,1.79407,1.72905,1.69373,1.6704,1.69713,1.76819,1.82067,1.82032,1.77907,1.74594,1.78152,1.87012,1.95453,2.02089,1.97967,1.83848,1.68304,1.5804,1.56759,1.62436,1.67461,1.68361,1.66139,1.62962,1.59554,1.56262,1.50745,1.43113,1.39202,1.39287,1.43124,1.49731,1.53657,1.5551,1.54822,1.53536,1.57459,1.61178,1.63748,1.64815,1.61436,1.54865,1.4757,1.45582,1.56915,1.62317,1.61656,1.57093,1.49649,1.44647,1.41951,1.36011,1.38039,1.44843,1.525,1.52316,1.47929,1.44235,1.45207,1.54207,1.72715,1.96165,2.22361,2.51809,2.79211,2.95737,2.88711,2.6714,2.52536,2.38956,2.25546,2.17407,2.22413,2.20718,2.187,2.16881,2.08643,1.95448,1.8065,1.67314,1.57196,1.50383,1.46331,1.46015,1.51197,1.57533,1.61626,1.65318,1.65373,1.63109,1.64324,1.64839,1.62944,1.75796,2.1459,2.73948,4.10117,4.03586,3.49781,3.57779,3.8833,3.28886,2.85584,2.93752,4.08486,4.64118,6.33453,7.08372,5.99238,5.23455,4.91189,4.8789,4.60733,4.32403,4.04864,4.0362,4.52916,4.33863,4.29297,4.24379,4.07471,3.91844,3.64253,3.36103,3.22981,3.09247,2.99999,2.94325,2.9539,3.07227,3.18296,3.22084,3.38672,4.01908,5.78694,5.75443,5.14794,4.54946,4.07939,3.79802,3.43409,3.26306,3.29101,3.70064,5.1781,4.60672,4.20667,3.85572,3.65987,3.45864,3.2075,3.03656,2.89359,2.77761,2.7255,2.66058,2.60057,2.55159,2.47198,2.3771,2.29484,2.20815,2.07209,1.9497,1.838,1.73197,1.64125,1.63658,1.87109,2.28705,2.62755,3.18583,3.51297,4.18489,3.88809,3.56314,3.55526,3.44554,3.44549,3.52468,3.52861,3.42547,3.47739,3.62949,3.62528,3.50748,3.36126,3.24926,3.57767,5.08784,5.65352,5.60762,6.72691,7.11389,7.36337,6.82069,6.47161,5.93945,5.47158,4.94446,4.50847,4.37019,4.47506,4.31268,4.02409,3.81781,3.74782,3.52177,3.58247,4.10536,4.56574,4.87088,5.17447,5.17091,4.98031,4.79781,4.56385,4.24021,3.92214,3.65507,3.44436,3.56999,3.75997,3.54179,3.39431,3.29574,3.20977,3.17892,2.97979,2.82802,2.7215,2.61383,2.47728,2.31963,2.15093,1.98691,1.84137,1.73178,1.64816,1.60343,1.63349,1.76172,1.89869,2.0275,2.14184,2.12025,2.00905,1.8618,1.77823,1.7053,1.66124,1.72078,1.92505,1.9703,1.89041,1.84692,1.74713,1.63769,1.50289,1.42303,1.41788,1.45998,1.5378,1.58903,1.54489,1.44009,1.3022,1.22074,1.15562,1.10124,1.05293,1.0314,1.02947,1.02032,1.00443,1.03306,1.14651,1.41851,1.72202,1.95343,2.10186,2.2379,2.32476,2.16576,2.20262,2.36042,2.50822,2.62235,2.75191,2.91046,3.04973,3.11905,3.11927,3.07202,2.989,2.87473,2.73597,2.57954,2.41646,2.26421,2.14819,2.07108,2.02561,2.02063,2.02504,2.00386,1.94716,1.89737,1.91878,2.02719,2.15202,2.22679,2.21098,2.23519,2.33131,2.32365,2.33508,2.34043,2.34249,2.35016,2.35969,2.3558,2.32121,2.33279,2.42823,2.4894,2.65399,2.86546,2.97739,2.88007,2.91052,2.77148,3.37835,4.09842,4.40984,3.8829,3.80302,4.06027,4.06817,4.20483,4.59576,5.00725,5.06637,4.87428,4.60705,4.35794,4.23049,4.0702,3.92171,3.79508,3.75411,3.63567,3.44279,3.2433,3.06259,2.91374,2.79653,2.69442,2.5802,2.41802,2.22724,2.19212,2.47348,2.90754,2.96683,2.7916,2.80509,2.94236,3.22245,3.50855,3.6086,3.59829,3.51364,3.34195,3.12716,2.93017,2.75561,2.59668,2.45475,2.33592,2.25595,2.43677,2.66681,2.66029,2.49583,2.31846,2.19808,2.18397,2.26777,2.29621,2.30767,2.34764,2.365,2.30537,2.20251,2.07358,1.96784,1.86298,1.78781,1.80408,1.78304,1.72657,1.86236,2.06249,2.26584,2.52766,2.64845,2.57815,2.49145,2.42245,2.37708,2.39253,2.34119,2.21255,2.07699,1.9457,1.88986,2.05994,2.74879,2.98588,3.29141,3.41938,3.34361,3.27923,3.15282,2.98249,2.8489,2.78269,2.73627,2.71482,2.7335,2.7582,2.75161,2.70436,2.67085,2.99464,3.36994,3.21809,3.21415,3.06325,3.22364,3.19478,3.19535,3.2212,3.2136,3.17773,3.13299,3.07396,2.99096,2.8954,2.79515,2.67793,2.53047,2.36633,2.21207,2.07066,1.91823,1.74285,1.60466,1.66481,1.9082,2.08801,2.19285,2.21436,2.15273,2.0368,1.90732,1.89506,1.93861,1.9526,1.99119,2.00895,1.97024,1.88522,1.80056,1.74467,1.71724,1.66438,1.60073,1.55952,1.54722,1.54397,1.53399,1.54005,1.55383,1.56525,1.57605,1.58557,1.60488,1.62923,1.6452,1.64128,1.62268,1.59445,1.5556,1.50636,1.46462,1.44442,1.43847,1.43747,1.4594,1.52489,1.66106,1.77713,1.74647,1.68319,1.65802,1.62764,1.60203,1.58905,1.57667,1.56611,1.56017,1.54897,1.52191,1.48466,1.44817,1.42059,1.40527,1.39767,1.38626,1.37547,1.36999,1.38304,1.40952,1.40704,1.3948,1.37858,1.3595,1.34562,1.3454,1.47659,1.63075,1.81255,1.81614,1.70894,1.62179,1.56064,1.52755,1.48872,1.45505,1.43542,1.42673,1.42537,1.42576,1.41725,1.40787,1.43981,1.52735,1.61451,1.61274,1.53941,1.50086,1.5773,1.65753,1.78597,1.72586,1.71535,1.76736,1.76279,1.71439,1.71897,1.76829,1.88715,2.0588,2.24034,2.3172,2.31443,2.28039,2.233,2.18394,2.12161,2.03647,1.93603,1.85689,1.82126,1.82064,1.82652,1.82019,1.81006,1.80226,1.79165,1.81518,1.87956,1.94563,1.97964,1.97602,1.97829,1.97146,2.10326,2.25122,2.44968,2.62984,2.67575,2.60557,2.50366,2.41752,2.44163,2.55132,2.63458,2.64969,2.64355,2.662,2.93018,3.27102,3.52165,3.03246,2.6765,2.51444,2.53639,2.67021,2.81581,2.90333,2.97219,3.02826,3.07453,3.10621,3.10633,3.05327,2.95076,2.83304,2.73799,2.7109,2.70685,2.73897,2.57931,2.63373,2.79309,2.73517,2.66991,2.50687,2.3122,2.16649,2.00114,1.96866,2.16908,2.12308,2.04809,2.00369,2.02233,2.07858,2.10132,2.02634,2.02172,2.0808,2.1247,2.14102,2.16272,2.15591,2.14076,2.10869,2.10103,2.16421,2.18987,2.15983,2.09053,2.01939,1.97768,1.9631,1.95819,1.95537,2.00366,2.05884,2.01477,1.94713,1.9055,1.90889,2.10368,2.22849,2.28307,2.22289,2.09803,2.01745,1.97369,2.03377,2.20539,2.24998,2.13351,1.96883,1.82571,1.71357,1.62228,1.57783,1.61,1.56751,1.51713,1.49398,1.49505,1.50389,1.50401,1.49689,1.52396,1.46912,1.46128,1.49266,1.5382,1.57764,1.62578,1.74862,1.93005,1.97265,1.98398,1.98759,2.06642,2.02972,1.8867,1.81194,1.83809,1.85836,1.82855,1.80228,1.79726,1.77127,1.74477,1.73141,1.85612,1.89827,1.90069,1.8092,1.72956,1.68423,1.69645,1.68306,1.64508,1.57213,1.5091,1.45855,1.41667,1.37958,1.34779,1.32778,1.35655,1.31523,1.26876,1.23323,1.21071,1.19455,1.18071,1.17129,1.28133,1.48493,1.56921,1.51565,1.43216,1.36289,1.29641,1.38862,1.58891,1.71477,1.80591,1.91909,2.20352,2.56412,2.81932,2.89392,2.96448,2.91756,2.78921,2.62161,2.52043,2.42624,2.36705,2.45406,2.5451,2.58005,2.48884,2.31023,2.1829,2.11592,2.02422,2.08559,2.224,2.19906,2.20675,2.14512,2.10603,2.0588,1.9596,2.02219,2.28483,2.29892,2.27099,2.10574,1.99175,1.89229,1.78587,1.70426,1.69283,1.68434,1.63618,1.559,1.50063,1.46561,1.44732,1.43662,1.42829,1.40251,1.36757,1.35705,1.47622,1.68871,2.06008,2.33201,2.40143,2.48838,2.56064,2.67138,2.81644,2.94528,3.01323,3.0012,2.95794,2.95195,3.00568,3.04474,3.04784,3.06,3.10451,3.08366,3.07427,3.26653,3.46882,3.43589,3.26269,3.05221,2.87793,2.64344,2.4377,2.31702,2.09886,1.86261,1.67688,1.57606,1.55719,1.63621,1.8852,1.91446,1.84208,1.75645,1.6695,1.58683,1.51718,1.46862,1.48176,1.48192,1.5343,1.63737,1.87679,2.26373,3.10313,3.90327,3.71413,3.4188,3.1645,3.02747,2.95007,2.8761,2.82251,2.81762,2.94372,3.01885,2.94609,2.91688,3.02062,3.20362,3.20484,3.13191,3.09184,3.11273,3.07438,2.89378,2.67433,2.46574,2.28754,2.14533,2.02663,1.93589,1.8708,1.84437,1.90178,1.94402,1.90902,1.86833,1.81281,1.7877,1.78785,1.86331,2.01572,2.05681,1.94889,1.96785,2.23114,2.41643,2.52342,2.764,2.82471,2.8154,2.77909,2.61213,2.47419,2.35002,2.24287,2.17054,2.30021,2.68409,2.55049,2.47741,2.50127,2.58442,2.60528,2.61961,2.62128,2.60287,2.5444,2.55826,2.62639,2.6916,2.64695,2.53517,2.39074,2.24885,2.12435,2.02276,2.10689,2.01434,1.95738,1.87645,1.76389,1.72296,1.71661,1.66045,1.60222,1.54199,1.46791,1.43127,1.37063,1.28251,1.2221,1.19037,1.17793,1.1903,1.19135,1.17285,1.16254,1.17118,1.18281,1.19692,1.28876,1.45652,1.50258,1.44689,1.36447,1.28619,1.22409,1.27382,1.51385,1.57197,1.53518,1.43602,1.38612,1.38476,1.40244,1.41457,1.52244,1.63474,1.62096,1.45249,1.30846,1.26684,1.31125,1.60609,1.9881,2.14207,2.14436,2.12358,2.04323,1.88461,1.75111,1.77398,1.92948,1.84519,1.70239,1.5674,1.52755,1.52094,1.52836,1.65611,1.83633,1.91776,1.90608,1.73193,1.53744,1.43126,1.37228,1.34131,1.59236,1.76134,1.77569,1.63341,1.47681,1.34874,1.25903,1.21718,1.44709,1.63997,1.62582,1.51866,1.43454,1.41844,1.40958,1.3586,1.32106,1.32567,1.36717,1.43919,1.45634,1.43668,1.41044,1.39504,1.45526,1.51522,1.61219,1.64795,1.60588,1.55987,1.54331,1.54931,1.57423,1.57872,1.56005,1.553,1.55661,1.54612,1.54639,1.55945,1.55255,1.51904,1.47441,1.43623,1.41859,1.41642,1.41768,1.40333,1.37116,1.35791,1.36108,1.37143,1.40357,1.42926,1.42732,1.40306,1.42829,1.483,1.47227,1.46656,1.5047,1.5221,1.51654,1.48607,1.45488,1.40846,1.36135,1.31703,1.28016,1.2472,1.20706,1.16814,1.14191,1.11297,1.08068,1.05247,1.02808,1.00978,0.99509,1.001,1.07122,1.1845,1.22106,1.22025,1.21374,1.21741,1.22862,1.2478,1.23885,1.20691,1.18545,1.17145,1.16278,1.16686,1.17528,1.17338,1.16774,1.16661,1.17305,1.16759,1.15364,1.13307,1.11186,1.1077,1.22078,1.28652,1.35165,1.31093,1.23204,1.16381,1.11463,1.09687,1.10416,1.06523,1.04639,1.0338,1.02043,1.00751,1.00065,0.99987,1.01029,1.10175,1.27256,1.32842,1.35281,1.43015,1.51456,1.60055,1.76056,1.74356,1.75602,1.73608,1.66364,1.56524,1.50019,1.57129,2.03618,2.13591,2.10649,2.00836,1.93739,1.89253,1.78972,1.88821,2.22504,2.28036,2.25971,2.15454,2.0042,1.82943,1.6415,1.49002,1.56333,1.63746,1.62669,1.60802,1.61247,1.65667,1.6733,1.65469,1.70521,1.74245,1.70444,1.66769,1.65092,1.64217,1.609,1.58652,1.60144,1.61958,1.5964,1.52713,1.43983,1.35832,1.29453,1.254,1.23233,1.21812,1.20284,1.18055,1.16003,1.16206,1.19092,1.23188,1.24021,1.21176,1.18712,1.19411,1.24808,1.32927,1.39212,1.4159,1.40934,1.38173,1.33794,1.28606,1.23426,1.18955,1.15178,1.11818,1.08656,1.05035,1.00838,0.9655,0.92621,0.94005,0.9631,0.96699,1.12077,1.19227,1.21695,1.19222,1.13331,1.07856,1.00173,0.97425,1.02132,1.1184,1.3292,1.54742,1.84798,2.05907,2.00842,1.93379,1.85169,1.82139,1.79659,1.76508,1.73109,1.69637,1.6627,1.66617,1.82672,1.86038,1.81238,1.72222,1.65488,1.62683,1.62037,1.66965,1.92251,2.05592,2.06068,2.01124,1.87337,1.81799,1.85218,1.84569,1.86956,1.87089,1.83251,1.80372,1.73795,1.68058,1.65096,1.75746,2.08063,2.15675,2.16075,2.03716,1.98748,1.96606,1.99737,2.16851,2.47093,2.53161,2.48393,2.31424,2.17546,2.02085,1.86024,1.79084,1.90326,1.86365,1.85154,1.71108,1.62674,1.59713,1.5772,1.55233,1.64937,1.67679,1.66625,1.58707,1.47088,1.36424,1.29391,1.3059,1.59037,1.80024,1.91005,1.92014,1.9477,2.00206,2.00839,2.12607,2.45637,2.50335,2.44771,2.21817,2.06869,2.00167,1.94581,1.88302,1.84182,1.77121,1.69957,1.64014,1.58817,1.54666,1.70375,1.83754,2.19974,2.13511,2.06093,2.0529,2.21496,2.15723,2.0499,2.02633,2.04693,2.05166,2.02705,1.97623,1.90463,1.82156,1.73476,1.64982,1.56979,1.5048,1.45391,1.36671,1.31121,1.30671,1.31979,1.37776,1.51391,1.52336,1.52873,1.55748,1.5975,1.63829,1.67062,1.68892,1.70237,1.71588,1.72807,1.73141,1.72185,1.69307,1.64448,1.58241,1.52935,1.47588,1.41677,1.35745,1.34468,1.46927,1.61774,1.70509,1.77593,1.84774,1.91595,1.90844,1.85858,1.80594,1.73918,1.69489,1.78777,1.77812,1.63371,1.46086,1.3449,1.31492,1.39495,1.53405,1.65433,1.72662,1.77085,1.79041,1.77401,1.81976,1.85433,1.82701,1.81105,1.80835,1.80291,1.78562,1.75606,1.72098,1.68339,1.64233,1.67186,1.82942,1.84518,1.85109,1.84405,1.79306,1.74662,1.69617,1.63872,1.57524,1.50911,1.44429,1.38065,1.31884,1.25974,1.20374,1.15507,1.10323,1.0719,1.13065,1.21971,1.2587,1.32792,1.51283,1.91279,2.16076,2.18003,2.02054,1.83426,1.6688,1.53858,1.44028,1.40946,1.34861,1.27636,1.21849,1.17049,1.12924,1.09324,1.06049,1.03031,1.0079,1.0264,1.02343,0.97407,0.92253,0.88235,0.85918,0.99716,1.1564,1.15697,1.12812,1.09204,1.04422,0.99088,0.93606,0.88888,0.88165,0.88167,0.84059,0.81774,0.80456,0.80991,0.89631,1.21023,1.4195,1.52482,1.44146,1.27111,1.13667,1.03466,0.98575,0.99415,0.93319,0.86975,0.82257,0.78503,0.76596,0.89499,1.07069,1.24979,1.22038,1.16417,1.13223,1.11489,1.09927,1.08436,1.07059,1.06043,1.03467,1.01146,0.98833,0.96388,0.93874,0.93242,0.94855,0.95328,1.08727,1.55487,1.68451,1.58386,1.40811,1.32351,1.3295,1.27315,1.18401,1.11544,1.08161,1.07119,1.07139,1.07235,1.06621,1.05471,1.04036,1.04835,1.0841,1.15667,1.24161,1.29714,1.33485,1.46366,1.53924,1.53304,1.49614,1.47669,1.46436,1.43157,1.37948,1.3326,1.30823,1.31159,1.34364,1.37528,1.3698,1.33834,1.31117,1.2901,1.2731,1.26109,1.24954,1.24103,1.24023,1.24979,1.29018,1.31895,1.32594,1.32698,1.32325,1.3235,1.31816,1.30867,1.30483,1.34335,1.41312,1.47278,1.47438,1.60778,1.56991,1.42841,1.34776,1.33746,1.33954,1.34355,1.34819,1.35516,1.35733,1.35074,1.33458,1.31001,1.28425,1.2574,1.23677,1.23979,1.2662,1.30799,1.3139,1.35938,1.38767,1.33572,1.2881,1.28444,1.29932,1.31137,1.43732,1.82668,1.90597,1.85011,1.75068,1.5786,1.46614,1.41727,1.39757,1.48528,1.54801,1.4951,1.43124,1.38364,1.35171,1.3335,1.32635,1.35016,1.39212,1.432,1.43502,1.43701,1.45527,1.46376,1.49355,1.61647,1.70702,1.73105,1.64621,1.53272,1.48689,1.45657,1.4808,1.79665,1.84992,1.83687,1.81616,1.80715,1.86464,1.94854,2.02956,2.14419,2.17115,2.11233,2.02566,1.94743,1.88749,1.83184,1.77607,1.72536,1.66789,1.61241,1.56099,1.51255,1.46871,1.437,1.42431,1.45688,1.54435,1.59513,1.57826,1.52054,1.44024,1.35812,1.28844,1.23401,1.19204,1.15865,1.12727,1.10019,1.05644,1.01933,0.98775,0.95847,0.93203,0.91728,0.90225,0.89667,0.91187,0.94862,0.99147,1.09034,1.15882,1.24783,1.28367,1.25078,1.22364,1.20464,1.21848,1.50637,1.628,1.6127,1.57391,1.52222,1.453,1.37866,1.34443,1.46137,1.77467,2.1084,2.26687,2.33582,2.32855,2.26748,2.17798,2.09496,2.03279,2.01458,1.98636,1.94511,1.92156,1.9963,2.23631,2.51391,2.74612,2.94663,3.07964,3.10208,3.02085,2.93556,2.85995,2.76531,2.63231,2.47384,2.30667,2.17714,2.18451,2.16295,2.08647,2.191,2.39343,2.74601,3.05479,3.14604,3.08766,2.97974,2.8793,2.82154,2.79118,2.75393,2.69507,2.61686,2.53587,2.53147,2.61159,2.91145,2.58839,2.53741,2.61401,2.68892,2.57402,2.43427,2.39607,2.41592,2.53363,2.58259,2.60126,2.63033,2.62758,2.61428,2.63231,2.79418,3.10504,3.38194,3.39562,3.30243,3.14472,2.90617,2.67007,2.4625,2.28205,2.17839,2.14289,1.90325,1.88944,2.05031,2.03738,2.00685,1.9656,1.93034,1.89761,1.86816,1.84361,1.81759,1.79791,1.80244,1.81631,1.80886,1.77207,1.77058,1.86725,1.92838,1.94926,1.94041,1.99936,2.05246,2.10133,2.24237,2.75171,3.10444,3.19048,3.32138,3.31402,3.27918,3.21121,3.11325,3.00091,2.88128,2.74819,2.61503,2.49393,2.39725,2.27592,2.13562,1.99603,1.86298,1.72334,1.61713,1.59581,1.66316,1.80868,2.03661,2.43732,2.68348,2.80189,2.90098,2.93153,2.88526,2.81859,2.73151,2.59329,2.4261,2.26537,2.13044,2.00856,1.87065,1.7262,1.57468,1.53819,1.82886,2.19869,2.42273,2.49348,2.50594,2.50439,2.50364,2.49832,2.47876,2.44027,2.42412,2.48538,2.62903,2.80672,3.07416,3.08913,3.06795,2.75715,2.56744,2.65644,2.87249,3.10431,3.27134,3.56425,3.75509,3.81496,3.89582,3.85726,3.8385,3.71808,3.57797,3.42035,3.27081,3.15103,3.04715,2.92462,2.76998,2.58742,2.37189,2.16158,2.16892,2.36054,2.45077,2.43296,2.40819,2.37918,2.36696,2.37343,2.39178,2.41559,2.44024,2.45379,2.43456,2.37586,2.28336,2.16726,2.09318,2.46517,3.15195,3.03348,3.11907,2.77514,2.48929,2.31898,2.15463,2.02158,1.8678,1.72743,1.63934,1.59311,1.71856,1.9592,2.39362,2.80617,2.53965,2.57455,2.65227,2.67619,2.75306,2.78902,2.78518,2.73677,2.71611,2.77278,2.77656,2.67352,2.57989,2.4769,2.32503,2.1389,1.95794,1.78864,1.62387,1.50896,1.50836,1.56888,1.93233,1.92153,2.03056,2.31854,2.7604,3.3627,3.89675,4.20718,4.29186,4.18861,4.09375,4.03555,3.94365,3.75309,3.5002,3.24303,2.99948,2.75185,2.52141,2.32005,2.11378,1.88824,1.67583,1.62769,1.79514,1.9377,1.98503,2.06186,2.47888,3.1805,3.62546,3.70857,3.55479,3.33743,3.145,2.92619,2.73431,2.55948,2.35422,2.17598,2.28929,2.87744,3.68511,3.90919,3.38292,3.02608,2.91057,2.896,2.87813,2.82116,2.72898,2.63144,2.56606,2.57025,2.64862,2.8067,3.04746,3.2468,3.27291,3.17376,3.0338,2.86958,2.7073,2.56913,2.44509,2.32182,2.24229,2.20816,2.12943,1.98692,1.86215,1.76946,1.66343,1.58627,1.5537,1.60069,1.54405,1.43165,1.34274,1.3139,1.30208,1.25862,1.22589,1.20823,1.20182,1.20122,1.19543,1.1889,1.18893,1.19537,1.24402,1.32322,1.37452,1.43105,1.54651,1.68589,1.76004,1.80203,1.84138,1.87731,1.89294,1.88607,1.87153,1.86769,1.83176,1.84771,2.07763,2.19915,2.05559,1.92121,1.97387,2.01488,1.96539,1.92726,1.93054,1.94122,1.82351,1.67189,1.55489,1.47139,1.41195,1.37652,1.58969,1.91759,2.98508,4.22588,4.99569,5.26797,5.17053,4.83,4.53639,4.25695,3.96236,3.70026,3.47423,3.32595,3.18824,3.04303,2.99051,3.26478,3.53506,3.30289,3.08102,2.88837,2.79034,2.74082,2.98858,3.00814,3.18218,3.34838,3.73592,4.0461,4.01679,3.81272,3.33362,3.0485,2.85198,2.74422,2.66627,2.57658,2.45675,2.32737,2.21923,2.14696,2.13127,2.13895,2.12453,2.06166,2.36139,2.71314,2.83441,3.13419,3.57735,3.89681,4.64257,4.13673,3.52761,3.40664,3.87985,4.60274,4.78809,4.57518,4.29506,4.07828,3.85669,3.58573,3.31145,3.05524,3.01527,3.02969,2.77459,2.49916,2.30026,2.1597,2.04281,1.9511,1.9205,1.90264,1.87361,1.84977,1.84185,1.82481,1.78755,1.77358,1.80608,2.00082,2.36524,2.5854,3.11131,3.08139,3.13164,3.08593,2.96677,2.93158,3.02552,3.03024,2.85318,2.56466,2.38359,2.33106,2.35745,2.31877,2.36864,2.57747,2.90852,3.37281,4.83544,6.33154,5.29159,4.45894,3.91888,3.57043,3.28801,3.02356,2.79911,2.61389,2.4482,2.31538,2.24669,2.22704,2.20553,2.15113,2.07625,1.98653,1.87864,1.7628,1.67012,1.62443,1.62723,1.68587,1.67708,1.60005,1.6815,1.82928,2.01307,2.26875,2.52092,2.76815,3.00962,3.14335,2.81821,2.66516,2.80524,2.57006,2.50753,2.48324,2.47806,2.60286,2.76274,2.93548,3.22789,3.72053,3.91251,4.4039,4.33727,4.04672,3.80802,3.66414,3.64248,3.53123,3.44715,3.45815,3.54483,3.87796,3.7747,3.7097,3.64818,3.6357,3.64313,3.55353,3.41183,3.31736,3.27441,3.29256,3.40717,3.47384,3.45263,3.31881,3.15623,2.93273,2.70182,2.49227,2.29768,2.1584,2.48845,3.00714,3.38899,3.46242,3.31538,3.09855,2.95013,2.96697,3.139,3.2503,3.32508,3.29276,3.4218,3.61543,4.0453,4.14054,4.34738,4.70116,4.59718,4.36396,3.83149,3.69235,3.87307,4.11811,4.15635,4.05423,3.90897,3.76859,3.66857,3.75278,3.52965,3.10736,2.81917,2.67499,2.7485,2.81758,3.00532,3.23908,3.41493,3.41796,3.39969,3.3193,3.30107,3.31159,3.34742,3.35745,3.28411,3.14052,3.01255,3.69081,5.57815,5.02397,4.08932,4.06908,4.05191,3.94047,3.86545,3.68495,3.46426,3.30075,3.20817,3.11345,2.96834,2.78753,2.59677,2.41723,2.27322,2.13478,2.05403,1.98726,1.90387,1.81832,1.73423,1.67566,1.68684,1.77017,1.83608,1.851,1.82951,1.78736,1.74535,1.71308,1.67989,1.63426,1.6185,1.63653,1.67288,1.53928,1.43649,1.36408,1.35858,1.43276,1.50992,1.90447,2.29388,2.60684,2.90464,3.18254,3.22354,2.9218,3.16495,3.36885,4.02568,4.34996,5.61685,6.46134,6.72191,5.8194,6.03303,5.95996,5.15179,4.60311,5.68168,5.66626,5.18588,4.46804,4.13825,4.1009,4.34552,5.11131,7.28896,8.42045,8.94331,9.07936,8.31827,6.95447,5.92502,5.33296,4.96295,4.72002,4.48911,4.35783,4.4905,4.78057,5.66676,5.34689,5.05518,5.08365,4.94247,5.00125,5.05184,4.85766,4.56527,4.30391,4.06723,3.84589,3.64031,3.47165,3.34819,3.24806,3.15072,3.04132,2.94195,2.84306,2.684,2.74009,2.85365,2.8152,2.86051,2.70276,2.70645,2.78601,2.95055,3.13836,3.41891,3.41967,3.36985,3.49082,3.89784,4.22845,4.29337,4.20299,4.02699,3.80834,3.57627,3.33955,3.09518,2.87494,2.82298,3.06689,3.34813,3.46372,3.48987,3.51046,3.52818,3.55278,3.50713,3.3586,3.17267,3.02406,3.03995,3.17642,3.20217,3.06197,2.88914,2.77594,2.70576,2.74509,2.89071,3.01843,3.087,3.15823,3.25404,3.26933,3.17068,3.02074,2.84729,2.70922,2.54935,2.43935,2.43847,2.62538,3.07615,3.48759,3.90226,4.19132,4.33781,4.38664,4.3308,4.22001,4.06524,3.86671,3.65556,3.51146,3.31943,3.03829,2.70489,2.38601,2.16397,2.15673,2.20214,2.43219,2.81951,3.28181,3.6548,3.78701,3.7544,3.64456,3.53678,3.46611,3.43017,3.41656,3.45079,3.5133,3.4772,3.37015,3.42408,3.51321,3.70536,3.69461,3.63807,4.17712,4.27992,4.11597,3.88436,3.85452,3.8395,3.78275,3.72338,3.69432,3.67481,3.66566,3.69924,3.76771,4.34039,4.97672,4.88419,4.88848,4.85755,4.83878,4.65337,4.40868,4.47692,4.59119}
            };
            std::vector<double> wave_height_input = as_vector_double("wave_significant_height");
            std::vector<double> wave_period_input = as_vector_double("wave_energy_period");
            ssc_number_t* energy_hourly = allocate("hourly_energy", 2920);
            ssc_number_t* sig_wave_height_index_mat = allocate("sig_wave_height_index_mat", 2920);
            ssc_number_t* energy_period_index_mat = allocate("energy_period_index_mat", 2920);
            ssc_number_t* wave_power_index_mat = allocate("wave_power_index_mat", 2920);
            double ts_significant_wave_height, ts_energy_period;
            /* Bilinear interpolation of power matrix for time series analysis*/
            double Q11, Q12, Q21, Q22;
            double x1, x2, y1, y2;
            double a0, a1, a2, a3;
            Q11 = wave_power_matrix.at(1, 1);
            Q12 = wave_power_matrix.at(1, wave_power_matrix.ncols() - 1);
            Q21 = wave_power_matrix.at(wave_power_matrix.nrows() - 1, 1);
            Q22 = wave_power_matrix.at(wave_power_matrix.nrows() - 1, wave_power_matrix.ncols() - 1);
            x1 = wave_power_matrix.at(1, 0);
            x2 = wave_power_matrix.at(wave_power_matrix.nrows() - 1, 0);
            y1 = wave_power_matrix.at(0, 1);
            y2 = wave_power_matrix.at(0, wave_power_matrix.ncols() - 1);

            a0 = (Q11 * x2 * y2) / ((x1 - x2) * (y1 - y2)) + (Q12 * x2 * y1) / ((x1 - x2) * (y2 - y1)) + (Q21 * x1 * y2) / ((x1 - x2) * (y2 - y1)) + (Q22 * x1 * y1) / ((x1 - x2) * (y1 - y2));
            a1 = (Q11 * y2) / ((x1 - x2) * (y2 - y1)) + (Q12 * y1) / ((x1 - x2) * (y1 - y2)) + (Q21 * y2) / ((x1 - x2) * (y1 - y2)) + (Q22 * y1) / ((x1 - x2) * (y2 - y1));
            a2 = (Q11 * x2) / ((x1 - x2) * (y2 - y1)) + (Q12 * x1) / ((x1 - x2) * (y1 - y2)) + (Q21 * x2) / ((x1 - x2) * (y1 - y2)) + (Q22 * x1) / ((x1 - x2) * (y2 - y1));
            a3 = (Q11) / ((x1 - x2) * (y1 - y2)) + (Q12) / ((x1 - x2) * (y2 - y1)) + (Q21) / ((x1 - x2) * (y2 - y1)) + (Q22) / ((x1 - x2) * (y1 - y2));

            double diff_sig_wave_height, diff_energy_period;
            double sig_wave_height_index, energy_period_index;
            for (size_t i = 0; i < 2920; i++) {
                //ts_significant_wave_height = wave_resource_time_series[1][i];
                ts_significant_wave_height = wave_height_input[i];
                //ts_energy_period = wave_resource_time_series[0][i];
                ts_energy_period = wave_period_input[i];
                for (size_t j = 0; j < (size_t)wave_power_matrix.nrows(); j++) {
                    if (abs(ts_significant_wave_height - wave_power_matrix.at(j, 0)) < 0.25) {
                        sig_wave_height_index = j;
                        sig_wave_height_index_mat[i] = sig_wave_height_index;
                    }
                }
                for (size_t m = 0; m < (size_t)wave_power_matrix.ncols(); m++) {
                    if (abs(ts_energy_period - wave_power_matrix.at(0, m)) < 0.50) {
                        energy_period_index = m;
                        energy_period_index_mat[i] = energy_period_index;
                    }
                }

            
                energy_hourly[i] = (ssc_number_t)(wave_power_matrix.at(sig_wave_height_index, energy_period_index)) * 3;
                sig_wave_height_index_mat[i] = (ssc_number_t)(wave_power_matrix.at(sig_wave_height_index, 0));
                energy_period_index_mat[i] = (ssc_number_t)(wave_power_matrix.at(0, energy_period_index));
                wave_power_index_mat[i] = (ssc_number_t)(wave_power_matrix.at(sig_wave_height_index, energy_period_index));
                annual_energy += energy_hourly[i];
                device_average_power += energy_hourly[i] / 8760;
                //To do: bilinear interpolation of power matrix to interpolate at each time step

            }
        }


        

		/*
		//Throw exception if default header row (of power curve and resource) does not match user input header column:
		std::vector<double> _check_header{ 0, 0.5, 1.5, 2.5, 3.5, 4.5, 5.5, 6.5, 7.5, 8.5, 9.5, 10.5,	11.5, 12.5,	13.5, 14.5, 15.5, 16.5, 17.5, 18.5, 19.5, 20.5 };
		if (_check_header != wave_resource_matrix[0])
			throw compute_module::exec_error("mhk_wave", "Time period bins of resource matrix don't match. Reset bins to default");
		if (_check_header != wave_power_matrix[0])
			throw compute_module::exec_error("mhk_wave", "Time period bins of wave power matrix don't match. Reset bins to default"); */


		//Factoring in losses in total annual energy production:
		//Factoring in losses in total annual energy production:
		annual_energy *= (1 - (total_loss / 100));
		// leave device average power without losses
		annual_energy *= number_devices;

        //TEST cost metrics in tidal page rather than cost page
        double device_cost = as_double("device_costs_total");
        double bos_cost = as_double("balance_of_system_cost_total");
        double financial_cost = as_double("financial_cost_total");
        double om_cost = as_double("total_operating_cost");
        double fcr = as_double("fixed_charge_rate");
        double total_capital_cost_kwh = fcr*(device_cost + bos_cost + financial_cost) / annual_energy;
        double total_device_cost_kwh = fcr*device_cost / annual_energy;
        double total_bos_cost_kwh = fcr*bos_cost / annual_energy;
        double total_financial_cost_kwh = fcr*financial_cost / annual_energy;
        double total_om_cost_kwh = om_cost / annual_energy;
        double total_capital_cost_lcoe = (fcr * (device_cost + bos_cost + financial_cost)) / (fcr * (device_cost + bos_cost + financial_cost) + om_cost) * 100;
        double total_device_cost_lcoe = (fcr * device_cost) / (fcr * (device_cost + bos_cost + financial_cost) + om_cost) * 100;
        double total_bos_cost_lcoe = (fcr * bos_cost) / (fcr * (device_cost + bos_cost + financial_cost) + om_cost) * 100;
        double total_financial_cost_lcoe = (fcr * financial_cost) / (fcr * (device_cost + bos_cost + financial_cost) + om_cost) * 100;
        double total_om_cost_lcoe = (om_cost) / (fcr * (device_cost + bos_cost + financial_cost) + om_cost) * 100;
        assign("total_capital_cost_kwh", var_data((ssc_number_t)total_capital_cost_kwh));
        assign("total_device_cost_kwh", var_data((ssc_number_t)total_device_cost_kwh));
        assign("total_bos_cost_kwh", var_data((ssc_number_t)total_bos_cost_kwh));
        assign("total_financial_cost_kwh", var_data((ssc_number_t)total_financial_cost_kwh));
        assign("total_om_cost_kwh", var_data((ssc_number_t)total_om_cost_kwh));
        assign("total_capital_cost_lcoe", var_data((ssc_number_t)total_capital_cost_lcoe));
        assign("total_device_cost_lcoe", var_data((ssc_number_t)total_device_cost_lcoe));
        assign("total_bos_cost_lcoe", var_data((ssc_number_t)total_bos_cost_lcoe));
        assign("total_financial_cost_lcoe", var_data((ssc_number_t)total_financial_cost_lcoe));
        assign("total_om_cost_lcoe", var_data((ssc_number_t)total_om_cost_lcoe));

		//Calculating capacity factor:
		capacity_factor = annual_energy / (device_rated_capacity * number_devices * 8760);
		
		//Assigning values to outputs:
		assign("annual_energy", var_data((ssc_number_t)annual_energy));
		assign("average_power", var_data((ssc_number_t)device_average_power));
		//assign("system_capacity", var_data((ssc_number_t)system_capacity));
		assign("capacity_factor", var_data((ssc_number_t)capacity_factor * 100));
		assign("device_average_power", var_data((ssc_number_t)device_average_power));
        


	}
};

DEFINE_MODULE_ENTRY(mhk_wave, "MHK Wave power calculation model using power distribution.", 3);
