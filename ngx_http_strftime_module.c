#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#define DATE_MAX_LEN 30

ngx_int_t
strftime_now(ngx_http_variable_value_t *var, ngx_http_request_t *pool)
{
    struct tm *ptr;
    time_t now;
    char buf[DATE_MAX_LEN];

    now = time(NULL);
    ptr = localtime(&now);

    var->len = strftime(buf, DATE_MAX_LEN, "%M", ptr);
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
    ngx_str_t var_name;
    ngx_http_variable_t *v;
    ngx_str_t *params;

    params = cf->args->elts;
    var_name.data = params[1].data;
    var_name.len = params[1].len;
 
    v = ngx_http_add_variable(cf, &var_name, NGX_HTTP_VAR_CHANGEABLE);
    v->get_handler = var_get_handler;
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
    NULL,
    NULL,

    NULL,
    NULL,

    NULL,
    NULL,

    NULL,
    NULL
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
    return strftime_now(var, r->pool);
}

