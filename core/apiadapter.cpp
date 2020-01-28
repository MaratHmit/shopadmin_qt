#include "apiadapter.h"

Api *ApiAdapter::api;


Api *ApiAdapter::getApi()
{
    if (!api)
        api = new Api();
    return api;
}


