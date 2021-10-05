import datetime
import json
import logging
import time

import requests
from jsonpath import jsonpath

from conf.set_cookie import set_cookie

# 设置日志等级
logger = logging.getLogger()
logger.setLevel(logging.INFO)


def set_region():
    # 设置全局变量区域代码
    region_code = '3701'  # 济南

    # 设置全局变量经纬度
    longitude = "104.0852763274418"
    latitude = "30.52735180802844"

    # 返回全局变量
    return {
        'region_code': region_code,
        "longitude": longitude,
        "latitude": latitude
    }


def get_departments():
    # 获取全局变量
    globalVariable = set_region()

    # 获取医院列表
    url = "https://mobile.scmttec.com/base/department/getDepartments.do"
    params = {
        "sortType": "1",
        "regionCode": globalVariable['region_code'],
        "offset": "0",
        "limit": "5",
        "isOpen": "1",
        "customId": "3",
        "longitude": globalVariable['longitude'],
        "latitude": globalVariable['latitude'],
    }

    # 获取响应值，将响应值转化为json对象
    res = requests.get(url=url, params=params)
    json_data = res.json()

    # 断言请求是否成功
    code = jsonpath(json_data, "$.code")  # 获取响应code
    if "0000" in code:
        pass
    else:
        logger.error(f"获取医院列表失败，请检查~\n"
                     f"错误信息：{json_data}")

    # 通过jsonPath获取医院参数
    hospital_name = jsonpath(json_data, "$.data.rows..name")  # 获取医院Name列表
    depa_code = jsonpath(json_data, "$.data.rows..code")  # 获取医院Code列表
    hospital_depa_vacc_id = jsonpath(json_data, "$.data.rows..depaVaccId")  # 获取医院depaVaccId列表
    vaccine_code = jsonpath(json_data, "$.data.rows..vaccineCode")  # 获取疫苗Code列表

    # 返回医院基础信息
    return {
        'hospital_name': hospital_name,
        'depa_code': depa_code,
        'hospital_depa_vacc_id': hospital_depa_vacc_id,
        'vaccine_code': vaccine_code
    }


def is_can_subscribe():
    # 获取cookie
    global_cookie = set_cookie()

    # 获取医院基本信息
    hospital_info = get_departments()
    type_code_list = []  # 医院状态码列表初始为空

    # 获取医院详情
    i = 0  # 医院下标
    for hospital_depa_vacc_id in hospital_info['hospital_depa_vacc_id']:
        hospital = hospital_info["hospital_name"][i]  # 获取每次循环取到的医院名称
        url = "https://mobile.scmttec.com/subscribe/subscribe/isCanSubscribe.do"
        params = {
            "vaccineCode": hospital_info['vaccine_code'][i],  # 疫苗code
            "linkmanId": global_cookie['linkman_id'],  # 人员id
            "id": hospital_depa_vacc_id,  # 医院id
            "depaCode": hospital_info['depa_code'][i]  # 医院code
        }
        headers = {
            'tk': global_cookie['tk'],
            'cookie': global_cookie['cookie']
        }
        i += 1  # 医院下标每次+1

        # 获取响应值，将响应值转化为json对象
        res = requests.get(url=url, params=params, headers=headers)
        json_data = res.json()

        # 断言请求是否成功
        code = jsonpath(json_data, "$.code")  # 获取响应code
        try:
            if "0000" in code:
                pass
        except TypeError:
            logger.error(f"获取{hospital}状态失败，请检查~\n"
                         f"错误信息：{json_data}")

        # 通过jsonPath获取状态
        typeCode = jsonpath(json_data, "$.data.typeCode")  # 获取医院状态
        try:
            typeCode = [str(i) for i in typeCode]  # 列表元素转化为字符串
            typeCode = int(','.join(typeCode))  # 列表元素转化为整形拼接
        except TypeError:
            typeCode = 4  # 如果返回False，设置缺省值为4
        type_code_list.append(typeCode)

        time.sleep(0.05)  # 设置等待，避免请求频繁返回False

    # 返回状态码列表
    return type_code_list


def send_notification_dd(hospital_name):
    # 通过钉钉机器人发送到苗信息
    send_time = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')  # 发送时间为当前时间

    url = "https://oapi.dingtalk.com/robot/send?access_token" \
          "=649a092161ceda91c94ec436ca32b5439d60af7287447e8d0767952073723b4b "
    data = {"msgtype": "text",
            "text": {
                "content": "新到疫苗提醒\n"
                           "\n"
                           f"时间:  {send_time}\n"
                           f"医院:  {hospital_name}\n"
            },
            "at": {"isAtAll": "false"}
            }
    headers = {'Content-Type': 'application/json'}

    # 获取响应值，将响应值转化为json对象
    res = requests.post(url, json.dumps(data), headers=headers)
    json_data = res.json()

    # 通过jsonPath获取消息发送状态
    errmsg = jsonpath(json_data, "$.errmsg")

    # 断言请求是否成功
    if 'ok' in errmsg:
        logger.info("钉钉机器人发送信息成功")
    else:
        logger.error(f"钉钉机器人发送信息失败，请检查~\n"
                     f"错误信息：{json_data}")


def send_notification_wx(hospital_name):
    # 通过微信机器人发送到苗信息
    send_time = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')  # 发送时间为当前时间

    url = "https://qyapi.weixin.qq.com/cgi-bin/webhook/send?key=076398e3-345d-47df-838b-6d413ee13557"
    data = {
        "msgtype": "text",
        "text": {
            "content": "新到疫苗提醒\n"
                       "\n"
                       f"时间:  {send_time}\n"
                       f"医院:  {hospital_name}\n"
        }
    }
    headers = {'Content-Type': 'application/json'}

    # 获取响应值，将响应值转化为json对象
    res = requests.post(url, json.dumps(data), headers=headers)
    json_data = res.json()

    # 通过jsonPath获取消息发送状态
    errmsg = jsonpath(json_data, "$.errmsg")

    # 断言请求是否成功
    if 'ok' in errmsg:
        logging.info("微信机器人发送信息成功")
    else:
        logger.error(f"微信机器人发送信息失败，请检查~\n"
                     f"错误信息：{json_data}")


def jn_execute():
    send_type = {}  # 发送状态字典初始为空
    rounds = 1  # 运行次数

    while True:
        # 获取医院基本信息
        hospital_info = get_departments()

        # 获取医院状态码列表
        type_code_list = is_can_subscribe()
        type_code_index = 0  # 医院状态码下标为0

        # 遍历列表查看医院是否可预约
        for type_code in type_code_list:
            hospital = hospital_info["hospital_name"][type_code_index]  # 获取每次循环取到的医院名称
            if type_code == 1:
                logging.info(f'{hospital}可预约')

                if hospital in send_type:  # 判断字典中是否存在该医院
                    if send_type[hospital] == 0:  # 判断字典中该医院通知状态

                        send_notification_dd(hospital)  # 通过钉钉机器人发送医院到苗信息
                        send_notification_wx(hospital)  # 通过微信机器人发送医院到苗信息
                        send_type[hospital] = 1  # 通知状态设置为到货已通知
                else:
                    send_type[hospital] = 0  # 将该医院添加到字典，初始值为缺货未通知

            elif type_code == 2:
                logging.info(f'{hospital}可订阅')
                send_type[hospital] = 0  # 通知状态设置为缺货未通知

            elif type_code == 3:
                logging.info(f'{hospital}已订阅等待到苗通知')
                send_type[hospital] = 0  # 通知状态设置为缺货未通知

            elif type_code == 4:
                logging.info(f'{hospital}暂停订阅')
                send_type[hospital] = 0  # 通知状态设置为缺货未通知

            else:
                pass

            type_code_index += 1  # 医院状态码下标每次+1

        rounds += 1  # 运行次数每次+1

        time.sleep(1)  # 设置等待，避免请求频繁返回False

        # 设置分隔线及提示信息
        now_date_time = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')

        logger.info(f"_______________________济南已运行{rounds}次,当前时间{now_date_time}秒__________________________________\n\n")


if __name__ == '__main__':
    jn_execute()
