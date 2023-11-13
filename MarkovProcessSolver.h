//
// Created by Akash Shrivastva on 11/9/23.
//

#ifndef MARKOVPROCESSSOLVER_MARKOVPROCESSSOLVER_H
#define MARKOVPROCESSSOLVER_MARKOVPROCESSSOLVER_H

#include "vector"
#include "algorithm"
#include "unordered_map"
#include "map"
#include <stdio.h>
#include "string"
#include "float.h"
#include "fstream"
#include "sstream"
#include "iostream"

using namespace std;

struct ProgramArguments {
    string inputFile;
    double discountFactor, tolerance;
    int iterations;
    bool maximise;

    ProgramArguments() {
        discountFactor = 1.0;
        tolerance = 0.001;
        maximise = true;
        iterations = 100;
        inputFile = "";
    }
};

class MarkovProcessSolver {

private:
    unordered_map<string, vector<string>> adj;
    unordered_map<string, vector<double>> prob;
    unordered_map<string, double> reward;
    map<string, double> value;
    map<string, string> policy;
    unordered_map<string, bool> decisionNode;
    double discountFactor;
    int iterations;
    double tolerance;
    bool maximise, correctInputFormat;

    void init() {

        //mark decision nodes
        for (auto itr = prob.begin(); itr!=prob.end(); itr++) {
            string node = itr->first;
            if (itr->second.size() == 1) {
                decisionNode[node] = true;
            }
        }

        // assign initial values to nodes
        for (auto itr = reward.begin(); itr!= reward.end(); itr++) {
            string node = itr->first;
            if (adj[node].empty()) {
                value[node] = reward[node];
            }
        }

        for (auto itr = adj.begin(); itr!=adj.end(); itr++) {
            string node = itr->first;
            if (!adj[node].empty() and prob.find(node)==prob.end()) {
                prob[node] = vector<double> {1.0};
                decisionNode[node] = true;
            }
        }

        for (auto itr = adj.begin(); itr!=adj.end(); itr++) {
            string node = itr->first;
            if (!adj[node].empty() and value.find(node)==value.end()) {
                value[node] = 0.0;
            }
        }

        // assign initial policies based on neighbor with most reward
        for (auto itr = decisionNode.begin(); itr!=decisionNode.end(); itr++) {
            string node = itr->first;
            string greedyNeighbor;
            double greedyNeighborReward;
            if (maximise) {
                greedyNeighborReward = -DBL_MAX;
                for (auto itr1 = adj[node].begin(); itr1!=adj[node].end(); itr1++) {
                    string neighbor = *itr1;
                    if (reward[neighbor] > greedyNeighborReward) {
                        greedyNeighborReward = reward[neighbor];
                        greedyNeighbor = neighbor;
                    }
                }
            } else {
                greedyNeighborReward = DBL_MAX;
                for (auto itr1 = adj[node].begin(); itr1!=adj[node].end(); itr1++) {
                    string neighbor = *itr1;
                    if (reward[neighbor] < greedyNeighborReward) {
                        greedyNeighborReward = reward[neighbor];
                        greedyNeighbor = neighbor;
                    }
                }
            }

            policy[node] = greedyNeighbor;
        }
    }

    void greedyPolicyComputation() {
        // assign  policies based on neighbor with most value
        for (auto itr = decisionNode.begin(); itr!=decisionNode.end(); itr++) {
            string node = itr->first;
            string greedyNeighbor;
            double greedyNeighborValue;
            if (maximise) {
                greedyNeighborValue = -DBL_MAX;
                for (auto itr1 = adj[node].begin(); itr1!=adj[node].end(); itr1++) {
                    string neighbor = *itr1;
                    if (value[neighbor] > greedyNeighborValue) {
                        greedyNeighborValue = value[neighbor];
                        greedyNeighbor = neighbor;
                    }
                }
            } else {
                greedyNeighborValue = DBL_MAX;
                for (auto itr1 = adj[node].begin(); itr1!=adj[node].end(); itr1++) {
                    string neighbor = *itr1;
                    if (value[neighbor] < greedyNeighborValue) {
                        greedyNeighborValue = value[neighbor];
                        greedyNeighbor = neighbor;
                    }
                }
            }

            policy[node] = greedyNeighbor;
        }
    }

    void valueIteration() {
        int i = 0;
        while (i<iterations) {
            int count = 0;
            for (auto itr = value.begin(); itr!=value.end(); itr++) {
                string node = itr->first;
                if (adj[node].empty()) {
                    continue;
                }
                double currentValue = value[node];
                double newValue = reward[node];
                bool isDecisionNode = false;
                if (decisionNode.find(node) != decisionNode.end()) {
                    isDecisionNode = true;
                }

                for (auto itr1 = adj[node].begin(); itr1!=adj[node].end(); itr1++) {
                    string neighbor = *itr1;
                    if (isDecisionNode) {
                        if (neighbor == policy[node]) {
                            newValue += discountFactor*prob[node][0]*value[neighbor];
                        } else {
                            if ((adj[node].size()-1) != 0) {
                                newValue += (discountFactor*(1.0 - prob[node][0])*value[neighbor])/(adj[node].size() - 1);
                            }
                        }
                    } else {
                        int index = itr1-adj[node].begin();
                        newValue += discountFactor*prob[node][index]*value[neighbor];
                    }
                }

                value[node] = newValue;
                if (abs(newValue - currentValue) <= tolerance) {
                    count++;
                }
            }
            if (count == value.size()) {
                break;
            }
            i++;
        }
    }

    map<string, string> createCopy(map<string, string> m) {
        vector<string> keys, values;

        for (auto itr = m.begin(); itr!=m.end(); itr++) {
            keys.push_back(itr->first);
            values.push_back(itr->second);
        }

        map<string, string> mnew;

        for (int i=0; i<keys.size(); i++) {
            mnew[keys[i]] = values[i];
        }

        return mnew;
    }

    bool isEquals(map<string, string> m1, map<string, string> m2) {
        if (m1.size()!=m2.size()) {
            return false;
        }

        for (auto itr = m1.begin(); itr!=m1.end(); itr++) {
            string node = itr->first;
            if (m1[node]!=m2[node]) {
                return false;
            }
        }

        return true;
    }

    void printPolicyAndValues() {
        for (auto itr = policy.begin(); itr!= policy.end(); itr++) {
            if (adj[itr->first].size()>1) {
                cout<<itr->first<<" -> "<<itr->second<<endl;
            }
        }

        cout<<endl;

        for (auto itr = value.begin(); itr!=value.end(); itr++) {
            cout<<itr->first<<"="<<itr->second<<" ";
        }
    }

    void markovProcessSolver() {
        int i = 0;
        while (1) {
            i++;
            map<string, string> oldPolicy = createCopy(policy);
            valueIteration();
            greedyPolicyComputation();
            if (isEquals(oldPolicy, policy)) {
                break;
            }
        }

        cout<<"Iterations: "<<i<<endl;
        printPolicyAndValues();
    }

    void readFile(string inputFile) {
        correctInputFormat = true;
        ifstream file(inputFile);
        string line;

        while (getline(file, line)) {
            removeLeadingAndTrailingWhitespace(line);

            if (line.empty() || line[0] == '#') {
                continue;
            }

            // parse this line
            if (line.find('=') != string::npos) {
                // reward for a node
                vector<string> nodeReward = split(line, '=');

                reward[nodeReward[0]] = stod(nodeReward[1]);
            } else if (line.find('%') != string::npos) {
                // probabilities for a node
                vector<string> nodeProbability = split(line, '%');

                string probabilities = nodeProbability[1];
                vector<string > p = split(probabilities, ' ');

                string node = nodeProbability[0];

                double ps = 0.0;
                for (const string& pr: p) {
                    prob[node].push_back(stod(pr));
                    ps += stod(pr);
                }

                if (prob[node].size()>1 and ps!=1.0) {
                    correctInputFormat = false;
                    cout<<"Error in line: "<<line<<endl;
                    break;
                }
            } else if (line.find(':') != string::npos) {
                // edges for a node
                vector<string> nodeEdges = split(line, ':');

                if (nodeEdges[1][0]!='[' || nodeEdges[1][nodeEdges[1].size()-1]!=']') {
                    correctInputFormat = false;
                    cout<<"Error in line: "<<line<<endl;
                    break;
                }

                nodeEdges[1] = nodeEdges[1].substr(1, nodeEdges[1].size()-2);
                vector<string> neighbors = split(nodeEdges[1], ',');

                string node = nodeEdges[0];
                for (const string& ne: neighbors) {
                    adj[node].push_back(ne);
                }
            }
        }
    }

    static vector<string> split(string input, char del) {
        stringstream ss(input);
        string token;
        vector<string> tokens;

        while (!ss.eof()) {
            getline(ss, token, del);
            removeLeadingAndTrailingWhitespace(token);
            tokens.push_back(token);
        }

        return tokens;
    }

    static void removeLeadingAndTrailingWhitespace(string &input) {
        if (!input.empty()) {
            input.erase(input.begin(), input.begin() + input.find_first_not_of(" \t"));
            input.erase(std::find_if(input.rbegin(), input.rend(), [](int ch) {
                return !std::isspace(ch);
            }).base(), input.end());
        }
    }

public:
    MarkovProcessSolver(ProgramArguments *arguments) {
        this->tolerance = arguments->tolerance;
        this->iterations = arguments->iterations;
        this->maximise = arguments->maximise;
        this->discountFactor = arguments->discountFactor;
        readFile(arguments->inputFile);
        // initialise policies and rewards
        init();
    }

    void solve() {
        markovProcessSolver();
    }
};


#endif //MARKOVPROCESSSOLVER_MARKOVPROCESSSOLVER_H
