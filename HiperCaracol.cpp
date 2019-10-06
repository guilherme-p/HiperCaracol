#include <iostream> 
#include <vector>
#include <utility>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <string>
#include <algorithm>
#include <limits.h> 
#include <queue>
#include <cstring> 

#define NIL -1

using namespace std;


class Graph { 
    int V, f, e, t;    // Numero de vertices 
  
    // Ponteiro para array que contem os unordered maps
    unordered_map<int, pair<int, int>> *adj;

    vector<int> abastecimentosAumentar;

    multimap<int, int> ligacoesAumentar;
    
public: 
    Graph(int V, int f, int e, int t);   // Construtor 
    ~Graph();                            // Destrutor
    
    void adicionarAresta(int u, int v, int capacidade);
    int getCapacidade(int u, int v);
    void changeCapacidade(int u, int v, int newCap);
    bool existeAresta(int u, int v);
    bool eAbastecimento(int e);
    bool eAbastecimentoLinha(int e);

    string printAbastecimentosAumentar();
    void printLigacoesAumentar();
    bool bfs(int s, int t, int parent[]);
    void bdfs(int s, bool visited[]);
    int fordFulkerson(int s, int t);
    void corteMinimo();
}; 

  
Graph::Graph(int V, int f, int e, int t) { 
    this->V = V;
    this->f = f; this->e = e; this->t = t;
    
    adj = new unordered_map<int, pair<int, int>>[V]; 
}

Graph::~Graph() {
    delete[] adj;
}
  
void Graph::adicionarAresta(int u, int v, int capacidade) { 
    adj[u].insert({v, make_pair(capacidade, capacidade)});
}

int Graph::getCapacidade(int u, int v) {
    return this->adj[u].at(v).first;
}

void Graph::changeCapacidade(int u, int v, int newCap) {
    this->adj[u].at(v).first = newCap;
}

bool Graph::existeAresta(int u, int v) {
    unordered_map<int, pair<int, int>>::const_iterator got = this->adj[u].find(v);
    return got != this->adj[u].end();
}

bool Graph::eAbastecimento(int e) {
    return e > this->f + 1;
}

bool Graph::eAbastecimentoLinha(int e) {
    return e >= this->f + 2 + this->e;
}

/* Devolve string por causa de formatacao (remover ultimo espaco) */
string Graph::printAbastecimentosAumentar() {
    string output = "";

    if (this->abastecimentosAumentar.empty())
        return output;

    sort(this->abastecimentosAumentar.begin(), this->abastecimentosAumentar.end());

    vector<int>::iterator it;
    for (it = this->abastecimentosAumentar.begin(); it != this->abastecimentosAumentar.end(); it++)
        output += to_string(*it) + " ";
    
    output.pop_back();

    return output;
}

void Graph::printLigacoesAumentar() {
    if (this->ligacoesAumentar.empty())
        return;

    map<int, int>::iterator it;
    for (it = this->ligacoesAumentar.begin(); it != this->ligacoesAumentar.end(); it++)
        cout << it->first << " " << it->second << "\n";
    
}

/* Devolve true se ha um caminho entre s e t no grafo residual. 
  Tambem preenche parent[] para guardar o caminho. */
bool Graph::bfs(int s, int t, int parent[]) { 
    bool visited[V]; 
    memset(visited, false, sizeof(visited)); 
  
    queue<int> q; 
    q.push(s); 
    visited[s] = true; 
    parent[s] = NIL; 
  
    while (!q.empty()) { 
        int u = q.front(); 
        q.pop();
  
        for (auto it : this->adj[u]) {
            int v = it.first;
            int cap = it.second.first; 

            if (visited[v] == false && cap > 0) { 
                q.push(v); 
                parent[v] = u; 
                visited[v] = true; 
            } 
        } 
    } 
  
    return (visited[t] == true); 
}

/* Backwards DFS */
void Graph::bdfs(int t, bool visited[]) { 
    visited[t] = true;

    // Verificar se existem arestas com capacidade para t
    for (int i = 1; i < V; i++)                                 // i = 1 para ignorar supersource
        if (existeAresta(i, t)) 
            if (getCapacidade(i, t) > 0 && !visited[i]) 
                bdfs(i, visited); 


    // Verificar se nao existem arestas com capacidade nao negativa para a frente de t
    for (auto it : this->adj[t]) {
        int v = it.first;
        
        int cap = it.second.first;
        int capOriginal = it.second.second;

        if (cap >= 0 && cap != capOriginal && !visited[v])
            bdfs(v, visited); 
    }
        
        
} 
 
int Graph::fordFulkerson(int s, int t) { 
    int u, v;
  
    int parent[this->V];
  
    int max_flow = 0;
  
    // Augmenta fluxo enquanto existe caminho entre source e sink 
    while (bfs(s, t, parent)) { 
        int path_flow = INT_MAX; 

        for (v = t; v != s; v = parent[v]) { 
            u = parent[v];
            path_flow = min(path_flow, getCapacidade(u, v));
        }
  
        for (v = t; v != s; v = parent[v]) { 
            u = parent[v];
            
            changeCapacidade(u, v, getCapacidade(u, v) - path_flow); 
            
            if (existeAresta(v, u)) 
                changeCapacidade(v, u, getCapacidade(v, u) + path_flow); 
        
        }
 
        max_flow += path_flow; 
    }
  
    return max_flow; 
}

void Graph::corteMinimo() {
    bool visited[this->V]; 
    memset(visited, false, sizeof(visited)); 

    bdfs(1, visited);
 
    for (int i = 1; i < this->V; i++) {
        for (int j = 1; j < this->V; j++) {
            if (!visited[i] && visited[j] && existeAresta(i, j)) {             
                if (eAbastecimento(i) && j == i + this->e)                      // ligacao de abastecimento para abastecimento linha (abastecimento + e)
                    this->abastecimentosAumentar.push_back(i);
                
                else {                                                          // converter abastecimento linha
                    if (eAbastecimentoLinha(i))                                 
                        this->ligacoesAumentar.insert({i - this->e, j});
                    else if (eAbastecimentoLinha(j))
                        this->ligacoesAumentar.insert({i, j - this->e});
                    else
                        this->ligacoesAumentar.insert({i, j}); 
                }

            }

        }
    }  
}


int main() {
    ios_base::sync_with_stdio(false); 
    cin.tie(NULL);

    int f, e, t;

    cin >> f >> e >> t;

    int V = 1 + 1 + f + 2*e; // Super source + Hiper + Fornecedores + postos de abastecimentos

    Graph g(V, f, e, t);

    int tProd;
    for (int i = 2; i <= f + 1; i++) {
        cin >> tProd;
        g.adicionarAresta(0, i, tProd);                         // Super source (posicao 0)
    }

    int tCap;
    for (int i = f + 2; i < f + 2 + e; i++) {
        cin >> tCap;
        g.adicionarAresta(i, i + e, tCap);
    }

    int tOrigem, tDestino;
    for (int i = 0; i < t; i++) {
        cin >> tOrigem >> tDestino >> tCap;

        if (g.eAbastecimento(tOrigem))
            tOrigem += e;
        
        g.adicionarAresta(tOrigem, tDestino, tCap);                         

    }
    
    cout << g.fordFulkerson(0, 1) << "\n";                     // Super source ate Hiper
    g.corteMinimo();
    cout << g.printAbastecimentosAumentar() << "\n";
    g.printLigacoesAumentar();
}