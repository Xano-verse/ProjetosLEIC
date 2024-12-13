#include <iostream>
#include <vector>
#include <chrono>

using namespace std;

int _n, _m;
vector<vector<int>> _table;
vector<int> _sequence;
int _result;


void parse_input() {
	
	int i, j;
	
	cin >> _n >> _m;
	
	_table.resize(_n+1, vector<int>(_n+1));
	_sequence.resize(_m+1);

	for (i = 1; i <= _n; i++) {

		for (j = 1; j <= _n; j++) {
			cin >> _table[i][j];
		}
		
	}

	for (i = 1; i <= _m; i++) {
		cin >> _sequence[i];
	}
	
	cin >> _result;

}


void fill_dp(vector<vector<vector<vector<int>>>>& dp) {
	
	bool foundResult = false;

	for (int i = 1; i <= _m; i++) {
		dp[i][i].push_back({_sequence[i], i, -1, -1});
	}

	for (int diagonal = 1; diagonal < _m && !foundResult; diagonal++) {

		for (int i = 1; i <= _m - diagonal; i++) {

			int j = i + diagonal;
			int counter = 0;
			vector<bool> aux = vector<bool> (_n+1, false);
			
			for (int k = j - 1; k >= i && counter != _n && !foundResult; k--) {

				vector<vector<int>>& left = dp[i][k]; 
				vector<vector<int>>& right = dp[k + 1][j];

				int max_left = left.size();
				int max_right = right.size();

				for (int o = 0; o < max_left && counter != _n && !foundResult; o++) {

	            	vector<int>& l = left[o];
				
					for (int p = 0; p < max_right && counter != _n && !foundResult; p++) {

						vector<int>& r = right[p]; 	
						
						if (i == 1 && j == _m) {

							if (_table[l[0]][r[0]] == _result) {
								foundResult = true;
								dp[i][j].push_back({_table[l[0]][r[0]], k, l[0], r[0]});
							}
							
						} else if (!aux[_table[l[0]][r[0]]]) {
							dp[i][j].push_back({_table[l[0]][r[0]], k, l[0], r[0]});
							aux[_table[l[0]][r[0]]] = true;
							counter++;
						}
					}
				}
			}
		}
	}
}


string putParenthesis(vector<vector<vector<vector<int>>>>& dp, int i, int j, int result) { 

	int k = 0, l = 0, r = 0;

	if (i == j) {
		return to_string(dp[i][j][0][0]);
	}

	for (int o = 0; o < (int) dp[i][j].size(); o++) {

		if (dp[i][j][o][0] == result) {

			k = dp[i][j][o][1];
			l = dp[i][j][o][2];
			r = dp[i][j][o][3];
			
			break;

		}

	}
	
	return "(" + putParenthesis(dp, i, k, l) +  " " + putParenthesis(dp, k + 1, j, r) + ")";
	
}


int main() {

	std::ios::sync_with_stdio(0);
	std::cin.tie(0);

	vector<vector<vector<vector<int>>>> dp;

	parse_input();

	dp.resize(_m+1, vector<vector<vector<int>>>(_m+1, vector<vector<int>>()));

	fill_dp(dp);

	if (dp[1][_m].size() != 0) {

		if (dp[1][_m][0][0] == _result) {

			cout << "1\n";
			cout << putParenthesis(dp, 1, _m, _result) << "\n";

			return 0;
		}

	}

	cout << "0\n";
		
	return 0;

}
