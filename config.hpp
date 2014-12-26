#ifndef CONFIG_HPP
#define CONFIG_HPP
using PrecisionType = float;
constexpr int stps_pr_itr{1};
constexpr PrecisionType nd_mss{0.5f};
constexpr PrecisionType grvt_x{0.f}, grvt_y{10.f};
constexpr PrecisionType strt_x{100.f}, strt_y{100.f};
constexpr PrecisionType sz_x{300.f}, sz_y{300.f};
constexpr int grd_sz_x{100}, grd_sz_y{100};
constexpr PrecisionType constraint_length{sz_x/grd_sz_x};
constexpr PrecisionType max_constraint_length{50.f};
constexpr int grd_sz{grd_sz_x*grd_sz_y};
constexpr int cnstrnts_sz{grd_sz-grd_sz_x-grd_sz_y};
constexpr PrecisionType ms_grb_rad{50.f};
#endif
