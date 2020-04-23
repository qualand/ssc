#include "logger.h"

#include "lib_util.h"
#include "lib_battery_capacity.h"
#include "lib_battery_voltage.h"
#include "lib_battery_lifetime.h"
#include "lib_battery.h"

/**
* Helper fx
*/

template<typename T>
std::ostream& operator<<(std::ostream& s, std::vector<T> t) {
    s.precision(2);
    s << "[";
    for (std::size_t i = 0; i < t.size(); i++) {
        s << t[i] << (i == t.size() - 1 ? " " : ", ");
    }
    return s << "]";
}

template<typename T>
std::ostream& operator<<(std::ostream& s, util::matrix_t<T> t) {
    size_t nr = t.nrows();
    size_t nc = t.ncols();

    s.precision(2);
    s << "[";
    for (std::size_t i = 0; i < nr; i++) {
        s << "[";
        for (std::size_t j = 0; j < nc; j++) {
            s << t.at(i, j) << (i == nc - 1 ? "" : ", ");
        }
        s << "]" << (i == nr - 1 ? "" : ", ");;

    }
    return s << "]";
}

std::ostream &operator<<(std::ostream &os, const voltage_params &p) {
    char buf[256];
    sprintf(buf, "voltage_params: { \"mode\": %d, \"num_cell_series\": %d, \"num_strings\": %d, "
                 "\"cell_voltage_nominal\": %.3f, \"R\": %.3f, \"dt_hr\": %.3f, "
                 "\"dynamic\": { \"Vfull\": %.3f, \"Vexp\": %.3f, \"Vnom\": %.3f, "
                 "\"Qfull\": %.3f, \"Qexp\": %.3f, \"Qnom\": %.3f, \"C_rate\": %.3f } }", p.mode, p.num_cells_series,
            p.num_strings,
            p.cell_voltage_nominal, p.R, p.dt_hr,
            p.dynamic.Vfull, p.dynamic.Vexp, p.dynamic.Vnom,
            p.dynamic.Qfull, p.dynamic.Qexp, p.dynamic.Qnom, p.dynamic.C_rate);
    os << buf;
    return os;
}

std::ostream &operator<<(std::ostream &os, const voltage_state &p) {
    char buf[128];
    sprintf(buf, "voltage_state: { \"cell_voltage\": %.3f }", p.cell_voltage);
    os << buf;
    return os;
}

std::ostream &operator<<(std::ostream &os, const capacity_state &p) {
    char buf[1024];
    sprintf(buf, "capacity_state: { \"q0\": %.3f, \"qmax_lifetime\": %.3f, \"qmax_thermal\": %.3f, \"I\": %.3f, "
                 "\"I_loss\": %.3f, \"SOC\": %.3f, \"DOD\": %.3f, \"DOD_prev\": %.3f, "
                 "\"charge_mode\": %d, \"prev_charge\": %d, \"chargeChange\": %d, "
                 "\"leadacid\": { \"q1_0\": %.3f, \"q2_0\": %.3f, \"q1\": %.3f, \"q2\": %.3f } }",
            p.q0, p.qmax_lifetime, p.qmax_thermal, p.I,
            p.I_loss, p.SOC, p.DOD, p.DOD_prev,
            p.charge_mode, p.prev_charge, p.chargeChange,
            p.leadacid.q1_0, p.leadacid.q2_0, p.leadacid.q1, p.leadacid.q2);
    os << buf;
    return os;
}

std::ostream &operator<<(std::ostream &os, const capacity_params &p) {
    char buf[1024];
    sprintf(buf, "capacity_params: { \"qmax_init\": %.3f, \"SOC_init\": %.3f, \"SOC_max\": %.3f, "
                 "\"SOC_min\": %.3f, \"dt_hr\": %.3f, "
                 "\"leadacid\": { \"t1\": %.3f, \"t2\": %.3f, \"F1\": %.3f, \"F2\": %.3f, "
                 "\"q10\": %.3f, \"q20\": %.3f, \"I20\": %.3f} }", p.qmax_init, p.SOC_init, p.SOC_max,
            p.SOC_min, p.dt_hr, p.leadacid.t1, p.leadacid.t2, p.leadacid.F1, p.leadacid.F2,
            p.leadacid.q10, p.leadacid.q20, p.leadacid.I20);
    os << buf;
    return os;
}

std::ostream &operator<<(std::ostream &os, const cycle_state &p) {
    char buf[1024];
    sprintf(buf, "cycle_state: { \"q_relative_cycle\": %.3f, \"n_cycles\": %d, \"range\": %.3f, \"average_range\": %.3f, "
                 "\"rainflow_Xlt\": %.3f, \"rainflow_Ylt\": %.3f, \"rainflow_jlt\": %d, \"peaks\": ",
            p.q_relative_cycle, p.n_cycles, p.range, p.average_range,
            p.rainflow_Xlt, p.rainflow_Ylt, p.rainflow_jlt);
    os << buf << p.peaks << " }";
    return os;
}

std::ostream &operator<<(std::ostream &os, const calendar_state &p) {
    char buf[1024];
    sprintf(buf, "calendar_state: { \"q_relative_calendar\": %.3f, \"day_age_of_battery\": %d, \"last_idx\": %zd, "
                 "\"dq_relative_calendar_old\": %.3f }",
            p.q_relative_calendar, p.day_age_of_battery, p.last_idx, p.dq_relative_calendar_old);
    os << buf;
    return os;
}

std::ostream &operator<<(std::ostream &os, const lifetime_state &p) {
    os.precision(3);
    os << R"(lifetime_state : { "q_relative": )" << p.q_relative << ", " << *p.cycle << ", " << *p.calendar << " }";
    return os;
}

std::ostream &operator<<(std::ostream &os, const lifetime_params &p) {
    os << R"(lifetime_params: { "cycling_matrix": )" << p.cycling_matrix;

    char buf[1024];
    sprintf(buf, ", \"calendar_choice\": %d, \"dt_hour\": %.3f, \"calendar_model_q0\": %.3f, "
                 "\"calendar_model_a\": %.3f, \"calendar_model_b\": %.3f, "
                 "\"calendar_model_c\": %.3f, ", p.calendar_choice, p.dt_hour, p.calendar_model_q0,
            p.calendar_model_a, p.calendar_model_b, p.calendar_model_c);
    os << buf;
    os << R"("calendar_matrix": )" << p.calendar_matrix << " }";
    return os;
}

std::ostream &operator<<(std::ostream &os, const replacement_state &p) {
    char buf[256];
    sprintf(buf, R"(replacement_state: { "n_replacements": %d, "indices_replaced": )", p.n_replacements);
    os << buf << p.indices_replaced << " }";
    return os;
}

std::ostream &operator<<(std::ostream &os, const replacement_params &p) {
    char buf[256];
    sprintf(buf, R"(replacement_params: {"option": %d, "capacity_percent": %.3f, )", p.option, p.capacity_percent);
    os << buf;
    os << R"("schedule": )" << p.schedule << R"(", "schedule_percent_to_replace": )" << p.schedule_percent_to_replace;
    os << " }";
    return os;
}
