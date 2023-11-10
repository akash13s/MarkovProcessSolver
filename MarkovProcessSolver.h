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
    bool maximise;

    void readFileV2(string inputFile) {
        bool correctInputFormat = true;

        ifstream file(inputFile);
        string line;

        vector<string> atoms;

        while (getline(file, line)) {
            removeLeadingAndTrailingWhitespace(line);

            if (line.empty()) {
                continue;
            }

            vector<string> v = split(line, ' ');

            for (int i=0; i<v.size(); i++) {
                vector<string> w = split(v[i], '=');
                if (w[0].length() > 2 || w[1].length() > 1) {
                    cout<<"Error in input file: "<<w[0]<<"="<<w[1]<<endl;
                    correctInputFormat = false;
                    break;
                }
                int row = w[0][0] - '0';
                int col = w[0][1] - '0';
                int val = w[1][0] - '0';
                // perform validation for row/col/val
                // string atom = getAtomForSquare(row, col, val, false);
                // atoms.push_back(atom);
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

    void readFile(string inputFile) {

        // write the logic for parsing the input file

        /*
         *
         * # maze example from class slides

            Z=1
            Y=-1
            A : [Z, C, Y]
            A % .8
            B : [Z, C, E]
            B % .8
            C : [A, B, D, F]
            C % .8
            D : [C, Y, G]
            D % .8
            E : [F, B]
            E % .8
            F : [E, C, G]
            F % .8
            G : [D, F]
            G % .8
         */
//        vector<string> A = {"Z", "C", "Y"};
//        vector<string> B = {"Z", "C", "E"};
//        vector<string> C = {"A", "B", "D", "F"};
//        vector<string> D = {"C", "Y", "G"};
//        vector<string> E = {"F", "B"};
//        vector<string> F = {"E", "C", "G"};
//        vector<string> G = {"D", "F"};
//
//        adj["A"] = A;
//        adj["B"] = B;
//        adj["C"] = C;
//        adj["D"] = D;
//        adj["E"] = E;
//        adj["F"] = F;
//        adj["G"] = G;
//
//        prob["A"] = vector<double> {0.8};
//        prob["B"] = vector<double> {0.8};
//        prob["C"] = vector<double> {0.8};
//        prob["D"] = vector<double> {0.8};
//        prob["E"] = vector<double> {0.8};
//        prob["F"] = vector<double> {0.8};
//        prob["G"] = vector<double> {0.8};
//
//        reward["Z"] = 1;
//        reward["Y"] = -1;

        /*
         * # The publisher decision tree
            S : [Reject, Publish, Consult]
            S % 1
            Reject=0
            Publish : [Success, Failure]
            Publish % .2 .8
            Success=50000
            Failure=-10000
            Consult=-500
            Consult : [For, Against]
            Consult % .46 .54
            Against=0
            For : [Success, Failure]
            For % .3 .7
         */

        vector<string> S = {"Reject", "Publish", "Consult"};
        vector<string> Publish = {"Success", "Failure"};
        vector<string> Consult = {"For", "Against"};
        vector<string> For = {"Success", "Failure"};
        adj["S"] = S;
        adj["Publish"] = Publish;
        adj["Consult"] = Consult;
        adj["For"] = For;

        prob["S"] = vector<double> {1.0};
        prob["Publish"] = vector<double> {0.2, 0.8};
        prob["Consult"] = vector<double> {0.46, 0.54};
        prob["For"] = vector<double> {0.3, 0.7};

        reward["Reject"] = 0;
        reward["Success"] = 50000;
        reward["Failure"] = -10000;
        reward["Consult"] = -500;
        reward["Against"] = 0;

        iterations = 100;
        tolerance = 0.001;
        discountFactor = 1;
        maximise = true;
    }

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
            cout<<itr->first<<" -> "<<itr->second<<endl;
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
