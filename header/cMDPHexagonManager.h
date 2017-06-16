// @REV_TAG SRL_10_2013
//
//													 Sergio Ruiz Loza. A01106919@itesm.mx
//																	  All Rights Reserved
//													  ITESM-CCM (http://www.ccm.itesm.mx)
//												 Computer Science Ph.D. Thesis Subproject
//										   Global Thesis Project: "Crowd Motion Planning"
//							Thesis Director: Dr. Benjamín Hernández A. hbenjamin@itesm.mx
//											   Programmed in C++ for OpenGL 3.1 and newer
//  														   Requires GLEW and FREEGLUT
//
//
//															   See ReadMe.txt for details
//
//=======================================================================================

#pragma once

#include "cLogManager.h"

#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include "mdp_gpu.h"
#include "mdp_modular_gpu.h"

using namespace std;

//=======================================================================================

#ifndef __MDP_HEXAGON_MANAGER
#define __MDP_HEXAGON_MANAGER

class MDPHexagonManager
{
public:
							MDPHexagonManager		(	LogManager*			log_manager_	);
							~MDPHexagonManager		(	void								);

	bool					solve_from_csv			(	string&				csvFilename,
														vector<float>&		policy			);
	void					get_policy				(	vector<float>&		policy			);
	int						getRows					(	void								);
	int						getColumns				(	void								);
	int						getIterations			(	void								);
	bool					getConvergence			(	void								);

	bool					in_bounds				(	int					r,
														int					c,
														int					q				);
	int						mdp_get_index			(	int					row,
														int					column,
														int					iteration		);
	int						mdp_get_qindex			(	int					row,
														int					column,
														int					iteration,
														int					q				);
	int						mdp_get_index			(	int					row,
														int					column,
														int					iteration,
														int					direction		);
	int						mdp_get_qindex			(	int					row,
														int					column,
														int					iteration,
														int					direction,
														int					q				);

//->FOR_INT_VECTORS
	int						mdp_get_value			(	int					row,
														int					column,
														int					iteration,
														vector<int>&		mdp_vector		);
	int						mdp_get_value			(	int					row,
														int					column,
														int					iteration,
														int					direction,
														vector<int>&		mdp_vector		);
	int						mdp_get_qvalue			(	int					row,
														int					column,
														int					iteration,
														int					direction,
														int					q,
														vector<int>&		mdp_vector		);
//<-FOR_INT_VECTORS

//->FOR_FLOAT_VECTORS
	float					mdp_get_value			(	int					row,
														int					column,
														int					iteration,
														vector<float>&		mdp_vector		);
	float					mdp_get_value			(	int					row,
														int					column,
														int					iteration,
														int					direction,
														vector<float>&		mdp_vector		);
	float					mdp_get_qvalue			(	int					row,
														int					column,
														int					iteration,
														int					direction,
														int					q,
														vector<float>&		mdp_vector		);
//<-FOR_FLOAT_VECTORS

	bool					mdp_is_wall				(	int					row,
														int					column			);
	bool					mdp_is_exit				(	int					row,
														int					column			);
	void					split					(	const string&		str,
														const string&		delimiters,
														vector<string>& 	tokens			);
	vector<float>			read_matrix				(	string				file_name		);
	void					write_policy			(	void								);
	void					print_rewards			(	void								);
	void					print_mdp				(	void								);
	void					init_mdp				(	void								);
	void					getRewards				(	vector<float>&		mdp				);

//->SINGLE_ITERATION_PROCESS
	void					init_structures_on_host	(	vector<float>&		mdp_topology	);
	void					init_perms_on_device	(	void								);
	void					upload_to_device		(	void								);
	void					iterate_on_device		(	void								);
	void					download_to_host		(	void								);
//<-SINGLE_ITERATION_PROCESS

private:
	bool					is_even					(	int					number			);
	bool					is_odd					(	int					number			);

	LogManager*				log_manager;

	int						cells;
	int						rows;
	int						columns;
	const int				NQ;
	float					discount;
	bool					convergence;
	float					iteratingTime;
	int						iterations;

	vector<int>				reachable;
	vector<float>			Q;								// NQ-sized
	vector<float>			V;
	vector<int>				P;
	vector<int>				is_wall;
	vector<float>			rewards;
	vector<float>			host_probability1;
	vector<float>			host_probability2;
	vector<float>			host_permutations;
	vector<int>				host_vicinity;
	vector<int>				host_vicinity6;
	vector<float>			iterationTimes;
// declare sieves and shifts for convolution:
	vector<float>			host_dir_rw_sieve;
	vector<vector<float>>	host_dir_rw_shifts;
	vector<float>			host_dir_pv_sieve;
	vector<vector<float>>	host_dir_pv_shifts;
	vector<int>				host_dir_ib_sieve;
	vector<vector<int>>		host_dir_ib_shifts;
	vector<int>				host_dir_iw_sieve;
	vector<vector<int>>		host_dir_iw_shifts;
	vector<float>			host_dir_rw;
	vector<float>			host_dir_rw6;
	vector<float>			host_dir_pv;
	vector<float>			host_dir_pv6;
	vector<int>				host_dir_ib;
	vector<int>				host_dir_iw;
	vector<int>				host_dir_ib_iw;
	vector<int>				host_dir_ib_iw6;
	vector<int>				host_dir_ib_iw6_inv;
};

#endif

//=======================================================================================
