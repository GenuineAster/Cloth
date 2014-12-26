#ifndef CONFIG_HPP
#define CONFIG_HPP
	using PrecisionType = long double;
	int stps_pr_itr{20};
	PrecisionType nd_mss{0.01f};
	PrecisionType grvt_x{0.1f}, grvt_y{0.2f};
	PrecisionType grvt_dmp{0.1f};
	PrecisionType strt_x{100.f}, strt_y{100.f};
	PrecisionType sz_x{300.f}, sz_y{300.f};
	int grd_sz_x{100}, grd_sz_y{50};
	PrecisionType constraint_length{sz_x/grd_sz_x};
	PrecisionType constraint_resistance{20.f};
	PrecisionType max_constraint_length{300.f};
	int grd_sz{grd_sz_x*grd_sz_y};
	int cnstrnts_sz{grd_sz-grd_sz_x-grd_sz_y};
	PrecisionType ms_grb_rad{100.f};
	bool ms_grb_whn_stpd{true};
	PrecisionType sim_spd{20.f};
#endif
