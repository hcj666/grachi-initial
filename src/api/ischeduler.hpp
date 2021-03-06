

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
 * Scheduler interface.
 */

#ifndef DEF_GRAPHCHI_ISCHEDULER
#define DEF_GRAPHCHI_ISCHEDULER

#include "graphchi_types.hpp"

namespace graphchi {

    class ischeduler {
        public:
            virtual ~ischeduler() {} 
            virtual void add_task(vid_t vid) = 0;
            virtual void remove_tasks(vid_t fromvertex, vid_t tovertex) = 0;
            virtual void add_task_to_all()  = 0;
            virtual bool is_scheduled(vid_t vertex) = 0;
    };

}


#endif

