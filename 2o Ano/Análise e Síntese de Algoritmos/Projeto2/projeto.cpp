//Projeto 2
#include <iostream>
#include <set>
#include <vector>
#include <queue>

using namespace std;


int _n, _m, _l;
vector<set<int>> _metro_lines;
vector<vector<int>> _graph; // linha x: linhas [y z w]


int parseInput() {

    cin >> _n >> _m >> _l;
	
	vector<bool> stations = vector<bool> (_n+1, false);
    _metro_lines.resize(_l+1);
	stations[0] = true;
	
    for (int i = 0; i < _m; i++) {
        int x, y, line;
        cin >> x >> y >> line;

        _metro_lines[line].insert(x);
        _metro_lines[line].insert(y);

		stations[x] = true;
		stations[y] = true;
		
    }
	
	// no caso de ser só 1 linha
	for (int i = 0; i < _l; i++) {
		if ((int)_metro_lines[i].size() == _n) {
            return 1;
        }
	}

	// ver se houve estacoes que faltam no input
	for (int i = 0; i < _n+1; i++) {
		if (stations[i] == false) {
			return -1;
		}
	}

   return 0;

}

void addEdge (int u, int v) {
    _graph[u].push_back(v);
    _graph[v].push_back(u);
}

void buildLinesGraph() {
    for (int i = 1; i <= _l; i++) {
        for (int j = i + 1; j <= _l; j++) {
            for (int station : _metro_lines[i]) {
			    // Se nao for encontrado nada ira dar return de _metro_lines.end()
				// Ou seja, if algo for encontrado... then...
                if (_metro_lines[j].find(station) != _metro_lines[j].end()) {
                    addEdge(i, j);
                    break;
                }
            }
        }
    }
}


// BFS from given source s
int bfs(int s) {
    // Create a queue for BFS
    queue<int> q; 

    int distances[_l+1];
	int max_min_distance = 0;
    int num_visited = 0;
    
    // Initially mark all the vertices as not visited
    // When we push a vertex into the q, we mark it as visited
    vector<bool> visited(_graph.size(), false);

    // Mark the source node as visited and enqueue it
    visited[s] = true;
    num_visited++;
    q.push(s);

	distances[s] = 0;

    // Iterate over the queue
    while (!q.empty()) {
      
        // Dequeue a vertex from queue and print it
        int curr = q.front();
        q.pop();

        // Get all adjacent vertices of the dequeued 
        // vertex curr If an adjacent has not been 
        // visited, mark it visited and enqueue it
        for (int x : _graph[curr]) {
            if (!visited[x]) {
                visited[x] = true;
                num_visited++;
                q.push(x);

				distances[x] = distances[curr] + 1; 
				max_min_distance = distances[x];
			}
		}
	}

	// caso haja linhas inteiras isoladas, partes do grafo disconectadas
    if (num_visited != _l){ 
        return -1;
    }

    return max_min_distance;
}


int main() {

	std::ios::sync_with_stdio(0);
	std::cin.tie(0);

    int mc = 0;
	int retParseInput = parseInput();

	// caso seja -1, ou seja, estacoes ou linhas isoladas
	if (retParseInput < 0) {
		cout << "-1" << "\n";
		return 0;

	// caso seja 1, ou seja, todas as estacoes em 1 linha 
	} else if (retParseInput > 0) {
        cout << "0" << "\n";
		return 0;
    }

    _graph.resize(_l+1);
    buildLinesGraph();


    for (int i = 1; i <= _l; i++) {

        int temp = 0;
        temp = bfs(i);

        // se for -1, podemos dar break
        if (temp == -1) {
            cout << "-1" << "\n";
            return 0;
        }

        if (temp > mc) {
            mc = temp;
        }
        
    }

    cout << mc << "\n";

	return 0;

}

