# ✨ wfrest: C++ Web Framework REST API

Fast, efficient, and easiest c++ async micro web framework based on [C++ Workflow](https://github.com/sogou/workflow).

## 🌟 Contents

- [wfrest: C++ Web Framework REST API](#✨-wfrest:-c++-web-framework-rest-api)
    - [Discussion](#💥-dicssussion)
    - [Contents](#🌟-contents)
    - [Build](#⌛️-build)
        - [CMake](#cmake)
        - [Docker](#docker)
    - [Quick start](#🚀-quick-start)
    - [API Examples](#🎆-api-examples)
      - [Parameters in path](#parameters-in-path)
      - [Query string parameters](#querystring-parameters)
      - [Post Form](#post-form)
      - [Header](#header)
      - [Send File](#send-file)
      - [Save File](#save-file)
      - [Upload Files](#upload-files)
      - [Json](#json)
      - [Computing Handler](#computing-handler)
      - [Series Handler](#series-handler)
      - [Compression](#compression)
      - [BluePrint](#blueprint)
      - [Serving static files](#serving-static-files)
      - [Cookie](#cookie)
      - [Custom Server Configuration](#custom-server-configuration)
      - [Aspect-oriented programming](#aspect-oriented-programming)
    - [How to use logger](#how-to-use-logger)
  
## 💥 Dicssussion

For more information, you can first see discussions:

**https://github.com/chanchann/wfrest/discussions**

## ⌛️ Build

### Cmake

```
git clone https://github.com/chanchann/wfrest.git
cd wfrest
mkdir build && cd build
cmake ..
make -j 
make install
```

### Docker

Use dockerfile

```
docker build -t wfrest .
```

Or you can Pull from DockerHub 

```
docker pull wfrest/wfrest
```

## 🚀 Quick start

```cpp
#include "wfrest/HttpServer.h"
using namespace wfrest;

int main()
{
    HttpServer svr;

    // curl -v http://ip:port/hello
    svr.GET("/hello", [](const HttpReq *req, HttpResp *resp)
    {
        resp->String("world\n");
    });
    // curl -v http://ip:port/data
    svr.GET("/data", [](const HttpReq *req, HttpResp *resp)
    {
        std::string str = "Hello world";
        resp->String(std::move(str));
    });

    // curl -v http://ip:port/post -d 'post hello world'
    svr.POST("/post", [](const HttpReq *req, HttpResp *resp)
    {
        // reference, no copy here
        std::string& body = req->body();
        fprintf(stderr, "post data : %s\n", body.c_str());
    });

    if (svr.start(8888) == 0)
    {
        getchar()
        svr.stop();
    } else
    {
        fprintf(stderr, "Cannot start server");
        exit(1);
    }
    return 0;
}
```

## 🎆 API Examples

### Parameters in path

```cpp
#include "wfrest/HttpServer.h"
using namespace wfrest;

int main()
{
    HttpServer svr;

    // This handler will match /user/chanchan but will not match /user/ or /user
    // curl -v "ip:port/user/chanchan/"
    svr.GET("/user/{name}", [](const HttpReq *req, HttpResp *resp)
    {
        // reference : no copy
        const std::string& name = req->param("name");
        // resp->set_status(HttpStatusOK); // automatically
        resp->String("Hello " + name + "\n");
    });

    // wildcast/chanchan/action... (prefix)
    svr.GET("/wildcast/{name}/action*", [](const HttpReq *req, HttpResp *resp)
    {
        const std::string& name = req->param("name");
        const std::string& match_path = req->match_path();

        resp->String("[name : " + name + "] [match path : " + match_path + "]\n");
    });

    // request will hold the route definition
    svr.GET("/user/{name}/match*", [](const HttpReq *req, HttpResp *resp)
    {
        // if /user/chanchan/match1234
        // full_path : /user/{name}/match*
        // current_path : /user/chanchan/match1234
        const std::string &full_path = req->full_path();
        const std::string &current_path = req->current_path();
        std::string res;
        if (full_path == "/user/{name}/match*")
        {
            res = full_path + " match : " + current_path;
        } else
        {
            res = full_path + " dosen't match";
        }
        resp->String(res);
    });

    // This handler will add a new router for /user/groups.
    // Exact routes are resolved before param routes, regardless of the order they were defined.
    // Routes starting with /user/groups are never interpreted as /user/{name}/... routes
    svr.GET("/user/groups", [](const HttpReq *req, HttpResp *resp)
    {
        resp->String(req->full_path());
    });

    if (svr.start(8888) == 0)
    {
        getchar()
        svr.stop();
    } else
    {
        fprintf(stderr, "Cannot start server");
        exit(1);
    }
    return 0;
}
```

### Querystring parameters

```cpp
#include "wfrest/HttpServer.h"
using namespace wfrest;

int main()
{
    HttpServer svr;

    // The request responds to a url matching:  /query_list?username=chanchann&password=yyy
    svr.GET("/query_list", [](const HttpReq *req, HttpResp *resp)
    {
        const std::map<std::string, std::string>& query_list = req->query_list();
        for (auto &query: query_list)
        {
            fprintf(stderr, "%s : %s\n", query.first.c_str(), query.second.c_str());
        }
    });

    // The request responds to a url matching:  /query?username=chanchann&password=yyy
    svr.GET("/query", [](const HttpReq *req, HttpResp *resp)
    {
        const std::string& user_name = req->query("username");
        const std::string& password = req->query("password");
        const std::string& info = req->query("info"); // no this field
        const std::string& address = req->default_query("address", "china");
        resp->String(user_name + " " + password + " " + info + " " + address + "\n");
    });

    // The request responds to a url matching:  /query_has?username=chanchann&password=
    // The logic for judging whether a parameter exists is that if the parameter value is empty, the parameter is considered to exist
    // and the parameter does not exist unless the parameter is submitted.
    svr.GET("/query_has", [](const HttpReq *req, HttpResp *resp)
    {
        if (req->has_query("password"))
        {
            fprintf(stderr, "has password query\n");
        }
        if (req->has_query("info"))
        {
            fprintf(stderr, "has info query\n");
        }
    });

    if (svr.start(8888) == 0)
    {
        getchar()
        svr.stop();
    } else
    {
        fprintf(stderr, "Cannot start server");
        exit(1);
    }
    return 0;
}
```

### Post Form

```cpp
#include "wfrest/HttpServer.h"
using namespace wfrest;

int main()
{
    HttpServer svr;

    // Urlencoded Form
    // curl -v http://ip:port/post \
    // -H "body-type:application/x-www-form-urlencoded" \
    // -d 'user=admin&pswd=123456'
    svr.POST("/post", [](const HttpReq *req, HttpResp *resp)
    {
        if (req->content_type() != APPLICATION_URLENCODED)
        {
            resp->set_status(HttpStatusBadRequest);
            return;
        }
        std::map<std::string, std::string> &form_kv = req->form_kv();
        for (auto &kv: form_kv)
        {
            fprintf(stderr, "key %s : vak %s\n", kv.first.c_str(), kv.second.c_str());
        }
    });

    // curl -X POST http://ip:port/form \
    // -F "file=@/path/file" \
    // -H "Content-Type: multipart/form-data"
    svr.POST("/form", [](const HttpReq *req, HttpResp *resp)
    {
        if (req->content_type() != MULTIPART_FORM_DATA)
        {
            resp->set_status(HttpStatusBadRequest);
            return;
        }
        /*
            // https://developer.mozilla.org/en-US/docs/Web/HTTP/Methods/POST
            // <name ,<filename, body>>
            using Form = std::map<std::string, std::pair<std::string, std::string>>;
        */
        const Form &form = req->form();
        for (auto &it: form)
        {
            auto &name = it.first;
            auto &file_info = it.second;
            fprintf(stderr, "%s : %s = %s",
                    name.c_str(),
                    file_info.first.c_str(),
                    file_info.second.c_str());
        }
    });

    if (svr.start(8888) == 0)
    {
        getchar()
        svr.stop();
    } else
    {
        fprintf(stderr, "Cannot start server");
        exit(1);
    }
    return 0;
}
```

### Header

```cpp
#include "wfrest/HttpServer.h"
using namespace wfrest;

int main()
{
    HttpServer svr;

    svr.POST("/post", [](const HttpReq *req, HttpResp *resp)
    {
        const std::string& host = req->header("Host");
        const std::string& content_type = req->header("Content-Type");
        if (req->has_header("User-Agent"))
        {
            fprintf(stderr, "Has User-Agent...");
        }
        resp->String(host + " " + content_type + "\n");
    });


    if (svr.start(8888) == 0)
    {
        getchar()
        svr.stop();
    } else
    {
        fprintf(stderr, "Cannot start server");
        exit(1);
    }
    return 0;
}
```

### Send File

```cpp
#include "wfrest/HttpServer.h"
using namespace wfrest;

int main()
{
    HttpServer svr;

    // single files
    svr.GET("/file1", [](const HttpReq *req, HttpResp *resp)
    {
        resp->File("todo.txt");
    });

    svr.GET("/file2", [](const HttpReq *req, HttpResp *resp)
    {
        resp->File("html/index.html");
    });

    svr.GET("/file3", [](const HttpReq *req, HttpResp *resp)
    {
        resp->File("/html/index.html");
    });

    svr.GET("/file4", [](const HttpReq *req, HttpResp *resp)
    {
        resp->File("todo.txt", 0);
    });

    svr.GET("/file5", [](const HttpReq *req, HttpResp *resp)
    {
        resp->File("todo.txt", 0, 10);
    });

    svr.GET("/file6", [](const HttpReq *req, HttpResp *resp)
    {
        resp->File("todo.txt", 5, 10);
    });

    // multiple files
    svr.GET("/multi_files", [](const HttpReq *req, HttpResp *resp)
    {
        std::vector<std::string> file_list = {"test.txt", "todo.txt", "test1.txt"};
        resp->File(file_list);
    });

    if (svr.start(8888) == 0)
    {
        getchar()
        svr.stop();
    } else
    {
        fprintf(stderr, "Cannot start server");
        exit(1);
    }
    return 0;
}
```

### Save File

```cpp
#include "wfrest/HttpServer.h"
using namespace wfrest;

int main()
{
    HttpServer svr;

    // curl -v -X POST "ip:port/file_write1" -F "file=@filename" -H "Content-Type: multipart/form-data"
    svr.POST("/file_write1", [](const HttpReq *req, HttpResp *resp)
    {
        std::string& body = req->body();   // multipart/form - body has boundary
        resp->Save("test.txt", std::move(body));
    });

    svr.GET("/file_write2", [](const HttpReq *req, HttpResp *resp)
    {
        std::string content = "1234567890987654321";

        resp->Save("test1.txt", std::move(content));
    });

    if (svr.start(8888) == 0)
    {
        getchar()
        svr.stop();
    } else
    {
        fprintf(stderr, "Cannot start server");
        exit(1);
    }
    return 0;
}
```

### Upload Files 

```cpp
#include "wfrest/HttpServer.h"
#include "wfrest/PathUtil.h"
using namespace wfrest;

int main()
{
    HttpServer svr;

    // Upload a file to parent dir is really dangerous.:
    // curl -v -X POST "ip:port/upload" -F "file=@demo.txt; filename=../demo.txt" -H "Content-Type: multipart/form-data"
    // Then you find the file is store in the parent dir, which is dangerous
    svr.POST("/upload", [](const HttpReq *req, HttpResp *resp)
    {
        Form &form = req->form();

        if (form.empty())
        {
            resp->set_status(HttpStatusBadRequest);
        } else
        {
            for(auto& part : form)
            {
                const std::string& name = part.first;
                // filename : filecontent
                std::pair<std::string, std::string>& fileinfo = part.second;
                // file->filename SHOULD NOT be trusted. See Content-Disposition on MDN
                // https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Content-Disposition#directives
                // The filename is always optional and must not be used blindly by the application:
                // path information should be stripped, and conversion to the server file system rules should be done.
                if(fileinfo.first.empty())
                {
                    continue;
                }
                fprintf(stderr, "filename : %s\n", fileinfo.first.c_str());

                resp->Save(PathUtil::base(fileinfo.first), std::move(fileinfo.second));
            }
        }
    });

    if (svr.start(8888) == 0)
    {
        getchar()
        svr.stop();
    } else
    {
        fprintf(stderr, "Cannot start server");
        exit(1);
    }
    return 0;
}
```

### Json

```cpp
#include "wfrest/HttpServer.h"
#include "wfrest/json.hpp"
using namespace wfrest;
using Json = nlohmann::json;

int main()
{
    HttpServer svr;

    // curl -v http://ip:port/json1
    svr.GET("/json1", [](const HttpReq *req, HttpResp *resp)
    {
        Json json;
        json["test"] = 123;
        json["json"] = "test json";
        resp->Json(json);
    });

    // curl -v http://ip:port/json2
    svr.GET("/json2", [](const HttpReq *req, HttpResp *resp)
    {
        std::string valid_text = R"(
        {
            "numbers": [1, 2, 3]
        }
        )";
        resp->Json(valid_text);
    });

    // curl -v http://ip:port/json3
    svr.GET("/json3", [](const HttpReq *req, HttpResp *resp)
    {
        std::string invalid_text = R"(
        {
            "strings": ["extra", "comma", ]
        }
        )";
        resp->Json(invalid_text);
    });

    // recieve json
    //   curl -X POST http://ip:port/json4
    //   -H 'Content-Type: application/json'
    //   -d '{"login":"my_login","password":"my_password"}'
    svr.POST("/json4", [](const HttpReq *req, HttpResp *resp)
    {
        if (req->content_type() != APPLICATION_JSON)
        {
            resp->String("NOT APPLICATION_JSON");
            return;
        }
        fprintf(stderr, "Json : %s", req->json().dump(4).c_str());
    });

    if (svr.start(8888) == 0)
    {
        getchar()
        svr.stop();
    } else
    {
        fprintf(stderr, "Cannot start server");
        exit(1);
    }
    return 0;
}
```

### Computing Handler

```cpp
#include "wfrest/HttpServer.h"
using namespace wfrest

void Fibonacci(int n, HttpResp *resp)
{
    unsigned long long x = 0, y = 1;
    if (n <= 0 || n > 94)
    {
        fprintf(stderr, "invalid parameter");
        return;
    }
    for (int i = 2; i < n; i++)
    {
        y = x + y;
        x = y - x;
    }
    if (n == 1)
        y = 0;
    resp->String("fib(" + std::to_string(n) + ") is : " + std::to_string(y) + "\n");
}

int main()
{
    HttpServer svr;
    // Second parameter means this computing queue id is 1
    // Then this handler become a computing task
    // curl -v http://ip:port/compute_task?num=20
    svr.GET("/compute_task", 1, [](const HttpReq *req, HttpResp *resp)
    {
        int num = std::stoi(req->query("num"));
        Fibonacci(num, resp);
    });

    if (svr.start(8888) == 0)
    {
        getchar();
        svr.stop();
    } else
    {
        fprintf(stderr, "Cannot start server");
        exit(1);
    }
    return 0;
}
```

### Series Handler

```cpp
#include "wfrest/HttpServer.h"
using namespace wfrest

int main()
{
    HttpServer svr;

    svr.GET("/series", [](const HttpReq *req, HttpResp *resp, SeriesWork* series)
    {
        auto *timer = WFTaskFactory::create_timer_task(5000000, [](WFTimerTask *) {
            printf("timer task complete(5s).\n");
        });

        series->push_back(timer);
    });

    if (svr.start(8888) == 0)
    {
        getchar();
        svr.stop();
    } else
    {
        fprintf(stderr, "Cannot start server");
        exit(1);
    }
    return 0;
}
```

### Compression

```cpp
// Server
#include "wfrest/HttpServer.h"
using namespace wfrest;

int main()
{
    HttpServer svr;
    
    // 1. You can `./13_compess_client` 
    // 2. or use python script `python3 13_compress_client.py`
    // 3. echo '{"testgzip": "gzip compress data"}' | gzip |  \
    // curl -v -i --data-binary @- -H "Content-Encoding: gzip" http://ip:port/gzip
    svr.POST("/gzip", [](const HttpReq *req, HttpResp *resp)
    {
        // We automatically decompress the compressed data sent from the client
        // Support gzip, br only now
        std::string& data = req->body();
        fprintf(stderr, "ungzip data : %s\n", data.c_str());
        resp->set_compress(Compress::GZIP);
        resp->String("Test for server send gzip data\n");
    });

    if (svr.start(8888) == 0)
    {
        getchar();
        svr.stop();
    } else
    {
        fprintf(stderr, "Cannot start server");
        exit(1);
    }
    return 0;
}
```

```cpp
// Client
#include "workflow/WFTaskFactory.h"
#include "wfrest/Compress.h"
using namespace protocol;
using namespace wfrest;

struct CompessContext
{
    std::string data;
};

void http_callback(WFHttpTask *task)
{
    const void *body;
    size_t body_len;
    task->get_resp()->get_parsed_body(&body, &body_len);
    std::string decompress_data = Compressor::ungzip(static_cast<const char *>(body), body_len);
    fprintf(stderr, "Decompress Data : %s", decompress_data.c_str());
    delete static_cast<CompessContext *>(task->user_data);
}

int main()
{
    std::string url = "http://127.0.0.1:8888";

    WFHttpTask *task = WFTaskFactory::create_http_task(url + "/gzip",
                                                       4,
                                                       2,
                                                       http_callback);
    std::string content = "Client send for test Gzip";
    auto *ctx = new CompessContext;
    ctx->data = std::move(Compressor::gzip(content.c_str(), content.size()));
    task->user_data = ctx;
    task->get_req()->set_method("POST");
    task->get_req()->add_header_pair("Content-Encoding", "gzip");
    task->get_req()->append_output_body_nocopy(ctx->data.c_str(), ctx->data.size());
    task->start();
    getchar();
}
```

As for how to write a client, you can see [workflow](https://github.com/sogou/workflow) which currently supports HTTP, Redis, MySQL and Kafka protocols. You can use it to write efficient asynchronous clients.

### BluePrint

wfrest supports flask style blueprints. 

You can see [What are Flask Blueprints, exactly?](https://stackoverflow.com/questions/24420857/what-are-flask-blueprints-exactly)

A blueprint is a limited wfresr server. It cannot handle networking. But it can handle routes.

For bigger projects, all your code shouldn't be in the same file. Instead you can segment or split bigger codes into separate files which makes your project a lot more modular.

```cpp
#include "wfrest/HttpServer.h"
using namespace wfrest;

// You can split your different business logic into different files / modules
void set_admin_bp(BluePrint &bp)
{
    bp.GET("/page/new/", [](const HttpReq *req, HttpResp *resp)
    {
        fprintf(stderr, "New page\n");
    });

    bp.GET("/page/edit/", [](const HttpReq *req, HttpResp *resp)
    {
        fprintf(stderr, "Edit page\n");
    });
}

int main()
{
    HttpServer svr;
    
    svr.POST("/page/{uri}", [](const HttpReq *req, HttpResp *resp)
    {
        fprintf(stderr, "Blog Page\n");
    });

    BluePrint admin_bp;
    set_admin_bp(admin_bp);

    svr.register_blueprint(admin_bp, "/admin");

    if (svr.start(8888) == 0)
    {
        svr.list_routes();
        getchar();
        svr.stop();
    } else
    {
        fprintf(stderr, "Cannot start server");
        exit(1);
    }
    return 0;
}
```

### Serving static files

```cpp
#include "wfrest/HttpServer.h"

using namespace wfrest;

int main()
{
    HttpServer svr;
    svr.Static("/static", "./www/static");

    svr.Static("/public", "./www");

    if (svr.start(8888) == 0)
    {
        svr.list_routes();
        wait_group.wait();
        svr.stop();
    } else
    {
        fprintf(stderr, "Cannot start server");
        exit(1);
    }
    return 0;
}
```

### Cookie

Here is a example for set and get a cookie.

```cpp
#include "wfrest/HttpServer.h"
using namespace wfrest;

int main()
{
    HttpServer svr;

    // curl --cookie "name=chanchan, passwd=123" "http://ip:port/cookie"
    svr.GET("/cookie", [](const HttpReq *req, HttpResp *resp)
    {
        const std::map<std::string, std::string> &cookie = req->cookies();
        if(cookie.empty())  // no cookie
        {
            HttpCookie cookie;
            // What you can set :
            // https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Set-Cookie
            cookie.set_path("/").set_http_only(true);
            resp->add_cookie(std::move(cookie));
            resp->String("Set Cookie\n");
        }
        fprintf(stderr, "cookie :\n");
        for(auto &c : cookie)
        {
            fprintf(stderr, "%s : %s\n", c.first.c_str(), c.second.c_str());
        }
    });

    if (svr.start(8888) == 0)
    {
        getchar();
        svr.stop();
    } else
    {
        fprintf(stderr, "Cannot start server");
        exit(1);
    }
    return 0;
}
```

Here is a more specific example, you can see the the [tutorial](https://github.com/wfrest/wfrest/discussions/60)

```cpp
#include "wfrest/HttpServer.h"
using namespace wfrest;

int main()
{
    HttpServer svr;

    svr.GET("/login", [](const HttpReq *req, HttpResp *resp)
    {
        HttpCookie cookie;
        cookie.set_key("user")
                .set_value("chanchan")
                .set_path("/")  
                .set_domain("localhost")
                .set_http_only(true);

        resp->add_cookie(std::move(cookie));
        resp->String("Login success");
    });

    svr.GET("/home", [](const HttpReq *req, HttpResp *resp)
    {
        const std::string &cookie_val = req->cookie("user");
        if(cookie_val != "chanchan")
        {
            resp->set_status(HttpStatusUnauthorized);
            std::string err = R"(
            {
                "error": "Unauthorized"
            }
            )";
            resp->Json(err);
        } else
        {
            std::string success = R"(
            {
                "home": "data"
            }
            )";
            resp->Json(success);
        }
    });

    if (svr.start(8888) == 0)
    {
        getchar();
        svr.stop();
    } else
    {
        fprintf(stderr, "Cannot start server");
        exit(1);
    }
    return 0;
}
```

### Custom Server Configuration

```cpp
#include "wfrest/HttpServer.h"
using namespace wfrest;

int main()
{
    HttpServer svr;
    
    svr.GET("/config", [](const HttpReq *req, HttpResp *resp)
    {
        resp->String("config");
    });

    if (svr.max_connections(4000)
            .peer_response_timeout(20 * 1000)
            .keep_alive_timeout(30 * 1000)
            .start(8888) == 0)
    {
        getchar();
        svr.stop();
    } else
    {
        fprintf(stderr, "Cannot start server");
        exit(1);
    }
    return 0;
}
```

Configuration items are as follows:

```cpp
struct WFServerParams
{
	size_t max_connections;
	int peer_response_timeout;	/* timeout of each read or write operation */
	int receive_timeout;	/* timeout of receiving the whole message */
	int keep_alive_timeout;
	size_t request_size_limit;
	int ssl_accept_timeout;	/* if not ssl, this will be ignored */
};
```

Default values are:

```cpp
static constexpr struct WFServerParams SERVER_PARAMS_DEFAULT =
{
	.max_connections		=	2000,
	.peer_response_timeout	=	10 * 1000,
	.receive_timeout		=	-1,
	.keep_alive_timeout		=	60 * 1000,
	.request_size_limit		=	(size_t)-1,
	.ssl_accept_timeout		=	10 * 1000,
};
```

### Aspect-oriented programming

In computing, aspect-oriented programming (AOP) is a programming paradigm that aims to increase modularity by allowing the separation of cross-cutting concerns.

For more information, you can see [What is AOP](https://en.wikipedia.org/wiki/Aspect-oriented_programming),

```cpp
#include "wfrest/HttpServer.h"
#include "wfrest/Aspect.h"
using namespace wfrest;

// Logging aspect
struct LogAop : public Aspect
{
    bool before(const HttpReq *req, HttpResp *resp) override 
    {
        fprintf(stderr, "before log\n");
        return true;
    }

    // 'after()' should be called after reply
    bool after(const HttpReq *req, HttpResp *resp) override
    {
        fprintf(stderr, "After log\n");
        return true;
    }
    };

int main()
{
    HttpServer svr;

    svr.GET("/aop", [](const HttpReq *req, HttpResp *resp)
    {
        resp->String("aop");
    }, LogAop());

    if (svr.start(8888) == 0)
    {
        getchar();
        svr.stop();
    } else
    {
        fprintf(stderr, "Cannot start server");
        exit(1);
    }
    return 0;
}
```

Transfer data from aspect to http handler:

```cpp
#include "wfrest/HttpServer.h"
#include "wfrest/Aspect.h"
using namespace wfrest;

struct TransferAop : public Aspect
{
    bool before(const HttpReq *req, HttpResp *resp) override 
    {
        auto *content = new std::string("transfer data");
        resp->user_data = content;
        return true;
    }

    // If resp's 'user_data' needs to be deleted, delete it in 'after()'.
    bool after(const HttpReq *req, HttpResp *resp) override
    { 
        fprintf(stderr, "state : %d\terror : %d\n", 
                resp->get_state(), resp->get_error());
        delete static_cast<std::string *>(resp->user_data);
        return true;
    }
};

int main()
{
    HttpServer svr;

    svr.GET("/aop", [](const HttpReq *req, HttpResp *resp)
    {
        auto *content = static_cast<std::string *>(resp->user_data);
        resp->String(std::move(*content));
    }, TransferAop());

    if (svr.start(8888) == 0)
    {
        getchar();
        svr.stop();
    } else
    {
        fprintf(stderr, "Cannot start server");
        exit(1);
    }
    return 0;
}
```

## How to use logger

```cpp
#include "wfrest/Logger.h"
using namespace wfrest

int main()
{
    // set the logger config
    LoggerSetting setting;
    setting.set_level(LogLevel::TRACE)
            .set_log_in_file(true);
    LOGGER(setting);

    int i = 1;
    LOG_DEBUG << (float)3.14;
    LOG_DEBUG << (const char)'8';
    LOG_DEBUG << &i;
    LOG_DEBUG << wfrest::Fmt("%.3f", 3.1415926);
    LOG_DEBUG << "debug";
    LOG_TRACE << "trace";
    LOG_INFO << "info";
    LOG_WARN << "warning";

    FILE *fp = fopen("/not_exist_file", "rb");
    if (fp == nullptr)
    {
        LOG_SYSERR << "syserr log!";
    }
    LOG_DEBUG  << abc << 123.345 << "chanchan" << '\n'
               << std::string("name");
    return 0;
}
```

All the configure fields are:

```cpp
class LoggerSetting
{
    LogLevel level_ = LogLevel::INFO;
    bool log_in_console_ = true;
    bool log_in_file_ = false;
    std::string file_path_ = "./";
    std::string file_base_name_ = "wfrest";
    std::string file_extension_ = ".log";
    uint64_t roll_size_ = 20 * 1024 * 1024;
    std::chrono::seconds flush_interval_ = std::chrono::seconds(3);
};
```

Sample Output

INFO Level :

```
2021-12-14 11:30:56.006802 Here is the Info level
```

Other Level:

```
2021-11-30 22:36:21.422271 822380  [ERROR]  [logger_test.cc:84] No such file or directory (errno=2) syserr log
```

