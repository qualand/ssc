#include <gtest/gtest.h>
#include <random>
#include <fstream>

#include <json/json.h>
#include "logger.h"
#include "lib_battery.h"
#include "lib_battery_lifetime_test.h"

TEST_F(lib_battery_lifetime_cycle_test, SetUpTest) {
    EXPECT_EQ(cycle_model->capacity_percent(), 100);
}


struct cycle_lifetime_state {
    double relative_q;
    double Xlt;
    double Ylt;
    double Range;
    double average_range;
    size_t nCycles;
    double jlt;
    std::vector<double> Peaks;
};

TEST_F(lib_battery_lifetime_cycle_test, runCycleLifetimeTest) {
    double DOD = 5;       // not used but required for function
    int idx = 0;
    while (idx < 500){
        if (idx % 2 != 0){
            DOD = 95;
        }
        else
            DOD = 5;
        cycle_model->runCycleLifetime(DOD);
        idx++;
    }
    lifetime_state s = cycle_model->get_state();
    EXPECT_NEAR(s.cycle->q_relative_cycle, 95.02, tol);
    EXPECT_NEAR(s.cycle->rainflow_Xlt, 90, tol);
    EXPECT_NEAR(s.cycle->rainflow_Ylt, 90, tol);
    EXPECT_NEAR(s.cycle->rainflow_jlt, 2, tol);
    EXPECT_NEAR(s.cycle_range, 90, tol);
    EXPECT_NEAR(s.average_range, 90, tol);
    EXPECT_NEAR(s.n_cycles, 249, tol);

    while (idx < 1000){
        if (idx % 2 != 0){
            DOD = 90;
        }
        cycle_model->runCycleLifetime(DOD);
        idx++;
    }
    s = cycle_model->get_state();
    EXPECT_NEAR(s.cycle->q_relative_cycle, 91.244, tol);
    EXPECT_NEAR(s.cycle->rainflow_Xlt, 0, tol);
    EXPECT_NEAR(s.cycle->rainflow_Ylt, 0, tol);
    EXPECT_NEAR(s.cycle->rainflow_jlt, 2, tol);
    EXPECT_NEAR(s.cycle_range, 0, tol);
    EXPECT_NEAR(s.average_range, 44.9098, tol);
    EXPECT_NEAR(s.n_cycles, 499, tol);
}

TEST_F(lib_battery_lifetime_cycle_test, runCycleLifetimeTestJaggedProfile) {
    std::vector<double> DOD = { 5, 95, 50, 85, 10, 50, 5, 95, 5 };  // 4 cycles
    int idx = 0;
    while (idx < DOD.size()) {
        cycle_model->runCycleLifetime(DOD[idx]);
        idx++;
    }
    lifetime_state s = cycle_model->get_state();
    EXPECT_NEAR(s.cycle->q_relative_cycle, 99.95, tol);
    EXPECT_NEAR(s.cycle->rainflow_Xlt, 90, tol);
    EXPECT_NEAR(s.cycle->rainflow_Ylt, 90, tol);
    EXPECT_NEAR(s.cycle->rainflow_jlt, 1, tol);
    EXPECT_NEAR(s.cycle_range, 90, tol);
    EXPECT_NEAR(s.average_range, 63.75, tol);
    EXPECT_NEAR(s.n_cycles, 4, tol);

}

TEST_F(lib_battery_lifetime_cycle_test, runCycleLifetimeTestKokamProfile) {
    std::vector<double> DOD = { 0.66, 1.0, 0.24722075172048893, 1.0, 0.24559790735021855, 0.9989411900454035, 0.24559790735021936, 0.9989411900454057, 0.24573025859454606, 0.9990735412897335, 0.24625966357184892, 0.9992058925340614, 0.2466567173048243, 0.9992058925340647, 0.2465243660605033, 0.9982794338237967, 0.24718612228213058, 0.9992058925340731, 0.24718612228213466, 0.9982794338238032, 0.24612731232753976, 0.9981470825794796, 0.24625966357186685, 0.9984117850681331, 0.24678906854916766, 0.9984117850681358, 0.24731847352646982, 0.9985441363124643, 0.24784787850377074, 0.9988088388011173, 0.24784787850377454 };  // 4 cycles
    int idx = 0;
    while (idx < DOD.size()) {
        cycle_model->runCycleLifetime((1-DOD[idx]) * 100.0);
        idx++;
    }
    lifetime_state s = cycle_model->get_state();
    EXPECT_NEAR(s.cycle->q_relative_cycle, 99.79, tol);
    EXPECT_NEAR(s.cycle->rainflow_Xlt, 75.09, tol);
    EXPECT_NEAR(s.cycle->rainflow_Ylt, 75.27, tol);
    EXPECT_NEAR(s.cycle->rainflow_jlt, 5, tol);
    EXPECT_NEAR(s.cycle_range, 75.07, tol);
    EXPECT_NEAR(s.average_range, 72.03, tol);
    EXPECT_NEAR(s.n_cycles, 13, tol);
}

TEST_F(lib_battery_lifetime_cycle_test, runCycleLifetimeTestWithNoise) {
    int seed = 100;
    double tol_high = 1.6; // Randomness will generate different results on different platforms

    // Initialize a default_random_engine with the seed
    std::default_random_engine randomEngine(seed);

    // Initialize a uniform_real_distribution to produce values between -1 and 1
    std::uniform_real_distribution<double> unifRealDist(-1.0, 1.0);

    double DOD = 5;       // not used but required for function
    int idx = 0;
    while (idx < 500) {
        double number = unifRealDist(randomEngine);
        if (idx % 2 != 0) {
            DOD = 95 + number;
        }
        else
            DOD = 5 + number;
        cycle_model->runCycleLifetime(DOD);
        idx++;
    }
    lifetime_state s = cycle_model->get_state();
    EXPECT_NEAR(s.cycle->q_relative_cycle, 95.06, tol_high);
    EXPECT_NEAR(s.cycle_range, 90.6, tol_high);
    EXPECT_NEAR(s.average_range, 90.02, tol_high);
    EXPECT_NEAR(s.n_cycles, 245, 10);
}

TEST_F(lib_battery_lifetime_cycle_test, replaceBatteryTest) {
    double DOD = 5;       // not used but required for function
    int idx = 0;
    while (idx < 1500){
        if (idx % 2 != 0){
            DOD = 95;
        }
        else
            DOD = 5;
        cycle_model->runCycleLifetime(DOD);
        idx++;
    }
    auto st = cycle_lifetime_state({85.02,90,90,90,90, 749, 2});
    lifetime_state s = cycle_model->get_state();
    EXPECT_NEAR(s.cycle->q_relative_cycle, 85.02, tol);
    EXPECT_NEAR(s.cycle->rainflow_Xlt, 90, tol);
    EXPECT_NEAR(s.cycle->rainflow_Ylt, 90, tol);
    EXPECT_NEAR(s.cycle->rainflow_jlt, 2, tol);
    EXPECT_NEAR(s.cycle_range, 90, tol);
    EXPECT_NEAR(s.average_range, 90, tol);
    EXPECT_NEAR(s.n_cycles, 749, tol);

    cycle_model->replaceBattery(5);

    s = cycle_model->get_state();
    EXPECT_NEAR(s.cycle->q_relative_cycle, 90.019, tol);
    EXPECT_NEAR(s.cycle->rainflow_Xlt, 0, tol);
    EXPECT_NEAR(s.cycle->rainflow_Ylt, 0, tol);
    EXPECT_NEAR(s.cycle->rainflow_jlt, 0, tol);
    EXPECT_NEAR(s.cycle_range, 90, tol);
    EXPECT_NEAR(s.average_range, 90, tol);
    EXPECT_NEAR(s.n_cycles, 749, tol);
}

TEST_F(lib_battery_lifetime_calendar_matrix_test, runCalendarMatrixTest) {
    double T = 278, SOC = 20;       // not used but required for function
    int idx = 0;
    while (idx < 500){
        if (idx % 2 != 0){
            SOC = 90;
        }
        cal_model->runLifetimeCalendarModel(idx, T, SOC);
        idx++;
    }
    lifetime_state s = cal_model->get_state();
    EXPECT_NEAR(s.day_age_of_battery, 20.79, tol);
    EXPECT_NEAR(s.calendar->q_relative_calendar, 99.89, tol);
    EXPECT_NEAR(s.calendar->dq_relative_calendar_old, 0, tol);

    while (idx < 1000){
        if (idx % 2 != 0){
            SOC = 90;
        }
        cal_model->runLifetimeCalendarModel(idx, T, SOC);
        idx++;
    }
    s = cal_model->get_state();
    EXPECT_NEAR(s.day_age_of_battery, 41.625, tol);
    EXPECT_NEAR(s.calendar->q_relative_calendar, 99.775, tol);
    EXPECT_NEAR(s.calendar->dq_relative_calendar_old, 0, tol);
}

TEST_F(lib_battery_lifetime_calendar_matrix_test, replaceBatteryTest) {
    double T = 4.85, SOC = 20;
    int idx = 0;
    while (idx < 200000){
        if (idx % 2 != 0){
            SOC = 90;
        }
        cal_model->runLifetimeCalendarModel(idx, T, SOC);
        idx++;
    }
    lifetime_state s = cal_model->get_state();
    EXPECT_NEAR(s.day_age_of_battery, 8333.29, tol);
    EXPECT_NEAR(s.calendar->q_relative_calendar, 41.51, tol);
    EXPECT_NEAR(s.calendar->dq_relative_calendar_old, 0, tol);

    cal_model->replaceBattery(5);

    s = cal_model->get_state();
    EXPECT_NEAR(s.day_age_of_battery, 0, tol);
    EXPECT_NEAR(s.calendar->q_relative_calendar, 46.51, tol);
    EXPECT_NEAR(s.calendar->dq_relative_calendar_old, 0, tol);
}

TEST_F(lib_battery_lifetime_calendar_model_test, SetUpTest) {
    EXPECT_EQ(cal_model->capacity_percent(), 102);
}

TEST_F(lib_battery_lifetime_calendar_model_test, runCalendarModelTest) {
    double T = 4.85, SOC = 20;       // not used but required for function
    int idx = 0;
    while (idx < 500){
        if (idx % 2 != 0){
            SOC = 90;
        }
        cal_model->runLifetimeCalendarModel(idx, T, SOC);
        idx++;
    }
    lifetime_state s = cal_model->get_state();
    EXPECT_NEAR(s.day_age_of_battery, 20.79, tol);
    EXPECT_NEAR(s.calendar->q_relative_calendar, 101.78, tol);
    EXPECT_NEAR(s.calendar->dq_relative_calendar_old, 0.00217, tol);

    while (idx < 1000){
        if (idx % 2 != 0){
            SOC = 90;
        }
        cal_model->runLifetimeCalendarModel(idx, T, SOC);
        idx++;
    }
    s = cal_model->get_state();
    EXPECT_NEAR(s.day_age_of_battery, 41.625, tol);
    EXPECT_NEAR(s.calendar->q_relative_calendar, 101.69, tol);
    EXPECT_NEAR(s.calendar->dq_relative_calendar_old, 0.00306, tol);
}

TEST_F(lib_battery_lifetime_calendar_model_test, replaceBatteryTest) {
    double T = 4.85, SOC = 20;
    int idx = 0;
    while (idx < 200000){
        if (idx % 2 != 0){
            SOC = 90;
        }
        cal_model->runLifetimeCalendarModel(idx, T, SOC);
        idx++;
    }
    lifetime_state s = cal_model->get_state();
    EXPECT_NEAR(s.day_age_of_battery, 8333.29, tol);
    EXPECT_NEAR(s.calendar->q_relative_calendar, 97.67, tol);
    EXPECT_NEAR(s.calendar->dq_relative_calendar_old, 0.043, tol);

    cal_model->replaceBattery(5);

    s = cal_model->get_state();
    EXPECT_NEAR(s.day_age_of_battery, 0, tol);
    EXPECT_NEAR(s.calendar->q_relative_calendar, 102, tol);
    EXPECT_NEAR(s.calendar->dq_relative_calendar_old, 0.0, tol);
}

TEST_F(lib_battery_lifetime_calendar_matrix_test, TestLifetimeDegradation) {
    double vals[] = { 0, 100, 365, 50 };
    util::matrix_t<double> lifetime_matrix;
    lifetime_matrix.assign(vals, 2, 2);

    double dt_hour = 1;
    lifetime_calendar_t hourly_lifetime(dt_hour, lifetime_matrix);

    for (int idx = 0; idx < 8760; idx++) {
        hourly_lifetime.runLifetimeCalendarModel(idx, 20, 80);
    }

    EXPECT_NEAR(hourly_lifetime.capacity_percent(), 50, 1);

    dt_hour = 1.0 / 12.0; // Every 5 mins
    lifetime_calendar_t subhourly_lifetime(dt_hour, lifetime_matrix);

    for (int idx = 0; idx < 8760 * 12; idx++) {
        subhourly_lifetime.runLifetimeCalendarModel(idx, 20, 80);
    }

    EXPECT_NEAR(subhourly_lifetime.capacity_percent(), 50, 1);
}


TEST_F(lib_battery_lifetime_calendar_model_test, TestLifetimeDegradation) {

    for (int idx = 0; idx < 8760; idx++) {
        cal_model->runLifetimeCalendarModel(idx, 20, 80);
    }

    EXPECT_NEAR(cal_model->capacity_percent(), 99.812, 1);

    dt_hour = 1.0 / 12.0; // Every 5 mins
    lifetime_calendar_t subhourly_lifetime(dt_hour);

    for (int idx = 0; idx < 8760 * 12; idx++) {
        subhourly_lifetime.runLifetimeCalendarModel(idx, 20, 80);
    }

    EXPECT_NEAR(subhourly_lifetime.capacity_percent(), 99.812, 1);
}

TEST_F(lib_battery_lifetime_test, updateCapacityTest) {
    size_t idx = 0;
    while (idx < 876){
        model->runLifetimeModels(idx, true, 5,95, 25);
        model->runLifetimeModels(idx, true, 95, 5, 25);

        auto state = model->get_state();
        EXPECT_EQ(state.cycle->q_relative_cycle, model->capacity_percent_cycle());
        EXPECT_EQ(state.calendar->q_relative_calendar, model->capacity_percent_calendar());

        idx ++;
    }
}

TEST_F(lib_battery_lifetime_test, runCycleLifetimeTestWithRestPeriod) {
    double tol = 0.01;

    std::vector<double> DOD = { 5, 50, 95, 50, 5, 5, 5, 50, 95, 50, 5, 5, 5, 50, 95, 50, 5 };  // 3 cycles 90% cycle_DOD
    std::vector<bool> charge_changed = { true, false, false, true, false, false, false, true, false, true, false, false, false, true, false, true, false };
    int idx = 0;
    double T_battery = 25; // deg C
    while (idx < DOD.size()) {
        double DOD_prev = 0;
        if (idx > 0) {
            DOD_prev = DOD[idx - 1];
        }
        model->runLifetimeModels(idx, charge_changed[idx], DOD_prev, DOD[idx], T_battery);
        idx++;
    }


    lifetime_state s = model->get_state();
    EXPECT_NEAR(s.cycle->q_relative_cycle, 99.96, tol);
    EXPECT_NEAR(s.cycle->rainflow_Xlt, 90, tol);
    EXPECT_NEAR(s.cycle->rainflow_Ylt, 90, tol);
    EXPECT_NEAR(s.cycle->rainflow_jlt, 2, tol);
    EXPECT_NEAR(s.cycle_range, 90, tol);
    EXPECT_NEAR(s.average_range, 90, tol);
    EXPECT_NEAR(s.n_cycles, 2, tol);
}

TEST_F(lib_battery_lifetime_nmc_test, InitTest) {
    double tol = 0.001;

    //check lifetime_nmc_state_initialization
    auto lifetime_state = model->get_state();
    EXPECT_NEAR(lifetime_state.nmc_li_neg->q_relative_neg, 100, tol);
    EXPECT_NEAR(lifetime_state.nmc_li_neg->q_relative_li, 100, tol);
    EXPECT_EQ(model->get_state().day_age_of_battery, 0);
    EXPECT_EQ(model->get_state().n_cycles, 0);

    //check U_neg, and Voc functions (SOC as a fractional input)
    EXPECT_NEAR(model->calculate_Uneg(0), 1.2868, tol);
    EXPECT_NEAR(model->calculate_Voc(0), 3, tol);
    EXPECT_NEAR(model->calculate_Uneg(0.1), 0.242, tol);
    EXPECT_NEAR(model->calculate_Voc(0.1), 3.4679, tol);
    EXPECT_NEAR(model->calculate_Uneg(0.5), 0.123, tol);
    EXPECT_NEAR(model->calculate_Voc(0.5), 3.6876, tol);
    EXPECT_NEAR(model->calculate_Uneg(0.9), 0.0876, tol);
    EXPECT_NEAR(model->calculate_Voc(0.9), 4.0668, tol);
    EXPECT_NEAR(model->calculate_Uneg(1), 0.0859, tol);
    EXPECT_NEAR(model->calculate_Voc(1), 4.193, tol);
}

TEST_F(lib_battery_lifetime_nmc_test, CopyTest) {
    double tol = 0.001;

    // check lifetime_nmc_state get & set
    auto state = model->get_state();
    state.nmc_li_neg->cycle_DOD_range = {0, 1};
    state.nmc_li_neg->cycle_DOD_max = {2, 3};
    model->set_state(state);

    state = model->get_state();
    EXPECT_EQ(state.nmc_li_neg->cycle_DOD_range[0], 0);
    EXPECT_EQ(state.nmc_li_neg->cycle_DOD_range[1], 1);
    EXPECT_EQ(state.nmc_li_neg->cycle_DOD_max[0], 2);
    EXPECT_EQ(state.nmc_li_neg->cycle_DOD_max[1], 3);

    auto new_model = lifetime_nmc_t(*model);
    state = new_model.get_state();
    EXPECT_EQ(state.nmc_li_neg->cycle_DOD_range[0], 0);
    EXPECT_EQ(state.nmc_li_neg->cycle_DOD_range[1], 1);
    EXPECT_EQ(state.nmc_li_neg->cycle_DOD_max[0], 2);
    EXPECT_EQ(state.nmc_li_neg->cycle_DOD_max[1], 3);
}

/// run at different days
TEST_F(lib_battery_lifetime_nmc_test, StorageDays) {
    std::vector<double> days = {0, 10, 50 , 500, 5000};
    std::vector<double> expected_q_li = {106.50, 104.36, 103.97, 102.835, 99.66};

    for (size_t i = 0; i < days.back() + 1; i++) {
        for (size_t h = 0; h < 24; h++) {
            size_t hr = i * 24 + h;
            model->runLifetimeModels(hr, false, 50, 50, 25);
        }
        auto pos = std::find(days.begin(), days.end(), i);
        if (pos != days.end()) {
            auto state = model->get_state();
            EXPECT_NEAR(state.nmc_li_neg->q_relative_li, expected_q_li[pos - days.begin()], 0.5);
        }
    }
    EXPECT_EQ((size_t)model->get_state().day_age_of_battery, 5001);
}

/// Run with minute timestep instead
TEST_F(lib_battery_lifetime_nmc_test, StorageMinuteTimestep) {
    double dt_hr = 1. / 60;
    model = std::unique_ptr<lifetime_nmc_t>(new lifetime_nmc_t(dt_hr));

    std::vector<double> days = {0, 10, 50 , 500, 5000};
    std::vector<double> expected_q_li = {106.50, 104.36, 103.97, 102.835, 99.66};

    auto steps_per_day = (size_t)(24 / dt_hr);
    for (size_t i = 0; i < days.back() + 1; i++) {
        for (size_t h = 0; h < steps_per_day; h++) {
            size_t hr = i * steps_per_day + h;
            model->runLifetimeModels(hr, false, 50, 50, 25);
        }
        auto pos = std::find(days.begin(), days.end(), i);
        if (pos != days.end()) {
            auto state = model->get_state();
            EXPECT_NEAR(state.nmc_li_neg->q_relative_li, expected_q_li[pos - days.begin()], 0.5);
        }
    }
    EXPECT_EQ((size_t)model->get_state().day_age_of_battery, 5001);
}

/// run at different days at different temperatures
TEST_F(lib_battery_lifetime_nmc_test, StorageTemp) {
    std::vector<double> temps = {0, 10, 15, 40};
    std::vector<double> expected_q_li = {81.73, 93.08, 97.43, 93};

    for (size_t n = 3; n < temps.size(); n++) {
        model = std::unique_ptr<lifetime_nmc_t>(new lifetime_nmc_t(dt_hour));
        for (size_t d = 0; d < 5000 + 1; d++) {
            for (size_t h = 0; h < 24; h++) {
                size_t hr = d * 24 + h;
                model->runLifetimeModels(hr, false, 50, 50, temps[n]);
            }
        }
        auto state = model->get_state();
        EXPECT_NEAR((size_t)state.nmc_li_neg->q_relative_li, expected_q_li[n], 1);
    }
}

TEST_F(lib_battery_lifetime_nmc_test, StorageTempSmallDt) {
    std::vector<double> temps = { 0, 10, 15, 40 };
    std::vector<double> expected_q_li = { 81.73, 93.08, 97.43, 90 };

    dt_hour = 1 / 60.0 / 60.0 * 10.0;

    for (size_t n = 3; n < temps.size(); n++) {
        model = std::unique_ptr<lifetime_nmc_t>(new lifetime_nmc_t(dt_hour));
        for (size_t d = 0; d < 5000 + 1; d++) {
            for (size_t h = 0; h < 24; h++) {
                size_t hr = d * 24 + h;
                for (size_t s = 0; s < 600; s++) {
                    model->runLifetimeModels(hr, false, 50, 50, temps[n]);
                }
            }
        }
        auto state = model->get_state();
        EXPECT_NEAR((size_t)state.nmc_li_neg->q_relative_li, expected_q_li[n], 1);
    }
}

TEST_F(lib_battery_lifetime_nmc_test, CyclingHighDOD) {
    size_t day = 0;
    double T = 25.15;
    while (day < 87) {
        for (size_t i = 0; i < 24; i++) {
            size_t idx = day * 24 + i;
            if (i == 0)
                model->runLifetimeModels(idx, false, 50, 90, T);
            else if (i == 1)
                model->runLifetimeModels(idx, true, 90, 10, T);
            else if (i == 3)
                model->runLifetimeModels(idx, true, 10, 50, T);
            else
                model->runLifetimeModels(idx, false, 50, 50, T);
        }
        day ++;
    }

    auto state = model->get_state();

    EXPECT_EQ(state.n_cycles, 86);
    EXPECT_NEAR(state.nmc_li_neg->q_relative_li, 101.19, 0.5);
    EXPECT_NEAR(state.nmc_li_neg->q_relative_neg, 100.6, 0.5);
    EXPECT_NEAR(state.day_age_of_battery, 87, 1e-3);

    while (day < 870) {
        for (size_t i = 0; i < 24; i++) {
            size_t idx = day * 24 + i;
            if (i == 0)
                model->runLifetimeModels(idx, false, 50, 90, T);
            else if (i == 1)
                model->runLifetimeModels(idx, true, 90, 10, T);
            else if (i == 3)
                model->runLifetimeModels(idx, true, 10, 50, T);
            else
                model->runLifetimeModels(idx, false, 50, 50, T);
        }
        day ++;
    }

    state = model->get_state();

    EXPECT_EQ(state.n_cycles, 869);
    EXPECT_NEAR(state.nmc_li_neg->q_relative_li, 94.09, 0.5);
    EXPECT_NEAR(state.nmc_li_neg->q_relative_neg, 99.30, 0.5);
    EXPECT_NEAR(state.day_age_of_battery, 870, 1e-3);

    while (day < 8700) {
        for (size_t i = 0; i < 24; i++) {
            size_t idx = day * 24 + i;
            if (i == 0)
                model->runLifetimeModels(idx, false, 50, 90, T);
            else if (i == 1)
                model->runLifetimeModels(idx, true, 90, 10, T);
            else if (i == 3)
                model->runLifetimeModels(idx, true, 10, 50, T);
            else
                model->runLifetimeModels(idx, false, 50, 50, T);
        }
        day ++;
    }

    state = model->get_state();

    EXPECT_EQ(state.n_cycles, 8699);
    EXPECT_EQ(state.cycle_range, 80);
    EXPECT_EQ(state.cycle_DOD, 90);
    EXPECT_NEAR(state.nmc_li_neg->q_relative_li, 63.42, 3);
    EXPECT_NEAR(state.nmc_li_neg->q_relative_neg, 84.43, 0.5);
    EXPECT_NEAR(state.day_age_of_battery, 8700, 1e-3);
}

TEST_F(lib_battery_lifetime_nmc_test, CyclingHighTemp) {
    size_t day = 0;
    double T = 35.;

    while (day < 87) {
        for (size_t i = 0; i < 24; i++) {
            size_t idx = day * 24 + i;
            if (i == 0)
                model->runLifetimeModels(idx, false, 50, 70, T);
            else if (i == 1)
                model->runLifetimeModels(idx, true, 70, 30, T);
            else if (i == 3)
                model->runLifetimeModels(idx, true, 30, 50, T);
            else
                model->runLifetimeModels(idx, false, 50, 50, T);
        }
        day ++;
    }

    auto state = model->get_state();

    EXPECT_EQ(state.n_cycles, 86);
    EXPECT_NEAR(state.nmc_li_neg->q_relative_li, 104.58, 0.6);
    EXPECT_NEAR(state.nmc_li_neg->q_relative_neg, 103.88, 0.5);
    EXPECT_NEAR(state.day_age_of_battery, 87, 1e-3);

    while (day < 870) {
        for (size_t i = 0; i < 24; i++) {
            size_t idx = day * 24 + i;
            if (i == 0)
                model->runLifetimeModels(idx, false, 50, 70, T);
            else if (i == 1)
                model->runLifetimeModels(idx, true, 70, 30, T);
            else if (i == 3)
                model->runLifetimeModels(idx, true, 30, 50, T);
            else
                model->runLifetimeModels(idx, false, 50, 50, T);
        }
        day ++;
    }

    state = model->get_state();

    EXPECT_EQ(state.n_cycles, 869);
    EXPECT_NEAR(state.nmc_li_neg->q_relative_li, 100.37, 0.5);
    EXPECT_NEAR(state.nmc_li_neg->q_relative_neg, 103.35, 0.5);
    EXPECT_NEAR(state.day_age_of_battery, 870, 1e-3);

    while (day < 8700) {
        for (size_t i = 0; i < 24; i++) {
            size_t idx = day * 24 + i;
            if (i == 0)
                model->runLifetimeModels(idx, false, 50, 70, T);
            else if (i == 1)
                model->runLifetimeModels(idx, true, 70, 30, T);
            else if (i == 3)
                model->runLifetimeModels(idx, true, 30, 50, T);
            else
                model->runLifetimeModels(idx, false, 50, 50, T);
        }
        day ++;
    }

    state = model->get_state();

    EXPECT_EQ(state.n_cycles, 8699);
    EXPECT_EQ(state.cycle_range, 40);
    EXPECT_EQ(state.cycle_DOD, 70);
    EXPECT_NEAR(state.nmc_li_neg->q_relative_li, 82.11, 0.5);
    EXPECT_NEAR(state.nmc_li_neg->q_relative_neg, 103.49, 0.5);
    EXPECT_NEAR(state.day_age_of_battery, 8700, 1e-3);
}

TEST_F(lib_battery_lifetime_nmc_test, CyclingCRate) {
    size_t day = 0;

    // 90 cycle_DOD cycle once per day, slower Crate than above
    std::vector<double> DODs_day = {50., 56.67, 63.33, 70., 76.67, 83.33,
                                    90., 83.33, 76.67, 70., 63.33, 56.67, 50., 43.33, 36.67, 30., 23.33, 16.67,
                                    10., 16.67, 23.33, 30., 36.67, 43.33};

    while (day < 87) {
        for (size_t i = 0; i < DODs_day.size(); i++) {
            size_t idx = day * 24 + i;
            bool charge_changed = i == 7 || i == 19;
            double prev_DOD = DODs_day[i % 24];
            double DOD = DODs_day[i];
            model->runLifetimeModels(idx, charge_changed, prev_DOD, DOD, 25);
        }
        day ++;
    }

    auto state = model->get_state();

    EXPECT_EQ(state.n_cycles, 86);
//    EXPECT_EQ(state.nmc_li_neg->DOD_min, 43.33);
    EXPECT_NEAR(state.nmc_li_neg->q_relative_li, 103, 1);
    EXPECT_NEAR(state.nmc_li_neg->q_relative_neg, 100, 1);
    EXPECT_NEAR(state.day_age_of_battery, 87, 1e-3);

    while (day < 870) {
        for (size_t i = 0; i < DODs_day.size(); i++) {
            size_t idx = day * 24 + i;
            bool charge_changed = i == 7 || i == 19;
            double prev_DOD = DODs_day[i % 24];
            double DOD = DODs_day[i];
            model->runLifetimeModels(idx, charge_changed, prev_DOD, DOD, 25);
        }
        day ++;
    }

    state = model->get_state();

    EXPECT_EQ(state.n_cycles, 869);
    EXPECT_NEAR(state.nmc_li_neg->q_relative_li, 97.61, 1);
    EXPECT_NEAR(state.nmc_li_neg->q_relative_neg, 100.17, 1);
    EXPECT_NEAR(state.day_age_of_battery, 870, 1e-3);
}

TEST_F(lib_battery_lifetime_nmc_test, CyclingCRateMinuteTimestep) {
    double dt_hr = 1. / 60;
    auto steps_per_day = (size_t)(24 / dt_hr);
    model = std::unique_ptr<lifetime_nmc_t>(new lifetime_nmc_t(dt_hr));

    size_t day = 0;
    size_t idx = 0;
    // 90 cycle_DOD cycle once per day, slower Crate than above
    std::vector<double> DODs_day = {50., 56.67, 63.33, 70., 76.67, 83.33,
                                    90., 83.33, 76.67, 70., 63.33, 56.67, 50., 43.33, 36.67, 30., 23.33, 16.67,
                                    10., 16.67, 23.33, 30., 36.67, 43.33};

    while (day < 87) {
        for (size_t hr = 0; hr < DODs_day.size(); hr++) {
            double prev_DOD = DODs_day[hr % 24];
            double DOD = DODs_day[hr];
            for (size_t min = 0; min < (size_t)(1. / dt_hr); min++) {
                bool charge_changed = (hr == 7 || hr == 19) && min == 0;
                if (min != 0)
                    prev_DOD = DOD;
                model->runLifetimeModels(idx, charge_changed, prev_DOD, DOD, 25);
                idx ++;
            }
        }
        day ++;
    }

    auto state = model->get_state();

    EXPECT_EQ(state.n_cycles, 86);
//    EXPECT_EQ(state.nmc_li_neg->DOD_min, 43.33);
    EXPECT_NEAR(state.nmc_li_neg->q_relative_li, 103, 1);
    EXPECT_NEAR(state.nmc_li_neg->q_relative_neg, 100, 1);
    EXPECT_NEAR(state.day_age_of_battery, 87, 1e-3);

    while (day < 870) {
        for (size_t hr = 0; hr < DODs_day.size(); hr++) {
            double prev_DOD = DODs_day[hr % 24];
            double DOD = DODs_day[hr];
            for (size_t min = 0; min < (size_t)(1. / dt_hr); min++) {
                bool charge_changed = (hr == 7 || hr == 19) && min == 0;
                if (min != 0)
                    prev_DOD = DOD;
                model->runLifetimeModels(idx, charge_changed, prev_DOD, DOD, 25);
                idx ++;
            }
        }
        day ++;
    }

    state = model->get_state();

    EXPECT_EQ(state.n_cycles, 869);
    EXPECT_NEAR(state.nmc_li_neg->q_relative_li, 97.61, 1);
    EXPECT_NEAR(state.nmc_li_neg->q_relative_neg, 100.17, 1);
    EXPECT_NEAR(state.day_age_of_battery, 870, 1e-3);
}

/// There's less accuracy since the degradation coefficients from the first day are lost when doing computation on day 2
TEST_F(lib_battery_lifetime_nmc_test, CyclingEveryTwoDays) {
    double T = 25.15;
    size_t day = 0;
    while (day < 87) {
        for (size_t i = 0; i < 48; i++) {
            size_t idx = day * 48 + i;
            if (i == 0)
                model->runLifetimeModels(idx, false, 50, 10, T);
            else if (i == 1)
                model->runLifetimeModels(idx, true, 10, 90, T);
            else if (i == 46)
                model->runLifetimeModels(idx, true, 90, 50, T);
            else
                model->runLifetimeModels(idx, false, 50, 50, T);
        }
        day += 2;
    }

    auto state = model->get_state();

    EXPECT_EQ(state.n_cycles, 43);
    EXPECT_NEAR(state.nmc_li_neg->q_relative_li, 101.88, 1);
    EXPECT_NEAR(state.nmc_li_neg->q_relative_neg, 100.6, 0.5);
    EXPECT_NEAR(state.day_age_of_battery, 88, 1e-3);
}

/** Test focusing on how different time steps affect the integration of a day's degradation in the NMC life model.
 * The integration of degradation is done at the end of each day when the elapsed time, `cum_dt` is exactly 1.
 * Check that if a simulation step has a timestep large enough that `cum_dt` passes from <1 to >1, that the effects
 * on lifetime are the same by breaking that timestep up and accruing the degradation and `cum_dt` correctly
 **/
TEST_F(lib_battery_lifetime_nmc_test, IrregularTimeStep) {
    double T = 35.15;
    auto state = model->get_state();

    auto b_params = std::make_shared<lifetime_params>(model->get_params());
    b_params->dt_hr = 0.5;
    auto b_state = std::make_shared<lifetime_state>(model->get_state());
    auto subhourly_model = std::unique_ptr<lifetime_nmc_t>(new lifetime_nmc_t(b_params, b_state));


    // run hourly
    size_t day = 0;
    while (day < 87) {
        size_t idx = 0;
        while (idx < 48) {
            size_t i = idx % 24;
            if (i == 0)
                model->runLifetimeModels(idx, false, 50, 70, T);
            else if (i == 1)
                model->runLifetimeModels(idx, true, 70, 30, T);
            else if (i == 3)
                model->runLifetimeModels(idx, true, 30, 50, T);
            else
                model->runLifetimeModels(idx, false, 50, 50, T);
            idx += 1;
        }
        day += 2;
    }
    state = model->get_state();

    EXPECT_EQ(state.n_cycles, 87);
    EXPECT_NEAR(state.nmc_li_neg->q_relative_li, 104.553, 1e-3);
    EXPECT_NEAR(state.nmc_li_neg->q_relative_neg, 103.92, 1e-3);
    EXPECT_NEAR(state.day_age_of_battery, 88, 1e-3);

    printf("\n");
    // run 30min timesteps for 23.5 hours then hourly for 24, then 1 0.5 hr time idx
    day = 0;
    while (day < 87) {
        size_t idx = 0;
        while (idx < (size_t) (23.5 * 2)) {
            size_t i = idx;
            if (i <= 1)
                subhourly_model->runLifetimeModels(idx, false, 50, 70, T);
            else if (i <= 3)
                subhourly_model->runLifetimeModels(idx, i == 2, 70, 30, T);
            else if (i == 6 || i == 7)
                subhourly_model->runLifetimeModels(idx, i == 6, 30, 50, T);
            else
                subhourly_model->runLifetimeModels(idx, false, 50, 50, T);
            idx += 1;
        }

        b_params->dt_hr = 1;
        b_state = std::make_shared<lifetime_state>(subhourly_model->get_state());
        subhourly_model = std::unique_ptr<lifetime_nmc_t>(new lifetime_nmc_t(b_params, b_state));
        idx = 0;
        while (idx < 24) {
            size_t i = idx % 24;
            if (i == 0)
                subhourly_model->runLifetimeModels(idx, false, 50, 70, T);
            else if (i == 1)
                subhourly_model->runLifetimeModels(idx, true, 70, 30, T);
            else if (i == 3)
                subhourly_model->runLifetimeModels(idx, true, 30, 50, T);
            else
                subhourly_model->runLifetimeModels(idx, false, 50, 50, T);
            idx += 1;
        }
        b_params->dt_hr = 0.5;
        b_state = std::make_shared<lifetime_state>(subhourly_model->get_state());
        subhourly_model = std::unique_ptr<lifetime_nmc_t>(new lifetime_nmc_t(b_params, b_state));
        subhourly_model->runLifetimeModels(idx, false, 50, 50, T);
        day += 2;
    }
    state = subhourly_model->get_state();

    EXPECT_EQ(state.n_cycles, 87);
    EXPECT_NEAR(state.nmc_li_neg->q_relative_li, 104.538, 1e-3);
    EXPECT_NEAR(state.nmc_li_neg->q_relative_neg, 103.92, 1e-3);
    EXPECT_NEAR(state.day_age_of_battery, 88, 1e-3);
}

TEST_F(lib_battery_lifetime_nmc_test, TestAgainstKokamData) {
    const char * SSCDIR = std::getenv("SSCDIR");

    std::string kokam_validation_path = std::string(SSCDIR) + "/test/input_cases/battery_nmc_life/";

    std::vector<int> cells_to_test = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

    dt_hour = 1. / 60 / 60 * 10;

    for (auto cell : cells_to_test) {
        // Get Cell input data
        std::string file_path = kokam_validation_path + "lifetime_validation_cell_" + std::to_string(cell) + ".json";
        std::ifstream file(file_path);

        Json::Value root;
        file >> root;

        std::vector<double> rpt_cycles;
        for (const auto & i : root["rpt_cycles_cum"])
            rpt_cycles.push_back(i.asDouble());

        std::vector<int> days_to_test;
        for (const auto & i : root["rpt_days_cum"])
            days_to_test.push_back((int)std::round(i.asDouble()));

        std::vector<double> full_soc_profile;
        for (const auto & i : root["15min_profile"])
            full_soc_profile.push_back(i.asDouble());
        dt_hour = 0.25;

        double cell_temp_K = root["temp"].asDouble();

        // Run Life model with the profile, which starts with charging and ends with discharging
        model = std::unique_ptr<lifetime_nmc_t>(new lifetime_nmc_t(dt_hour));

        // record capacity and cycles elapsed at RPT days, days_to_test
        std::vector<double> life_model_caps;
        std::vector<double> life_model_qLi;
        std::vector<double> life_model_qNeg;
        std::vector<double> cycs;

        // charging = -1; discharging = 1
        int charge_mode = -1;
        int prev_charge_mode = 1;

        for (int i = 1; i < full_soc_profile.size(); i++) {
            int j = i + 1;
            if (j > full_soc_profile.size() - 1)
                j = 0;
            double prev_SOC = full_soc_profile[i];
            double SOC = full_soc_profile[j];

            if (SOC - prev_SOC > 1e-5)
                charge_mode = -1;
            else if (prev_SOC - SOC > 1e-5)
                charge_mode = 1;

            bool charge_changed = false;

            if (charge_mode != prev_charge_mode) {
                charge_changed = true;
            }
            model->runLifetimeModels(i, charge_changed, (1. - prev_SOC) * 100, (1. - SOC) * 100., cell_temp_K - 273.15);
            prev_charge_mode = charge_mode;

            if (model->day_age_of_battery() - days_to_test[0] > -1e-7) {
                auto s = model->get_state();
                life_model_caps.push_back(s.q_relative * 0.75);
                life_model_qLi.push_back(s.nmc_li_neg->q_relative_li * 0.75);
                life_model_qNeg.push_back(s.nmc_li_neg->q_relative_neg * 0.75);
                cycs.push_back(s.n_cycles);
                days_to_test.erase(days_to_test.begin());
            }
            if (days_to_test.empty())
                break;
        }

        // Get Expected Cycle Count & Model Prediction
        std::vector<double> sam_cap_Ah;
        for (const auto & i : root["sam_cap_Ah"])
            sam_cap_Ah.push_back(i.asDouble());

        std::vector<double> sam_cap_cycles;
        for (const auto & i : root["sam_cap_cycles"])
            sam_cap_cycles.push_back(i.asDouble());

        for (size_t n = 0; n < life_model_caps.size(); n++) {
            EXPECT_NEAR(sam_cap_Ah[n], life_model_caps[n], 1e-3) << "cell" << cell;
            EXPECT_NEAR(sam_cap_cycles[n], cycs[n], 1e-3) << "cell" << cell;
        }
    }
}

TEST_F(lib_battery_lifetime_nmc_test, replaceBatteryTest) {
    double tol = 0.01;
    double T = 25.15;
    size_t day = 0;
    while (day < 2000) {
        for (size_t i = 0; i < 24; i++) {
            size_t idx = day * 24 + i;
            if (i == 0)
                model->runLifetimeModels(idx, false, 50, 90, T);
            else if (i == 1)
                model->runLifetimeModels(idx, true, 90, 10, T);
            else if (i == 3)
                model->runLifetimeModels(idx, true, 10, 50, T);
            else
                model->runLifetimeModels(idx, false, 50, 50, T);
        }
        day ++;
    }

    auto s = model->get_state();

    EXPECT_NEAR(s.nmc_li_neg->q_relative_li, 87.90, tol);
    EXPECT_NEAR(s.nmc_li_neg->q_relative_neg, 97.15, tol);

    model->replaceBattery(10);
    s = model->get_state();
    EXPECT_NEAR(s.nmc_li_neg->q_relative_li, 97.90, tol);
    EXPECT_NEAR(s.nmc_li_neg->q_relative_neg, 100, tol);
    EXPECT_NEAR(s.q_relative, 97.90, tol);
    EXPECT_NEAR(s.cycle->rainflow_Xlt, 0, tol);
    EXPECT_NEAR(s.cycle->rainflow_Ylt, 0, tol);
    EXPECT_NEAR(s.cycle->rainflow_jlt, 0, tol);
    EXPECT_NEAR(s.cycle_range, 80, tol);
    EXPECT_NEAR(s.average_range, 80, tol);
    EXPECT_NEAR(s.n_cycles, 1999, tol);

    model->replaceBattery(100);
    s = model->get_state();
    EXPECT_NEAR(s.nmc_li_neg->q_relative_li, 100, tol);
    EXPECT_NEAR(s.nmc_li_neg->q_relative_li, 100, tol);
    EXPECT_NEAR(s.q_relative, 100, tol);
    EXPECT_NEAR(s.cycle->rainflow_Xlt, 0, tol);
    EXPECT_NEAR(s.cycle->rainflow_Ylt, 0, tol);
    EXPECT_NEAR(s.cycle->rainflow_jlt, 0, tol);
    EXPECT_NEAR(s.cycle_range, 0, tol);
    EXPECT_NEAR(s.average_range, 0, tol);
    EXPECT_NEAR(s.n_cycles, 0, tol);
}
