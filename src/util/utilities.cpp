
#include "utilities.hpp"
#include <iostream>
#include <cstring>
#include <algorithm>

using std::cout;
using std::cin;
using std::string;
using std::get;

/*
 * Estimates the value of the window size in LSH and Hypercube implementations.
 * 'w' should be approx. 1 - 10 times the average distance between all points.
 * Because this app handles big data, the average distance is being computed between
 * a subset (1%) of data.
 */
uint32_t estimate_window_size(vector<Point *> *data, double(*distance)(Point&, Point&)) {

    // Shuffle data
    uint32_t data_size = data->size();
    std::vector<uint32_t> indices(data_size);
    std::iota(indices.begin(), indices.end(), 0);
    std::shuffle(indices.begin(), indices.end(), std::mt19937(std::random_device()()));

    // If data consists of > 10.000 points, collect its 1%
    double dist = 0.0;
    uint32_t subset = data->size() <= 9999 ? data_size : (uint32_t)(data_size * 0.01);

    for(uint32_t i = 0; i < subset; i++)
        for(uint32_t j = i + 1; j < subset; ++j)
            dist += ( distance((*(*data)[i]), (*(*data)[j])) ) / (double)subset ;
    // Return avg distance
    return (uint32_t)(dist * (1.0 / subset));
}

// Modulo operation between two numbers
uint32_t mod(long a, uint32_t b) {
    long c = a % b;
    return (c < 0) ? (uint32_t)(c + b) : (uint32_t)c;
}

// Receives a string and decides whether the string is an integer or not.
bool is_uint (const char *str) {
    uint32_t size = strlen(str);
    for (uint32_t i = 0; i < size; i++)
        if (isdigit(str[i]) == false && str[i] != '0')
            return false;
    return true;
}

// Gets a file specified by the user and returns it
string get_path(const string& mode) {
    string path;
    cout << "Enter the path of the " << mode << ":" << endl;
    cin >> path;
    return path;
}

// Gets a file specified by the user, and returns it (removes the existing one if exists)
string config_output_file() {
    string path;
    cout << "Enter the path of the output file:" << endl;
    cin >> path;
    remove(path.c_str());
    return path;
}

bool ask_user_to_repeat() {
    int res = 0;
    while (res == 0)
    {
        string answer;
        cout << "\nRepeat the process with another query file?" << endl
             << R"(Type "y" if YES, or "n" if NO.)"  << endl;
        cin >> answer;
        remove(answer.c_str());
        if (answer == "y")
            res = 1;
        else if (answer == "n")
            res = 2;
    }
    return res == 1;
}

void apply_floor_to_vector(vector<double> *v) {
    for(double & i : *v)
        i = floor(i);
}

// Fully handles the LSH input provided to the app
int input_handle_lsh(int narg, char const *argvect[], string *inf, string *qf, string *outf, uint32_t *k, uint32_t *L, uint32_t *NN, uint32_t *radius) {

    if(narg == 0) return 0;

    /* Correct number of arguments check */
    if (narg > 15 || narg % 2 == 0) {
        cout << "Usage: ./lsh –i <input file> –q <query file> –k <int> M <int> -ο <output file> -Ν <number of nearest> -R <radius>" << endl;
        return -1;
    }

    /* in-line parameters */
    char argI[3] = "-i", argQ[3] = "-q", argO[3] = "-o", argK[3] = "-k", argL[3] = "M", argN[3] = "-N", argR[3] = "-R";

    /* Check if the in-line parameters are being written */
    for(int i = 1; i < narg; i += 2) {

        if (strcmp(argvect[i], argI) == 0 && is_uint(argvect[i + 1]) == false) {
            *inf = argvect[i + 1];
        }
        else if (strcmp(argvect[i], argQ) == 0 && is_uint(argvect[i + 1]) == false) {
            *qf = argvect[i + 1];
        }
        else if (strcmp(argvect[i], argO) == 0 && is_uint(argvect[i + 1]) == false) {
            *outf = argvect[i + 1];
        }
        else if (strcmp(argvect[i], argK) == 0 && is_uint(argvect[i + 1])) {
            *k = (int)strtol(argvect[i + 1], nullptr, 10);
        }
        else if (strcmp(argvect[i], argL) == 0 && is_uint(argvect[i + 1])) {
            *L = (int)strtol(argvect[i + 1], nullptr, 10);
        }
        else if (strcmp(argvect[i], argN) == 0 && is_uint(argvect[i + 1])) {
            *NN = (int)strtol(argvect[i + 1], nullptr, 10);
        }
        else if (strcmp(argvect[i], argR) == 0 && is_uint(argvect[i + 1])) {
            *radius = (int)strtol(argvect[i + 1], nullptr, 10);
        }
    }

    if (*k == 0) {
        cout << "Error, number of hash functions must be a positive integer." << endl;
        return -1;
    }

    /* Number of amplified hash functions cannot be zero */
    if (*L == 0) {
        cout << "Error, number amplified hash functions must be a positive integer." << endl;
        return -1;
    }

    /* Nearest neighbour number cannot be zero */
    if (*NN == 0) {
        cout << "Error, number of nearest neighbours must be a positive integer." << endl;
        return -1;
    }

    /* Radius cannot be zero */
    if (*radius == 0) {
        cout << "Error, radius must be a positive integer." << endl;
        return -1;
    }
    return 0;
}

// Fully handles the Hypercube input provided to the app
int input_handle_hypercube(int narg, char const *argvect[], string *inf, string *qf, string *outf, uint32_t *k, uint32_t *M, uint32_t *probes, uint32_t *NN, uint32_t *radius) {

    if(narg == 0) return 0;

    /* Correct number of arguments check */
    if (narg > 17 || narg % 2 == 0) {
        cout << "Usage: ./cube –i <input file> –q <query file> –k <int> -M <int> -probes <int> -ο <output file> -Ν <number of nearest> -R <radius>" << endl;
        return -1;
    }

    /* in-line parameters */
    char argI[3] = "-i", argQ[3] = "-q", argO[3] = "-o", argK[3] = "-k", argM[3] = "-M", argN[3] = "-N", argR[3] = "-R";
    char argProbes[8] = "-probes";

    /* Check if the in-line parameters are being written */
    for(int i = 1; i < narg; i += 2) {

        if (strcmp(argvect[i], argI) == 0 && is_uint(argvect[i + 1]) == false) {
            *inf = argvect[i + 1];
        }
        else if (strcmp(argvect[i], argQ) == 0 && is_uint(argvect[i + 1]) == false) {
            *qf = argvect[i + 1];
        }
        else if (strcmp(argvect[i], argO) == 0 && is_uint(argvect[i + 1]) == false) {
            *outf = argvect[i + 1];
        }
        else if (strcmp(argvect[i], argK) == 0 && is_uint(argvect[i + 1])) {
            *k = (int)strtol(argvect[i + 1], nullptr, 10);
        }
        else if (strcmp(argvect[i], argM) == 0 && is_uint(argvect[i + 1])) {
            *M = (int)strtol(argvect[i + 1], nullptr, 10);
        }
        else if (strcmp(argvect[i], argN) == 0 && is_uint(argvect[i + 1])) {
            *NN = (int)strtol(argvect[i + 1], nullptr, 10);
        }
        else if (strcmp(argvect[i], argR) == 0 && is_uint(argvect[i + 1])) {
            *radius = (int)strtol(argvect[i + 1], nullptr, 10);
        }
        else if (strcmp(argvect[i], argProbes) == 0 && is_uint(argvect[i + 1])) {
            *probes = (int)strtol(argvect[i + 1], nullptr, 10);
        }
    }

    if (*k == 0) {
        cout << "Error, number of *Projected Dimensions* must be a positive integer." << endl;
        return -1;
    }

    /* Number of max points to be checked cannot be zero */
    if (*M == 0) {
        cout << "Error, max points to be checked must be a positive integer." << endl;
        return -1;
    }

    if (*probes == 0) {
        cout << "Error, max vertices to be checked must be a positive integer." << endl;
        return -1;
    }

    /* Nearest neighbour number cannot be zero */
    if (*NN == 0) {
        cout << "Error, number of nearest neighbours must be a positive integer." << endl;
        return -1;
    }

    /* Radius cannot be zero */
    if (*radius == 0) {
        cout << "Error, radius must be a positive integer." << endl;
        return -1;
    }
    return 0;
}

// Fully copies a point and returns the newly allocated one
Point *copy_point(Point *p) {
    return Point::copy_point(*p);
}

// Fully handles the Cluster input provided to the app
int input_handle_cluster(int narg, char const *argvect[], string *inf, string *cf, string *outf, string *method, bool *verbose) {

    if(narg == 0) return 0;

    /* Correct number of arguments check */
    if (narg > 11) {
        cout << "Usage ./cluster –i <input file> –c <configuration file> -o <output file> -complete <optional> -m <method: Classic or LSH or Hypercube>" << endl;
        return -1;
    }

    /* in-line parameters */
    char argI[3] = "-i", argC[3] = "-c", argO[3] = "-o", argM[3] = "-m", argComplete[10] = "-complete";

    /* Check if the in-line parameters are being written */
    for(int i = 1; i < narg; i++) {

        if (strcmp(argvect[i], argI) == 0 && is_uint(argvect[i + 1]) == false) {
            *inf = argvect[i + 1];
        }
        else if (strcmp(argvect[i], argC) == 0 && is_uint(argvect[i + 1]) == false) {
            *cf = argvect[i + 1];
        }
        else if (strcmp(argvect[i], argO) == 0 && is_uint(argvect[i + 1]) == false) {
            *outf = argvect[i + 1];
        }
        else if (strcmp(argvect[i], argM) == 0 && is_uint(argvect[i + 1]) == false) {
            *method = argvect[i + 1];
        }
        else if (strcmp(argvect[i], argComplete) == 0) {
            *verbose = true;
        }
    }
    return 0;
}