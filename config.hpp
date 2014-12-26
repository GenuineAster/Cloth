#ifndef CONFIG_HPP
#define CONFIG_HPP
using PrecisionType = float;
constexpr int stps_pr_itr{5};
constexpr PrecisionType nd_mss{0.01f};
constexpr PrecisionType grvt_x{2.f}, grvt_y{10.f};
constexpr PrecisionType strt_x{100.f}, strt_y{100.f};
constexpr PrecisionType sz_x{300.f}, sz_y{300.f};
constexpr int grd_sz_x{500}, grd_sz_y{10};
constexpr PrecisionType constraint_length{sz_x/grd_sz_x};
constexpr PrecisionType constraint_resistance{10.f};
constexpr PrecisionType max_constraint_length{1000.f};
constexpr int grd_sz{grd_sz_x*grd_sz_y};
constexpr int cnstrnts_sz{grd_sz-grd_sz_x-grd_sz_y};
constexpr PrecisionType ms_grb_rad{100.f};
constexpr bool ms_grb_whn_stpd{true};
constexpr PrecisionType sim_spd{5.f};
#endif
