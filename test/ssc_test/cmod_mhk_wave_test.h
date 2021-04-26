#ifndef CMOD_MHK_WAVE_TEST_H_
#define CMOD_MHK_WAVE_TEST_H_

#include <gtest/gtest.h>
#include "../test/input_cases/mhk/mhk_wave_inputs.h"

#include "core.h"
#include "sscapi.h"

#include "vartab.h"
#include "../ssc/common.h"
#include "../test/input_cases/code_generator_utilities.h"

class CM_MHKWave : public ::testing::Test {
private:
public:
	ssc_data_t data;
	ssc_number_t calculated_value;
	ssc_number_t * calculated_array;

	void SetUp() {
		data = ssc_data_create();
		wave_inputs(data);
	}
	
	void TearDown() {
		if (data)
			ssc_data_clear(data);
	}

	void SetCalculated(std::string name)
	{
		ssc_data_get_number(data, const_cast<char *>(name.c_str()), &calculated_value);
	}
	// apparently memory of the array is managed internally to the sscapi.
	void SetCalculatedArray(std::string name)
	{
		int n;
		calculated_array = ssc_data_get_array(data, const_cast<char *>(name.c_str()), &n);
	}

    var_data* create_wavedata_array(int intervalsPerHour, int nMeasurementHeights) {
        size_t timeLength = 2920;
        double* height_data = new double[2920];
        double* period_data = new double[2920];
        for (int i = 0; i < (int)timeLength; i++) {
            height_data[i] = 5 + fmod(i, 10)/10;
            period_data[i] = 10 + fmod(i, 10) / 10;
        }

        
        
        var_data height_vd = var_data(height_data, int(timeLength));
        var_data periods_vd = var_data(period_data, int(timeLength));

        var_table* vt = new var_table;
        vt->assign("significant_wave_height", height_vd);
        vt->assign("energy_period", periods_vd);
        

        var_data* input = new var_data;
        input->type = SSC_TABLE;
        input->table = *vt;
        return input;
    }

};

#endif // !CMOD_MHK_WAVE_TEST_H_

