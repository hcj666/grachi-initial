
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
 * Analyses output of label propagation algorithms such as connected components
 * and community detection. Memory efficient implementation.
 *
 * @author Aapo Kyrola
 */


#include <vector>
#include <algorithm>
#include <errno.h>
#include <assert.h>

#include "logger/logger.hpp"
#include "util/merge.hpp"
#include "util/ioutil.hpp"
#include "util/qsort.hpp"
#include "api/chifilenames.hpp"

#ifndef DEF_GRAPHCHI_LABELANALYSIS
#define DEF_GRAPHCHI_LABELANALYSIS

using namespace graphchi;

template <typename LabelType>
struct labelcount_tt {
    LabelType label;
    unsigned int count;  // Count excludes the vertex which has its own id as the label. (Important optimization)
    labelcount_tt(LabelType l, int c) : label(l), count(c) {}
    labelcount_tt() {}
};

template <typename LabelType>
bool label_count_greater(const labelcount_tt<LabelType> &a, const labelcount_tt<LabelType> &b) {
    return a.count > b.count;
}

template <typename LabelType>
void analyze_labels(std::string base_filename, int printtop = 20) {    
    typedef labelcount_tt<LabelType> labelcount_t;
    /**
     * NOTE: this implementation is quite a mouthful. Cleaner implementation
     * could be done by using a map implementation. But STL map takes too much
     * memory, and I want to avoid Boost dependency - which would have boost::unordered_map.
     */
    std::string filename = filename_vertex_data<LabelType>(base_filename);
    int f = open(filename.c_str(), O_RDONLY);
    
    if (f < 0) {
        logstream(LOG_ERROR) << "Could not open file: " << filename << 
        " error: " << strerror(errno) << std::endl;
        return;
    }
    size_t sz = lseek(f, 0, SEEK_END);
    
    /* Setup buffer sizes */    
    size_t bufsize = 1024 * 1024; // Read one megabyte a time
    int nbuf = bufsize / sizeof(LabelType);
    
    std::vector<labelcount_t> curlabels;
    size_t nread = 0;
    bool first = true;
    vid_t curvid = 0;
    LabelType * buffer = (LabelType*) calloc(nbuf, sizeof(LabelType));
    
    while (nread < sz) {
        size_t len = std::min(sz - nread, bufsize);
        preada(f, buffer, len, nread); 
        nread += len;
        
        int nt = len / sizeof(LabelType);
        
        /* Mark vertices with its own label with 0xffffffff so they will be ignored */
        for(int i=0; i < nt; i++) { 
            LabelType l = buffer[i];
            if (l == curvid) buffer[i] = 0xffffffff;
            curvid++;
        }
        
        /* First sort the buffer */
        quickSort(buffer, nt, std::less<LabelType>());
        
        /* Then collect */
        std::vector<labelcount_t> newlabels;
        newlabels.reserve(nt);
        vid_t lastlabel = 0xffffffff;
        for(int i=0; i < nt; i++) {
            if (buffer[i] != 0xffffffff) {
                if (buffer[i] != lastlabel) {
                    newlabels.push_back(labelcount_t(buffer[i], 1));
                } else {
                    newlabels[newlabels.size() - 1].count ++;
                }
                lastlabel = buffer[i];
            }
            
            /* Check sorted (sanity check) */
            //  for(int i=1; i < newlabels.size(); i++) {
            //      assert(newlabels[i].label > newlabels[i-1].label);
            //  }
        }
        
        if (first) {
            for(int i=0; i < (int)newlabels.size(); i++) {
                curlabels.push_back(newlabels[i]);
            }
            
            /* Check sorted (sanity check) */
            //for(int i=1; i < curlabels.size(); i++) {
            //    assert(curlabels[i].label > curlabels[i-1].label);
            //}
        } else {
            /* Merge current and new label counts */
            int cl = 0;
            int nl = 0;
            std::vector< labelcount_t > merged;
            merged.reserve(curlabels.size() + newlabels.size());
            while(cl < (int)curlabels.size() && nl < (int)newlabels.size()) {
                if (newlabels[nl].label == curlabels[cl].label) {
                    merged.push_back(labelcount_t(newlabels[nl].label, newlabels[nl].count + curlabels[cl].count));
                    nl++; cl++;
                } else {
                    if (newlabels[nl].label < curlabels[cl].label) {
                        merged.push_back(newlabels[nl]);
                        nl++;
                    } else {
                        merged.push_back(curlabels[cl]);
                        cl++;
                    }
                }
            }
            while(cl < (int)curlabels.size()) merged.push_back(curlabels[cl++]);
            while(nl < (int)newlabels.size()) merged.push_back(newlabels[nl++]);
            
            curlabels = merged;
            
            /* Check sorted (sanity check) */
            // for(int i=1; i < curlabels.size(); i++) {
            //     assert(curlabels[i].label > curlabels[i-1].label);
            // }
        }
        
        first = false;
    }
    
    /* Sort */
    std::sort(curlabels.begin(), curlabels.end(), label_count_greater<LabelType>);
    
    /* Write output file */
    std::string outname = base_filename + "_components.txt";
    FILE * resf = fopen(outname.c_str(), "w");
    if (resf == NULL) {
        logstream(LOG_ERROR) << "Could not write label outputfile : " << outname << std::endl;
        return;
    }
    for(int i=0; i < (int) curlabels.size(); i++) {
        fprintf(resf, "%u,%u\n", curlabels[i].label, curlabels[i].count + 1);
    }
    fclose(resf);
    
    std::cout << "Total number of different labels (components/communities): " << curlabels.size() << std::endl;
    std::cout << "List of labels was written to file: " << outname << std::endl;
    
    for(int i=0; i < (int)std::min((size_t)printtop, curlabels.size()); i++) {
        std::cout << (i+1) << ". label: " << curlabels[i].label << ", size: " << curlabels[i].count << std::endl;
    }
}

#endif


