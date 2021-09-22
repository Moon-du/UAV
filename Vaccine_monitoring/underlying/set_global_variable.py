def set_global_variable():
    # 设置全局变量Cookie
    cookie = '_xxhm_=%7B%22id%22%3A10145539%2C%22openId%22%3A%22o********%22%2C%22unionid%22%3A%22o********%22%2C' \
             '%22mobile%22%3A%2215231947520%22%2C%22nickName%22%3A%22%E9%95%BF%E5%AE%89%22%2C%22headerImg%22%3A' \
             '%22http%3A%2F%2Fthirdwx.qlogo.cn%2Fmmopen' \
             '%2FajNVdqHZLLCb5hiaOFgibR5oARc9ibSOMPtfKibxibzEugpn6QH9G2ibZeATAbn5aFibH2cZAbhMvlyXX7KWOImrdAicNw%2F132' \
             '%22%2C%22regionCode%22%3A%22510122%22%2C%22name%22%3A%22%E5%86%AF**%22%2C%22uFrom%22%3A' \
             '%22depa_vacc_detail%22%2C%22wxSubscribed%22%3A1%2C%22birthday%22%3A%221997-05-24+02%3A00%3A00%22%2C' \
             '%22sex%22%3A1%2C%22hasPassword%22%3Atrue%2C%22birthdayStr%22%3A%221997-05-24%22%2C' \
             '%22ymWxMobileAppOpenId%22%3A%22%22%2C%22weiboUid%22%3A%22%22%2C%22appleUserId%22%3A%22%22%7D;_xzkj_' \
             '=ymapptoken:59f284011b32f06e53453cbaa1eb9d8d_f821869ad690efe4731b2369efc3a7e4;cntbzgfwy' \
             '=49df21c56c4fae9ad474ad3404a9d2f4;tgw_l7_route=764ac716d702bebcf51544392a3d3da9 '

    # 设置全局变量tk
    tk = 'ymapptoken:59f284011b32f06e53453cbaa1eb9d8d_f821869ad690efe4731b2369efc3a7e4'

    # 设置全局变量人员ID
    linkman_id = '18246898'

    # 设置全局变量可预约针次
    vacc_index = [1, 2, 3]

    # 设置全局变量区域代码
    region_code = '3701'  # 济南

    # 设置全局变量经纬度
    longitude = "104.0852763274418"
    latitude = "30.52735180802844"

    return {
        'cookie': cookie,
        'tk': tk,
        'linkman_id': linkman_id,
        'region_code': region_code,
        'vacc_index': vacc_index,
        "longitude": longitude,
        "latitude": latitude
    }


if __name__ == '__main__':
    set_global_variable()
