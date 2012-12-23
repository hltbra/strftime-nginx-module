#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#define DATE_MAX_LEN 30

typedef struct {
    u_char *date_fmt;
} ngx_http_strftime_ctx_t;


ngx_int_t
strftime_now(ngx_http_variable_value_t *var, u_char *date_fmt, ngx_pool_t *pool)
{
    struct tm *ptr;
    time_t now;
    char buf[DATE_MAX_LEN];

    now = time(NULL);
    ptr = localtime(&now);

    var->len = strftime(buf, DATE_MAX_LEN, (char *)date_fmt, ptr);
    if (var->len == 0) {
        return NGX_ERROR;
    }

    var->data = ngx_palloc(pool, var->len);
    if (var->data == NULL) {
        return NGX_ERROR;
    }

    ngx_memcpy(var->data, buf, var->len);
    return NGX_OK;
}


ngx_int_t
var_get_handler(ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data);


static char *
ngx_http_strftime(ngx_conf_t *cf, ngx_command_t *cmd, void *conf) {
    ngx_http_strftime_ctx_t *ctx;
    ngx_str_t var_name;
    ngx_http_variable_t *v;
    ngx_str_t *params;

    params = cf->args->elts;
    var_name.data = params[1].data;
    var_name.len = params[1].len;

    ctx = ngx_palloc(cf->pool, sizeof(ngx_http_strftime_ctx_t));
    if (ctx == NULL) {
        return NGX_CONF_ERROR;
    }

    ctx->date_fmt = ngx_palloc(cf->pool, params[2].len + 1);
    if (ctx->date_fmt == NULL) {
        return NGX_CONF_ERROR;
    }
    ngx_memcpy(ctx->date_fmt, params[2].data, params[2].len);
    ctx->date_fmt[params[2].len] = '\0';

    v = ngx_http_add_variable(cf, &var_name, NGX_HTTP_VAR_NOCACHEABLE);
    v->get_handler = var_get_handler;
    v->data = (uintptr_t) ctx;
    return NGX_CONF_OK;
}

static ngx_command_t ngx_http_strftime_commands[] = {
    {ngx_string("strftime"),
     NGX_HTTP_LOC_CONF|NGX_CONF_TAKE2,
     ngx_http_strftime,
     0,
     0,
     NULL
    },

    ngx_null_command
};


static ngx_http_module_t ngx_http_strftime_module_ctx = {
    NULL,                                  /* preconfiguration */
    NULL,                                  /* postconfiguration */

    NULL,                                  /* create main configuration */
    NULL,                                  /* init main configuration */

    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */

    NULL,                                  /* create location configuration */
    NULL                                   /* merge location configuration */
};

ngx_module_t ngx_http_strftime_module = {
    NGX_MODULE_V1,
    &ngx_http_strftime_module_ctx,
    ngx_http_strftime_commands,
    NGX_HTTP_MODULE,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NGX_MODULE_V1_PADDING
};

ngx_int_t
var_get_handler(ngx_http_request_t *r, ngx_http_variable_value_t *var, uintptr_t data)
{
    ngx_http_strftime_ctx_t *ctx = (ngx_http_strftime_ctx_t *) data;
    return strftime_now(var, ctx->date_fmt, r->pool);
}

