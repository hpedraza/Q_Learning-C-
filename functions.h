/*
	Programmer's (student's) name: Humberto Pedraza
	Class: CMPE 4350: Artifical Intelligence
	Due Date: 2015 May 5

	Program: Trolls and burglar using Q-Learning
*/

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <stdio.h>      
#include <stdlib.h>     
#include <time.h>
#include <algorithm>
#include <math.h>
/*
		work on has_been vector
*/

using namespace std;

void parse_line(int line_number , string temp , int& number_of_ponies , int& number_of_trolls , int& n , int& escape_x, int& escape_y , vector<int>& pony_coordinates , vector<int>& troll_coordinates, vector<int> obstructions , int& number_of_ob){
	//cout << "IN PARSE LINE" << endl;

	if (line_number == 0){ // line 1
		vector<char> numPon;
		vector<char> Graph_Size;
		vector<char> numTroll;
		int tracker = 0;

		for (int i = 0; i < temp.size(); i++){
			if (temp[i] != ' ' && tracker == 0){
				Graph_Size.push_back(temp[i]);
				
			}
			else if (temp[i] != ' ' && tracker == 1) {
				numTroll.push_back(temp[i]);
			}
			else if (temp[i] != ' ' && tracker == 2) {
				numPon.push_back(temp[i]);
			}
			else{
				tracker++;
			}
		} // END OF FOR LOOP
		string s = string(Graph_Size.begin() , Graph_Size.end());
		string r = string(numPon.begin(),numPon.end());
		string t = string(numTroll.begin() , numTroll.end());
		n = atoi(s.c_str());
		number_of_ponies = atoi(r.c_str());
		number_of_trolls = atoi(t.c_str());
		

	} // END OF LINE 1

	else if (line_number == 1){ // line 2 (escape coordinates)
		vector<char> x;
		vector<char> y;
		int tracker = 0;
		for (int i = 0; i < temp.size(); i++){
			if (temp[i] != ' ' && tracker == 0){
				x.push_back(temp[i]);
			}
			else if (temp[i] != ' ' && tracker == 1){
				y.push_back(temp[i]);
			}
			else{
				tracker++;
			}
		} // END OF FOR LOOP
		string a = string(x.begin() , x.end());
		string b = string(y.begin() , y.end());
		escape_x = atoi(a.c_str());
		escape_y = atoi(b.c_str());

	} // END OF LINE 2

	else if (line_number == 2){ // line 3 pony coordinates
		string a;
		for (int i = 0; i < temp.size(); i++){
	
			if (temp[i] == ' ' || i == (temp.size() - 1) ){
				if (i == (temp.size() - 1))
					a += temp[i];
				pony_coordinates.push_back(atoi(a.c_str()));
				//a.clear;
				a = "";
			}
			else{
				a += temp[i];
			}
		}
	} // END OF LINE 3
	else if (line_number == 3){ // line 4 coordinates of obstructions
		string a;
		for (int i = 0; i < temp.size(); i++){
			if (temp[i] == ' ' || i == (temp.size() - 1)){
				if (i == (temp.size() - 1))
					a += temp[i];
				obstructions.push_back(atoi(a.c_str()));
				number_of_ob++;
				
				a = "";
			}
			else if (temp[i] == '-'){
				number_of_ob = 0;
				break;
			}
			else{
				a += temp[i];
			}
		}
	}// END OF LINE 4
	else if (line_number == 4){ // line 5 coordinates of troll locations
		string a;
		for (int i = 0; i < temp.size(); i++){
			if (temp[i] == ' ' || i == (temp.size() - 1)){
				if (i == (temp.size() - 1))
					a += temp[i];
				troll_coordinates.push_back(atoi(a.c_str()));
				
				a = "";
			}

			else {
				a += temp[i];
			}
		}
	}// END OF LINE 5

}
void read_input(vector<int>& pony_coordinates, vector<int>& troll_coordinates,vector<int>& obstructions, ifstream& input, int& escape_x,int& escape_y, int& n , int& number_of_ponies, int& number_of_trolls , int& number_of_obstructions){
	// text file has five lines
	/*
		10 1 6 || Line 1: 10 = value of N, 1 = number of trolls, 6 = # of trolls
		8 3 || Line 2 : Coordinates of E, escape location
		4 2 4 4 5 5 6 5 8 5 3 6 || Line 3: pony coordinates
		- 1 - 1 || Line 4: coordinates for obstructions
		1 7 || Line 5: coordinates of troll locations

		while (!(graph.eof())){
		getline(graph , temp);
		Parse_Line(Graph , temp , Graph_Size);
		}

	*/
	//cout << "IN READ INPUT" << endl;
	string temp;
	int line_number = 0;


	while (!(input.eof())){
		getline(input, temp);
		parse_line(line_number,temp , number_of_ponies, number_of_trolls , n, escape_y,escape_x , pony_coordinates , troll_coordinates,obstructions , number_of_obstructions);
		line_number++;
	}
}

void make_graph(vector<vector<char> >& graph, vector<int> pony_coordinates, vector<int> troll_coordinates, vector<int> obstructions, int& N, int number_of_ponies, int number_of_trolls, int number_of_obstructions, vector<vector<double> >& R, int escape_x, int escape_y, vector<vector<char> >& back_up_graph, vector<vector<char> >& has_been , vector<vector<int> >& many_graph){
	// intialize reward matrix too
	//cout << "IN MAKE GRAPH" << endl;
	graph.resize(N); // create n number of vectors
	R.resize(N); // Reward matrix
	has_been.resize(N);
	many_graph.resize(N);

	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++){
			graph[i].push_back('-');

			R[i].push_back(1);

			has_been[i].push_back('N');
			many_graph[i].push_back(0);

		}
	}
	graph[escape_y][escape_x] = 'E';
	R[escape_y][escape_x] = 11;

	// put ponies in graph
	for (int i = 0; i < number_of_ponies*2;i += 2){
		graph[pony_coordinates[i+1]][pony_coordinates[i]] = 'P';
		R[pony_coordinates[i+1]][pony_coordinates[i]] = 10;

	}
	
	// put trolls in graph
	for (int i = 0; i < number_of_trolls*2; i+=2){
		graph[troll_coordinates[i+1]][troll_coordinates[i]] = 'T';
		R[troll_coordinates[i+1]][troll_coordinates[i]] = -11;
	}

	// put obstructions in graph
	for (int i = 0; i < number_of_obstructions*2; i++){
		graph[obstructions[i+1]][obstructions[i]] = 'O';
	}
	back_up_graph = graph;
}

void make_q_matrix(vector<vector<double> >& Q , int N){
	//cout << "IN MATRIC FUNC" << endl;
	Q.resize(N);
	for (int i = 0; i < N; i++){
		for (int j = 0; j < N; j++)
			Q[i].push_back(0);
	}
}



void put_burglar_in_random_state(vector<vector<char> >& graph, int& burglar_x, int& burglar_y, vector<vector<char> >& has_been){
	//cout << "------------------------>>>>>IN PUT BURGLAR IN RANDOM STATE" << endl;
	int x, y;
	int N = graph.size();
	

	x = rand() % N;
	y = rand() % N;
	if (x == N){
		x--;
	}
	if (y == N){
		y--;
	}
	bool burglar_in_graph = false;
	
	while (!(burglar_in_graph)){
		if (graph[y][x] == '-'){
			graph[y][x] = 'B';
			burglar_in_graph = true;
			burglar_y = y;
			burglar_x = x;
			has_been[y][x] = 'Y';
		}
		else{
			x = rand() % N;
			y = rand() % N;
		}

		

	} 
}
void reset_has_been_matrix(vector<vector<char> >& has_been){
	for (int i = 0; i < has_been.size(); i++){
		for (int j = 0; j < has_been.size(); j++)
			has_been[i][j] = 'N';
	}

}

void select_next_state(vector<vector<char> > graph, int& x, int& y, int burglar_x, int burglar_y, vector<vector<double> > Q, double double_random_chance, vector<vector<char> >& has_been , int moves){
	// choose next path evaluating q matrix, use E-Greedy with a small probability it will choose another.
	//cout << "IN SELECT NEXT STATE" << endl;
	int N = graph.size();
	string direction;
	bool loop_controller = true;
	string optimal;
	double ran = rand() % 100 + 1;
	bool choose_random_path = false;
	bool has_been_stuck = true;

	int tempX = burglar_x;
	int tempY = burglar_y;
	if (moves > 350){ double_random_chance = 100; }
	if (ran * .01 <= double_random_chance){
		choose_random_path = true;
	}

	bool obstruction = false;
	if (choose_random_path == false){
		while (!(obstruction)){
			double m = -9999;
			// if burgar at top left corner
			if (burglar_x == 0 && burglar_y == 0){
				if (Q[burglar_y + 1][burglar_x] >= Q[burglar_y + 1][burglar_x + 1] && has_been[burglar_y + 1][burglar_x] == 'N'){ // if down is greater than its down right
					m = Q[burglar_y + 1][burglar_x];
					y= burglar_y + 1;
					x= burglar_x;
					has_been_stuck = false;
				}
				else if (has_been[burglar_y + 1][burglar_x + 1] == 'N'){
					m = Q[burglar_y + 1][burglar_x + 1];
					x = burglar_x + 1;
					y = burglar_y + 1;
					has_been_stuck = false;
				}
			

				// if its right is greater than the other two options
				if (m <= Q[burglar_y][burglar_x + 1] && has_been[burglar_y][burglar_x + 1] == 'N'){
					m = Q[burglar_y][burglar_x + 1];
					y = burglar_y;
					x = burglar_x + 1;
					has_been_stuck = false;
				}

				if (has_been_stuck == true){
					reset_has_been_matrix(has_been);
				}
				


			}

			// if at bottom left corner
			else if (burglar_y == N - 1 && burglar_x == 0){
				if (Q[burglar_y - 1][burglar_x] >= Q[burglar_y - 1][burglar_x + 1] && has_been[burglar_y - 1][burglar_x] == 'N'){// compare up with up right
						m = Q[burglar_y - 1][burglar_x];
						y = burglar_y - 1;
						x= burglar_x;
						has_been_stuck = false;
					}
				else if (has_been[burglar_y - 1][burglar_x + 1] == 'N'){
						m = Q[burglar_y - 1][burglar_x + 1];
						y = burglar_y - 1;
						x= burglar_x + 1;
						has_been_stuck = false;
					}

					// compare with right
				if (m <= Q[burglar_y][burglar_x + 1] && has_been[burglar_y][burglar_x + 1] == 'N'){
						m = Q[burglar_y][burglar_x + 1];
						y = burglar_y;
						x = burglar_x + 1;
						has_been_stuck = false;
					}

				else if (has_been_stuck == true){
					reset_has_been_matrix(has_been);
				}


				}

			    // if burglar is at top right corner
				else if (burglar_y == 0 && burglar_x == N - 1){
					if (Q[burglar_y][burglar_x - 1] >= Q[burglar_y + 1][burglar_x] && has_been[burglar_y][burglar_x - 1] == 'N'){ // compare left with down
						m = Q[burglar_y][burglar_x - 1];
						y = burglar_y;
						x = burglar_x - 1;
						has_been_stuck = false;
					}
					else if (has_been[burglar_y + 1][burglar_x] == 'N'){
						m = Q[burglar_y + 1][burglar_x];
						y= burglar_y + 1;
						x = burglar_x;
						has_been_stuck = false;
					}


					if (m <= Q[burglar_y + 1][burglar_x - 1] && has_been[burglar_y + 1][burglar_x - 1] == 'N'){ // compare wih left-down
						m = Q[burglar_y + 1][burglar_x - 1];
						y = burglar_y + 1;
						x = burglar_x - 1;
						has_been_stuck = false;
					}
					else if (has_been_stuck == true){
						reset_has_been_matrix(has_been);
					}

				}

				// if burglar is at bottom right corner
				else if (burglar_y == N - 1 && burglar_x == N - 1){ 
					if (Q[burglar_y - 1][burglar_x] >= Q[burglar_y - 1][burglar_x - 1] && has_been[burglar_y - 1][burglar_x] == 'N'){ // compare up with up-left
						m = Q[burglar_y - 1][burglar_x];
						y = burglar_y - 1;
						x = burglar_x;
						has_been_stuck = false;
					}
					else if (has_been[burglar_y - 1][burglar_x - 1] == 'N'){
						m = Q[burglar_y - 1][burglar_x - 1];
						y = burglar_y - 1;
						x = burglar_x - 1;
						has_been_stuck = false;
					}


					if (m <= Q[burglar_y][burglar_x - 1] && has_been[burglar_y][burglar_x - 1] == 'N'){ // comapre with left
						m = Q[burglar_y][burglar_x - 1];
						y = burglar_y;
						x = burglar_x - 1;
						has_been_stuck = false;
					}
					else if (has_been_stuck == true){
						reset_has_been_matrix(has_been);
					}

				}

				else if (burglar_y == 0){// is on top edge but not on a corner
					double n =-9999;
					int mx, my, nx, ny;
					if (Q[burglar_y][burglar_x - 1] >= Q[burglar_y + 1][burglar_x - 1] && has_been[burglar_y][burglar_x - 1] == 'N'){ // compare left with down-left
						m = Q[burglar_y][burglar_x - 1];
						my = burglar_y;
						mx = burglar_x - 1;
						
					}
					else if (has_been[burglar_y + 1][burglar_x - 1] == 'N'){
						m = Q[burglar_y + 1][burglar_x - 1];
						my = burglar_y + 1;
						mx = burglar_x - 1;
						
					}
					if (Q[burglar_y + 1][burglar_x] >= Q[burglar_y + 1][burglar_x + 1] && has_been[burglar_y + 1][burglar_x] == 'N'){ // comapre down with down-right
						n = Q[burglar_y + 1][burglar_x];
						ny = burglar_y + 1;
						nx = burglar_x;
						
					}
					else if (has_been[burglar_y + 1][burglar_x + 1] == 'N'){
						n = Q[burglar_y + 1][burglar_x + 1];
						ny = burglar_y + 1;
						nx = burglar_x + 1;
					
					}

					
					if (m >= n  && (m != -9999)){
						if (m >= Q[burglar_y][burglar_x+ 1]){ // compare with right
							y = my;
							x = mx;
							has_been_stuck = false;
						}
						else if (has_been[burglar_y][burglar_x + 1] == 'N'){
							y = burglar_y;
							x = burglar_x + 1;
							has_been_stuck = false;
						}
					}

					else if(n != -9999){
						if (n >= Q[burglar_y][burglar_x + 1]){// compare with right
							y = ny;
							x = nx;
							has_been_stuck = false;
						}
						else if (has_been[burglar_y][burglar_x + 1] == 'N'){
							y = burglar_y;
							x = burglar_x + 1;
							has_been_stuck = false;
						}

					}

					if (has_been_stuck == true){
						reset_has_been_matrix(has_been);
					}

				}

				// if burglar is at right edge but not on corner
				else if (burglar_x == N - 1){//
					double n = -9999;
					int mx, my, nx, ny;
					if (Q[burglar_y - 1][burglar_x] >= Q[burglar_y - 1][burglar_x - 1] && has_been[burglar_y - 1][burglar_x] == 'N'){ // compare up with up-left
						m = Q[burglar_y - 1][burglar_x];
						my = burglar_y - 1;
						mx = burglar_x;
						
					}
					else if (has_been[burglar_y - 1][burglar_x - 1] == 'N'){
						m = Q[burglar_y - 1][burglar_x - 1];
						my = burglar_y - 1;
						mx = burglar_x - 1;
						
					}
					if (Q[burglar_y][burglar_x - 1] >= Q[burglar_y + 1][burglar_x - 1] && has_been[burglar_y][burglar_x - 1] == 'N'){ // compare left with down left
						n = Q[burglar_y][burglar_x - 1];
						ny = burglar_y;
						nx = burglar_x - 1;
						
					}
					else if (has_been[burglar_y + 1][burglar_x - 1] == 'N'){
						n = Q[burglar_y + 1][burglar_x - 1];
						ny = burglar_y + 1;
						nx= burglar_x- 1;
					
					}

					if (m >= n && m != -9999){ // compare with down
						if (m >= Q[burglar_y + 1][burglar_x]){ 
							y = my;
							x = mx;
							has_been_stuck = false;
						}
						else if (has_been[burglar_y + 1][burglar_x]){
							y = burglar_y+ 1;
							x = burglar_x;
							has_been_stuck = false;
						}
					}

					else if(n != -9999){
						if (n > Q[burglar_y + 1][burglar_x]){
							y = ny;
							x= nx;
							has_been_stuck = false;
						}
						else if (has_been[burglar_y + 1][burglar_x]){
							y = burglar_y + 1;
							x = burglar_x;
							has_been_stuck = false;
						}

					}

					if (has_been_stuck == true){
						reset_has_been_matrix(has_been);
					}

				}


				// if burglar is at botom edge but not on a corner
				else if (burglar_y == N - 1){
					double n = -9999;
					int mx, my, nx, ny;
					if (Q[burglar_y][burglar_x - 1] >= Q[burglar_y - 1][burglar_x - 1] && has_been[burglar_y][burglar_x - 1] == 'N'){ // left with up-left
						m = Q[burglar_y][burglar_x - 1];
						my= burglar_y;
						mx= burglar_x - 1;
						
					}
					else if (has_been[burglar_y - 1][burglar_x - 1] == 'N'){
						m = Q[burglar_y - 1][burglar_x - 1];
						my = burglar_y - 1;
						mx = burglar_x - 1;
						
					}

					if (Q[burglar_y - 1][burglar_x] >= Q[burglar_y - 1][burglar_x + 1] && has_been[burglar_y - 1][burglar_x] == 'N'){ // up with up-right
						n = Q[burglar_y - 1][burglar_x];
						ny = burglar_y - 1;
						nx= burglar_x;
						
					}
					else if (has_been[burglar_y - 1][burglar_x + 1] == 'N'){
						n = Q[burglar_y - 1][burglar_x + 1];
						ny = burglar_y - 1;
						nx = burglar_x + 1;
						
					}

					if (m >= n && m != -9999){ // compare with right
						if (m >= Q[burglar_y][burglar_x + 1]){
							y = my;
							x = mx;
							has_been_stuck = false;
						}
						else if (has_been[burglar_y][burglar_x + 1] == 'N'){
							y= burglar_y;
							x = burglar_x + 1;
							has_been_stuck = false;
						}
					}

					else if(n != -9999){
						if (n >= Q[burglar_y][burglar_x + 1]){
							y = ny;
							x = nx;
							has_been_stuck = false;
						}
						else if (has_been[burglar_y][burglar_x + 1] == 'N'){
							y = burglar_y;
							x = burglar_x + 1;
							has_been_stuck = false;
						}

					}

					if (has_been_stuck == true){
						reset_has_been_matrix(has_been);
					}



				}

				// if burglar is at left edge but not in a corner
				else if (burglar_x == 0){
					double n = -9999;
					int mx, my, nx, ny;
					if (Q[burglar_y - 1][burglar_x] >= Q[burglar_y - 1][burglar_x + 1] && has_been[burglar_y - 1][burglar_x] == 'N'){ // up with up-rigt
						m = Q[burglar_y- 1][burglar_x];
						my = burglar_y - 1;
						mx = burglar_x;
					
					}
					else if (has_been[burglar_y - 1][burglar_x + 1] = 'N'){
						m = Q[burglar_y - 1][burglar_x + 1];
						my = burglar_y - 1;
						mx = burglar_x + 1;
						
					}

					if (Q[burglar_y][burglar_x + 1] >= Q[burglar_y + 1][burglar_x + 1] && has_been[burglar_y][burglar_x + 1] == 'N'){ // right with down-right
						n = Q[burglar_y][burglar_x + 1];
						ny = burglar_y;
						nx = burglar_x + 1;
						
					}
					else if (has_been[burglar_y + 1][burglar_x + 1] == 'N'){
						n = Q[burglar_y + 1][burglar_x + 1];
						ny = burglar_y + 1;
						nx = burglar_x + 1;
						
					}

					if (m >= n && m != -9999){ // compare with down
						if (m > Q[burglar_y + 1][burglar_x]){
							y = my;
							x = mx;
							has_been_stuck = false;
						}
						else if (has_been[burglar_y + 1][burglar_x] == 'N'){
							y = burglar_y + 1;
							x = burglar_x;
							has_been_stuck = false;
						}
					}

					else if(n != -9999){
						if (n > Q[burglar_y + 1][burglar_x]){
							y = ny;
							x = nx;
							has_been_stuck = false;
						}
						else if (has_been[burglar_y + 1][burglar_x] == 'N'){
							y = burglar_y + 1;
							x = burglar_x;
							has_been_stuck = false;
						}

					}

					if (has_been_stuck == true){
						reset_has_been_matrix(has_been);
					}

				}
				else{ // not on a corner
					double n, o, p;
					n = -9999;
					o = -9999;
					p = -9999;
					int mx, my, nx, ny, ox, oy, px, py;
					if (Q[burglar_y - 1][burglar_x - 1] >= Q[burglar_y - 1][burglar_x + 1] && has_been[burglar_y - 1][burglar_x - 1] == 'N'){ // comapre up-left with up right
						m = Q[burglar_y - 1][burglar_x - 1];
						my = burglar_y - 1;
						mx = burglar_x - 1;
						
					}
					else if (has_been[burglar_y - 1][burglar_x + 1] == 'N'){
						m = Q[burglar_y - 1][burglar_x + 1];
						my = burglar_y - 1;
						mx = burglar_x + 1;
						
					}

					if (Q[burglar_y + 1][burglar_x + 1] >= Q[burglar_y + 1][burglar_x - 1] && has_been[burglar_y + 1][burglar_x + 1] == 'N'){ // compare down-right with down-left
						n = Q[burglar_y + 1][burglar_x + 1];
						ny = burglar_y + 1;
						nx = burglar_x + 1;
					
					}
					else if (has_been[burglar_y + 1][burglar_x - 1] == 'N'){
						n = Q[burglar_y + 1][burglar_x - 1];
						ny = burglar_y + 1;
						nx = burglar_x - 1;
						
					}

					if (Q[burglar_y + 1][burglar_x] >= Q[burglar_y - 1][burglar_x] && has_been[burglar_y + 1][burglar_x] == 'N'){ // compare up with down
						o = Q[burglar_y + 1][burglar_x];
						oy = burglar_y + 1;
						ox = burglar_x;
					
					}
					else if (has_been[burglar_y - 1][burglar_x] == 'N'){
						o = Q[burglar_y - 1][burglar_x];
						oy = burglar_y - 1;
						ox = burglar_x;
						
					}

					if (Q[burglar_y][burglar_x - 1] >= Q[burglar_y][burglar_x + 1] && has_been[burglar_y][burglar_x - 1] == 'N'){ // compare left with right
						p = Q[burglar_y][burglar_x - 1];
						py = burglar_y;
						px = burglar_x - 1;
						
					}
					else if (has_been[burglar_y][burglar_x + 1] == 'N'){
						p = Q[burglar_y][burglar_x+ 1];
						py = burglar_y;
						px = burglar_x + 1;
						
					}


					bool found_one = false;

					if (m >= n && m != -9999){
						if (m >= o){
							if (m >= p){
								y = my;
								x = mx;
								found_one = true;
								has_been_stuck = false;
							}
						}

					}
					if (n >= o && n != -9999 && found_one == false){
						if (n > p){
							y = ny;
							x = nx;
							found_one = true;
							has_been_stuck = false;
						}

					}

					if (o >= p && o != -9999 && found_one == false){
						y = oy;
						x = ox;
						found_one = true;
						has_been_stuck = false;
					}

					if (p != -9999 && found_one == false){
						y = py;
						x = px;
						found_one = true;
						has_been_stuck = false;
					}
				}

				if (has_been_stuck == true ){
					reset_has_been_matrix(has_been);
				}
				else if (tempX == x && tempY == y){ 
					//redo loop
				}
				else if (graph[y][x] != 'O')
					obstruction = true;

			}// END OF WHILE


			
			
		} // END OF IF STATEMENT WHERE WE CHOOSE PATH WITH BEST Q value
	

	else{
		while (loop_controller){
			int randomX = rand() % 8;
			switch (randomX){
			case 0:
				if (burglar_y != 0){ // go north
					direction = "north";
					x = burglar_x;
					y = burglar_y - 1;
				}
				else{ // else go south
					direction = "south";
					x = burglar_x;
					y = burglar_y + 1;
				}
				break;

			case 1:
				if (burglar_y != N - 1){// go south
					direction = "south";
					x = burglar_x;
					y = burglar_y + 1;
				}
				else{ // go north
					direction = "north";
					x = burglar_x;
					y = burglar_y - 1;
				}
				break;

			case 2:
				if (burglar_x != N - 1){ // go east
					direction = "east";
					x = burglar_x + 1;
					y = burglar_y;
				}
				else{ // else go west
					direction = "west";
					x = burglar_x - 1;
					y = burglar_y;
				}
				break;

			case 3:
				if (burglar_x != 0){ // go west
					direction = "west";
					x = burglar_x - 1;
					y = burglar_y;
				}
				else{ // else go east
					direction = "east";
					x = burglar_x + 1;
					y = burglar_y;
				}
				break;

			case 4:
				if (burglar_x != N - 1 && burglar_y != 0){ // go north east
					direction = "north_east";
					x = burglar_x + 1;
					y = burglar_y - 1;
				}
				else if (burglar_x == N - 1 && burglar_y == 0){ // else go south west
					direction = "south_west";
					x = burglar_x - 1;
					y = burglar_y + 1;
				}
				else if (burglar_x == N - 1){ // north west
					direction = "north_west";
					x = burglar_x - 1;
					y = burglar_y - 1;

				}
				else if (burglar_y == 0){ // south east
					direction = "south_east";
					x = burglar_x + 1;
					y = burglar_y + 1;
				}

				break;

			case 5: // go north west
				if (burglar_x != 0 && burglar_y != 0){ // go north west
					direction = "north_west";
					x = burglar_x - 1;
					y = burglar_y - 1;
				}
				else if (burglar_x == 0 && burglar_y == 0){ // else go south east
					direction = "south_east";
					x = burglar_x + 1;
					y = burglar_y + 1;
				}
				else if (burglar_x == 0){ // north east
					direction = "north_east";
					x = burglar_x + 1;
					y = burglar_y - 1;
				}
				else if (burglar_y == 0){ // south west
					direction = "south_west";
					x = burglar_x - 1;
					y = burglar_y + 1;
				}

				break;

			case 6:// go south west
				if (burglar_x != 0 && burglar_y != N - 1){ // go south west
					direction = "south_west";
					x = burglar_x - 1;
					y = burglar_y + 1;
				}
				else if (burglar_x == 0 && burglar_y == N - 1){ // else go north east
					direction = "north_east";
					x = burglar_x + 1;
					y = burglar_y - 1;
				}
				else if (burglar_x == 0){ // south east
					direction = "south_east";
					x = burglar_x + 1;
					y = burglar_y + 1;
				}
				else if (burglar_y == N - 1){ // north west
					direction = "north_west";
					x = burglar_x - 1;
					y = burglar_y - 1;
				}

				break;

			case 7:
				if (burglar_x != N - 1 && burglar_y != N - 1){ // go south east
					direction = "south_east";
					x = burglar_x + 1;
					y = burglar_y + 1;
				}
				else if (burglar_x == N - 1 && burglar_y == N - 1){ // else go north west
					direction = "north_west";
					x = burglar_x - 1;
					y = burglar_y - 1;
				}
				else if (burglar_x == N - 1){ // south west
					direction = "south_west";
					x = burglar_x - 1;
					y = burglar_y + 1;
				}
				else if (burglar_y == 0){ // north east
					direction = "north_east";
					x = burglar_x + 1;
					y = burglar_y - 1;
				}

				break;
			}



			if (graph[y][x] == 'O' || (x == tempX && y == tempY)){
				// continue while loop
			}
			else{
				loop_controller = false;
			}
		}// END OF WHILE LOOP

	} // END OF ELSE



}
double get_max(vector<vector<double> > Q, int y, int x, int ey, int ex, vector<int> troll_coordinates, vector<int> PC){
	//cout << "IN GET MAX" << endl;
	
	double MAXIMUS = 0;


	if (x > 0 && x < Q.size() - 1 && y > 0 && y < Q.size() - 1){
		if (Q[y][x - 1] > MAXIMUS){ MAXIMUS = Q[y][x - 1]; } // check left
		if (Q[y][x + 1] > MAXIMUS){ MAXIMUS = Q[y][x + 1]; } // check right
		if (Q[y - 1][x] > MAXIMUS){ MAXIMUS = Q[y - 1][x]; } // check up
		if (Q[y + 1][x] > MAXIMUS){ MAXIMUS = Q[y + 1][x]; } // check down
		if (Q[y - 1][x - 1] > MAXIMUS){ MAXIMUS = Q[y - 1][x - 1]; } // check up-left
		if (Q[y + 1][x - 1] > MAXIMUS){ MAXIMUS = Q[y + 1][x - 1]; } // check down-left
		if (Q[y + 1][x + 1] > MAXIMUS){ MAXIMUS = Q[y + 1][x + 1]; } // check down-right
		if (Q[y - 1][x + 1] > MAXIMUS){ MAXIMUS = Q[y - 1][x + 1]; } // check up-right
	}

	// if at top left corner
	else if (x == 0 && y == 0){
		if (Q[y][x + 1] > MAXIMUS){ MAXIMUS = Q[y][x + 1]; } // check right
		if (Q[y + 1][x] > MAXIMUS){ MAXIMUS = Q[y + 1][x]; } // check down
		if (Q[y + 1][x + 1] > MAXIMUS){ MAXIMUS = Q[y + 1][x + 1]; } // check down-right

	}
	// if at top right
	else if (y == 0 && x == Q.size() - 1){
		if (Q[y][x - 1] > MAXIMUS){ MAXIMUS = Q[y][x - 1]; } // check left
		if (Q[y + 1][x] > MAXIMUS){ MAXIMUS = Q[y + 1][x]; } // check down
		if (Q[y + 1][x - 1] > MAXIMUS){ MAXIMUS = Q[y + 1][x - 1]; } // check down-left

	}

	// if at bottom left
	else if (x == 0 && y == Q.size() - 1){

		if (Q[y][x + 1] > MAXIMUS){ MAXIMUS = Q[y][x + 1]; } // check right
		if (Q[y - 1][x] > MAXIMUS){ MAXIMUS = Q[y - 1][x]; } // check up
		if (Q[y - 1][x + 1] > MAXIMUS){ MAXIMUS = Q[y - 1][x + 1]; } // check up-right
	}

	// if at bottom right
	else if (x == Q.size() - 1 && y == Q.size() - 1){
		if (Q[y][x - 1] > MAXIMUS){ MAXIMUS = Q[y][x - 1]; } // check left
		if (Q[y - 1][x] > MAXIMUS){ MAXIMUS = Q[y - 1][x]; } // check up
		if (Q[y - 1][x - 1] > MAXIMUS){ MAXIMUS = Q[y - 1][x - 1]; } // check up-left


	}

	// if at left
	else if (x == 0){
		if (Q[y][x + 1] > MAXIMUS){ MAXIMUS = Q[y][x + 1]; } // check right
		if (Q[y - 1][x] > MAXIMUS){ MAXIMUS = Q[y - 1][x]; } // check up
		if (Q[y + 1][x] > MAXIMUS){ MAXIMUS = Q[y + 1][x]; } // check down
		if (Q[y + 1][x + 1] > MAXIMUS){ MAXIMUS = Q[y + 1][x + 1]; } // check down-right
		if (Q[y - 1][x + 1] > MAXIMUS){ MAXIMUS = Q[y - 1][x + 1]; } // check up-right
	}

	// if at right
	else if (x == Q.size() - 1){
		if (Q[y][x - 1] > MAXIMUS){ MAXIMUS = Q[y][x - 1]; } // check left
		if (Q[y - 1][x] > MAXIMUS){ MAXIMUS = Q[y - 1][x]; } // check up
		if (Q[y + 1][x] > MAXIMUS){ MAXIMUS = Q[y + 1][x]; } // check down
		if (Q[y - 1][x - 1] > MAXIMUS){ MAXIMUS = Q[y - 1][x - 1]; } // check up-left
		if (Q[y + 1][x - 1] > MAXIMUS){ MAXIMUS = Q[y + 1][x - 1]; } // check down-left

	}

	// if at top
	else if (y == 0){
		if (Q[y][x - 1] > MAXIMUS){ MAXIMUS = Q[y][x - 1]; } // check left
		if (Q[y][x + 1] > MAXIMUS){ MAXIMUS = Q[y][x + 1]; } // check right
		if (Q[y + 1][x] > MAXIMUS){ MAXIMUS = Q[y + 1][x]; } // check down
		if (Q[y + 1][x - 1] > MAXIMUS){ MAXIMUS = Q[y + 1][x - 1]; } // check down-left
		if (Q[y + 1][x + 1] > MAXIMUS){ MAXIMUS = Q[y + 1][x + 1]; } // check down-right
	}

	// if at bottom
	else if (y == Q.size() - 1){
		if (Q[y][x - 1] > MAXIMUS){ MAXIMUS = Q[y][x - 1]; } // check left
		if (Q[y][x + 1] > MAXIMUS){ MAXIMUS = Q[y][x + 1]; } // check right
		if (Q[y - 1][x] > MAXIMUS){ MAXIMUS = Q[y - 1][x]; } // check up
		if (Q[y - 1][x - 1] > MAXIMUS){ MAXIMUS = Q[y - 1][x - 1]; } // check up-left
		if (Q[y - 1][x + 1] > MAXIMUS){ MAXIMUS = Q[y - 1][x + 1]; } // check up-right
	}


	return MAXIMUS;
}
void compute_Q(vector<vector<double> >& Q, int x, int y, vector<vector<double> > R, double alpha, double gamma, int bx, int by, int escape_y, int escape_x, vector<int> troll_coordinates, vector<int> pony_coordinates, vector<vector <char> > has_been, vector<vector<int> >& many_graph){
	//cout << "IN COMPUTE Q" << endl;
	//-- Compute: Q(state, action) = Q(state,action) + alpha * R(state, action) + gamma * Max[Q(next state, all actions)] --//
	double MAX = get_max(Q, y, x, escape_y, escape_x, troll_coordinates ,pony_coordinates );
	if (has_been[y][x] == 'N'){
		Q[y][x] = Q[y][x] + alpha/((many_graph[y][x] * .01) + 1) * (R[y][x] + (gamma * MAX) - Q[y][x]);
	}

}



void check_what_is_on_next_state(int& saved, vector<vector<char> > graph , int x , int y , bool& been_eaten , bool& burglar_at_E){
	//cout << "IN CHECK WHAT IS ON NEXT STATE" << endl;
	if (graph[y][x] == 'P'){
		saved++;
	}
	else if (graph[y][x] == 'T'){
		been_eaten = true;
	}
	else if (graph[y][x] == 'E'){
		burglar_at_E = true;
	}
}

void burglar_moves(vector<vector<char> >& graph, int x, int y, int& burglar_x, int& burglar_y, vector<vector<char> >& has_been , vector<vector<int> >& many_graph){
	//cout << "IN BURGLAR MOVES" << endl;
	graph[burglar_y][burglar_x] = '-';
	graph[y][x] = 'B';

	burglar_x = x;
	burglar_y = y;
	has_been[y][x] = 'Y';
	many_graph[y][x]++; 

}
void print_graph(vector<vector<char> > graph){
	//for (int i = 0; i) print out # to match teachers printed out graph
	for (int i = 0; i < graph.size(); i++){
		for (int j = 0; j < graph.size(); j++){
			cout << graph[i][j];
		}
		cout << endl;
	}
	cout << endl << endl << endl << endl;
}
void reset_graph(vector<vector<char> >& graph, bool& burglar_at_escape, bool& been_eaten, vector<vector<char> >& g, vector<vector <char> > has_been){
	//cout << "IN RESET_GRAPH FUNCTION" << endl;
	burglar_at_escape = false;
	been_eaten = false;
	graph = g;
	for (int hb = 0; hb < has_been.size(); hb++){
		for (int hb2 = 0; hb2 < has_been.size(); hb2++){
			has_been[hb][hb2] = 'N';
		}
	}

}

void cool_down_random_chance(double& double_random_chance, double simulated_annealing_beta){
	//cout << "IN COOL DOWN FUNCTION" << endl;
	double_random_chance = double_random_chance / (1 + (double_random_chance * simulated_annealing_beta));
}
void normalize_Q(vector<vector<double> >& Q , int& counter , int y , int x){
	double big = 0;
	for (int loop = 0; loop < Q.size(); loop++){
		for (int loop_in = 0; loop_in < Q.size(); loop_in++){
			if (big < Q[loop][loop_in]){
				big = Q[loop][loop_in];
			}

		}

	}

	for (int loop = 0; loop < Q.size(); loop++){
		for (int loop_in = 0; loop_in < Q.size(); loop_in++){
	
				(Q[loop][loop_in] / big ) * 100;
			

		}

	}
	/*
	double big = 100000000000000000;
	double small = 100000000000000000;
	for (int i = 0; i < Q.size(); i++){
		for (int j = 0; j < Q.size(); j++){
			if (0 < log10(Q[i][j]) && log10(Q[i][j]) != INFINITY){
				if (big > log10(Q[i][j]))
					big = log10(Q[i][j]);
			}
			else{
				if (small > abs(log10(Q[i][j])) && abs(log10(Q[i][j])) != INFINITY)
					small = abs(log10(Q[i][j]));

			}
		}

	}


	for (int i = 0; i < Q.size(); i++){
		for (int j = 0; j < Q.size(); j++){
			if (0 <= log10(Q[i][j])){
				if (big < 100000000000000000){
					//cout << Q[i][j] << endl;
					Q[i][j] = Q[i][j] * pow(10 , -big);
					//cout << Q[i][j] << endl;
				}
			}
			else{ 
				if (small < 100000000000000000){
					//cout << Q[i][j] << endl;
					Q[i][j] = Q[i][j] * pow(10, small);
					//cout << Q[i][j] << endl;
				}
			
			}
		///cout << "big     " <<  pow(big, -1) << endl;
			//cout << "small  " << small << endl;
			//cout << big+1<< endl;
			int u = 0;
		}

	}

	/*
	for (int i = 0; i < Q.size(); i++){
		for (int j = 0; j < Q.size(); j++){
			Q[i][j] = Q[i][j] / highest_value;
			if (Q[i][j] >= 0){ Q[i][j] = Q[i][j] *  .01; }
			else{ Q[i][j] = Q[i][j] * 100; }
			
		}

	}
	*/
	


}