#include <stdio.h>
#include <stdlib.h>
#include "time.h"
#include <fstream>
#include <random>
#include <climits>
#include "graph.hpp"
#include "algorithm.hpp"


// -----------------------------------------------------------------------------------------------------------------------
// Definicije lokalnih funkcija
// -----------------------------------------------------------------------------------------------------------------------

// Dodaje u usmeren graf V^2 grana sa random tezinama
void generateRandomV2DirectedEdges(Graph& g){
    std::default_random_engine generator(std::random_device{}());;
    std::uniform_int_distribution<int> distribution(INT_MIN + 1, INT_MAX - 1);

    g.setDirected(true);

    for(int i = 0; i < g.size(); ++i){
        for(int j = 0; j < g.size(); ++j){
            if(i != j){
                int weight = distribution(generator);
                g.addEdge(i, j, weight);
            }
        }
    }
}

// Dodaje u neusmeren graf V^2 grana sa random tezinama
void generateRandomV2UndirectedEdges(Graph& g){
    std::default_random_engine generator(std::random_device{}());;
    std::uniform_int_distribution<int> distribution(INT_MIN + 1, INT_MAX - 1);

    g.setDirected(false);

    for(int i = 0; i < g.size(); ++i){
        for(int j = i + 1; j < g.size(); ++j){
            int weight = distribution(generator);
            g.addEdge(i, j, weight);
            g.addEdge(j, i, weight);
        }
    }
}

// Dodaje u usmeren graf random O(V) grana sa random tezinama
void generateRandomOVDirectedEdges(Graph& g){
    std::default_random_engine generator(std::random_device{}());(std::random_device{}());
    std::uniform_int_distribution<int> weights_distribution(INT_MIN + 1, INT_MAX - 1);
    std::uniform_int_distribution<int> edges_distribution(0, g.size());
    int goal = std::rand() % g.size();

    g.setDirected(true);

    for(int i = 0; i < g.size(); ++i){
        for(int j = 0; j < g.size(); ++j){
            if(i != j && edges_distribution(generator) == goal){
                int weight = weights_distribution(generator);
                g.addEdge(i, j, weight);
            }
        }
    }
}

// Dodaje u neusmeren graf random O(V) grana sa random tezinama
void generateRandomOVUndirectedEdges(Graph& g){
    std::default_random_engine generator(std::random_device{}());;
    std::uniform_int_distribution<int> weights_distribution(INT_MIN + 1, INT_MAX - 1);
    std::uniform_int_distribution<int> edges_distribution(0, g.size());
    int goal = std::rand() % g.size();

    g.setDirected(false);

    for(int i = 0; i < g.size(); ++i){
        for(int j = i + 1; j < g.size(); ++j){
            if(edges_distribution(generator) == goal){
                int weight = weights_distribution(generator);
                g.addEdge(i, j, weight);
                g.addEdge(j, i, weight);
            }
        }
    }
}

void generateConstantO1Edges(Graph& g){
    g.setDirected(true);
    if(g.size() < 5){
        return;
    }
    g.addEdge(0, 1, 1);
    g.addEdge(0, 2, 2);
    g.addEdge(2, 1, 2);
    g.addEdge(1, 0, 2);
    g.addEdge(2, 4, 200);
    g.addEdge(4, 3, 200);
    g.addEdge(3, 1, 200);
}

void generateConstantOVEdges(Graph& g){
    g.setDirected(true);
    for(int i = 2; i < g.size(); ++i){
        g.addEdge(i, i - 1, 10);
    }
    for(int i = 2; i < g.size(); ++i){
        g.addEdge(0, i, i);
    }
}

// Funkcija check_result proverava da li je rezultat algoritma ispravan
bool check_result(const Graph& g, int src, int dest, const Path& path, int& bottleneck, bool first_test){
    if(path.empty()){
        if(g.connected(src, dest)){
            std::cout << "[ERROR] Alg. returned null path, but path between vertices exists! " << std::endl;
            return false;
        }
        else{
            return true;
        }
    }

    int prevbottleneck = bottleneck;
    if(g.getMinEdge(path, bottleneck) == false){
        std::cout << "[ERROR] Path that alg. returned does not exist in graph! " << std::endl;
        return false;
    }

    if(!first_test && prevbottleneck != bottleneck){
        std::cout << "[ERROR] Different path output! " << 
        "previous alg. returned path with bottleneck " << prevbottleneck << 
        ", this alg. returned path with bottleneck " << bottleneck << std::endl;
        return false;
    }
    return true;
}

// Ispisuje na standardni izlaz naziv funkcije name, kao i vreme izvrsavanja
// time_s predstavlja vreme pocetka izvrsavanja funkcije
// time_e predstavlja vreme zavrsetka izvrsavanja funkcije
double printTimeSpecs(std::string name, struct timespec time_s, struct timespec time_e){
    double duration = (double)(time_e.tv_nsec - time_s.tv_nsec) / 1000000.0 +
                      (double)(time_e.tv_sec - time_s.tv_sec) * 1000.0;

    std::cout << "[" << name << "]" << " [ms]:" << duration << " ";
    return duration;
}

// Funkcija runTests izvrsava testove tj. widestPath(g, src, dest)
// za sve razlicite implementacije algoritma widestPath.
// Alg. widestPathBruteForce je ukljucen u testove akko je flag incbf true.
// Vremena izvrsavanja algoritama se vracaju kroz times
bool runTests(Graph& g, int src, int dest, std::vector<double>& times, bool incbf){
    std::vector<std::pair<Path, int>> ps;
    Path path;
    struct timespec time_s, time_e;
    double duration;
    int bottleneck;
    int prevbottleneck;
    bool result = true;

    std::cout << "--------------------------------" << std::endl;
    std::cout << "[runTests] src=" << src << ", dest=" << dest << std::endl;
    std::cout << "--------------------------------" << std::endl;

    if(incbf){
        clock_gettime(CLOCK_MONOTONIC, &time_s);
        path = widestPathBruteForce(g, src, dest);
        clock_gettime(CLOCK_MONOTONIC, &time_e);
        duration = printTimeSpecs("widestPathBruteForce", time_s, time_e);
        times.push_back(duration);
        printPath(path);
        result = check_result(g, src, dest, path, bottleneck, true) ? result : false;
    }
    else{
        std::cout << "[widestPathBruteForce] [not executed] " << std::endl;
        times.push_back(0);
    }

    clock_gettime(CLOCK_MONOTONIC, &time_s);
    path = widestPathDijkstra(g, src, dest);
    clock_gettime(CLOCK_MONOTONIC, &time_e);
    duration = printTimeSpecs("widestPathDijkstra", time_s, time_e);
    times.push_back(duration);
    printPath(path);
    result = check_result(g, src, dest, path, bottleneck, incbf ? false : true) ? result : false;

    clock_gettime(CLOCK_MONOTONIC, &time_s);
    path = widestPathMedianEdgeWeight(g, src, dest);
    clock_gettime(CLOCK_MONOTONIC, &time_e);
    duration = printTimeSpecs("widestPathMedianEdgeWeight", time_s, time_e);
    times.push_back(duration);
    printPath(path);
    result = check_result(g, src, dest, path, bottleneck, true) ? result : false;

    if(g.isDirected() == false){
        clock_gettime(CLOCK_MONOTONIC, &time_s);
        path = widestPathInUndirectedGraph(g, src, dest);
        clock_gettime(CLOCK_MONOTONIC, &time_e);
        duration = printTimeSpecs("widestPathInUndirectedGraph", time_s, time_e);
        times.push_back(duration);
        printPath(path);
        result = check_result(g, src, dest, path, bottleneck, true) ? result : false;
    }
    else{
        std::cout << "[widestPathInUndirectedGraph] [not executed] " << std::endl;
        times.push_back(0);
    }

    clock_gettime(CLOCK_MONOTONIC, &time_s);
    path = widestPathEdgesOrdering(g, src, dest);
    clock_gettime(CLOCK_MONOTONIC, &time_e);
    duration = printTimeSpecs("widestPathEdgesOrdering", time_s, time_e);
    times.push_back(duration);
    printPath(path);
    result = check_result(g, src, dest, path, bottleneck, false) ? result : false;

    if(result){
        std::cout << "[success]" << std::endl;
    }
    else{
        std::cout << "[ERROR][TEST FAILED]" << std::endl;
    }
    std::cout << "--------------------------------" << std::endl;
    return result;
}

// Funkcija updateAvgs se koristi za racunanje prosecnog vremena izvravanja testova
void updateAvgs(std::vector<std::pair<std::string, double>>& avgs, std::vector<double> times, int divident){
    for(int i = 0; i < avgs.size(); ++i){
        avgs[i].second += times[i] / divident;
    }
}

// Funkcija printAvgs stampa na standardni izlaz prosecno vreme izvravanja testiranig algoritama
void printAvgs(std::vector<std::pair<std::string, double>> avgs){
    std::cout << "--------------------------------" << std::endl;
    std::cout << "[AVERAGE EXECUTION TIME]" << std::endl;
    std::cout << "--------------------------------" << std::endl;
    for(int i = 0; i < avgs.size(); ++i){
        std::cout << "[" << avgs[i].first << "]" << " [ms]:" << avgs[i].second << std::endl;
    }
}

// Funkcija printResult stampa na standardni izlaz rezultat testova,
// pri cemu je succcess broj testova koji su se zavrsili uspesno,
// dok je total ukupa broj testova
void printResult(int success, int total){
    std::cout << "--------------------------------" << std::endl;
    if(success == total){
        std::cout << "All tests passed!" << std::endl;
    }
    else{
        std::cout << "Tests results: " << "[" << success << "/" << total << "] tests passed." << std::endl;
    }
    std::cout << "--------------------------------" << std::endl;
}