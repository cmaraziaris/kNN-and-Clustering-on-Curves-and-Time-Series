
#ifndef PROJECT_1_FILE_READER_HPP
#define PROJECT_1_FILE_READER_HPP

#include "../dataset/dataset.hpp"
#include <vector>
#include <list>
#include <map>

/*
 *  Read the given "filename" and return a vector of points.
 *  Each point consists of a tuple: (point's ID, vector of doubles).
 */
std::vector<Point *> *file_reader(const std::string& filename);

// Writes all info necessary for the output file
void write_data_to_out_file(const std::string& query_id,
                            std::multimap<double, std::string>&,
                                    double,
                                    std::multimap<double, std::string>&,
                                            double,
                                            std::list<std::tuple<Point *, double>>&,
                                            int,
                                            const std::string&, const std::string& method="LSH");

void read_cluster_config_file(const std::string& filename, uint32_t *noc, uint32_t *noht,
                              uint32_t *nohf, uint32_t *mMh, uint32_t *nhd, uint32_t *nop);

#endif //PROJECT_1_FILE_READER_HPP
