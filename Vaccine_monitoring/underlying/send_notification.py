import datetime
import json

import requests
from jsonpath import jsonpath


def send_notification(hospital_name):
    # 通过钉钉机器人发送到苗信息
    now_date_time = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    url = "https://oapi.dingtalk.com/robot/send?access_token" \
          "=2720160daa227da3556fc67be5a99d94fff53b771cbd879f07976be404f9300b "
    data = {"msgtype": "markdown",
            "markdown": {
                "title": "新到疫苗提醒~",
                "text": "##### 新到疫苗提醒~\n"
                        f"> ##### 时间:{now_date_time}\n"
                        f"> ##### 医院:{hospital_name}\n"
            },
            "at": {"isAtAll": "false"}
            }
    headers = {'Content-Type': 'application/json'}

    # 获取响应值，将响应值转化为dict对象
    res = requests.post(url, json.dumps(data), headers=headers)
    json_data = res.json()

    # 通过jsonPath获取消息发送状态
    errmsg = jsonpath(json_data, "$.errmsg")  # 获取日期是否约满状态码列表

    # 断言请求是否成功
    if 'ok' in errmsg:
        print("钉钉机器人发送信息成功")
    else:
        print("钉钉机器人发送信息失败，请检查")


if __name__ == '__main__':
    send_notification("")
