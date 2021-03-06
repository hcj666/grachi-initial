/*
 *  chi_httpadmin.hpp
 *  graphchi_graphprocessing.π
 *
 *  Created by Aapo Kyrola on 6/8/12.
 *  Copyright 2012 Carnegie Mellon University. All rights reserved.
 *
 */

#ifndef CHI_HTTPADMIN_DEF
#define CHI_HTTPADMIN_DEF

#include <assert.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>

#include "external/vpiotr-mongoose-cpp/mongoose.h"

extern "C" {
#include "external/vpiotr-mongoose-cpp/mongoose.c"
}
namespace graphchi {

    static const char *ajax_reply_start =
    "HTTP/1.1 200 OK\r\n"
    "Cache: no-cache\r\n"
    "Content-Type: application/x-javascript\r\n"
    "\r\n";
    
    static const char *options[] = {
        "document_root", "conf/adminhtml",
        "listening_ports", "3333",
        "num_threads", "1",
        NULL
    };
    
    static void get_qsvar(const struct mg_request_info *request_info,
                          const char *name, char *dst, size_t dst_len) {
        const char *qs = request_info->query_string;
        mg_get_var(qs, strlen(qs == NULL ? "" : qs), name, dst, dst_len);
    }
    
    // If "callback" param is present in query string, this is JSONP call.
    // Return 1 in this case, or 0 if "callback" is not specified.
    // Wrap an output in Javascript function call.
    static int handle_jsonp(struct mg_connection *conn,
                            const struct mg_request_info *request_info) {
        char cb[64];
        
        get_qsvar(request_info, "callback", cb, sizeof(cb));
        if (cb[0] != '\0') {
            mg_printf(conn, "%s(", cb);
        }
        
        return cb[0] == '\0' ? 0 : 1;
    }
    
    template <typename ENGINE>
    static void ajax_send_message(struct mg_connection *conn,
                                  const struct mg_request_info *request_info) {
         int is_jsonp;
        
        ENGINE * engine = (ENGINE*) request_info->user_data;
        
        mg_printf(conn, "%s", ajax_reply_start);
        is_jsonp = handle_jsonp(conn, request_info);
        
        
        std::string json_info = engine->get_info_json();
        const char * cstr = json_info.c_str();
        int len = (int)strlen(cstr);
        
        //mg_printf(conn, "%s", json_info.c_str());
        // Send read bytes to the client, exit the loop on error
        int num_written = 0;
        
        while (len > 0) {
            if ((num_written = mg_write(conn, cstr, (size_t)len)) != len)
                break;
            len -= num_written;
            cstr += num_written;
        }
        
        if (is_jsonp) {
            mg_printf(conn, "%s", ")");
        }
    }
    

    
    
    template <typename ENGINE>
    static void *event_handler(enum mg_event event,
                               struct mg_connection *conn,
                               const struct mg_request_info *request_info) {
        void *processed = (void*) "yes";
          
        if (event == MG_NEW_REQUEST) {
            if (strcmp(request_info->uri, "/ajax/getinfo") == 0) {
                ajax_send_message<ENGINE>(conn, request_info);
            } else if (strcmp(request_info->uri, "/ajax/getinfo2") == 0) {
            
            } else {
                // No suitable handler found, mark as not processed. Mongoose will
                // try to serve the request.
                processed = NULL;
            }
        } else {
            processed = NULL;
        }
        
        return processed;
    }
    
    
    template <typename ENGINE>
    void start_httpadmin(ENGINE * engine) {
        struct mg_context *ctx;
        
        ctx = mg_start(&event_handler<ENGINE>, (void*)engine, options);
        assert(ctx != NULL);
        std::cout << "Started HTTP admin server. " << std::endl;
    }
    

};

#endif

