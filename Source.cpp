/*
	Programmer's (student's) name: Humberto Pedraza
	Class: CMPE 4350: Artifical Intelligence
	Due Date: 2015 May 5

	Program: Trolls and burglar using Q-Learning
*/



#include "functions.h"
#include <limits>

using namespace std;

int main()
{

	//-- VARIABLES LIST --//
	srand(time(NULL));
	int number_of_ponies_saved = 0;
	int episodes = 10000;  //number of runs burglar will try reaching his/her goal
	double alpha = 0.5;  // learning factor
	double gamma = 0.5;   // reward discount factor
	vector<vector<double> > Q; // Q matrix
	vector<vector<double> > R; // Reward matrix
	vector<vector<char> > graph; // n by n graph
	vector<vector<char> > back_up_graph;
	vector<vector<char> > final_graph;
	vector<vector<int> > been_at_that_state_many_times;
	vector<vector <char> > has_visited;
	vector<int> final_path_taken;
	ifstream input;
	ofstream output;
	
	output.open("DISPLAY.txt");
	vector<int> pony_coordinates;
	vector<int> troll_coordinates;
	vector<int> obstructions;
	int number_of_ponies;
	int number_of_trolls;
	int number_of_obsructions = 0;
	int N;
	int escape_x;
	int escape_y;
	bool been_eaten = false;
	bool burglar_at_escape = false;
	int next_x;
	int next_y;
	int burglar_x;
	int burglar_y;
	
	double simulated_annealing_beta = .03;
	double double_random_chance = .99;
	int normalize_Q_counter = 0;
	int moves = 0;
	int moves_in_current_state = 0;
	bool do_it = true;
	int check_every_this = 0; // check every ten
	int final_saved;
	char text_in[25];
	//-- END OF VARIABLE LIST
	cout << "Please insert file name used to set up graph: ";
	cin >> text_in;
	cout << endl;
	input.open(text_in);

	read_input(pony_coordinates, troll_coordinates, obstructions , input, escape_x,escape_y , N , number_of_ponies,number_of_trolls , number_of_obsructions);
	input.close();

	// Initialize graph: set ponies, trolls, obstrcutions and reward matrix
	make_graph(graph , pony_coordinates , troll_coordinates ,obstructions, N , number_of_ponies, number_of_trolls , number_of_obsructions , R , escape_x , escape_y , back_up_graph , has_visited,been_at_that_state_many_times);

	number_of_ponies = pony_coordinates.size() / 2;

	// intialize q matrix
	make_q_matrix(Q , N);


	// For each episode:
	for (int epoch = 0; epoch < episodes; epoch++){
		//Select a random initial state for each epoch.
		put_burglar_in_random_state(graph , burglar_x , burglar_y , has_visited);

		// Do While the goal state, 'E', hasn't been reached.
		do{
			
				//-- Select one among all possible actions for the current state. --//
			select_next_state(graph, next_x, next_y, burglar_x, burglar_y, Q, double_random_chance , has_visited , moves_in_current_state);

				//-- Using this possible action, consider going to the next state. --//
			check_what_is_on_next_state(number_of_ponies_saved , graph ,next_x , next_y, been_eaten , burglar_at_escape);

				//-- Get maximum Q value for this next state based on all possible actions. --//
				// -- check and see if you have been eaten by a troll or you have saved a pony.
				//-- Compute: Q(state, action) = Q(state,action) + alpha * R(state, action) + gamma * Max[Q(next state, all actions)] --//
			compute_Q(Q , next_x , next_y , R, alpha , gamma , burglar_x , burglar_y , escape_y , escape_x , troll_coordinates , pony_coordinates , has_visited , been_at_that_state_many_times);




			// print beginning state and final state, if it is the final epidose print out every move
			if ( (check_every_this >= 25 && do_it) || epoch == episodes-1){
				//print_graph(graph);
				do_it = false;
				output << "EPOCH #" << epoch << endl;
				output << "Ponies Saved: " <<  number_of_ponies_saved << " out of " << number_of_ponies  << endl;
				if (epoch == episodes - 1){
					do_it = true;
					cout << "EPOCH #" << epoch << endl;
					cout << "Ponies Saved: " <<  number_of_ponies_saved << " out of " << number_of_ponies  << endl;
					for (int i = 0; i < graph.size(); i++){
						for (int j = 0; j < graph.size(); j++){
							cout << graph[i][j];
							if(graph[i][j] == 'B'){ final_path_taken.push_back(j); final_path_taken.push_back(i);
							}
					}
							cout << endl;
				}
							cout << endl << endl << endl << endl;	
				}

				for (int i = 0; i < graph.size(); i++){
					for (int j = 0; j < graph.size(); j++){
						output << graph[i][j];
					}
					output << endl;
				}
				output << endl << endl << endl << endl;				
			}

			//-- Set the next state as the current state. --//
			burglar_moves(graph, next_x, next_y, burglar_x, burglar_y, has_visited,been_at_that_state_many_times);
			if (moves == 500){
				normalize_Q(Q, moves, escape_y, escape_x);
				moves = 0;

			}

			moves++;
			moves_in_current_state++;

		} while (!(burglar_at_escape) && !(been_eaten)); 

		// end of an epoch
		moves_in_current_state = 0;

		// print final state
		if (check_every_this >= 25){
			do_it = true;
			output << "EPOCH #" << epoch << endl;
			output << "Ponies Saved: " <<  number_of_ponies_saved << " out of " << number_of_ponies  << endl;

			if(epoch == episodes){
					cout << "EPOCH #" << epoch << endl;
					cout << "Ponies Saved: " <<  number_of_ponies_saved << " out of " << number_of_ponies  << endl;
					for (int i = 0; i < graph.size(); i++){
						for (int j = 0; j < graph.size(); j++){
							cout << graph[i][j];
							if(graph[i][j] == 'B'){ final_path_taken.push_back(j); final_path_taken.push_back(i);
							}
					}
							cout << endl;
				}
							cout << endl << endl << endl << endl;
			}

			for (int i = 0; i < graph.size(); i++){
				
				for (int j = 0; j < graph.size(); j++){
					output << graph[i][j];
				}
				output << endl;
			}
			output << endl << endl << endl << endl;
			check_every_this = 0;
		}

		final_graph = graph;
		final_saved = number_of_ponies_saved;
		reset_graph(graph, burglar_at_escape, been_eaten, back_up_graph, has_visited);
		number_of_ponies_saved = 0;
		cool_down_random_chance(double_random_chance, simulated_annealing_beta);
		normalize_Q_counter++;
		check_every_this++;
	} // END of for loop episodes

	//print_graph(graph);

			output << "EPOCH #" << episodes << endl;
			output << "Ponies Saved: " <<  final_saved << " out of " << number_of_ponies  << endl;

			
					cout << "EPOCH #" << episodes << endl;
					cout << "Ponies Saved: " <<  final_saved << " out of " << number_of_ponies  << endl;
					for (int i = 0; i < graph.size(); i++){
						for (int j = 0; j < graph.size(); j++){
							cout << final_graph[i][j];
							if(final_graph[i][j] == 'B'){ final_path_taken.push_back(j); final_path_taken.push_back(i);
							}
					}
							cout << endl;
				}
							cout << endl << endl << endl << endl;
			

			for (int i = 0; i < graph.size(); i++){
				
				for (int j = 0; j < graph.size(); j++){
					output << final_graph[i][j];
				}
				output << endl;
			}
			output << endl << endl << endl << endl;
		
		

	for(int i = 0; i < final_path_taken.size(); i+=2){
		cout << "[" << final_path_taken[i] << "," << final_path_taken[i+1] << "]" << ",";
		output << "[" << final_path_taken[i] << "," << final_path_taken[i+1] << "]" << ",";
	}
	output.close();


	system("pause");
	return 0;
}