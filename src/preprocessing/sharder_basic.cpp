/**
 * @file
 * @author  Aapo Kyrola <akyrola@cs.cmu.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * Copyright [2012] [Aapo Kyrola, Guy Blelloch, Carlos Guestrin / Carnegie Mellon University]
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 
 *
 * @section DESCRIPTION
 *
 * Sharder_basic can convert graphs from the edgelist and adjacency
 * list representations to shards used by the GraphChi system.
 */

#include <iostream>
#include <stdlib.h>
#include <string>
#include <assert.h>

#include "logger/logger.hpp"
#include "preprocessing/conversions.hpp"
#include "preprocessing/sharder.hpp"
#include "util/cmdopts.hpp"

using namespace graphchi;

//wyh is sb
int main(int argc, const char ** argv) {
    graphchi_init(argc, argv); //εΎεε§ε
    
    global_logger().set_log_level(LOG_DEBUG);
    
    std::string basefile = get_option_string_interactive("file", "[path to the input graph]");
    std::string edge_data_type = get_option_string_interactive("edgedatatype", "int, uint, short, float, char, double, boolean, long, float-float, int-int");
    std::string nshards_str = get_option_string_interactive("nshards", "Number of shards to create, or 'auto'");
    
    if (edge_data_type == "float") {
        convert<float>(basefile, nshards_str);
    } if (edge_data_type == "float-float") {
        convert<PairContainer<float> >(basefile, nshards_str);
    } else if (edge_data_type == "int") {
        convert<int>(basefile, nshards_str);
    } else if (edge_data_type == "uint") {
        convert<unsigned int>(basefile, nshards_str);
    } else if (edge_data_type == "int-int") {
        convert<PairContainer<int> >(basefile, nshards_str);
    } else if (edge_data_type == "short") {
        convert<short>(basefile, nshards_str);
    } else if (edge_data_type == "double") {
        convert<double>(basefile, nshards_str);
    } else if (edge_data_type == "char") {
        convert<char>(basefile, nshards_str);
    } else if (edge_data_type == "boolean") {
        convert<bool>(basefile, nshards_str);
    } else if (edge_data_type == "long") {
        convert<long>(basefile, nshards_str);
    } else {
        logstream(LOG_ERROR) << "You need to specify edgedatatype. Currently supported: int, short, float, char, double, boolean, long.";
        return -1;    
    }
    
    return 0;
}




