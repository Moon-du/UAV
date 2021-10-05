def set_cookie():
    # 设置全局变量Cookie
    cookie = '_xzkj_=wxtoken:8562b9aa0ff97e0b3528bc0326e1513c_c150a53f7073537d87362144415e0e0d; ' \
             '_xxhm_=%7B%22nickName%22%3A%22%E9%95%BF%E5%AE%89%22%2C%22headerImg%22%3A%22https%3A%2F%2Fthirdwx.qlogo' \
             '.cn%2Fmmopen%2Fvi_32' \
             '%2FSiaIsFrgGgIaWr0T45nndIjRXkxG7Wol0Ml1ib6HvgD9ibrqbok1UiaQmALtJEATiaWiafykibQrLEQbkI5HFekWChS3A%2F132' \
             '%22%2C%22name%22%3A%22%22%2C%22wxSubscribed%22%3A0%2C%22sex%22%3A1%2C%22hasPassword%22%3Afalse%7D; ' \
             'UM_distinctid=17c501701e22ac-0c00c7fced65f7-67365e3c-7e900-17c501701e32f; ' \
             'CNZZDATA1261985103=663366073-1633420858-%7C1633420858 '

    # 设置全局变量tk
    tk = '8562b9aa0ff97e0b3528bc0326e1513c_c150a53f7073537d87362144415e0e0d'

    # 设置全局变量人员ID
    linkman_id = '18246898'

    return {
        'cookie': cookie,
        'tk': tk,
        'linkman_id': linkman_id
    }


if __name__ == '__main__':
    set_cookie()
