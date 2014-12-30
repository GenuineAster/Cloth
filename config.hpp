#ifndef CONFIG_HPP
#define CONFIG_HPP
	using PrecisionType = float;
	int stps_pr_itr{30};
	PrecisionType nd_mss{0.01f};
	PrecisionType grvt_x{0.1f}, grvt_y{10.f};
	PrecisionType grvt_dmp{0.005f};
	PrecisionType strt_x{100.f}, strt_y{100.f};
	PrecisionType sz_x{300.f}, sz_y{300.f};
	int grd_sz_x{50}, grd_sz_y{25};
	PrecisionType constraint_length{(sz_x/grd_sz_x)*3.3f};
	PrecisionType constraint_resistance{1000.f};
	PrecisionType max_constraint_length{100.f};
	int grd_sz{grd_sz_x*grd_sz_y};
	int cnstrnts_sz{grd_sz-grd_sz_x-grd_sz_y};
	PrecisionType ms_grb_rad{15.f};
	bool ms_grb_whn_stpd{true};
	PrecisionType sim_spd{20.f};
#endif
