#include "wfrest/BluePrint.h"
#include "wfrest/HttpMsg.h"

using namespace wfrest;


void BluePrint::GET(const char *route, const Handler &handler)
{
    WrapHandler wrap_handler =
            [handler](const HttpReq *req,
                            HttpResp *resp,
                            SeriesWork *) -> WFGoTask *
            {
                handler(req, resp);
                return nullptr;
            };

    router_.handle(route, -1, wrap_handler, Verb::GET);
}

void BluePrint::GET(const char *route, int compute_queue_id, const Handler &handler)
{
    WrapHandler wrap_handler =
            [handler, compute_queue_id](HttpReq *req,
                                        HttpResp *resp,
                                        SeriesWork *) -> WFGoTask *
            {
                WFGoTask *go_task = WFTaskFactory::create_go_task(
                        "wfrest" + std::to_string(compute_queue_id),
                        handler,
                        req,
                        resp);

                return go_task;
            };

    router_.handle(route, compute_queue_id, wrap_handler, Verb::GET);
}

void BluePrint::POST(const char *route, const Handler &handler)
{
    WrapHandler wrap_handler =
            [handler](const HttpReq *req,
                            HttpResp *resp,
                            SeriesWork *) -> WFGoTask *
            {
                handler(req, resp);
                return nullptr;
            };

    router_.handle(route, -1, wrap_handler, Verb::POST);
}

void BluePrint::POST(const char *route, int compute_queue_id, const Handler &handler)
{
    WrapHandler wrap_handler =
            [handler, compute_queue_id](HttpReq *req,
                                        HttpResp *resp,
                                        SeriesWork *) -> WFGoTask *
            {
                WFGoTask *go_task = WFTaskFactory::create_go_task(
                        "wfrest" + std::to_string(compute_queue_id),
                        handler,
                        req,
                        resp);

                return go_task;
            };

    router_.handle(route, compute_queue_id, wrap_handler, Verb::POST);
}

void BluePrint::GET(const char *route, const SeriesHandler &handler)
{
    WrapHandler wrap_handler =
            [handler](const HttpReq *req,
                            HttpResp *resp,
                            SeriesWork *series) -> WFGoTask *
            {
                handler(req, resp, series);
                return nullptr;
            };

    router_.handle(route, -1, wrap_handler, Verb::GET);
}

void BluePrint::GET(const char *route, int compute_queue_id, const SeriesHandler &handler)
{
    WrapHandler wrap_handler =
            [handler, compute_queue_id](HttpReq *req,
                                        HttpResp *resp,
                                        SeriesWork *series) -> WFGoTask *
            {
                WFGoTask *go_task = WFTaskFactory::create_go_task(
                        "wfrest" + std::to_string(compute_queue_id),
                        handler,
                        req,
                        resp,
                        series);

                return go_task;
            };

    router_.handle(route, compute_queue_id, wrap_handler, Verb::GET);
}

void BluePrint::POST(const char *route, const SeriesHandler &handler)
{
    WrapHandler wrap_handler =
            [handler](const HttpReq *req,
                            HttpResp *resp,
                            SeriesWork *series) -> WFGoTask *
            {
                handler(req, resp, series);
                return nullptr;
            };

    router_.handle(route, -1, wrap_handler, Verb::POST);
}

void BluePrint::POST(const char *route, int compute_queue_id, const SeriesHandler &handler)
{
    WrapHandler wrap_handler =
            [handler, compute_queue_id](HttpReq *req,
                                        HttpResp *resp,
                                        SeriesWork *series) -> WFGoTask *
            {
                WFGoTask *go_task = WFTaskFactory::create_go_task(
                        "wfrest" + std::to_string(compute_queue_id),
                        handler,
                        req,
                        resp,
                        series);

                return go_task;
            };

    router_.handle(route, compute_queue_id, wrap_handler, Verb::POST);
}

void BluePrint::add_blueprint(const BluePrint &bp, const std::string &url_prefix)
{
    bp.router_.routes_map_.all_routes([this, &url_prefix](
            const std::string &sub_prefix, VerbHandler verb_handler)
                                      {
                                          if (!url_prefix.empty() && url_prefix.back() == '/')
                                              verb_handler.path = url_prefix + sub_prefix;
                                          else
                                              verb_handler.path = url_prefix + "/" + sub_prefix;

                                          this->router_.routes_map_[verb_handler.path.c_str()] = verb_handler;
                                      });
}
